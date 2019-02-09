#include "WindowFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"
#include "Array.h"
#include "VmLog.h"

#include <QDebug>

#include <StDataGlobal.h>
#include <QColor>
#include <SpaceWidget.h>
#include <SpaceScene.h>
#include <Space.h>
#include <QScreen>
#include <QGuiApplication>
#include <QWindow>
#include <Object.h>
#include <Class.h>
#include <QPainter>
#include <SpaceView.h>

using namespace StData;
using namespace StSpace;

namespace StVirtualMachine {

void setupWindowFunctions()
{
    operations[V_CREATEWINDOWEX] = createWindowEx;
    operations[LOADSPACEWINDOW] = loadSpaceWindow;
    operations[OPENSCHEMEWINDOW] = openSchemeWindow;
    operations[VM_OPENCLASSSCHEME] = openClassScheme;
    operations[VM_OPENCLASSSCHEME] = openClassScheme;

    operations[VM_CLOSECLASSSCHEME] = closeClassScheme;
    operations[CLOSEWINDOW] = closeWindow;

    operations[V_GETNAMEBYHSP] = getWindowName;
    operations[V_GETHSPBYNAME] = getWindowSpace;

    operations[GETCLIENTWIDTH] = getClientWidth;
    operations[GETCLIENTHEIGHT] = getClientHeight;
    operations[V_SETCLIENTSIZE] = setClientSize;

    operations[WIN_ORGX] = getWindowOrgX;
    operations[WIN_ORGY] = getWindowOrgY;
    operations[SETWINDOWORG] = setWindowOrg;
    operations[SETWINDOWPOS] = setWindowPos;

    operations[WIN_SIZEX] = getWindowWidth;
    operations[WIN_SIZEY] = getWindowHeight;
    operations[SETWINDOWSIZE] = setWindowSize;

    operations[GETWINDOWTITLE] = getWindowTitle;
    operations[SETWINDOWTITLE] = setWindowTitle;

    operations[SETSCROLLRANGE] = setScrollRange;

    operations[VM_SETWINDOWTRANSPARENT] = setWindowTransparentW;
    operations[VM_SETWINDOWTRANSPARENT_H] = setWindowTransparentH;
    operations[VM_SETWINDOWTRANSPARENTCOLOR] = setWindowTransparentColorW;
    operations[VM_SETWINDOWTRANSPARENTCOLOR_H] = setWindowTransparentColorH;

    operations[VM_SETWINDOWREGION] = setWindowRegionW;
    operations[VM_SETWINDOWREGION_H] = setWindowRegionH;
    operations[VM_SETWINDOWOWNER] = setWindowOwner;

    operations[VM_SCREENSHOT] = screenshot;
    operations[VM_SCREENSHOT_FULL] = screenshotFull;
    operations[VM_SCREENSHOT_DESKTOP] = screenshotDesktop;
    operations[VM_SCREENSHOT_DESKTOP_FULL] = screenshotDesktopFull;

    operations[SHOWWINDOW] = showWindow;
    operations[BRINGWINDOWTOTOP] = bringWindowToTop;

    operations[ISICONIC] = isIconic;
    operations[ISWINDOWEXIST] = isWindowExist;
    operations[ISWINDOWVISIBLE] = isWindowVisible;

    operations[VM_GETWINDOWPROP] = getWindowProp;

    operations[CASCADEWINDOWS] = cascadeWindows;
    operations[TILE] = tile;
    operations[ARRANGEICONS] = arrangeIcons;
}

void createWindowEx()
{
    QString style = valueStack->popString();
    int h = valueStack->popDouble();
    int w = valueStack->popDouble();
    int y = valueStack->popDouble();
    int x = valueStack->popDouble();
    QString contentSource = valueStack->popString();
    QString parentWinName = valueStack->popString();
    QString winName = valueStack->popString();

    int hSpace = 0;
    if(parentWinName.isEmpty() || windowManager->getWidget(parentWinName))
        hSpace = windowManager->createWindowEx(winName, parentWinName, contentSource, 0, x,y, w,h, style);

    valueStack->pushInt32(hSpace);
}

void loadSpaceWindow()
{
    QString style = valueStack->popString();
    QString fileName = valueStack->popString();
    QString windowName = valueStack->popString();

    //    if(fileName.isEmpty())
    //        fileName = executedObject->cls()->fileInfo().absoluteFilePath();
    //    fileName = executedProject->projectDir().absoluteFilePath(fileName);

    int hSpace = windowManager->loadSchemeWindow(windowName, fileName, style);
    valueStack->pushInt32(hSpace);
}

void openSchemeWindow()
{
    QString style = valueStack->popString();
    QString className = valueStack->popString();
    QString windowName = valueStack->popString();

    int hSpace = windowManager->openSchemeWindow(windowName, className, style);
    valueStack->pushInt32(hSpace);
}

void openClassScheme()
{
    /*int flags = */valueStack->popDouble();
    QString className = valueStack->popString();

    int hSpace = windowManager->openSchemeWindow(className, className, "");
    valueStack->pushInt32(hSpace);
}

void closeClassScheme()
{
    //    QString className = valueStack->popString();
    //    valueStack->pushDouble(windowManager->closeClassScheme(className));
    valueStack->popString();
    valueStack->pushDouble(0.0);
}

void closeWindow()
{
    QString windowName = valueStack->popString();
    valueStack->pushDouble(windowManager->closeWindow(windowName));
}

void getWindowName()
{
    int hSpace = valueStack->popInt32();
    QString windowName = windowManager->getWindowName(hSpace);

    valueStack->pushString(windowName);
}

void getClientWidth()
{
    QString winName = valueStack->popString();
    valueStack->pushDouble(windowManager->getClientWidth(winName));
}

void getClientHeight()
{
    QString winName = valueStack->popString();
    valueStack->pushDouble(windowManager->getClientHeight(winName));
}

void setClientSize()
{
    int h = valueStack->popDouble();
    int w = valueStack->popDouble();
    QString winName = valueStack->popString();
    valueStack->pushDouble(windowManager->setClientSize(winName, w, h));
}

void getWindowOrgX()
{
    QString winName = valueStack->popString();
    valueStack->pushDouble(windowManager->getWindowOrgX(winName));
}

void getWindowOrgY()
{
    QString winName = valueStack->popString();
    valueStack->pushDouble(windowManager->getWindowOrgY(winName));
}

void setWindowOrg()
{
    int y = valueStack->popDouble();
    int x = valueStack->popDouble();
    QString winName = valueStack->popString();

    valueStack->pushDouble(windowManager->setWindowOrg(winName, x, y));
}

void setWindowPos()
{
    int h = valueStack->popDouble();
    int w = valueStack->popDouble();
    int y = valueStack->popDouble();
    int x = valueStack->popDouble();
    QString winName = valueStack->popString();

    valueStack->pushDouble(windowManager->setWindowPos(winName, x, y, w, h));
}

void getWindowWidth()
{
    QString winName = valueStack->popString();
    valueStack->pushDouble(windowManager->getWindowWidth(winName));
}

void getWindowHeight()
{
    QString winName = valueStack->popString();
    valueStack->pushDouble(windowManager->getWindowHeight(winName));
}

void setWindowSize()
{
    int h = valueStack->popDouble();
    int w = valueStack->popDouble();
    QString winName = valueStack->popString();
    valueStack->pushDouble(windowManager->setWindowSize(winName, w, h));
}

void getWindowSpace()
{
    QString windowName = valueStack->popString();
    valueStack->pushInt32(windowManager->getWindowSpace(windowName));
}

void getWindowTitle()
{
    QString windowName = valueStack->popString();
    valueStack->pushString(windowManager->getWindowTitle(windowName));
}

void setWindowTitle()
{
    QString title = valueStack->popString();
    QString windowName = valueStack->popString();
    valueStack->pushDouble(windowManager->setWindowTitle(windowName, title));
}

void setScrollRange()
{
    double max = valueStack->popDouble();
    double min = valueStack->popDouble();
    int scrollType = valueStack->popDouble();
    QString windowName = valueStack->popString();
    if(SpaceWidget* w = windowManager->getWidget(windowName))
    {
        bool res = w->setScrollRange(scrollType, min, max);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
    //    VmLog::instance().error("'setScrollRange' - function not implemented");
}

void setWindowTransparentW()
{
    int transparentLevel = valueStack->popDouble();
    QString windowName = valueStack->popString();
    int result = windowManager->setWindowTransparentW(windowName, transparentLevel);
    valueStack->pushDouble(result);
}

void setWindowTransparentH()
{
    int transparentLevel = valueStack->popDouble();
    int spaceHandle = valueStack->popInt32();
    int result = windowManager->setWindowTransparentH(spaceHandle, transparentLevel);
    valueStack->pushDouble(result);
}

void setWindowTransparentColorW()
{
    int rgba = valueStack->popInt32();
    QString wName = valueStack->popString();
    int result = windowManager->setWindowTransparentColorW(wName, intToColor(rgba));
    valueStack->pushDouble(result);
}

void setWindowTransparentColorH()
{
    int rgba = valueStack->popInt32();
    int hSpace = valueStack->popInt32();
    int result = windowManager->setWindowTransparentColorH(hSpace, intToColor(rgba));
    valueStack->pushDouble(result);
}

void setWindowRegionW()
{
    int arrayHandle = valueStack->popInt32();
    QString windowName = valueStack->popString();

    int result = 0;
    if(Array* array = arrayManager->getArray(arrayHandle))
        result = windowManager->setWindowRegionW(windowName, array);

    valueStack->pushDouble(result);
}

void setWindowRegionH()
{
    int arrayHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    int result = 0;
    if(Array* array = arrayManager->getArray(arrayHandle))
        result = windowManager->setWindowRegionH(spaceHandle, array);

    valueStack->pushDouble(result);
}

void setWindowOwner()
{
    int parentSpaceHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    SpaceWidget* wp = windowManager->getWidget(parentSpaceHandle);
    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        w->setParent(wp);
        valueStack->pushDouble(1.0);
    }
    else
        valueStack->pushDouble(0.0);
}


void screenshot()
{
    double h = valueStack->popDouble();
    double w = valueStack->popDouble();
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();

    int spaceTargetHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();


    SpaceWidget* spaceTargetWidget = windowManager->getWidget(spaceTargetHandle);
    SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle);

