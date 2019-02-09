#include "CopySpaceItemInitializer.h"
#include "SpaceItem.h"
#include "Tool2d.h"

namespace StData {

CopySpaceItemInitializer::CopySpaceItemInitializer()
{}

void CopySpaceItemInitializer::visit(TextureItem2d* item)
{
    if(auto texture = item->texture())
        item->setTextureHandle(texture->handle());
}

void CopySpaceItemInitializer::visit(GroupItem* item)
{
    for(auto chilItem : item->items())
        chilItem->accept(this);
}

void CopySpaceItemInitializer::visit(PolylineItem2d* item)
{
    if(auto penTool = item->penTool())
        item->setPenHandle(penTool->handle());

    if(auto brushTool = item->brushTool())
        item->setBrushHandle(brushTool->handle());
}

void CopySpaceItemInitializer::visit(TextItem2d* item)
{
    if(auto rasterTextTool = item->rasterText())
        item->setRasterTextHandle(rasterTextTool->handle());
}

}
