#ifndef SPACEITEMVISITORBASE_H
#define SPACEITEMVISITORBASE_H

#include "stdata_global.h"
#include "StDataTypes.h"

namespace StData {

class STDATASHARED_EXPORT SpaceItemVisitorBase
{
public:
    SpaceItemVisitorBase();

    virtual void visit(ControlItem2d* item);
    virtual void visit(GroupItem* item);
    virtual void visit(PolylineItem2d* item);
    virtual void visit(TextItem2d* item);
    virtual void visit(TextureItem2d* item);
    virtual void visit(UserObject* item);
};

}

#endif // SPACEITEMVISITORBASE_H
