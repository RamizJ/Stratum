QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

CONFIG += c++11

TEMPLATE = app

SOURCES += \ 
    tst_classtests.cpp


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../StData/release/ -lStData
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../StData/debug/ -lStData
else:unix: LIBS += -L$$OUT_PWD/../StData/ -lStData
INCLUDEPATH += $$PWD/../StData
DEPENDPATH += $$PWD/../StData