    if(spaceWidget && spaceTargetWidget)
    {
        QImage img = QImage(w, h, QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&img);
        painter.setBrush(QBrush(QColor::fromRgb(255,255,255)));
        painter.drawRect(QRect(0,0, w,h));
        spaceWidget->spaceScene()->render(&painter, QRect(0,0, w,h), QRect(x, y, w, h), Qt::IgnoreAspectRatio);

        //static int scr_i = 0;
        //img.save(QString("scr_%1.png").arg(scr_i++), "PNG");

        QPixmap screenPixmap = QPixmap::fromImage(img);
        int dibHandle = spaceTargetWidget->spaceScene()->space()->createDib(screenPixmap, false);
        valueStack->pushInt32(dibHandle);
    }
    else
        valueStack->pushInt32(0);
}

void screenshotFull()
{
    int spaceHandle = valueStack->popInt32();
    int screenSpaceHandle = valueStack->popInt32();
    SpaceWidget* screenWidget = windowManager->getWidget(screenSpaceHandle);
    SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle);

    if(spaceWidget && screenWidget)
    {
        QPixmap pixmap = screenWidget->grab();
        int dibHandle = spaceWidget->spaceScene()->space()->createDib(pixmap, false);
        valueStack->pushInt32(dibHandle);
    }
    else
    {
        valueStack->pushInt32(0);
    }
}

