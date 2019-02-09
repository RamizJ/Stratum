#ifndef VARMANAGER_H
#define VARMANAGER_H

#include "stdata_global.h"

#include <QObject>
#include <QVariant>

namespace StData {

class St2000DataStream;

class STDATASHARED_EXPORT ProjectVariable
{
public:
    enum VarType {Int, Double, String};

    VarType type() const {return m_type;}
    void setType(const VarType& varType) {m_type = varType;}

    QString name() const {return m_name;}
    void setName(const QString& varName) {m_name = varName;}

    QVariant value() const {return m_value;}
    void setValue(const QVariant& varValue) {m_value = varValue;}

private:
    VarType m_type;
    QString m_name;
    QVariant m_value;
};

class STDATASHARED_EXPORT ProjectVarCollection
{
public:
    ProjectVarCollection();
    ~ProjectVarCollection();

    //TODO: вынести в factory
//    void load(Sc2000DataStream& stream);

    QString getVarS(QString varName);
    int getVarI(QString varName);
    double getVarD(QString varName);

    bool contains(QString varName);
    bool isVarTypeEqual(QString varName, ProjectVariable::VarType varType);

    QList<ProjectVariable*> varList() const {return m_varList;}
    void addVariable(ProjectVariable* variable);

private:
    QList<ProjectVariable*> m_varList;
    QHash<QString, ProjectVariable*> m_varForName;
};

}

#endif // VARMANAGER_H
