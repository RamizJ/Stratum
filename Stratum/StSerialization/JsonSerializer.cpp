#include "JsonSerializer.h"
#include "JsonToolSerializer.h"
#include "JsonBrushToolSerializer.h"
#include "JsonFontToolSerializer.h"
#include "JsonGroupItemSerializer.h"
#include "JsonPenToolSerializer.h"
#include "JsonPolylineItemSerializer.h"
#include "JsonRasterTextToolSerializer.h"
#include "JsonStringToolSerializer.h"
#include "JsonTextItemSerializer.h"
#include "JsonTextureItemSerializer.h"
#include "JsonTextureToolSerializer.h"
#include "ToolHelper.h"

#include <PolylineArrow.h>
#include <QBuffer>
#include <QJsonArray>
#include <QJsonDocument>
#include <Space.h>
#include <SpaceItem.h>
#include <Tool2d.h>

#include <QtGui/QBitmap>
#include <QtGui/QPen>

using namespace StData;

namespace StSerialization {

JsonSerializer::JsonSerializer(bool compactMode):
    m_compactMode(compactMode)
{}

QByteArray JsonSerializer::serialize(StData::SpaceItem* spaceItem)
{
    QList<Tool*> tools;
    collectTools(spaceItem, tools);

    QJsonArray toolsData;
    for(Tool* tool : tools)
        toolsData.append(serializeTool(tool));

    QJsonObject spaceItemData = serializeItem(spaceItem);

    QJsonObject serializedData;
    serializedData[KEY_SERIALIZER_SPACE_DIR] = spaceItem->space()->spaceDir().absolutePath();
    serializedData[KEY_SERIALIZER_TOOLS] = toolsData;
    serializedData[KEY_SERIALIZER_SPACE_ITEM] = spaceItemData;

    QJsonDocument jsonDocument(serializedData);

    return jsonDocument.toJson(m_compactMode ? QJsonDocument::Compact : QJsonDocument::Indented);
}

SpaceItem* JsonSerializer::deserialize(const QByteArray& data, QList<Tool*>& tools)
{
    if(data.isEmpty()) return nullptr;

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject serializedData = doc.object();

    QString spaceDirPath = serializedData.value(KEY_SERIALIZER_SPACE_DIR).toString();
    QJsonObject spaceItemData = serializedData.value(KEY_SERIALIZER_SPACE_ITEM).toObject();
    QJsonArray toolsData = serializedData.value(KEY_SERIALIZER_TOOLS).toArray();

    QDir spaceDir(spaceDirPath);
    SpaceItem* spaceItem = deserializeItem(spaceItemData);
    tools = deserializeTools(toolsData, spaceDir);

    initializeTools(tools);
    initializeItem(spaceItem, tools);

    return spaceItem;
}

QJsonObject JsonSerializer::serializeItem(SpaceItem* item)
{
    QJsonObject result;
    if(auto polylineItem = dynamic_cast<PolylineItem2d*>(item))
        result = JsonPolylineItemSerializer().serialize(polylineItem);

    else if(auto textureItem = dynamic_cast<TextureItem2d*>(item))
        result = JsonTextureItemSerializer().serialize(textureItem);

    else if(auto textItem = dynamic_cast<TextItem2d*>(item))
        result = JsonTextItemSerializer().serialize(textItem);

    else if(auto groupItem = dynamic_cast<GroupItem*>(item))
        result = JsonGroupItemSerializer().serialize(groupItem);

    return result;
}

QJsonObject JsonSerializer::serializeTool(Tool* tool)
{
    QJsonObject result;

    if(auto penTool = dynamic_cast<PenTool*>(tool))
        result = JsonPenToolSerializer().serialize(penTool);

    else if(auto brushTool = dynamic_cast<BrushTool*>(tool))
        result = JsonBrushToolSerializer().serialize(brushTool);

    else if(auto textureTool = dynamic_cast<TextureTool*>(tool))
        result = JsonTextureToolSerializer().serialize(textureTool);

    else if(auto rasterTextTool = dynamic_cast<RasterTextTool*>(tool))
        result = JsonRasterTextToolSerializer().serialize(rasterTextTool);

    else if(auto fontTool = dynamic_cast<FontTool*>(tool))
        result = JsonFontToolSerializer().serialize(fontTool);

    else if(auto stringTool = dynamic_cast<StringTool*>(tool))
        result = JsonStringToolSerializer().serialize(stringTool);

    return result;
}

SpaceItem* JsonSerializer::deserializeItem(const QJsonObject& itemData)
{
    QString itemDataType = itemData.value(KEY_ITEM_TYPE).toString();

    if(itemDataType == ITEM_TYPE_POLYLINE)
        return JsonPolylineItemSerializer().deserialize(itemData);

    if(itemDataType == ITEM_TYPE_TEXTURE || itemDataType == ITEM_TYPE_MASKED_TEXTURE)
        return JsonTextureItemSerializer().deserialize(itemData);

    if(itemDataType == ITEM_TYPE_TEXT)
        return JsonTextItemSerializer().deserialize(itemData);

    if(itemDataType == ITEM_TYPE_GROUP)
        return JsonGroupItemSerializer().deserialize(itemData);

    return nullptr;
}

QList<Tool*> JsonSerializer::deserializeTools(const QJsonArray& toolData, const QDir& spaceDir)
{
    QList<Tool*> tools;
    for(QJsonValue toolDataItem : toolData)
    {
        QJsonObject toolData =  toolDataItem.toObject();
        int toolType = toolData.value(KEY_TOOL_TYPE).toInt();

        if(toolType == Tool::Pen)
        {
            tools << JsonPenToolSerializer().deserialize(toolData);
        }
        else if(toolType == Tool::Brush)
        {
            tools << JsonBrushToolSerializer().deserialize(toolData);
        }
        else if(toolType == Tool::Texture || toolType == Tool::MaskedTexture)
        {
            tools << JsonTextureToolSerializer().deserialize(toolData, spaceDir);
        }
        else if(toolType == Tool::Text)
        {
            tools << JsonRasterTextToolSerializer().deserialize(toolData);
        }
        else if(toolType == Tool::Font)
        {
            tools << JsonFontToolSerializer().deserialize(toolData);
        }
        else if(toolType == Tool::String)
        {
            tools << JsonStringToolSerializer().deserialize(toolData);
        }
    }
    return tools;
}

void JsonSerializer::collectTools(SpaceItem* spaceItem, QList<Tool*>& tools)
{
    if(auto item = dynamic_cast<PolylineItem2d*>(spaceItem))
    {
        addTool(item->penTool(), tools);
        addTool(item->brushTool(), tools);
        if(item->brushTool() && item->brushTool()->texture())
            addTool(item->brushTool()->texture(), tools);
    }
    else if(auto item = dynamic_cast<TextureItem2d*>(spaceItem))
    {
        addTool(item->texture(), tools);
    }
    else if(auto item = dynamic_cast<TextItem2d*>(spaceItem))
    {
        auto rasterText = item->rasterText();
        for(auto textFragmet : rasterText->textFragments())
        {
            addTool(textFragmet->fontTool(), tools);
            addTool(textFragmet->stringTool(), tools);
        }
        addTool(rasterText, tools);
    }
    else if(auto item = dynamic_cast<GroupItem*>(spaceItem))
    {
        for(auto childItem : item->items())
            collectTools(childItem, tools);
    }
}

void JsonSerializer::addTool(Tool* tool, QList<Tool*>& tools)
{
    if(tool && !tools.contains(tool))
        tools.append(tool);
}

void JsonSerializer::initializeTools(const QList<Tool*>& tools)
{
    for(Tool* tool : tools)
    {
        if(auto brushTool = dynamic_cast<BrushTool*>(tool))
        {
            if(brushTool->textureHandle() != 0)
                brushTool->setTexture(findTool<TextureTool>(tools, brushTool->textureHandle()));
        }
        else if(auto rasterTextTool = dynamic_cast<RasterTextTool*>(tool))
        {
            for(TextFragment* textFragment : rasterTextTool->textFragments())
            {
                textFragment->setFont(findTool<FontTool>(tools, textFragment->fontHandle()));
                textFragment->setString(findTool<StringTool>(tools, textFragment->stringHandle()));
            }
        }
    }
}

void JsonSerializer::initializeItem(SpaceItem* item, const QList<Tool*>& tools)
{
    if(auto polylineItem = dynamic_cast<PolylineItem2d*>(item))
    {
        polylineItem->setPenTool(findTool<PenTool>(tools, polylineItem->penHandle()));
        polylineItem->setBrushTool(findTool<BrushTool>(tools, polylineItem->brushHandle()));
    }
    else if(auto textureItem = dynamic_cast<TextureItem2d*>(item))
    {
        textureItem->setTexture(findTool<TextureTool>(tools, textureItem->textureHandle()));
    }
    else if(auto textItem = dynamic_cast<TextItem2d*>(item))
    {
       textItem->setRasterText(findTool<RasterTextTool>(tools, textItem->rasterTextHandle()));
    }
    else if(auto groupItem = dynamic_cast<GroupItem*>(item))
    {
        for(SpaceItem* childGroupItem : groupItem->items())
            initializeItem(childGroupItem, tools);
    }
}

}
