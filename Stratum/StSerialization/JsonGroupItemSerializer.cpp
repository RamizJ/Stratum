#include "JsonGroupItemSerializer.h"

#include "JsonPolylineItemSerializer.h"
#include "JsonTextItemSerializer.h"
#include "JsonTextureItemSerializer.h"

#include <QJsonArray>
#include <SpaceItem.h>

using namespace StData;

namespace StSerialization {

JsonGroupItemSerializer::JsonGroupItemSerializer() :
    JsonSpaceItemSerializer()
{}

QJsonObject JsonGroupItemSerializer::serialize(StData::GroupItem* item)
{
    QJsonObject itemData = JsonSpaceItemSerializer::serialize(item);

    QJsonArray childsData;
    for(SpaceItem* child : item->items())
    {
        if(auto polylineItem = dynamic_cast<PolylineItem2d*>(child))
            childsData.append(JsonPolylineItemSerializer().serialize(polylineItem));

        else if(auto textureItem = dynamic_cast<TextureItem2d*>(child))
            childsData.append(JsonTextureItemSerializer().serialize(textureItem));

        else if(auto textItem = dynamic_cast<TextItem2d*>(child))
            childsData.append(JsonTextItemSerializer().serialize(textItem));

        else if(auto groupItem = dynamic_cast<GroupItem*>(child))
            childsData.append(JsonGroupItemSerializer().serialize(groupItem));
    }
    itemData[KEY_GROUPITEM_ITEMS] = childsData;

    return itemData;
}

GroupItem* JsonGroupItemSerializer::deserialize(const QJsonObject& itemData)
{
    if(itemData.isEmpty()) return nullptr;

    GroupItem* groupItem = new GroupItem(nullptr);
    JsonSpaceItemSerializer::deserialize(itemData, groupItem);

    QJsonArray itemsData = itemData.value(KEY_GROUPITEM_ITEMS).toArray();
    for(QJsonValue childValue : itemsData)
    {
        QJsonObject childData = childValue.toObject();
        QString itemTypeName = childData.value(KEY_ITEM_TYPE).toString();

        if(itemTypeName == ITEM_TYPE_POLYLINE)
        {
            PolylineItem2d* polylineItem = JsonPolylineItemSerializer().deserialize(childData);
            groupItem->append(polylineItem);
        }
        else if(itemTypeName == ITEM_TYPE_TEXTURE || itemTypeName == ITEM_TYPE_MASKED_TEXTURE)
        {
            TextureItem2d* textureItem = JsonTextureItemSerializer().deserialize(childData);
            groupItem->append(textureItem);
        }
        else if(itemTypeName == ITEM_TYPE_TEXT)
        {
            TextItem2d* textItem = JsonTextItemSerializer().deserialize(childData);
            groupItem->append(textItem);
        }
        else if(itemTypeName == ITEM_TYPE_GROUP)
        {
            GroupItem* childGroupItem = JsonGroupItemSerializer().deserialize(childData);
            groupItem->append(childGroupItem);
        }
    }

    return groupItem;
}

}
