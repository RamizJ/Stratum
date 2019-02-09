#include "ToolFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"
#include "VmLog.h"

#include <StDataGlobal.h>

#include <QColor>
#include <Space.h>
#include <SpaceWidget.h>

using namespace StData;
using namespace StSpace;

namespace StVirtualMachine {

void setupToolFunctions()
{
    //Tools
    operations[DELETETOOL2D] = deleteTool2d;
    operations[VM_GETNEXTTOOL2D] = getNextTool2d;
    operations[VM_GETTOOLREF2D] = getToolRef2d;

    //Colors
    operations[RGB_COLOR] = rgb;
    operations[RGB_COLORex] = rgbEx;
    operations[VM_GETRVALUE] = getRValue;
    operations[VM_GETGVALUE] = getGValue;
    operations[VM_GETBVALUE] = getBValue;
}

//Tools
void deleteTool2d()
{
    int toolHandle = valueStack->popInt32();
    int toolType = valueStack->popDouble();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        bool res = w->space()->deleteTool(toolHandle, toolType);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
}

void getNextTool2d()
{
    valueStack->popInt32();
    valueStack->popDouble();
    valueStack->popInt32();

    VmLog::instance().error("'getNextTool2d' - function not implemented");

    valueStack->pushInt32(0.0);
}

void getToolRef2d()
{
    valueStack->popInt32();
    valueStack->popDouble();
    valueStack->popInt32();

    VmLog::instance().error("'getToolRef2d' - function not implemented");

    valueStack->pushDouble(0.0);
}

//Colors
void rgb()
{
    quint8 b = valueStack->popDouble();
    quint8 g = valueStack->popDouble();
    quint8 r = valueStack->popDouble();

    valueStack->pushInt32(getRGBA(r,g,b));
}

void rgbEx()
{
    quint8 a = valueStack->popDouble();
    quint8 b = valueStack->popDouble();
    quint8 g = valueStack->popDouble();
    quint8 r = valueStack->popDouble();

    valueStack->pushInt32(getRGBA(r,g,b,a));
}

void getRValue()
{
    QColor color = intToColor(valueStack->popInt32());
    valueStack->pushDouble(color.red());
}

void getGValue()
{
    QColor color = intToColor(valueStack->popInt32());
    valueStack->pushDouble(color.green());
}

void getBValue()
{
    QColor color = intToColor(valueStack->popInt32());
    valueStack->pushDouble(color.blue());
}

int getRGBA(quint8 r, quint8 g, quint8 b, quint8 a)
{
    return rgbaToInt(r, g, b, a);
}

}
