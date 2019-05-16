#include "SpaceScene.h"

#include <QGraphicsSceneContextMenuEvent>
#include <Space.h>
#include <SpaceItem.h>
#include <Class.h>

#include <Object.h>
#include <ObjectInfo.h>
#include <QDebug>
#include <QMenu>
#include <QtMath>
#include <QDebug>
#include <Project.h>
#include <Tool2d.h>

#include "ObjectGraphicsItem.h"
#include "GroupGraphicsItem.h"
#include "LinkGraphicsItem.h"
#include "PixmapGraphicsItem.h"
#include "RasterTextGraphicsItem.h"
#include "PolylineGraphicsItem.h"
#include "ControlGraphicsItem.h"

#include <Log.h>
#include <QDebug>

using namespace StData;

namespace StSpace {

SpaceScene::SpaceScene(Space* space, QObject* parent) :
    QGraphicsScene(parent),
    m_space(space),
    m_bkBrush(0)
{
    setBkBrush(m_space->getBkBrush());
    initializeGraphicsItems();
}

SpaceScene::~SpaceScene()
{}

void SpaceScene::setSpace(Space* space)
{
    if(m_space == space)
        return;

    m_space = space;
    m_graphicsItemForHandle.clear();
    clear();

    initializeGraphicsItems();
}

SpaceGraphicsItem* SpaceScene::getSpaceGraphicsItem(int itemHandle) const
{
    if(auto result = m_graphicsItemForHandle.value(itemHandle, nullptr))
        return dynamic_cast<SpaceGraphicsItem*>(result);

    return nullptr;
}

double SpaceScene::getObjectOrg2dX(int objectHandle)
{
    SpaceItem* spaceItem = m_space->getSpaceItem(objectHandle);
    if(SpaceItem2d* item2d = dynamic_cast<SpaceItem2d*>(spaceItem))
        return item2d->origin().x();

    return 0.0;
}

double SpaceScene::getObjectOrg2dY(int objectHandle)
{
    SpaceItem* spaceItem = m_space->getSpaceItem(objectHandle);
    if(SpaceItem2d* item2d = dynamic_cast<SpaceItem2d*>(spaceItem))
        return item2d->origin().y();

    return 0.0;
}

bool SpaceScene::setObjectOrg2d(int objectHandle, double x, double y)
{
    QGraphicsItem* item = m_graphicsItemForHandle.value(objectHandle);
    if(SpaceGraphicsItem* spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(item))
    {
        spaceGraphicsItem->setSpaceItemOrg(x, y);
        return true;
    }
    return false;
}

double SpaceScene::getObjectWidth(int objectHandle)
{
    QGraphicsItem* item = m_graphicsItemForHandle.value(objectHandle);
    if(SpaceGraphicsItem* spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(item))
        return spaceGraphicsItem->getSpaceItemSize().width();

    return 0.0;
}

double SpaceScene::getObjectHeight(int objectHandle)
{
    QGraphicsItem* item = m_graphicsItemForHandle.value(objectHandle);
    if(SpaceGraphicsItem* spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(item))
        return spaceGraphicsItem->getSpaceItemSize().height();

    return 0.0;
}

bool SpaceScene::setObjectSize(int objectHandle, double w, double h)
{
    QGraphicsItem* item = m_graphicsItemForHandle.value(objectHandle);
    if(SpaceGraphicsItem* spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(item))
    {
        spaceGraphicsItem->setSpaceItemSize(w, h);
        return true;
    }
    return false;
}

double SpaceScene::getActualHeight(int objectHandle)
{
    double width, height;
    if(getActualSize(objectHandle, &width, &height))
        return height;

    return 0.0;
}

double SpaceScene::getActualWidth(int objectHandle)
{
    double width, height;
    if(getActualSize(objectHandle, &width, &height))
        return width;

    return 0.0;
}

bool SpaceScene::getActualSize(int objectHandle, double* width, double* height)
{
    if(QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(objectHandle))
    {
        if(auto pixmapGraphicsItem = dynamic_cast<PixmapGraphicsItem*>(graphicsItem))
        {
            TextureItem2d* textureItem = pixmapGraphicsItem->pixmapItem();
            if(TextureTool* texture = textureItem->texture())
            {
                QPixmap pixmap = texture->pixmap();
                *width = pixmap.width();
                *height = pixmap.height();
            }
        }
        else if(auto textGraphicsItem = dynamic_cast<RasterTextGraphicsItem*>(graphicsItem))
        {
            //QTextDocumentPtr doc = textGraphicsItem->documentForSize();
            //doc->adjustSize();
            QSizeF docSize = textGraphicsItem->renderSize();
            *width = docSize.width();
            *height = docSize.height();
        }
        else if(auto spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(graphicsItem))
        {
            QSizeF size = spaceGraphicsItem->getSpaceItemSize();
            *width = size.width();
            *height = size.height();
        }
        return true;
    }
    return false;
}

double SpaceScene::getObjectAngle2d(int objectHandle)
{
    if(QGraphicsItem* item = m_graphicsItemForHandle.value(objectHandle))
    {
        if(auto rasterTextGraphicsItem = dynamic_cast<RasterTextGraphicsItem*>(item))
        {
            double angle = rasterTextGraphicsItem->textItem()->textAngle() / 10;
            return qDegreesToRadians(angle);
        }
    }
    return 0.0;
}

bool SpaceScene::rotateObject2d(int objectHandle, double x, double y, double angle)
{
    if(QGraphicsItem* item = m_graphicsItemForHandle.value(objectHandle))
    {
        if(auto spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(item))
        {
            spaceGraphicsItem->rotateObject2d(QPointF(x, y), angle);
            return true;
        }
    }
    return false;
}

double SpaceScene::getObjectAlpha2d(int objectHandle)
{
    QGraphicsItem* item = m_graphicsItemForHandle.value(objectHandle);
    if(SpaceGraphicsItem* spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(item))
        return spaceGraphicsItem->getSpaceItemOpacity();
    return 0.0;
}

bool SpaceScene::setObjectAlpha2d(int objectHandle, double opacity)
{
    //    opacity = qMax(0, qMin(opacity, 1));
    QGraphicsItem* item = m_graphicsItemForHandle.value(objectHandle);
    if(SpaceGraphicsItem* spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(item))
    {
        if(spaceGraphicsItem->getSpaceItemOpacity() != opacity)
            spaceGraphicsItem->setSpaceItemOpacity(opacity);

        return true;
    }
    return false;
}

bool SpaceScene::setShowObject2d(int objectHandle, bool isVisible)
{
    QGraphicsItem* item = m_graphicsItemForHandle.value(objectHandle);
    if(SpaceGraphicsItem* spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(item))
    {
        spaceGraphicsItem->setSpaceItemsVisible(isVisible);
        return true;
    }
    return false;
}

bool SpaceScene::isObjectsIntersected(int object1Handle, int object2Handle)
{
//    QGraphicsItem* item1 = m_graphicsItemForHandle.value(object1Handle);
//    QGraphicsItem* item2 = m_graphicsItemForHandle.value(object2Handle);

//    if(item1 && item2)
//    {
//        return item1->collidesWithItem(item2, Qt::IntersectsItemBoundingRect);
//    }

    auto spaceItem1 = dynamic_cast<SpaceItem2d*>(m_space->getSpaceItem(object1Handle));
    auto spaceItem2 = dynamic_cast<SpaceItem2d*>(m_space->getSpaceItem(object2Handle));

    if(spaceItem1 && spaceItem2)
    {
        QRectF rect1 = spaceItem1->rect();
        QRectF rect2 = spaceItem2->rect();
        return rect1.intersects(rect2);
    }

    return false;
}

int SpaceScene::getObjectFromPoint2d(double x, double y)
{
    SpaceItem* item = m_space->getItemFromPoint2d(x, y, [this, &x, &y](int handle)
    {
        QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(handle);
        if(auto pixmapGraphicsItem = dynamic_cast<QGraphicsPixmapItem*>(graphicsItem))
        {
            return pixmapGraphicsItem->contains(graphicsItem->mapFromScene(x, y));
        }
        return true;
    });

    if(item)
    {
        while(item->ownerItem())
            item = item->ownerItem();

        return item->handle();
    }
    return 0;
}

bool SpaceScene::objectToBottom(int objectHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(objectHandle);
    if(SpaceGraphicsItem* spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(graphicsItem))
    {
        if(m_space->objectToBottom(objectHandle))
        {
            updateItemsZOrder();
//            spaceGraphicsItem->updateZOrder();
            return true;
        }
    }
    return false;
}

bool SpaceScene::objectToTop(int objectHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(objectHandle);
    if(auto spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(graphicsItem))
    {
        if(m_space->objectToTop(objectHandle))
        {
            updateItemsZOrder();
            return true;
        }
    }
    return false;
}

bool SpaceScene::setZOrder(int objectHandle, int zOrder)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(objectHandle);
    if(SpaceGraphicsItem* spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(graphicsItem))
    {
        if(m_space->setObjectZOrder(objectHandle, zOrder))
        {
            updateItemsZOrder();
            return true;
        }
    }
    return false;
}

