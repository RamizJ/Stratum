#include "SpaceWidget.h"
#include "SpaceScene.h"
#include "SpaceView.h"
#include "SpaceGraphicsItem.h"

#include <Class.h>
#include <Space.h>
#include <HyperBase.h>
#include <Log.h>

#include <QVBoxLayout>
#include <QCloseEvent>
#include <SpaceItem.h>
#include <QGraphicsItem>
#include <QDateTime>
#include <QApplication>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QScrollBar>
#include <functional>

#ifdef Q_OS_WIN
#include <windows.h>
#include <winuser.h>
#endif

using namespace StData;
using namespace std;

namespace StSpace{

SpaceWidget::SpaceWidget(const QString winName, Space* space, const QString& style, const QPoint pos, const QSize size,
                         SpaceWidget* parentSpaceWindow) :
    QWidget(parentSpaceWindow),
    m_windowName(winName),
    m_style(style),
    m_space(space),
    m_lastRequestedHandleFromPoint(0),
    m_wndActualSize(size),
    m_closeInProcess(false),
    m_isNoResize(false),
    m_autoScale(1.0)
{
    setMinimumSize(5, 5);

    if(space)
    {
        m_spaceScene = new SpaceScene(space, this);
        m_spaceScene->setSceneRect(-100000, -100000, 200000, 200000);
        m_spaceScene->installEventFilter(this);

        m_spaceView = new SpaceView(this);
        m_spaceView->setScene(m_spaceScene);
        m_spaceView->setFrameShape(QFrame::NoFrame);
        m_spaceView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_spaceView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_spaceView->setScrollingEnabled(false);
        //        m_spaceView->setRenderHints(QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing | QPainter::Antialiasing);
        m_spaceView->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
        m_spaceView->setTransformationAnchor(QGraphicsView::NoAnchor);
        m_spaceView->setResizeAnchor(QGraphicsView::NoAnchor);

        updateTransform();

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setMargin(0);
        layout->addWidget(m_spaceView);
        setLayout(layout);

        setupWindowParams(style);

        if(!pos.isNull())
            move(pos);

        else if((windowFlags() & Qt::Popup) != 0)
        {
            QPoint cursorPos = QCursor::pos();
            QPoint optimalPos = cursorPos;
            QSize screenSize = QApplication::primaryScreen()->size();

            if(cursorPos.x() + m_wndActualSize.width() > screenSize.width())
            {
                optimalPos.setX(cursorPos.x() - width());
                if(optimalPos.x() <= 0 )
                    optimalPos.setX(4);
            }

            if(cursorPos.y() + m_wndActualSize.height() > screenSize.height())
            {
                optimalPos.setY(cursorPos.y() - height());
                if(optimalPos.y() <=0 )
                    optimalPos.setY(4);
            }

            move(optimalPos);
        }
    }
    setWindowTitle(m_windowName);
}

void SpaceWidget::setupWindowParams(const QString& style)
{
    Qt::WindowFlags wndFlags;
    Qt::WindowState wndState;
    QRectF sceneRect = m_spaceScene->calculateSceneRect();

    bool isVScrollEnabled = false;
    bool isHScrollEnabled = false;
    m_isNoResize = false;
    if(style.contains("WS_BYSPACE", Qt::CaseInsensitive))
    {
        if(auto hyperKey = m_space->hyperKey())
        {
            if(!hyperKey->windowSize().isEmpty())
                m_wndActualSize = hyperKey->windowSize();

            wndFlags = hyperKey->windowFlags();
            wndState = hyperKey->windowState();

            if(hyperKey->isAutoOrg())
                setSpaceOrg(sceneRect.topLeft());

            if(hyperKey->isSpaceSize())
                m_wndActualSize = sceneRect.size().toSize();

            if(hyperKey->isNoResize())
                m_isNoResize = true;

            isHScrollEnabled = hyperKey->isHScrollEnabled();
            isVScrollEnabled = hyperKey->isVScrollEnabled();
        }
    }

    if(style.contains("WS_POPUP", Qt::CaseInsensitive))
        wndFlags |= Qt::Popup;

    if(style.contains("WS_DIALOG", Qt::CaseInsensitive) && !style.contains("WS_TOOL", Qt::CaseInsensitive))
        wndFlags |= Qt::Dialog | Qt::WindowCloseButtonHint;

    if(style.contains("WS_NOCAPTION", Qt::CaseInsensitive))
        wndFlags |= (Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    if(style.contains("WS_TOOL", Qt::CaseInsensitive))
        wndFlags |= Qt::Tool | Qt::WindowStaysOnTopHint;

    if(style.contains("WS_MAXIMIZED", Qt::CaseInsensitive))
        wndState = Qt::WindowMaximized;

    else if(style.contains("WS_MINIMIZE", Qt::CaseInsensitive))
        wndState = Qt::WindowMinimized;

    if(style.contains("WS_AUTOORG", Qt::CaseInsensitive))
        setSpaceOrg(sceneRect.topLeft());

    if(style.contains("WS_SPACESIZE", Qt::CaseInsensitive))
        m_wndActualSize = sceneRect.size().toSize();

    if(style.contains("WS_NORESIZE", Qt::CaseInsensitive))
        m_isNoResize = true;

    if(style.contains("WS_CORRECTPOS", Qt::CaseInsensitive))
    {
        QRect rect = QGuiApplication::primaryScreen()->geometry();
        int x = rect.center().x() - width()/2;
        int y = rect.center().y() - height()/2;
        move(x, y);
    }

    if(style.contains("WS_VSCROLL", Qt::CaseInsensitive) || isVScrollEnabled)
    {
        m_spaceView->setScrollingEnabled(true);
        m_spaceView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        m_spaceView->setSceneRect(sceneRect);
    }

    if(style.contains("WS_NORESIZE", Qt::CaseInsensitive))
        m_isNoResize = true;

    if(style.contains("WS_HSCROLL", Qt::CaseInsensitive) || isHScrollEnabled)
    {
        m_spaceView->setScrollingEnabled(true);
        m_spaceView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        m_spaceView->setSceneRect(sceneRect);
    }

    setWindowFlags(wndFlags);
    setWindowState(wndState);

    if(m_isNoResize)
        setFixedSize(m_wndActualSize);
    else
        resize(m_wndActualSize);
}

void SpaceWidget::updateTransform()
{
    double scale = m_space->scale() * m_autoScale;

    SystemLog::instance().info(QString("autoScale: %1").arg(scale));

    QTransform t;
    t.translate(-m_space->org().x()* scale, -m_space->org().y() * scale);
    t.scale(scale, scale);
    m_spaceView->setTransform(t);
}

void SpaceWidget::setSpace(StData::Space* space)
{
    if(m_space == space)
        return;

    m_space = space;
    m_spaceScene->setSpace(m_space);
}

bool SpaceWidget::isCreateBySpaceSize() const
{
    return m_style.toUpper().contains("WS_SPACESIZE");
}

bool SpaceWidget::isCreateWithCorrectPos() const
{
    return m_style.toUpper().contains("WS_CORRECTPOS");
}

QPointF SpaceWidget::getSpaceOrg()
{
    return m_space->org();
}

void SpaceWidget::setSpaceOrg(double x, double y)
{
    //m_spaceView->translate(m_space->org().x() - x, m_space->org().y() - y);
    m_space->setOrg(QPointF(x, y));
    updateTransform();
}

void SpaceWidget::setSpaceOrg(const QPointF& point)
{
//    m_spaceView->translate(m_space->org().x() - point.x(), m_space->org().y() - point.y());
//    m_space->setOrg(point);
    setSpaceOrg(point.x(), point.y());
}

double SpaceWidget::getSpaceScale()
{
    return m_space->scale();
}

void SpaceWidget::setSpaceScale(double k)
{
    m_space->setScale(k);
    //    QTransform transform = m_spaceView->transform();
    //    transform.scale(k, k);
    //    m_spaceView->setTransform(transform);
    updateTransform();
}

void SpaceWidget::setAutoScale(double autoScale)
{
    m_autoScale = autoScale;

    updateTransform();
}

int SpaceWidget::spaceLayers()
{
    return m_space->layers();
}

void SpaceWidget::setSpaceLayers(int layers)
{
    m_space->setLayers(layers);
    m_spaceScene->updateItemsVisibility();
}

bool SpaceWidget::saveSpaceArea(const QString& fileName, int colorBits,
                                double x, double y, double width, double height)
{
    QRectF rect(x,y, width, height);

    switch(colorBits)
    {
        case 1:
        {
            QImage image(width, height, QImage::Format_Mono);
            image.fill(Qt::white);
            QPainter painter(&image);
            m_spaceScene->render(&painter, rect, rect);
            return image.save(fileName);
        }

            //        case 4:
            //        {
            //            QImage image(width, height, QImage::Format_Indexed8);
            //            image.fill(Qt::white);
            //            QPainter painter(&image);
            //            m_spaceScene->render(&painter, rect, rect);
            //            return image.save(fileName);
            //        }
    }

    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(Qt::white);
    QPainter painter(&image);
    m_spaceScene->render(&painter, rect, rect);
    return image.save(fileName, "PNG");
}

QString SpaceWidget::getObjectName2d(int objectHandle)
{
    if(SpaceItem* item = m_space->getSpaceItem(objectHandle))
        return item->name();
    return "";
}

bool SpaceWidget::setObjectName2d(int objectHandle, const QString& name)
{
    if(SpaceItem* item = m_space->getSpaceItem(objectHandle))
    {
        item->setName(name);
        return true;
    }
    return false;
}

int SpaceWidget::getObject2dByName(int groupHandle, const QString& name)
{
    if(groupHandle != 0)
    {
        SpaceItem* item = m_space->getSpaceItem(groupHandle);
        if(GroupItem* groupItem = dynamic_cast<GroupItem*>(item))
            return getByNameInGroup(groupItem, name);
    }
    else
    {
        for(SpaceItem* item : m_space->spaceItems().items())
        {
            if(QString::compare(item->name(), name, Qt::CaseInsensitive) == 0)
                return item->handle();
        }
    }
    //    SpaceItem* item = m_space->getSpaceItem(groupHandle);
    //    GroupItem* groupItem = dynamic_cast<GroupItem*>(item);

    //    for(SpaceItem* item : m_space->spaceItems().items())
    //    {
    //        if(item->name() == name && (item->isChildFor(groupItem) || groupHandle == 0))
    //            return item->handle();
    //    }
    return 0;
}

int SpaceWidget::getByNameInGroup(SpaceItem* item, const QString& name)
{
    if(QString::compare(item->name(), name, Qt::CaseInsensitive) == 0)
        return item->handle();

    if(GroupItem* groupItem = dynamic_cast<GroupItem*>(item))
    {
        for(SpaceItem* childItem : groupItem->items())
        {
            if(int handle = getByNameInGroup(childItem, name))
                return handle;
        }
    }
    return 0;
}

double SpaceWidget::getObjectOrgX(int objectHandle)
{
    return m_spaceScene->getObjectOrg2dX(objectHandle);
}

double SpaceWidget::getObjectOrgY(int objectHandle)
{
    return m_spaceScene->getObjectOrg2dY(objectHandle);
}

bool SpaceWidget::setObjectOrg2d(int objectHandle, double x, double y)
{
    return m_spaceScene->setObjectOrg2d(objectHandle, x, y);
}

double SpaceWidget::getObjectWidth(int objectHandle)
{
    return m_spaceScene->getObjectWidth(objectHandle);
}

double SpaceWidget::getObjectHeight(int objectHandle)
{
    return m_spaceScene->getObjectHeight(objectHandle);
}

bool SpaceWidget::setObjectSize(int objectHandle, double w, double h)
{
    return m_spaceScene->setObjectSize(objectHandle, w, h);
}

double SpaceWidget::getActualWidth(int objectHandle)
{
    return m_spaceScene->getActualWidth(objectHandle);
}

double SpaceWidget::getActualHeight(int objectHandle)
{
    return m_spaceScene->getActualHeight(objectHandle);
}

bool SpaceWidget::getActualSize(int objectHandle, double* width, double* height)
{
    return m_spaceScene->getActualSize(objectHandle, width, height);
}

double SpaceWidget::getObjectAngle2d(int objectHandle)
{
    return m_spaceScene->getObjectAngle2d(objectHandle);
}

bool SpaceWidget::rotateObject2d(int objectHandle, double x, double y, double angle)
{
    return m_spaceScene->rotateObject2d(objectHandle, x, y, angle);
}

double SpaceWidget::getObjectAlpha2d(int objectHandle)
{
    return m_spaceScene->getObjectAlpha2d(objectHandle);
}

bool SpaceWidget::setObjectAlpha2d(int objectHandle, double alpha)
{
    return m_spaceScene->setObjectAlpha2d(objectHandle, alpha);
}

bool SpaceWidget::setShowObject2d(int objectHandle, bool show)
{
    return m_spaceScene->setShowObject2d(objectHandle, show);
}

bool SpaceWidget::showObject2d(int objectHandle)
{
    return m_spaceScene->setShowObject2d(objectHandle, true);
}

bool SpaceWidget::hideObject2d(int objectHandle)
{
    return m_spaceScene->setShowObject2d(objectHandle, false);
}

int SpaceWidget::getObjectFromPoint2d(double x, double y)
{    
    m_lastRequestedHandleFromPoint = m_spaceScene->getObjectFromPoint2d(x, y);
    return m_lastRequestedHandleFromPoint;
    //Сильно грузит проц
    //    {
    //        //    QList<QGraphicsItem*> itemsFromPoint = m_spaceScene->items(QRectF(x - 1.5, y - 1.5, 3, 3), Qt::IntersectsItemBoundingRect);
    //        QList<QGraphicsItem*> itemsFromPoint = m_spaceScene->items(QPointF(x, y));
    //        for(QGraphicsItem* item : itemsFromPoint)
    //        {
    //            if(SpaceGraphicsItem* spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(item))
    //            {
    //                SpaceItem* spaceItem = spaceGraphicsItem->spaceItem();
    //                while(spaceItem->ownerItem())
    //                    spaceItem = spaceItem->ownerItem();

    //                m_lastRequestedHandleFromPoint = spaceItem->handle();
    //                return spaceItem->handle();
    //            }
    //        }
    //    }
    //    return 0;
}

int SpaceWidget::getObjectFromPoint2dEx(double x, double y, int layer)
{
    m_lastRequestedHandleFromPoint = 0;
    QList<QGraphicsItem*> itemsFromPoint = m_spaceScene->items(QRectF(x - 1.5, y - 1.5, 3, 3));
    for(QGraphicsItem* item : itemsFromPoint)
    {
        if(SpaceGraphicsItem* spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(item))
            if((spaceGraphicsItem->spaceItem()->layer() == layer))
            {
                m_lastRequestedHandleFromPoint = spaceGraphicsItem->handle();
                return spaceGraphicsItem->handle();
            }
    }
    return 0;
}

int SpaceWidget::getLastPrimary()
{
    return m_lastRequestedHandleFromPoint;
}

bool SpaceWidget::isObjectsIntersect2d(int object1Handle, int object2Handle, int flag)
{
    return m_spaceScene->isObjectsIntersected(object1Handle, object2Handle);
}

bool SpaceWidget::isPointInItem2d(double x, double y, int objectHandle)
{
    QList<QGraphicsItem*> itemsFromPoint = m_spaceScene->items(QPointF(x, y));
    for(QGraphicsItem* item : itemsFromPoint)
        if(SpaceGraphicsItem* spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(item))
            if(spaceGraphicsItem->handle() == objectHandle)
                return true;

    return false;
}

int SpaceWidget::getObjectAttribute2d(int objectHandle)
{
    return m_space->getObjectAttribute2d(objectHandle);
}

bool SpaceWidget::setObjectAttribute(int objectHandle, int attrMode, int attrValue)
{
    return m_space->setObjectAttribute(objectHandle, attrMode, attrValue);
}

int SpaceWidget::getNextObject(int objectHandle)
{
    m_currentObjectHandle = m_space->getNextItemHandle(objectHandle);
    return m_currentObjectHandle;

    //    m_currentObjectHandle = 0;
    //    if(m_space->spaceItems().isEmpty())
    //        return 0;

    //    if(objectHandle == 0)
    //    {
    //        m_currentObjectHandle = m_space->spaceItems().handles().first();
    //        return m_currentObjectHandle;
    //    }

    //    int index = m_space->spaceItems().handles().indexOf(objectHandle);
    //    if(index == -1 || index == m_space->spaceItems().count() - 1)
    //        return 0;

    //    m_currentObjectHandle = m_space->spaceItems().handles().at(index + 1);
    //    return m_currentObjectHandle;
}

int SpaceWidget::getBottomObject()
{
    if(m_space->zOrderList().isEmpty())
        return 0;

    return m_space->zOrderList().first()->handle();
}

int SpaceWidget::getTopObject()
{
    if(m_space->zOrderList().isEmpty())
        return 0;

    return m_space->zOrderList().last()->handle();
}

int SpaceWidget::getUpperObject(int objectHandle)
{
    int index = m_space->getZOrderIndexByHandle(objectHandle);
    if(index >=0 && index <  m_space->zOrderList().count() - 1)
        return m_space->zOrderList().at(index + 1)->handle();

    return 0;
}

int SpaceWidget::getLowerObject(int objectHandle)
{
    int index = m_space->getZOrderIndexByHandle(objectHandle);
    if(index > 0 )
        return m_space->zOrderList().at(index - 1)->handle();

    return 0;
}

int SpaceWidget::getObjectFromZOrder(int zOrder)
{
    if(zOrder > 0 && zOrder <= m_space->zOrderList().count())
        return m_space->zOrderList().at(zOrder - 1)->handle();

    return 0;
}

int SpaceWidget::getZOrder(int objectHandle)
{
    SpaceItem* spaceItem = m_space->getSpaceItem(objectHandle);
    if(spaceItem)
        return spaceItem->zOrder();

    return 0;
}

bool SpaceWidget::objectToBottom(int objectHandle)
{
    return m_spaceScene->objectToBottom(objectHandle);
}

bool SpaceWidget::objectToTop(int objectHandle)
{
    return m_spaceScene->objectToTop(objectHandle);
}

bool SpaceWidget::setObjectZOrder(int objectHandle, int zOrder)
{
    return m_spaceScene->setZOrder(objectHandle, zOrder);
}

bool SpaceWidget::swapObjectsZOrder(int objectHandle1, int objectHandle2)
{
    return m_spaceScene->swapZOrder(objectHandle1, objectHandle2);
}

int SpaceWidget::createPolyline(int penHandle, int brushHandle, const QPolygonF& polyline)
{
    return m_spaceScene->createPolyline(penHandle, brushHandle, polyline);
}

int SpaceWidget::createControl(const QString& className, const QString& text, int style,
                               int x, int y, int width, int height, int id)
{
    return m_spaceScene->createControl(className, text, style, x, y, width, height, id);
}

void SpaceWidget::lockSceneUpdate(bool lock)
{
    if(lock)
        m_lockCount++;
    else if(m_lockCount > 0)
        m_lockCount--;

    if(m_lockCount == 0)
    {
        m_spaceView->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
        m_spaceView->viewport()->update();
    }
    else
        m_spaceView->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
}

void SpaceWidget::setWndSize(const QSize& wndSize)
{
    if(m_wndActualSize != wndSize)
    {
        m_wndActualSize = wndSize;

        bool bySpacePopup = false;
        if(auto hKey = m_space->hyperKey())
            bySpacePopup = (hKey->windowFlags() & Qt::Popup) != 0;

        QSize realSize = m_wndActualSize;
        if(m_style.contains("WS_BYSPACE", Qt::CaseInsensitive) && bySpacePopup)
            realSize = QSize(m_wndActualSize.width() - 10, m_wndActualSize.height() - 10);

        if(m_isNoResize)
            setFixedSize(realSize);
        else
            resize(realSize);
    }
}

void SpaceWidget::updateViewport()
{
    m_spaceView->viewport()->update();
    //    m_spaceScene->update();
}

void SpaceWidget::updateViewport(const QRect& rect)
{
    m_spaceView->viewport()->update(rect);
}

bool SpaceWidget::setScrollRange(int scrollType, double min, double max)
{
    bool result = false;

    if(scrollType == 1)
        result = m_spaceView->setHorizontalScrollRange(min, max);

    else if(scrollType == 0)
        result = m_spaceView->setVertivalScrollRange(min, max);

    return result;
}

bool SpaceWidget::event(QEvent* event)
{
    if(event->type() == QEvent::Hide)
    {
        if(FLAG_EQUAL(windowFlags(), Qt::Popup))
            close();
    }
    return QWidget::event(event);
}

bool SpaceWidget::eventFilter(QObject* watched, QEvent* event)
{
    if(event->type() == QEvent::GraphicsSceneMousePress ||
       event->type() == QEvent::GraphicsSceneMouseMove ||
       event->type() == QEvent::GraphicsSceneMouseRelease)
    {
        emit sceneMouseEventOnParent(static_cast<QGraphicsSceneMouseEvent*>(event));
    }
    return QWidget::eventFilter(watched, event);
}

//void SpaceWidget::onSceneMouseEventOnParent(QGraphicsSceneMouseEvent* parentEvent)
//{
//    QPoint screenPos = parentEvent->screenPos();
//    QPoint pos = m_spaceView->mapFromGlobal(screenPos);
//    QPointF scenePos = m_spaceView->mapToScene(pos);

//    QGraphicsSceneMouseEvent childEvent(parentEvent->type());
//    childEvent.setScreenPos(screenPos);
//    childEvent.setPos(pos);
//    childEvent.setScenePos(scenePos);
//    childEvent.setButtons(parentEvent->buttons());
//    childEvent.setButton(parentEvent->button());
//    childEvent.setModifiers(parentEvent->modifiers());

//    SystemLog::instance().warning("mouse event on parent: " + m_windowName);

//    QApplication::sendEvent(m_spaceScene, &childEvent);
//}

bool SpaceWidget::deleteObject(int objectHandle)
{
    return m_spaceScene->deleteObject(objectHandle);
}

int SpaceWidget::getObjectType(int objectHandle)
{
    return m_space->getObjectType(objectHandle);
}

void SpaceWidget::closeEvent(QCloseEvent* event)
{
    m_closeInProcess = true;
    emit windowClosed();
    QWidget::closeEvent(event);
}

}
