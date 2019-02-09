#ifndef VARMANAGERLOADER_H
#define VARMANAGERLOADER_H

namespace StData {
class St2000DataStream;
class ProjectVarCollection;

class ProjectVarsLoader
{
public:
    ProjectVarsLoader();
    ~ProjectVarsLoader();

    void load(StData::St2000DataStream& stream, StData::ProjectVarCollection* varManager);
};

}

#endif // VARMANAGERLOADER_H
