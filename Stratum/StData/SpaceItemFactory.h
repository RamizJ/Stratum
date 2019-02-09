#ifndef SPACEITEMFACTORY_H
#define SPACEITEMFACTORY_H

namespace StCore {

class SpaceItem;
class Sc2000DataStream;

class SpaceItemFactory
{
public:
    static SpaceItem* load(Sc2000DataStream& stream);
};

}

#endif // SPACEITEMFACTORY_H
