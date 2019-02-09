#include "EditGroupDialog.h"
#include "EditLinkDialog.h"
#include "EditObjectDialog.h"
#include "EditPixmapDialog.h"
#include "EditPolylineDialog.h"
#include "EditRasterTextDialog.h"
#include "SpaceEditWidget.h"
#include "SpaceContextMenu.h"
#include "GroupGraphicsItem.h"

#include <Space.h>
#include <Object.h>
#include <SpaceScene.h>
#include <SpaceView.h>

#include <QVBoxLayout>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <ObjectGraphicsItem.h>
#include <PolylineGraphicsItem.h>
#include <PixmapGraphicsItem.h>
#include <RasterTextGraphicsItem.h>
#include <LinkGraphicsItem.h>
#include <SpaceItem.h>

#include <QDebug>

using namespace StData;
using namespace StSpace;

SpaceEditWidget::SpaceEditWidget(StData::Space* space, StData::Object* spaceOwnerObject, QWidget *parent) :
    QWidget(parent),
    m_space(space),
    m_spaceOwnerObject(spaceOwnerObject)
{
    m_spaceScene = new SpaceScene(m_space, this);
//    m_spaceScene->setSceneRect(-100000, -100000, 200000, 200000);

    m_spaceView = new SpaceView(this);
    m_spaceView->setDragMode(QGraphicsView::RubberBandDrag);
//    m_spaceView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform |
//                                QPainter::TextAntialiasing);
    m_spaceView->setScene(m_spaceScene);

    for(QGraphicsItem* item : m_spaceScene->items())
    {
        SpaceGraphicsItem* spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(item);
        if(!spaceGraphicsItem->spaceItem()->ownerItem())
            item->setFlag(QGraphicsItem::ItemIsSelectable);

        item->setFlags(item->flags() |
                       QGraphicsItem::ItemSendsGeometryChanges |
                       QGraphicsItem::ItemStacksBehindParent);

        if(!dynamic_cast<LinkGraphicsItem*>(item))
            item->setFlags(item->flags() | QGraphicsItem::ItemIsMovable);
    }

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(2);
    layout->addWidget(m_spaceView);
    setLayout(layout);

    m_spaceContextMenu = new SpaceContextMenu(m_spaceOwnerObject, this);

    connect(m_spaceScene, &SpaceScene::contextMenu, m_spaceContextMenu, &SpaceContextMenu::openContextMenu);
    connect(m_spaceScene, &SpaceScene::doubleClick, this, &SpaceEditWidget::spaceDoubleClick);
}

void SpaceEditWidget::spaceDoubleClick(QGraphicsItem* graphicsItem)
{
    if(auto objectItem = dynamic_cast<ObjectGraphicsItem*>(graphicsItem))
    {
        Object* object = m_spaceOwnerObject->childByHandle(objectItem->objectHandle());
        EditObjectDialog(object).exec();
    }
    else if(auto linkItem = dynamic_cast<LinkGraphicsItem*>(graphicsItem))
        EditLinkDialog(linkItem).exec();

    else if(auto polylineItem = dynamic_cast<PolylineGraphicsItem*>(graphicsItem))
        EditPolylineDialog(polylineItem).exec();

    else if(auto pixmapItem = dynamic_cast<PixmapGraphicsItem*>(graphicsItem))
        EditPixmapDialog(pixmapItem).exec();

    else if(auto groupItem = dynamic_cast<GroupGraphicsItem*>(graphicsItem))
        EditGroupDialog(groupItem).exec();

    else if(auto textItem = dynamic_cast<RasterTextGraphicsItem*>(graphicsItem))
        EditRasterTextDialog(textItem).exec();
}


