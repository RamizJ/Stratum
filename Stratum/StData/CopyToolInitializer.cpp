#include "CopyToolInitializer.h"
#include "Tool2d.h"
#include "Space.h"

namespace StData {

CopyToolInitializer::CopyToolInitializer(Space* space) :
    m_space(space)
{}

void CopyToolInitializer::visit(PenTool* tool)
{
    m_space->pens().insert(tool);
}

void CopyToolInitializer::visit(BrushTool* tool)
{
    m_space->brushes().insert(tool);

    if(auto textureTool = tool->texture())
        tool->setTextureHandle(textureTool->handle());
}

void CopyToolInitializer::visit(FontTool* tool)
{
    m_space->fonts().insert(tool);
}

void CopyToolInitializer::visit(StringTool* tool)
{
    m_space->strings().insert(tool);
}

void CopyToolInitializer::visit(TextureTool* tool)
{
    if(dynamic_cast<MaskedTextureTool*>(tool))
        m_space->maskedPixmaps().insert(tool);
    else
        m_space->pixmaps().insert(tool);
}

void CopyToolInitializer::visit(RasterTextTool* tool)
{
    m_space->texts().insert(tool);

    for(auto textFragment : tool->textFragments())
    {
        if(auto fontTool = textFragment->fontTool())
            textFragment->setFontHandle(fontTool->handle());

        if(auto stringTool = textFragment->stringTool())
            textFragment->setStringHandle(stringTool->handle());
    }
}

}
