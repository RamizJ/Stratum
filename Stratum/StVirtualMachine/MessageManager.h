#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "SpaceWindowItem.h"
#include "VmTypes.h"

#include <QMap>
#include <QObject>

namespace StData {
class Object;
class SpaceItem;
}

namespace StSpace {
class SpaceWidget;
}

namespace StVirtualMachine {

class VirtualMachine;

class MessageManager : public QObject
{
    Q_OBJECT
public:
    explicit MessageManager(VirtualMachine* virtualMachine, QObject* parent = nullptr);

    bool registerObject(SpaceWindowItem* spaceWindowItem, StData::Object* object,
                        StData::SpaceItem* spaceItem, int code, int flag);

    bool unregisterObject(SpaceWindowItem* spaceWindowItem, StData::Object* object, int msgCode);
    void unregisterObjects(SpaceWindowItem* spaceWindowItem);

    void sendMessage(StData::Object* sourceObject, const QString& className, const QString& targetObjectsPath,
                     const QList<QPair<QString, QString> >& varNames);

    void reset();
    void reset(StData::Project* project);

    void setCapture(StData::Object* object, SpaceWindowItem* spaceWindowItem, int flag);
    void releaseCapture();

    QList<MessageHandlerPtr> messageHandlers() const { return m_messageHandlers; }

    void processSendedMessages();

private slots:
    void windowClosed();

private:
    bool isMessageExist(StData::Object* object, StData::SpaceItem* spaceItem, int code);
    MessageHandlerPtr create(StVirtualMachine::SpaceWindowItem* spaceWindowItem,
                             StData::Object* object, StData::SpaceItem* spaceItem,
                             int code, int flag);

private:
    QList<MessageHandlerPtr> m_messageHandlers;
    MessageHandlerPtr m_capturedHandler;
    VirtualMachine* m_virtualMachine;
};

}

#endif // MESSAGEMANAGER_H
