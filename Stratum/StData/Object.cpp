#include "Object.h"

#include "Class.h"
#include "ObjectInfo.h"
#include "ClassVarInfo.h"
#include "Project.h"
#include "VarManager.h"
#include "ObjectVar.h"
#include "LinkInfo.h"

#include <QDebug>
#include <QRegularExpression>

namespace StData {

Object::Object(Project* project, Class* cls, ObjectInfo* objectInfo, Object* parent) :
    QObject(parent),
    m_project(project),
    m_objectInfo(objectInfo),
    m_parentObject(parent),
    m_class(cls),
    m_flags(0),
    m_disableVar(nullptr),
    m_enableVar(nullptr)
{
    recreateVariables();
}

void Object::recreateVariables()
{
    qDeleteAll(m_variables);
    m_enableVar = nullptr;
    m_disableVar = nullptr;

    for(ClassVarInfo* varInfo : m_class->varInfoList())
    {
        ObjectVar* var = new ObjectVar(varInfo);
        m_variables << var;
        m_varByName[varInfo->varName().toLower()] = var;

        if(!m_enableVar && !m_disableVar)
        {
            if(varInfo->isEnableVar())
                m_enableVar = var;

            else if(varInfo->isDisableVar())
                m_disableVar = var;
        }
    }
}


Object::~Object()
{
    qDeleteAll(m_childObjectDictionary);
    qDeleteAll(m_varByName);

    m_childObjectDictionary.clear();
    m_varByName.clear();
}

int Object::childsCount()
{
    return m_childObjects.count();
}

Object*Object::childAt(int index)
{
    return m_childObjects.at(index);
}

void Object::addChild(Object* childObject)
{
    if(childObject)
    {
        int handle = childObject->objectInfo()->handle();
        if(!m_childObjectDictionary.contains(handle))
        {
            m_childObjects << childObject;
            m_childObjectDictionary[handle] = childObject;
        }
    }
}

Object* Object::childByHandle(qint16 handle)
{
    return m_childObjectDictionary.value(handle);
}

Object* Object::objectByHandle(qint16 handle)
{
    if(handle == 0)
        return this;

    return childByHandle(handle);
}

Object* Object::createByClass(Project* project, Class* cls, ObjectInfo* objectInfo)
{
    Object* result = nullptr;
    if(cls)
    {
        cls->load();

        result = new Object(project, cls, objectInfo);
        cls->addObject(result);

        //создание дочерних объектов
        for(ObjectInfo* objInfo : cls->objectInfoList())
        {
            //TODO проверка на рекурсию в родителях
            Class* objectClass = objInfo->cls();
            Object* childObject = createByClass(project, objectClass, objInfo);
            if(childObject)
            {
                childObject->setParentObject(result);
                result->addChild(childObject);
            }
        }
    }
    return result;
}

ObjectVar* Object::varByName(const QString& varName)
{
    return m_varByName.value(varName.toLower(), nullptr);
}

ObjectVar* Object::varByIndex(int index)
{
    return m_variables.at(index);
}

void Object::setVar(const QString& varName, const QString& varData)
{
    ObjectVar* var = m_varByName.value(varName.toLower(), nullptr);
    if(var)
    {
        var->setData(varData);
        var->save();
    }
}

void Object::setFlags(const qint32& flags)
{
    m_flags = flags;

    m_layer = ((m_flags & 0x1f00) >> 8);
    m_visible = ((m_flags & 1) == 0);
}

bool Object::isProcedure() const
{
    return m_class->isProcedure();
}

void Object::saveState()
{
    copyState(CopyState::Save);
}

void Object::restoreState()
{
    copyState(CopyState::Restore);
}

void Object::saveStartupState()
{
    for(Object* childObject : m_childObjects)
        childObject->saveStartupState();

    for(ObjectVar* var : m_variables)
        var->saveAsStartup();
}

void Object::toDefaultState()
{
    for(Object* childObject : m_childObjects)
        childObject->toDefaultState();

    for(ObjectVar* var : m_variables)
    {
        updateInternalVar(var);
        var->toDefaultState();
    }
}

void Object::toStartupState()
{
    for(Object* childObject : m_childObjects)
        childObject->toStartupState();

    for(ObjectVar* var : m_variables)
        var->toStartupState();
}

bool Object::updateInternalVar(ObjectVar* var)
{
    bool result = true;
    if(m_objectInfo == nullptr)
        return result;

    QString varName = var->info()->varName();

    if(varName.compare("orgx", Qt::CaseInsensitive) == 0)
        var->setDouble(m_objectInfo->position().x());

    else if(varName.compare("orgy", Qt::CaseInsensitive) == 0)
        var->setDouble(m_objectInfo->position().y());

    else if(varName.compare("_classname", Qt::CaseInsensitive) == 0)
        var->setString(m_class->originalName());

    else if(varName.compare("_objname", Qt::CaseInsensitive) == 0)
        var->setString(m_objectInfo->name());

    else if(varName.compare("_hobject", Qt::CaseInsensitive) == 0)
        var->setInt(m_objectInfo->handle());

    else
        result = false;

    if(result)
        var->save();

    return result;
}

Object* Object::rootObject()
{
    return m_project->rootObject();
}

QList<Object*> Object::getObjectsByPath(const QString& path)
{
    QList<Object*> result;

    Object* obj = isProcedure() && m_parentObject != nullptr ? m_parentObject : this;

    if(path.isEmpty())
    {
        result << obj;
        return result;
    }
    else
    {
        if(path[0] == '\\')
        {
            obj = rootObject();
            if(path.count() == 1)
            {
                result << obj;
                return result;
            }
        }

        //Проходим по пути до последнего элемента
        QStringList pathItems = path.split('\\', QString::SkipEmptyParts);
        for(int i = 0; i < pathItems.count() - 1 && obj; i++)
        {
            QString pathItem = pathItems[i];

            if(pathItem == ".." && obj->parentObject())
                obj = obj->parentObject();

            else if(pathItem[0] == '#' && (obj->parentObject() || obj == rootObject()))
            {
                Object* parentObject = obj == rootObject() ? rootObject() : obj->parentObject();
                int handle = pathItem.mid(1).toInt();
                obj = parentObject->childByHandle(handle);
                break;
            }
            else if(obj->parentObject() || obj == rootObject())
            {
                Object* parentObject = obj->parentObject() != nullptr ? obj->parentObject() : rootObject();

                bool childFound = false;
                QRegularExpression regExp(QString("^%1$").arg(pathItem.replace("?", "\\w")));
                regExp.setPatternOptions(regExp.patternOptions() | QRegularExpression::CaseInsensitiveOption);
                for(Object* childObject : parentObject->childObjects())
                {
                    QString objName = childObject->objectInfo()->name();
                    if(regExp.match(objName).hasMatch())
                    {
                        obj = childObject;
                        childFound = true;
                        break;
                    }
                }

                if(!childFound)
                    obj = nullptr;
            }
        }

        //Обрабатываем последний элемент
        if(obj && pathItems.count() > 0)
        {
            QString objectName = pathItems.last();
            if(!objectName.isEmpty())
            {
                if(objectName == "*")
                    return obj->childObjects();

                else if(objectName == "..")
                    result << obj->parentObject();

                else if(objectName[0] == '#')
                {
                    int handle = objectName.mid(1).toInt();
                    Object* objByHandle = obj->childByHandle(handle);
                    result << objByHandle;
                }
                else
                {
                    QRegularExpression regExp(QString("^%1$").arg(objectName.replace("?", "\\w")));
                    regExp.setPatternOptions(regExp.patternOptions() | QRegularExpression::CaseInsensitiveOption);
                    for(Object* childObject : obj->childObjects())
                    {
                        if(regExp.match(childObject->objectInfo()->name()).hasMatch())
                            result << childObject;
                    }
                }
            }
        }
    }
    return result;
}

Object* Object::getObjectByPath(const QString& path)
{
    if(path.isNull() || path.isEmpty())
        return this;

    QList<Object*> objects = getObjectsByPath(path);
    if(!objects.isEmpty())
        return objects.first();

    return nullptr;
}

void Object::setChildCalcOrder(int index, int order)
{
    if(index == order || index < 0 || index >= m_childObjects.count())
        return;

    Object* object = m_childObjects.takeAt(index);
    m_childObjects.insert(order, object);
}

void Object::copyState(Object::CopyState state)
{
    if((m_flags & Disabled) && (m_class->flags() & (qint32)Class::Flag::Disabled))
        return;

    for(Object* child : childObjects())
        child->copyState(state);

    for(ObjectVar* var : m_variables)
    {
        if(state == CopyState::Save)
            var->save();
        else
            var->restore();
    }
}


void Object::load(Project* /*project*/)
{}

bool Object::isExecutionEnabled() const
{
    if(m_enableVar)
        return m_enableVar->doubleValue() > 0;

    else if(m_disableVar)
        return !(m_disableVar->doubleValue() > 0);

    return true;
}

qint32 Object::handle() const
{
    return m_objectInfo == nullptr ? 0 : m_objectInfo->handle();
}

}
