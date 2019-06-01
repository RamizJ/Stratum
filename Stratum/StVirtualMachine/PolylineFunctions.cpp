#include "PolylineFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"

#include <Tool2d.h>
#include <Space.h>
#include <SpaceWidget.h>
#include <SpaceItem.h>
#include <SpaceScene.h>
#include <Log.h>

using namespace StData;
using namespace StSpace;

namespace StVirtualMachine {

void setupPolylineFunctions()
{
    operations[CREATELINE2D] = createLine2d;
    operations[CREATEPOLYLINE2D] = createPolyLine2d;

    operations[GETVECTORNUMPOINTS2D] = getVectorNumPoints2d;
    operations[ADDPOINT2D] = addPoint2d;
    operations[DELPOINT2D] = delPoint2d;

    operations[GETBRUSHOBJECT2D] = getBrushObject2d;
    operations[SETBRUSHOBJECT2D] = setBrushObject2d;

    operations[GETPENOBJECT2D] = getPenObject2d;
    operations[SETPENOBJECT2D] = setPenObject2d;

    operations[GETVECTORPOINT2DX] = getVectorPoint2dX;
    operations[GETVECTORPOINT2DY] = getVectorPoint2dY;
    operations[SETVECTORPOINT2D] = setVectorPoint2d;

    operations[GETRGNCREATEMODE] = getRgnCreateMode;
    operations[SETRGNCREATEMODE] = setRgnCreateMode;
    operations[SETARROW2D] = setLineArrows2d;
}

void createLine2d()
{
    QPointF point;
    point.setY(valueStack->popDouble());
    point.setX(valueStack->popDouble());

    int brushHandle = valueStack->popInt32();
    int penHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    QPolygonF polyline;
    polyline << point;

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(widget->createPolyline(penHandle, brushHandle, polyline));
    else
        valueStack->pushInt32(0);
}

void createPolyLine2d()
{
    qint16 pointsCount = codePointer->getCode() / 2;
    codePointer->incPosition();

    QPolygonF polyline(pointsCount);
    for(int i = pointsCount - 1; i >= 0; i--)
    {
        QPointF point;
        point.setY(valueStack->popDouble());
        point.setX(valueStack->popDouble());
        polyline[i] = point;
    }

    int brushHandle = valueStack->popInt32();
    int penHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    //SystemLog::instance().info(QString("Polyline brush: %1. Pen: %2").arg(brushHandle).arg(penHandle));

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(widget->createPolyline(penHandle, brushHandle, polyline));
    else
        valueStack->pushInt32(0);
}

void getVectorNumPoints2d()
{
    int polylineHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(widget->spaceScene()->getPolylinePointsCount(polylineHandle));
    else
        valueStack->pushDouble(0.0);
}

void addPoint2d()
{
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();
    int index = valueStack->popDouble();
    int polylineHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(widget->spaceScene()->addPolylinePoint(polylineHandle, index, x, y));
    else
        valueStack->pushDouble(0.0);
}

void delPoint2d()
{
    int index = valueStack->popDouble();
    int polylineHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(widget->spaceScene()->delPolylinePoint(polylineHandle, index));
    else
        valueStack->pushDouble(0.0);
}

void getPenObject2d()
{
    int polylineHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(widget->spaceScene()->getPolylinePen(polylineHandle));
    else
        valueStack->pushInt32(0);
}

void setPenObject2d()
{
    int penHandle = valueStack->popInt32();
    int polylineHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(widget->spaceScene()->setPolylinePen(polylineHandle, penHandle));
    else
        valueStack->pushInt32(0);
}

void getBrushObject2d()
{
    int polylineHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(widget->spaceScene()->getPolylineBrush(polylineHandle));
    else
        valueStack->pushInt32(0);
}

void setBrushObject2d()
{
    int brushHandle = valueStack->popInt32();
    int polylineHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushInt32(widget->spaceScene()->setPolylineBrush(polylineHandle, brushHandle));
    else
        valueStack->pushInt32(0);
}


void getVectorPoint2dX()
{
    int index = valueStack->popDouble();
    int polylineHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(widget->spaceScene()->getPolylinePointX(polylineHandle, index));
    else
        valueStack->pushDouble(0.0);
}

void getVectorPoint2dY()
{
    int index = valueStack->popDouble();
    int polylineHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(widget->spaceScene()->getPolylinePointY(polylineHandle, index));
    else
        valueStack->pushDouble(0.0);
}

void setVectorPoint2d()
{
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();
    int index = valueStack->popDouble();
    int polylineHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(widget->spaceScene()->setPolylinePoint(polylineHandle, index, x, y));
    else
        valueStack->pushDouble(0.0);
}

void getRgnCreateMode()
{
    int polylineHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(widget->spaceScene()->getPolylineFillRule(polylineHandle));
    else
        valueStack->pushDouble(0.0);
}

void setRgnCreateMode()
{
    int fillRule = valueStack->popDouble();
    int polylineHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(widget->spaceScene()->setPolylineFillRule(polylineHandle, fillRule));
    else
        valueStack->pushDouble(0.0);
}

void setLineArrows2d()
{
    double bs = valueStack->popDouble();
    double bl = valueStack->popDouble();
    double ba = valueStack->popDouble();

    double as = valueStack->popDouble();
    double al = valueStack->popDouble();
    double aa = valueStack->popDouble();

    int polylineHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* widget = windowManager->getWidget(spaceHandle))
        valueStack->pushDouble(widget->spaceScene()->setLineArrows(polylineHandle, aa, al, as, ba, bl, bs));
    else
        valueStack->pushDouble(0.0);
}

}