bool SpaceScene::swapZOrder(int objectHandle1, int objectHandle2)
{
    QGraphicsItem* item1 = m_graphicsItemForHandle.value(objectHandle1);
    QGraphicsItem* item2 = m_graphicsItemForHandle.value(objectHandle2);
    SpaceGraphicsItem* spaceGraphicItem1 = dynamic_cast<SpaceGraphicsItem*>(item1);
    SpaceGraphicsItem* spaceGraphicItem2 = dynamic_cast<SpaceGraphicsItem*>(item2);

    if(item1 && item2)
    {
        if(m_space->swapObjectsZOrder(objectHandle1, objectHandle2))
        {
            spaceGraphicItem1->updateZOrder();
            spaceGraphicItem2->updateZOrder();
            return true;
        }
    }
    return false;
}

int SpaceScene::createPolyline(int penHandle, int brushHandle, const QPolygonF& polyline)
{
    PolylineItem2d* polylineItem = m_space->createPolyline(penHandle, brushHandle, polyline);
    if(polylineItem)
    {
        PolylineGraphicsItem* polylineGraphicsItem = new PolylineGraphicsItem(polylineItem);
        m_graphicsItemForHandle[polylineItem->handle()] = polylineGraphicsItem;
        addItem(polylineGraphicsItem);
        return polylineItem->handle();
    }
    return 0;
}

