#include "GroupGraphicsItem.h"
#include "PixmapGraphicsItem.h"
#include "RasterTextGraphicsItem.h"
#include "PolylineGraphicsItem.h"
#include "GraphicsItemType.h"

#include <Class.h>
#include <Space.h>
#include <SpaceItem.h>
#include <ObjectInfo.h>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsScene>

#include <QDebug>

using namespace StData;

namespace StSpace {

GroupGraphicsItem::GroupGraphicsItem(StData::GroupItem* groupItem, QGraphicsScene* scene,
                                     bool initializeItems, QGraphicsItem* parent) :
    SpaceGraphicsItem(groupItem),
    QGraphicsItem(parent),
    m_groupItem(groupItem),
    m_scene(scene)
{
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setPosintionForGroupItemOnly(m_groupItem->origin());
    setVisible(m_groupItem->isVisible());

    if(initializeItems)
    {
        initializeGroupItems();
        updateZOrder();
    }
}

GroupGraphicsItem::~GroupGraphicsItem()
{}

int GroupGraphicsItem::type() const
{
    return UserType + static_cast<int>(GraphicsItemType::Group);
}

QRectF GroupGraphicsItem::boundingRect() const
{
//    QRectF groupRect = m_groupItem->calculateGroupRect();
//    return QRectF(QPointF(), groupRect.size());
    return QRectF(QPointF(), QSizeF());
}

//TODO объявить в StGui редактируемого наследника и убрать этот код туда
void GroupGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* /*widget*/)
{
    if(option->state & QStyle::State_Selected)
    {
        painter->save();
        painter->setPen(QPen(QBrush(Qt::black), 1, Qt::DashLine));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(boundingRect());
        painter->restore();
    }
}

void GroupGraphicsItem::setSpaceItemOrg(double x, double y)
{
    QPointF org(x, y);
    if(org != pos())
        setPos(org);
}

void GroupGraphicsItem::setSpaceItemSize(double w, double h)
{
    QSizeF newSize(w, h);
    if(newSize != m_groupItem->size())
    {
        prepareGeometryChange();
        m_groupItem->setSize(QSizeF(w, h));

        for(QGraphicsItem* groupItem : m_groupItems)
            if(SpaceGraphicsItem* childSpaceItem = dynamic_cast<SpaceGraphicsItem*>(groupItem))
                childSpaceItem->updateSize();
    }
}

void GroupGraphicsItem::setSpaceItemsVisible(bool visible)
{
    setVisible(visible);

    for(QGraphicsItem* graphicsItem : m_groupItems)
        if(auto spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(graphicsItem))
            spaceGraphicsItem->setSpaceItemsVisible(visible);
}

void GroupGraphicsItem::rotateObject2d(const QPointF& base, const double& angle)
{
    prepareGeometryChange();
    for(QGraphicsItem* graphicsItem : m_groupItems)
        if(auto spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(graphicsItem))
            spaceGraphicsItem->rotateObject2d(base, angle);
}

void GroupGraphicsItem::updateZOrder()
{
    int maxZOrder = 0;
    for(QGraphicsItem* graphicsItem : m_groupItems)
        if(graphicsItem->zValue() > maxZOrder)
            maxZOrder = graphicsItem->zValue();

    setZValue(maxZOrder + 1);
}

void GroupGraphicsItem::updatePosition()
{
    if(pos() != m_groupItem->origin())
    {
        setPos(m_groupItem->origin());
        updateChildsPosition();
    }
}

void GroupGraphicsItem::updateSize()
{
    if(boundingRect().size() != m_groupItem->size())
    {
        for(QGraphicsItem* childItem : groupItems())
            if(SpaceGraphicsItem* childSpaceItem = dynamic_cast<SpaceGraphicsItem*>(childItem))
                childSpaceItem->updateSize();
    }
}

QVariant GroupGraphicsItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if(change == ItemPositionChange && !m_lockUpdateItemsPos)
        translateChilds(value.toPointF());

    return QGraphicsItem::itemChange(change, value);
}

void GroupGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mousePressEvent(event);
    if(m_hyperJumpActivated)
        spaceItem()->invokeHyperJump();
}

void GroupGraphicsItem::initializeGroupItems()
{
    for(SpaceItem* spaceItem : m_groupItem->items())
    {
        QGraphicsItem* item = nullptr;

        if(PolylineItem2d* polyline = dynamic_cast<PolylineItem2d*>(spaceItem))
            item = new PolylineGraphicsItem(polyline);

        else if(GroupItem* childGroup = dynamic_cast<GroupItem*>(spaceItem))
            item = new GroupGraphicsItem(childGroup, m_scene);

        else if(TextItem2d* textItem = dynamic_cast<TextItem2d*>(spaceItem))
            item = new RasterTextGraphicsItem(textItem);

        else if(TextureItem2d* pixmapItem = dynamic_cast<TextureItem2d*>(spaceItem))
            item = new PixmapGraphicsItem(pixmapItem);

        if(item)
        {
            m_groupItems << item;
            m_scene->addItem(item);
        }
    }
}

void GroupGraphicsItem::translateChilds(const QPointF& newPos)
{
    m_groupItem->setOrigin(newPos);
    updateChildsPosition();
}

void GroupGraphicsItem::updateChildsPosition()
{
    for(QGraphicsItem* childItem : groupItems())
        if(SpaceGraphicsItem* childSpaceItem = dynamic_cast<SpaceGraphicsItem*>(childItem))
            childSpaceItem->updatePosition();
}

void GroupGraphicsItem::setPosintionForGroupItemOnly(const QPointF& newPos)
{
    m_lockUpdateItemsPos = true;
    setPos(newPos);
    m_lockUpdateItemsPos = false;
}

void GroupGraphicsItem::addToGroup(QGraphicsItem* item)
{
    if(item)
    {
//        prepareGeometryChange();
        m_groupItems << item;
    }
}

void GroupGraphicsItem::removeFromGroup(SpaceItem* spaceItem)
{
    if(spaceItem)
    {
        if(m_groupItem->contains(spaceItem))
        {
            m_groupItem->remove(spaceItem);

            prepareGeometryChange();
            if(QGraphicsItem* graphicItem = m_groupItems.value(spaceItem->handle()))
                m_groupItems.removeOne(graphicItem);
        }
    }
}

void GroupGraphicsItem::removeGraphicsItemFromGroup(QGraphicsItem* item)
{
    if(item)
    {
        prepareGeometryChange();
        m_groupItems.removeOne(item);
    }
}

void GroupGraphicsItem::removeGraphicsItemFromGroup(int itemHandle)
{
    prepareGeometryChange();

    for(QGraphicsItem* graphicItem : m_groupItems)
        if(auto spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(graphicItem))
            if(spaceGraphicsItem->handle() == itemHandle)
            {
                m_groupItems.removeOne(graphicItem);
                break;
            }
}

void GroupGraphicsItem::clearGroup()
{
    m_groupItem->clear();
    m_groupItems.clear();
}

}
