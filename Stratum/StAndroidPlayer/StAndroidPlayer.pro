#-------------------------------------------------
#
# Project created by QtCreator 2018-08-04T17:56:07
#
#-------------------------------------------------

QT       += core gui opengl network
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StratumPlayer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        MainWindow.cpp \
    AndroidFileDialog.cpp \
    AndroidFunctions.cpp

HEADERS += \
        MainWindow.h \
    AndroidFileDialog.h \
    AndroidFunctions.h

FORMS += \
        MainWindow.ui

CONFIG += mobility
MOBILITY = 

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

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
}

RESOURCES +=
