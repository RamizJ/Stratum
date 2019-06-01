#include "SpaceWindowItem.h"
//#include "MessageManager.h"
#include "VmGlobal.h"
#include "VmLog.h"

#include <Class.h>
#include <Sc2000DataStream.h>
#include <Scheme.h>
#include <Space.h>
#include <SpaceScene.h>
#include <SpaceWidget.h>
#include <limits>
#include <SystemFunctions.h>
#include <Settings.h>
#include <QApplication>
#include <QScreen>
#include <math.h>
#include <QDebug>

using namespace StSpace;
using namespace StData;

namespace StVirtualMachine {

SpaceWindowItem::SpaceWindowItem(const QString& windowName, SpaceWindowItem* parentItem,
                                 Space* space, const QString& style, const QPoint& pos, const QSize& size, int handle) :
    QObject(),
    HandleItem(handle),
    m_space(space),
    m_windowName(windowName),
    m_class(nullptr),
    m_parentItem(nullptr),
    m_proxyWidget(nullptr),
    m_isChildWindow(false),
    m_isAutoResized(false)
{
    m_spaceWidget = new SpaceWidget(windowName, m_space, style, pos, size, nullptr);
    m_project = executedProject;

    if(parentItem && style.contains("WS_CHILD", Qt::CaseInsensitive))
    {
        parentItem->addChildWindow(this);
        m_isChildWindow = true;
        move(pos);
    }

    m_isAutoResized = style.contains("WS_AUTOSIZE", Qt::CaseInsensitive);

    connect(m_spaceWidget, &SpaceWidget::windowClosed, this, &SpaceWindowItem::windowClosed);
    connect(m_spaceWidget, &SpaceWidget::destroyed, m_space, &Space::deleteLater);
}

SpaceWindowItem::~SpaceWindowItem()
{
    emit windowDestroyed();

    for(SpaceWindowItem* childItem : m_childItems)
        childItem->deleteLater();

    if(m_spaceWidget != nullptr)
    {
        messageManager->unregisterObjects(this);

        if(m_proxyWidget)
            m_proxyWidget->deleteLater();

        else if(!m_spaceWidget->closeInProcess())
            m_spaceWidget->deleteLater();
    }
}

void SpaceWindowItem::setSpace(Space* space)
{
    m_spaceWidget->setSpace(space);
}

void SpaceWindowItem::addChildWindow(SpaceWindowItem* childItem)
{
    if(childItem)
    {
        QGraphicsProxyWidget* childProxyWidget = m_spaceWidget->spaceScene()->addWidget(childItem->spaceWidget());
        childItem->setParentItem(this);
        childItem->setProxyWidget(childProxyWidget);

        m_childItems << childItem;

//        m_spaceWidget->installEventFilter(childItem->spaceWidget());
//        m_spaceWidget->spaceScene()->installEventFilter(childItem->spaceWidget()->spaceScene());
//        connect(m_spaceWidget, &SpaceWidget::sceneMouseEventOnParent,
//                childItem->spaceWidget(), &SpaceWidget::onSceneMouseEventOnParent);

        connect(childItem, &SpaceWindowItem::windowDestroyed, this, &SpaceWindowItem::removeChildItem);

        childProxyWidget->setZValue(std::numeric_limits<double>::max());
    }
}

void SpaceWindowItem::setParentItem(SpaceWindowItem* parentItem)
{
//    VmLog::instance().error("SetParentWindow - function incomplete");
    m_parentItem = parentItem;
}

void SpaceWindowItem::move(int x, int y)
{
    if(!m_isAutoResized)
    {
        m_spaceWidget->move(x, y);
    }
}

void SpaceWindowItem::move(const QPoint& pos)
{
    move(pos.x(), pos.y());
}

void SpaceWindowItem::resize(int w, int h)
{
    if(m_isAutoResized)
        autoResize(w, h);
    else
        m_spaceWidget->setWndSize(QSize(w, h));
}

void SpaceWindowItem::resize(const QSizeF& size)
{
    resize(size.width(), size.height());
}

void SpaceWindowItem::setSpaceScale(double scale)
{
    if(!m_isAutoResized)
    {
        m_spaceWidget->setSpaceScale(scale);
    }
}

void SpaceWindowItem::autoResize(int w, int h)
{
//    QScreen* scr = QApplication::primaryScreen();
//    QSize screenSize = scr->size();
    QSize screenSize = Settings::instance().mainWindowSize();
    SystemLog::instance().info(QString("Autoresize to: %1x%2").arg(screenSize.width()).arg(screenSize.height()));

    double wRatio = w / (double)screenSize.width();
    double hRatio = h / (double)screenSize.height();

    double ratio = wRatio > hRatio ? wRatio : hRatio;
    ratio = abs(ratio) > 1e-4 ? ratio : 1;
    double scale = 1 / ratio;

    w *= scale;
    h *= scale;

    m_spaceWidget->setWndSize(QSize(w, h));
    m_spaceWidget->move((screenSize.width() - w)*0.5, (screenSize.height() - h)*0.5);
    m_spaceWidget->setAutoScale(scale);
}

void SpaceWindowItem::removeChildItem()
{
    QObject* s = sender();
    if(SpaceWindowItem* childItem = dynamic_cast<SpaceWindowItem*>(s))
    {
        m_spaceWidget->spaceScene()->removeItem(childItem->proxyWidget());
        m_childItems.removeOne(childItem);
    }
}

QList<SpaceWindowItem*> SpaceWindowItem::childItems() const
{
    return m_childItems;
}

bool SpaceWindowItem::isChildWindow() const
{
    return m_isChildWindow;
}

}
