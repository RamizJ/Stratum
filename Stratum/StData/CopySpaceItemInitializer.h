#ifndef COPYSPACEITEMINITIALIZER_H
#define COPYSPACEITEMINITIALIZER_H

#include "StDataTypes.h"
#include "SpaceItemVisitorBase.h"

namespace StData {

class CopySpaceItemInitializer : public SpaceItemVisitorBase
{
public:
    CopySpaceItemInitializer();

public:
    void visit(TextureItem2d* item);
    void visit(GroupItem* item);
    void visit(PolylineItem2d* item);
    void visit(TextItem2d* item);
};

}

#endif // COPYSPACEITEMINITIALIZER_H
