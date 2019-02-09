#ifndef JSONSTRINGTOOLSERIALIZER_H
#define JSONSTRINGTOOLSERIALIZER_H

#include "JsonToolSerializer.h"

#include <QJsonObject>

namespace StData {
class StringTool;
}

namespace StSerialization {

#define KEY_STRING_TOOL_DATA "String"

class JsonStringToolSerializer : JsonToolSerializer
{
public:
    JsonStringToolSerializer();

    QJsonObject serialize(StData::StringTool* tool);
    StData::StringTool* deserialize(const QJsonObject& toolData);
};

}

#endif // JSONSTRINGTOOLSERIALIZER_H
