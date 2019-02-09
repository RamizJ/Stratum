#ifndef JSONHYPERKEYSERIALIZER_H
#define JSONHYPERKEYSERIALIZER_H

#include <QJsonObject>
#include <StDataTypes.h>

namespace StSerialization{

#define KEY_TARGET "Target"
#define KEY_WINDOW_NAME "WindowName"
#define KEY_OBJECT "Object"
#define KEY_PARAMS "Params"
#define KEY_OPEN_MODE "OpenMode"
#define KEY_EFFECT "Effect"
#define KEY_TIME "Time"
#define KEY_DISABLED "Disabled"

class JsonHyperKeySerializer
{
public:
    JsonHyperKeySerializer();

    QJsonObject serialize(const StData::SpaceItemHyperKeyPtr& hyperKey);
};

}

#endif // JSONHYPERKEYSERIALIZER_H
