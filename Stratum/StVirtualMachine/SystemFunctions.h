#ifndef SYSTEMFUNCTIONS_H
#define SYSTEMFUNCTIONS_H

#include <QSet>

namespace StVirtualMachine {

extern void setupSystemFunctions();

extern void system();
extern void systemStr();

extern void getAsyncKeyState();
extern void getKeyboardLayout();

extern void joyGetX();
extern void joyGetY();
extern void joyGetZ();
extern void joyGetButtons();

extern void getMousePos();
extern void showCursor();
extern void setStandartCursorH();
extern void setStandartCursorW();

extern void loadCursorH();
extern void loadCursorW();

extern void getTickCount();
extern void getTime();
extern void getDate();

extern void setHyperJump2d();
extern void stdHyperJump();

extern void getScreenWidth();
extern void getScreenHeight();
extern void getWorkAreaX();
extern void getWorkAreaY();
extern void getWorkAreaWidth();
extern void getWorkAreaHeight();

int titleHeight();
int smallTitleHeight();
int fixedFrameWidth();
int fixedFrameHeight();
int sizeFrameWidth();
int sizeFrameHeight();

extern void getTitleHeight();
extern void getSmallTitleHeight();
extern void getFixedFrameWidth();
extern void getFixedFrameHeight();
extern void getSizeFrameWidth();
extern void getSizeFrameHeight();

extern void winExecute();
extern void shell();
extern void shellWait();
extern bool shellExecute(const QString& command, const QString& paramsStr, const QString& dir, int cmdShow);

extern void sendSms();
extern void sendMail();

extern void logMessage();

extern void vFunction();

extern bool isKeyPressed(int keyCode);

}

#endif // SYSTEMFUNCTIONS_H
