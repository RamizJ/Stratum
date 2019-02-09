#include "ObjectInfoLoader.h"
#include "ClassLoader.h"

#include "Sc2000DataStream.h"
#include "Project.h"
#include "Log.h"
#include "Context.h"
#include "ClassInfo.h"
#include "ObjectInfo.h"

using namespace StCore;

namespace StLoading {

ObjectInfoLoader::ObjectInfoLoader()
{}

ObjectInfoLoader::~ObjectInfoLoader()
{}

QList<ObjectInfo*> ObjectInfoLoader::loadChilds(Sc2000DataStream& stream, Project* project, qint32 fileVersion, qint16 code)
{
    QList<ObjectInfo*> result;
    Log* log = Context::instance().log();

    qint16 childCount = stream.readInt16();
    for(int i = 0; i < childCount; i++)
    {
        ObjectInfo* objectInfo = new ObjectInfo();

        QString type = stream.readString(256);
        objectInfo->setClassInfo(project->classInfoByName(type));
        if(!objectInfo->classInfo())
        {
            objectInfo->setAbsentClass(type);
            log->error("Child class not found: " + type);
        }
        else
        {
            ClassLoader classLoader;
            classLoader.load(project, objectInfo->classInfo());
//            childInfo->classInfo()->loadClass(project);
        }

        objectInfo->setId(stream.readInt16());
        if(code == ChildSname || code == ChildSnameXY)
            objectInfo->setName(stream.readString());

        if(code == ChildSnameXY)
        {
            if(fileVersion < 0x3002)
            {
                objectInfo->setPosition(QPointF(stream.readInt16(), stream.readInt16()));
                objectInfo->setFlags(stream.readUint8());
            }
            else
            {
                qreal x = stream.readDouble();
                qreal y = stream.readDouble();
                objectInfo->setPosition(QPointF(x, y));
                stream.readUint8();
            }
        }
        else
            objectInfo->setFlags(ObjectInfo::NeedUpdate);

        result << objectInfo;
    }

    return result;
}

}
