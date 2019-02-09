#ifndef STRINGADAPTER_H
#define STRINGADAPTER_H

#include <QTextCodec>

namespace StData {

class StringAdapter
{
public:
    QString toUnicode(const QByteArray& data);

private:
    StringAdapter();

private:
    static QTextCodec* m_textCodec;
};

}

#endif // STRINGADAPTER_H
