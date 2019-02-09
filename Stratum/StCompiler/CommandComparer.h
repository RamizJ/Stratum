#ifndef COMMANDCOMPARER
#define COMMANDCOMPARER

#include <Command.h>

namespace StCompiler {

class CommandComparer
{
public:
    int compare(Command* cmd1, Command* cmd2)
    {
        int res = QString::compare(cmd1->name(), cmd2->name(), Qt::CaseInsensitive);
        if(res != 0)
            return res;

        if (cmd1->returnType() != cmd2->returnType())
            return cmd1->returnType() > cmd2->returnType() ? 1 : -1;

        if (cmd1->args().count() != cmd2->args().count())
            return cmd1->argsCount() > cmd2->optionalArgsCount() ? 1 : -1;

        for (int i = 0; i < cmd1->argsCount(); i++)
            if(cmd1->args().at(i) != cmd2->args().at(i))
                return cmd1->args().at(i) > cmd2->args().at(i) ? 1 : -1;

        if (cmd1->argsCount() != cmd2->optionalArgsCount())
            return cmd1->argsCount() > cmd2->optionalArgsCount() ? 1 : -1;

        for (int i = 0; i < cmd1->optionalArgsCount(); i++)
        {
            if (cmd1->optionalArgs().at(i) != cmd2->optionalArgs().at(i))
                return cmd1->optionalArgs().at(i) > cmd2->optionalArgs().at(i) ? 1 : -1;
        }

        return 0;
    }
};

class CommandNameComparer
{
public:
    int compare(Command* cmd1, Command* cmd2)
    {
        return QString::compare(cmd1->name(), cmd2->name());
    }
};

}

#endif // COMMANDCOMPARER

