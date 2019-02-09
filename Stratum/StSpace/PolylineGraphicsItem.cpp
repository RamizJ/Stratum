#include "PolylineGraphicsItem.h"

#include <SpaceItem.h>
#include <Space.h>
#include <Class.h>

#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <Tool2d.h>

#include "GraphicsItemType.h"

#include <HyperBase.h>
#include <PolylineArrow.h>
#include <QDebug>
#include <QPainter>
#include <QtMath>

using namespace StData;

namespace StSpace {

PolylineGraphicsItem::PolylineGraphicsItem(PolylineItem2d* polylineItem, QGraphicsItem* parent) :
    QObject(),
    QGraphicsPathItem(parent),
    SpaceGraphicsItem(polylineItem),
    m_polylineItem(polylineItem),
    m_emptyPen(Qt::NoPen),
    m_emptyBrush(Qt::NoBrush)
{
    setPen(m_polylineItem->getPen());
    setBrush(m_polylineItem->getBrush());

    updatePath();

    //    setPos(m_polylineItem->origin());
    setRotation(m_polylineItem->angle());

    setVisible(m_polylineItem->isVisible());
    setZValue(m_polylineItem->zOrder());

    if(PenTool* pen = m_polylineItem->penTool())
        connect(pen, &PenTool::changed, this, &PolylineGraphicsItem::penChanged);

    if(BrushTool* brush = m_polylineItem->brushTool())
        connect(brush, &BrushTool::changed, this, &PolylineGraphicsItem::brushChanged);
}

bool PolylineGraphicsItem::insertPoint(int index, const QPointF& point)
{
    if(index < -1 || index > m_polylineItem->polyline().count())
        return false;

    if(index == -1)
        m_polylineItem->addPoint(point);
    else
        m_polylineItem->insertPoint(index, point);

    updatePath();
    return true;
}

bool PolylineGraphicsItem::removePoint(int index)
{
    if(m_polylineItem->polyline().isEmpty() || index > m_polylineItem->polyline().count() ||
       m_polylineItem->polyline().count()==1)
        return false;

    if(index < 0)
        m_polylineItem->removePoint(m_polylineItem->polyline().count()-1);
    else
        m_polylineItem->removePoint(index);

    updatePath();
    return true;
}

void PolylineGraphicsItem::setPenTool(PenTool* penTool)
{
    if(penTool == m_polylineItem->penTool())
        return;

    if(m_polylineItem->penTool())
        disconnect(m_polylineItem->penTool());

    m_polylineItem->setPenTool(penTool);
    if(penTool)
    {
        setPen(penTool->pen());
        connect(penTool, &PenTool::changed, this, &PolylineGraphicsItem::penChanged);
    }
    else
    {
        setPen(Qt::NoPen);
    }
}

void PolylineGraphicsItem::setBrushTool(BrushTool* brushTool)
{
    if(brushTool == m_polylineItem->brushTool())
        return;

    if(m_polylineItem->brushTool())
        disconnect(m_polylineItem->brushTool());

    m_polylineItem->setBrushTool(brushTool);
    if(brushTool)
    {
        setBrush(brushTool->brush());
        updatePath();
        connect(brushTool, &BrushTool::changed, this, &PolylineGraphicsItem::brushChanged);
    }
    else
    {
        setBrush(Qt::NoBrush);
    }
}

QPointF PolylineGraphicsItem::getPoint(int pointIndex)
{
    if(pointIndex < 0 || pointIndex >= m_polylineItem->polyline().count())
        return QPointF();

    return m_polylineItem->polyline().at(pointIndex);
}

bool PolylineGraphicsItem::setPoint(int pointIndex, const QPointF& point)
{
    if(pointIndex < 0 || pointIndex >= m_polylineItem->polyline().count())
        return false;

    if(point != m_polylineItem->getPoint(pointIndex))
    {
        m_polylineItem->setPoint(pointIndex, point);
        updatePath();
    }
    return true;
}

int PolylineGraphicsItem::getPolylineFillRule()
{
    return m_polylineItem->fillRule();
}

void PolylineGraphicsItem::setPolylineFillRule(int fillRule)
{
    m_polylineItem->setFillRule(fillRule);

    QPainterPath painterPath = path();
    painterPath.setFillRule(fillRule == 1 ? Qt::OddEvenFill : Qt::WindingFill);
    setPath(painterPath);
}

void PolylineGraphicsItem::setLineArrows(double aa, double al, double as, double ba, double bl, double bs)
{
    m_polylineItem->setLineArrows(aa, al, as, ba, bl, bs);
    updatePath();
}

void PolylineGraphicsItem::setSpaceItemOrg(double x, double y)
{
    QPointF org(x, y);
    if(org != m_polylineItem->origin())
    {
        m_polylineItem->setOrigin(org);
        setPos(m_polylineItem->origin());
    }
}

void PolylineGraphicsItem::updatePosition()
{
    if(pos() != m_polylineItem->origin())
        setPos(m_polylineItem->origin());
}

void PolylineGraphicsItem::updateSize()
{
    if(path().boundingRect().size() != m_polylineItem->size())
    {
        prepareGeometryChange();
        updatePath();
    }
}

void PolylineGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsPathItem::mousePressEvent(event);
    if(m_hyperJumpActivated)
        spaceItem()->invokeHyperJump();
}

