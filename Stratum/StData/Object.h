#ifndef CLASSOBJECT_H
#define CLASSOBJECT_H

#include "stdata_global.h"

#include <QHash>
#include <QMap>
#include <QObject>

namespace StData {

class Class;
class ObjectInfo;
class ObjectVar;
class Project;

class STDATASHARED_EXPORT Object : public QObject
{
public:
    enum Flags {Disabled = 1, OnePhase = 2, Error = 4};

public:
    ~Object();

    void recreateVariables();

    ObjectInfo* objectInfo() const {return m_objectInfo;}
    void setObjectInfo(ObjectInfo* objectInfo) {m_objectInfo = objectInfo;}

    Object* parentObject() const {return m_parentObject;}
    void setParentObject(Object* parentObject) {m_parentObject = parentObject;}

    QList<Object*> childObjects() const {return m_childObjects;}
    int childsCount();
    Object* childAt(int index);
    void addChild(Object* childObject);
    Object* childByHandle(qint16 handle);
    Object* objectByHandle(qint16 handle);

    Project* project() const {return m_project;}
    Class* cls() const {return m_class;}

    QList<ObjectVar*>& variables() {return m_variables;}
    ObjectVar* varByName(const QString& varName);
    ObjectVar* varByIndex(int index);
    void setVar(const QString& varName, const QString& varData);

    qint32 flags() const { return m_flags; }
    void setFlags(const qint32& flags);

    bool isProcedure() const;

    void saveState();
    void restoreState();

    void saveStartupState();
    void toDefaultState();
    void toStartupState();

    bool updateInternalVar(ObjectVar* var);

    Object* rootObject();
    QList<Object*> getObjectsByPath(const QString& path);
    Object* getObjectByPath(const QString& path);

    void setChildCalcOrder(int index, int order);

    void load(Project* project);

    bool isExecutionEnabled() const;
    bool isHierarchyExecutionEnabled();

    static bool isHierarchyExecutionEnabled(Object* object);

    qint32 handle() const;

public:
    static Object* createByClass(Project* project, Class* cls, StData::ObjectInfo* objectInfo = nullptr);

private:
    enum Code
    {
        Code_StreamVersion = 40, Code_Classes = 1000, Code_Vars = 1001,
        Code_SetVar = 1002
    };
    enum class CopyState {Save, Restore};

private:
    Object(StData::Project* project, Class* cls, ObjectInfo* objectInfo = nullptr, Object* parent = nullptr);
    void copyState(CopyState state);

private:
    StData::Project* m_project;

    ObjectInfo* m_objectInfo;
    QList<Object*> m_childObjects;
    QMap<qint16, Object*> m_childObjectDictionary;

    QList<ObjectVar*> m_variables;
    QHash<QString, ObjectVar*> m_varByName;

    ObjectVar* m_enableVar;
    ObjectVar* m_disableVar;

    Object* m_parentObject;

    Class* m_class;
    qint32 m_flags;

    bool m_visible;
    qint16 m_layer;
//    QList<QString> m_internalStringVarNames;
//    QList<QString> m_internalFloatVarNames;
//    QList<QString> m_internalHandleVarNames;
};

}

#endif // CLASSOBJECT_H
