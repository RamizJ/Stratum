#include "PenFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"

#include <Tool2d.h>
#include <SpaceWidget.h>
#include <Space.h>

using namespace StData;
using namespace StSpace;

namespace StVirtualMachine {

void setupPenFunctions()
{
    operations[CREATEPEN2D] = createPen2d;

    operations[GETPENCOLOR2D] = getPenColor2d;
    operations[GETPENROP2D] = getPenROP2d;
    operations[GETPENSTYLE2D] = getPenStyle2d;
    operations[GETPENWIDTH2D] = getPenWidth2d;

    operations[SETPENCOLOR2D] = setPenColor2d;
    operations[SETPENROP2D] = setPenROP2d;
    operations[SETPENSTYLE2D] = setPenStyle2d;
    operations[SETPENWIDTH2D] = setPenWidth2d;
}

void createPen2d()
{
    qint16 rop = valueStack->popDouble();
    int rgba = valueStack->popInt32();
    int width = valueStack->popDouble();
    int style = valueStack->popDouble();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(spaceWidget->space()->createPen(rop, rgba, width, style));
    else
        valueStack->pushInt32(0);
}

void getPenColor2d()
{
    int penHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        if(PenTool* penInfo = spaceWidget->space()->getPen(penHandle))
        {
            valueStack->pushInt32(penInfo->rgba());
            return;
        }
    }
    valueStack->pushInt32(0);
}

void getPenROP2d()
{
    int penHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        if(PenTool* penInfo = spaceWidget->space()->getPen(penHandle))
        {
            valueStack->pushDouble(penInfo->rop());
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void getPenStyle2d()
{
    int penHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        if(PenTool* penInfo = spaceWidget->space()->getPen(penHandle))
        {
            valueStack->pushDouble(penInfo->style());
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void getPenWidth2d()
{
    int penHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        if(PenTool* penInfo = spaceWidget->space()->getPen(penHandle))
        {
            valueStack->pushDouble(penInfo->width());
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void setPenROP2d()
{
    qint16 rop = valueStack->popDouble();
    int penHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        if(PenTool* penInfo = spaceWidget->space()->getPen(penHandle))
        {
            penInfo->setRop(rop);
            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void setPenColor2d()
{
    int rgba = valueStack->popInt32();
    int penHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        if(PenTool* penInfo = spaceWidget->space()->getPen(penHandle))
        {
            penInfo->setRgba(rgba);
            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void setPenStyle2d()
{
    int style = valueStack->popDouble();
    int penHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        if(PenTool* penTool = spaceWidget->space()->getPen(penHandle))
        {
            penTool->setStyle(style);
            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void setPenWidth2d()
{
    double width = valueStack->popDouble();
    int penHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        if(PenTool* penInfo = spaceWidget->space()->getPen(penHandle))
        {
            penInfo->setWidth(width);
            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

}
