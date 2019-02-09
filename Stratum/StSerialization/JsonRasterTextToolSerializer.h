#ifndef JSONRASTERTEXTTOOLSERIALIZER_H
#define JSONRASTERTEXTTOOLSERIALIZER_H

#include "JsonToolSerializer.h"

#include <QJsonObject>

namespace StData {
class RasterTextTool;
}

namespace StSerialization {

#define KEY_TEXT_TOOL_FRAGMENTS "Fragments"
#define KEY_TEXT_TOOL_BG "Bg"
#define KEY_TEXT_TOOL_FG "Fg"
#define KEY_TEXT_TOOL_FONT_HANDLE "Font"
#define KEY_TEXT_TOOL_STRING_HANDLE "String"

class JsonRasterTextToolSerializer : JsonToolSerializer
{
public:
    JsonRasterTextToolSerializer() {}

    QJsonObject serialize(StData::RasterTextTool* tool);
    StData::RasterTextTool* deserialize(const QJsonObject& toolData);
};

}

#endif // JSONRASTERTEXTTOOLSERIALIZER_H
