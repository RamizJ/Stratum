#-------------------------------------------------
#
# Project created by QtCreator 2015-10-10T15:59:39
#
#-------------------------------------------------

QT       += widgets

TARGET = StVirtualMachine
TEMPLATE = lib

CONFIG += c++11

DEFINES += STVIRTUALMACHINE_LIBRARY

SOURCES += \
    VirtualMachine.cpp \
    VarValueStack.cpp \
    Context.cpp \
    Array.cpp \
    ArrayManager.cpp \
    StreamItem.cpp \
    StreamManager.cpp \
    SpaceWindowManager.cpp \
    SpaceWindowItem.cpp \
    Matrix.cpp \
    MatrixManager.cpp \
    VmLog.cpp \
    VarFunctions.cpp \
    MathFunctions.cpp \
    StringFunctions.cpp \
    LogicBinaryFunctions.cpp \
    StreamFunctions.cpp \
    FileSystemFunctions.cpp \
    ArrayFunctions.cpp \
    MatrixFunctions.cpp \
    MatrixEditor.cpp \
    DialogFunctions.cpp \
    WindowFunctions.cpp \
    SpaceFunctions.cpp \
    SpaceObjectFunctions.cpp \
    ToolFunctions.cpp \
    PenFunctions.cpp \
    BrushFunctions.cpp \
    TextFunctions.cpp \
    PixmapFunctions.cpp \
    PolylineFunctions.cpp \
    GroupFunctions.cpp \
    SystemFunctions.cpp \
    KeyStateObserver.cpp \
    VmGlobal.cpp \
    CodePointer.cpp \
    MessageFunctions.cpp \
    MessageHandler.cpp \
    MessageManager.cpp \
    ClassFunctions.cpp \
    ControlsFunctions.cpp \
    MultimediaFunctions.cpp \
    HyperJumpManager.cpp \
    VmContext.cpp \
    MessageData.cpp \
    MessageDataVisitor.cpp \
    Clipboard.cpp

HEADERS +=\
        stvirtualmachine_global.h \
    VirtualMachine.h \
    VarValueStack.h \
    Context.h \
    Array.h \
    ArrayManager.h \
    StreamItem.h \
    StreamManager.h \
    SpaceWindowManager.h \
    SpaceWindowItem.h \
    Matrix.h \
    MatrixData.h \
    MatrixManager.h \
    VmLog.h \
    VarFunctions.h \
    MathFunctions.h \
    StringFunctions.h \
    LogicBinaryFunctions.h \
    StreamFunctions.h \
    FileSystemFunctions.h \
    ArrayFunctions.h \
    MatrixFunctions.h \
    MatrixEditor.h \
    DialogFunctions.h \
    WindowFunctions.h \
    SpaceFunctions.h \
    SpaceObjectFunctions.h \
    ToolFunctions.h \
    PenFunctions.h \
    BrushFunctions.h \
    TextFunctions.h \
    PixmapFunctions.h \
    PolylineFunctions.h \
    GroupFunctions.h \
    SystemFunctions.h \
    KeyStateObserver.h \
    VmGlobal.h \
    CodePointer.h \
    MessageFunctions.h \
    MessageHandler.h \
    MessageManager.h \
    ClassFunctions.h \
    ControlsFunctions.h \
    MultimediaFunctions.h \
    HyperJumpManager.h \
    VmTypes.h \
    VmContext.h \
    MessageData.h \
    MessageDataVisitor.h \
    Clipboard.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StData/release/ -lStData
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StData/debug/ -lStData
else:unix: LIBS += -L$$OUT_PWD/../StData/ -lStData
INCLUDEPATH += $$PWD/../StData
DEPENDPATH += $$PWD/../StData

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StSpace/release/ -lStSpace
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StSpace/debug/ -lStSpace
else:unix: LIBS += -L$$OUT_PWD/../StSpace/ -lStSpace
INCLUDEPATH += $$PWD/../StSpace
DEPENDPATH += $$PWD/../StSpace

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StInterfaces/release/ -lStInterfaces
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StInterfaces/debug/ -lStInterfaces
else:unix: LIBS += -L$$OUT_PWD/../StInterfaces/ -lStInterfaces
INCLUDEPATH += $$PWD/../StInterfaces
DEPENDPATH += $$PWD/../StInterfaces

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StSerialization/release/ -lStSerialization
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StSerialization/debug/ -lStSerialization
else:unix: LIBS += -L$$OUT_PWD/../StSerialization/ -lStSerialization
INCLUDEPATH += $$PWD/../StSerialization
DEPENDPATH += $$PWD/../StSerialization

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StCompiler/release/ -lStCompiler
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StCompiler/debug/ -lStCompiler
else:unix: LIBS += -L$$OUT_PWD/../StCompiler/ -lStCompiler
INCLUDEPATH += $$PWD/../StCompiler
DEPENDPATH += $$PWD/../StCompiler


#WINAPI
win32:CONFIG(release, debug|release): LIBS += -lUser32
else:win32:CONFIG(debug, debug|release): LIBS += -lUser32

win32:CONFIG(release, debug|release): LIBS += -lShell32
else:win32:CONFIG(debug, debug|release): LIBS += -lShell32

win32:CONFIG(release, debug|release): LIBS += -lKernel32
else:win32:CONFIG(debug, debug|release): LIBS += -lKernel32

win32:CONFIG(release, debug|release): LIBS += -lGdi32
else:win32:CONFIG(debug, debug|release): LIBS += -lGdi32