void screenshotDesktop()
{
    double h = valueStack->popDouble();
    double w = valueStack->popDouble();
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();
    int spaceHandle = valueStack->popInt32();
    SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle);

    if(spaceWidget)
    {
        QPixmap pixmap = QGuiApplication::primaryScreen()->grabWindow(0, x,y, w,h);
        int dibHandle = spaceWidget->spaceScene()->space()->createDib(pixmap, false);
        valueStack->pushInt32(dibHandle);
    }
    else
        valueStack->pushInt32(0);
}

void screenshotDesktopFull()
{
    int spaceHandle = valueStack->popInt32();
    SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle);

    if(spaceWidget)
    {
        QPixmap pixmap = QGuiApplication::primaryScreen()->grabWindow(0);
        int dibHandle = spaceWidget->spaceScene()->space()->createDib(pixmap, false);
        valueStack->pushInt32(dibHandle);
    }
    else
        valueStack->pushInt32(0);
}

void showWindow()
{
    int showFlag = valueStack->popDouble();
    QString windowName = valueStack->popString();

    int res = windowManager->showWindow(windowName, showFlag);
    valueStack->pushDouble(res);
}

void bringWindowToTop()
{
    QString windowName = valueStack->popString();
    int res = windowManager->bringWindowToTop(windowName);
    valueStack->pushDouble(res);
}

void isIconic()
{
    QString windowName = valueStack->popString();
    int res = windowManager->isIconic(windowName);
    valueStack->pushDouble(res);
}

void isWindowExist()
{
    QString windowName = valueStack->popString();
    int res = windowManager->isWindowExist(windowName);
    valueStack->pushDouble(res);
}

void isWindowVisible()
{
    QString windowName = valueStack->popString();
    int res = windowManager->isWindowVisible(windowName);
    valueStack->pushDouble(res);
}

void getWindowProp()
{
    QString propName = valueStack->popString();
    QString windowName = valueStack->popString();

    QString propValue = windowManager->getWindowProp(windowName, propName);

    valueStack->pushString(propValue);
}

void cascadeWindows()
{
    VmLog::instance().error(QObject::tr("'CascadeWindows' - function not implemented"));
}

void tile()
{
    VmLog::instance().error(QObject::tr("'Tile' - function not implemented"));
}

void arrangeIcons()
{
    VmLog::instance().error(QObject::tr("'ArrangeIcons' - function not implemented"));
}

}
