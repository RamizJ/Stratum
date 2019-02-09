#include "Error.h"

namespace StData {

Error::Error(const QString& msg, int errorCode, const ErrorPtr& innerError) :
    m_message(msg),
    m_code(errorCode),
    m_innerError(innerError)
{}

QString Error::toString()
{
    QString errStr = QString("%1\n").arg(m_message);

    if(m_innerError)
        errStr += m_innerError->toString();

    return errStr;
}










}
