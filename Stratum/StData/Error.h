#ifndef ERROR_H
#define ERROR_H

#include <stdata_global.h>
#include <QString>
#include <memory>

namespace StData {

class Error;
typedef std::shared_ptr<Error> ErrorPtr;

class STDATASHARED_EXPORT Error
{
public:
    Error(const QString& msg, int errorCode = -1, const ErrorPtr& innerError = nullptr);

    QString message() const { return m_message; }
    int code() const { return m_code; }
    ErrorPtr internalError() const { return m_innerError; }

    QString toString();

private:
    int m_code;
    QString m_message;

    ErrorPtr m_innerError;
};

}

#endif // ERROR_H
