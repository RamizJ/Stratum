#include "TplLoader.h"
#include "TplCursor.h"
#include "CommandCollection.h"

#include <QDebug>
#include <QFileInfo>
#include <Context.h>

using namespace StCore;

namespace StCompiler {

TplLoader::TplLoader()
{}

TplLoader::~TplLoader()
{}

bool TplLoader::loadCommands(StCompiler::Cursor& cursor, CommandCollection* commandCollection)
{
    cursor.read();
    //формируем список функций из подключаемых файлов с шаблонами
    while(cursor.string() == "include")
    {
        cursor.read();
        loadInclude(cursor.string() + ".tpl", commandCollection);
        cursor.read();
    }
    //формируем список функций/операторов    (<name> [imp] [arg] [ret] [out])
    while (cursor.string() == "name")
    {
        Command* command = new Command();
        command->initialize(cursor);

        if(!commandCollection->addCommand(command))
        {
            qCritical() << QString("Duplicated operator (%1) in compiler.tpl!").arg(command->name());
            delete command;
        }
        if (cursor.isEof())
            return true;
    }
    return true;
}

bool TplLoader::loadInclude(const QString& tplFileName, CommandCollection* commandColletion)
{

    QString absoluteTplFilePath = Context::instance().stratumDir().absoluteFilePath(tplFileName);
    QFileInfo tplFileInfo(absoluteTplFilePath);

    QFile includeTplFile(absoluteTplFilePath);
    if(includeTplFile.open(QFile::ReadOnly))
    {
        QTextStream stream(&includeTplFile);
        Cursor cursor(stream);

        cursor.read();
        while(!cursor.isEof())
        {
            if(cursor.string() == "name")
            {
                Command* command = new Command();
                command->initialize(cursor);
                qDebug() << command->name();

                if(!commandColletion->addCommand(command))
                {
                    qCritical() << QString("Duplicated operator (%1) in %2!").arg(command->name()).arg(tplFileInfo.fileName());
                    delete command;
                    return false;
                }
            }
            else
            {
                qCritical() << "Error in file: " << absoluteTplFilePath;
                return false;
            }
        }
    }
    else
    {
        qCritical() << "Can't open compiler template file: " << absoluteTplFilePath;
        return false;
    }
    return true;
}

bool TplLoader::loadConstants(QHash<QString, double>& constantDictionary)
{
    QString absoluteTplFilePath = Context::instance().stratumDir().absoluteFilePath("constant.tpl");
    //    QFileInfo tplFileInfo(absoluteTplFilePath);

    QFile includeTplFile(absoluteTplFilePath);
    if(includeTplFile.open(QFile::ReadOnly))
    {
        QTextStream stream(&includeTplFile);
        Cursor cursor(stream);

        while(!cursor.isEof())
        {
            cursor.read();
            if(cursor.isEof())
                return true;

            if(cursor.type() == OperatorType::opnd)
            {
                QString constName = cursor.string();
                double value(0.0);

                cursor.read();
                switch (cursor.type())
                {
                    case OperatorType::dconst:
                    case OperatorType::fconst:
                        value = cursor.string().toDouble();
                        break;

                    case OperatorType::hconst:
                        value = cursor.string().toInt(0, 16);
                        break;

                    default:
                        qCritical() << "error type for constant:" << constName;
                        break;
                }

                if(!constantDictionary.contains(constName))
                    constantDictionary[constName] = value;
                else
                {
                    qCritical() << QString("Duplicated constant (%1) in constant.tpl!").arg(constName);
                    return false;
                }
            }
            else
            {
                qCritical() << "error while reading const from constant.tpl";
                return false;
            }
        }
    }
    else
    {
        qCritical() << "Can't open constant template file: " << absoluteTplFilePath;
        return false;
    }
    return true;
}

}
