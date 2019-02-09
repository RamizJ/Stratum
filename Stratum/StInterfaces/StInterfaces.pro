#-------------------------------------------------
#
# Project created by QtCreator 2015-10-08T18:31:18
#
#-------------------------------------------------

QT       -= gui

CONFIG += c++11

TARGET = StInterfaces
TEMPLATE = lib

DEFINES += STINTERFACES_LIBRARY

SOURCES += \
    IVirtualMachine.cpp

HEADERS +=\
        stinterfaces_global.h \
    IVirtualMachine.h

unix {
    target.path = /usr/lib
    INSTALLS += target
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}

android {
QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StData/release/ -lStData
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StData/debug/ -lStData
else:unix: LIBS += -L$$OUT_PWD/../StData/ -lStData
INCLUDEPATH += $$PWD/../StData
DEPENDPATH += $$PWD/../StData
