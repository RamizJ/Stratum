#include "JsonFontToolSerializer.h"

#include <Tool2d.h>

using namespace StData;

namespace StSerialization {

JsonFontToolSerializer::JsonFontToolSerializer() :
    JsonToolSerializer()
{}

QJsonObject JsonFontToolSerializer::serialize(FontTool* tool)
{
    QJsonObject result = JsonToolSerializer::serialize(tool);

    result[KEY_TOOL_FONT_SIZEINPIXELS] = tool->sizeInPixels();
    result[KEY_TOOL_FONT_DATA] = tool->font().toString();

    return result;
}

FontTool* JsonFontToolSerializer::deserialize(const QJsonObject& toolData)
{
    FontTool* tool = new FontTool;

    tool->setHandle(toolData.value(KEY_TOOL_HANDLE).toInt());

    QFont font;
    font.fromString(toolData.value(KEY_TOOL_FONT_DATA).toString());
    tool->setFont(font);

    return tool;
}

}
