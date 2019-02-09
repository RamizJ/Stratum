#ifndef JSONSPACEITEM2DSERIALIZER_H
#define JSONSPACEITEM2DSERIALIZER_H

#include <JsonSpaceItemSerializer.h>
#include <QPointF>
#include <QSizeF>

namespace StData {
class SpaceItem2d;
}

namespace StSerialization {

#define KEY_SPACEITEM2D_ORIGIN "Origin"
#define KEY_SPACEITEM2D_SIZE "Size"
#define KEY_SPACEITEM2D_ANGLE "Angle"

class JsonSpaceItem2dSerializer : public JsonSpaceItemSerializer
{
public:
    JsonSpaceItem2dSerializer();

protected:
    QJsonObject serialize(StData::SpaceItem2d* spaceItem2d);
    void deserialize(const QJsonObject& itemData, StData::SpaceItem2d* spaceItem2d);

    QString pointToString(const QPointF& p);
    QString sizeToString(const QSizeF& s);

    QPointF pointFromString(const QString& str);
    QSizeF sizeFromString(const QString& str);
};

}

#endif // JSONSPACEITEM2DSERIALIZER_H
