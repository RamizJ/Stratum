#ifndef MACROCOLLECTION_H
#define MACROCOLLECTION_H

#include <SortedCollection.h>
#include <QString>

using namespace StData;

namespace StCompiler {

class Macro;
class MacroComparer;
class TplCursor;

class MacroCollection : public SortedCollection<Macro*, MacroComparer>
{
public:
    MacroCollection();

    ~MacroCollection();

    bool load();

    bool search(const QString& name, qint16& index);
};

class MacroComparer
{
public:
    int compare(Macro* macro1, Macro* macro2);
};

}

#endif // MACROCOLLECTION_H
