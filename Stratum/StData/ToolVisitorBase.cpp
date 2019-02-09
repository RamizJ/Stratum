#include "ToolVisitorBase.h"

#include "Tool2d.h"

namespace StData {

ToolVisitorBase::ToolVisitorBase()
{}

void ToolVisitorBase::visit(BrushTool* tool)
{}

void ToolVisitorBase::visit(PenTool* tool)
{}

void ToolVisitorBase::visit(FontTool* tool)
{}

void ToolVisitorBase::visit(RasterTextTool* tool)
{}

void ToolVisitorBase::visit(StringTool* tool)
{}

void ToolVisitorBase::visit(TextureTool* tool)
{}

}
