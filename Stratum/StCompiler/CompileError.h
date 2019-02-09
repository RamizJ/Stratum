#ifndef COMPILEERROR_H
#define COMPILEERROR_H

#include <QMap>
#include <QString>
#include <memory>

namespace StCompiler {

class TplCursor;

class CompileError
{
public:
    enum ErrorCode
    {
        Syntax,
        Internal,
        UnknownType,
        NfOptr,
        ExtraPrm,
        FewPrm,
        FuncNotFound,
        MbVar,
        MbOpnd,
        MbOptr,
        UnEof,
        UnConstr,
        ReturnValue,
        FirstVar,
        VarExist,
        NsType,
        MbNum,
        EqInternal,
        VarNotFound,
        Arg,
        Incorrect,
        Incorrectt,
        Wait,
        MbString,
        Misplaced,
    };

public:
    CompileError(TplCursor& cursor, ErrorCode errorCode, const QString message);
    CompileError(ErrorCode errorCode, const QString message);
    ~CompileError();

    QString message() const { return m_message; }
    void setMessage(const QString& message) { m_message = message; }

    ErrorCode errorCode() const { return m_errorCode; }

    bool isFuncNameChar(const QChar& ch);

    int startPos() const {return m_startPos;}
    int endPos() const {return m_endPos;}

private:
    QString m_message;
    ErrorCode m_errorCode;
    QString m_detailText;

    int m_startPos;
    int m_endPos;
};

}

#endif // COMPILEERROR_H
