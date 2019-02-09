#include "MessageData.h"
#include "MessageDataVisitor.h"

namespace StVirtualMachine {

MessageData::MessageData() :
    m_messageCode(0)
{}

int MessageData::messageCode() const
{
    return m_messageCode;
}

void MessageData::setMessageCode(int messageCode)
{
    m_messageCode = messageCode;
}



GeneralMessageData::GeneralMessageData() : MessageData()
{}

void GeneralMessageData::accept(MessageDataVisitor* visitor)
{
    visitor->visit(this);
}



MouseMessageData::MouseMessageData() : MessageData(),
    m_x(0),
    m_y(0),
    m_keys(0)
{}


void MouseMessageData::accept(MessageDataVisitor* visitor)
{
    visitor->visit(this);
}

double MouseMessageData::x() const
{
    return m_x;
}

void MouseMessageData::setX(double x)
{
    m_x = x;
}

double MouseMessageData::y() const
{
    return m_y;
}

void MouseMessageData::setY(double y)
{
    m_y = y;
}

double MouseMessageData::keys() const
{
    return m_keys;
}

void MouseMessageData::setKeys(double keys)
{
    m_keys = keys;
}


KeyboardMessageData::KeyboardMessageData() : MessageData(),
    m_scanCode(0),
    m_wVkey(0),
    m_repeat(0)
{}


void KeyboardMessageData::accept(MessageDataVisitor* visitor)
{
    visitor->visit(this);
}

double KeyboardMessageData::wVkey() const
{
    return m_wVkey;
}

void KeyboardMessageData::setWVkey(double wVkey)
{
    m_wVkey = wVkey;
}

double KeyboardMessageData::repeat() const
{
    return m_repeat;
}

void KeyboardMessageData::setRepeat(double repeat)
{
    m_repeat = repeat;
}

double KeyboardMessageData::scanCode() const
{
    return m_scanCode;
}

void KeyboardMessageData::setScanCode(double scanCode)
{
    m_scanCode = scanCode;
}



CommandMessageData::CommandMessageData() : MessageData(),
    m_id(0),
    m_wNotifyCode(0),
    m_handle(0)
{}

double CommandMessageData::Id() const
{
    return m_id;
}

void CommandMessageData::setId(double idItem)
{
    m_id = idItem;
}

double CommandMessageData::wNotifyCode() const
{
    return m_wNotifyCode;
}

void CommandMessageData::setWNotifyCode(double wNotifyCode)
{
    m_wNotifyCode = wNotifyCode;
}

int CommandMessageData::handle() const
{
    return m_handle;
}

void CommandMessageData::setHandle(int handle)
{
    m_handle = handle;
}


void CommandMessageData::accept(MessageDataVisitor* visitor)
{
    visitor->visit(this);
}



SizeMessageData::SizeMessageData() : MessageData(),
    m_fwSizeType(0),
    m_nHeight(0),
    m_nWidth(0)
{}

double SizeMessageData::fwSizeType() const
{
    return m_fwSizeType;
}

void SizeMessageData::setFwSizeType(double fwSizeType)
{
    m_fwSizeType = fwSizeType;
}

double SizeMessageData::nWidth() const
{
    return m_nWidth;
}

void SizeMessageData::setNWidth(double nWidth)
{
    m_nWidth = nWidth;
}

double SizeMessageData::nHeight() const
{
    return m_nHeight;
}

void SizeMessageData::setNHeight(double nHeight)
{
    m_nHeight = nHeight;
}

void SizeMessageData::accept(MessageDataVisitor* visitor)
{
    visitor->visit(this);
}



MinMaxMessageData::MinMaxMessageData() : MessageData(),
    m_maxSizeX(0),
    m_maxSizeY(0),
    m_maxPositionX(0),
    m_maxPositionY(0),
    m_minTrackSizeX(0),
    m_minTrackSizeY(0),
    m_maxTrackSizeX(0),
    m_maxTrackSizeY(0)
{}

double MinMaxMessageData::maxSizeX() const
{
    return m_maxSizeX;
}

void MinMaxMessageData::setMaxSizeX(double maxSizeX)
{
    m_maxSizeX = maxSizeX;
}

double MinMaxMessageData::maxSizeY() const
{
    return m_maxSizeY;
}

void MinMaxMessageData::setMaxSizeY(double maxSizeY)
{
    m_maxSizeY = maxSizeY;
}

double MinMaxMessageData::maxPositionX() const
{
    return m_maxPositionX;
}

void MinMaxMessageData::setMaxPositionX(double maxPositionX)
{
    m_maxPositionX = maxPositionX;
}

double MinMaxMessageData::maxPositionY() const
{
    return m_maxPositionY;
}

void MinMaxMessageData::setMaxPositionY(double maxPositionY)
{
    m_maxPositionY = maxPositionY;
}

double MinMaxMessageData::minTrackSizeX() const
{
    return m_minTrackSizeX;
}

void MinMaxMessageData::setMinTrackSizeX(double minTrackSizeX)
{
    m_minTrackSizeX = minTrackSizeX;
}

double MinMaxMessageData::minTrackSizeY() const
{
    return m_minTrackSizeY;
}

void MinMaxMessageData::setMinTrackSizeY(double minTrackSizeY)
{
    m_minTrackSizeY = minTrackSizeY;
}

double MinMaxMessageData::maxTrackSizeX() const
{
    return m_maxTrackSizeX;
}

void MinMaxMessageData::setMaxTrackSizeX(double maxTrackSizeX)
{
    m_maxTrackSizeX = maxTrackSizeX;
}

double MinMaxMessageData::maxTrackSizeY() const
{
    return m_maxTrackSizeY;
}

void MinMaxMessageData::setMaxTrackSizeY(double maxTrackSizeY)
{
    m_maxTrackSizeY = maxTrackSizeY;
}

void MinMaxMessageData::accept(MessageDataVisitor* visitor)
{
    visitor->visit(this);
}


}
