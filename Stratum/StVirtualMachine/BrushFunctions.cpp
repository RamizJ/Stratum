#include "BrushFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"

#include <Space.h>
#include <SpaceWidget.h>
#include <Tool2d.h>

using namespace StData;
using namespace StSpace;

namespace StVirtualMachine {

void setupBrushFunctions()
{
    operations[CREATEBRUSH2D] = createBrush2d;

    operations[GETBRUSHCOLOR2D] = getBrushColor2d;
    operations[GETBRUSHROP2D] = getBrushROP2d;
    operations[GETBRUSHSTYLE2D] = getBrushStyle2d;
    operations[GETBRUSHHATCH2D] = getBrushHatch2d;
    operations[GETBRUSHDIB2D] = getBrushDib2d;

    operations[SETBRUSHCOLOR2D] = setBrushColor2d;
    operations[SETBRUSHROP2D] = setBrushROP2d;
    operations[SETBRUSHSTYLE2D] = setBrushStyle2d;
    operations[SETBRUSHHATCH2D] = setBrushHatch2d;
    operations[SETBRUSHDIB2D] = setBrushDib2d;
}

void createBrush2d()
{
    int rop = static_cast<int>(valueStack->popDouble());
    int dibHandle = valueStack->popInt32();
    int rgba = valueStack->popInt32();
    int hatch = static_cast<int>(valueStack->popDouble());
    int style = static_cast<int>(valueStack->popDouble());
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(spaceWidget->space()->createBrush(style, hatch, rgba, dibHandle, rop));
    else
        valueStack->pushInt32(0);
}

void getBrushColor2d()
{
    int brushHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        BrushTool* brushInfo = spaceWidget->space()->getBrush(brushHandle);
        if(brushInfo)
        {
            valueStack->pushInt32(brushInfo->rgba());
            return;
        }
    }
    valueStack->pushInt32(0);
}

void getBrushROP2d()
{
    int brushHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        BrushTool* brushInfo = spaceWidget->space()->getBrush(brushHandle);
        if(brushInfo)
        {
            valueStack->pushDouble(brushInfo->rop());
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void getBrushStyle2d()
{
    int brushHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        BrushTool* brushInfo = spaceWidget->space()->getBrush(brushHandle);
        if(brushInfo)
        {
            valueStack->pushDouble(brushInfo->style());
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void getBrushHatch2d()
{
    int brushHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        BrushTool* brushInfo = spaceWidget->space()->getBrush(brushHandle);
        if(brushInfo)
        {
            valueStack->pushDouble(brushInfo->hatch());
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void getBrushDib2d()
{
    int brushHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
        if(BrushTool* brush = spaceWidget->space()->getBrush(brushHandle))
            if(brush->texture())
            {
                valueStack->pushInt32(brush->texture()->handle());
                return;
            }

    valueStack->pushInt32(0);
}

void setBrushColor2d()
{
    int rgba = valueStack->popInt32();
    int brushHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        BrushTool* brushInfo = spaceWidget->space()->getBrush(brushHandle);
        if(brushInfo)
        {
            brushInfo->setRgba(rgba);
            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void setBrushROP2d()
{
    int rop = valueStack->popDouble();
    int brushHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        BrushTool* brushInfo = spaceWidget->space()->getBrush(brushHandle);
        if(brushInfo)
        {
            brushInfo->setRop(rop);
            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void setBrushStyle2d()
{
    int style = valueStack->popDouble();
    int brushHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        BrushTool* brushInfo = spaceWidget->space()->getBrush(brushHandle);
        if(brushInfo)
        {
            brushInfo->setStyle(style);
            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void setBrushHatch2d()
{
    int hatch = valueStack->popDouble();
    int brushHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        BrushTool* brushInfo = spaceWidget->space()->getBrush(brushHandle);
        if(brushInfo)
        {
            brushInfo->setHatch(hatch);
            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void setBrushDib2d()
{
    int dibHadnle = valueStack->popDouble();
    int brushHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        if(BrushTool* brush = spaceWidget->space()->getBrush(brushHandle))
        {
            TextureTool* texture = spaceWidget->space()->getTexture(dibHadnle);
            brush->setTexture(texture);
            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void setBrushColors2d()
{
    int rgba = valueStack->popInt32();
    int brushHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* spaceWidget = windowManager->getWidget(spaceHandle))
    {
        BrushTool* brushInfo = spaceWidget->space()->getBrush(brushHandle);
        if(brushInfo)
        {
            brushInfo->setRgba(rgba);
            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}


}
