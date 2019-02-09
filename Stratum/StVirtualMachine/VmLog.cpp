#include "VmLog.h"

using namespace StData;

namespace StVirtualMachine {

VmLog& VmLog::instance()
{
    static VmLog log;
    return log;
}

VmLog::VmLog(QObject* parent) :
    QObject(parent)
{
    m_log = new Log(this);
    connect(m_log, &Log::messageWrited, this, &VmLog::messageWrited);
}

void VmLog::info(const QString& message)
{
    m_log->info(message);
}

void VmLog::warning(const QString& message)
{
    m_log->warning(message);
}

void VmLog::error(const QString& message)
{
    m_log->error(message);
}

}
