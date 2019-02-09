#include "JsonPenToolSerializer.h"

#include <Tool2d.h>

using namespace StData;

namespace StSerialization {

JsonPenToolSerializer::JsonPenToolSerializer() :
    JsonToolSerializer()
{}

QJsonObject JsonPenToolSerializer::serialize(PenTool* tool)
{
    QJsonObject penToolData = JsonToolSerializer::serialize(tool);

    penToolData[KEY_PEN_RGBA] = tool->rgba();
    penToolData[KEY_PEN_WIDTH] = tool->width();
    penToolData[KEY_PEN_STYLE] = tool->style();
    penToolData[KEY_PEN_ROP] = tool->rop();

    return penToolData;
}

PenTool* JsonPenToolSerializer::deserialize(const QJsonObject& tool)
{
    auto penTool = new PenTool();

    penTool->setHandle(tool.value(KEY_TOOL_HANDLE).toInt());
    penTool->setRgba(tool.value(KEY_PEN_RGBA).toInt());
    penTool->setRop(tool.value(KEY_PEN_ROP).toInt());
    penTool->setStyle(tool.value(KEY_PEN_STYLE).toInt());
    penTool->setWidth(tool.value(KEY_PEN_WIDTH).toDouble());

    return penTool;
}

}
