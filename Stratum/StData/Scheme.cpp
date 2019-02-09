#include "Scheme.h"

#include "Class.h"
#include "Sc2000DataStream.h"
#include "Log.h"
#include "Space.h"
#include "Library.h"

namespace StData {

Scheme::Scheme(Class* cls, QObject* parent) :
    QObject(parent),
    m_class(cls),
    m_space(NULL)
{}

Scheme::~Scheme()
{
    if(m_space)
        delete m_space;
}

void Scheme::load(St2000DataStream& stream)
{
    qint64 pos = stream.device()->pos();
    qint32 size = stream.readInt32();

    m_flags = stream.readInt32();

    if(m_flags & Scheme::External)
        m_fileName = stream.readString();

    else
    {
        qint32 schemeSize = stream.readInt32();
        m_spaceData = stream.readBytes(schemeSize);
//        m_space = createSpaceFromSpaceData();
    }

    qint32 newPos = stream.device()->pos();
    if(newPos - size != pos)
    {
        SystemLog::instance().error("Bad scheme size");
        stream.device()->seek(pos);
    }
}

Space* Scheme::createSpaceFromSpaceData()
{
    return Space::createFromScheme(this);
}

Space* Scheme::createSpaceFromFileName()
{
    QString filePath = m_class->library()->libraryDir().absoluteFilePath(m_fileName);
    return Space::createFromFile(filePath);
}

void Scheme::skip(St2000DataStream& stream)
{
    stream.skipRawData(4);

    qint32 flags = stream.readInt32();
    if(flags & Scheme::External)
        stream.skipString();

    else
        stream.skipRawData(stream.readInt32());
}

Space* Scheme::getSpace()
{
    if(!m_space)
    {
        if(!m_fileName.isEmpty())
        {
            QString filePath = m_class->library()->libraryDir().absoluteFilePath(m_fileName);
            m_space = Space::createFromFile(filePath);
        }
        else
            m_space = Space::createFromScheme(this);
    }
    return m_space;
}

}