int SpaceScene::getPolylinePointsCount(int polylineHandle)
{
    SpaceItem* spaceItem = m_space->getSpaceItem(polylineHandle);
    if(PolylineItem2d* polylineItem = dynamic_cast<PolylineItem2d*>(spaceItem))
        return polylineItem->polyline().count();

    return 0;
}

bool SpaceScene::addPolylinePoint(int polylineHandle, int index, double x, double y)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(polylineHandle);
    if(PolylineGraphicsItem* polylineGraphicsItem = dynamic_cast<PolylineGraphicsItem*>(graphicsItem))
        return polylineGraphicsItem->insertPoint(index, QPointF(x, y));

    return false;
}

bool SpaceScene::delPolylinePoint(int polylineHandle, int index)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(polylineHandle);
    if(PolylineGraphicsItem* polylineGraphicsItem = dynamic_cast<PolylineGraphicsItem*>(graphicsItem))
        return polylineGraphicsItem->removePoint(index);

    return false;
}

int SpaceScene::getPolylinePen(int polylineHandle)
{
    SpaceItem* spaceItem = m_space->getSpaceItem(polylineHandle);
    if(PolylineItem2d* polylineItem = dynamic_cast<PolylineItem2d*>(spaceItem))
        if(polylineItem->penTool())
            return polylineItem->penTool()->handle();

    return 0;
}

bool SpaceScene::setPolylinePen(int polylineHandle, int penHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(polylineHandle);
    if(auto polylineGraphicsItem = dynamic_cast<PolylineGraphicsItem*>(graphicsItem))
    {
        polylineGraphicsItem->setPenTool(m_space->getPen(penHandle));
        return true;
    }

    return false;
}

int SpaceScene::getPolylineBrush(int polylineHandle)
{
    SpaceItem* spaceItem = m_space->getSpaceItem(polylineHandle);
    if(PolylineItem2d* polylineItem = dynamic_cast<PolylineItem2d*>(spaceItem))
        if(BrushTool* brush = polylineItem->brushTool())
            return brush->handle();

    return 0;
}

bool SpaceScene::setPolylineBrush(int polylineHandle, int brushHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(polylineHandle);
    if(auto polylineGraphicsItem = dynamic_cast<PolylineGraphicsItem*>(graphicsItem))
    {
        polylineGraphicsItem->setBrushTool(m_space->getBrush(brushHandle));
        return true;
    }
    return false;
}

double SpaceScene::getPolylinePointX(int polylineHandle, int index)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(polylineHandle);
    if(auto polylineGraphicsItem = dynamic_cast<PolylineGraphicsItem*>(graphicsItem))
        return polylineGraphicsItem->getPoint(index).x();

    return 0.0;
}

