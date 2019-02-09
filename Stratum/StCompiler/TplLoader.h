#ifndef TPLLOADER_H
#define TPLLOADER_H

#include <QHash>
#include <QString>

namespace StCompiler {

class Cursor;
class Command;
class CommandCollection;

class TplLoader
{
public:
    static bool loadCommands(Cursor& cursor, CommandCollection* commandCollection);
    static bool loadInclude(const QString& tplFileName, CommandCollection* commandColletion);
    static bool loadConstants(QHash<QString, double>& constantDictionary);

private:
    TplLoader();
    ~TplLoader();

    bool loadInclude();
};

}

#endif // TPLLOADER_H
