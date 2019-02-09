#ifndef JSONTEXTUREITEMSERIALIZER_H
#define JSONTEXTUREITEMSERIALIZER_H

#include <QJsonObject>
#include <JsonSpaceItem2dSerializer.h>

namespace StData {
class TextureItem2d;
}

namespace StSerialization {

#define KEY_TEXTUREITEM_TEXTUREHANDLE "TextureHandle"
#define KEY_TEXTUREITEM_VIEWPORTORIGIN "ViewportOrigin"
#define KEY_TEXTUREITEM_VIEWPORTSIZE "ViewportSize"

class JsonTextureItemSerializer : JsonSpaceItem2dSerializer
{
public:
    JsonTextureItemSerializer();

    QJsonObject serialize(StData::TextureItem2d* item);
    StData::TextureItem2d* deserialize(const QJsonObject& itemData);
};

}

#endif // JSONTEXTUREITEMSERIALIZER_H
