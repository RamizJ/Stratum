#ifndef JSONSPACEITEMSERIALIZER_H
#define JSONSPACEITEMSERIALIZER_H

#include <QJsonObject>
#include <StDataTypes.h>

namespace StData {
class SpaceItem;
}

namespace StSerialization {

#define ITEM_TYPE_POLYLINE "Polyline"
#define ITEM_TYPE_TEXTURE "Texture"
#define ITEM_TYPE_MASKED_TEXTURE "MaskedTexture"
#define ITEM_TYPE_TEXT "Text"
#define ITEM_TYPE_CONTROL "Control"
#define ITEM_TYPE_GROUP "Group"

#define KEY_ITEM_HANDLE "Handle"
#define KEY_ITEM_TYPE "ItemType"
#define KEY_ITEM_SPACE_TYPE "SpaceType"
#define KEY_NAME "Name"
#define KEY_OPTIONS "Options"
#define KEY_OPACITY "Opacity"
#define KEY_LAYER "Layer"
#define KEY_IS_VISIBLE "IsVisible"
#define KEY_LOADING_TYPE "LoadingType"
#define KEY_Z_ORDER "ZOrder"
#define KEY_HYPERKEY "HyperKey"

class JsonSpaceItemSerializer
{
public:
    JsonSpaceItemSerializer();
    virtual ~JsonSpaceItemSerializer();

protected:
    QJsonObject serialize(StData::SpaceItem* item);
    void deserialize(const QJsonObject& itemData, StData::SpaceItem* item);

private:
    QString getItemTypeName(StData::SpaceItem* item);
};

}

#endif // JSONSPACEITEMSERIALIZER_H
