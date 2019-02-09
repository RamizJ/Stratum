#include "MessageManager.h"
#include "MessageHandler.h"
#include "Context.h"
#include "VmGlobal.h"
#include "VmLog.h"
#include "SpaceWindowItem.h"

#include <Object.h>
#include <QEvent>
#include <SpaceWidget.h>

#include <Class.h>
#include <LibraryCollection.h>
#include <ObjectVar.h>
#include <QPair>

#include <QDebug>
#include <Space.h>
#include <StandardTypes.h>

using namespace StData;
using namespace StSpace;

namespace StVirtualMachine {

MessageManager::MessageManager(VirtualMachine* virtualMachine, QObject* parent) :
    QObject(parent),
    m_virtualMachine(virtualMachine),
    m_capturedHandler(nullptr)
{}

bool MessageManager::registerObject(SpaceWindowItem* spaceWindowItem, Object* object, SpaceItem* spaceItem,
                                    int code, int flag)
{
    if(!object || !spaceWindowItem)
        return false;

    if(isMessageExist(object, spaceItem, code))
        return false;

    MessageHandlerPtr msgHandler = create(spaceWindowItem, object, spaceItem, code, flag);
    if(msgHandler)
        m_messageHandlers << msgHandler;

    connect(spaceWindowItem, &SpaceWindowItem::windowClosed, this, &MessageManager::windowClosed);

    return true;
}

bool MessageManager::unregisterObject(SpaceWindowItem* spaceWindowItem, Object* object, int msgCode)
{
    if(spaceWindowItem == nullptr || object == nullptr)
        return false;

    for(int i = m_messageHandlers.count() - 1; i >= 0; i--)
    {
        MessageHandlerPtr msgHandler = m_messageHandlers.at(i);
        if(msgHandler->spaceWidget() == spaceWindowItem->spaceWidget() &&
           (msgCode == 0 || msgHandler->code() == msgCode) &&
           msgHandler->object() == object)
        {
            if(msgHandler == m_capturedHandler)
                m_capturedHandler = nullptr;

            m_messageHandlers.removeAt(i);
        }
    }
    return true;
}

void MessageManager::unregisterObjects(SpaceWindowItem* spaceWindowItem)
{
    for(int i = m_messageHandlers.count() - 1; i >= 0; i--)
    {
        MessageHandlerPtr msgHandler = m_messageHandlers.at(i);
        if(msgHandler->spaceWidget() == spaceWindowItem->spaceWidget())
        {
            if(msgHandler == m_capturedHandler)
                m_capturedHandler = nullptr;

            m_messageHandlers.removeAt(i);
        }
    }
}

void MessageManager::sendMessage(Object* sourceObject, const QString& className, const QString& targetObjectsPath,
                                 const QList<QPair<QString, QString>>& varNames)
{
    QList<Object*> targetObjects;

    Class* targetClass = nullptr;
    if(!className.isEmpty())
    {
        targetClass = LibraryCollection::getClassByName(className);
        if(!targetClass)
            return;
    }

    if(!targetObjectsPath.isEmpty())
    {
        targetObjects = sourceObject->getObjectsByPath(targetObjectsPath);

        for(int i = targetObjects.count() - 1; i >= 0; i--)
            if(targetClass && targetObjects[i]->cls() != targetClass)
                targetObjects.removeAt(i);
    }
    else if(targetClass)
        targetObjects = targetClass->objects();

    if(targetObjects.isEmpty())
        return;

    for(Object* targetObject : targetObjects)
    {
        if(targetObject == sourceObject)
            continue;

        //Устанавливаем переменные в целевом объекте
        for(QPair<QString, QString> sourceTargetVarName : varNames)
        {
            ObjectVar* sourceVar = sourceObject->varByName(sourceTargetVarName.first);
            ObjectVar* targetVar = targetObject->varByName(sourceTargetVarName.second);

            if(sourceVar && targetVar)
                targetVar->setVar(sourceVar);
        }

        //Выполняем код объекта
        execute(targetObject, true);
        targetObject->saveState();

        //Восстанавливаем переменные в исходном объекте
        for(QPair<QString, QString> sourceTargetVarName : varNames)
        {
            ObjectVar* sourceVar = sourceObject->varByName(sourceTargetVarName.first);
            ObjectVar* targetVar = targetObject->varByName(sourceTargetVarName.second);

            if(sourceVar && targetVar)
                sourceVar->setVar(targetVar);
        }
    }
}

void MessageManager::reset()
{
    m_messageHandlers.clear();
}

void MessageManager::reset(StData::Project* project)
{
    for(int i = m_messageHandlers.count()-1; i >= 0; i--)
    {
        MessageHandlerPtr messageHandler = m_messageHandlers.at(i);
        if(SpaceWindowItem* spaceWindowItem = messageHandler->spaceWindowItem())
        {
            if(project == spaceWindowItem->project())
            {
                if(messageHandler == m_capturedHandler)
                    m_capturedHandler = nullptr;

                m_messageHandlers.removeOne(messageHandler);
            }
        }
    }
}

void MessageManager::setCapture(Object* object, SpaceWindowItem* spaceWindowItem, int /*flag*/)
{
    if(!object)
        return;

    if(m_capturedHandler)
    {
        if(m_capturedHandler->object() == object)
            return;

        m_capturedHandler->releaseCapture();
        m_capturedHandler = nullptr;
    }

    for(MessageHandlerPtr msgHandler : m_messageHandlers)
    {
        if(msgHandler->object() == object && msgHandler->spaceWidget() == spaceWindowItem->spaceWidget())
        {
            msgHandler->setCapture();
            m_capturedHandler = msgHandler;
            break;
        }
    }
}

void MessageManager::releaseCapture()
{
    if(m_capturedHandler)
    {
        m_capturedHandler->releaseCapture();
        m_capturedHandler = nullptr;
    }
}

void MessageManager::processSendedMessages()
{
    for(MessageHandlerPtr msgHandler : m_messageHandlers)
        msgHandler->processSendedMessages();
}

void MessageManager::windowClosed()
{
    if(SpaceWindowItem* spaceWindowItem = static_cast<SpaceWindowItem*>(sender()))
        unregisterObjects(spaceWindowItem);
}

bool MessageManager::isMessageExist(Object* object, SpaceItem* spaceItem, int code)
{
    for(MessageHandlerPtr messageHandler : m_messageHandlers)
    {
        if(messageHandler->object() == object && messageHandler->spaceItem() == spaceItem &&
           messageHandler->code() == code)
        {
            return true;
        }
    }

    return false;
}

MessageHandlerPtr MessageManager::create(SpaceWindowItem* spaceWindowItem,
                                         Object* object, SpaceItem* spaceItem,
                                         int code, int flag)
{
    if(!object)
    {
        VmLog::instance().warning("Object to message not found!");
        return nullptr;
    }

    ObjectVar* msgVar = object->varByName("msg");
    if(!msgVar || !StandardTypes::isFloat(msgVar->type()))
    {
        VmLog::instance().warning("Var (msg) of type FLOAT not found or type mismatch");
        return nullptr;
    }

    return std::make_shared<MessageHandler>(m_virtualMachine, spaceWindowItem,
                                            object, spaceItem, code, flag, this);
}

}
