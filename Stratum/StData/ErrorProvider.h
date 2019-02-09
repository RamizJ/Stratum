#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <stdata_global.h>
#include <QString>
#include <memory>

namespace StData {

class Error;
typedef std::shared_ptr<Error> ErrorPtr;

class STDATASHARED_EXPORT ErrorHolder
{
public:
    ErrorHolder();

    void cleanError();
    void setError(const QString& errMsg, ErrorPtr innerError = nullptr, int code = -1);
    void setError(const ErrorPtr& error);

    bool hasError();
    ErrorPtr lastError() const {return m_lastError;}

    QString errorText();

private:
    ErrorPtr m_lastError;
};

}

#endif // ERRORHANDLER_H
