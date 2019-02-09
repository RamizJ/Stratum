#ifndef JSONFONTTOOLSERIALIZER_H
#define JSONFONTTOOLSERIALIZER_H

#include "JsonToolSerializer.h"

#include <QJsonObject>


namespace StData {
class FontTool;
}

namespace StSerialization {

#define KEY_TOOL_FONT_SIZEINPIXELS "SizeInPixels"
#define KEY_TOOL_FONT_DATA "FontData"

class JsonFontToolSerializer : JsonToolSerializer
{
public:
    JsonFontToolSerializer();

    QJsonObject serialize(StData::FontTool* tool);
    StData::FontTool* deserialize(const QJsonObject& toolData);
};

}

#endif // JSONFONTTOOLSERIALIZER_H
