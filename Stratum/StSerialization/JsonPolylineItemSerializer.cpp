#include "JsonPolylineItemSerializer.h"

#include <PolylineArrow.h>
#include <QJsonArray>
#include <SpaceItem.h>
#include <SpaceItemResult.h>
#include <Tool2d.h>

using namespace StData;

namespace StSerialization {

JsonPolylineItemSerializer::JsonPolylineItemSerializer() :
    JsonSpaceItem2dSerializer()
{}

QJsonObject JsonPolylineItemSerializer::serialize(PolylineItem2d* item)
{
    QJsonObject itemData = JsonSpaceItem2dSerializer::serialize(item);

    itemData[KEY_POLYLINE_FILLRULE] = item->fillRule();
    itemData[KEY_POLYLINE_PEN_HANDLE] = item->penTool() ? item->penTool()->handle() : 0;
    itemData[KEY_POLYLINE_BRUSH_HANDLE] = item->brushTool() ? item->brushTool()->handle() : 0;

    QJsonArray pointsJson;
    for(QPointF point : item->polyline())
        pointsJson.append(QString("%1;%2").arg(point.x()).arg(point.y()));

    itemData[KEY_POLYLINE_POINTS] = pointsJson;

    if(PolylineArrowPtr arrow = item->arrow())
    {
        QJsonObject arrowData;
        arrowData[KEY_ARROW_START_ANGLE] = arrow->startAngle();
        arrowData[KEY_ARROW_START_LENGTH] = arrow->startLength();
        arrowData[KEY_ARROW_END_ANGLE] = arrow->endAngle();
        arrowData[KEY_ARROW_END_LENGTH] = arrow->endLength();
        arrowData[KEY_ARROW_FILLFLAGS] = arrow->flags();

        itemData[KEY_POLYLINE_ARROWS] = arrowData;
    }

    return itemData;
}

PolylineItem2d* JsonPolylineItemSerializer::deserialize(const QJsonObject& itemData)
{
    auto item = new PolylineItem2d(nullptr);

    JsonSpaceItemSerializer::deserialize(itemData, item);

    item->setFillRule(itemData.value(KEY_POLYLINE_FILLRULE).toInt());
    item->setPenHandle(itemData.value(KEY_POLYLINE_PEN_HANDLE).toInt());
    item->setBrushHandle(itemData.value(KEY_POLYLINE_BRUSH_HANDLE).toInt());

    QJsonArray pointsData = itemData.value(KEY_POLYLINE_POINTS).toArray();
    for(QJsonValue pointValue: pointsData)
    {
        QList<QString> pointCoors = pointValue.toString().split(';', QString::SkipEmptyParts);
        QPointF point(pointCoors.at(0).toDouble(), pointCoors.at(1).toDouble());
        item->addPoint(point);
    }

    QJsonObject arrowData = itemData.value(KEY_POLYLINE_ARROWS).toObject();

    PolylineArrowPtr arrow = std::make_shared<PolylineArrow>();
    arrow->setStartAngle(arrowData.value(KEY_ARROW_START_ANGLE).toDouble());
    arrow->setStartLength(arrowData.value(KEY_ARROW_START_LENGTH).toDouble());
    arrow->setEndAngle(arrowData.value(KEY_ARROW_END_ANGLE).toDouble());
    arrow->setEndLength(arrowData.value(KEY_ARROW_END_LENGTH).toDouble());
    arrow->setFlags(arrowData.value(KEY_ARROW_FILLFLAGS).toInt());

    item->setArrow(arrow);

    return item;
}

}
