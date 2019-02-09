#ifndef TOOLVISITORBASE_H
#define TOOLVISITORBASE_H

#include "stdata_global.h"
#include "StDataTypes.h"

namespace StData {

class STDATASHARED_EXPORT ToolVisitorBase
{
public:
    ToolVisitorBase();

    virtual void visit(BrushTool* tool);
    virtual void visit(PenTool* tool);
    virtual void visit(FontTool* tool);
    virtual void visit(RasterTextTool* tool);
    virtual void visit(StringTool* tool);
    virtual void visit(TextureTool* tool);
};

}

#endif // TOOLVISITORBASE_H