double SpaceScene::getPolylinePointY(int polylineHandle, int index)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(polylineHandle);
    if(auto polylineGraphicsItem = dynamic_cast<PolylineGraphicsItem*>(graphicsItem))
        return polylineGraphicsItem->getPoint(index).y();

    return 0.0;
}

bool SpaceScene::setPolylinePoint(int polylineHandle, int index, double x, double y)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(polylineHandle);
    if(auto polylineGraphicsItem = dynamic_cast<PolylineGraphicsItem*>(graphicsItem))
        return polylineGraphicsItem->setPoint(index, QPointF(x, y));

    return false;
}

double SpaceScene::getPolylineFillRule(int polylineHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(polylineHandle);
    if(auto polylineGraphicsItem = dynamic_cast<PolylineGraphicsItem*>(graphicsItem))
        return polylineGraphicsItem->getPolylineFillRule();

    return false;
}

bool SpaceScene::setPolylineFillRule(int polylineHandle, int fillRule)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(polylineHandle);
    if(auto polylineGraphicsItem = dynamic_cast<PolylineGraphicsItem*>(graphicsItem))
    {
        polylineGraphicsItem->setPolylineFillRule(fillRule);
        return true;
    }

    return false;
}

bool SpaceScene::setLineArrows(int polylineHandle, double aa, double al, double as, double ba, double bl, double bs)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(polylineHandle);
    if(auto polylineGraphicsItem = dynamic_cast<PolylineGraphicsItem*>(graphicsItem))
    {
        polylineGraphicsItem->setLineArrows(aa, al, as, ba, bl, bs);
        return true;
    }

    return false;
}

int SpaceScene::createRasterText(int textHandle, double x, double y, double angle)
{
    if(TextItem2d* textItem = m_space->createRasterText(textHandle, x, y, angle))
    {
        RasterTextGraphicsItem* rasterTextItem = new RasterTextGraphicsItem(textItem);
        m_graphicsItemForHandle[textItem->handle()] = rasterTextItem;
        addItem(rasterTextItem);

        textItem->setSize(rasterTextItem->document()->size());
        return textItem->handle();
    }
    return 0;
}

int SpaceScene::createPixmap(int dibHandle, double x, double y)
{
    if(TextureItem2d* pixmapItem = m_space->createPixmap(dibHandle, x, y))
    {
        PixmapGraphicsItem* pixmapGraphicsItem = new PixmapGraphicsItem(pixmapItem);
        m_graphicsItemForHandle[pixmapItem->handle()] = pixmapGraphicsItem;
        addItem(pixmapGraphicsItem);
        return pixmapItem->handle();
    }
    return 0;
}

int SpaceScene::createMaskedPixmap(int doubleDibHandle, double x, double y)
{
    if(MaskedTextureItem2d* pixmapItem = m_space->createMaskedPixmap(doubleDibHandle, x, y))
    {
        PixmapGraphicsItem* pixmapGraphicsItem = new PixmapGraphicsItem(pixmapItem);
        m_graphicsItemForHandle[pixmapItem->handle()] = pixmapGraphicsItem;
        addItem(pixmapGraphicsItem);
        return pixmapItem->handle();
    }
    return 0;
}

int SpaceScene::getDibObject(int pixmapItemHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(pixmapItemHandle);
    if(auto pixmapGraphicsItem = dynamic_cast<PixmapGraphicsItem*>(graphicsItem))
    {
        if(TextureTool* textureTool = pixmapGraphicsItem->texture())
            return textureTool->handle();
    }

    return 0;
}

bool SpaceScene::setDibObject(int pixmapItemHandle, int dibHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(pixmapItemHandle);
    if(PixmapGraphicsItem* pixmapGraphicsItem = dynamic_cast<PixmapGraphicsItem*>(graphicsItem))
    {
        if(TextureTool* textureTool = m_space->getTexture(dibHandle))
        {
            pixmapGraphicsItem->setTexture(textureTool);
            return true;
        }
    }
    return false;
}

bool SpaceScene::setDoubleDibObject(int pixmapItemHandle, int doubleDibHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(pixmapItemHandle);
    if(PixmapGraphicsItem* pixmapGraphicsItem = dynamic_cast<PixmapGraphicsItem*>(graphicsItem))
    {
        if(TextureTool* pixmapInfo = m_space->getMaskedTexture(doubleDibHandle))
        {
            pixmapGraphicsItem->setTexture(pixmapInfo);
            return true;
        }
    }
    return false;
}

