#ifndef SPACEWINDOW_H
#define SPACEWINDOW_H

#include "stspace_global.h"

#include <QDir>
#include <QWidget>
#include <QGraphicsSceneMouseEvent>

namespace StData {
class Project;
class Class;
class Space;
class SpaceItem;
}

namespace StSpace{

class SpaceScene;
class SpaceView;

//TODO перенести ф-ции сцены в SpaceScene
class STSPACESHARED_EXPORT SpaceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SpaceWidget(const QString winName, StData::Space* space, const QString& style,
                         const QPoint pos = QPoint(), const QSize size = QSize(),
                         SpaceWidget* parentSpaceWindow = nullptr);

    QString windowName() const { return m_windowName; }
    void setWindowName(const QString& windowName) { m_windowName = windowName; }

    StData::Space* space() const { return m_space; }
    void setSpace(StData::Space* space);

    StSpace::SpaceScene* spaceScene() const { return m_spaceScene; }
    StSpace::SpaceView* spaceView() const { return m_spaceView; }

    bool isCreateBySpaceSize() const;
    bool isCreateWithCorrectPos() const;

    QString spaceWidgetStyle() const { return m_style; }

public:
    QPointF getSpaceOrg();
    void setSpaceOrg(double x, double y);
    void setSpaceOrg(const QPointF& point);

    double getSpaceScale();
    void setSpaceScale(double k);
    void setAutoScale(double autoScale);

    int spaceLayers();
    void setSpaceLayers(int layers);

    bool saveSpaceArea(const QString& fileName, int colorBits,
                       double x, double y, double width, double height);

    int getObject2dByName(int groupHandle, const QString& name);
    int getByNameInGroup(StData::SpaceItem* item, const QString& name);
    QString getObjectName2d(int objectHandle);
    bool setObjectName2d(int objectHandle, const QString& name);

    bool deleteObject(int objectHandle);
    int getObjectType(int objectHandle);

    double getObjectOrgX(int objectHandle);
    double getObjectOrgY(int objectHandle);
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

    bool setShowObject2d(int objectHandle, bool show);
    bool showObject2d(int objectHandle);
    bool hideObject2d(int objectHandle);

    int getObjectFromPoint2d(double x, double y);
    int getObjectFromPoint2dEx(double x, double y, int layer);
    int getLastPrimary();
    bool isObjectsIntersect2d(int object1Handle, int object2Handle, int flag);
    bool isPointInItem2d(double x, double y, int objectHandle);

    int getObjectAttribute2d(int objectHandle);
    bool setObjectAttribute(int objectHandle, int attrMode, int attrValue);

    int getNextObject(int objectHandle);
    int getCurrentObject();
    int setCurrentObject(int objectHandle);

    int getBottomObject();
    int getTopObject();
    int getUpperObject(int objectHandle);
    int getLowerObject(int objectHandle);
    int getObjectFromZOrder(int zOrder);
    int getZOrder(int objectHandle);
    bool objectToBottom(int objectHandle);
    bool objectToTop(int objectHandle);
    bool setObjectZOrder(int objectHandle, int zOrder);
    bool swapObjectsZOrder(int objectHandle1, int objectHandle2);

    int createPolyline(int penHandle, int brushHandle, const QPolygonF& polyline);
    int createControl(const QString& className, const QString& text, int style,
                      int x, int y, int width, int height, int id);

    void lockSceneUpdate(bool lock);

    //NOTE: размеры виджета обновляются после перерисовки, что приводит к ошибкам при запросе размера до перерисовки.
    //для этого определены эти ф-ции, чтобы возвращать актуальный размер
    QSize wndActualSize() const { return m_wndActualSize; }
    void setWndSize(const QSize& wndActualSize);

    void updateViewport();
    void updateViewport(const QRect& rect);

    bool closeInProcess() const { return m_closeInProcess; }

    bool setScrollRange(int scrollType, double min, double max);

signals:
    void windowClosed();
    void sceneMouseEventOnParent(QGraphicsSceneMouseEvent* event);

public:
    bool event(QEvent* event);
    bool eventFilter(QObject* watched, QEvent* event);

//public slots:
//    void onSceneMouseEventOnParent(QGraphicsSceneMouseEvent* parentEvent);

protected:
    void closeEvent(QCloseEvent* event);

private:
    void setupWindowParams(const QString& style);
    void updateTransform();

private:
    QString m_windowName;
    QString m_style;

    SpaceScene* m_spaceScene;
    SpaceView* m_spaceView;

    StData::Space* m_space;

    int m_lastRequestedHandleFromPoint;
    int m_currentObjectHandle;
    int m_lockCount;
    QSize m_wndActualSize;
    bool m_isNoResize;

    bool m_closeInProcess;
    double m_autoScale;
};

}

#endif // SPACEWINDOW_H
