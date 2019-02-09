#include "JsonHyperKeySerializer.h"
#include "JsonSpaceItemSerializer.h"

#include <HyperBase.h>
#include <SpaceItem.h>

using namespace StData;

namespace StSerialization {

JsonSpaceItemSerializer::JsonSpaceItemSerializer(){}

JsonSpaceItemSerializer::~JsonSpaceItemSerializer(){}

QJsonObject JsonSpaceItemSerializer::serialize(SpaceItem* item)
{
    QJsonObject itemData;
    itemData[KEY_ITEM_TYPE] = getItemTypeName(item);
    itemData[KEY_ITEM_SPACE_TYPE] = item->typeOnSpace();
    itemData[KEY_NAME] = item->name();
    itemData[KEY_OPTIONS] = item->options();
    itemData[KEY_OPACITY] = item->opacity();
    itemData[KEY_LAYER] = item->layer();
    itemData[KEY_IS_VISIBLE] = item->isItemVisible();
    itemData[KEY_LOADING_TYPE] = item->loadingType();
//    result[KEY_Z_ORDER] = item->zOrder();

    if(auto hyperKey = item->hyperKey())
        itemData[KEY_HYPERKEY] = JsonHyperKeySerializer().serialize(hyperKey);

    return itemData;
}

void JsonSpaceItemSerializer::deserialize(const QJsonObject& itemData, SpaceItem* item)
{
    if(item == nullptr || itemData.isEmpty()) return;

    item->setName(itemData.value(KEY_NAME).toString());
    item->setTypeOnSpace((SpaceItem::Type)itemData.value(KEY_ITEM_SPACE_TYPE).toInt());
    item->setOptions(itemData.value(KEY_OPTIONS).toInt());
    item->setOpacity(itemData.value(KEY_OPACITY).toInt());
    item->setLayer(itemData.value(KEY_LAYER).toInt());
    item->setItemVisible(itemData.value(KEY_IS_VISIBLE).toBool());
    item->setLoadingType(itemData.value(KEY_LOADING_TYPE).toInt());
}

QString JsonSpaceItemSerializer::getItemTypeName(SpaceItem* item)
{
    if(dynamic_cast<PolylineItem2d*>(item)) return ITEM_TYPE_POLYLINE;
    else if(dynamic_cast<TextureItem2d*>(item)) return ITEM_TYPE_TEXTURE;
    else if(dynamic_cast<TextItem2d*>(item)) return ITEM_TYPE_TEXT;
    else if(dynamic_cast<ControlItem2d*>(item)) return ITEM_TYPE_CONTROL;
    else if(dynamic_cast<GroupItem*>(item)) return ITEM_TYPE_GROUP;

    return "";
}

}
