#include "StringAdapter.h"

#include <stdexcept>

namespace StData {

QTextCodec* StringAdapter::m_textCodec = QTextCodec::codecForName("windows-1251");

StringAdapter::StringAdapter()
{}

QString StringAdapter::toUnicode(const QByteArray& data)
{
    if(m_textCodec == nullptr)
        throw std::runtime_error("Cannot convert data to unicode. TextCodec not found");

    return m_textCodec->toUnicode(data);
}

}
