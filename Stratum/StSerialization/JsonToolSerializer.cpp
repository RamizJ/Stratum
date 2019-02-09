#include "JsonToolSerializer.h"

#include <Tool2d.h>

namespace StSerialization {

JsonToolSerializer::JsonToolSerializer()
{}

JsonToolSerializer::~JsonToolSerializer()
{}

QJsonObject JsonToolSerializer::serialize(StData::Tool* tool)
{
    QJsonObject toolData;

    toolData[KEY_TOOL_TYPE] = tool->toolType();
    toolData[KEY_TOOL_HANDLE] = tool->handle();

    return toolData;
}

}
