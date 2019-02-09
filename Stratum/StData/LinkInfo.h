#ifndef LINKINFO_H
#define LINKINFO_H

#include "stdata_global.h"
#include "HandleItem.h"

namespace StData {

class St2000DataStream;
class LinkEntryInfo;

class STDATASHARED_EXPORT LinkInfo : public HandleItem
{
public:
    enum Flags {Disabled = 1, CorrectPos = 0x8000ul, AutoLink = 0x0002};

public:
    explicit LinkInfo(qint16 handle = 0);
    ~LinkInfo();

    qint16 sourceObjectHandle() const {return m_sourceObjectHandle;}
    void setSourceObjHandle(int sourceObjHandle) {m_sourceObjectHandle = sourceObjHandle;}

    qint16 targetObjectHandle() const {return m_targetObjectHandle;}
    void setTargetObjectHandle(int targetObjHandle) {m_targetObjectHandle = targetObjHandle;}

    qint32 flags() const {return m_flags;}
    void setFlags(int flags) {m_flags = flags;}

    qint16 varHandle() const {return m_varHandle;}
    void setVarHandle(int varHandle) {m_varHandle = varHandle;}

    QList<LinkEntryInfo*> linkEntries() const {return m_linkEntryList;}
    void addLinkEntry(LinkEntryInfo* linkEntry);

    void load(St2000DataStream& stream);
    static void skip(St2000DataStream& stream);

private:
    int m_sourceObjectHandle;
    int m_targetObjectHandle;
    int m_flags;
    int m_varHandle;

    QList<LinkEntryInfo*> m_linkEntryList;
};

class LinkEntryInfo
{
public:

    QString sourceVarName() const {return m_sourceVarName;}
    void setSourceVarName(const QString& sourceVarName) {m_sourceVarName = sourceVarName;}

    QString targetVarName() const {return m_targetVarName;}
    void setTargetVarName(const QString& targetVarName) {m_targetVarName = targetVarName;}

private:
    QString m_sourceVarName;
    QString m_targetVarName;
};

}

#endif // LINKINFO_H
