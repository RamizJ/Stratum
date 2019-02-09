#include "CompileError.h"
#include "TplCursor.h"
#include <QObject>

namespace StCompiler {

CompileError::CompileError(TplCursor& cursor, ErrorCode errorCode,
                           const QString message) :
    m_errorCode(errorCode),
    m_message(message),
    m_startPos(0),
    m_endPos(0)
{
    switch(errorCode)
    {
        case ErrorCode::UnknownType:
            m_startPos = m_endPos = cursor.counter() - cursor.string().length();
            break;

        case ErrorCode::ExtraPrm:
        case ErrorCode::FewPrm:
        case ErrorCode::FuncNotFound:
        {
            int i = 1;
            int f = 1;
            for (; f > 0; i++)
            {
                if(cursor.buffer()[cursor.counter() - i] == ')')
                    f++;

                else if(cursor.buffer()[cursor.counter() - i] == '(')
                    f--;
            }
            //    	Err_x2 = id->counter-i+1;
            for (i++; isFuncNameChar(cursor.buffer().at(cursor.counter()-i)); i++);

            m_startPos = cursor.counter() - i + 1;
            m_endPos = cursor.counter() + 1;
        }break;

        case ErrorCode::VarExist:
            m_startPos = m_endPos = cursor.counter() + 1 - cursor.string().length();
            break;

        default:
            m_startPos = cursor.counter() - cursor.string().length() - 1;
            m_endPos = cursor.counter();
            break;
    }
}

CompileError::CompileError(ErrorCode errorCode, const QString message) :
    m_errorCode(errorCode),
    m_message(message),
    m_startPos(0),
    m_endPos(0)
{}

CompileError::~CompileError()
{}

bool CompileError::isFuncNameChar(const QChar& ch)
{
    return ch.isLetterOrNumber() || ch == '_';
}

}
