# Generated from qml.pro.

#####################################################################
## qml Tool:
#####################################################################

qt_get_tool_target_name(target_name qml)
qt_internal_add_tool(${target_name}
    TARGET_DESCRIPTION "QML Runtime"
    TOOLS_TARGET Qml # special case
    SOURCES
        conf.h
        main.cpp
    PUBLIC_LIBRARIES
        Qt::CorePrivate
        Qt::QmlPrivate
)

# special case begin
# Don't set properties on a host tool when cross compiling, because it
# is not being built.
if(CMAKE_CROSSCOMPILING AND NOT QT_BUILD_TOOLS_WHEN_CROSSCOMPILING)
    return()
endif()

set_source_files_properties(
    conf/default.qml
    conf/resizeToItem.qml
    conf/content/resizeItemToWindow.qml
    conf/content/resizeWindowToItem.qml
    PROPERTIES QT_SKIP_QUICKCOMPILER 1
)
# special case end

# Resources:
set(qml_resource_files
    "conf/content/resizeItemToWindow.qml"
    "conf/content/resizeWindowToItem.qml"
    "conf/default.qml"
    "conf/resizeToItem.qml"
    "resources/qml-64.png"
)

qt_internal_add_resource(${target_name} "qml"
    PREFIX
        "/qt-project.org/QmlRuntime"
    FILES
        ${qml_resource_files}
)

# Turn the tool into its own self-contained qml module
qt6_add_qml_module(${target_name}
    URI QmlRuntime.Config
    VERSION 1.0
    NO_CREATE_PLUGIN_TARGET
)

qt_internal_extend_target(${target_name} CONDITION TARGET Qt::Gui
    PUBLIC_LIBRARIES
        Qt::Gui
)

qt_internal_extend_target(${target_name} CONDITION TARGET Qt::Widgets
    PUBLIC_LIBRARIES
        Qt::Widgets
)

#### Keys ignored in scope 4:.:.:qml.pro:WIN32:
# RC_ICONS = "resources/qml.ico"

#### Keys ignored in scope 5:.:.:qml.pro:APPLE:
# ICON = "resources/qml.icns"
# OTHER_FILES = "resources/Info.plist"
# QMAKE_INFO_PLIST = "resources/Info.plist"

qt_internal_extend_target(${target_name} CONDITION QT_FEATURE_qml_debug
    DEFINES
        QT_QML_DEBUG_NO_WARNING
)
