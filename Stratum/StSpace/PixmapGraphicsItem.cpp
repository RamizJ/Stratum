#include "PixmapGraphicsItem.h"
#include "GraphicsItemType.h"

#include <Tool2d.h>
#include <Space.h>
#include <SpaceItem.h>
#include <QGraphicsSceneContextMenuEvent>
#include <Project.h>
#include <QDir>
#include <QtMath>

#include <QCursor>
#include <QDebug>

using namespace StData;

namespace StSpace {

PixmapGraphicsItem::PixmapGraphicsItem(TextureItem2d* pixmapItem, QGraphicsItem* parent) :
    QObject(),
    SpaceGraphicsItem(pixmapItem),
    QGraphicsPixmapItem(parent),
    m_pixmapItem(pixmapItem)
{
//    Space* space = m_pixmapItem->space();
//    PixmapInfo* pixmapInfo = m_pixmapItem->graphicsType() == SpaceItem::GraphicsType_MaskedPixmap ?
//                                 space->getMaskedPixmap(m_pixmapItem->pixmapHandle()) :
//                                 space->getPixmap(m_pixmapItem->pixmapHandle());

    updatePixmap();

    setPos(m_pixmapItem->origin());
    setRotation(m_pixmapItem->angle());

    setVisible(m_pixmapItem->isVisible());
    setZValue(m_pixmapItem->zOrder());

    setFlag(ItemClipsToShape);

    connect(m_pixmapItem->texture(), &TextureTool::changed, this, &PixmapGraphicsItem::updatePixmap);
}

TextureTool *PixmapGraphicsItem::texture() const
{
    return m_pixmapItem->texture();
}

void PixmapGraphicsItem::setTexture(TextureTool* texture)
{
    if(texture)
    {
        disconnect(m_pixmapItem->texture());

        m_pixmapItem->setTexture(texture);
        updatePixmap();

        connect(m_pixmapItem->texture(), &TextureTool::changed, this, &PixmapGraphicsItem::updatePixmap);
    }
}

void PixmapGraphicsItem::setSrcRect(const QRectF& rect)
{
    if(rect.isValid())
    {
        m_pixmapItem->setTextureOrigin(rect.topLeft());
        m_pixmapItem->setTextureSize(rect.size());

        updatePixmap();
    }
}

int PixmapGraphicsItem::type() const
{
    return UserType + static_cast<int>(GraphicsItemType::Pixmap);
}

QRectF PixmapGraphicsItem::boundingRect() const
{
    return QRectF(QPointF(), m_pixmapItem->size());
}

//QPainterPath PixmapGraphicsItem::shape() const
//{
//    QPainterPath path;
//    path.addRect(boundingRect());
//    return path;
//}

void PixmapGraphicsItem::setSpaceItemSize(double w, double h)
{
    QSizeF newSize(w, h);
    if(newSize != m_pixmapItem->size())
    {
        m_pixmapItem->setSize(newSize);

        //prepareGeometryChange();
        //setPixmap(pixmap().scaled(newSize.toSize()));
        updatePixmap();
    }
}

void PixmapGraphicsItem::rotateObject2d(const QPointF& base, const double& angle)
{
    QPointF origin = m_pixmapItem->origin();
    rotatePoint(&origin, base, angle);
    m_pixmapItem->setOrigin(origin);

    double a = m_pixmapItem->angle();
    a += qRadiansToDegrees(angle);
    m_pixmapItem->setAngle(a);

    setRotation(a);
    setPos(origin);
}

void PixmapGraphicsItem::updateSize()
{
    if(pixmap().size() != m_pixmapItem->size())
    {
        prepareGeometryChange();
        updatePixmap();
        //setPixmap(pixmap().scaled(m_pixmapItem->size().toSize()));
    }
}

void PixmapGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mousePressEvent(event);
    if(m_hyperJumpActivated)
        spaceItem()->invokeHyperJump();
}

void PixmapGraphicsItem::updatePixmap()
{
    TextureTool* texture = m_pixmapItem->texture();
    QPixmap pixmap = texture->pixmap();

    QRect offsetRect = m_pixmapItem->offsetRect().toRect();
    if(offsetRect.isValid())
        pixmap = pixmap.copy(offsetRect);
    pixmap = pixmap.scaled(m_pixmapItem->size().toSize());

    setPixmap(pixmap);
}

}
