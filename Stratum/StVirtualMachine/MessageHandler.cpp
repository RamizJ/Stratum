#include "MessageHandler.h"
#include "VirtualMachine.h"
#include "VmLog.h"
#include "Context.h"
#include "VmGlobal.h"
#include "MessageData.h"
#include "VmTypes.h"

#include <Object.h>
#include <ObjectVar.h>
#include <SpaceWidget.h>
#include <SpaceScene.h>
#include <Space.h>
#include <SpaceItem.h>
#include <SpaceView.h>
#include <StandardTypes.h>
#include <Class.h>
#include <ControlGraphicsItem.h>
#include <StDataGlobal.h>

#include <QEvent>
#include <QApplication>
#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QWindow>
#include <qevent.h>
#include <qgraphicssceneevent.h>

using namespace StData;
using namespace StSpace;

namespace StVirtualMachine {

/*-------------------------------------------------------------------------------------------------------------*/
MessageHandler::MessageHandler(VirtualMachine* virtualMachine, SpaceWindowItem* spaceWindowItem,
                               Object* object, SpaceItem* spaceItem,
                               int code, int flag, QObject* parent) :
    QObject(parent),
    m_virtualMachine(virtualMachine),
    m_spaceWindowItem(spaceWindowItem),
    m_spaceWidget(m_spaceWindowItem->spaceWidget()),
    m_object(object),
    m_spaceItem(spaceItem),
    m_code(code),
    m_flag(flag),
    m_inProcess(false),
    m_specialVars(100, nullptr),
    m_msg(nullptr),
    m_isCaptured(false)
{
    m_specialVars[VI_MSG] = m_msg = m_object->varByName("msg");
    m_specialVars[VI_XPOS] = m_object->varByName("xpos");
    m_specialVars[VI_YPOS] = m_object->varByName("ypos");
    m_specialVars[VI_FWKEYS] = m_object->varByName("fwkeys");
    m_specialVars[VI_WVKEY] = m_object->varByName("wvkey");
    m_specialVars[VI_REPEAT] = m_object->varByName("repeat");
    m_specialVars[VI_SCANCODE] = m_object->varByName("scancode");
    m_specialVars[VI_CANCLOSE] = m_object->varByName("canclose");
    m_specialVars[VI_MAXSIZEX] = m_object->varByName("maxsizex");
    m_specialVars[VI_MAXSIZEY] = m_object->varByName("maxsizey");
    m_specialVars[VI_MAXPOSITIONX] = m_object->varByName("maxpositionx");
    m_specialVars[VI_MAXPOSITIONY] = m_object->varByName("maxpositiony");
    m_specialVars[VI_MINTRACKSIZEX] = m_object->varByName("mintracksizex");
    m_specialVars[VI_MINTRACKSIZEY] = m_object->varByName("mintracksizey");
    m_specialVars[VI_MAXTRACKSIZEX] = m_object->varByName("maxtracksizex");
    m_specialVars[VI_MAXTRACKSIZEY] = m_object->varByName("maxtracksizey");
    m_specialVars[VI_FWSIZETYPE] = m_object->varByName("fwsizetype");
    m_specialVars[VI_NWIDTH] = m_object->varByName("nwidth");
    m_specialVars[VI_NHEIGHT] = m_object->varByName("nheight");
    m_specialVars[VI_IDITEM] = m_object->varByName("iditem");
    m_specialVars[VI_HOBJECT] = m_object->varByName("_hobject");
    m_specialVars[VI_NOTIFYCODE] = m_object->varByName("wnotifycode");
    m_specialVars[VI_TARGET] = m_object->varByName("_target");
    m_specialVars[VI_HSPACE] = m_object->varByName("_hspace");
    m_specialVars[VI_WNDNAME] = m_object->varByName("_windowname");

    if(code == WM_MOUSEMOVE || code == WM_ALLMOUSEMESSAGE)
        m_spaceWidget->spaceView()->setMouseTracking(true);

    m_spaceWidget->installEventFilter(this);
    m_spaceWidget->spaceScene()->installEventFilter(this);

    if(SpaceWindowItem* parentItem = m_spaceWindowItem->parentItem())
    {
        connect(parentItem->spaceWidget(), &SpaceWidget::sceneMouseEventOnParent,
                this, &MessageHandler::onSceneMouseEventOnParent);
    }

    if(m_spaceItem != nullptr)
    {
        SpaceGraphicsItem* spaceGraphicsItem = m_spaceWindowItem->spaceWidget()->spaceScene()->
                                               getSpaceGraphicsItem(m_spaceItem->handle());

        auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(spaceGraphicsItem);
        if(controlGraphicsItem != nullptr)
        {
            connect(controlGraphicsItem, &ControlGraphicsItem::editTextChanged, this,
                    &MessageHandler::editTextChanged);
            connect(controlGraphicsItem, &ControlGraphicsItem::editFocusIn, this,
                    &MessageHandler::editFocusIn);
            connect(controlGraphicsItem, &ControlGraphicsItem::editFocusOut, this,
                    &MessageHandler::editFocusOut);
            connect(controlGraphicsItem, &ControlGraphicsItem::buttonClicked, this,
                    &MessageHandler::buttonCLicked);
        }
    }
}

MessageHandler::~MessageHandler()
{
    disconnect();
}

//MessageHandlerPtr MessageHandler::create(VirtualMachine* virtualMachine, SpaceWindowItem* spaceWindowItem,
//                                         Object* object, SpaceItem* spaceItem,
//                                         int code, int flag, QObject* parent)
//{
//    if(!object)
//    {
//        VmLog::instance().warning("Object to message not found!");
//        return nullptr;
//    }

//    ObjectVar* msgVar = object->varByName("msg");
//    if(!msgVar || !StandardTypes::isFloat(msgVar->type()))
//    {
//        VmLog::instance().warning("Var (msg) of type FLOAT not found or type mismatch");
//        return nullptr;
//    }

//    return std::make_shared<MessageHandler>(virtualMachine, spaceWindowItem,
//                                            object, spaceItem, code, flag, parent);
//}

void MessageHandler::processSendedMessages()
{
    int messagesCount = m_sendedMessages.count();
    for(int i = 0; i < messagesCount; i++)
    {
        MessageDataPtr msgData = m_sendedMessages.dequeue();
        msgData->accept(this);
    }
}


bool MessageHandler::event(QEvent* event)
{
    if(m_virtualMachine->isJumpingState())
        return QObject::event(event);

    if(event->type() == QEvent::GraphicsSceneMousePress ||
       event->type() == QEvent::GraphicsSceneMouseMove ||
       event->type() == QEvent::GraphicsSceneMouseRelease)
        eventFilter(this, event);

    return QObject::event(event);
}

bool MessageHandler::eventFilter(QObject* sender, QEvent* event)
{
    if(m_inProcess || m_virtualMachine->isJumpingState() || m_object->project() != m_virtualMachine->project())
        return false;

    SpaceScene* scene = m_spaceWidget->spaceScene();

    switch(event->type())
    {
        case QEvent::GraphicsSceneMousePress:
            if(m_code == WM_LBUTTONDOWN || m_code == WM_MBUTTONDOWN || m_code == WM_RBUTTONDOWN ||
               m_code == WM_ALLMOUSEMESSAGE)
            {
                auto mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
                QPointF scenePos = mouseEvent->scenePos();

                QWidget* childWindow;
                if(isChildWindowUnderPoint(scene, scenePos, childWindow))
                    return false;

                //                if(dynamic_cast<QGraphicsProxyWidget*>(m_spaceWidget->parentWidget()))
                //                    scenePos += m_spaceWidget->pos();

                if((m_flag & 1) && m_spaceItem)
                {
                    int spaceItemHandle = m_spaceWidget->getObjectFromPoint2d(scenePos.x(), scenePos.y());
                    if(spaceItemHandle != m_spaceItem->handle() && !m_isCaptured)
                        return false;

                    else if(m_flag & 4)
                    {
                        double itemX = m_spaceWidget->spaceScene()->getObjectOrg2dX(spaceItemHandle);
                        double itemY = m_spaceWidget->spaceScene()->getObjectOrg2dY(spaceItemHandle);
                        scenePos.setX(scenePos.x() - itemX);
                        scenePos.setY(scenePos.y() - itemY);
                    }
                }

                if(mouseEvent->button() == Qt::LeftButton && (m_code == WM_LBUTTONDOWN || m_code == WM_ALLMOUSEMESSAGE))
                {
                    //                    setVarF(VI_XPOS, scenePos.x());
                    //                    setVarF(VI_YPOS, scenePos.y());
                    //                    setVarF(VI_FWKEYS, 1);
                    //                    send(WM_LBUTTONDOWN);
                    sendMouseMesssage(WM_LBUTTONDOWN, scenePos.x(), scenePos.y(), 1);
                }
                else if(mouseEvent->button() == Qt::RightButton && (m_code == WM_RBUTTONDOWN || m_code == WM_ALLMOUSEMESSAGE))
                {
                    //                    setVarF(VI_XPOS, scenePos.x());
                    //                    setVarF(VI_YPOS, scenePos.y());
                    //                    setVarF(VI_FWKEYS, 2);
                    //                    send(WM_RBUTTONDOWN);
                    sendMouseMesssage(WM_RBUTTONDOWN, scenePos.x(), scenePos.y(), 2);
                }
                else if(mouseEvent->button() == Qt::MiddleButton && (m_code == WM_MBUTTONDOWN || m_code == WM_ALLMOUSEMESSAGE))
                {
                    //                    setVarF(VI_XPOS, scenePos.x());
                    //                    setVarF(VI_YPOS, scenePos.y());
                    //                    setVarF(VI_FWKEYS, 16);
                    //                    send(WM_MBUTTONDOWN);
                    sendMouseMesssage(WM_MBUTTONDOWN, scenePos.x(), scenePos.y(), 16);
                }
            }
            break;

        case QEvent::GraphicsSceneMouseMove:
            if(m_code == WM_MOUSEMOVE || m_code == WM_ALLMOUSEMESSAGE)
            {
                auto mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
                QPointF scenePos = mouseEvent->scenePos();

                if(!m_spaceWidget->geometry().contains(mouseEvent->screenPos()) && !m_isCaptured)
                    return false;

                QWidget* childWindow;
                if(isChildWindowUnderPoint(scene, scenePos, childWindow))
                    return false;

                if((m_flag & 1) && m_spaceItem)
                {
                    int spaceItemHandle = m_spaceWidget->getObjectFromPoint2d(scenePos.x(), scenePos.y());
                    if(spaceItemHandle != m_spaceItem->handle() && !m_isCaptured)
                        return false;

                    else if(m_flag & 4)
                    {
                        double itemX = m_spaceWidget->spaceScene()->getObjectOrg2dX(spaceItemHandle);
                        double itemY = m_spaceWidget->spaceScene()->getObjectOrg2dY(spaceItemHandle);
                        scenePos.setX(scenePos.x() - itemX);
                        scenePos.setY(scenePos.y() - itemY);
                    }
                }

                double fwKeys = 0;
                if(mouseEvent->buttons() == Qt::LeftButton)
                {
                    //                    setVarF(VI_FWKEYS, 1);
                    fwKeys = 1;
                }
                if(mouseEvent->buttons() == Qt::RightButton)
                {
                    //                    setVarF(VI_FWKEYS, 2);
                    fwKeys = 2;
                }
                if(mouseEvent->buttons() == Qt::MiddleButton)
                {
                    //                    setVarF(VI_FWKEYS, 16);
                    fwKeys = 16;
                }

                //                setVarF(VI_XPOS, scenePos.x());
                //                setVarF(VI_YPOS, scenePos.y());
                //                send(WM_MOUSEMOVE);
                sendMouseMesssage(WM_MOUSEMOVE, scenePos.x(), scenePos.y(), fwKeys);
            }
            break;

        case QEvent::GraphicsSceneMouseRelease:
            if(m_code == WM_LBUTTONUP || m_code == WM_MBUTTONUP || m_code == WM_RBUTTONUP ||
               m_code == WM_ALLMOUSEMESSAGE)
            {
                auto mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
                QPointF scenePos = mouseEvent->scenePos();

                if(!m_spaceWidget->geometry().contains(mouseEvent->screenPos()) && !m_isCaptured)
                    return false;

                QWidget* childWindow;
                if(isChildWindowUnderPoint(scene, scenePos, childWindow))
                    return false;

                if((m_flag & 1) && m_spaceItem)
                {
                    int spaceItemHandle = m_spaceWidget->getObjectFromPoint2d(scenePos.x(), scenePos.y());
                    if(spaceItemHandle != m_spaceItem->handle() && !m_isCaptured)
                        return false;

                    else if(m_flag & 4)
                    {
                        double itemX = m_spaceWidget->spaceScene()->getObjectOrg2dX(spaceItemHandle);
                        double itemY = m_spaceWidget->spaceScene()->getObjectOrg2dY(spaceItemHandle);
                        scenePos.setX(scenePos.x() - itemX);
                        scenePos.setY(scenePos.y() - itemY);
                    }
                }

                //                setVarF(VI_FWKEYS, 0);

                if(mouseEvent->button() == Qt::LeftButton && (m_code == WM_LBUTTONUP || m_code == WM_ALLMOUSEMESSAGE))
                {
                    //                    setVarF(VI_XPOS, scenePos.x());
                    //                    setVarF(VI_YPOS, scenePos.y());
                    //                    send(WM_LBUTTONUP);
                    sendMouseMesssage(WM_LBUTTONUP, scenePos.x(), scenePos.y(), 0);

                }
                else if(mouseEvent->button() == Qt::RightButton && (m_code == WM_RBUTTONUP || m_code == WM_ALLMOUSEMESSAGE))
                {
                    //                    setVarF(VI_XPOS, scenePos.x());
                    //                    setVarF(VI_YPOS, scenePos.y());
                    //                    send(WM_RBUTTONUP);
                    sendMouseMesssage(WM_RBUTTONUP, scenePos.x(), scenePos.y(), 0);
                }
                else if(mouseEvent->button() == Qt::MiddleButton && (m_code == WM_MBUTTONUP || m_code == WM_ALLMOUSEMESSAGE))
                {
                    //                    setVarF(VI_XPOS, scenePos.x());
                    //                    setVarF(VI_YPOS, scenePos.y());
                    //                    send(WM_MBUTTONUP);
                    sendMouseMesssage(WM_MBUTTONUP, scenePos.x(), scenePos.y(), 0);
                }
            }
            break;

        case QEvent::GraphicsSceneMouseDoubleClick:
            if(m_code == WM_LBUTTONDBLCLK || m_code == WM_MBUTTONDBLCLK || m_code == WM_RBUTTONDBLCLK ||
               m_code == WM_ALLMOUSEMESSAGE)
            {
                auto mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
                QPointF scenePos = mouseEvent->scenePos();

                QWidget* childWindow = nullptr;
                if(isChildWindowUnderPoint(scene, scenePos, childWindow))
                    return false;

                if((m_flag & 1) && m_spaceItem)
                {
                    int spaceItemHandle = m_spaceWidget->getObjectFromPoint2d(scenePos.x(), scenePos.y());
                    if(spaceItemHandle != m_spaceItem->handle())
                        return false;

                    else if(m_flag & 4)
                    {
                        double itemX = m_spaceWidget->spaceScene()->getObjectOrg2dX(spaceItemHandle);
                        double itemY = m_spaceWidget->spaceScene()->getObjectOrg2dY(spaceItemHandle);
                        scenePos.setX(scenePos.x() - itemX);
                        scenePos.setY(scenePos.y() - itemY);
                    }
                }

                if(mouseEvent->button() == Qt::LeftButton && (m_code == WM_LBUTTONDBLCLK || m_code == WM_ALLMOUSEMESSAGE))
                {
                    //                    setVarF(VI_XPOS, scenePos.x());
                    //                    setVarF(VI_YPOS, scenePos.y());
                    //                    setVarF(VI_FWKEYS, 1);
                    //                    send(WM_LBUTTONDBLCLK);
                    sendMouseMesssage(WM_LBUTTONDBLCLK, scenePos.x(), scenePos.y(), 1);
                }
                else if(mouseEvent->button() == Qt::RightButton && (m_code == WM_RBUTTONDBLCLK || m_code == WM_ALLMOUSEMESSAGE))
                {
                    //                    setVarF(VI_XPOS, scenePos.x());
                    //                    setVarF(VI_YPOS, scenePos.y());
                    //                    setVarF(VI_FWKEYS, 2);
                    //                    send(WM_RBUTTONDBLCLK);
                    sendMouseMesssage(WM_RBUTTONDBLCLK, scenePos.x(), scenePos.y(), 2);
                }
                else if(mouseEvent->button() == Qt::MiddleButton && (m_code == WM_MBUTTONDBLCLK || m_code == WM_ALLMOUSEMESSAGE))
                {
                    //                    setVarF(VI_XPOS, scenePos.x());
                    //                    setVarF(VI_YPOS, scenePos.y());
                    //                    setVarF(VI_FWKEYS, 16);
                    //                    send(WM_MBUTTONDBLCLK);
                    sendMouseMesssage(WM_MBUTTONDBLCLK, scenePos.x(), scenePos.y(), 16);
                }
            }
            break;

        case QEvent::KeyPress:
            //sender == m_spaceWidget - необходимо для исключения дублирования такого же события от сцены
            if(sender == m_spaceWidget && (m_code == WM_KEYDOWN || m_code == WM_ALLKEYMESSAGE))
            {
                QPoint viewPos = m_spaceWidget->spaceView()->mapFromGlobal(QCursor::pos());
                QPointF scenePos = m_spaceWidget->spaceView()->mapToScene(viewPos);

                QWidget* childWindow = NULL;
                if(isChildWindowUnderPoint(scene, scenePos, childWindow))
                    return false;

                QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

                //                setVarF(VI_WVKEY, keyEvent->nativeVirtualKey());
                //                setVarF(VI_SCANCODE, keyEvent->nativeScanCode());
                //                setVarF(VI_REPEAT, keyEvent->isAutoRepeat());
                //                send(WM_KEYDOWN);
                sendKeyboardMessage(WM_KEYDOWN, keyEvent->nativeScanCode(),
                                    keyEvent->nativeVirtualKey(), keyEvent->isAutoRepeat());

                int key = keyEvent->key();
                if(key >= Qt::Key_Space)
                {
                    if(keyEvent->text().length() > 0)
                    {
//                        quint8 c = keyEvent->text().toLocal8Bit().at(0);
                        quint8 c = toWindows1251(keyEvent->text()).at(0);

                        //                        setVarF(VI_WVKEY, c);
                        //                        send(WM_CHAR);
                        sendKeyboardMessage(WM_CHAR, keyEvent->nativeScanCode(),
                                            c, keyEvent->isAutoRepeat());
                    }
                }
            }
            break;

        case QEvent::KeyRelease:
            //sender == m_spaceWidget - необходимо для исключения дублирования такого же события от сцены
            if(sender == m_spaceWidget && (m_code == WM_KEYUP || m_code == WM_ALLKEYMESSAGE))
            {
                QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
                if(keyEvent->isAutoRepeat())
                    return false;

                QPoint viewPos = m_spaceWidget->spaceView()->mapFromGlobal(QCursor::pos());
                QPointF scenePos = m_spaceWidget->spaceView()->mapToScene(viewPos);

                QWidget* childWindow = NULL;
                if(isChildWindowUnderPoint(scene, scenePos, childWindow))
                    return false;

                //                setVarF(VI_WVKEY, keyEvent->nativeVirtualKey());
                //                setVarF(VI_SCANCODE, keyEvent->nativeScanCode());
                //                setVarF(VI_REPEAT, keyEvent->isAutoRepeat());
                //                send(WM_KEYUP);
                sendKeyboardMessage(WM_KEYUP, keyEvent->nativeScanCode(),
                                    keyEvent->nativeVirtualKey(), keyEvent->isAutoRepeat());
            }
            break;

        case QEvent::Resize:
            if(m_code == WM_SIZE)
            {
                //если позиция курсора рядом с бортиком и нажата левая кнопка мыши
                bool nearLeftBorder = abs(QCursor::pos().x() - m_spaceWidget->x()) < 10;
                bool nearTopBorder = abs(QCursor::pos().y() - m_spaceWidget->y()) < 10;
                bool nearRightBorder = abs(QCursor::pos().x() - (m_spaceWidget->x() + m_spaceWidget->width())) < 10;
                bool nearBottomBorder = abs(QCursor::pos().y() - (m_spaceWidget->y() + m_spaceWidget->height())) < 10;
                bool nearBorder = nearLeftBorder || nearTopBorder || nearRightBorder || nearBottomBorder;

                if(nearBorder && QApplication::mouseButtons() && Qt::LeftButton)
                {
                    QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
                    //                    setVarF(VI_NWIDTH, resizeEvent->size().width());
                    //                    setVarF(VI_NHEIGHT, resizeEvent->size().height());
                    //                    send(WM_SIZE);
                    sendSizeMessage(WM_SIZE, resizeEvent->size().width(), resizeEvent->size().height(), 0);
                }
            }
            break;

        case QEvent::Move:
            if(m_code == WM_MOVE)
            {
                QMoveEvent* moveEvent = static_cast<QMoveEvent*>(event);
                //                setVarF(VI_XPOS, moveEvent->pos().x());
                //                setVarF(VI_YPOS, moveEvent->pos().y());
                //                send(WM_MOVE);
                sendMouseMesssage(WM_MOVE, moveEvent->pos().x(), moveEvent->pos().y(), 0);
            }break;

        case QEvent::Destroy:
            sendGeneralMessage(WM_DESTROY);
            //send(WM_DESTROY);
            break;

        case QEvent::Close:
            if(m_code == WM_CANCLOSE)
            {
                ObjectVar* canCloseVar = m_specialVars.at(VI_CANCLOSE);
                if(canCloseVar)
                    canCloseVar->setDouble(1);

                //send(WM_CANCLOSE);
                sendGeneralMessage(WM_CANCLOSE);

                if(canCloseVar && canCloseVar->doubleValue() == 0.0)
                {
                    event->ignore();
                    return true;
                }
            }
            else if(m_code == WM_CLOSE)
                sendGeneralMessage(WM_CLOSE);
            //send(WM_CLOSE);

            else if(m_code == WM_SPACEDONE)
                sendGeneralMessage(WM_SPACEDONE);
            //send(WM_SPACEDONE);

            event->accept();
            break;

        default:
            break;
    }
    return false;
}

void MessageHandler::visit(GeneralMessageData* msgData)
{
    processSendedMessage(msgData->messageCode());
}

void MessageHandler::visit(MouseMessageData* msgData)
{
    setVarF(VI_XPOS, msgData->x());
    setVarF(VI_YPOS, msgData->y());
    setVarF(VI_FWKEYS, msgData->keys());
    processSendedMessage(msgData->messageCode());
}

void MessageHandler::visit(KeyboardMessageData* msgData)
{
    setVarF(VI_WVKEY, msgData->wVkey());
    setVarF(VI_SCANCODE, msgData->scanCode());
    setVarF(VI_REPEAT, msgData->repeat());
    processSendedMessage(msgData->messageCode());
}

void MessageHandler::visit(CommandMessageData* msgData)
{
    setVarH(VI_HOBJECT, msgData->handle());
    setVarF(VI_IDITEM, msgData->Id());
    setVarF(VI_NOTIFYCODE, msgData->wNotifyCode());
    processSendedMessage(msgData->messageCode());
}

void MessageHandler::visit(SizeMessageData* msgData)
{
    setVarF(VI_NWIDTH, msgData->nWidth());
    setVarF(VI_NHEIGHT, msgData->nHeight());
    processSendedMessage(msgData->messageCode());
}

void MessageHandler::visit(MinMaxMessageData* /*msgData*/)
{}

void MessageHandler::onSceneMouseEventOnParent(QGraphicsSceneMouseEvent* parentEvent)
{
    if(m_virtualMachine->isJumpingState())
        return;

    bool canSendEvent = parentEvent->type() == QEvent::GraphicsSceneMouseRelease ||
                        (parentEvent->type() == QEvent::GraphicsSceneMouseMove && QApplication::mouseButtons());

    if(canSendEvent)
    {
        SpaceView* view = m_spaceWindowItem->spaceWidget()->spaceView();

        QPoint screenPos = parentEvent->screenPos();
        QPoint pos = view->mapFromGlobal(screenPos);
        QPointF scenePos = view->mapToScene(pos);

        QGraphicsSceneMouseEvent childEvent(parentEvent->type());
        childEvent.setScreenPos(screenPos);
        childEvent.setPos(pos);
        childEvent.setScenePos(scenePos);
        childEvent.setButtons(parentEvent->buttons());
        childEvent.setButton(parentEvent->button());
        childEvent.setModifiers(parentEvent->modifiers());

        QApplication::sendEvent(this, &childEvent);
    }
}

void MessageHandler::editTextChanged()
{
    auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(QObject::sender());
    if(controlGraphicsItem == nullptr)
        return;

    int handle = controlGraphicsItem->controlItem()->handle();
    int id = controlGraphicsItem->controlItem()->id();
    if(m_code == WM_COMMAND)
    {
        sendCommandMessage(WM_COMMAND, handle, 1024, id);
        sendCommandMessage(WM_COMMAND, handle, 768, id);
    }
    else if(m_code == WM_CONTROLNOTIFY)
    {
        sendCommandMessage(WM_CONTROLNOTIFY, handle, 1024, id);
        sendCommandMessage(WM_CONTROLNOTIFY, handle, 768, id);
    }
}

void MessageHandler::editFocusIn()
{
    auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(QObject::sender());
    if(controlGraphicsItem == nullptr)
        return;

    int handle = controlGraphicsItem->controlItem()->handle();
    int id = controlGraphicsItem->controlItem()->id();
    if(m_code == WM_COMMAND)
        sendCommandMessage(WM_COMMAND, handle, 256, id);
    else if(m_code == WM_CONTROLNOTIFY)
        sendCommandMessage(WM_CONTROLNOTIFY, handle, 256, id);
}

void MessageHandler::editFocusOut()
{
    auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(QObject::sender());
    if(controlGraphicsItem == nullptr)
        return;

    int handle = controlGraphicsItem->controlItem()->handle();
    int id = controlGraphicsItem->controlItem()->id();
    if(m_code == WM_COMMAND)
        sendCommandMessage(WM_COMMAND, handle, 512, id);
    else if(m_code == WM_CONTROLNOTIFY)
        sendCommandMessage(WM_CONTROLNOTIFY, handle, 512, id);
}

void MessageHandler::buttonCLicked()
{
    auto controlGraphicsItem = dynamic_cast<ControlGraphicsItem*>(QObject::sender());
    if(controlGraphicsItem == nullptr)
        return;

    int handle = controlGraphicsItem->controlItem()->handle();
    int id = controlGraphicsItem->controlItem()->id();
    if(m_code == WM_COMMAND)
        sendCommandMessage(WM_COMMAND, handle, 0, id);
    else if(m_code == WM_CONTROLNOTIFY)
        sendCommandMessage(WM_CONTROLNOTIFY, handle, 0, id);
}

void MessageHandler::setVarF(const int& index, const double& value)
{
    ObjectVar* var = m_specialVars.at(index);
    if(var)
    {
        var->setDouble(value);
        var->save();
    }
}

void MessageHandler::setVarS(const int& index, const QString& value)
{
    ObjectVar* var = m_specialVars.at(index);
    if(var)
    {
        var->setString(value);
        var->save();
    }
}

void MessageHandler::setVarH(const int& index, const int& value)
{
    ObjectVar* var = m_specialVars.at(index);
    if(var)
    {
        var->setInt(value);
        var->save();
    }
}

void MessageHandler::send(const MessageDataPtr& msgData)
{
    m_sendedMessages.enqueue(msgData);

    //    if(m_inProcess)
    //        return;

    //    m_inProcess = true;

    //    m_msg->setDouble(messageCode);
    //    m_msg->save();

    //    if(exec)
    //    {
    //        if(pushContext())
    //        {
    //            execute(m_object, true);
    //            m_object->saveState();
    //        }
    //        popContext();
    //    }

    //    m_inProcess = false;
}

void MessageHandler::sendGeneralMessage(int msgCode)
{
    GeneralMessageDataPtr msgData = make_shared<GeneralMessageData>();
    msgData->setMessageCode(msgCode);
    send(msgData);
}

void MessageHandler::sendMouseMesssage(int messageCode, double x, double y, double fwKeys)
{
    MouseMessageDataPtr msgData = std::make_shared<MouseMessageData>();
    msgData->setMessageCode(messageCode);
    msgData->setX(x);
    msgData->setY(y);
    msgData->setKeys(fwKeys);
    send(msgData);
}

void MessageHandler::sendKeyboardMessage(int messageCode, double scanCode, double wKey, double repeat)
{
    KeyboardMessageDataPtr msgData = std::make_shared<KeyboardMessageData>();
    msgData->setMessageCode(messageCode);
    msgData->setScanCode(scanCode);
    msgData->setWVkey(wKey);
    msgData->setRepeat(repeat);
    send(msgData);
}

void MessageHandler::sendSizeMessage(int msgCode, double w, double h, double type)
{
    SizeMessageDataPtr msgData = make_shared<SizeMessageData>();
    msgData->setMessageCode(msgCode);
    msgData->setNWidth(w);
    msgData->setNHeight(h);
    msgData->setFwSizeType(type);
    send(msgData);
}

void MessageHandler::sendCommandMessage(int msgCode, int handle, double notyfyCode, double id)
{
    CommandMessageDataPtr msgData = make_shared<CommandMessageData>();
    msgData->setMessageCode(msgCode);
    msgData->setHandle(handle);
    msgData->setWNotifyCode(notyfyCode);
    msgData->setId(id);
    send(msgData);
}

void MessageHandler::processSendedMessage(int messageCode)
{
    m_msg->setDouble(messageCode);
    m_msg->save();

    if(pushContext())
    {
        execute(m_object, true);
        m_object->saveState();
    }
    popContext();
}

bool MessageHandler::isChildWindowUnderPoint(SpaceScene* scene, const QPointF& scenePos, QWidget* childWindow)
{
    childWindow = nullptr;
    QGraphicsItem* item = scene->itemAt(scenePos.x(), scenePos.y(), QTransform());
    if(auto proxyWidget = dynamic_cast<QGraphicsProxyWidget*>(item))
        if(dynamic_cast<SpaceWidget*>(proxyWidget->widget()))
        {
            childWindow = proxyWidget->widget();
            return true;
        }

    if(dynamic_cast<ControlGraphicsItem*>(item))
        return true;

    return false;
}

SpaceWindowItem* MessageHandler::spaceWindowItem() const
{
    return m_spaceWindowItem;
}

}