void PolylineGraphicsItem::setSpaceItemSize(double w, double h)
{
    QSizeF newSize(w, h);
    if(newSize != m_polylineItem->size())
    {
        m_polylineItem->setSize(newSize);
        updatePath();
    }
}

void PolylineGraphicsItem::rotateObject2d(const QPointF& base, const double& angle)
{
    for(int i  = 0; i < m_polylineItem->pointsCount(); i++)
    {
        //        if(angle != 0.0)
        {
            QPointF point = m_polylineItem->getPoint(i);
            rotatePoint(&point, base, angle);
            m_polylineItem->setPoint(i, point);
        }
    }
    updatePath();
}

int PolylineGraphicsItem::type() const
{
    return QGraphicsItem::UserType + static_cast<int>(GraphicsItemType::Polyline);
}

QRectF PolylineGraphicsItem::boundingRect() const
{
//        double w = pen().width();
    //    QRectF rect = QGraphicsPathItem::boundingRect().adjusted(-w,-w, w,w);
    QRectF br = QGraphicsPathItem::boundingRect();
//    QRectF sr = QGraphicsPathItem::shape().boundingRect().adjusted(-w,-w, w,w);
    return br;
}

QPainterPath PolylineGraphicsItem::shape() const
{
    QPainterPath polylineShape = path();

    if(!m_startArrowPath.isEmpty())
        polylineShape.addPath(m_startArrowPath);

    if(!m_endArrowPath.isEmpty())
        polylineShape.addPath(m_endArrowPath);

    return polylineShape;
}

void PolylineGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
    if(m_polylineItem->isPoint())
    {
        if(m_polylineItem->getPen().style() == Qt::NoPen)
            return;

        int penRop = m_polylineItem->penTool() ? m_polylineItem->penTool()->rop() : 0;

        double pointRadius = pen().widthF() / 2;
        painter->save();

        setCompositionModeByRop(painter, penRop);
        painter->setPen(Qt::NoPen);
        painter->setBrush(pen().brush());
        painter->drawEllipse(QPointF(), pointRadius, pointRadius);
        painter->restore();
    }
    else
    {
        int penRop = m_polylineItem->penTool() ? m_polylineItem->penTool()->rop() : 0;
        int brushRop = m_polylineItem->brushTool() ? m_polylineItem->brushTool()->rop() : 0;

        bool isComposed = (penRop != PenTool::RS_CopyPen) ||
                          (brushRop != PenTool::RS_CopyPen);

        painter->save();
        {
            if(isComposed)
            {
                if(penRop == brushRop)
                {
                    setCompositionModeByRop(painter, penRop);
                    setPenByRop(painter, penRop);
                    setBrushByRop(painter, brushRop);
                    painter->drawPath(path());
                }
                else
                {
                    setCompositionModeByRop(painter, brushRop);
                    painter->setPen(Qt::NoPen);
                    setBrushByRop(painter, brushRop);
                    painter->drawPath(path());

                    setCompositionModeByRop(painter, penRop);
                    setPenByRop(painter, penRop);
                    painter->setBrush(Qt::NoBrush);
                    painter->drawPath(path());
                }
            }
            else
            {
                painter->setPen(pen());
                painter->setBrush(brush());
                painter->drawPath(path());
            }

            PolylineArrowPtr arrow = m_polylineItem->arrow();
            if(arrow != nullptr)
            {
                painter->setPen(pen());
                painter->setBrush(Qt::NoBrush);
                if(arrow->isStartFill())
                    painter->setBrush(QBrush(pen().color()));

                if(!m_startArrowPath.isEmpty())
                    painter->drawPath(m_startArrowPath);

                painter->setBrush(Qt::NoBrush);
                if(arrow->isEndFill())
                    painter->setBrush(QBrush(pen().color()));

                if(!m_endArrowPath.isEmpty())
                    painter->drawPath(m_endArrowPath);
            }
        }
        painter->restore();
    }
}

void PolylineGraphicsItem::updatePath()
{
    prepareGeometryChange();

    QPainterPath path;

    QPolygonF polyline = m_polylineItem->polyline();
    polyline = roundPathPoints(polyline);

    QRectF polylineRect = polyline.boundingRect();
    QPointF offset = polylineRect.topLeft();

    polyline.translate(-offset);

    //Если есть заливка, то замыкаем полилинию, так как это было в старом стратуме
    if(m_polylineItem->isPoint())
    {
        double penWidth = pen().widthF();
        path.addEllipse(-penWidth/2,-penWidth/2, penWidth, penWidth);
    }
    else
    {
        if(m_polylineItem->brushTool())
        {
            if(!polyline.isEmpty() && polyline.first() != polyline.last())
                polyline.append(polyline.first());

            path.addPolygon(polyline);
        }
        else
        {
            //Так сделано чтобы фигура выделялась мышью только при клике по контуру
            for(int i = 0; i < polyline.count()-1; i++)
            {
                QPolygonF line;
                line << polyline.at(i);
                line << polyline.at(i+1);

                path.addPolygon(line);
            }
        }
        path.setFillRule(m_polylineItem->fillRule() == 1 ? Qt::OddEvenFill : Qt::WindingFill);

    }
    setPath(path);
    setPos(offset);

    buildArrows(-offset);
}

QPolygonF PolylineGraphicsItem::roundPathPoints(const QPolygonF& polyline)
{
    return QPolygonF(polyline.toPolygon());
}

void PolylineGraphicsItem::buildArrows(const QPointF& offset)
{
    m_startArrowPath = QPainterPath();
    m_endArrowPath = QPainterPath();

    PolylineArrowPtr arrow = m_polylineItem->arrow();
    if(arrow == nullptr || m_polylineItem->pointsCount() <= 1 || m_polylineItem->isClosed())
        return;

    QPolygonF polyline = m_polylineItem->polyline();
    double startLength = arrow->startLength();
    double endLength = arrow->endLength();

    auto createArrow = [](const QPointF& p1, const QPointF& p2, double angle, double length, bool fill) -> QPolygonF
    {
        QLineF line1 = QLineF(p1, p2);
        QLineF line2 = QLineF(p1, p2);
        line1.setLength(length);
        line2.setLength(length);
        line1.setAngle(line1.angle() - angle);
        line2.setAngle(line2.angle() + angle);

        QPolygonF arrowShape;
        if(fill)
            arrowShape << line1.p1() << line1.p2() << line2.p2() << line1.p1();
        else
            arrowShape << line1.p1() << line1.p2() << line1.p1() << line2.p2();

        return arrowShape;
    };

    //Строим первую стрелку
    if(startLength > 0)
    {
        double angle = qRadiansToDegrees(arrow->startAngle());

        QPolygonF arrowShape = createArrow(polyline.at(0) + offset, polyline.at(1) + offset, angle, startLength, arrow->isStartFill());
        m_startArrowPath.addPolygon(arrowShape);
    }

    //Строим вторую стрелку
    if(endLength > 0)
    {
        double angle = qRadiansToDegrees(arrow->endAngle());
        int lastIndex = polyline.count() - 1;

        QPolygonF arrowShape = createArrow(polyline.at(lastIndex) + offset, polyline.at(lastIndex-1) + offset, angle, endLength, arrow->isEndFill());
        m_endArrowPath.addPolygon(arrowShape);
    }
}

