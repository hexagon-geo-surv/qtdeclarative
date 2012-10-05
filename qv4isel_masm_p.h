#ifndef QV4ISEL_MASM_P_H
#define QV4ISEL_MASM_P_H

#include "qv4ir_p.h"
#include "qmljs_objects.h"
#include "qmljs_runtime.h"

#include <QtCore/QHash>
#include <config.h>
#include <wtf/Vector.h>
#include <assembler/MacroAssembler.h>

namespace QQmlJS {
namespace MASM {

class InstructionSelection: protected IR::StmtVisitor, public JSC::MacroAssembler
{
public:
    InstructionSelection(VM::ExecutionEngine *engine, IR::Module *module, uchar *code);
    ~InstructionSelection();

    void operator()(IR::Function *function);

protected:
#if CPU(X86)
    static const RegisterID StackFrameRegister = JSC::X86Registers::ebp;
    static const RegisterID StackPointerRegister = JSC::X86Registers::esp;
    static const RegisterID ContextRegister = JSC::X86Registers::esi;
    static const RegisterID ReturnValueRegister = JSC::X86Registers::eax;
    static const RegisterID Gpr0 = JSC::X86Registers::eax;
    static const RegisterID Gpr1 = JSC::X86Registers::ecx;
    static const RegisterID Gpr2 = JSC::X86Registers::edx;
    static const RegisterID Gpr3 = JSC::X86Registers::esi;
    static const RegisterID CalleeSavedGpr = Gpr3;
    static const FPRegisterID FPGpr0 = JSC::X86Registers::xmm0;
#else
#error Argh.
#endif

#if CPU(X86) || CPU(X86_64)
    void enterStandardStackFrame(int locals)
    {
        push(StackFrameRegister);
        move(StackPointerRegister, StackFrameRegister);
        sub32(TrustedImm32(locals), StackPointerRegister);
        push(CalleeSavedGpr);
    }
    void leaveStandardStackFrame(int locals)
    {
        pop(CalleeSavedGpr);
        add32(TrustedImm32(locals), StackPointerRegister);
        pop(StackFrameRegister);
    }
#else
#error Argh.
#endif

    Address addressForArgument(int index) const
    {
        // ### CPU specific: on x86/x86_64 we need +2 to jump over ebp and the return address
        // on the stack. Maybe same on arm if we save lr on stack on enter.
        return Address(StackFrameRegister, (index + 2) * sizeof(void*));
    }

    // Some run-time functions take (Value* args, int argc). This function is for populating
    // the args.
    Address argumentAddressForCall(int argument)
    {
        const int index = _function->maxNumberOfArguments - argument;
        return Address(StackFrameRegister, sizeof(VM::Value) * (-index)
                                          - sizeof(void*) // size of ebp
                       );
    }
    Address baseAddressForCallArguments()
    {
        return argumentAddressForCall(0);
    }

    VM::String *identifier(const QString &s);
    Address loadTempAddress(RegisterID reg, IR::Temp *t);
    void callActivationProperty(IR::Call *call, IR::Temp *result);
    void callProperty(IR::Call *call, IR::Temp *result);
    void constructActivationProperty(IR::New *call, IR::Temp *result);
    void constructProperty(IR::New *ctor, IR::Temp *result);
    void callValue(IR::Call *call, IR::Temp *result);
    void constructValue(IR::New *call, IR::Temp *result);
    void checkExceptions();

    virtual void visitExp(IR::Exp *);
    virtual void visitEnter(IR::Enter *);
    virtual void visitLeave(IR::Leave *);
    virtual void visitMove(IR::Move *);
    virtual void visitJump(IR::Jump *);
    virtual void visitCJump(IR::CJump *);
    virtual void visitRet(IR::Ret *);

private:
    void jumpToBlock(IR::BasicBlock *target);

    typedef JSC::FunctionPtr FunctionPtr;

    void callAbsolute(FunctionPtr function) {
        CallToLink ctl;
        ctl.call = call();
        ctl.externalFunction = function;
        _callsToLink.append(ctl);
    }

    class FunctionCall
    {
    public:
        FunctionCall(InstructionSelection* selection)
            : isel(selection)
        {}

        void addArgumentFromRegister(RegisterID reg)
        {
            Argument arg;
            arg.setRegister(reg);
            arguments << arg;
        }
        void addArgumentFromMemory(const Address& address)
        {
            Argument arg;
            arg.setMemoryValue(address);
            arguments << arg;
        }
        void addArgumentAsAddress(const Address& address)
        {
            Argument arg;
            arg.setAddress(address);
            arguments << arg;
        }
        void addArgumentByValue(TrustedImm32 value)
        {
            Argument arg;
            arg.setValue(value);
            arguments << arg;
        }

        void addArgument(IR::Temp* temp)
        {
            Argument arg;
            arg.setValue(temp);
            arguments << arg;
        }

        int prepareVariableArguments(IR::ExprList* args);

