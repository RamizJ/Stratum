#ifndef JSONTEXTITEMSERIALIZER_H
#define JSONTEXTITEMSERIALIZER_H

#include <QJsonObject>
#include <JsonSpaceItem2dSerializer.h>

namespace StData {
class TextItem2d;
}

namespace StSerialization {

#define KEY_TEXTITEM_ANGLE "TextAngle"
#define KEY_TEXTITEM_TEXTTOOL "RasterText"

class JsonTextItemSerializer : JsonSpaceItem2dSerializer
{
public:
    JsonTextItemSerializer();

    QJsonObject serialize(StData::TextItem2d* item);
    StData::TextItem2d* deserialize(const QJsonObject& itemData);
};

}

#endif // JSONTEXTITEMSERIALIZER_H
