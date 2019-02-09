#-------------------------------------------------
#
# Project created by QtCreator 2015-07-02T23:50:15
#
#-------------------------------------------------

QT       += widgets opengl
CONFIG += c++11
#QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

TARGET = StSpace
TEMPLATE = lib

DEFINES += STSPACE_LIBRARY

SOURCES += \
    LinkGraphicsItem.cpp \
    ObjectGraphicsItem.cpp \
    PixmapGraphicsItem.cpp \
    PolylineGraphicsItem.cpp \
    SpaceScene.cpp \
    SpaceView.cpp \
    SpaceWidget.cpp \
    GroupGraphicsItem.cpp \
    RasterTextGraphicsItem.cpp \
    ControlGraphicsItem.cpp \
    SpaceGraphicsItem.cpp

HEADERS +=\
        stspace_global.h \
    GraphicsItemType.h \
    LinkGraphicsItem.h \
    ObjectGraphicsItem.h \
    PixmapGraphicsItem.h \
    PolylineGraphicsItem.h \
    SpaceScene.h \
    SpaceView.h \
    SpaceWidget.h \
    GroupGraphicsItem.h \
    RasterTextGraphicsItem.h \
    ControlGraphicsItem.h \
    SpaceGraphicsItem.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StData/release/ -lStData
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StData/debug/ -lStData
else:unix: LIBS += -L$$OUT_PWD/../StData/ -lStData
INCLUDEPATH += $$PWD/../StData
DEPENDPATH += $$PWD/../StData


win32:CONFIG(release, debug|release): LIBS += -lUser32
else:win32:CONFIG(debug, debug|release): LIBS += -lUser32
