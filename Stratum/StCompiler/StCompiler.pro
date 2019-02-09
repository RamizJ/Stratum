#-------------------------------------------------
#
# Project created by QtCreator 2015-03-17T17:19:39
#
#-------------------------------------------------

#QT       -= gui
QT       += gui widgets

TARGET = StCompiler
TEMPLATE = lib

CONFIG += c++11
#CONFIG += staticlib
#QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

DEFINES += STCOMPILER_LIBRARY

SOURCES += \
    Compiler.cpp \
    TplCursor.cpp \
    Command.cpp \
    CommandCollection.cpp \
    CompilerVarCollection.cpp \
    MacroCollection.cpp \
    Macro.cpp \
    CompilerFunctions.cpp \
    VarStack.cpp \
    CompileError.cpp

HEADERS +=\
        stcompiler_global.h \
    Compiler.h \
    TplCursor.h \
    Command.h \
    CommandCollection.h \
    CommandComparer.h \
    CompilerVarCollection.h \
    MacroCollection.h \
    Macro.h \
    CompilerFunctions.h \
    VarStack.h \
    CompileError.h

unix {
    target.path = /usr/lib
    INSTALLS += target
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StData/release/ -lStData
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StData/debug/ -lStData
else:unix: LIBS += -L$$OUT_PWD/../StData/ -lStData
INCLUDEPATH += $$PWD/../StData
DEPENDPATH += $$PWD/../StData

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StVmData/release/ -lStVmData
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StVmData/debug/ -lStVmData
else:unix: LIBS += -L$$OUT_PWD/../StVmData/ -lStVmData
win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../StVmData/release/libStVmData.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../StVmData/debug/libStVmData.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../StVmData/release/StVmData.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../StVmData/debug/StVmData.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../StVmData/libStVmData.a
INCLUDEPATH += $$PWD/../StVmData
DEPENDPATH += $$PWD/../StVmData

