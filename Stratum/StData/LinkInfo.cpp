#include "LinkInfo.h"

#include "Sc2000DataStream.h"
#include "Log.h"

namespace StData {

LinkInfo::LinkInfo(qint16 id) : HandleItem(id)
{}

LinkInfo::~LinkInfo()
{
    qDeleteAll(m_linkEntryList);
}

void LinkInfo::addLinkEntry(LinkEntryInfo* linkEntry)
{
    m_linkEntryList << linkEntry;
}

void LinkInfo::load(St2000DataStream& stream)
{
    m_sourceObjectHandle = stream.readInt16();
    m_targetObjectHandle = stream.readInt16();

    setHandle(stream.readInt16());

    m_flags = stream.readInt32();

    qint16 count = stream.readInt16();
    m_varHandle = stream.readInt16();

    for(int i = 0; i < count; i++)
    {
        LinkEntryInfo* linkEntry = new LinkEntryInfo();

        linkEntry->setSourceVarName(stream.readString());
        linkEntry->setTargetVarName(stream.readString());
        addLinkEntry(linkEntry);
    }
}

void LinkInfo::skip(St2000DataStream& stream)
{
    stream.skipRawData(10);
    qint16 count = stream.readInt16();
    stream.skipRawData(2);

    for(int i = 0; i < count; i++)
    {
        stream.skipString();
        stream.skipString();
    }
}

}
