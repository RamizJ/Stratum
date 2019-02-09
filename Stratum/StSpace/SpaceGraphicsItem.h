#ifndef ISPACEGRAPHICSITEM_H
#define ISPACEGRAPHICSITEM_H

#include "stspace_global.h"

#include <QGraphicsItem>
#include <QPointF>
#include <QSizeF>
#include <memory>


namespace StData {
class SpaceItem2d;
class SpaceItemHyperKey;

typedef std::shared_ptr<SpaceItemHyperKey> SpaceItemHyperKeyPtr;
}

namespace StSpace {

class STSPACESHARED_EXPORT SpaceGraphicsItem
{
public:
    SpaceGraphicsItem(StData::SpaceItem2d* spaceItem);

    StData::SpaceItem2d* spaceItem() {return m_spaceItem;}
    int handle();

    virtual QGraphicsItem* graphicsItem() = 0;

    QPointF getSpaceItemOrg();
    virtual void setSpaceItemOrg(double x, double y);

    QSizeF getSpaceItemSize();
    virtual void setSpaceItemSize(double w, double h) = 0;

    virtual void setSpaceItemsVisible(bool visible);

    virtual double getSpaceItemOpacity();
    virtual void setSpaceItemOpacity(double opacity);

    virtual int getZOrder();
    virtual void rotateObject2d(const QPointF& base, const double& angle) = 0;

    virtual void updateVisibility();
    virtual void updateZOrder();
    virtual void updatePosition();
    virtual void updateSize() = 0;

    void activateHyperJump();

    void setHyperKey(const StData::SpaceItemHyperKeyPtr& key);

    void stdHyperJump();

protected:
    void rotatePoint(QPointF* point, const QPointF& base, double angle);
    void itemChanged();

protected:
    bool m_hyperJumpActivated;
    bool m_isItemChanged;

private:
    StData::SpaceItem2d* m_spaceItem;
};

}

#endif // ISPACEGRAPHICSITEM_H
