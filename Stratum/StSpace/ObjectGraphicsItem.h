#ifndef CLASSOBJECTGRAPHICSITEM_H
#define CLASSOBJECTGRAPHICSITEM_H

#include "stspace_global.h"
#include "GroupGraphicsItem.h"

#include <QGraphicsItemGroup>

namespace StData {
class GroupItem;
class ObjectInfo;
}

namespace StSpace {

class SpaceScene;

class STSPACESHARED_EXPORT ObjectGraphicsItem : public GroupGraphicsItem
{
public:
    ObjectGraphicsItem(StData::ObjectInfo* objectInfo, StData::GroupItem* groupItem,
                       SpaceScene* scene, QGraphicsItem *parent = 0);
    ~ObjectGraphicsItem();

    QGraphicsItem* graphicsItem() {return this;}

    int objectHandle();

    // QGraphicsItem interface
public:
    int type() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

private:
    StData::ObjectInfo* m_objectInfo;
};

}

#endif // CLASSOBJECTGRAPHICSITEM_H
