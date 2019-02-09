#include "LinkGraphicsItem.h"
#include "GraphicsItemType.h"

#include <LinkInfo.h>
#include <SpaceItem.h>
#include <Space.h>
#include <Class.h>
#include <QGraphicsSceneContextMenuEvent>

using namespace StData;

namespace StSpace {

LinkGraphicsItem::LinkGraphicsItem(StData::PolylineItem2d* polylineItem, QGraphicsItem* parent) :
    PolylineGraphicsItem(polylineItem, parent)
{
    m_link = polylineItem->space()->ownerClass()->linkById(polylineItem->handle());
//    setToolTip(QObject::tr("Связь"));
}

LinkGraphicsItem::~LinkGraphicsItem()
{}

void LinkGraphicsItem::setSpaceItemOrg(double, double)
{}

void LinkGraphicsItem::setSpaceItemSize(double /*w*/, double /*h*/)
{}

void LinkGraphicsItem::rotateObject2d(const QPointF& /*base*/, const double& /*angle*/)
{}

int LinkGraphicsItem::type() const
{
    return UserType + static_cast<int>(GraphicsItemType::Link);
}

}
