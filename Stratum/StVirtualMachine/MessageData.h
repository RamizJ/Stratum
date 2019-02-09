#ifndef MESSAGEDATA_H
#define MESSAGEDATA_H

#include "VmTypes.h"

namespace StVirtualMachine {


class MessageData
{
public:
    MessageData();
    virtual ~MessageData() = default;

    virtual void accept(MessageDataVisitor* visitor) = 0;

    int messageCode() const;
    void setMessageCode(int messageCode);

private:
    int m_messageCode;
};

class GeneralMessageData : public MessageData
{
public:
    GeneralMessageData();

    // MessageData interface
public:
    void accept(MessageDataVisitor* visitor);
};


class MouseMessageData : public MessageData
{
public:
    MouseMessageData();

    double x() const;
    void setX(double x);

    double y() const;
    void setY(double y);

    double keys() const;
    void setKeys(double keys);

    // MessageData interface
public:
    void accept(MessageDataVisitor* visitor);

private:
    double m_x;
    double m_y;
    double m_keys;
};


class KeyboardMessageData : public MessageData
{
public:
    KeyboardMessageData();

    double wVkey() const;
    void setWVkey(double wVkey);

    double repeat() const;
    void setRepeat(double repeat);

    double scanCode() const;
    void setScanCode(double scanCode);

    // MessageData interface
public:
    void accept(MessageDataVisitor* visitor);

private:
    double m_wVkey;
    double m_repeat;
    double m_scanCode;
};


class CommandMessageData : public MessageData
{
public:
    CommandMessageData();

    double Id() const;
    void setId(double Id);

    double wNotifyCode() const;
    void setWNotifyCode(double wNotifyCode);

    int handle() const;
    void setHandle(int handle);

    // MessageData interface
public:
    void accept(MessageDataVisitor* visitor);

private:
    double m_id;
    double m_wNotifyCode;
    int m_handle;
};


class SizeMessageData : public MessageData
{
public:
    SizeMessageData();

    double fwSizeType() const;
    void setFwSizeType(double fwSizeType);

    double nWidth() const;
    void setNWidth(double nWidth);

    double nHeight() const;
    void setNHeight(double nHeight);

    // MessageData interface
public:
    void accept(MessageDataVisitor* visitor);


private:
    double m_fwSizeType;
    double m_nWidth;
    double m_nHeight;
};


class MinMaxMessageData : public MessageData
{
public:
    MinMaxMessageData();

    double maxSizeX() const;
    void setMaxSizeX(double maxSizeX);

    double maxSizeY() const;
    void setMaxSizeY(double maxSizeY);

    double maxPositionX() const;
    void setMaxPositionX(double maxPositionX);

    double maxPositionY() const;
    void setMaxPositionY(double maxPositionY);

    double minTrackSizeX() const;
    void setMinTrackSizeX(double minTrackSizeX);

    double minTrackSizeY() const;
    void setMinTrackSizeY(double minTrackSizeY);

    double maxTrackSizeX() const;
    void setMaxTrackSizeX(double maxTrackSizeX);

    double maxTrackSizeY() const;
    void setMaxTrackSizeY(double maxTrackSizeY);

    // MessageData interface
public:
    void accept(MessageDataVisitor* visitor);

private:
    double m_maxSizeX;
    double m_maxSizeY;

    double m_maxPositionX;
    double m_maxPositionY;

    double m_minTrackSizeX;
    double m_minTrackSizeY;

    double m_maxTrackSizeX;
    double m_maxTrackSizeY;
};


}

#endif // MESSAGEDATA_H