bool SpaceScene::setPixmapSrcRect(int pixmapItemHandle, double x, double y, double width, double height)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(pixmapItemHandle);
    if(PixmapGraphicsItem* pixmapGraphicsItem = dynamic_cast<PixmapGraphicsItem*>(graphicsItem))
    {
        QRectF rect(x, y, width, height);
        if(rect.isValid())
        {
            pixmapGraphicsItem->setSrcRect(rect);
            return true;
        }
    }
    return false;
}

int SpaceScene::createGroup(const QVector<int>& objectHandles)
{
    if(GroupItem* groupItem = m_space->createGroup(objectHandles))
    {
        auto groupGraphicsItem = new GroupGraphicsItem(groupItem, this, false);
        addItem(groupGraphicsItem);

        for(SpaceItem* item : groupItem->items())
        {
            if(QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(item->handle()))
                groupGraphicsItem->addToGroup(graphicsItem);
        }
        m_graphicsItemForHandle[groupItem->handle()] = groupGraphicsItem;
        return groupItem->handle();
    }
    return 0;
}

bool SpaceScene::deleteGroup(int groupHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(groupHandle);
    SpaceItem* item = m_space->getSpaceItem(groupHandle);
    if(GroupGraphicsItem* groupGraphicsItem = dynamic_cast<GroupGraphicsItem*>(graphicsItem))
    {
        if(groupGraphicsItem->spaceItem()->ownerItem())
        {
            int parentHandle = groupGraphicsItem->spaceItem()->ownerItem()->handle();
            QGraphicsItem* parentGraphicsItem = m_graphicsItemForHandle.value(parentHandle);
            GroupGraphicsItem* groupGraphicsItem = static_cast<GroupGraphicsItem*>(parentGraphicsItem);
            groupGraphicsItem->removeGraphicsItemFromGroup(groupHandle);
        }
        removeItem(groupGraphicsItem);
        delete groupGraphicsItem;

        updateItemsHandles();
        return m_space->deleteGroup(groupHandle);
    }
    return false;
}

bool SpaceScene::addItemToGroup(int groupHandle, int itemHandle)
{
    if(m_space->addItemToGroup(groupHandle, itemHandle))
    {
        QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(groupHandle);
        auto groupGraphicsItem = dynamic_cast<GroupGraphicsItem*>(graphicsItem);
        graphicsItem = m_graphicsItemForHandle.value(itemHandle);

        if(groupGraphicsItem && graphicsItem)
        {
            groupGraphicsItem->addToGroup(graphicsItem);
            return true;
        }
    }
    return false;
}

bool SpaceScene::deleteGroupItem(int groupHandle, int itemHandle)
{
    if(m_space->deleteGroupItem(groupHandle, itemHandle))
    {
        QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(groupHandle);
        GroupGraphicsItem* groupGraphicsItem = dynamic_cast<GroupGraphicsItem*>(graphicsItem);
        graphicsItem = m_graphicsItemForHandle.value(itemHandle);

        if(groupGraphicsItem && graphicsItem)
        {
            groupGraphicsItem->removeGraphicsItemFromGroup(graphicsItem);
            return true;
        }
    }
    return false;
}

bool SpaceScene::setGroupItem2d(int groupHandle, int replacingItemIndex, int newItemHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(groupHandle);
    GroupGraphicsItem* groupGraphicsItem = dynamic_cast<GroupGraphicsItem*>(graphicsItem);
    graphicsItem = m_graphicsItemForHandle.value(newItemHandle);
    SpaceGraphicsItem* newGroupGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(graphicsItem);

    if(groupGraphicsItem && newGroupGraphicsItem)
    {
        GroupItem* groupItem = groupGraphicsItem->groupItem();
        SpaceItem* newGroupItem = newGroupGraphicsItem->spaceItem();

        if(groupItem->containsHandle(newItemHandle) || groupItem->isSelfOrAncestor(newGroupItem))
            return true;

        SpaceItem* replacingItem = nullptr;
        if(replacingItemIndex >= 0 && replacingItemIndex < groupItem->itemsCount())
            replacingItem = groupItem->itemAt(replacingItemIndex);

        if(replacingItem)
        {
            for(QGraphicsItem* childItem : groupGraphicsItem->groupItems())
                if(SpaceGraphicsItem* spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(childItem))
                    if(spaceGraphicsItem->spaceItem() == replacingItem)
                    {
                        groupGraphicsItem->removeGraphicsItemFromGroup(childItem);
                        break;
                    }

            groupItem->remove(replacingItem);
            groupItem->insertAt(replacingItemIndex, newGroupGraphicsItem->spaceItem());
            groupGraphicsItem->addToGroup(graphicsItem);
            return true;
        }
    }
    return false;
}

