#include "ErrorProvider.h"
#include <Error.h>
#include <Log.h>

namespace StData {

ErrorHolder::ErrorHolder() :
    m_lastError(nullptr)
{}

void ErrorHolder::cleanError()
{
    m_lastError = nullptr;
}

void ErrorHolder::setError(const QString& errMsg, ErrorPtr innerError, int code)
{
    m_lastError = std::make_shared<Error>(errMsg, code, innerError);
    SystemLog::instance().error(errMsg);
}

void ErrorHolder::setError(const ErrorPtr& error)
{
    m_lastError = error;
}

bool ErrorHolder::hasError()
{
    return m_lastError != nullptr;
}

QString ErrorHolder::errorText()
{
    return m_lastError != nullptr ? m_lastError->toString() : "";
}

}
