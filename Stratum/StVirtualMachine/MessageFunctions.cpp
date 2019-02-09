#include "MessageFunctions.h"
#include "SpaceWindowItem.h"
#include "VmCodes.h"
#include "VmGlobal.h"
#include "VmLog.h"
#include "Context.h"

#include <Object.h>
#include <SpaceItem.h>
#include <SpaceWidget.h>
#include <Space.h>
#include <SpaceScene.h>

using namespace StData;
using namespace StSpace;

namespace StVirtualMachine {

void setupMessageFunctions()
{
    operations[V_REGISTEROBJECT] = registerObjectH;
    operations[V_UNREGISTEROBJECT] = unregisterObjectH;
    operations[REGISTEROBJECTS] = registerObjectW;
    operations[UNREGISTEROBJECTS] = unregisterObjectW;
    operations[V_SETCAPTURE] = setCapture;
    operations[V_RELEASECAPTURE] = releaseCapture;
    operations[VM_SENDMESSAGE] = sendMessage;
}

void registerObject(SpaceWindowItem* spaceWindowItem, int objectHandle, const QString& path, int msgCode, int flags)
{
    if(spaceWindowItem)
    {
        if(!executedObject)
            return;

        QList<Object*> objectsByPath = executedObject->getObjectsByPath(path);
        Object* object = objectsByPath.isEmpty() ? nullptr : objectsByPath.first();

        SpaceItem* spaceItem = spaceWindowItem->spaceWidget()->space()->getSpaceItem(objectHandle);
        messageManager->registerObject(spaceWindowItem, object, spaceItem, msgCode, flags);
    }
}

void registerObjectH()
{
    int flags = valueStack->popDouble();
    int code = valueStack->popDouble();
    QString path = valueStack->popString();
    int objectHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWindowItem* winItem = windowManager->getWindowItem(spaceHandle))
        registerObject(winItem, objectHandle, path, code, flags);
}

void registerObjectW()
{
    int flags = valueStack->popDouble();
    int code = valueStack->popDouble();
    QString path = valueStack->popString();
    int objectHandle = valueStack->popInt32();
    QString wndName = valueStack->popString();

    if(SpaceWindowItem* winItem = windowManager->getWindowItem(wndName))
        registerObject(winItem, objectHandle, path, code, flags);
}

void unregisterObject(SpaceWindowItem* winItem, const QString& path, int msgCode)
{
    if(winItem)
    {
        Object* object = nullptr;
        QList<Object*> objects = executedObject->getObjectsByPath(path);
        if(!objects.isEmpty())
            object = objects.first();

        messageManager->unregisterObject(winItem, object, msgCode);
    }
}

void unregisterObjectH()
{
    int msgCode = valueStack->popDouble();
    QString path = valueStack->popString();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWindowItem* winItem = windowManager->getWindowItem(spaceHandle))
        unregisterObject(winItem, path, msgCode);
}

void unregisterObjectW()
{
    int msgCode = valueStack->popDouble();
    QString path = valueStack->popString();
    QString wndName = valueStack->popString();

    if(SpaceWindowItem* winItem = windowManager->getWindowItem(wndName))
        unregisterObject(winItem, path, msgCode);
}

void setCapture()
{
    int flag = valueStack->popDouble();
    QString path = valueStack->popString();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWindowItem* winItem = windowManager->getWindowItem(spaceHandle))
    {
        Object* object = executedObject->getObjectByPath(path);
        messageManager->setCapture(object, winItem, flag);
    }
}

void releaseCapture()
{
    messageManager->releaseCapture();
}

void sendMessage()
{
    int varNamesCount = codePointer->getCode();
    codePointer->incPosition();

    QList<QPair<QString, QString>> varNames;
    for(int i = 0; i < varNamesCount; i += 2)
    {
        QString targetVarName = valueStack->popString();
        QString sourceVarName = valueStack->popString();
        varNames << QPair<QString, QString>(sourceVarName, targetVarName);
    }

    QString className = valueStack->popString();
    QString objectPath = valueStack->popString();

    if(pushContext())
    {
        messageManager->sendMessage(executedObject, className, objectPath, varNames);
    }
    popContext();
}


}
