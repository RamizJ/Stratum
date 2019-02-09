#ifndef JSONTEXTURETOOLSERIALIZER_H
#define JSONTEXTURETOOLSERIALIZER_H

#include "JsonToolSerializer.h"

#include <QDir>
#include <QJsonObject>

namespace StData {
class TextureTool;
}

namespace StSerialization {

#define KEY_TEXTURE_PATH "Path"
#define KEY_TEXTURE_PIXMAP "Pixmap"
#define KEY_TEXTURE_MASK "Mask"

class JsonTextureToolSerializer : JsonToolSerializer
{
public:
    JsonTextureToolSerializer();

    QJsonObject serialize(StData::TextureTool* tool);
    StData::TextureTool* deserialize(const QJsonObject& toolData, const QDir& spaceDir);
};

}

#endif // JSONTEXTURETOOLSERIALIZER_H
