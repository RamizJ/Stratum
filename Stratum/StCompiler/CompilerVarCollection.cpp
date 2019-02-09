#include "CompilerVarCollection.h"

#include <Class.h>
#include <ClassVarInfo.h>
#include <LibraryCollection.h>

using namespace StData;

namespace StCompiler {

CompileVarCollection::CompileVarCollection(StData::Class* cls) :
    m_class(cls),
    m_isModified(false),
    m_isSubModified(false)
{
    if(m_class)
    {
        m_flags = 0;

        for(ClassVarInfo* classVar : m_class->varInfoList())
            m_variables << new ClassVarInfo(*classVar);
    }
}

CompileVarCollection::CompileVarCollection(CompileVarCollection& varCollection)
{
    m_flags = 0;
    m_class = varCollection.cls();

    for(ClassVarInfo* classVar : m_class->varInfoList())
        m_variables << new ClassVarInfo(*classVar);
}

CompileVarCollection::~CompileVarCollection()
{}

ClassVarInfo* CompileVarCollection::at(int index)
{
    return m_variables.at(index);
}

qint16 CompileVarCollection::insert(const QString& name, const QString& typeName, bool fix)
{
    qint16 index;
    if (name.isEmpty())
        return -1;

    ClassVarInfo* classVarInfo = search(name, index);

    if(!classVarInfo)
    {
        Class* cl = typeName.isEmpty() ?
                    LibraryCollection::getClassByName("float") :
                    LibraryCollection::getClassByName(typeName);

        if(cl)
        {
            classVarInfo = new ClassVarInfo(name, cl);
            quint32 flags = ClassVarInfo::ByCompiler;
            if (!fix)
                flags |= ClassVarInfo::NotFixed;

            classVarInfo->setFlags(flags);

            m_variables << classVarInfo;
            index = m_variables.count() - 1;

            m_isModified = true;
            m_isSubModified = true;
        }
        else
            return -1;

    }
    else
    {
        if(!typeName.isEmpty() && (classVarInfo->varType()->originalName().toLower() != typeName.toLower()))
            return -2;
    }
    return index;
}

void CompileVarCollection::deleteVar(int index)
{
    delete m_variables.takeAt(index);
    m_isModified = m_isSubModified = true;
}

ClassVarInfo* CompileVarCollection::search(const QString& name, qint16& index)
{
    for(qint16 i = 0; i < m_variables.count(); i++)
    {
        ClassVarInfo* varInfo = m_variables.at(i);
        if (varInfo->varName() == name)
        {
            index = i;
            return varInfo;
        }
    }
    return nullptr;
}

void CompileVarCollection::fix()
{
    for(ClassVarInfo* varInfo : m_variables)
        varInfo->setFlags(varInfo->flags() & (~ClassVarInfo::NotFixed));
}

void CompileVarCollection::undo()
{
    for(int i = m_variables.count()-1; i >= 0; i--)
        if(m_variables[i]->flags() & ClassVarInfo::NotFixed)
            delete m_variables.takeAt(i);

    m_isModified = m_isSubModified = true;
}

qint16 CompileVarCollection::deleteCompilerVars()
{
    quint16 result = m_variables.count();

    qDeleteAll(m_variables);
    m_variables.clear();

    m_isModified = m_isSubModified = true;
    return result;
}

bool CompileVarCollection::isNeedModifyVars(Class* cl, CompileVarCollection* oldVars)
{
    if(cl == nullptr || oldVars == nullptr) return false;

    bool result = false;
    if(m_variables.count() == oldVars->variables().count())
    {
        QList<ClassVarInfo*> oldVariables = oldVars->variables();
        for (int i = 0; i < m_variables.count(); ++i)
        {
            ClassVarInfo* var = m_variables[i];
            ClassVarInfo* oldVar = oldVariables[i];

            if(QString::compare(var->varName(), oldVar->varName()) != 0 || var->varType() != oldVar->varType())
            {
                result = true;
            }
        }
    }
    else
    {
        result = true;
    }

    return result;

//    if(need)
//    {
//        modifyVars(cl);
//    }
//    else
//    {
//        for (int i = 0; i < m_variables.count(); ++i)
//            cl->varInfoByIndex(i)->setFlags(m_variables[i]->flags());
//    }
//    cl->updateDisableVars();
}

}
