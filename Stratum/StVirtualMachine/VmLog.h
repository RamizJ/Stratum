#ifndef VMLOG_H
#define VMLOG_H

#include <QObject>
#include <Log.h>

#include "stvirtualmachine_global.h"

namespace StData {
class LogMessage;
}

namespace StVirtualMachine {

class STVIRTUALMACHINESHARED_EXPORT VmLog : public QObject
{
    Q_OBJECT
private:
    explicit VmLog(QObject* parent = nullptr);

public:
    static VmLog& instance();

    void info(const QString& message);
    void warning(const QString& message);
    void error(const QString& message);

signals:
    void messageWrited(const QString& formattedMessage, const StData::MessageType& msgType);

private:
    StData::Log* m_log;
};

}

#endif // VMLOG_H
