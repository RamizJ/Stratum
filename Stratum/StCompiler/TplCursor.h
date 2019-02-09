#ifndef TPLPARSER_H
#define TPLPARSER_H

#include <QString>
#include <QTextCodec>
#include <QTextStream>
#include "Command.h"

namespace StCompiler {

class TplCursor
{
public:
    TplCursor(QTextStream& stream);
    ~TplCursor();

    OperatorType read();
    uchar getNextChar();
    bool isNextOperator(uchar ch);

    QString string();
    char charAt(int index) const;
    char bufCharAt(int index) const;
    bool isEof() const {return m_isEof;}
    OperatorType type() const;

    bool isEquation();

    qint16 scanChar(char ch);
    void back();

    qint16 newLine() const { return m_newLine; }
    void setNewLine(const qint16& newLine) { m_newLine = newLine; }

    qint16 lastPos() const { return m_lastPos; }
    qint16 counter() const { return m_counter; }

    QByteArray buffer() const { return m_buffer; }

private:
    qint16 getCharGroup(uchar ch);
    OperatorType handleOperator(char ch);

private:
    QTextStream& m_stream;
    bool m_isEof;
    int m_counter;

    QByteArray m_buffer;
    QByteArray m_str;

    OperatorType m_type, m_lastType;
    qint16 m_lastPos;

    qint16 m_lineNumber;
    qint16 m_newLine;

    QList<quint8> m_charGroupDictionary;
};

}

#endif // TPLPARSER_H
