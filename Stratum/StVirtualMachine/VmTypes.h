#ifndef VMTYPES_H
#define VMTYPES_H

#include <memory>

namespace StVirtualMachine {

class MessageHandler;

class MessageData;
class GeneralMessageData;
class MouseMessageData;
class KeyboardMessageData;
class CommandMessageData;
class SizeMessageData;
class MinMaxMessageData;

class MessageDataVisitor;


typedef std::shared_ptr<MessageHandler> MessageHandlerPtr;
typedef std::weak_ptr<MessageHandler> MessageHandlerWPtr;

typedef std::shared_ptr<MessageData> MessageDataPtr;
typedef std::shared_ptr<GeneralMessageData> GeneralMessageDataPtr;
typedef std::shared_ptr<MouseMessageData> MouseMessageDataPtr;
typedef std::shared_ptr<KeyboardMessageData> KeyboardMessageDataPtr;
typedef std::shared_ptr<CommandMessageData> CommandMessageDataPtr;
typedef std::shared_ptr<SizeMessageData> SizeMessageDataPtr;
typedef std::shared_ptr<MinMaxMessageData> MinMaxMessageDataPtr;

}

#endif // VMTYPES_H
