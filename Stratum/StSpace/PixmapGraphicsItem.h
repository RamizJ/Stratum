#ifndef PIXMAPGRAPHICSITEM_H
#define PIXMAPGRAPHICSITEM_H

#include "stspace_global.h"
#include "SpaceGraphicsItem.h"

#include <QDir>
#include <QGraphicsPixmapItem>

namespace StData {
class TextureTool;
class TextureItem2d;
}

namespace StSpace {

class STSPACESHARED_EXPORT PixmapGraphicsItem : public QObject, public QGraphicsPixmapItem, public SpaceGraphicsItem
{
    Q_OBJECT
public:
    PixmapGraphicsItem(StData::TextureItem2d* pixmapItem2d, QGraphicsItem* parent = 0);

    QGraphicsItem* graphicsItem() {return this;}

    StData::TextureItem2d* pixmapItem() const {return m_pixmapItem;}

    StData::TextureTool* texture() const;
    void setTexture(StData::TextureTool* pixmapInfo);

    void setSrcRect(const QRectF& rect);

    // QGraphicsItem interface
public:
    int type() const;
    QRectF boundingRect() const;
//    QPainterPath shape() const;

    // ISpaceGraphicsItem interface
public:
    void setSpaceItemSize(double w, double h);
    void rotateObject2d(const QPointF& base, const double& angle);

    void updateSize();

    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

private slots:
    void updatePixmap();

private:
    StData::TextureItem2d* m_pixmapItem;
};

}

#endif // PIXMAPGRAPHICSITEM_H
