#include "Space.h"

#include "Log.h"

#include "Sc2000DataStream.h"
#include "Tool2d.h"
#include "SpaceItem.h"
#include "SpaceChunk.h"
#include "SpaceCollectionResult.h"
#include "SpaceItemResult.h"
#include "GraphicsHelper.h"
#include "Class.h"
#include "Library.h"
#include "Scheme.h"
#include "SpaceToolResult.h"
#include "SpaceLoader.h"
#include "HyperBase.h"
#include "CopyToolInitializer.h"
#include "CopySpaceItemInitializer.h"
#include "Log.h"

#include <QBitArray>
#include <map>
#include <QDebug>
#include <QFontMetrics>
#include <QFileInfo>

#include <QDebug>
#include <QImageReader>

using namespace std;

namespace StData {

Space::Space() : Space(nullptr)
{}

Space::Space(Scheme* scheme) :
    m_layerBits(32, true),
    m_scale(1.0),
    m_scheme(scheme),
    m_brushHandle(0)
{
    if(scheme)
        m_dir = scheme->cls()->library()->libraryDir();
}

Space::Space(const QString& fileName) :
    m_layerBits(32, true),
    m_scale(1.0),
    m_scheme(nullptr),
    m_brushHandle(0),
    m_fileName(fileName)
{
    m_dir = QFileInfo(m_fileName).absoluteDir();
}

Space::~Space()
{
    m_items.deleteAll();
    m_rasterTexts.deleteAll();

    m_pens.deleteAll();
    m_brushes.deleteAll();
    m_textures.deleteAll();
    m_maskedTextures.deleteAll();
    m_fonts.deleteAll();
    m_strings.deleteAll();
}

Class* Space::ownerClass() const
{
    return m_scheme ? m_scheme->cls() : nullptr;
}

void Space::setLayers(const quint32& layers)
{
    m_layers = layers;
    for(int i = 0; i < 32; i++)
        m_layerBits[i] = ((m_layers >> i) & 1) == 0;
}

bool Space::isLayerVisible(int layerIndex)
{
    if(layerIndex >= 0 && layerIndex < m_layerBits.count())
        return m_layerBits[layerIndex];

    return false;
}

void Space::addItem(SpaceItem* spaceItem)
{
    m_items.insert(spaceItem);
}

int Space::getHandleByZOrder(int zOrderIndex) const
{
    if(zOrderIndex >= 0 && zOrderIndex < m_zOrderList.count())
        return m_zOrderList.at(zOrderIndex)->handle();

    return 0;
}

int Space::getZOrderIndex(SpaceItem* item) const
{
    return m_zOrderList.indexOf(item);
}

int Space::getZOrderIndexByHandle(int handle)
{
    SpaceItem* item = m_items.getItem(handle);
    return m_zOrderList.indexOf(item);
}

QList<SpaceItem*> Space::toTree(const QList<SpaceItem*>& linearList)
{
    QList<SpaceItem*> tree;

    for(SpaceItem* item : linearList)
        if(!item->ownerItem())
            tree.append(item);

    return tree;
}

void Space::toLinear(const QList<SpaceItem*>& treeList, QList<SpaceItem*>* linearList)
{
    for(SpaceItem* item : treeList)
    {
        if(GroupItem* groupItem = dynamic_cast<GroupItem*>(item))
            toLinear(groupItem->items(), linearList);

        linearList->append(item);
    }
}

void Space::deleteObject(int handle)
{
    SpaceItem* item = m_items.take(handle);
    if(!item)
        return;
    
    if(GroupItem* parentGroup = item->ownerItem())
        parentGroup->remove(item);

    if(GroupItem* groupItem = dynamic_cast<GroupItem*>(item))
        for(SpaceItem* childItem : groupItem->items())
            deleteObject(childItem->handle());

    m_zOrderList.removeOne(item);

    delete item;
}

int Space::getObjectType(int handle)
{
    if(SpaceItem* item = m_items.getItem(handle))
        return item->loadingType();

    return 0;
}

int Space::getObjectAttribute2d(int objectHandle)
{
    if(SpaceItem* item = m_items.getItem(objectHandle))
        return item->options();

    return 0;
}

bool Space::setObjectAttribute(int objectHandle, int attrMode, int attrValue)
{
    if(SpaceItem* item = m_items.getItem(objectHandle))
    {
        item->setOptions((SpaceItem::OptionsMode)attrMode, attrValue);
        return true;
    }
    return false;
}

SpaceItem* Space::getItemFromPoint2d(double x, double y, std::function<bool(int)> predicate)
{
    for(int i = m_zOrderList.count() - 1; i >= 0; i--)
    {
        SpaceItem* item = m_zOrderList.at(i);
        if(item->isVisible() && item->isSelectable())
        {
            SpaceItem2d* spaceItem2d = static_cast<SpaceItem2d*>(item);
            if(spaceItem2d->containsPoint(x, y))
            {
                if(predicate == nullptr)
                    return item;

                else if(predicate(item->handle()))
                    return item;
            }
        }
    }
    return nullptr;
}

bool Space::objectToTop(int objectHandle)
{
    if(SpaceItem* item = m_items.getItem(objectHandle))
    {
        int index = m_zOrderList.indexOf(item);

        if(index >= 0 && index < m_zOrderList.count()-1)
        {
            m_zOrderList.removeAt(index);
            m_zOrderList.append(item);
        }
        controlsToTop();

        return true;
    }
    return false;
}

bool Space::objectToBottom(int objectHandle)
{
    if(SpaceItem* item = m_items.getItem(objectHandle))
    {
        int index = m_zOrderList.indexOf(item);
        if(index != -1)
        {
            m_zOrderList.removeAt(index);
            m_zOrderList.prepend(item);
            controlsToTop();
        }
        return true;
    }
    return false;
}

bool Space::setObjectZOrder(int objectHandle, int zOrder)
{
    SpaceItem* item = m_items.getItem(objectHandle);
    int index = m_zOrderList.indexOf(item);

    if(index != -1 && (zOrder > 0 && zOrder <= m_zOrderList.count()))
    {
        m_zOrderList.move(index, zOrder - 1);
        controlsToTop();
        return true;
    }
    return false;
}

bool Space::swapObjectsZOrder(int objectHandle1, int objectHandle2)
{
    int index1 = getZOrderIndexByHandle(objectHandle1);
    int index2 = getZOrderIndexByHandle(objectHandle2);

    if(index1 != -1 && index2 != -1)
    {
        m_zOrderList.swap(index1, index2);
        controlsToTop();
        return true;
    }
    return false;
}

int Space::createPen(qint16 rop, int rgba, int width, int style)
{
    //    auto comparer = [&](PenTool* penTool)
    //    {
    //        return penTool->rgba() == rgba &&
    //                penTool->width() == width &&
    //                penTool->style() == style &&
    //                penTool->rop() == rop;
    //    };
    //    QList<PenTool*> pens = m_pens.items();
    //    auto it = std::find_if(pens.begin(), pens.end(), comparer);
    //    if(it != pens.end())
    //        return (*it)->handle();

    PenTool* newPenTool = m_pens.createItem();
    newPenTool->setRgba(rgba);
    newPenTool->setWidth(width);
    newPenTool->setStyle(style);
    newPenTool->setRop(rop);
    return newPenTool->handle();
}

int Space::createBrush(int style, int hatch, int rgba, int dibHandle, qint16 rop)
{
    //    auto comparer = [&](BrushTool* brushTool)
    //    {
    //        bool ok = brushTool->rgba() == rgba &&
    //                  brushTool->style() == style &&
    //                  brushTool->rop() == rop &&
    //                  brushTool->hatch() == hatch;

    //        if(ok && dibHandle != 0)
    //            ok = brushTool->texture() && brushTool->texture()->handle() == dibHandle;

    //        return ok;
    //    };
    //    QList<BrushTool*> brushes = m_brushes.items();
    //    auto it = std::find_if(brushes.begin(), brushes.end(), comparer);
    //    if(it != brushes.end())
    //        return (*it)->handle();

    BrushTool* newBrushTool = m_brushes.createItem();
    newBrushTool->setStyle(style);
    newBrushTool->setHatch(hatch);
    newBrushTool->setRgba(rgba);
    newBrushTool->setTexture(m_textures.getItem(dibHandle));
    newBrushTool->setRop(rop);
    return newBrushTool->handle();
}

int Space::createFont(const QFont& font, bool sizeInPixels)
{
    //    auto comparer = [&](FontTool* tool) { return tool->font() == font; };
    //    QList<FontTool*> fonts = m_fonts.items();
    //    auto it = std::find_if(fonts.begin(), fonts.end(), comparer);
    //    if(it != fonts.end())
    //        return (*it)->handle();

    FontTool* fontTool = m_fonts.createItem();
    fontTool->setFont(font);
    fontTool->setSizeInPixels(sizeInPixels);
    return fontTool->handle();
}

int Space::createString(const QString& data)
{
    //    auto comparer = [&](StringTool* tool) { return tool->string() == data; };
    //    QList<StringTool*> strings = m_strings.items();
    //    auto it = std::find_if(strings.begin(), strings.end(), comparer);
    //    if(it != strings.end())
    //        return (*it)->handle();

    StringTool* stringTool = m_strings.createItem();
    stringTool->setString(data);
    return stringTool->handle();
}

int Space::createText(int fontHandle, int stringHandle, const QColor& bgColor, const QColor& fgColor)
{
    FontTool* fontTool = getFont(fontHandle);
    StringTool* stringTool = getString(stringHandle);

    if(fontTool && stringTool)
    {
        TextFragment* textTool = new TextFragment();
        textTool->setData(fontTool, stringTool, bgColor, fgColor);

        RasterTextTool* rasterText = m_rasterTexts.createItem();
        rasterText->append(textTool);
        return rasterText->handle();

        //        auto comparer = [&](TextFragmentTool* tool)
        //        {
        //            return tool->stringTool()->handle() == stringHandle &&
        //                    tool->fontTool()->handle() == fontHandle &&
        //                    tool->fgColor() == fgColor &&
        //                    tool->bgColor() == bgColor;
        //        };
        //        QList<TextFragmentTool*> texts = m_texts.items();
        //        auto it = std::find_if(texts.begin(), texts.end(), comparer);
        //        if(it != texts.end())
        //            return (*it)->handle();

        //        TextFragmentTool* textTool = m_texts.createItem();
        //        textTool->setData(fontTool, stringTool, bgColor, fgColor);
        //        return textTool->handle();
    }
    return 0;
}

int Space::createDib(const QString& absoluteFileName)
{
//    int ddibHandle = createDoubleDib(absoluteFileName);
//    if(ddibHandle != 0)
//        return ddibHandle;

    return createDib(QPixmap(absoluteFileName), false);
}

int Space::createDib(const QPixmap& pixmap, bool autoConvert)
{
    if(!pixmap.isNull())
    {
        //        auto comparer = [&](TextureTool* tool)
        //        {
        //            return tool->pixmap().toImage() == pixmap.toImage();
        //        };
        //        QList<TextureTool*> textures = m_textures.items();
        //        auto it = std::find_if(textures.begin(), textures.end(), comparer);
        //        if(it != textures.end())
        //            return (*it)->handle();

        if(pixmap.mask().isNull() || !autoConvert)
        {
            TextureTool* textureTool = m_textures.createItem();
            textureTool->setPixmap(pixmap);
            return textureTool->handle();
        }
        return createDoubleDib(pixmap);
    }
    return 0;
}

int Space::createRDib(QString fileName)
{
    //    auto comparer = [&](TextureTool* tool)
    //    {
    //        if(TextureRefTool* refTextureTool = dynamic_cast<TextureRefTool*>(tool))
    //            return refTextureTool->reference() == fileName;

    //        return false;
    //    };
    //    QList<TextureTool*> textures = m_textures.items();
    //    auto it = std::find_if(textures.begin(), textures.end(), comparer);
    //    if(it != textures.end())
    //        return (*it)->handle();

    TextureRefTool* pixmapInfo = new TextureRefTool();
    pixmapInfo->setPixmapPath(fileName);
    if(QFileInfo(fileName).isRelative())
        fileName = m_dir.absoluteFilePath(fileName);

    pixmapInfo->setPixmap(QPixmap(fileName));
    return m_textures.insert(pixmapInfo);
}

int Space::createDoubleDib(const QString& absoluteFileName)
{
    QPixmap pixmap = GraphicsHelper::loadMaskedPixmap(absoluteFileName);
    return createDoubleDib(pixmap);
    //    if(!pixmap.isNull())
    //    {
    //        if(!pixmap.mask().isNull())
    //        {
    //            TextureTool* pixmapInfo = m_maskedTextures.createItem();
    //            pixmapInfo->setPixmap(pixmap);
    //            return pixmapInfo->handle();
    //        }
    //    }
    //    return 0;
}

int Space::createDoubleDib(const QPixmap& pixmap)
{
    //            auto comparer = [&](TextureTool* tool)
    //            {
    //                return tool->pixmap().toImage() == pixmap.toImage();
    //            };
    //            QList<TextureTool*> maskedTextures = m_maskedTextures.items();
    //            auto it = std::find_if(maskedTextures.begin(), maskedTextures.end(), comparer);
    //            if(it != maskedTextures.end())
    //                return (*it)->handle();

    if(!pixmap.isNull())
    {
        if(!pixmap.mask().isNull())
        {
            SystemLog::instance().info(QString("Mask is not null"));

            TextureTool* pixmapInfo = m_maskedTextures.createItem();
            pixmapInfo->setPixmap(pixmap);
            return pixmapInfo->handle();
        }
    }
    return 0;
}

int Space::createDoubleRDib(QString fileName)
{
    if(QFileInfo(fileName).isRelative())
        fileName = m_dir.absoluteFilePath(fileName);

    QPixmap pixmap = GraphicsHelper::loadMaskedPixmap(fileName);
    if(!pixmap.isNull())
    {
        //        auto comparer = [&](TextureTool* tool)
        //        {
        //            if(TextureRefTool* refTextureTool = dynamic_cast<TextureRefTool*>(tool))
        //                return refTextureTool->reference() == fileName;
        //            return false;
        //        };
        //        QList<TextureTool*> maskedTextures = m_maskedTextures.items();
        //        auto it = std::find_if(maskedTextures.begin(), maskedTextures.end(), comparer);
        //        if(it != maskedTextures.end())
        //            return (*it)->handle();

        TextureRefTool* pixmapRefInfo = new TextureRefTool();
        pixmapRefInfo->setPixmap(pixmap);
        pixmapRefInfo->setPixmapPath(fileName);
        return m_maskedTextures.insert(pixmapRefInfo);
    }
    return 0;
}

bool Space::insertText(int rasterTextHandle, int index, int fontHandle, int stringHandle,
                       const QColor& bgColor, const QColor& fgColor)
{
    FontTool* fontTool = getFont(fontHandle);
    StringTool* stringTool = getString(stringHandle);
    RasterTextTool* rasterText = getRasterText(rasterTextHandle);

    if(index == -1)
        index = rasterText->textFragments().count();

    if(fontTool && stringTool && index >= 0 && index <= rasterText->textFragments().count())
    {
        //        TextFragmentTool* textInfo = m_texts.createItem();
        rasterText->insertFragment(index, fontTool, stringTool, bgColor, fgColor);
        return true;
    }
    return false;
}

bool Space::removeText(int rasterTextHandle, int index)
{
    if(RasterTextTool* rasterText = getRasterText(rasterTextHandle))
    {
        if(index >= 0 && index < rasterText->textFragments().count())
        {
            rasterText->removeAt(index);
            return true;
        }
    }
    return false;
}

PolylineItem2d* Space::createPolyline(int penHandle, int brushHandle, const QPolygonF& polyline)
{
    if(polyline.isEmpty() || (penHandle == 0 && brushHandle == 0))
        return nullptr;

    PenTool* penTool = m_pens.getItem(penHandle);
    BrushTool* brushTool = m_brushes.getItem(brushHandle);

    if((penHandle && !penTool) || (brushHandle && !brushTool))
        return nullptr;

    PolylineItem2d* polylineItem = new PolylineItem2d(this);
    polylineItem->setPolyline(polyline);

    polylineItem->setPenTool(penTool);
    polylineItem->setBrushTool(brushTool);

    m_items.insert(polylineItem);
    m_zOrderList.append(polylineItem);

    return polylineItem;
}

TextItem2d* Space::createRasterText(int textHandle, double x, double y, double angle)
{
    if(RasterTextTool* rasterText = getRasterText(textHandle))
    {
        //        RasterTextTool* rasterText = m_rasterTexts.createItem();
        //        rasterText->append(rasterTextTool);

        TextItem2d* textItem2d = new TextItem2d(this);
        textItem2d->setTextAngle(-angle);
        textItem2d->setOrigin(QPointF(x, y));
        textItem2d->setRasterText(rasterText);

        //        if(rasterText->fontTool() && rasterText->stringTool())
        //        {
        //            QFontMetrics fontMetrics(rasterText->fontTool()->font());
        //            double w = fontMetrics.width(rasterText->stringTool()->string());
        //            double h = fontMetrics.height();
        //            textItem2d->setSize(QSizeF(w, h));
        //        }
        m_items.insert(textItem2d);
        m_zOrderList.append(textItem2d);

        return textItem2d;
    }
    return nullptr;
}

TextureItem2d* Space::createPixmap(int dibHandle, double x, double y)
{
    if(TextureTool* texture = getTexture(dibHandle))
    {
        TextureItem2d* pixmapItem2d = new TextureItem2d(this);
        pixmapItem2d->setTexture(texture);
        pixmapItem2d->setOrigin(QPointF(x, y));

        pixmapItem2d->setSize(texture->pixmap().size());
        pixmapItem2d->setTextureSize(texture->pixmap().size());

        m_items.insert(pixmapItem2d);
        m_zOrderList.append(pixmapItem2d);

        return pixmapItem2d;
    }
    return nullptr;
}

MaskedTextureItem2d* Space::createMaskedPixmap(int doublDibHandle, double x, double y)
{
    if(TextureTool* texture = getMaskedTexture(doublDibHandle))
    {
        MaskedTextureItem2d* pixmapItem2d = new MaskedTextureItem2d(this);
        pixmapItem2d->setTexture(texture);
        pixmapItem2d->setOrigin(QPointF(x, y));
        pixmapItem2d->setSize(texture->pixmap().size());
        pixmapItem2d->setTextureSize(texture->pixmap().size());

        m_items.insert(pixmapItem2d);
        m_zOrderList.append(pixmapItem2d);

        return pixmapItem2d;
    }
    return nullptr;
}

int Space::getDibPixelRgba(int dibHandle, int x, int y)
{
    int rgba = 0;
    if(TextureTool* pixmapInfo = getTexture(dibHandle))
    {
        QImage image = pixmapInfo->pixmap().toImage();
        if(x >=0 && x < image.width() && y >= 0 && y < image.height())
        {
            rgba = image.pixel(x, y);
        }
    }
    return rgba;
}

bool Space::setDibPixelColor(int dibHandle, int x, int y, int rgba)
{
    if(TextureTool* pixmapInfo = getTexture(dibHandle))
    {
        QImage image = pixmapInfo->pixmap().toImage();
        if(x >= 0 && x < image.width() && y >= 0 && y < image.height())
        {
            image.setPixel(x, y, rgba);
            pixmapInfo->setPixmap(QPixmap::fromImage(image));
            return true;
        }
    }
    return false;
}

GroupItem* Space::createGroup(const QVector<int>& objectHandles)
{
    //    for(int objectHandle : objectHandles)
    //        if(!m_spaceItems.getItem(objectHandle))
    //            return nullptr;

    GroupItem* groupItem = new GroupItem(this);
    groupItem->addItemHandles(objectHandles.toList());
    m_items.insert(groupItem);

    return groupItem;
}

bool Space::deleteGroup(int handle)
{
    if(GroupItem* groupItem = dynamic_cast<GroupItem*>(getSpaceItem(handle)))
    {
        m_items.remove(handle);
        groupItem->prepareToRemoving();
        delete groupItem;
        return true;
    }
    return false;
}

bool Space::addItemToGroup(int groupHandle, int itemHandle)
{
    GroupItem* groupItem = dynamic_cast<GroupItem*>(getSpaceItem(groupHandle));
    SpaceItem* spaceItem = getSpaceItem(itemHandle);

    if(groupItem && spaceItem && spaceItem != groupItem && spaceItem->ownerItem() == nullptr)
    {
        groupItem->addItemHandle(spaceItem->handle());
        return true;
    }
    return false;
}

bool Space::deleteGroupItem(int groupHandle, int itemHandle)
{
    GroupItem* groupItem = dynamic_cast<GroupItem*>(getSpaceItem(groupHandle));
    SpaceItem* spaceItem = getSpaceItem(itemHandle);

    if(groupItem && spaceItem && spaceItem != groupItem && spaceItem->ownerItem() == groupItem)
    {
        groupItem->removeItemHandle(itemHandle);
        spaceItem->setOwnerItem(nullptr);
        return true;
    }
    return false;
}

int Space::getGroupItem(int groupHandle, int index)
{
    GroupItem* groupItem = dynamic_cast<GroupItem*>(getSpaceItem(groupHandle));
    if(groupItem && index >= 0 && index < groupItem->itemsCount())
        return groupItem->itemAt(index)->handle();

    return 0;
}

int Space::getGroupItemsCount(int groupHandle)
{
    if(GroupItem* groupItem = dynamic_cast<GroupItem*>(getSpaceItem(groupHandle)))
        return groupItem->itemsCount();

    return 0;
}

int Space::getNextItemHandle(int fromHandle)
{
    return m_items.getNextHandle(fromHandle);
}

QList<SpaceItem*> Space::copyItems()
{
    QList<SpaceItem*> copyTree;
    for(SpaceItem* spaceItem : m_items.items())
        if(!spaceItem->ownerItem())
            copyTree << spaceItem->deepCopy();

    return copyTree;
}

void Space::pasteItems(Space* sourceSpace, const QList<SpaceItem*>& items, int startZOrder)
{
    for(SpaceItem* item : items)
        if(item->ownerItem())
            item->setOwnerItem(nullptr);

    QList<SpaceItem*> linearCopyList;
    toLinear(items, &linearCopyList);

    QVector<SpaceItem*> zOrderArray = QVector<SpaceItem*>(sourceSpace->zOrderList().count(), nullptr);
    for(SpaceItem* copyItem : linearCopyList)
    {
        int index = sourceSpace->getZOrderIndexByHandle(copyItem->handle());
        if(index >=0 && index < zOrderArray.count())
            zOrderArray[index] = copyItem;
    }

    for(int i = 0; i < zOrderArray.count(); i++)
    {
        SpaceItem* copyItem = zOrderArray.at(i);
        if(startZOrder == -1)
            m_zOrderList << copyItem;
        else
            m_zOrderList.insert(startZOrder + i+1, copyItem);
    }

    for(SpaceItem* copyItem : linearCopyList)
    {
        addItemTools(copyItem);
        copyItem->setSpace(this);

        m_items.insert(copyItem);
    }
}

void Space::pasteItem(SpaceItem* spaceItem, const QList<Tool*>& tools)
{
    if(!spaceItem) return;

    CopyToolInitializer toolInitializer(this);
    for(Tool* tool : tools)
        tool->accept(&toolInitializer);

    CopySpaceItemInitializer spaceItemInitializer;
    spaceItem->accept(&spaceItemInitializer);

    pasteItem(spaceItem);
}

void Space::pasteItem(SpaceItem* spaceItem)
{
    spaceItem->setSpace(this);
    m_items.insert(spaceItem);
    m_zOrderList << spaceItem;

    if(auto groupItem = dynamic_cast<GroupItem*>(spaceItem))
    {
        for(SpaceItem* childGroupItem : groupItem->items())
            pasteItem(childGroupItem);
    }
}

void Space::addItemTools(SpaceItem* spaceItem)
{
    if(ShapeItem2d* shapeItem = dynamic_cast<ShapeItem2d*>(spaceItem))
    {
        if(shapeItem->penTool())
            m_pens.insert(shapeItem->penTool());

        if(shapeItem->brushTool())
            m_brushes.insert(shapeItem->brushTool());
    }

    else if(MaskedTextureItem2d* maskedPixmapItem = dynamic_cast<MaskedTextureItem2d*>(spaceItem))
        m_maskedTextures.insert(maskedPixmapItem->texture());

    else if(TextureItem2d* pixmapItem = dynamic_cast<TextureItem2d*>(spaceItem))
        m_textures.insert(pixmapItem->texture());

    else if(TextItem2d* textItem = dynamic_cast<TextItem2d*>(spaceItem))
    {
        m_rasterTexts.insert(textItem->rasterText());
        QList<FontTool*> fontList;
        QList<StringTool*> stringList;
        for(TextFragment* text : textItem->rasterText()->textFragments())
        {
            if(!fontList.contains(text->fontTool()))
                fontList.append(text->fontTool());

            if(!stringList.contains(text->stringTool()))
                stringList.append(text->stringTool());
        }
        m_fonts.insert(fontList);
        m_strings.insert(stringList);
    }
}

bool Space::deleteTool(int toolHandle, int toolType)
{
    Tool* tool = nullptr;
    switch(toolType)
    {
        case Tool::Pen:
            tool = m_pens.getItem(toolHandle);
            if(tool && tool->refCount() == 0)
            {
                m_pens.deleteItem(toolHandle);
                return true;
            }
            break;

        case Tool::Brush:
            tool = m_brushes.getItem(toolHandle);
            if(tool && tool->refCount() == 0)
            {
                m_brushes.deleteItem(toolHandle);
                return true;
            }
            break;

        case Tool::Texture:
            tool = m_textures.getItem(toolHandle);
            if(tool && tool->refCount() == 0)
            {
                m_textures.deleteItem(toolHandle);
                return true;
            }
            break;

        case Tool::MaskedTexture:
            tool = m_maskedTextures.getItem(toolHandle);
            if(tool && tool->refCount() == 0)
            {
                m_maskedTextures.deleteItem(toolHandle);
                return true;
            }
            break;

        case Tool::Font:
            tool = m_fonts.getItem(toolHandle);
            if(tool && tool->refCount() == 0)
            {
                m_fonts.deleteItem(toolHandle);
                return true;
            }
            break;

        case Tool::String:
            tool = m_strings.getItem(toolHandle);
            if(tool && tool->refCount() == 0)
            {
                m_strings.deleteItem(toolHandle);
                return true;
            }
            break;

        case Tool::Text:
            tool = m_rasterTexts.getItem(toolHandle);
            if(tool && tool->refCount() == 0)
            {
                m_rasterTexts.deleteItem(toolHandle);
                return true;
            }
            break;
    }
    return false;
}

ControlItem2d* Space::createControl(const QString& className, const QString& text,
                                    int style, int x, int y, int width, int height, int id)
{
    ControlItem2d* controlItem = ControlItem2d::create(this, className, style, text, QSizeF(width, height), id);
    if(controlItem)
    {
        controlItem->setOrigin(QPointF(x, y));
        controlItem->setSize(QSizeF(width, height));

        m_items.insert(controlItem);
        m_zOrderList << controlItem;
        m_controlItems << controlItem;
    }
    return controlItem;
}

bool Space::load(St2000DataStream& stream)
{
    SpaceLoader spaceLoader(this);
    if(!spaceLoader.load(stream))
        return false;

    m_org = spaceLoader.spaceOrg();
    m_scaleDiv = spaceLoader.scaleDiv();
    m_scaleMul = spaceLoader.scaleMul();
    m_state = spaceLoader.state();
    m_brushHandle = spaceLoader.brushHandle();

    setLayers(spaceLoader.layers());
    m_defaultFlags = spaceLoader.defaultFlags();

    setupTools(&spaceLoader);

    QMap<GroupItemResult*, GroupItem*> groupForResult;
    setupItems(&spaceLoader, &groupForResult);

    for(int itemHandle : spaceLoader.zOrderHandles())
        m_zOrderList << m_items.getItem(itemHandle);

    setupGroups(groupForResult);

    setupHyperBase(&spaceLoader);

    return true;
}

void Space::setupTools(SpaceLoader* spaceLoader)
{
    for(PenResult* res : spaceLoader->penResults())
    {
        PenTool* pen = new PenTool(res->handle());
        pen->setData(res->rgba(), res->width(), res->style(), res->rop());
        m_pens.insert(pen, true);
    }
    for(TextureResult* res : spaceLoader->textureResults())
    {
        TextureTool* texture = new TextureTool(res->handle());
        texture->setPixmap(res->pixmap());
        m_textures.insert(texture, true);
    }
    for(MaskedTextureResult* res : spaceLoader->maskedTextureResults())
    {
        TextureTool* texture = new TextureTool(res->handle());
        texture->setPixmap(res->pixmap());
        m_maskedTextures.insert(texture, true);
    }
    for(BrushResult* res : spaceLoader->brushResults())
    {
        BrushTool* brush = new BrushTool(res->handle());
        TextureTool* texture = m_textures.getItem(res->textureHandle());
        brush->setData(res->rgba(), res->style(), res->hatch(), res->rop(), texture);
        m_brushes.insert(brush, true);
    }
    for(FontResult* res : spaceLoader->fontResults())
    {
        FontTool* font = new FontTool(res->handle());
        font->setFont(res->font());
        font->setSizeInPixels(res->isSizeInPixels());
        m_fonts.insert(font, true);
    }
    for(StringResult* res : spaceLoader->stringResults())
    {
        StringTool* string = new StringTool(res->handle());
        string->setString(res->str());
        m_strings.insert(string, true);
    }
    for(RasterTextResult* res : spaceLoader->rasterTextResults())
    {
        RasterTextTool* rasterText = new RasterTextTool(res->handle());
        for(TextResult* textRes : res->textResults())
        {
            TextFragment* text = new TextFragment();
            text->setBgColor(textRes->bgColor());
            text->setFgColor(textRes->fgColor());
            text->setFont(m_fonts.getItem(textRes->fontHandle()));
            text->setString(m_strings.getItem(textRes->stringHandle()));

            rasterText->append(text);
            //            m_texts.insert(text);
        }
        m_rasterTexts.insert(rasterText, true);
    }
}

void Space::setupItems(SpaceLoader* spaceLoader, QMap<GroupItemResult*, GroupItem*>* groupForResult)
{
    for(SpaceItemResult* res : spaceLoader->spaceItemResults())
    {
        SpaceItem* spaceItem = nullptr;
        if(PolylineResult* polylineResult = dynamic_cast<PolylineResult*>(res))
        {
            PolylineItem2d* polylineItem = new PolylineItem2d(this);
            polylineItem->setPolyline(polylineResult->polyline());
            polylineItem->setArrow(polylineResult->arrow());
            spaceItem = polylineItem;
        }
        else if(TextureItem2dResult* textureResult = dynamic_cast<TextureItem2dResult*>(res))
        {
            TextureItem2d* textureItem;
            if(dynamic_cast<MaskedTextureItem2dResult*>(res))
            {
                textureItem = new MaskedTextureItem2d(this);
                textureItem->setTexture(m_maskedTextures.getItem(textureResult->textureHandle()));
            }
            else
            {
                textureItem = new TextureItem2d(this);
                textureItem->setTexture(m_textures.getItem(textureResult->textureHandle()));
            }

            textureItem->setTextureOrigin(textureResult->imageOrigin());
            textureItem->setTextureSize(textureResult->imageSize());
            spaceItem = textureItem;
        }
        else if(ControlItem2dResult* controlResult = dynamic_cast<ControlItem2dResult*>(res))
        {
            ControlItem2d* controlItem = ControlItem2d::create(this, controlResult->className(),
                                                               controlResult->dwStyle(),
                                                               controlResult->text(),
                                                               controlResult->size(),
                                                               controlItem->id());
            spaceItem = controlItem;
        }
        else if(TextItem2dResult* textResult = dynamic_cast<TextItem2dResult*>(res))
        {
            TextItem2d* textItem = new TextItem2d(this);
            RasterTextTool* rasterText = m_rasterTexts.getItem(textResult->rasterTextHandle());
            textItem->setRasterText(rasterText);
            textItem->setTextAngle(textResult->textAngle());
            spaceItem = textItem;
        }
        else if(GroupItemResult* groupResult = dynamic_cast<GroupItemResult*>(res))
        {
            GroupItem* groupItem = new GroupItem(this);
            spaceItem = groupItem;
            groupForResult->insert(groupResult, groupItem);
        }

        if(ShapeItem2d* shapeItem = dynamic_cast<ShapeItem2d*>(spaceItem))
        {
            ShapeItem2dResult* shapeResult = static_cast<ShapeItem2dResult*>(res);
            shapeItem->setPenTool(m_pens.getItem(shapeResult->penHandle()));
            shapeItem->setBrushTool(m_brushes.getItem(shapeResult->brushHandle()));
        }
        if(SpaceItem2d* spaceItem2d = dynamic_cast<SpaceItem2d*>(spaceItem))
        {
            SpaceItem2dResult* spaceItem2dResult = static_cast<SpaceItem2dResult*>(res);
            spaceItem2d->setOrigin(spaceItem2dResult->origin());
            spaceItem2d->setSize(spaceItem2dResult->size());
        }

        if(spaceItem)
        {
            spaceItem->setHandle(res->handle());
            spaceItem->setLoadingType(res->loadingType());
            spaceItem->setName(res->name());
            spaceItem->setOptions(res->options());

            auto hyperBase = HyperBase(res->hyperDataMap());
            spaceItem->setHyperKey(hyperBase.getSpaceItemHyperKey());

            m_items.insert(spaceItem, true);
        }
    }
}

void Space::setupGroups(QMap<GroupItemResult*, GroupItem*> groupForResult)
{
    for(auto it = groupForResult.begin(); it != groupForResult.end(); ++it)
    {
        GroupItemResult* result = it.key();
        GroupItem* group = it.value();

        for(int itemHandle : result->itemsHandles())
            group->append(m_items.getItem(itemHandle));
    }
}

void Space::setupHyperBase(SpaceLoader* spaceLoader)
{
    //    hyperBase = make_shared<HyperBase>(spaceLoader->spaceHyperData());
    auto hyperBase = HyperBase(spaceLoader->spaceHyperData());
    m_hyperKey = hyperBase.getSpaceHyperKey();

    auto itemsHyperDataMap = spaceLoader->itemsHyperData();
    if(!itemsHyperDataMap.isEmpty())
    {
        for(auto it = itemsHyperDataMap.begin(); it != itemsHyperDataMap.end(); ++it)
        {
            if(SpaceItem* item = getSpaceItem(it.key()))
            {
                hyperBase = HyperBase(spaceLoader->spaceHyperData());
                item->setHyperKey(hyperBase.getSpaceItemHyperKey());
            }
        }
    }
}

void Space::controlsToTop()
{
    for(SpaceItem* controlItem : m_controlItems)
    {
        m_zOrderList.removeOne(controlItem);
        m_zOrderList << controlItem;
    }
}

Space* Space::createFromScheme(Scheme* scheme)
{
    if(!scheme)
        return nullptr;
    
    St2000DataStream stream(scheme->spaceData());
    Space* space = new Space(scheme);
    if(!space->load(stream))
    {
        delete space;
        space = nullptr;
    }
    return space;
}

Space* Space::createFromFile(const QString& fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly))
    {
        St2000DataStream stream(&file);
        Space* space = new Space(fileName);
        if(!space->load(stream))
        {
            delete space;
            space = nullptr;
        }
        return space;
    }
    return nullptr;
}

}
