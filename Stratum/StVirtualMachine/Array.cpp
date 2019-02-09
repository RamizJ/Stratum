#include "Array.h"
#include "StreamItem.h"

#include <Class.h>
#include <ClassVarInfo.h>
#include <LibraryCollection.h>
#include <ObjectVar.h>
#include <StandardTypes.h>
#include <Sc2000DataStream.h>

using namespace StData;

namespace StVirtualMachine {

Array::Array(int handle) :
    HandleItem(handle)
{}

Array::~Array()
{
    qDeleteAll(m_items);
}

void Array::insertItem(ArrayItem* item)
{
    m_items << item;
}

void Array::removeItem(int index)
{
    m_items.removeAt(index);
}

ArrayItem* Array::takeAt(int index)
{
    return m_items.takeAt(index);
}

ArrayItem* Array::at(int index)
{
    return m_items.at(index);
}

int Array::count()
{
    return m_items.count();
}

int Array::length()
{
    return m_items.length();
}

bool Array::isEmpty()
{
    return m_items.isEmpty();
}

void Array::save(St2000DataStream& stream)
{
    stream.writeInt16(m_items.count());
    for(ArrayItem* item : m_items)
        item->save(stream);
}

void Array::load(St2000DataStream& stream)
{
    int count = stream.readInt16();
    for(int i = 0; i < count; i++)
    {
        ArrayItem* item = ArrayItem::load(stream);
        insertItem(item);
    }
}

/*-----------------------------------------------------------------------------------------*/
ArrayItem::ArrayItem(Class* cls) :
    m_type(cls),
    m_simpleTypeVar(nullptr)
{
    if(cls->varInfoList().count() > 0)
    {
        for(ClassVarInfo* varInfo : cls->varInfoList())
        {
            ObjectVar* var = new ObjectVar(varInfo);
            m_vars << var;
            m_varDictionary[varInfo->varName().toLower()] = var;
        }
    }

    if(StandardTypes::isStandandType(cls))
        m_simpleTypeVar = new ObjectVar(new ClassVarInfo("", cls));
}

ArrayItem::~ArrayItem()
{
    if(m_simpleTypeVar)
        delete m_simpleTypeVar;

    qDeleteAll(m_vars);
}

ObjectVar* ArrayItem::varByName(const QString& name)
{
    if(StandardTypes::isStandandType(m_type))
        return m_simpleTypeVar;

    return m_varDictionary.value(name.toLower(), nullptr);
}

void ArrayItem::setVarS(const QString& varName, const QString& value)
{
    if(ObjectVar* var = varByName(varName))
        var->setString(value);
}

void ArrayItem::setVarF(const QString& varName, double value)
{
    if(ObjectVar* var = varByName(varName))
        var->setDouble(value);
}

void ArrayItem::setVarH(const QString& varName, int value)
{
    if(ObjectVar* var = varByName(varName))
        var->setInt(value);
}

void ArrayItem::save(St2000DataStream& stream)
{
    stream.writeString(m_type->originalName());
    stream.writeInt16(m_vars.count());

    if(!StandardTypes::isStandandType(m_type))
    {
        for(ObjectVar* var : m_vars)
        {
            stream.writeString(var->info()->varName());
            stream.writeString(var->info()->varType()->originalName());
            writeVar(stream, var);
        }
    }
    else
        writeVar(stream, m_simpleTypeVar);
}

ArrayItem* ArrayItem::load(St2000DataStream& stream)
{
    QString itemTypeName = stream.readString();
    Class* itemType = LibraryCollection::getClassByName(itemTypeName);
    if(itemType)
    {
        int varsCount = stream.readInt16();
        ArrayItem* item = new ArrayItem(itemType);
        if(StandardTypes::isStandandType(itemType))
        {
            readVar(stream, item->var());
        }
        else
        {
            for(int i = 0; i < varsCount; i++)
            {
                QString varName = stream.readString();
                /*QString varType = */stream.readString();
                ObjectVar* var = item->varByName(varName);
                if(var)
                    readVar(stream, var);
                else
                    return nullptr;
            }
        }
        return item;
    }
    return nullptr;
}

void ArrayItem::writeVar(St2000DataStream& stream, ObjectVar* var)
{
    Class* varType = var->info()->varType();
    if(StandardTypes::isNumeric(varType))
    {
        if(StandardTypes::isFloat(varType))
            stream.writeDouble(var->doubleValue());
        else
            stream.writeInt32(var->intValue());
    }
    else if(StandardTypes::isColorRef(varType))
        stream.writeInt32(var->intValue());
    else
        stream.writeString(var->stringValue());
}

void ArrayItem::readVar(St2000DataStream& stream, ObjectVar* var)
{
    Class* varType = var->info()->varType();
    if(StandardTypes::isNumeric(varType))
    {
        if(StandardTypes::isFloat(varType))
            var->setDouble(stream.readDouble());
        else
            var->setInt(stream.readInt32());
    }
    else if(StandardTypes::isColorRef(varType))
        var->setInt(stream.readInt32());
    else
        var->setString(stream.readString());
}



}
