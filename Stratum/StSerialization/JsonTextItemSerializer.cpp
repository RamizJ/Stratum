#include "JsonTextItemSerializer.h"

#include <SpaceItem.h>
#include <Tool2d.h>

using namespace StData;

namespace StSerialization {

JsonTextItemSerializer::JsonTextItemSerializer() :
    JsonSpaceItem2dSerializer()
{}

QJsonObject JsonTextItemSerializer::serialize(TextItem2d* item)
{
    QJsonObject result = JsonSpaceItem2dSerializer::serialize(item);

    result[KEY_TEXTITEM_ANGLE] = item->textAngle();
    result[KEY_TEXTITEM_TEXTTOOL] = item->rasterText()->handle();

    return result;
}

TextItem2d* JsonTextItemSerializer::deserialize(const QJsonObject& itemData)
{
    if(itemData.isEmpty()) return nullptr;

    auto item = new TextItem2d(nullptr);

    JsonSpaceItem2dSerializer::deserialize(itemData, item);

    item->setTextAngle(itemData.value(KEY_TEXTITEM_ANGLE).toDouble());
    item->setRasterTextHandle(itemData.value(KEY_TEXTITEM_TEXTTOOL).toInt());

    return item;
}

}
