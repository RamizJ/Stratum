#include "ObjectInfo.h"

#include "Sc2000DataStream.h"
#include "Project.h"
#include "Class.h"
#include "LibraryCollection.h"


namespace StData {

ObjectInfo::ObjectInfo() :
    m_flags(0),
    m_class(NULL)
{}

void ObjectInfo::load(St2000DataStream& stream, qint32 fileVersion, bool loadName, bool loadPosition)
{
    QString className = stream.readString(256);

//    m_classInfo = project->classInfoByName(type);
    m_class = LibraryCollection::getClassByName(className);
    if(!m_class)
        m_absentClass = className;

    setHandle(stream.readInt16());
    if(loadName)
        m_name = stream.readString();

    if(loadPosition)
    {
        if(fileVersion < 0x3002)
        {
            m_position = QPointF(stream.readInt16(), stream.readInt16());
            m_flags = stream.readUint8();
        }
        else
        {
            qreal x = stream.readDouble();
            qreal y = stream.readDouble();
            m_position = QPointF(x, y);
            stream.readUint8();
        }
    }
    else
        setFlags(ObjectInfo::NeedUpdate);
}

void ObjectInfo::skip(St2000DataStream& stream, qint32 fileVersion, bool loadName, bool loadPosition)
{
    stream.skipString();
    stream.skipRawData(2);
    if(loadName)
        stream.skipString();

    if(loadPosition)
    {
        if(fileVersion < 0x3002)
            stream.skipRawData(5);
        else
            stream.skipRawData(sizeof(double)*2 + 1);
    }
}

}
