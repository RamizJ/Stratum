#ifndef MESSAGE_H
#define MESSAGE_H

#include "VmTypes.h"
#include "MessageDataVisitor.h"

#include <QGraphicsSceneMouseEvent>
#include <QMap>
#include <QObject>
#include <QVector>
#include <QQueue>
#include <QKeyEvent>

namespace StSpace {
class SpaceWidget;
class SpaceScene;
}

namespace StData {
class Project;
class Object;
class SpaceItem;
class ObjectVar;
struct VarData;
}

namespace StVirtualMachine {

class VirtualMachine;
class SpaceWindowItem;

class MessageHandler : public QObject, MessageDataVisitor
{
    Q_OBJECT
public:
    enum MessageCode
    {
        WM_DESTROY = 2, WM_MOVE = 3, WM_SIZE = 5, WM_CLOSE = 16, WM_GETMINMAXINFO = 36,
        WM_COMMAND = 273,
        WM_MOUSEMOVE = 512, WM_LBUTTONDOWN = 513, WM_LBUTTONUP = 514, WM_LBUTTONDBLCLK = 515,
        WM_RBUTTONDOWN = 516, WM_RBUTTONUP = 517, WM_RBUTTONDBLCLK = 518,
        WM_MBUTTONDOWN = 519, WM_MBUTTONUP = 520, WM_MBUTTONDBLCLK = 521, WM_ALLMOUSEMESSAGE = 1536,
        WM_ALLKEYMESSAGE = 1537, WM_KEYDOWN = 256, WM_KEYUP = 257, WM_CHAR = 258,

        WM_CANCLOSE = 1538, WM_SPACEDONE = 1539, WM_SPACEINIT = 1540, WM_CONTROLNOTIFY = 1544,
        WM_HYPERJUMP = 1546,
    };

public:
    explicit MessageHandler(VirtualMachine* virtualMachine, StVirtualMachine::SpaceWindowItem* spaceWindowItem,
                            StData::Object* object, StData::SpaceItem* spaceItem,
                            int code, int flag, QObject* parent = nullptr);

//    static MessageHandlerPtr create(VirtualMachine* virtualMachine, StVirtualMachine::SpaceWindowItem* spaceWindowItem,
//                                    StData::Object* object, StData::SpaceItem* spaceItem,
//                                    int code, int flag, QObject* parent = nullptr);
    ~MessageHandler();

    SpaceWindowItem* spaceWindowItem() const;

    StSpace::SpaceWidget* spaceWidget() const { return m_spaceWidget; }
    void setSpaceWidget(StSpace::SpaceWidget* spaceWidget) { m_spaceWidget = spaceWidget; }

    StData::Object* object() const { return m_object; }
    void setObject(StData::Object* object) { m_object = object; }

    StData::SpaceItem* spaceItem() const { return m_spaceItem; }
    void setSpaceItem(StData::SpaceItem* spaceItem) { m_spaceItem = spaceItem; }

    int code() const { return m_code; }
    void setCode(int code) { m_code = code; }

    int flag() const { return m_flag; }
    void setFlag(int flag) { m_flag = flag; }

    bool isCapture() const { return m_isCaptured; }
    void setCapture() { m_isCaptured = true; }
    void releaseCapture() { m_isCaptured = false; }

    void processSendedMessages();

public:
    bool event(QEvent*event);
    bool eventFilter(QObject* sender, QEvent* event);

    // MessageDataVisitor interface
public:
    void visit(GeneralMessageData* msgData);
    void visit(MouseMessageData* msgData);
    void visit(KeyboardMessageData* msgData);
    void visit(CommandMessageData* msgData);
    void visit(SizeMessageData* msgData);
    void visit(MinMaxMessageData*);

private slots:
    void onSceneMouseEventOnParent(QGraphicsSceneMouseEvent* parentEvent);
    void editTextChanged();
    void editFocusIn();
    void editFocusOut();
    void buttonCLicked();

private:

    void setVarF(const int& index, const double& value);
    void setVarH(const int& index, const int& value);
    void setVarS(const int& index, const QString& value);

    void send(const MessageDataPtr& msgData);
    void sendGeneralMessage(int msgCode);
    void sendMouseMesssage(int messageCode, double x, double y, double fwKeys = 0);
    void sendKeyboardMessage(int messageCode, double scanCode, double wKey, double repeat);
    void sendSizeMessage(int msgCode, double w, double h, double type);
    void sendCommandMessage(int msgCode, int handle, double notyfyCode, double id);
    void processSendedMessage(int messageCode);

    bool isChildWindowUnderPoint(StSpace::SpaceScene* scene, const QPointF& scenePos, QWidget* childWindow);


private:
    enum VarIndex
    {
        VI_MSG, VI_XPOS, VI_YPOS, VI_FWKEYS,
        VI_NWIDTH, VI_NHEIGHT, VI_FWSIZETYPE,
        VI_WVKEY, VI_REPEAT, VI_SCANCODE,
        VI_CANCLOSE, VI_HSPACE, VI_WNDNAME, VI_TARGET, VI_HOBJECT, VI_NOTIFYCODE,
        VI_MAXSIZEX, VI_MAXSIZEY, VI_MAXPOSITIONX, VI_MAXPOSITIONY,
        VI_MINTRACKSIZEX, VI_MINTRACKSIZEY, VI_MAXTRACKSIZEX, VI_MAXTRACKSIZEY,
        VI_IDITEM
    };

private:
    VirtualMachine* m_virtualMachine;
    SpaceWindowItem* m_spaceWindowItem;
    StSpace::SpaceWidget* m_spaceWidget;
    StData::Object* m_object;
    StData::SpaceItem* m_spaceItem;
    int m_code;
    int m_flag;

    QVector<StData::ObjectVar*> m_specialVars;
    StData::ObjectVar* m_msg;

    QQueue<MessageDataPtr> m_sendedMessages;

    bool m_inProcess;
    bool m_isCaptured;
};

}

#endif // MESSAGE_H
