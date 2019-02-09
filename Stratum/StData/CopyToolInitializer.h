#ifndef COPYTOOLINITIALIZER_H
#define COPYTOOLINITIALIZER_H

#include "StDataTypes.h"
#include "ToolVisitorBase.h"

namespace StData {

class Space;

class CopyToolInitializer : public ToolVisitorBase
{
public:
    CopyToolInitializer(Space* space);

public:
    void visit(PenTool* tool);
    void visit(BrushTool* tool);
    void visit(FontTool* tool);
    void visit(StringTool* tool);
    void visit(TextureTool* tool);
    void visit(RasterTextTool* tool);


private:
    Space* m_space;
};

}

#endif // COPYTOOLINITIALIZER_H
