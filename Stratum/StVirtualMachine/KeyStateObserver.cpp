#include "KeyStateObserver.h"
#include "VmGlobal.h"
#include "SystemFunctions.h"

#include <QKeyEvent>
#include <QApplication>

namespace StVirtualMachine {

KeyStateObserver::KeyStateObserver(QObject *parent) :
    QObject(parent),
    m_observe(false)
{
    QApplication::instance()->installEventFilter(this);
}

void KeyStateObserver::start()
{
    m_observe = true;
}

void KeyStateObserver::stop()
{
    m_observe = false;
}

bool KeyStateObserver::eventFilter(QObject*, QEvent* event)
{
    if(!m_observe)
        return false;

    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        pressedKeys << keyEvent->nativeVirtualKey();
    }
    if(event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(!keyEvent->isAutoRepeat())
            pressedKeys.remove(keyEvent->nativeVirtualKey());
    }
    if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        pressedKeys << mouseEvent->button();
    }
    if(event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        pressedKeys.remove(mouseEvent->button());
    }
    if(event->type() == QEvent::MouseMove)
    {}
    return false;
}

}
