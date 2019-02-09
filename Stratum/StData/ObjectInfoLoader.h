#ifndef OBJECTINFOLOADER_H
#define OBJECTINFOLOADER_H

#include <QList>

namespace StData {
class St2000DataStream;
class Project;
class ObjectInfo;
}

namespace StLoading {

class ObjectInfoLoader
{
public:
    enum Codes {ChildSnameXY = 17, ChildSname = 16, Childs = 6};

public:
    ObjectInfoLoader();
    ~ObjectInfoLoader();

    QList<StData::ObjectInfo*> loadChilds(StData::St2000DataStream& stream, StData::Project* project, qint32 fileVersion, qint16 code);
};

}

#endif // OBJECTINFOLOADER_H
