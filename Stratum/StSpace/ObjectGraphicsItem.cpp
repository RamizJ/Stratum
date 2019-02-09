#include "ObjectGraphicsItem.h"
#include "GroupGraphicsItem.h"
#include "GraphicsItemType.h"
#include "SpaceScene.h"

#include <SpaceItem.h>
#include <Space.h>
#include <Class.h>
#include <Object.h>
#include <ObjectInfo.h>
#include <Scheme.h>
#include <QStyleOptionGraphicsItem>
#include <QApplication>
#include <QPainter>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

using namespace StData;

namespace StSpace {

ObjectGraphicsItem::ObjectGraphicsItem(StData::ObjectInfo* objectInfo, StData::GroupItem* groupItem,
                                       SpaceScene* scene, QGraphicsItem* parent) :
    GroupGraphicsItem(groupItem, scene, false, parent),
    m_objectInfo(objectInfo)
{
    Space* imageSpace = nullptr;
    if(m_objectInfo && m_objectInfo->cls() && m_objectInfo->cls()->image())
        imageSpace = m_objectInfo->cls()->image()->getSpace();

    if(imageSpace)
    {
        int startZOrder = -1;
        if(groupItem->itemsCount() > 0)
            startZOrder = groupItem->itemAt(0)->zOrder();

        QList<SpaceItem*> imageItems = imageSpace->copyItems();

        Space* space = scene->space();
        space->pasteItems(imageSpace, imageItems/*, startZOrder*/);

        SpaceItem* imageItem = nullptr;
        if(imageItems.count() > 1)
        {
            GroupItem* imageItemsGroup = new GroupItem(space);
            imageItemsGroup->append(imageItems);

            space->addItem(imageItemsGroup);
            imageItem = imageItemsGroup;
        }
        else if(imageItems.count() == 1)
            imageItem = imageItems.first();

        if(SpaceItem2d* imageItem2d = static_cast<SpaceItem2d*>(imageItem))
        {
            imageItem2d->setOrigin(m_objectInfo->position());
            groupItem->append(imageItem);

            groupItem->itemAt(0)->setItemVisible(false);
            SpaceItem2d* spaceItem2d = static_cast<SpaceItem2d*>(groupItem->itemAt(0));

            spaceItem2d->setItemVisible(false);
            spaceItem2d->setSize(QSizeF());
        }
    }
    initializeGroupItems();
    setVisible(groupItem->isVisible());
    updateZOrder();
}

ObjectGraphicsItem::~ObjectGraphicsItem()
{}

int ObjectGraphicsItem::objectHandle()
{
    return groupItem()->handle();
}

int ObjectGraphicsItem::type() const
{
    return QGraphicsItem::UserType + static_cast<int>(GraphicsItemType::ClassObject);
}

void ObjectGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* /*widget*/)
{
    if(option->state & QStyle::State_Selected)
    {
        QColor penColor = QApplication::style()->standardPalette().color(QPalette::Highlight);
        QColor brushColor(penColor);
        brushColor.setAlpha(50);

        painter->save();
        {
            painter->setPen(QPen(penColor, 1, Qt::SolidLine));
            painter->setBrush(brushColor);
            painter->drawRect(boundingRect());
        }
        painter->restore();
    }
}

//void ObjectGraphicsItem::addGroupItems(GroupGraphicsItem* groupGraphicsItem)
//{
//    for(QGraphicsItem* graphicsItem : groupGraphicsItem->groupItems())
//    {
//        if(auto childGroupGraphicsItem = dynamic_cast<GroupGraphicsItem*>(graphicsItem))
//        {
//            addToGroup(childGroupGraphicsItem);
//            addGroupItems(childGroupGraphicsItem);
//        }
//        else
//            addToGroup(graphicsItem);
//    }
//}

}
