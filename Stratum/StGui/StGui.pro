#-------------------------------------------------
#
# Project created by QtCreator 2014-12-01T11:32:01
#
#-------------------------------------------------

QT       += core gui opengl network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Stratum
TEMPLATE = app

CONFIG += c++11

#QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

SOURCES += main.cpp\
        MainWindow.cpp \
    OutputWidget.cpp \
    OutputListModel.cpp \
    LibrariesWidget.cpp \
    LibrariesModel.cpp \
    AddLibraryDialog.cpp \
    LibraryPropertiesDialog.cpp \
    ProjectHierarchyWidget.cpp \
    TabManager.cpp \
    SourceCodeEditor.cpp \
    SyntaxHighlighter.cpp \
    ObjectHierarchyModel.cpp \
    IconLibraryModel.cpp \
    ObjectVariablesModel.cpp \
    EditObjectDialog.cpp \
    EditPolylineDialog.cpp \
    EditPenWidget.cpp \
    EditBrushWidget.cpp \
    EditLinkDialog.cpp \
    EditPixmapDialog.cpp \
    EditSchemeDialog.cpp \
    EditGroupDialog.cpp \
    EditHyperbaseWidget.cpp \
    EditLocationWidget.cpp \
    PixmapEditor.cpp \
    GroupGraphicsItemModel.cpp \
    SourceCodeWidget.cpp \
    GuiContext.cpp \
    SpaceEditWidget.cpp \
    SpaceContextMenu.cpp \
    EditRasterTextDialog.cpp \
    RemoteLogger.cpp \
    QtMsgFileLogger.cpp \

HEADERS  += MainWindow.h \
    OutputWidget.h \
    OutputListModel.h \
    LibrariesWidget.h \
    LibrariesModel.h \
    AddLibraryDialog.h \
    LibraryPropertiesDialog.h \
    ProjectHierarchyWidget.h \
    TabManager.h \
    SourceCodeEditor.h \
    SyntaxHighlighter.h \
    ObjectHierarchyModel.h \
    IconLibraryModel.h \
    ObjectVariablesModel.h \
    EditObjectDialog.h \
    EditPolylineDialog.h \
    EditPenWidget.h \
    EditBrushWidget.h \
    EditLinkDialog.h \
    EditPixmapDialog.h \
    EditSchemeDialog.h \
    EditGroupDialog.h \
    EditHyperbaseWidget.h \
    EditLocationWidget.h \
    PixmapEditor.h \
    GroupGraphicsItemModel.h \
    TreeNode.h \
    SourceCodeWidget.h \
    GuiContext.h \
    SpaceEditWidget.h \
    SpaceContextMenu.h \
    EditRasterTextDialog.h \
    RemoteLogger.h \
    QtMsgFileLogger.h \

FORMS    += MainWindow.ui \
    OutputWidget.ui \
    LibrariesWidget.ui \
    AddLibraryDialog.ui \
    LibraryPropertiesDialog.ui \
    ProjectHierarchyWidget.ui \
    EditObjectDialog.ui \
    EditPolylineDialog.ui \
    EditPenWidget.ui \
    EditBrushWidget.ui \
    EditLinkDialog.ui \
    EditPixmapDialog.ui \
    EditSchemeDialog.ui \
    EditGroupDialog.ui \
    EditHyperbaseWidget.ui \
    EditLocationWidget.ui \
    PixmapEditor.ui \
    SourceCodeWidget.ui \
    EditRasterTextDialog.ui

RESOURCES += \
    Resources/Resources.qrc

DISTFILES += \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/AndroidManifest.xml \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StSerialization/release/ -lStSerialization
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StSerialization/debug/ -lStSerialization
else:unix: LIBS += -L$$OUT_PWD/../StSerialization/ -lStSerialization
INCLUDEPATH += $$PWD/../StSerialization
DEPENDPATH += $$PWD/../StSerialization

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StCore/release/ -lStCore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StCore/debug/ -lStCore
else:unix: LIBS += -L$$OUT_PWD/../StCore/ -lStCore
INCLUDEPATH += $$PWD/../StCore
DEPENDPATH += $$PWD/../StCore

android {
QT += androidextras
QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder

#LIBS += -L$$OUT_PWD/../StData/ -lStData
#INCLUDEPATH += $$PWD/../StData
#DEPENDPATH += $$PWD/../StData

#LIBS += -L$$OUT_PWD/../StSpace/ -lStSpace
#INCLUDEPATH += $$PWD/../StSpace
#DEPENDPATH += $$PWD/../StSpace

#LIBS += -L$$OUT_PWD/../StInterfaces/ -lStInterfaces
#INCLUDEPATH += $$PWD/../StInterfaces
#DEPENDPATH += $$PWD/../StInterfaces

#LIBS += -L$$OUT_PWD/../StVirtualMachine/ -lStVirtualMachine
#INCLUDEPATH += $$PWD/../StVirtualMachine
#DEPENDPATH += $$PWD/../StVirtualMachine

#LIBS += -L$$OUT_PWD/../StCompiler/ -lStCompiler
#INCLUDEPATH += $$PWD/../StCompiler
#DEPENDPATH += $$PWD/../StCompiler

#LIBS += -L$$OUT_PWD/../StCore/release/ -lStCore
#DEPENDPATH += $$PWD/../StCore
#INCLUDEPATH += $$PWD/../StCore

#ANDROID_EXTRA_LIBS += c:/Projects/Qt/StratumSource/build-Stratum-Android_armeabi_v7a_GCC_4_9_Qt_5_5_1-Debug/StData/libStData.so
#ANDROID_EXTRA_LIBS += c:/Projects/Qt/StratumSource/build-Stratum-Android_armeabi_v7a_GCC_4_9_Qt_5_5_1-Debug/StSpace/libStSpace.so
#ANDROID_EXTRA_LIBS += c:/Projects/Qt/StratumSource/build-Stratum-Android_armeabi_v7a_GCC_4_9_Qt_5_5_1-Debug/StInterfaces/libStInterfaces.so
#ANDROID_EXTRA_LIBS += c:/Projects/Qt/StratumSource/build-Stratum-Android_armeabi_v7a_GCC_4_9_Qt_5_5_1-Debug/StVirtualMachine/libStVirtualMachine.so
#ANDROID_EXTRA_LIBS += c:/Projects/Qt/StratumSource/build-Stratum-Android_armeabi_v7a_GCC_4_9_Qt_5_5_1-Debug/StCompiler/libStCompiler.so
#ANDROID_EXTRA_LIBS += c:/Projects/Qt/StratumSource/build-Stratum-Android_armeabi_v7a_GCC_4_9_Qt_5_5_1-Debug/StCore/libStCore.so

}
