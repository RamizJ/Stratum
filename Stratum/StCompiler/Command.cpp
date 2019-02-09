#include "Command.h"
#include "TplCursor.h"

#include <LibraryCollection.h>
#include <Log.h>
#include <QDebug>
#include <Sc2000DataStream.h>
#include <memory>

using namespace StData;

namespace StCompiler {

Command::Command(const QString& name, const QList<Class*>& args,
                 const QList<Class*>& optionalArgs, Class* returnType) :
    m_id(0),
    m_startPriority(0),
    m_endPriority(0),
    m_isOut(0),
    m_isIndex(0),
    m_isName(false),
    m_name(name),
    m_args(args),
    m_optionalArgs(optionalArgs),
    m_returnType(returnType),
    m_refArgsNumbers(0)
{}

Command::~Command()
{}

bool Command::initialize(TplCursor& cursor)
{
    if (cursor.read() == OperatorType::sconst)
        m_name = cursor.string().toLower();
    else
        errorResult("Read command name");

//    qDebug() << "load command: " << m_name;
    cursor.read();
    if(cursor.string() == "imp")
    {
        if(!readPriority(cursor))
            return false;
    }
    if (cursor.string() == "arg")
    {
        if(!readArgs(cursor))
            return false;
    }
    return readReturn(cursor);
}

bool Command::out(St2000DataStream& stream, qint16 index)
{
    if (m_isOut)
        stream << m_id;

    if (m_isIndex)
    {
        stream << index;
        return true;
    }

    if (m_isName)
        stream << m_name;

    return true;
}

void Command::changeName(const QString& name)
{
    m_name = name;
}

bool Command::argIsRef(qint16 argIndex)
{
    return (m_refArgsNumbers & (1 << argIndex)) != 0;
}

bool Command::readOptionalArgs(TplCursor& cursor)
{
    do
    {
        if (cursor.read() == OperatorType::optr && cursor.string()[0] == '&') // ссылка !
        {
            m_refArgsNumbers |= (1 << (m_args.count() + m_optionalArgs.count() - 2));
            cursor.read();
        }
        if (cursor.type() != OperatorType::sconst)
            return errorResult("Next lexeme must be a type name");

        if(Class* cl = LibraryCollection::getClassByName(cursor.string()))
            m_optionalArgs << cl;
        else
            return errorResult(QString("Compiler temlate file syntax error : Unknown type: %1 detected !")
                               .arg(cursor.string()));
    }
    while (cursor.read() == OperatorType::optr && cursor.string()[0] == ',');

    if (cursor.string()[0] != ']')
        return errorResult("End of optional args");

    cursor.read();

    return true;
}

bool Command::readPriority(TplCursor& cursor)
{
    if (cursor.read() != OperatorType::dconst)
        return errorResult("Read start priority");

    m_startPriority = cursor.string().toInt();
    m_endPriority = m_startPriority;

    cursor.read();
    if(cursor.string()[0] == ',')
    {
        if (cursor.read() != OperatorType::dconst)
            return errorResult("Read end priority");

        m_endPriority = cursor.string().toInt();
        cursor.read();
    }
    return true;
}

bool Command::readArgs(TplCursor& cursor)
{
    int i = 0;
    do
    {
        cursor.read();

        if (cursor.type() == OperatorType::optr && cursor.string()[0] == '[')
            return readOptionalArgs(cursor);

        if (cursor.type() == OperatorType::optr && cursor.string()[0] == '&')   // ссылка !
        {
            m_refArgsNumbers |= 1 << i;
            cursor.read();
        }

        if(cursor.type() == OperatorType::sconst)
        {
            if(Class* cl = LibraryCollection::getClassByName(cursor.string()))
            {
                m_args << cl;
                i++;
            }
            else
                return errorResult(QString("Compiler temlate file syntax error : Unknown type: %1 detected !")
                                   .arg(cursor.string()));
        }
        else
            return errorResult("Next lexeme must be a type name");
    }
    while (cursor.read() == OperatorType::optr && cursor.charAt(0) == ',');

    return true;
}

bool Command::readReturn(TplCursor& cursor)
{
    if (cursor.string() == "ret")
    {
        if (cursor.read() != OperatorType::sconst)
            return errorResult("Bad return type name");

        if(Class* cl = LibraryCollection::getClassByName(cursor.string()))
            m_returnType = cl;
        else
            return errorResult(QString("Syntax error: Unknown type: %1 detected !")
                               .arg(cursor.string()));

        cursor.read();
    }

    if (cursor.string() == "out")
    {
        if (cursor.read() == OperatorType::dconst)
        {
            m_id = cursor.string().toInt();
            m_isOut = true;

            cursor.read();
            if (cursor.string()[0] != ',')
                return true;

            cursor.read();
        }

        if (cursor.type() != OperatorType::optr)
            errorResult("Bad return type name");

        if (cursor.string()[0] == '$')
        {
            cursor.read();
            if (cursor.string() == "name")
            {
                m_isName = true;
                cursor.read();
                return true;
            }

            if (cursor.string() == "index")
            {
                m_isIndex = true;
                cursor.read();
                return true;
            }
            return errorResult("Need keyword name or index");
        }
    }
    if (cursor.type() != OperatorType::opnd)
        return errorResult("Next type must be operand");

    return true;
}

bool Command::errorResult(const QString& msg)
{
    SystemLog::instance().error("Error compiler command initialize. " + msg);
    return false;
}

}
