#ifndef JSONGROUPITEMSERIALIZER_H
#define JSONGROUPITEMSERIALIZER_H

#include "JsonSpaceItemSerializer.h"

#include <StDataTypes.h>

namespace StSerialization {

#define KEY_GROUPITEM_ITEMS "Items"

class JsonGroupItemSerializer : JsonSpaceItemSerializer
{
public:
    JsonGroupItemSerializer();

    QJsonObject serialize(StData::GroupItem* item);
    StData::GroupItem* deserialize(const QJsonObject& itemData);
};

}

#endif // JSONGROUPITEMSERIALIZER_H
