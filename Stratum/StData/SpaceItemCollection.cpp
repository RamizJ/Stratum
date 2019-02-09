#include "Sc2000DataStream.h"
#include "SpaceItemCollection.h"

namespace StCore {

SpaceItemCollection::SpaceItemCollection()
{
}

SpaceItemCollection::~SpaceItemCollection()
{
}

void SpaceItemCollection::load(Sc2000DataStream& stream)
{
    m_count = stream.readInt16();
    m_limit = stream.readInt16();
    m_delta = stream.readInt16();
}

void SpaceItemIdCollection::load(Sc2000DataStream& stream)
{
    SpaceItemCollection::load(stream);

    for(int i = 0; i < count(); i++)
        m_ids << stream.readInt16();
}

}
