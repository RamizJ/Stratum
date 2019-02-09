#include "SpaceGraphicsItem.h"

#include <HyperBase.h>
#include <QCursor>
#include <SpaceItem.h>
#include <math.h>

using namespace StData;

namespace StSpace {

SpaceGraphicsItem::SpaceGraphicsItem(StData::SpaceItem2d* spaceItem) :
    m_spaceItem(spaceItem),
    m_hyperJumpActivated(false)
{}

int SpaceGraphicsItem::handle()
{
    return m_spaceItem->handle();
}

QPointF SpaceGraphicsItem::getSpaceItemOrg()
{
    return m_spaceItem->origin();
}

void SpaceGraphicsItem::setSpaceItemOrg(double x, double y)
{
    QPointF org(x, y);
    if(org != m_spaceItem->origin())
    {
        m_spaceItem->setOrigin(org);
        graphicsItem()->setPos(x, y);
    }
}

QSizeF SpaceGraphicsItem::getSpaceItemSize()
{
    return m_spaceItem->size();
}

void SpaceGraphicsItem::setSpaceItemsVisible(bool visible)
{
    if(m_spaceItem->isItemVisible() != visible)
    {
        m_spaceItem->setItemVisible(visible);
        graphicsItem()->setVisible(visible);
    }
}

double SpaceGraphicsItem::getSpaceItemOpacity()
{
    return m_spaceItem->opacity();
}

void SpaceGraphicsItem::setSpaceItemOpacity(double opacity)
{
    if(opacity != m_spaceItem->opacity())
    {
        m_spaceItem->setOpacity(opacity);
        graphicsItem()->setOpacity(opacity);
    }
}

int SpaceGraphicsItem::getZOrder()
{
    return m_spaceItem->zOrder();
}

void SpaceGraphicsItem::updateVisibility()
{
    if(graphicsItem()->isVisible() != m_spaceItem->isVisible())
        graphicsItem()->setVisible(m_spaceItem->isVisible());
}

void SpaceGraphicsItem::updateZOrder()
{
    if(graphicsItem()->zValue() != m_spaceItem->zOrder())
    {
        graphicsItem()->setZValue(m_spaceItem->zOrder());

        auto ownerItem = dynamic_cast<SpaceGraphicsItem*>(graphicsItem()->parentItem());
        while(ownerItem)
        {
            ownerItem->updateZOrder();
            ownerItem = dynamic_cast<SpaceGraphicsItem*>(ownerItem->graphicsItem()->parentItem());
        }
    }
}

void SpaceGraphicsItem::updatePosition()
{
    if(graphicsItem()->pos() != m_spaceItem->origin())
        graphicsItem()->setPos(m_spaceItem->origin());
}

void SpaceGraphicsItem::activateHyperJump()
{
    if(SpaceItemHyperKeyPtr key = m_spaceItem->actualHyperKey())
    {
        if(key == m_spaceItem->hyperKey() && !m_spaceItem->isSelectable())
        {
            return;
        }

        m_hyperJumpActivated = true;
        graphicsItem()->setCursor(Qt::PointingHandCursor);
    }
}

void SpaceGraphicsItem::setHyperKey(const SpaceItemHyperKeyPtr& key)
{
    m_spaceItem->setHyperKey(key);
}

void SpaceGraphicsItem::stdHyperJump()
{

}

void SpaceGraphicsItem::rotatePoint(QPointF* point, const QPointF& base, double angle)
{
    *point -= base;
    double sinA = sin(angle);
    double cosA = cos(angle);
    double x = point->x() * cosA - point->y() * sinA;
    double y = point->x() * sinA + point->y() * cosA;
    point->setX(x + base.x());
    point->setY(y + base.y());
}

}
