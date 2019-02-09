#ifndef SPACE2DSCENE_H
#define SPACE2DSCENE_H

#include <HandleCollection.h>
#include <QDir>
#include <QGraphicsScene>
#include <QMap>
#include <memory>

#include "stspace_global.h"
#include "StDataTypes.h"

namespace StData {
class Space;
class SpaceItem;
class Object;
class BrushTool;
class SpaceItemHyperKey;

typedef std::shared_ptr<SpaceItemHyperKey> SpaceItemHyperKeyPtr;
}

namespace StSpace {

class SpaceGraphicsItem;
class GroupGraphicsItem;


//TODO вестисписок z-порядков элементов сцены, чтобы не обновлять весь список
class STSPACESHARED_EXPORT SpaceScene : public QGraphicsScene
{
    Q_OBJECT
public:
    SpaceScene(StData::Space* space, QObject* parent = 0);
    ~SpaceScene();

    StData::Space* space() const {return m_space;}
    void setSpace(StData::Space* space);

    SpaceGraphicsItem* getSpaceGraphicsItem(int itemHandle) const;

    bool deleteObject(int objectHandle);

    double getObjectOrg2dX(int objectHandle);
    double getObjectOrg2dY(int objectHandle);
    bool setObjectOrg2d(int objectHandle, double x, double y);

    double getObjectWidth(int objectHandle);
    double getObjectHeight(int objectHandle);
    bool setObjectSize(int objectHandle, double w, double h);

    double getActualHeight(int objectHandle);
    double getActualWidth(int objectHandle);
    bool getActualSize(int objectHandle, double* width, double* height);

    double getObjectAngle2d(int objectHandle);
    bool rotateObject2d(int objectHandle, double x, double y, double angle);
    double getObjectAlpha2d(int objectHandle);
    bool setObjectAlpha2d(int objectHandle, double alpha);

    bool setShowObject2d(int objectHandle, bool isVisible);
    void updateItemsVisibility();
    bool isObjectsIntersected(int object1Handle, int object2Handle);
    int getObjectFromPoint2d(double x, double y);

    bool objectToBottom(int objectHandle);
    bool objectToTop(int objectHandle);
    bool setZOrder(int objectHandle, int zOrder);
    bool swapZOrder(int objectHandle1, int objectHandle2);

    int createPolyline(int penHandle, int brushHandle, const QPolygonF& polyline);
    int getPolylinePointsCount(int polylineHandle);
    bool addPolylinePoint(int polylineHandle, int index, double x, double y);
    bool delPolylinePoint(int polylineHandle, int index);
    int getPolylinePen(int polylineHandle);
    bool setPolylinePen(int polylineHandle, int penHandle);
    int getPolylineBrush(int polylineHandle);
    bool setPolylineBrush(int polylineHandle, int brushHandle);
    double getPolylinePointX(int polylineHandle, int index);
    double getPolylinePointY(int polylineHandle, int index);
    bool setPolylinePoint(int polylineHandle, int index, double x, double y);
    double getPolylineFillRule(int polylineHandle);
    bool setPolylineFillRule(int polylineHandle, int fillRule);
    bool setLineArrows(int polylineHandle,
                       double aa, double al, double as,
                       double ba, double bl, double bs);

    int createRasterText(int textHandle, double x, double y, double angle);

    int createPixmap(int dibHandle, double x, double y);
    int createMaskedPixmap(int doubleDibHandle, double x, double y);
    int getDibObject(int pixmapItemHandle);
    bool setDibObject(int pixmapItemHandle, int dibHandle);
    bool setDoubleDibObject(int pixmapItemHandle, int doubleDibHandle);
    bool setPixmapSrcRect(int pixmapItemHandle, double x, double y, double width, double height);

    int createGroup(const QVector<int>& objectHandles);
    bool deleteGroup(int groupHandle);
    bool addItemToGroup(int groupHandle, int itemHandle);
    bool deleteGroupItem(int groupHandle, int itemHandle);
    bool setGroupItem2d(int groupHandle, int replacingItemIndex, int newItemHandle);
    bool setGroupItems2d(int groupHandle, const QVector<int>& objectHandles);

    int createControl(const QString& className, const QString& text, int style, int x, int y, int width, int height, int id);
    bool isControlEnabled(int controlhandle);
    bool setControlEnabled(int controlHandle, bool enabled);
    bool setControlFocus(int controlHandle);
    QString getControlText(int controlhandle);
    bool setControlText(int controlhandle, const QString& text);
    bool setControlFont(int controlHandle, int fontHandle);
    int getControlStyle(int controlHandle);
    bool setControlStyle(int controlHandle, int style);
    bool isButtonChecked(int controlHandle);
    bool setButtonChecked(int controlHandle, int checkState);
    bool listAddItem(int controlHandle, const QString& itemText);
    bool listInsertItem(int controlHandle, int index, const QString& itemText);
    QString listGetItem(int controlHandle, int index);
    bool listClear(int controlHandle);
    bool listRemoveItem(int controlHandle, int index);
    int listItemsCount(int controlHandle);
    int listCurrentIndex(int controlHandle);
    QList<int> listSelectedIndexes(int controlHandle);
    bool listSetCurrentIndex(int controlHandle, int index);
    int listFindItem(int controlHandle, const QString text, int startIndex);
    int listFindItemExact(int controlHandle, const QString text, int startIndex);

    QRectF calculateSceneRect();

    StData::BrushTool* bkBrush() const { return m_bkBrush; }
    void setBkBrush(StData::BrushTool* bkBrush);

    void setHyperJump(int objectHandle, const StData::SpaceItemHyperKeyPtr& key);
    void stdHyperJump(int objectHandle);

    SpaceGraphicsItem* createGraphicsItem(StData::SpaceItem* spaceItem);
    void pasteItem(StData::SpaceItem* item, const QList<StData::Tool*>& tools);

signals:
    void contextMenu(const QPoint& pos, QGraphicsItem* item = nullptr);
    void doubleClick(QGraphicsItem* item);

public:
    static StData::HandleCollection<StData::SpaceItem> cleanSpaceItems(StData::Space* space);

    void setHorizontalScrollRange(double min, double max);
    void setVertivalScrollRange(double min, double max);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

private:
    void initializeGraphicsItems();

    void addGroupItemsToDictionary(GroupGraphicsItem* groupGraphicsItem);
    void updateItemsHandles();
    void updateItemsHandle(SpaceGraphicsItem* item);
    void updateItemsZOrder(int fromZOrder = 0);

    void removeGraphicsItem(QGraphicsItem* item);
    void removeGroupGraphicsItem(QGraphicsItem* item);

private:
    StData::Space* m_space;
    QMap<int, QGraphicsItem*> m_graphicsItemForHandle;
    StData::BrushTool* m_bkBrush;

    double m_horizontalScrollRangeMin, m_horizontalScrollRangeMax;
    double m_verticalScrollRangeMin, m_verticalScrollRangeMax;
};

}
#endif // SPACE2DSCENE_H
