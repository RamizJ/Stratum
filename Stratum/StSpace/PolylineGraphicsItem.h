#ifndef SPACE2DGRAPHICSITEM_H
#define SPACE2DGRAPHICSITEM_H

#include "stspace_global.h"
#include "SpaceGraphicsItem.h"

#include <QGraphicsItem>
#include <QPen>

namespace StData {
class PenTool;
class BrushTool;
class PolylineItem2d;
}

namespace StSpace {

class STSPACESHARED_EXPORT PolylineGraphicsItem : public QObject, public QGraphicsPathItem, public SpaceGraphicsItem
{
    Q_OBJECT
public:
    PolylineGraphicsItem(StData::PolylineItem2d* polylineItem, QGraphicsItem* parent = 0);

    QGraphicsItem* graphicsItem() {return this;}

    StData::PolylineItem2d* polylineItem() const {return m_polylineItem;}

    bool insertPoint(int index, const QPointF& point);
    bool removePoint(int index);

    void setPenTool(StData::PenTool* penTool);
    void setBrushTool(StData::BrushTool* brush);

    QPointF getPoint(int pointIndex);
    bool setPoint(int pointIndex, const QPointF& point);

    int getPolylineFillRule();
    void setPolylineFillRule(int fillRule);

    void setLineArrows(double aa, double al, double as, double ba, double bl, double bs);

    // QGraphicsItem interface
public:
    int type() const;
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    // ISpaceGraphicsItem interface
public:
    virtual void setSpaceItemOrg(double x, double y);
    virtual void setSpaceItemSize(double w, double h);
    virtual void rotateObject2d(const QPointF& base, const double& angle);

    void updatePosition();
    void updateSize();

    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

private slots:
    void penChanged();
    void brushChanged();

private:
    void updatePath();
    QPolygonF roundPathPoints(const QPolygonF& polyline);
    void buildArrows(const QPointF& offset);

//    bool polylineIsPoint() const;
    void setCompositionModeByRop(QPainter* painter, int rop);
    void setPenByRop(QPainter* painter, int rop);
    void setBrushByRop(QPainter* painter, int rop);

private:
    StData::PolylineItem2d* m_polylineItem;

    QPainterPath m_startArrowPath;
    QPainterPath m_endArrowPath;

    QPen m_emptyPen;
    QBrush m_emptyBrush;
};

}

#endif // SPACE2DGRAPHICSITEM_H