        void call(FunctionPtr function)
        {
            for (int i = arguments.count() - 1; i >= 0; --i)
                arguments.at(i).push(isel, /*scratch register*/ Gpr0);
            isel->callAbsolute(function);
            isel->add32(TrustedImm32(arguments.count() * sizeof(void*)), StackPointerRegister);
        }

        typedef void (*ActivationMethod)(VM::Context *, VM::Value *result, VM::String *name, VM::Value *args, int argc);
        typedef void (*BuiltinMethod)(VM::Context *, VM::Value *result, VM::Value *args, int argc);
        void callRuntimeMethod(ActivationMethod method, IR::Temp *result, IR::Expr *base, IR::ExprList *args);
        void callRuntimeMethod(BuiltinMethod method, IR::Temp *result, IR::ExprList *args);

    private:
        class Argument {
        public:
            Argument()
                : type(None)
                , address(InstructionSelection::Gpr0, 0)
            {}

            void setRegister(RegisterID regi)
            {
                reg = regi;
                type = Register;
            }
            void setMemoryValue(const Address& address)
            {
                this->address = address;
                type = MemoryValue;
            }
            void setAddress(const Address& address)
            {
                this->address = address;
                type = Address;
            }
            void setValue(TrustedImm32 value)
            {
                this->value = value;
                type = Value;
            }
            void setValue(IR::Temp *temp)
            {
                this->temp = temp;
                type = Temp;
            }

            void push(InstructionSelection* assembler, RegisterID scratchRegister) const
            {
                switch (type) {
                    case None: break;
                    case Register: assembler->push(reg); break;
                    case Value: assembler->push(value); break;
                    case MemoryValue: assembler->push(address); break;
                    case Address:
                        assembler->add32(TrustedImm32(address.offset), address.base, scratchRegister);
                        assembler->push(scratchRegister);
                        break;
                    case Temp: {
                        JSC::MacroAssembler::Address tempAddr = assembler->loadTempAddress(scratchRegister, temp);
                        assembler->add32(TrustedImm32(tempAddr.offset), tempAddr.base, scratchRegister);
                        assembler->push(scratchRegister);
                        break;
                    }
                }
            }
        private:
            enum Type {
                None,
                Register,
                Value,
                MemoryValue,
                Address,
                Temp
            } type;
            RegisterID reg;
            TrustedImm32 value;
            JSC::MacroAssembler::Address address;
            IR::Temp* temp;
        };
        QList<Argument> arguments;
        InstructionSelection* isel;
    };

    using JSC::MacroAssembler::push;
    void push(IR::Temp* temp)
    {
        Address addr = loadTempAddress(Gpr0, temp);
        add32(TrustedImm32(addr.offset), addr.base, Gpr0);
        push(Gpr0);
    }
    void push(TrustedImmPtr ptr)
    {
        move(TrustedImmPtr(ptr), Gpr0);
        push(Gpr0);
    }
    void push(VM::String* name)
    {
        push(TrustedImmPtr(name));
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    void generateFunctionCall(FunctionPtr function, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
    {
        // Reverse order
        push(arg4);
        push(arg3);
        push(arg2);
        push(arg1);
        callAbsolute(function);
        add32(TrustedImm32(4 * sizeof(void*)), StackPointerRegister);
    }


    template <typename Arg1, typename Arg2, typename Arg3>
    void generateFunctionCall(FunctionPtr function, Arg1 arg1, Arg2 arg2, Arg3 arg3)
    {
        // Reverse order
        push(arg3);
        push(arg2);
        push(arg1);
        callAbsolute(function);
        add32(TrustedImm32(3 * sizeof(void*)), StackPointerRegister);
    }

    template <typename Arg1, typename Arg2>
    void generateFunctionCall(FunctionPtr function, Arg1 arg1, Arg2 arg2)
    {
        // Reverse order
        push(arg2);
        push(arg1);
        callAbsolute(function);
        add32(TrustedImm32(2 * sizeof(void*)), StackPointerRegister);
    }

    struct CallToLink {
        Call call;
        FunctionPtr externalFunction;
    };

    template <VM::Value::ValueType type>
    void storeValue(TrustedImm32 value, Address destination)
    {
        destination.offset += offsetof(VM::ValueData, tag);
        store32(TrustedImm32(type), destination);
        destination.offset -= offsetof(VM::ValueData, tag);
        destination.offset += VM::ValueOffsetHelper<type>::DataOffset;
        store32(value, destination);
    }

    VM::ExecutionEngine *_engine;
    IR::Module *_module;
    IR::Function *_function;
    IR::BasicBlock *_block;
    uchar *_buffer;
    uchar *_code;
    uchar *_codePtr;
    QHash<IR::BasicBlock *, QVector<Jump> > _patches;
    QHash<IR::BasicBlock *, Label> _addrs;
    QList<CallToLink> _callsToLink;
};

} // end of namespace MASM
} // end of namespace QQmlJS

#endif // QV4ISEL_MASM_P_H
