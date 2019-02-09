#include "JsonHyperKeySerializer.h"
#include "HyperBase.h"

using namespace StData;

namespace StSerialization{

JsonHyperKeySerializer::JsonHyperKeySerializer()
{}

QJsonObject JsonHyperKeySerializer::serialize(const SpaceItemHyperKeyPtr& hyperKey)
{
    QJsonObject object;

    object[KEY_TARGET] = hyperKey->target();
    object[KEY_WINDOW_NAME] = hyperKey->windowName();
    object[KEY_OBJECT] = hyperKey->object();
    object[KEY_PARAMS] = hyperKey->params();
    object[KEY_OPEN_MODE] = (int)hyperKey->openMode();
    object[KEY_EFFECT] = hyperKey->effect();
    object[KEY_TIME] = hyperKey->time();
    object[KEY_DISABLED] = hyperKey->disabled();

    return object;
}

}
