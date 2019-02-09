#ifndef JSONSERIALIZER_H
#define JSONSERIALIZER_H

#include <stserialization_global.h>

#include <QDir>
#include <QJsonObject>
#include <QList>

namespace StData {
class SpaceItem;
class PolylineItem2d;
class RasterTextItem;
class TextureItem2d;
class TextItem2d;
class GroupItem;

class Tool;
class PenTool;
class BrushTool;
class TextureTool;
class RasterTextTool;
class FontTool;
class StringTool;
}

namespace StSerialization {

#define KEY_SERIALIZER_TOOLS "Tools"
#define KEY_SERIALIZER_SPACE_ITEM "SpaceItem"
#define KEY_SERIALIZER_SPACE_DIR "SpaceDir"

class STSERIALIZATIONSHARED_EXPORT JsonSerializer
{
public:
    enum SpaceItemType{Polyline = 1, Image, MaskedImage, Text, Cotrol, Group };

public:
    JsonSerializer(bool compactMode = true);
    QByteArray serialize(StData::SpaceItem* spaceItem);
    StData::SpaceItem* deserialize(const QByteArray& data, QList<StData::Tool*>& tools);

    bool compactMode() const { return m_compactMode; }
    void setCompactMode(bool compactMode) { m_compactMode = compactMode; }

private:
    QJsonObject serializeItem(StData::SpaceItem* item);
    QJsonObject serializeTool(StData::Tool* tool);

    StData::SpaceItem* deserializeItem(const QJsonObject& itemData);
    QList<StData::Tool*> deserializeTools(const QJsonArray& toolData, const QDir& spaceDir);

    void collectTools(StData::SpaceItem* spaceItem, QList<StData::Tool*>& tools);
    void addTool(StData::Tool* tool, QList<StData::Tool*>& tools);

    void initializeTools(const QList<StData::Tool*>& tools);
    void initializeItem(StData::SpaceItem* item, const QList<StData::Tool*>& tools);

private:
    bool m_compactMode;
};

}

#endif // JSONSPACESERIALIZER_H
