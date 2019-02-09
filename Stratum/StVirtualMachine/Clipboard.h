#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QByteArray>

namespace StVirtualMachine {

class Clipboard
{
public:
    static QByteArray getData() {return m_data; }
    static void setData(const QByteArray& data) { m_data = data; }


private:
    Clipboard();

private:
    static QByteArray m_data;
};

}

#endif // CLIPBOARD_H
