# Collect test data
file(GLOB_RECURSE test_data_glob
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    data/*)
list(APPEND test_data ${test_data_glob})

qt_internal_add_test(tst_qmlcompiler_manual
    SOURCES
        ../../shared/util.cpp ../../shared/util.h
        tst_qmlcompiler_manual.cpp
    INCLUDE_DIRECTORIES
        ../../shared
    LIBRARIES
        Qt::CorePrivate
        Qt::QmlPrivate
        Qt::Quick
    TESTDATA ${test_data}
)

## Scopes:
#####################################################################

qt_add_resources(tst_qmlcompiler_manual "qmlcompiler"
    PREFIX
        "/"
    FILES
        "data/signalHandlers.qml"
)

qt_internal_extend_target(tst_qmlcompiler_manual CONDITION ANDROID OR IOS
    DEFINES
        QT_QMLTEST_DATADIR=\\\":/data\\\"
)

qt_internal_extend_target(tst_qmlcompiler_manual CONDITION NOT ANDROID AND NOT IOS
    DEFINES
        QT_QMLTEST_DATADIR=\\\"${CMAKE_CURRENT_SOURCE_DIR}/data\\\"
)
