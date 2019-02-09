#include "JsonBrushToolSerializer.h"

#include <Tool2d.h>

using namespace StData;

namespace StSerialization {

QJsonObject JsonBrushToolSerializer::serialize(BrushTool* tool)
{
    QJsonObject result = JsonToolSerializer::serialize(tool);

    result[KEY_BRUSH_RGBA] = tool->rgba();
    result[KEY_BRUSH_STYLE] = tool->style();
    result[KEY_BRUSH_HATCH] = tool->hatch();
    result[KEY_BRUSH_ROP] = tool->rop();

    if(tool->texture() != nullptr)
        result[KEY_BRUSH_TEXTURE_HANDLE] = tool->texture()->handle();

    return result;
}

BrushTool* JsonBrushToolSerializer::deserialize(const QJsonObject& toolData)
{
    auto brushTool = new BrushTool;

    brushTool->setHandle(toolData.value(KEY_TOOL_HANDLE).toInt());
    brushTool->setHatch(toolData.value(KEY_BRUSH_HATCH).toInt());
    brushTool->setRgba(toolData.value(KEY_BRUSH_RGBA).toInt());
    brushTool->setRop(toolData.value(KEY_BRUSH_ROP).toInt());
    brushTool->setStyle(toolData.value(KEY_BRUSH_STYLE).toInt());
    brushTool->setTextureHandle(toolData.value(KEY_BRUSH_TEXTURE_HANDLE).toInt());

    return brushTool;
}

}
