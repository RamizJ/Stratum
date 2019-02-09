#include "Array.h"
#include "SpaceWindowItem.h"
#include "SpaceWindowManager.h"
#include "VmGlobal.h"

#include <QDebug>

#include <Class.h>
#include <Scheme.h>
#include <Space.h>
#include <LibraryCollection.h>
#include <SpaceWidget.h>
#include <ObjectVar.h>

#include <HyperBase.h>
#include <Log.h>
#include <Project.h>
#include <QBitmap>
#include <SpaceWidget.h>
#include <StandardTypes.h>

using namespace StData;
using namespace StSpace;

namespace StVirtualMachine {

SpaceWindowManager::SpaceWindowManager(QObject *parent) :
    QObject(parent)
{}

SpaceWindowManager::~SpaceWindowManager()
{}

int SpaceWindowManager::createWindowEx(const QString& windowName, const QString& parentWindowName,
                                       const QString& contentSource, int hSpace,
                                       int x, int y, int w, int h, const QString& style)
{
    SpaceWindowItem* item = findByName(windowName);
    SpaceWindowItem* parentItem = findByName(parentWindowName);

    Scheme* scheme = nullptr;
    Space* space = nullptr;
    Class* cls = LibraryCollection::getClassByName(contentSource);

    if(SpaceWindowItem* spaceItem = m_windowItems.getItem(hSpace))
        scheme = spaceItem->space()->scheme();

    else if(cls)
        scheme = cls->scheme();

    if(scheme)
    {
        space = Space::createFromScheme(scheme);
    }
    else if(!contentSource.isEmpty())
    {
        QString filePath = executedProject->projectDir().absoluteFilePath(contentSource);
        space = Space::createFromFile(filePath);
    }

    if(item)
    {
        if(space && !isSpaceInWindowMode(space))
            item->setSpace(space);
    }
    else
    {
        if(!space)
            space = new Space();

        item = createWindowItem(windowName, parentItem, space, style, QPoint(x,y), QSize(w,h));
        item->setClass(cls);
        item->spaceWidget()->show();
        insert(item);
        hyperJumpManager->registerWindow(item->spaceWidget());
    }

    return item ? item->handle() : 0;
}

int SpaceWindowManager::openSchemeWindow(const QString& windowName, const QString& className, const QString& style)
{
    SpaceWindowItem* item = findByName(windowName);

    Scheme* scheme = nullptr;
    Class* cls = LibraryCollection::getClassByName(className);
    if(cls)
        scheme = cls->scheme();

    if(Space* space = Space::createFromScheme(scheme))
    {
        if(item && !isSpaceInWindowMode(space))
        {
            item->setSpace(space);
        }
        else
        {
            item = createWindowItem(windowName, nullptr, space, style);
            item->setClass(cls);
            item->spaceWidget()->show();
            insert(item);
            hyperJumpManager->registerWindow(item->spaceWidget());
        }
    }
    return item ? item->handle() : 0;
}

int SpaceWindowManager::loadSchemeWindow(const QString& windowName, const QString& fileName,
                                         const QString& style)
{
    SpaceWindowItem* item = findByName(windowName);
    {
        QString filePath = executedProject->projectDir().absoluteFilePath(fileName);
        Space* space = Space::createFromFile(filePath);
        if(!space)
            space = new Space();

        if(item && !isSpaceInWindowMode(space))
        {
            item->setSpace(space);
        }
        else
        {
            item = createWindowItem(windowName, nullptr, space, style);
            item->spaceWidget()->show();
            insert(item);
            hyperJumpManager->registerWindow(item->spaceWidget());
        }
    }
    return item ? item->handle() : 0;
}

bool SpaceWindowManager::closeClassScheme(const QString& className)
{
    if(Class* cls = LibraryCollection::getClassByName(className))
    {
        for(SpaceWindowItem* item : m_windowItems.items())
            if(cls == item->cls())
            {
                closeWindow(item->windowName());
                return true;
            }
    }
    return false;
}

int SpaceWindowManager::closeWindow(const QString& windowName)
{
    if(SpaceWindowItem* winItem = take(windowName))
    {
        delete winItem;
        return 1;
    }
    return 0;
}

QString SpaceWindowManager::getWindowName(int spaceHandle)
{
    if(SpaceWindowItem* item = getItem(spaceHandle))
        return item->windowName();
    return "";
}

int SpaceWindowManager::getWindowSpace(const QString& windowName)
{
    if(SpaceWindowItem* item = getItem(windowName))
        return item->handle();
    return 0;
}

int SpaceWindowManager::getClientWidth(const QString& windowName)
{
    if(SpaceWindowItem* item = getItem(windowName))
        return item->spaceWidget()->wndActualSize().width();

    return 0;
}

int SpaceWindowManager::getClientHeight(const QString& windowName)
{
    if(SpaceWindowItem* item = getItem(windowName))
        return item->spaceWidget()->wndActualSize().height();

    return 0;
}

int SpaceWindowManager::setClientSize(const QString& windowName, int w, int h)
{
    if(SpaceWindowItem* item = getItem(windowName))
    {
        item->resize(w, h);
        return 1;
    }
    return 0;
}

int SpaceWindowManager::getWindowOrgX(const QString& windowName)
{
    if(SpaceWindowItem* item = getItem(windowName))
        return item->spaceWidget()->x();

    return 0;
}

int SpaceWindowManager::getWindowOrgY(const QString& windowName)
{
    if(SpaceWindowItem* item = getItem(windowName))
        return item->spaceWidget()->y();

    return 0;
}

int SpaceWindowManager::setWindowOrg(const QString& windowName, int x, int y)
{
    if(SpaceWindowItem* item = getItem(windowName))
    {
        item->move(x, y);
        return 1;
    }
    return 0;
}

int SpaceWindowManager::getWindowWidth(const QString& windowName)
{
    if(SpaceWindowItem* item = getItem(windowName))
        return item->spaceWidget()->frameGeometry().width();

    return 0;
}

int SpaceWindowManager::getWindowHeight(const QString& windowName)
{
    if(SpaceWindowItem* item = getItem(windowName))
        return item->spaceWidget()->frameGeometry().height();

    return 0;
}

bool SpaceWindowManager::setWindowSize(const QString& windowName, int w, int h)
{
    if(SpaceWindowItem* item = getItem(windowName))
    {
        QRect rect = item->spaceWidget()->frameGeometry();
        int dw = rect.width() - item->spaceWidget()->width();
        int dh = rect.height() - item->spaceWidget()->height();

        item->resize(w - dw, h - dh);
        return true;
    }
    return false;
}

bool SpaceWindowManager::setWindowPos(const QString& windowName, int x, int y, int w, int h)
{
    if(SpaceWindowItem* item = getItem(windowName))
    {
        item->move(x, y);
        item->resize(w, h);
        return true;
    }
    return false;
}

QString SpaceWindowManager::getWindowTitle(const QString& windowName)
{
    if(SpaceWindowItem* item = getItem(windowName))
        return item->spaceWidget()->windowTitle();
    return "";
}

int SpaceWindowManager::setWindowTitle(const QString& windowName, const QString& title)
{
    if(SpaceWindowItem* item = getItem(windowName))
    {
        item->spaceWidget()->setWindowTitle(title);
        return 1;
    }
    return 0;
}

int SpaceWindowManager::setWindowTransparentW(const QString windowName, int transparency)
{
    if(SpaceWindowItem* item = getItem(windowName))
    {
        qreal opacity = (transparency) / 100.0;
        item->spaceWidget()->setWindowOpacity(opacity);
        return 1;
    }
    return 0;
}

int SpaceWindowManager::setWindowTransparentH(int spaceHandle, int transparency)
{
    QString windowName = getWindowName(spaceHandle);
    return setWindowTransparentW(windowName, transparency);
}

bool SpaceWindowManager::setWindowTransparentColorW(const QString& windowName, const QColor& color)
{
    if(SpaceWindowItem* item = getItem(windowName))
    {
        QRect windowRect = item->spaceWidget()->rect();
        QPixmap pixmap(windowRect.size());
        item->spaceWidget()->render(&pixmap, windowRect.topLeft(), QRegion(windowRect));

        QBitmap mask = pixmap.createMaskFromColor(color);
        //        mask.save(executedProject->projectDir().absoluteFilePath("123.bmp"));
        item->spaceWidget()->setMask(mask);

        return true;
    }
    return false;
}

bool SpaceWindowManager::setWindowTransparentColorH(int spaceHandle, const QColor& color)
{
    QString windowName = getWindowName(spaceHandle);
    return setWindowTransparentColorW(windowName, color);
}

int SpaceWindowManager::setWindowRegionW(const QString& windowName, Array* array)
{
    if(array == nullptr || array->length() == 0 || array->count() %2 != 0)
        return 0;

    if(SpaceWindowItem* item = getItem(windowName))
    {
        QPolygon polygon;
        for(int i = 0; i < array->length(); i += 2)
        {
            if(!StandardTypes::isFloat(array->at(i)->type()))
                return 0;

            double x = array->at(i)->var()->doubleValue();
            double y = array->at(i + 1)->var()->doubleValue();
            polygon << QPoint(x, y);
        }

        item->spaceWidget()->setMask(QRegion(polygon));
        return 1;
    }
    return 0;
}

int SpaceWindowManager::setWindowRegionH(int spaceHandle, Array* array)
{
    QString windowName = getWindowName(spaceHandle);
    return setWindowRegionW(windowName, array);
}

int SpaceWindowManager::showWindow(const QString windowName, int attribute)
{
    if(SpaceWindowItem* item = getItem(windowName))
    {
        //        if(item->parentItem())
        //        {
        //            if(attribute == 0)
        //                item->proxyWidget()->hide();

        //            else if(attribute == 5)
        //                item->proxyWidget()->show();
        //        }
        //        else
        {
            item->spaceWidget()->setAttribute(Qt::WA_ShowWithoutActivating, false);
            switch(attribute)
            {
                case 0: //SW_HIDE
                    item->spaceWidget()->hide();
                    break;

                case 1: //SW_NORMAL
                    item->spaceWidget()->showNormal();
                    break;

                case 2: //SW_SHOWMINIMIZED
                    //                item->window()->setWindowState(Qt::WindowMinimized);
                    item->spaceWidget()->showMinimized();
                    break;

                case 3: //SW_SHOWMAXIMIZED
                    item->spaceWidget()->showMaximized();
                    break;

                case 4:
                case 8: //SW_SHOWNOACTIVATE
                    item->spaceWidget()->setAttribute(Qt::WA_ShowWithoutActivating, true);
                    break;

                case 5: //SW_SHOW
                    item->spaceWidget()->show();
                    break;

                case 6: //SW_MINIMIZE
                    item->spaceWidget()->setWindowState(Qt::WindowMinimized);
                    break;

                case 7: //SW_SHOWMINNOACTIVE
                    item->spaceWidget()->setWindowState(Qt::WindowMinimized);
                    item->spaceWidget()->setAttribute(Qt::WA_ShowWithoutActivating, true);
                    break;

                case 9: //SW_RESTORE
                    item->spaceWidget()->setWindowState(Qt::WindowActive);
                    break;
            }
        }
        return 1;
    }
    return 0;
}

int SpaceWindowManager::bringWindowToTop(const QString windowName)
{
    if(SpaceWindowItem* item = getItem(windowName))
    {
        item->spaceWidget()->activateWindow();
        item->spaceWidget()->raise();
        return 1;
    }
    return 0;
}

int SpaceWindowManager::isIconic(const QString& windowName)
{
    if(SpaceWindowItem* item = getItem(windowName))
        return item->spaceWidget()->isMinimized();

    return 0;
}

int SpaceWindowManager::isWindowExist(const QString& windowName)
{
    return (getItem(windowName) != nullptr);
}

int SpaceWindowManager::isWindowVisible(const QString& windowName)
{
    if(SpaceWindowItem* item = getItem(windowName))
        return item->spaceWidget()->isVisible();

    return 0;
}

QString SpaceWindowManager::getWindowProp(const QString& windowName, const QString& propName)
{
    if(SpaceWindowItem* item = getItem(windowName))
    {
        Space* space = item->spaceWidget()->space();
        if(propName.compare("classname", Qt::CaseInsensitive) == 0)
        {

            return space->ownerClass() ? space->ownerClass()->originalName() : "";
        }
        else if(propName.compare("filename", Qt::CaseInsensitive) == 0)
            return space->fileName();
    }
    return "";
}

void SpaceWindowManager::clear(Project* project)
{
    QList<SpaceWindowItem*> items = m_windowItems.items();
    for(SpaceWindowItem* windowItem : items)
        if(windowItem->project() == project)
        {
            if(windowItem->parentItem() == nullptr)
                m_windowItems.deleteItem(windowItem->handle());
            else
                m_windowItems.remove(windowItem->handle());
        }
}

void SpaceWindowManager::clearAll()
{
    for(SpaceWindowItem* windowItem : m_windowItems.items())
        if(windowItem->parentItem() == nullptr)
            m_windowItems.deleteItem(windowItem->handle());
        else
            m_windowItems.remove(windowItem->handle());
}

void SpaceWindowManager::windowClosed()
{
    if(auto windowItem = dynamic_cast<SpaceWindowItem*>(sender()))
    {
        m_windowItems.remove(windowItem->handle());

        int windowCount = 0;
        Project* project = windowItem->project();
        QList<SpaceWindowItem*> items = m_windowItems.items();

        for(SpaceWindowItem* wndItem : items)
            if(wndItem->project() == project)
                windowCount++;

        if(windowCount == 0)
            execFlags |= EF_MUSTSTOP;
    }
}

SpaceWidget* SpaceWindowManager::getWidget(const QString& windowName)
{
    SpaceWidget* window = nullptr;
    if(SpaceWindowItem* item = getItem(windowName))
        window = item->spaceWidget();

    return window;
}

SpaceWidget* SpaceWindowManager::getWidget(int spaceHandle)
{
    SpaceWidget* window = nullptr;
    if(SpaceWindowItem* item = getItem(spaceHandle))
        window = item->spaceWidget();

    return window;
}

SpaceScene* SpaceWindowManager::getSpaceScene(int spaceHandle)
{
    if(auto w = getWidget(spaceHandle))
        return w->spaceScene();

    return nullptr;
}

SpaceWindowItem* SpaceWindowManager::getWindowItem(const QString& windowName)
{
    return getItem(windowName);
}

SpaceWindowItem* SpaceWindowManager::getWindowItem(int spaceHandle)
{
    return getItem(spaceHandle);
}

SpaceWindowItem* SpaceWindowManager::take(const QString& name)
{
    SpaceWindowItem* item = findByName(name);
    if(item)
        m_windowItems.remove(item->handle());

    return item;
}

SpaceWindowItem*SpaceWindowManager::take(int handle)
{
    return m_windowItems.take(handle);
}

void SpaceWindowManager::remove(const QString& name)
{
    if(SpaceWindowItem* item = findByName(name))
        m_windowItems.remove(item->handle());
}

void SpaceWindowManager::remove(int handle)
{
    m_windowItems.remove(handle);
}

SpaceWindowItem* SpaceWindowManager::getItem(const QString& name)
{
    return findByName(name);
}

SpaceWindowItem* SpaceWindowManager::getItem(int spaceHandle)
{
    return m_windowItems.getItem(spaceHandle);
}

int SpaceWindowManager::insert(SpaceWindowItem* item)
{
    return m_windowItems.insert(item);
}

SpaceWindowItem* SpaceWindowManager::findByName(const QString& name)
{
    for(SpaceWindowItem* wndItem : m_windowItems.items())
        if(wndItem->windowName() == name)
            return wndItem;

    return nullptr;
}

SpaceWindowItem* SpaceWindowManager::createWindowItem(const QString& windowName, SpaceWindowItem* parentItem,
                                                      Space* space, const QString& style,
                                                      const QPoint& pos, const QSize& size)
{
    SpaceWindowItem* item = new SpaceWindowItem(windowName, parentItem, space, style, pos, size);
    connect(item, &SpaceWindowItem::windowClosed, this, &SpaceWindowManager::windowClosed);
    return item;
}

bool SpaceWindowManager::isSpaceInWindowMode(Space* space)
{
    if(space == nullptr || space->hyperKey() == nullptr)
        return false;

    Qt::WindowFlags wndFlags = space->hyperKey()->windowFlags();

    if((wndFlags & Qt::Popup) != 0)
        return true;

    if((wndFlags & Qt::Dialog) != 0)
        return true;

    return false;
}

}
