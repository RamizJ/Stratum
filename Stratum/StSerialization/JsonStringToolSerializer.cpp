#include "JsonStringToolSerializer.h"

#include <Tool2d.h>

using namespace StData;

namespace StSerialization {

JsonStringToolSerializer::JsonStringToolSerializer() :
    JsonToolSerializer()
{}

QJsonObject JsonStringToolSerializer::serialize(StringTool* tool)
{
    QJsonObject result = JsonToolSerializer::serialize(tool);

    result[KEY_STRING_TOOL_DATA] = tool->string();

    return result;
}

StringTool* JsonStringToolSerializer::deserialize(const QJsonObject& toolData)
{
    auto tool = new StringTool;

    tool->setHandle(toolData.value(KEY_TOOL_HANDLE).toInt());
    tool->setString(toolData.value(KEY_STRING_TOOL_DATA).toString());

    return tool;
}

}
