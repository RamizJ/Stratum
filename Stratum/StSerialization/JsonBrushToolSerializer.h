#ifndef JSONBRUSHTOOLSERIALIZER_H
#define JSONBRUSHTOOLSERIALIZER_H

#include "JsonToolSerializer.h"
#include <QJsonObject>

namespace StData {
class BrushTool;
}

namespace StSerialization {

#define KEY_BRUSH_RGBA "Rgba"
#define KEY_BRUSH_STYLE "Style"
#define KEY_BRUSH_HATCH "Hatch"
#define KEY_BRUSH_ROP "Rop"
#define KEY_BRUSH_TEXTURE_HANDLE "TextureHandle"

class JsonBrushToolSerializer : JsonToolSerializer
{
public:
    JsonBrushToolSerializer() {}

    QJsonObject serialize(StData::BrushTool* tool);
    StData::BrushTool* deserialize(const QJsonObject& toolData);
};

}

#endif // JSONBRUSHTOOLSERIALIZER_H
