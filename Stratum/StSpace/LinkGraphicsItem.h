#ifndef LINKGRAPHICSITEM_H
#define LINKGRAPHICSITEM_H

#include "stspace_global.h"
#include "PolylineGraphicsItem.h"

namespace StData {
class LinkInfo;
}

namespace StSpace {

class STSPACESHARED_EXPORT LinkGraphicsItem : public PolylineGraphicsItem
{
public:
    LinkGraphicsItem(StData::PolylineItem2d* polylineItem, QGraphicsItem* parent = 0);
    ~LinkGraphicsItem();

    StData::LinkInfo* link() const {return m_link;}

    // ISpaceGraphicsItem interface
public:
    void setSpaceItemOrg(double, double);
    void setSpaceItemSize(double, double);
    void rotateObject2d(const QPointF&, const double&);

public:
    int type() const;

private:
    StData::LinkInfo* m_link;
};

}

#endif // LINKGRAPHICSITEM_H
