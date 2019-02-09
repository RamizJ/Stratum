#ifndef JSONPOLYLINEITEMSERIALIZER_H
#define JSONPOLYLINEITEMSERIALIZER_H

#include <QJsonObject>
#include <JsonSpaceItem2dSerializer.h>

namespace StData {
class PolylineItem2d;
}

namespace StSerialization {

#define KEY_POLYLINE_PEN_HANDLE "PenHandle"
#define KEY_POLYLINE_BRUSH_HANDLE "BrushHandle"
#define KEY_POLYLINE_FILLRULE "FillRule"
#define KEY_POLYLINE_POINTS "Points"
#define KEY_POLYLINE_ARROWS "Arrows"

#define KEY_ARROW_START_ANGLE "StartAngle"
#define KEY_ARROW_START_LENGTH "StartLength"
#define KEY_ARROW_END_ANGLE "EndAngle"
#define KEY_ARROW_END_LENGTH "EndLength"
#define KEY_ARROW_FILLFLAGS "FillFlags"

class JsonPolylineItemSerializer : public JsonSpaceItem2dSerializer
{
public:
    JsonPolylineItemSerializer();

    QJsonObject serialize(StData::PolylineItem2d* item);
    StData::PolylineItem2d* deserialize(const QJsonObject& itemData);
};

}

#endif // JSONPOLYLINEITEMSERIALIZER_H
