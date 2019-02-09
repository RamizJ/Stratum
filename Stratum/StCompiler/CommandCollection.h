#ifndef COMMANDCOLLECTION_H
#define COMMANDCOLLECTION_H

#include <QHash>
#include <SortedCollection.h>
#include "CommandComparer.h"

namespace StData {
class Class;
}

namespace StCompiler {

class Command;
class CommandNameComparer;
class TplCursor;

class CommandCollection : public StData::SortedCollection<Command*, CommandComparer>
{
public:
    CommandCollection();
    ~CommandCollection();

    //NSearch
    bool searchByName(const QString& cmdName, qint16& index, qint16 startIndex = 0);

    //FSearch
    bool searchBySpec(qint16& index, const QString& cmdName,
                             QList<StData::Class*> args, qint16 startIndex = 0,
                             QList<StData::Class*> optionalArgs = QList<StData::Class*>());

    //SpecFSearch
    bool searchByRetSpec(const QString& cmdName, StData::Class* cl,
                                qint16& index, qint16 startIndex= 0);

    //FindKeyword
    bool findKeyword(const QString& cmdName, qint16& index);

    bool load(TplCursor& cursor);
    bool loadInclude(const QString& tplFileName);
};

}

#endif // COMMANDCOLLECTION_H
