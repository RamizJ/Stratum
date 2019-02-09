#include "Class.h"
#include "ObjectVarsLinker.h"
#include "LinkInfo.h"
#include "Object.h"
#include "ObjectVar.h"
#include "Log.h"
#include "ClassVarInfo.h"

#include <QDebug>

namespace StData {

ObjectVarsLinker::ObjectVarsLinker(QObject* parent) :
    QObject(parent),
    m_rootObject(nullptr)
{}

ObjectVarsLinker::~ObjectVarsLinker()
{}

void ObjectVarsLinker::link(bool saveVarsData)
{
    if(!m_rootObject) return;

    QStack<VarData> varsDataStack;
    if(saveVarsData)
        storeVarsData(m_rootObject, &varsDataStack);

    cleanObjectVarsData(m_rootObject);
    linkVars(m_rootObject);

//    createUnlinkedVarsData(m_rootObject);

    if(saveVarsData)
        restoreVarsData(m_rootObject, &varsDataStack);
}

//void ObjectVarsLinker::linkObject(Object* containerObject, Object* object, QList<Object*>& handledObjects)
//{
//    if(handledObjects.contains(object))
//        return;

//    if(object)
//    {
//        handledObjects << object;
//        QList<LinkInfo*> objectLinks = getObjectLinks(containerObject, object);
//        for(LinkInfo* linkInfo : objectLinks)
//        {
//            if(linkInfo->flags() && LinkInfo::Disabled)
//                continue;

//            Object* sourceObj = containerObject->objectByHandle(linkInfo->sourceObjectHandle());
//            Object* targetObj = containerObject->objectByHandle(linkInfo->targetObjectHandle());

//            if(sourceObj && targetObj)
//            {
//                linkVars(linkInfo, sourceObj, targetObj);

//                Object* nextLinkObject = object == sourceObj ? targetObj : sourceObj;
//                linkObject(containerObject, nextLinkObject, handledObjects);
//            }
//        }
//    }
//    else
//        throw std::runtime_error("Error while linking object variables. Object is null");
//}

void ObjectVarsLinker::linkVars(Object* object)
{
    for(ObjectVar* var : object->variables())
    {
        if(!var->varData())
        {
            VarDataPtr varData = std::make_shared<VarData>();
            var->setVarData(varData);

            m_handledLinkedObjects.clear();
            QList<ObjectVar*> linkedVars = findLinkedVars(object, var);

            for(ObjectVar* linkedVar : linkedVars)
                linkedVar->setVarData(varData);
        }
    }

    for(Object* childObject : object->childObjects())
        linkVars(childObject);


    //    QList<Object*> handledSchemaObjects;
    //    for(Object* childObject : object->childObjects())
    //    {
    //        linkObject(object, childObject, handledSchemaObjects);
    //    }

//        for(Object* childObject : object->childObjects())
//            linkVars(childObject);
}

QList<ObjectVar*> ObjectVarsLinker::findLinkedVars(Object* object, ObjectVar* var)
{
    QList<ObjectVar*> result;

    auto findChildLinkedVars = [this](Object* container, Object* object, ObjectVar* var)
    {
        QList<ObjectVar*> linkedVars;
        QString varName = var->info()->varName();

        if(!container)
            return linkedVars;

        for(LinkInfo* link : container->cls()->links())
        {
            if(link->flags() & LinkInfo::Disabled)
                continue;

            Object* sourceObject = container->objectByHandle(link->sourceObjectHandle());
            Object* targetObject = container->objectByHandle(link->targetObjectHandle());

            if(sourceObject == nullptr || targetObject == nullptr)
            {
                QString errorStr = QString("Link error: #%1 <--> #%2 in '%3' schema")
                        .arg(link->sourceObjectHandle())
                        .arg(link->targetObjectHandle())
                        .arg(container->cls()->originalName());
                SystemLog::instance().error(errorStr);
                continue;
            }

            if(sourceObject == object || targetObject == object)
            {
                for(LinkEntryInfo* linkEntry : link->linkEntries())
                {
                    if(varName.compare(linkEntry->sourceVarName(), Qt::CaseInsensitive) == 0 && sourceObject == object)
                    {
                        if(ObjectVar* linkedVar = targetObject->varByName(linkEntry->targetVarName()))
                        {
                            linkedVars << linkedVar;
                            linkedVars.append(findLinkedVars(targetObject, linkedVar));
                        }
                    }

                    if(varName.compare(linkEntry->targetVarName(), Qt::CaseInsensitive) == 0 && targetObject == object)
                    {
                        if(ObjectVar* linkedVar = sourceObject->varByName(linkEntry->sourceVarName()))
                        {
                            linkedVars << linkedVar;
                            linkedVars.append(findLinkedVars(sourceObject, linkedVar));
                        }
                    }
                }
            }
        }
        return linkedVars;
    };

    if(m_handledLinkedObjects.contains(object))
        return result;
    m_handledLinkedObjects << object;

    //Обход дочерних связей
    result = findChildLinkedVars(object, object, var);

    //Обход родительских связей
    if(Object* parentObject = object->parentObject())
        result.append(findChildLinkedVars(parentObject, object, var));

    return result;
}

void ObjectVarsLinker::storeVarsData(Object* object, QStack<VarData>* varDataStack)
{
    for(ObjectVar* var : object->variables())
        varDataStack->push(*(var->varData()));

    for(Object* childObject : object->childObjects())
        storeVarsData(childObject, varDataStack);
}

void ObjectVarsLinker::restoreVarsData(Object* object, QStack<VarData>* varDataStack)
{
    for(ObjectVar* var : object->variables())
    {
        VarDataPtr varData = var->varData();
        *varData = varDataStack->pop();
    }

    for(Object* childObject : object->childObjects())
        restoreVarsData(childObject, varDataStack);
}

void ObjectVarsLinker::cleanObjectVarsData(Object* object)
{
    for(ObjectVar* var : object->variables())
        var->setVarData(nullptr);

    for(Object* childObject : object->childObjects())
        cleanObjectVarsData(childObject);
}

//void ObjectVarsLinker::createUnlinkedVarsData(Object* object)
//{
//    for(ObjectVar* var : object->variables())
//    {
//        if(!var->varData())
//            var->setVarData(std::make_shared<VarData>());
//    }

//    for(Object* childObject : object->childObjects())
//        createUnlinkedVarsData(childObject);
//}

//QList<LinkInfo*> ObjectVarsLinker::getObjectLinks(Object* containerObject, Object* object)
//{
//    QList<LinkInfo*> result;

//    for(LinkInfo* linkInfo : containerObject->cls()->links())
//    {
//        Object* sourceObject = containerObject->objectByHandle(linkInfo->sourceObjectHandle());
//        Object* targetObject = containerObject->objectByHandle(linkInfo->targetObjectHandle());

//        if(sourceObject == object || targetObject == object)
//            result << linkInfo;
//    }

//    return result;
//}

//void ObjectVarsLinker::linkVars(LinkInfo* link, Object* sourceObj, Object* targetObj)
//{
//    if(!link || !sourceObj || !targetObj)
//        return;

//    for(LinkEntryInfo* linkEntryInfo : link->linkEntries())
//    {
//        ObjectVar* sourceVar = sourceObj->varByName(linkEntryInfo->sourceVarName());
//        ObjectVar* targetVar = targetObj->varByName(linkEntryInfo->targetVarName());

//        if(!sourceVar || ! targetVar)
//        {
//            QString notExistedVarName = !sourceVar ? linkEntryInfo->sourceVarName() :
//                                                     linkEntryInfo->targetVarName();

//            SystemLog::instance().warning(QString("Link error '%1' <--> '%2'). Var not exist: %3")
//                                          .arg(sourceObj->cls()->originalName())
//                                          .arg(targetObj->cls()->originalName())
//                                          .arg(notExistedVarName));

//            continue;
//        }

//        if(sourceVar->varData() && targetVar->varData())
//        {
//            continue;
//        }
//        else if(!sourceVar->varData() && !targetVar->varData())
//        {
//            //TODO tryFindInitializedVar(sourceVar) and set initialized var data
//            VarDataPtr varData = std::make_shared<VarData>();
//            sourceVar->setVarData(varData);
//            targetVar->setVarData(varData);
//        }
//        else if(sourceVar->varData())
//        {
//            targetVar->setVarData(sourceVar->varData());
//        }
//        else if(targetVar->varData())
//        {
//            sourceVar->setVarData(targetVar->varData());
//        }
//    }
//}

}
