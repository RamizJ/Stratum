#include "PixmapFunctions.h"
#include "FileSystemFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"

#include <Tool2d.h>
#include <SpaceItem.h>
#include <Space.h>
#include <SpaceWidget.h>
#include <SpaceScene.h>
#include <StDataGlobal.h>
#include <PathManager.h>

using namespace StData;
using namespace StSpace;

namespace StVirtualMachine {

void setupTextureFunctions()
{
    operations[CREATEDID2D] = createDib2d;
    operations[CREATERDID2D] = createRDib2d;
    operations[CREATEDOUBLEDID2D] = createDoubleDIB2d;
    operations[CREATERDOUBLEDID2D] = createRefDoubleDib2d;
    operations[CREATEBITMAP2D] = createBitmap2d;
    operations[CREATEDOUBLEBITMAP2D] = createDoubleBitmap2d;

    operations[GETPIXEL2D] = getDibPixel2d;
    operations[VM_SETPIXEL2D] = setDibPixel2d;

    operations[GETBITMAPSRCRECT] = getBitmapSrcRect2d;
    operations[SETBITMAPSRCRECT] = setBitmapSrcRect2d;

    operations[VM_GETDIBOBJECT2D] = getDibObject2d;
    operations[VM_SETDIBOBJECT2D] = setDibObject2d;

    operations[VM_GETDDIBOBJECT2D] = getDDibObject2d;
    operations[VM_SETDDIBOBJECT2D] = setDDibObject2d;
}

void createDib2d()
{
    QString fileName = valueStack->popString();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        if(QFileInfo(fileName).isRelative())
            fileName = absolutePath(fileName);
        else
            fileName = PathManager::cleanPath(fileName);

        valueStack->pushInt32(widget->space()->createDib(fileName));
    }
    else
        valueStack->pushInt32(0);
}

void createRDib2d()
{
    QString fileName = valueStack->popString();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(widget->space()->createRDib(fileName));
    else
        valueStack->pushInt32(0);
}

void createDoubleDIB2d()
{
    QString fileName = valueStack->popString();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        QFileInfo fileInfo(fileName);
        if(fileInfo.isRelative())
            fileName = absolutePath(fileName);
        else
            fileName = PathManager::cleanPath(fileName);

        valueStack->pushInt32(widget->space()->createDoubleDib(fileName));
    }
    else
        valueStack->pushInt32(0);
}

void createRefDoubleDib2d()
{
    QString fileName = valueStack->popString();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(widget->space()->createDoubleRDib(fileName));
    else
        valueStack->pushInt32(0);
}

void createBitmap2d()
{
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();
    int dibHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(widget->spaceScene()->createPixmap(dibHandle, x, y));
    else
        valueStack->pushInt32(0);
}

void createDoubleBitmap2d()
{
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();
    int doubleDibHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(widget->spaceScene()->createMaskedPixmap(doubleDibHandle, x, y));
    else
        valueStack->pushInt32(0);
}

void getDibPixel2d()
{
    int y = valueStack->popDouble();
    int x = valueStack->popDouble();
    int dibHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        QColor pixelColor = widget->space()->getDibPixelRgba(dibHandle, x, y);
        valueStack->pushInt32(colorToInt(pixelColor));
        return;
    }
    valueStack->pushInt32(0);
}

void setDibPixel2d()
{
    int rgba = valueStack->popInt32();
    int y = valueStack->popDouble();
    int x = valueStack->popDouble();
    int dibHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        bool res = widget->space()->setDibPixelColor(dibHandle, x, y, rgba);
        valueStack->pushDouble(res);
        return;
    }
    valueStack->pushDouble(0.0);
}

void getBitmapSrcRect2d()
{
    VarData* height = valueStack->popVarData();
    VarData* width = valueStack->popVarData();
    VarData* y = valueStack->popVarData();
    VarData* x = valueStack->popVarData();
    int pixmapItemHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        SpaceItem* spaceItem = widget->space()->getSpaceItem(pixmapItemHandle);
        if(TextureItem2d* pixmapItem = dynamic_cast<TextureItem2d*>(spaceItem))
        {
            x->setDouble(pixmapItem->textureOrigin().x());
            y->setDouble(pixmapItem->textureOrigin().y());
            width->setDouble(pixmapItem->textureSize().width());
            height->setDouble(pixmapItem->textureSize().height());

            valueStack->pushDouble(1.0);
            return;
        }
    }
    valueStack->pushDouble(0.0);
}

void setBitmapSrcRect2d()
{
    double height = valueStack->popDouble();
    double width = valueStack->popDouble();
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();
    int pixmapItemHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    bool res = false;
    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        res = widget->spaceScene()->setPixmapSrcRect(pixmapItemHandle, x, y, width, height);

    valueStack->pushDouble(res);
}

void getDibObject2d()
{
    int pixmapItemHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        int dibHandle = widget->spaceScene()->getDibObject(pixmapItemHandle);
        valueStack->pushInt32(dibHandle);
    }
    else
    {
        valueStack->pushInt32(0);
    }
}

void setDibObject2d()
{
    int dibHandle = valueStack->popInt32();
    int pixmapItemHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        double res = widget->spaceScene()->setDibObject(pixmapItemHandle, dibHandle);
        valueStack->pushDouble(res);
    }
    else
    {
        valueStack->pushDouble(0.0);
    }

}

void getDDibObject2d()
{
    getDibObject2d();
}

void setDDibObject2d()
{
    int doubleDibHandle = valueStack->popInt32();
    int pixmapItemHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
    {
        valueStack->pushDouble(widget->spaceScene()->setDoubleDibObject(pixmapItemHandle, doubleDibHandle));
        return;
    }
    valueStack->pushDouble(0.0);
}


}
