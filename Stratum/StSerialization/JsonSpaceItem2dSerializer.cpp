#include "JsonSpaceItem2dSerializer.h"

#include <SpaceItem.h>

using namespace StData;

namespace StSerialization {

JsonSpaceItem2dSerializer::JsonSpaceItem2dSerializer() :
    JsonSpaceItemSerializer()
{}

QJsonObject JsonSpaceItem2dSerializer::serialize(SpaceItem2d* spaceItem2d)
{
    QJsonObject result = JsonSpaceItemSerializer::serialize(spaceItem2d);

    QPointF origin = spaceItem2d->origin();
    QSizeF size = spaceItem2d->size();

    result[KEY_SPACEITEM2D_ORIGIN] = pointToString(origin);
    result[KEY_SPACEITEM2D_SIZE] = sizeToString(size);
    result[KEY_SPACEITEM2D_ANGLE] = spaceItem2d->angle();

    return result;
}

void JsonSpaceItem2dSerializer::deserialize(const QJsonObject& itemData, SpaceItem2d* spaceItem2d)
{
    if(spaceItem2d == nullptr || itemData.isEmpty()) return;

    JsonSpaceItemSerializer::deserialize(itemData, spaceItem2d);

    QString originData = itemData.value(KEY_SPACEITEM2D_ORIGIN).toString();
    QString sizeData = itemData.value(KEY_SPACEITEM2D_SIZE).toString();

    QList<QString> coords = originData.split(';', QString::SkipEmptyParts);
    double x = coords.at(0).toDouble();
    double y = coords.at(1).toDouble();

    QList<QString> sizeParams = sizeData.split(';', QString::SkipEmptyParts);
    double w = sizeParams.at(0).toDouble();
    double h = sizeParams.at(1).toDouble();

    spaceItem2d->setOrigin(QPointF(x, y));
    spaceItem2d->setSize(QSizeF(w, h));
}

QString JsonSpaceItem2dSerializer::pointToString(const QPointF& p)
{
    return QString("%1;%2").arg(p.x()).arg(p.y());
}

QString JsonSpaceItem2dSerializer::sizeToString(const QSizeF& s)
{
    return QString("%1;%2").arg(s.width()).arg(s.height());
}

QPointF JsonSpaceItem2dSerializer::pointFromString(const QString& str)
{
    QList<QString> xy = str.split(';', QString::SkipEmptyParts);

    if(xy.length() == 2)
        return QPointF(xy.at(0).toDouble(), xy.at(1).toDouble());

    return QPointF();
}

QSizeF JsonSpaceItem2dSerializer::sizeFromString(const QString& str)
{
    QList<QString> wh = str.split(';', QString::SkipEmptyParts);
    if(wh.length() == 2)
        return QSizeF(wh.at(0).toDouble(), wh.at(1).toDouble());

    return QSizeF();
}


}
