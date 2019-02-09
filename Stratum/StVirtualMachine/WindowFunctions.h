#ifndef WINDOWFUNCTIONS_H
#define WINDOWFUNCTIONS_H

namespace StVirtualMachine {

extern void setupWindowFunctions();

extern void createWindowEx();
extern void loadSpaceWindow();
extern void openSchemeWindow();
extern void openClassScheme();

extern void closeClassScheme();
extern void closeWindow();

extern void getWindowName();
extern void getWindowSpace();

extern void getClientWidth();
extern void getClientHeight();
extern void setClientSize();

extern void getWindowOrgX();
extern void getWindowOrgY();
extern void setWindowOrg();
extern void setWindowPos();

extern void getWindowWidth();
extern void getWindowHeight();
extern void setWindowSize();

extern void getWindowTitle();
extern void setWindowTitle();

extern void setScrollRange();

extern void setWindowTransparentW();
extern void setWindowTransparentH();
extern void setWindowTransparentColorW();
extern void setWindowTransparentColorH();

extern void setWindowRegionW();
extern void setWindowRegionH();
extern void setWindowOwner();

extern void screenshot();
extern void screenshotFull();
extern void screenshotDesktop();
extern void screenshotDesktopFull();

extern void showWindow();
extern void bringWindowToTop();

extern void isIconic();
extern void isWindowExist();
extern void isWindowVisible();

extern void cascadeWindows();
extern void tile();
extern void arrangeIcons();

extern void getWindowProp();

}

#endif // WINDOWFUNCTIONS_H