bool SpaceScene::setGroupItems2d(int groupHandle, const QVector<int>& objectHandles)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(groupHandle);
    GroupGraphicsItem* groupGraphicsItem = dynamic_cast<GroupGraphicsItem*>(graphicsItem);

    if(groupGraphicsItem)
    {
        GroupItem* groupItem = groupGraphicsItem->groupItem();
        for(int objectHandle : objectHandles)
        {
            SpaceItem* spaceItem = m_space->getSpaceItem(objectHandle);
            if(!spaceItem || groupItem->isSelfOrAncestor(spaceItem))
                return false;
        }

        groupGraphicsItem->clearGroup();
        groupGraphicsItem->groupItem()->addItemHandles(objectHandles.toList());
        for(int objectHandle : objectHandles)
            groupGraphicsItem->addToGroup(m_graphicsItemForHandle.value(objectHandle));

        return true;
    }
    return false;
}

int SpaceScene::createControl(const QString& className, const QString& text, int style, int x, int y, int width, int height, int id)
{
    if(ControlItem2d* controlItem = m_space->createControl(className, text, style, x, y, width, height, id))
    {
        auto controlGraphicsItem = new ControlGraphicsItem(controlItem);
        m_graphicsItemForHandle[controlItem->handle()] = controlGraphicsItem;
        addItem(controlGraphicsItem);
        return controlItem->handle();
    }
    return 0;
}

bool SpaceScene::isControlEnabled(int controlhandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlhandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
        return controlGraphicsItem->isControlEnabled();

    return false;
}

bool SpaceScene::setControlEnabled(int controlHandle, bool enabled)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
    {
        controlGraphicsItem->setControlEnabled(enabled);
        return true;
    }

    return false;
}

bool SpaceScene::setControlFocus(int controlHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
    {
        controlGraphicsItem->setControlFocus();
        return true;
    }

    return false;
}

QString SpaceScene::getControlText(int controlhandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlhandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
        return controlGraphicsItem->text();

    return "";
}

bool SpaceScene::setControlText(int controlHandle, const QString& text)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
    {
        controlGraphicsItem->setText(text);
        return true;
    }

    return false;
}

bool SpaceScene::setControlFont(int controlHandle, int fontHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
    {
        if(FontTool* fontInfo = m_space->getFont(fontHandle))
        {
            controlGraphicsItem->setControlFont(fontInfo);
            return true;
        }
    }
    return false;
}

int SpaceScene::getControlStyle(int controlHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
        return controlGraphicsItem->getControlStyle();

    return 0;
}

bool SpaceScene::setControlStyle(int controlHandle, int style)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
    {
        controlGraphicsItem->setControlStyle(style);
        return true;
    }
    return false;
}

bool SpaceScene::isButtonChecked(int controlHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
        return controlGraphicsItem->isButtonChecked();
    return false;
}

bool SpaceScene::setButtonChecked(int controlHandle, int checkState)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
    {
        controlGraphicsItem->setButtonChecked(checkState);
        return true;
    }

    return false;
}

bool SpaceScene::listAddItem(int controlHandle, const QString& itemText)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
    {
        controlGraphicsItem->listInsertItem(itemText);
        return true;
    }
    return false;
}

bool SpaceScene::listInsertItem(int controlHandle, int startIndex, const QString& itemText)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
    {
        controlGraphicsItem->listInsertItem(itemText, startIndex);
        return true;
    }
    return false;
}

QString SpaceScene::listGetItem(int controlHandle, int index)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
        return controlGraphicsItem->listGetItem(index);

    return "";
}

bool SpaceScene::listClear(int controlHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
    {
        controlGraphicsItem->listClear();
        return true;
    }
    return false;
}

bool SpaceScene::listRemoveItem(int controlHandle, int index)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
        return controlGraphicsItem->listRemoveItem(index);

    return false;
}

int SpaceScene::listItemsCount(int controlHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
        return controlGraphicsItem->listItemsCount();

    return 0;
}

int SpaceScene::listCurrentIndex(int controlHandle)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
        return controlGraphicsItem->listCurrentIndex();

    return 0;
}

QList<int> SpaceScene::listSelectedIndexes(int controlHandle)
{
    QList<int> selectedIndexes;
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
        selectedIndexes = controlGraphicsItem->listSelectedIndexes();

    return selectedIndexes;
}

