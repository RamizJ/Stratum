#include "CommandCollection.h"
#include "Command.h"
#include "TplCursor.h"

#include <QDebug>
#include <QFileInfo>
#include <PathManager.h>
#include <memory>

using namespace StData;

namespace StCompiler {

CommandCollection::CommandCollection()
{}

CommandCollection::~CommandCollection()
{
    qDeleteAll(m_items);
    m_items.clear();
}

bool CommandCollection::searchByName(const QString& cmdName, qint16& index, qint16 startIndex)
{
    for (qint16 i = startIndex; i < m_items.count(); i++)
        if(cmdName.toLower() == m_items.at(i)->name())
        {
            index = i;
            return true;
        }
    index = m_items.count();
    return false;
}

bool CommandCollection::searchByRetSpec(const QString& cmdName, StData::Class* cl,
                                        qint16& index, qint16 startIndex)
{
    for (int i = startIndex; i < m_items.count(); i++)
        if(cmdName.toLower() == m_items.at(i)->name())
        {
            if(cl == m_items.at(i)->returnType())
            {
                index = i;
                return true;
            }
        }

    index = m_items.count();
    return false;
}

bool CommandCollection::findKeyword(const QString& cmdName, qint16& index)
{
    Command cmd(cmdName.toLower());
    return binaryFindIndex<Command*, CommandNameComparer>(m_items, &cmd, (int&)index);
}

bool CommandCollection::searchBySpec(qint16& index, const QString& cmdName,
                                     QList<StData::Class*> args,
                                     qint16 startIndex,
                                     QList<StData::Class*> optionalArgs)
{

    qint16 f;
    for (qint16 i = startIndex; i < items().count(); i++)
    {
        if(m_items.at(i)->name() == cmdName.toLower() && m_items.at(i)->args().count() == args.count())
        {
            f = 0;
            for (qint16 j = 0; j < args.count(); j++)
            {
                if(args.at(args.count() - (j + 1)) == m_items.at(i)->args().at(j))
                    f++;
            }
            if (f == args.count())
            {
                if(!optionalArgs.isEmpty())
                {
                    if(m_items.at(i)->optionalArgs().count() == optionalArgs.count())
                    {
                        f = 0;
                        for (qint16 j = 0; j < optionalArgs.count(); j++)
                            if(optionalArgs.at(optionalArgs.count() - (j + 1)) == m_items.at(i)->optionalArgs().at(j))
                                f++;

                        if (f == optionalArgs.count())
                        {
                            index = i;
                            return true;
                        }
                    }
                }
                else
                {
                    index = i;
                    return true;
                }
            }
        }
    }
    index = m_items.count();
    return false;
}

bool CommandCollection::load(TplCursor& cursor)
{
    cursor.read();
    //формируем список функций из подключаемых файлов с шаблонами
    while(cursor.string() == "include")
    {
        cursor.read();
//        qDebug() << "load include: " << cursor.string() + ".tpl";
        QString tplFileaName = cursor.string() + ".tpl";
        loadInclude(tplFileaName.toUpper());
        cursor.read();
    }
    //формируем список функций/операторов    (<name> [imp] [arg] [ret] [out])
    while (cursor.string() == "name")
    {
        Command* command = new Command();
        command->initialize(cursor);

        if(insert(command) < 0)
        {
            qCritical() << QString("Duplicated operator (%1) in compiler.tpl!").arg(command->name());
            delete command;
        }
        if (cursor.isEof())
            return true;
    }
    return true;
}

bool CommandCollection::loadInclude(const QString& tplFileName)
{
    QString includeTplPath = PathManager::instance().templateDir().absoluteFilePath(tplFileName.toUpper());
//#ifdef Q_OS_ANDROID
//    includeTplPath = "assets:/template/" + tplFileName.toUpper();
//#endif
    QFileInfo tplFileInfo(includeTplPath);

    QFile includeTplFile(includeTplPath);
    if(includeTplFile.open(QFile::ReadOnly))
    {
        QTextStream stream(&includeTplFile);
        TplCursor cursor(stream);

        cursor.read();
        while(!cursor.isEof())
        {
            if(cursor.string() == "name")
            {
                Command* command = new Command();
                command->initialize(cursor);

                if(insert(command) < 0)
                {
                    qCritical() << QString("Duplicated operator (%1) in %2!").arg(command->name()).arg(tplFileInfo.fileName());
                    delete command;
                    return false;
                }
            }
            else
            {
                qCritical() << "Error in file: " << includeTplPath;
                return false;
            }
        }
    }
    else
    {
        qCritical() << "Can't open compiler template file: " << includeTplPath;
        return false;
    }
    return true;
}

}
