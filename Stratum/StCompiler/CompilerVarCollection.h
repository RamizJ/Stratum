#ifndef COMPILERVARIABLES_H
#define COMPILERVARIABLES_H

#include <QList>
#include <QString>
#include <QtGlobal>

namespace StData {
class Class;
class ClassVarInfo;
}

namespace StCompiler {

class CompileVarCollection
{
public:
    CompileVarCollection(StData::Class* cls);
    CompileVarCollection(CompileVarCollection& varCollection);
    virtual ~CompileVarCollection();

    StData::Class* cls() { return m_class; }

    QList<StData::ClassVarInfo*> variables() { return m_variables; }
    StData::ClassVarInfo* at(int index);
    qint16 insert(const QString& name, const QString& type, bool fix = false);
    void deleteVar(int index);
    StData::ClassVarInfo* search(const QString& name, qint16& index);

    bool isModified() const {return m_isModified;}
    bool isSubModified() const {return m_isSubModified;}

    void fix();
    void undo();

    qint16 deleteCompilerVars();

    bool isNeedModifyVars(StData::Class* cl, CompileVarCollection* oldVars);

private:
    qint32 m_flags;
    QList<StData::ClassVarInfo*> m_variables;
    StData::Class* m_class;

    bool m_isModified;
    bool m_isSubModified;
};

}

#endif // COMPILERVARIABLES_H