bool SpaceScene::listSetCurrentIndex(int controlHandle, int index)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
    {
        controlGraphicsItem->listSetSelectedIndex(index);
        return true;
    }
    return false;
}

int SpaceScene::listFindItem(int controlHandle, const QString text, int startIndex)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
        return controlGraphicsItem->listFindItem(text, startIndex);

    return -1;
}

int SpaceScene::listFindItemExact(int controlHandle, const QString text, int startIndex)
{
    QGraphicsItem* graphicsItem = m_graphicsItemForHandle.value(controlHandle);
    if(auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(graphicsItem))
        return controlGraphicsItem->listFindItem(text, startIndex, true);

    return -1;
}

QRectF SpaceScene::calculateSceneRect()
{
    QRectF rect;
    for(QGraphicsItem* graphicItem : items())
    {
        if(auto item = dynamic_cast<SpaceGraphicsItem*>(graphicItem))
        {
            QRectF itemRect(item->getSpaceItemOrg(), item->getSpaceItemSize());
            rect |= itemRect;
        }
    }
    return rect;
}

void SpaceScene::setBkBrush(BrushTool* bkBrush)
{
    if(m_bkBrush != bkBrush)
    {
        m_bkBrush = bkBrush;
        setBackgroundBrush(m_bkBrush ? m_bkBrush->brush() : Qt::NoBrush);
    }
}

void SpaceScene::setHyperJump(int objectHandle, const SpaceItemHyperKeyPtr& key)
{
    auto graphicsItem = m_graphicsItemForHandle.value(objectHandle);
    if(auto spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(graphicsItem))
    {
        spaceGraphicsItem->setHyperKey(key);
        spaceGraphicsItem->activateHyperJump();
    }
}

void SpaceScene::stdHyperJump(int objectHandle)
{
    if(auto spaceItem = m_space->getSpaceItem(objectHandle))
        spaceItem->invokeHyperJump();
}

void SpaceScene::pasteItem(SpaceItem* item, const QList<Tool*>& tools)
{
    if(!item) return;

    m_space->pasteItem(item, tools);

    SpaceGraphicsItem* spaceGraphicsItem = createGraphicsItem(item);
    updateItemsHandle(spaceGraphicsItem);
}

void SpaceScene::setHorizontalScrollRange(double min, double max)
{
    if(min < max)
    {
        m_horizontalScrollRangeMin = min;
        m_horizontalScrollRangeMax = max;

        QRectF rect = sceneRect();
        rect.setLeft(min);
        rect.setRight(max);
        setSceneRect(rect);
    }
}

void SpaceScene::setVertivalScrollRange(double min, double max)
{
    if(min < max)
    {
        m_verticalScrollRangeMin = min;
        m_verticalScrollRangeMax = max;

        QRectF rect = sceneRect();
        rect.setTop(min);
        rect.setBottom(max);
        setSceneRect(rect);
    }
}

void SpaceScene::removeGraphicsItem(QGraphicsItem* item)
{
    if(auto spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(item))
    {
        if(GroupItem* groupItem = spaceGraphicsItem->spaceItem()->ownerItem())
        {
            auto groupGraphicsItem = dynamic_cast<GroupGraphicsItem*>(m_graphicsItemForHandle.value(groupItem->handle()));
            if(groupGraphicsItem)
                groupGraphicsItem->removeGraphicsItemFromGroup(spaceGraphicsItem->spaceItem()->handle());
        }
    }

    if(auto groupItem = dynamic_cast<GroupGraphicsItem*>(item))
        removeGroupGraphicsItem(groupItem);
    else
        delete item;
}

void SpaceScene::removeGroupGraphicsItem(QGraphicsItem* item)
{
    if(auto groupItem = dynamic_cast<GroupGraphicsItem*>(item))
    {
        for(QGraphicsItem* item : groupItem->groupItems())
            removeGraphicsItem(item);

        delete groupItem;
    }
}

bool SpaceScene::deleteObject(int objectHandle)
{
    if(QGraphicsItem* item = m_graphicsItemForHandle.value(objectHandle))
    {
        removeGraphicsItem(item);
        updateItemsHandles();
        m_space->deleteObject(objectHandle);

        updateItemsZOrder();

        return true;
    }
    return false;
}

void SpaceScene::updateItemsVisibility()
{
    for(QGraphicsItem* item : items())
        if(SpaceGraphicsItem* spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(item))
            spaceGraphicsItem->updateVisibility();
}

