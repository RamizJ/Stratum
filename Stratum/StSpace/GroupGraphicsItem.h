#ifndef GRAPHICSGROUPITEM_H
#define GRAPHICSGROUPITEM_H

#include <QDir>
#include <QGraphicsItem>

#include "stspace_global.h"
#include "SpaceGraphicsItem.h"

namespace StData {
class SpaceItem;
class GroupItem;
}

namespace StSpace {

class STSPACESHARED_EXPORT GroupGraphicsItem : public QGraphicsItem, public SpaceGraphicsItem
{
public:
    GroupGraphicsItem(StData::GroupItem* groupItem, QGraphicsScene* scene,
                      bool initializeItems = true, QGraphicsItem* parent = 0);
    ~GroupGraphicsItem();

    StData::GroupItem* groupItem() const {return m_groupItem;}
    QList<QGraphicsItem*> groupItems() const { return m_groupItems; }

    void addToGroup(QGraphicsItem* item);
    void removeFromGroup(StData::SpaceItem* spaceItem);
    void removeGraphicsItemFromGroup(QGraphicsItem* item);
    void removeGraphicsItemFromGroup(int itemHandle);
    void clearGroup();

    // QGraphicsItem interface
public:
    int type() const;
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    // ISpaceGraphicsItem interface
public:
    QGraphicsItem* graphicsItem() {return this;}

    void setSpaceItemOrg(double x, double y);
    void setSpaceItemSize(double w, double h);
    void setSpaceItemsVisible(bool visible);
    void rotateObject2d(const QPointF& base, const double& angle);

    void updateZOrder();
    void updatePosition();
    void updateSize();

    // QGraphicsItem interface
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value);
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

protected:
    void initializeGroupItems();

private:
    void translateChilds(const QPointF& newPos);
    void updateChildsPosition();
    void setPosintionForGroupItemOnly(const QPointF& newPos);

private:
    StData::GroupItem* m_groupItem;
    QList<QGraphicsItem*> m_groupItems;
    QGraphicsScene* m_scene;

    QPointF m_deltaPos;
    bool m_lockUpdateItemsPos;
};

}

#endif // GRAPHICSGROUPITEM_H
