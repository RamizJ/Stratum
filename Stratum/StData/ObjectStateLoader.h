#ifndef OBJECTSTATELOADER_H
#define OBJECTSTATELOADER_H

#include <QString>
#include <QObject>

namespace StData {
class Project;
class Class;
class Object;
class St2000DataStream;

class ClassData;

class ObjectVarsLoader
{
public:
    enum Code {StreamVersion = 40, Classes = 1000, Vars = 1001, SetVars = 1002};

public:
    ObjectVarsLoader();
    ~ObjectVarsLoader();

    void load(StData::Project* project, StData::Object* object);

private:
    QList<ClassData*> loadClassDataList(StData::St2000DataStream& stream);
    void loadVars(StData::St2000DataStream& stream, StData::Project* project);
    void loadObjectVars(StData::St2000DataStream& stream, StData::Object* object, qint32 endOfData);
};

class ChildData;
class VarDataInfo;

class ClassData
{
public:
    explicit ClassData(){}
    ~ClassData();

    QString name() const {return m_name;}
    void setName(const QString& name) {m_name = name;}

    quint32 classId() const {return m_classId;}
    void setClassId(const quint32& classId) {m_classId = classId;}

    QString className() const {return m_className;}
    void setClassName(const QString& className) {m_className = className;}

    qint16 varSize() const {return m_varSize;}
    void setVarSize(const qint16& varSize) {m_varSize = varSize;}

    QList<ChildData*> childDataList() const {return m_childDataList;}
    void addChildData(ChildData* childData);

    QList<VarDataInfo*> varDataList() const {return m_varDataList;}
    void addVarData(VarDataInfo* varData);

private:
    QString m_name;
    quint32 m_classId;

    QString m_className;
    qint16 m_varSize;

    QList<ChildData*> m_childDataList;
    QList<VarDataInfo*> m_varDataList;

    StData::Class* m_class;
};

class ChildData
{
public:
    ClassData* classData() const {return m_classData;}
    void setClassData(ClassData* classData) {m_classData = classData;}

    qint16 id() const {return m_id;}
    void setId(const qint16& id) {m_id = id;}

private:
    ClassData* m_classData;
    qint16 m_id;
};

class VarDataInfo
{
public:
    QString name() const {return m_name;}
    void setName(const QString& name) {m_name = name;}

    ClassData* classData() const {return m_classData;}
    void setClassData(ClassData* classData) {m_classData = classData;}

    QString data() const {return m_data;}
    void setData(const QString& data) {m_data = data;}

private:
    QString m_name;
    QString m_data;

    ClassData* m_classData;
};

}

#endif // OBJECTSTATELOADER_H
