#-------------------------------------------------
#
# Project created by QtCreator 2014-12-01T11:33:04
#
#-------------------------------------------------

QT       += gui widgets
CONFIG += c++11
#CONFIG += staticlib
#QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

TARGET = StData
TEMPLATE = lib

DEFINES += STDATA_LIBRARY

SOURCES += \
    Project.cpp \
    VarManager.cpp \
    Log.cpp \
    Library.cpp \
    Sc2000DataStream.cpp \
    Class.cpp \
    Equation.cpp \
    Scheme.cpp \
    Icon.cpp \
    SpaceItem.cpp \
    Tool2d.cpp \
    GraphicsHelper.cpp \
    IconManager.cpp \
    ObjectInfo.cpp \
    ClassVarInfo.cpp \
    SpaceChunk.cpp \
    SpaceCollectionResult.cpp \
    SpaceToolResult.cpp \
    SpaceItemResult.cpp \
    VarManagerLoader.cpp \
    ObjectStateLoader.cpp \
    PathManager.cpp \
    LibraryCollection.cpp \
    CoreFunctions.cpp \
    StandardTypes.cpp \
    Object.cpp \
    ObjectVar.cpp \
    Error.cpp \
    ErrorProvider.cpp \
    HandleItem.cpp \
    Space.cpp \
    LinkInfo.cpp \
    ObjectVarsLinker.cpp \
    SpaceLoader.cpp \
    StDataGlobal.cpp \
    Exceptions.cpp \
    HyperBase.cpp \
    PolylineArrow.cpp \
    FontHelper.cpp \
    StringHelper.cpp \
    Settings.cpp \
    SpaceItemVisitorBase.cpp \
    ToolVisitorBase.cpp \
    CopySpaceItemInitializer.cpp \
    CopyToolInitializer.cpp \
    DebugFunctions.cpp

HEADERS +=\
    Project.h \
    VarManager.h \
    Log.h \
    Library.h \
    Sc2000DataStream.h \
    Class.h \
    Equation.h \
    Scheme.h \
    Icon.h \
    SpaceItem.h \
    Tool2d.h \
    GraphicsHelper.h \
    IconManager.h \
    ObjectInfo.h \
    ClassVarInfo.h \
    FlagHelper.h \
    SpaceChunk.h \
    SpaceCollectionResult.h \
    SpaceToolResult.h \
    SpaceItemResult.h \
    VarManagerLoader.h \
    ObjectStateLoader.h \
    PathManager.h \
    LibraryCollection.h \
    SortedCollection.h \
    CoreFunctions.h \
    StandardTypes.h \
    Object.h \
    ObjectVar.h \
    stdata_global.h \
    ErrorProvider.h \
    HandleItem.h \
    HandleCollection.h \
    Error.h \
    Space.h \
    LinkInfo.h \
    ObjectVarsLinker.h \
    SpaceLoader.h \
    StDataGlobal.h \
    Exceptions.h \
    HyperBase.h \
    StDataTypes.h \
    PolylineArrow.h \
    FontHelper.h \
    StringHelper.h \
    Settings.h \
    SpaceItemVisitorBase.h \
    ToolVisitorBase.h \
    CopySpaceItemInitializer.h \
    CopyToolInitializer.h \
    DebugFunctions.h

RESOURCES +=

unix {
    target.path = /usr/lib
    INSTALLS += target
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
}
