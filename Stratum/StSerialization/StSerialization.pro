#-------------------------------------------------
#
# Project created by QtCreator 2016-07-16T16:31:05
#
#-------------------------------------------------

QT       += gui

CONFIG += c++11

TARGET = StSerialization
TEMPLATE = lib

DEFINES += STSERIALIZATION_LIBRARY

SOURCES += \
    JsonPenToolSerializer.cpp \
    JsonBrushToolSerializer.cpp \
    JsonTextureToolSerializer.cpp \
    JsonRasterTextToolSerializer.cpp \
    JsonFontToolSerializer.cpp \
    JsonStringToolSerializer.cpp \
    JsonPolylineItemSerializer.cpp \
    JsonTextureItemSerializer.cpp \
    JsonTextItemSerializer.cpp \
    JsonSpaceItemSerializer.cpp \
    JsonGroupItemSerializer.cpp \
    JsonSpaceItem2dSerializer.cpp \
    JsonSerializer.cpp \
    JsonHyperKeySerializer.cpp \
    JsonToolSerializer.cpp

HEADERS +=\
        stserialization_global.h \
    JsonPenToolSerializer.h \
    JsonBrushToolSerializer.h \
    JsonTextureToolSerializer.h \
    JsonRasterTextToolSerializer.h \
    JsonFontToolSerializer.h \
    JsonStringToolSerializer.h \
    JsonPolylineItemSerializer.h \
    JsonTextureItemSerializer.h \
    JsonTextItemSerializer.h \
    JsonSpaceItemSerializer.h \
    JsonGroupItemSerializer.h \
    StSerializationTypes.h \
    JsonSpaceItem2dSerializer.h \
    SerializationKeys.h \
    JsonSerializer.h \
    JsonHyperKeySerializer.h \
    JsonToolSerializer.h \
    ToolHelper.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StData/release/ -lStData
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StData/debug/ -lStData
else:unix: LIBS += -L$$OUT_PWD/../StData/ -lStData

INCLUDEPATH += $$PWD/../StData
DEPENDPATH += $$PWD/../StData
