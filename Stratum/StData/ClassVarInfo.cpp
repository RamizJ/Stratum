#include "ClassVarInfo.h"
#include "Sc2000DataStream.h"
#include "LibraryCollection.h"
#include "Log.h"

#define VAR_ENABLE_NAME "_enable"
#define VAR_DISABLE_NAME "_disable"

namespace StData {

ClassVarInfo::ClassVarInfo(const QString& name, Class* varType) :
    m_varName(name),
    m_flags(0),
    m_varType(varType)
{}

ClassVarInfo::ClassVarInfo(const ClassVarInfo& varInfo)
{
    m_varType = varInfo.varType();
    m_flags = varInfo.flags();
    m_info = varInfo.info();
    m_defValue = varInfo.defValue();
    m_varName = varInfo.varName();
}

void ClassVarInfo::load(St2000DataStream& stream, bool hasDefValue)
{
    m_varName = stream.readString();
    m_info = stream.readString();

    if(hasDefValue)
        m_defValue = stream.readString();

    QString typeName = stream.readString(256);
    m_varType = LibraryCollection::getClassByName(typeName);
    if(!m_varType)
        SystemLog::instance().error("class var type not found. typeName: " + typeName);

    m_flags = stream.readInt32();
    if(m_flags & ClassVarInfo::OnScheme)
        stream.readInt16();
}

void ClassVarInfo::skip(St2000DataStream& stream, bool hasDefValue)
{
    stream.skipString();
    stream.skipString();

    if(hasDefValue)
        stream.skipString();

    stream.skipString();

    qint32 flags = stream.readInt32();
    if(flags & ClassVarInfo::OnScheme)
        stream.skipRawData(2);
}

bool ClassVarInfo::isEnableVar() const
{
    return QString::compare(m_varName, VAR_ENABLE_NAME, Qt::CaseInsensitive) == 0;
}

bool ClassVarInfo::isDisableVar() const
{
    return QString::compare(m_varName, VAR_DISABLE_NAME, Qt::CaseInsensitive) == 0;
}

}
