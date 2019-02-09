#include "JsonTextureItemSerializer.h"

#include <SpaceItem.h>
#include <Tool2d.h>

using namespace StData;

namespace StSerialization {

JsonTextureItemSerializer::JsonTextureItemSerializer() :
    JsonSpaceItem2dSerializer()
{}

QJsonObject JsonTextureItemSerializer::serialize(StData::TextureItem2d* item)
{
    QJsonObject itemData = JsonSpaceItem2dSerializer::serialize(item);

    QPointF viewportOrigin = item->textureOrigin();
    QSizeF viewportSize = item->textureSize();

    itemData[KEY_TEXTUREITEM_TEXTUREHANDLE] = item->texture()->handle();
    itemData[KEY_TEXTUREITEM_VIEWPORTORIGIN] = pointToString(viewportOrigin);
    itemData[KEY_TEXTUREITEM_VIEWPORTSIZE] = sizeToString(viewportSize);

    return itemData;
}

TextureItem2d* JsonTextureItemSerializer::deserialize(const QJsonObject& itemData)
{
    if(itemData.isEmpty()) return nullptr;

    auto item = new TextureItem2d(nullptr);
    JsonSpaceItem2dSerializer::deserialize(itemData, item);

    QPointF textureOrigin = pointFromString(itemData.value(KEY_TEXTUREITEM_VIEWPORTORIGIN).toString());
    QSizeF textureSize = sizeFromString(itemData.value(KEY_TEXTUREITEM_VIEWPORTSIZE).toString());
    int textureHandle = itemData.value(KEY_TEXTUREITEM_TEXTUREHANDLE).toInt();

    item->setTextureOrigin(textureOrigin);
    item->setTextureSize(textureSize);
    item->setTextureHandle(textureHandle);

    return item;
}

}
