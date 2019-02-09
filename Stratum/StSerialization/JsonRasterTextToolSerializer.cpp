#include "JsonRasterTextToolSerializer.h"

#include <QJsonArray>
#include <Tool2d.h>

using namespace StData;

namespace StSerialization {

QJsonObject JsonRasterTextToolSerializer::serialize(StData::RasterTextTool* tool)
{
    QJsonObject toolData = JsonToolSerializer::serialize(tool);

    QJsonArray textFragmentsData;
    for(TextFragment* fragment : tool->textFragments())
    {
        QJsonObject textFragmentData;
        textFragmentData[KEY_TEXT_TOOL_BG] = (int)fragment->bgColor().rgba();
        textFragmentData[KEY_TEXT_TOOL_FG] = (int)fragment->fgColor().rgba();
        textFragmentData[KEY_TEXT_TOOL_FONT_HANDLE] = fragment->fontHandle();
        textFragmentData[KEY_TEXT_TOOL_STRING_HANDLE] = fragment->stringHandle();

        textFragmentsData.append(textFragmentData);
    }
    toolData[KEY_TEXT_TOOL_FRAGMENTS] = textFragmentsData;

    return toolData;
}

RasterTextTool* JsonRasterTextToolSerializer::deserialize(const QJsonObject& toolData)
{
    RasterTextTool* tool = new RasterTextTool;

    tool->setHandle(toolData.value(KEY_TOOL_HANDLE).toInt());

    QJsonArray fragmentsData = toolData.value(KEY_TEXT_TOOL_FRAGMENTS).toArray();
    for(QJsonValue fragmentValue : fragmentsData)
    {
        QJsonObject fragmentData = fragmentValue.toObject();
        QColor bgColor = QColor::fromRgba(fragmentData.value(KEY_TEXT_TOOL_BG).toInt());
        QColor fgColor = QColor::fromRgba(fragmentData.value(KEY_TEXT_TOOL_FG).toInt());
        int fontHandle = fragmentData.value(KEY_TEXT_TOOL_FONT_HANDLE).toInt();
        int stringHandle = fragmentData.value(KEY_TEXT_TOOL_STRING_HANDLE).toInt();

        TextFragment* fragment = new TextFragment;
        fragment->setBgColor(bgColor);
        fragment->setFgColor(fgColor);
        fragment->setFontHandle(fontHandle);
        fragment->setStringHandle(stringHandle);

        tool->append(fragment);
    }

    return tool;
}

}
