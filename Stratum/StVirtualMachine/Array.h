#ifndef ARRAY_H
#define ARRAY_H

#include <QList>
#include <QMap>
#include <HandleItem.h>

namespace StData {
class Project;
class ObjectVar;
class Class;
class St2000DataStream;
}

namespace StVirtualMachine {

class ArrayItem;
class StreamItem;

class Array : public StData::HandleItem
{
public:
    explicit Array(int handle = 0);
    ~Array();

    void insertItem(ArrayItem* item);
    void removeItem(int index);
    ArrayItem* takeAt(int index);
    ArrayItem* at(int index);

    int count();
    int length();
    bool isEmpty();

    QList<ArrayItem*> items() const { return m_items; }

    void save(StData::St2000DataStream& stream);
    void load(StData::St2000DataStream& stream);

    bool sort(const QVector<QString>& fieldNames, bool desc);
    bool sort(const QVector<std::tuple<QString, bool>>& fieldNames);

private:
    //    StData::Project* m_project;
    QList<ArrayItem*> m_items;
};


/*-----------------------------------------------------------------------------------------*/
class ArrayItem
{
public:
    explicit ArrayItem(StData::Class* cls);
    ~ArrayItem();

    StData::Class* type() const { return m_type; }
    void setType(StData::Class* type) { m_type = type; }

    StData::ObjectVar* varByName(const QString& name);

    StData::ObjectVar* var() const { return m_simpleTypeVar; }
    QList<StData::ObjectVar*> vars() const { return m_varDictionary.values(); }

    void setVarS(const QString& varName, const QString& value);
    void setVarF(const QString& varName, double value);
    void setVarH(const QString& varName, int value);

    void save(StData::St2000DataStream& stream);
    static ArrayItem* load(StData::St2000DataStream& stream);

private:
    static void writeVar(StData::St2000DataStream& stream, StData::ObjectVar* var);
    static void readVar(StData::St2000DataStream& stream, StData::ObjectVar* var);

private:
    QMap<QString, StData::ObjectVar*> m_varDictionary;
    QList<StData::ObjectVar*> m_vars;
    StData::ObjectVar* m_simpleTypeVar;
    StData::Class* m_type;
};

}

#endif // ARRAY_H
