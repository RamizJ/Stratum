#include "ClassFunctions.h"

#include "VmGlobal.h"
#include "VmCodes.h"
#include "VmLog.h"
#include "VirtualMachine.h"
#include "Context.h"

#include <Class.h>
#include <Object.h>
#include <LibraryCollection.h>
#include <Array.h>
#include <Library.h>
#include <QDateTime>
#include <ObjectInfo.h>
#include <ClassVarInfo.h>
#include <Compiler.h>

#include <memory>
#include <QApplication>

#ifdef QT_DEBUG
#include <DebugFunctions.h>
#endif

using namespace StData;
using namespace StSpace;
using namespace StCompiler;

namespace StVirtualMachine {

void setupClassFunctions()
{
    operations[V_GETCLASS] = getClassName;
    operations[GETCHILDCLASS] = getObjectClass;
    operations[VM_SETIMAGENAME] = setObjectName;
    operations[VM_GETPROJECTCLASSES] = getProjectClasses;
    operations[VM_GETCLASSFILE] = getClassFile;
    operations[GETCHILDCOUNT] = getObjectCount;
    operations[V_GETSCHEMEOBJECT] = getHObjectByName;
    operations[GETCHILDIDBYNUM] = getHObjectByNum;
    operations[VM_GETCALCORDER] = getCalcOrder;
    operations[VM_SETCALCORDER] = setCalcOrder;
    operations[VM_GETUNIQUCLASSNAME] = getUniqueClassName;
    operations[VM_GETMODELTEXT] = getModelText;
    operations[VM_SETMODELTEXT] = setModelText;
    operations[V_GETVARH] = getVarH;
    operations[V_GETVARF] = getVarF;
    operations[V_GETVARS] = getVarS;
    operations[V_SETVARH] = setVarH;
    operations[V_SETVARF] = setVarF;
    operations[V_SETVARS] = setVarS;
    operations[VM_GETVARCOUNT] = getVarCount;
    operations[VM_GETVARINFO] = getVarInfo;
    operations[VM_GETVARINFOFULL] = getVarInfoFull;
    operations[VM_GETNAMEBYHANDLE] = getNameByHandle;
    operations[VM_SETVARTODEF] = setVarsToDefault;
    operations[V_CLOSEALL] = closeAll;
    operations[V_STOP] = stop;
    operations[V_QUITSC] = quit;
    operations[GETOBJECTHANDLE] = getHObject;
}

void getProjectClasses()
{
    bool withExtraLibraries = valueStack->popDouble();

    int arrayHandle = arrayManager->createArray();
    if(Array* array = arrayManager->getArray(arrayHandle))
    {
        LibraryCollection* libCollection = executedProject->libraryCollection();
        Class* stringClass = libCollection->classByName("string");

        QList<Class*> classes = libCollection->projectLibrary()->classList();
        if(withExtraLibraries)
            for(ExtraLibrary* extraLib : libCollection->extraLibraries())
                classes.append(extraLib->classList());

        for(Class* cls : classes)
        {
            ArrayItem* item = new ArrayItem(stringClass);
            item->var()->setString(cls->originalName());
            array->insertItem(item);
        }
    }
    valueStack->pushInt32(arrayHandle);
}

void createClass()
{
    throw std::runtime_error("createClass - function not implemented");
}

void deleteClass()
{
    throw std::runtime_error("deleteClass - function not implemented");
}

void getUniqueClassName()
{
    QString prefixName = valueStack->popString();

    if(prefixName.count() < 3)
        prefixName = prefixName.leftJustified(4, '_');

    QDateTime dateTime = QDateTime::currentDateTime();

    QString uniqueClassName = QString("%1_%2%3%4%5_%6%7%8")
                              .arg(prefixName)
                              .arg(QString::number(dateTime.time().hour(), 16))
                              .arg(QString::number(dateTime.time().minute(), 16))
                              .arg(QString::number(dateTime.time().second(), 16))
                              .arg(QString::number(dateTime.time().msec(), 16))
                              .arg(QString::number(dateTime.date().month(), 16))
                              .arg(QString::number(dateTime.date().day(), 16))
                              .arg(QString::number(dateTime.date().year(), 16));

    int i = 0;
    while(executedProject->getClassByName(uniqueClassName))
        i++;

    if(i > 0)
        uniqueClassName += QString("_%1").arg(i);

    valueStack->pushString(uniqueClassName);
}

void getClassName()
{
    QString path = valueStack->popString();

    QList<Object*> objects = executedObject->getObjectsByPath(path);
    QString className = objects.isEmpty() ? "" : objects.first()->cls()->originalName();

    valueStack->pushString(className);
}

void getClassFile()
{
    QString className = valueStack->popString();
    if(Class* cls = executedProject->getClassByName(className))
        valueStack->pushString(cls->fileInfo().baseName());
    else
        valueStack->pushString("");
}

void getCalcOrder()
{
    int objHandle = valueStack->popInt32();
    QString className = valueStack->popString();

    int index = -1;
    if(Class* cls = executedProject->getClassByName(className))
        index = cls->childIndexByHandle(objHandle) + 1;

    valueStack->pushDouble(index);
}

void setCalcOrder()
{
    int order = valueStack->popDouble();
    int objectHandle = valueStack->popInt32();
    QString className = valueStack->popString();

    bool result = false;
    if(Class* cls = executedProject->getClassByName(className))
        result = cls->setChildCalcOrder(objectHandle, order - 1);

    valueStack->pushDouble(result);
}

void createObject()
{
    throw std::runtime_error("createObject - function not implemented");
}

void deleteObject()
{
    throw std::runtime_error("deleteObject - function not implemented");
}

void getObjectCount()
{
    QString className = valueStack->popString();

    int objectsCount = 0;
    if(Class* cls = executedProject->getClassByName(className))
        objectsCount = cls->childsCount();

    valueStack->pushDouble(objectsCount);
}

void getHObject()
{
    if(executedObject)
        valueStack->pushInt32(executedObject->objectInfo()->handle());
    else
        valueStack->pushInt32(0);
}

void getHObjectByName()
{
    QString objectName = valueStack->popString();

    int handle = 0;
    QList<Object*> objectsByPath = executedObject->getObjectsByPath(objectName);
    if(!objectsByPath.isEmpty())
        handle = objectsByPath.first()->objectInfo()->handle();

    valueStack->pushInt32(handle);
}

void getHObjectByNum()
{
    int index = valueStack->popDouble();
    QString className = valueStack->popString();

    int handle = 0;
    if(Class* cls = executedProject->getClassByName(className))
        handle = cls->childHandleByIndex(index);

    valueStack->pushInt32(handle);
}

void getObjectClass()
{
    int objectHandle = valueStack->popInt32();
    QString containerClassName = valueStack->popString();

    QString className;
    if(Class* cls = executedProject->getClassByName(containerClassName))
        if(ObjectInfo* objInfo = cls->objectInfoByHandle(objectHandle))
            className = objInfo->cls()->originalName();

    valueStack->pushString(className);
}

void createLink()
{
    throw std::runtime_error("createLink - function not implemented");
}

void setLinkVars()
{
    throw std::runtime_error("setLinkVars - function not implemented");
}

void getLink()
{
    throw std::runtime_error("getLink - function not implemented");
}

void removeLink()
{
    throw std::runtime_error("removeLink - function not implemented");
}

void setModelText()
{
    int streamHandle = valueStack->popInt32();
    QString className = valueStack->popString();

    bool result = false;
    if(Class* cl = executedProject->getClassByName(className))
    {
        if(StreamItem* streamItem = streamManager->getStreamItem(streamHandle))
        {
            if(canModifyClass(cl) && !cl->isProtected())
            {
                QString modelText = streamItem->readAll();
                if(setClassText(cl, modelText))
                {
                    result = true;
                    updateVars();
                }
            }
        }
    }
    valueStack->pushDouble(result);
}



void getModelText()
{
    int streamHandle = valueStack->popInt32();
    QString className = valueStack->popString();

    bool result = false;
    if(Class* cl = executedProject->getClassByName(className))
    {

#ifdef QT_DEBUG
            DebugFunctions::SaveDataToFile("d:\\oldCodeText.txt", cl->vmCode());
#endif
        if(StreamItem* streamItem = streamManager->getStreamItem(streamHandle))
            result = streamItem->write(cl->text());
    }

    valueStack->pushDouble(result);
}

void getVarF()
{
    QString varName = valueStack->popString();
    QString objName = valueStack->popString();

    double value = 0;

    QList<Object*> objects = executedObject->getObjectsByPath(objName);
    if(!objects.isEmpty())
    {
        if(ObjectVar* var = objects.first()->varByName(varName))
            value = var->doubleValue();
        else
            VmLog::instance().warning(QString("Переменная '%1' не найдена").arg(varName));
    }

    valueStack->pushDouble(value);
}

void getVarS()
{
    QString varName = valueStack->popString();
    QString objName = valueStack->popString();

    QString value= "";

    QList<Object*> objects = executedObject->getObjectsByPath(objName);
    if(!objects.isEmpty())
    {
        if(ObjectVar* var = objects.first()->varByName(varName))
            value = var->stringValue();
        else
            VmLog::instance().warning(QString("Переменная '%1' не найдена").arg(varName));
    }

    valueStack->pushString(value);
}

void getVarH()
{
    QString varName = valueStack->popString();
    QString objName = valueStack->popString();

    int value = 0;

    QList<Object*> objects = executedObject->getObjectsByPath(objName);
    if(!objects.isEmpty())
    {
        if(ObjectVar* var = objects.first()->varByName(varName))
            value = var->intValue();
        else
            VmLog::instance().warning(QString("Переменная '%1' не найдена").arg(varName));
    }

    valueStack->pushInt32(value);
}

void setVarH()
{
    int value = valueStack->popInt32();
    QString varName = valueStack->popString();
    QString objName = valueStack->popString();

    QList<Object*> objects = executedObject->getObjectsByPath(objName);
    if(!objects.isEmpty())
        if(ObjectVar* var = objects.first()->varByName(varName))
            var->setInt(value);
}

void setVarF()
{
    double value = valueStack->popDouble();
    QString varName = valueStack->popString();
    QString objName = valueStack->popString();

    QList<Object*> objects = executedObject->getObjectsByPath(objName);
    if(!objects.isEmpty())
        if(ObjectVar* var = objects.first()->varByName(varName))
            var->setDouble(value);
}

void setVarS()
{
    QString value = valueStack->popString();
    QString varName = valueStack->popString();
    QString objName = valueStack->popString();

    QList<Object*> objects = executedObject->getObjectsByPath(objName);
    if(!objects.isEmpty())
        if(ObjectVar* var = objects.first()->varByName(varName))
            var->setString(value);
}

void getVarInfo()
{
    VarData* descriptionVar = valueStack->popVarData();
    VarData* defValueVar = valueStack->popVarData();
    VarData* typeVar = valueStack->popVarData();
    VarData* nameVar = valueStack->popVarData();

    int index = valueStack->popDouble();
    QString className = valueStack->popString();

    Class* cl = executedProject->getClassByName(className);
    bool isIndexValid = (index >=0 && index < cl->varInfoList().count());

    bool result = false;
    if(cl && isIndexValid)
    {
        ClassVarInfo* varInfo = cl->varInfoByIndex(index);
        descriptionVar->setString(varInfo->info());
        defValueVar->setString(varInfo->defValue());
        typeVar->setString(varInfo->varType()->originalName());
        nameVar->setString(varInfo->varName());
        result = true;
    }
    valueStack->pushDouble(result);
}

void getVarInfoFull()
{
    VarData* flagVar = valueStack->popVarData();
    VarData* descriptionVar = valueStack->popVarData();
    VarData* defValueVar = valueStack->popVarData();
    VarData* typeVar = valueStack->popVarData();
    VarData* nameVar = valueStack->popVarData();

    int index = valueStack->popDouble();
    QString className = valueStack->popString();

    Class* cl = executedProject->getClassByName(className);
    bool isIndexValid = (index >=0 && index < cl->varInfoList().count());

    bool result = false;
    if(cl && isIndexValid)
    {
        ClassVarInfo* varInfo = cl->varInfoByIndex(index);
        descriptionVar->setString(varInfo->info());
        defValueVar->setString(varInfo->defValue());
        typeVar->setString(varInfo->varType()->originalName());
        nameVar->setString(varInfo->varName());
        flagVar->setDouble(varInfo->flags());
        result = true;
    }
    valueStack->pushDouble(result);
}

void getVarCount()
{
    QString className = valueStack->popString();

    int varsCount = 0;
    if(Class* cl = executedProject->getClassByName(className))
        varsCount = cl->varInfoList().count();

    valueStack->pushDouble(varsCount);
}

void setObjectName()
{
    QString name = valueStack->popString();
    int objectHandle = valueStack->popInt32();
    QString className = valueStack->popString();

    bool result = false;
    if(Class* cls = executedProject->getClassByName(className))
        if(ObjectInfo* objInfo = cls->objectInfoByHandle(objectHandle))
        {
            objInfo->setName(name);
            result = true;
        }
    valueStack->pushDouble(result);
}

void getNameByHandle()
{
    int objectHandle = valueStack->popInt32();
    QString className = valueStack->popString();

    QString objectName;
    if(Class* cl = executedProject->getClassByName(className))
        if(ObjectInfo* objInfo = cl->objectInfoByHandle(objectHandle))
            objectName = objInfo->name();
    valueStack->pushString(objectName);
}

void saveObjectState()
{
    throw std::runtime_error("saveObjectState - function not implemented");
}

void loadObjectState()
{
    throw std::runtime_error("loadObjectState - function not implemented");
}

void setVarsToDefault()
{
    QString objPath = valueStack->popString();

    bool result = false;
    if(Object* object = executedObject->getObjectByPath(objPath))
    {
        object->toDefaultState();
        result = true;
    }
    valueStack->pushDouble(result);
}

void stop()
{
    if(valueStack->popDouble())
        virtualMachine->stop();
}

void quit()
{
    if(valueStack->popDouble())
        QApplication::quit();
}

void closeAll()
{
    execFlags |= EF_MUSTSTOP;
}

bool setClassText(Class* cl, const QString& modelText)
{
    if(cl == nullptr) return false;

    if(compiler == nullptr)
    {
        compiler = std::make_shared<Compiler>();
        if(!compiler->initialize())
            return false;
    }

    bool relinkVars;
    if(compiler->compile(cl, modelText, relinkVars))
    {
        if(relinkVars)
            executedProject->setupLinks(true);

        return true;
    }

    return false;
}

bool canModifyClass(Class* cls)
{
    Object* obj = executedObject;
    while(obj != nullptr)
    {
        if(obj->cls() == cls)
            return false;

        obj = obj->parentObject();
    }

    ContextPtr context;
    foreach (context, contextStack)
    {
        if(context->object->cls() == cls)
            return false;
    }

    return true;
}

}
