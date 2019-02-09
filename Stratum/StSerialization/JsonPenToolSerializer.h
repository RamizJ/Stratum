#ifndef JSONPENTOOLSERIALIZER_H
#define JSONPENTOOLSERIALIZER_H

#include "JsonToolSerializer.h"

#include <QJsonObject>

namespace StData {
class PenTool;
}

namespace StSerialization {

#define KEY_PEN_RGBA "Rgba"
#define KEY_PEN_WIDTH "Width"
#define KEY_PEN_STYLE "Style"
#define KEY_PEN_ROP "Rop"

class JsonPenToolSerializer : JsonToolSerializer
{
public:
    JsonPenToolSerializer();

    QJsonObject serialize(StData::PenTool* tool);
    StData::PenTool* deserialize(const QJsonObject& tool);
};

}

#endif // JSONPENTOOLSERIALIZER_H
