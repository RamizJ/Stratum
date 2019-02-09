#include "SpaceItemFactory.h"

#include "Sc2000DataStream.h"
#include "SpaceItem.h"

namespace StCore {

SpaceItem* SpaceItemFactory::load(Sc2000DataStream& stream)
{
    SpaceItem* spaceItem = new SpaceItem();
    return spaceItem;
}

}