void PolylineGraphicsItem::setCompositionModeByRop(QPainter* painter, int rop)
{
    switch(rop)
    {
        case PenTool::RS_MaskPen:
            painter->setCompositionMode(QPainter::RasterOp_SourceAndDestination);
            break;

        case PenTool::RS_NotMergePen:
            painter->setCompositionMode(QPainter::RasterOp_NotSourceAndNotDestination);
            break;

        case PenTool::RS_MaskPenNot:
            painter->setCompositionMode(QPainter::RasterOp_SourceAndNotDestination);
            break;

        case PenTool::RS_MaskNotPen:
            painter->setCompositionMode(QPainter::RasterOp_NotSourceAndDestination);
            break;

        case PenTool::RS_NotMaskPen:
            painter->setCompositionMode(QPainter::RasterOp_NotSourceOrNotDestination);
            break;

        case PenTool::RS_MergeNotPen:
            painter->setCompositionMode(QPainter::RasterOp_NotSourceOrDestination);
            break;

        case PenTool::RS_MergePen:
            painter->setCompositionMode(QPainter::RasterOp_SourceOrDestination);
            break;

        case PenTool::RS_MergePenNot:
            painter->setCompositionMode(QPainter::RasterOp_SourceOrNotDestination);
            break;

        case PenTool::RS_NotXorPen:
            painter->setCompositionMode(QPainter::RasterOp_NotSourceXorDestination);
            break;

        case PenTool::RS_XorPen:
            painter->setCompositionMode(QPainter::RasterOp_SourceXorDestination);
            break;

        case PenTool::RS_NotCopyPen:
            painter->setCompositionMode(QPainter::RasterOp_NotSource);
            break;

        case PenTool::RS_Not:
            painter->setCompositionMode(QPainter::RasterOp_NotDestination);
            break;

        case PenTool::RS_Nop:
            painter->setCompositionMode(QPainter::CompositionMode_Destination);
            break;

        default:
            painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
            break;
    }
}

void PolylineGraphicsItem::setPenByRop(QPainter* painter, int rop)
{
    if(rop == PenTool::RS_Black)
    {
        QPen pen = this->pen();
        pen.setColor(Qt::black);
        painter->setPen(pen);
    }
    else if(rop == PenTool::RS_White)
    {
        QPen pen = this->pen();
        pen.setColor(Qt::white);
        painter->setPen(pen);
    }
    else
        painter->setPen(pen());
}

void PolylineGraphicsItem::setBrushByRop(QPainter* painter, int rop)
{
    if(rop == PenTool::RS_Black)
    {
        QBrush brush = this->brush();
        brush.setColor(Qt::black);
        painter->setBrush(brush);
    }
    else if(rop == PenTool::RS_White)
    {
        QBrush brush = this->brush();
        brush.setColor(Qt::white);
        painter->setBrush(brush);
    }
    else
        painter->setBrush(brush());
}

void PolylineGraphicsItem::penChanged()
{
    setPen(m_polylineItem->getPen());
    if(m_polylineItem->isPoint())
        updatePath();
}

void PolylineGraphicsItem::brushChanged()
{
    setBrush(m_polylineItem->getBrush());
}

}