//HandleCollection<SpaceItem> SpaceScene::cleanSpaceItems(Space* space)
//{
//    HandleCollection<SpaceItem> cleanSpaceItems;

//    for(SpaceItem* item : space->spaceItems().items())
//        if(!item->ownerItem())
//            cleanSpaceItems.insert(item, true);

//    return cleanSpaceItems;
//}

void SpaceScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    QGraphicsItem* graphicsItem = itemAt(event->scenePos(), QTransform());
    emit contextMenu(event->screenPos(), graphicsItem);
}

void SpaceScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    emit doubleClick(itemAt(event->scenePos(), QTransform()));
    QGraphicsScene::mouseDoubleClickEvent(event);
}

void SpaceScene::initializeGraphicsItems()
{
    m_graphicsItemForHandle.clear();
    clear();

    QList<SpaceItem*> spaceItemsTree = Space::toTree(m_space->spaceItems().items());

    for(SpaceItem* spaceItem : spaceItemsTree)
    {
        if(!spaceItem)
            continue;

        createGraphicsItem(spaceItem);
    }
    updateItemsHandles();
}

SpaceGraphicsItem* SpaceScene::createGraphicsItem(SpaceItem* spaceItem)
{
    SpaceGraphicsItem* spaceGraphicsItem = nullptr;

    if(auto polylineItem2d = dynamic_cast<PolylineItem2d*>(spaceItem))
    {
        spaceGraphicsItem = polylineItem2d->typeOnSpace() == SpaceItem::Type_Link ?
                           new LinkGraphicsItem(polylineItem2d) :
                           new PolylineGraphicsItem(polylineItem2d);
    }
    else if(auto groupItem = dynamic_cast<GroupItem*>(spaceItem))
    {
        if(groupItem->typeOnSpace() == SpaceItem::Type_Object)
        {
            ObjectInfo* objInfo = m_space->ownerClass()->objectInfoByHandle(groupItem->handle());
            spaceGraphicsItem = new ObjectGraphicsItem(objInfo, groupItem, this);
        }
        else
        {
            auto groupGraphicsItem = new GroupGraphicsItem(groupItem, this);
            spaceGraphicsItem = groupGraphicsItem;
        }
    }
    else if(auto textItem = dynamic_cast<TextItem2d*>(spaceItem))
    {
        auto item = new RasterTextGraphicsItem(textItem);
        spaceGraphicsItem = item;
    }
    else if(TextureItem2d* pixmapItem = dynamic_cast<TextureItem2d*>(spaceItem))
    {
        auto item = new PixmapGraphicsItem(pixmapItem);
        spaceGraphicsItem = item;
    }

    if(spaceGraphicsItem)
        addItem(spaceGraphicsItem->graphicsItem());

    return spaceGraphicsItem;
}

void SpaceScene::addGroupItemsToDictionary(GroupGraphicsItem* groupGraphicsItem)
{
    for(QGraphicsItem* childGraphicsItem : groupGraphicsItem->groupItems())
    {
        if(auto spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(childGraphicsItem))
            m_graphicsItemForHandle[spaceGraphicsItem->handle()] = childGraphicsItem;

        if(auto childGroupGraphicsItem = dynamic_cast<GroupGraphicsItem*>(childGraphicsItem))
            addGroupItemsToDictionary(childGroupGraphicsItem);
    }
}

void SpaceScene::updateItemsHandles()
{
    m_graphicsItemForHandle.clear();

    for(QGraphicsItem* item : items())
        updateItemsHandle(dynamic_cast<SpaceGraphicsItem*>(item));
}

void SpaceScene::updateItemsHandle(SpaceGraphicsItem* item)
{
    if(item == nullptr) return;

    m_graphicsItemForHandle[item->handle()] = item->graphicsItem();
    if(auto groupGraphicsItem = dynamic_cast<GroupGraphicsItem*>(item))
        addGroupItemsToDictionary(groupGraphicsItem);
//    else
//        m_graphicsItemForHandle[spaceGraphicsItem->handle()] = item->graphicsItem();
}

//TODO обновлять z-порядок начиная с позиции
void SpaceScene::updateItemsZOrder(int /*fromZOrder*/)
{
    for(QGraphicsItem* item : items())
        if(SpaceGraphicsItem* spaceItem = dynamic_cast<SpaceGraphicsItem*>(item))
            spaceItem->updateZOrder();
}

}
