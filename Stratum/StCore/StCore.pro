#-------------------------------------------------
#
# Project created by QtCreator 2015-06-05T20:57:47
#
#-------------------------------------------------

#QT       -= gui

TARGET = StCore
TEMPLATE = lib

CONFIG += c++11
#CONFIG += staticlib

DEFINES += STCORE_LIBRARY

SOURCES += \
    Core.cpp \
    Run.cpp

HEADERS +=\
    Core.h \
    stcore_global.h \
    Run.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StVirtualMachine/release/ -lStVirtualMachine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StVirtualMachine/debug/ -lStVirtualMachine
else:unix: LIBS += -L$$OUT_PWD/../StVirtualMachine/ -lStVirtualMachine
INCLUDEPATH += $$PWD/../StVirtualMachine
DEPENDPATH += $$PWD/../StVirtualMachine

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StCompiler/release/ -lStCompiler
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StCompiler/debug/ -lStCompiler
else:unix: LIBS += -L$$OUT_PWD/../StCompiler/ -lStCompiler
INCLUDEPATH += $$PWD/../StCompiler
DEPENDPATH += $$PWD/../StCompiler
