#ifndef MESSAGEDATAVISITOR_H
#define MESSAGEDATAVISITOR_H

namespace StVirtualMachine {

class GeneralMessageData;
class MouseMessageData;
class KeyboardMessageData;
class CommandMessageData;
class SizeMessageData;
class MinMaxMessageData;


class MessageDataVisitor
{
public:
    virtual ~MessageDataVisitor() = default;

    virtual void visit(GeneralMessageData* msgData) = 0;
    virtual void visit(MouseMessageData* msgData) = 0;
    virtual void visit(KeyboardMessageData* msgData) = 0;
    virtual void visit(CommandMessageData* msgData) = 0;
    virtual void visit(SizeMessageData* msgData) = 0;
    virtual void visit(MinMaxMessageData* msgData) = 0;
};

}

#endif // MESSAGEDATAVISITOR_H
