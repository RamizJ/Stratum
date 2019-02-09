#ifndef JSONTOOLSERIALIZER_H
#define JSONTOOLSERIALIZER_H

#include "StDataTypes.h"
#include <QJsonObject>

namespace StSerialization {

#define KEY_TOOL_TYPE "ToolType"
#define KEY_TOOL_HANDLE "Handle"

class JsonToolSerializer
{
public:
    JsonToolSerializer();
    virtual ~JsonToolSerializer();

protected:
    QJsonObject serialize(StData::Tool* tool);
};

}

#endif // JSONTOOLSERIALIZER_H
