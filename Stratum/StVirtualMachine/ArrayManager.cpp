#include "ArrayManager.h"
#include "Array.h"
#include "StreamManager.h"

#include <Class.h>
#include <LibraryCollection.h>
#include <ObjectVar.h>
#include <ClassVarInfo.h>
#include <StandardTypes.h>

using namespace StData;

namespace StVirtualMachine {

ArrayManager::ArrayManager(QObject* parent) :
    QObject(parent),
    m_arrays(16384)
{}

ArrayManager::~ArrayManager()
{
    m_arrays.deleteAll();
}

//TODO возвращать Array*
int ArrayManager::createArray()
{
    cleanError();
    Array* array = new Array();
    int arrayHandle = insertArray(array);
    if(!arrayHandle)
    {
        delete array;
        setError(tr("Невозможно создать динамическую переменную"));
    }
    return arrayHandle;
}

int ArrayManager::insertArray(Array* array)
{
    if(array)
        return m_arrays.insert(array);

    return 0;
}

int ArrayManager::deleteArray(int handle)
{
    cleanError();
    if(Array* var = m_arrays.take(handle))
    {
        delete var;
        return true;
    }
    setError(tr("Невозможно удалить переменную из массива. Некорректный идентификатор"));
    return false;
}

int ArrayManager::insertItem(int handle, const QString& typeName)
{
    if(Array* array = m_arrays.getItem(handle))
    {
        if(Class* itemType = LibraryCollection::getClassByName(typeName))
        {
            if(itemType->varInfoList().count() > 0 || StandardTypes::isStandandType(itemType))
            {
                array->insertItem(new ArrayItem(itemType));
                return array->count();
            }
            else
                setError("Неудалось вставить элемент в массив. "
                         "Тип не содержит переменных или не является стандартным");
        }
        else
            setError("Неудалось вставить элемент в массив. "
                     "Некорректный тип элемента");
    }
    return 0;
}

bool ArrayManager::deleteItem(int handle, int index)
{
    Array* array = m_arrays.getItem(handle);
    if(array && isIndexValid(array, index))
    {
        ArrayItem* item = array->takeAt(index);
        if(item)
            delete item;
        return true;
    }
    return false;
}

int ArrayManager::getCount(int handle)
{
    if(Array* array = m_arrays.getItem(handle))
        return array->count();

    return 0;
}

Array* ArrayManager::getArray(int handle)
{
    Array* array = m_arrays.getItem(handle);
    if(!array)
        setError(tr("Некорректный дескриптор массива"));

    return array;
}

QString ArrayManager::getItemTypeName(int handle, int index)
{
    ArrayItem* item = getItem(handle, index);
    if(item && item->type())
        return item->type()->originalName();

    return "";
}

int ArrayManager::getVarTypeIndex(int handle, int index, const QString& varName)
{
    ArrayItem* item = getItem(handle, index);
    if(item)
    {
        ObjectVar* var = item->varByName(varName);
        if(var)
            return StandardTypes::typeCode(var->info()->varType());
    }
    return -1;
}

bool ArrayManager::getVarF(int handle, int index, const QString varName, double& value)
{
    if(ArrayItem* item = getItem(handle, index))
    {
        ObjectVar* var = item->varByName(varName);
        if(var)
            value = var->doubleValue();
        return true;
    }
    return false;
}

bool ArrayManager::getVarH(int handle, int index, const QString varName, int& value)
{
    if(ArrayItem* item = getItem(handle, index))
    {
        ObjectVar* var = item->varByName(varName);
        if(var)
            value = var->handleValue();
        return true;
    }
    return false;
}

bool ArrayManager::getVarS(int handle, int index, const QString varName, QString& value)
{
    if(ArrayItem* item = getItem(handle, index))
    {
        ObjectVar* var = item->varByName(varName);
        if(var)
            value = var->stringValue();
        return true;
    }
    return false;
}

bool ArrayManager::setVarF(int handle, int index, const QString& varName, double data)
{
    if(ArrayItem* item = getItem(handle, index))
    {
        ObjectVar* var = item->varByName(varName);
        if(var)
            var->setDouble(data);
        return true;
    }
    return false;
}

bool ArrayManager::setVarH(int handle, int index, const QString& varName, int data)
{
    if(ArrayItem* item = getItem(handle, index))
    {
        ObjectVar* var = item->varByName(varName);
        if(var)
            var->setInt(data);
        return true;
    }
    return false;
}

bool ArrayManager::setVarS(int handle, int index, const QString& varName, const QString& data)
{
    if(ArrayItem* item = getItem(handle, index))
    {
        ObjectVar* var = item->varByName(varName);
        if(var)
            var->setString(data);
        return true;
    }
    return false;
}

bool ArrayManager::save(int handle, StreamItem* streamItem)
{
    if(!streamItem)
        return false;

    Array* array = getArray(handle);
    if(array)
    {
        array->save(streamItem->stream());
        return true;
    }

    return false;
}

int ArrayManager::load(StreamItem* streamItem)
{
    if(!streamItem)
        return 0;

    Array* array = new Array();
    array->load(streamItem->stream());

    return insertArray(array);
}

void ArrayManager::reset()
{
    m_arrays.deleteAll();
}

ArrayItem* ArrayManager::getItem(int handle, int index)
{
    Array* array = m_arrays.getItem(handle);
    if(array && index >= 0 && index < array->count() )
        return array->at(index);

    return nullptr;
}

bool ArrayManager::isIndexValid(Array* array, int index)
{
    return index >=0 && index < array->count();
}

}
