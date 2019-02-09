#include "CodePointer.h"

#include <Sc2000DataStream.h>
#include <StDataGlobal.h>

using namespace StData;

namespace StVirtualMachine {

CodePointer::CodePointer(QObject* parent) :
    QObject(parent),
    m_isNullCode(false)
//    m_nullCode(0),
//    m_codes(nullptr),
//    m_pointer(nullptr),
//    m_codesCount(0),
//    m_currentIndex(0)
{}

void CodePointer::setCodeBytes(const QByteArray& vmByteCode)
{
    m_byteCode = vmByteCode;
//    m_codesCount = vmByteCode.count() / 2;
//    m_currentIndex = 0;

    m_byteCodeStream = std::make_shared<St2000DataStream>(m_byteCode);
    m_isNullCode = false;

//    if(m_codesCount > 0)
//        m_pointer = m_codes = (qint16*)vmByteCode.data();
//    else
//        m_pointer = m_codes = nullptr;
}

void CodePointer::reset()
{
    if(!m_byteCode.isEmpty())
        m_byteCodeStream->device()->seek(0);
//    m_currentIndex = 0;
}

qint16 CodePointer::getCode(int offset)
{
    if(m_isNullCode)
        return 0;

    int pos = m_byteCodeStream->pos();

    m_byteCodeStream->seek(pos + offset);
    qint16 code = m_byteCodeStream->readInt16();
    m_byteCodeStream->seek(pos);

    return code;

//    if(m_currentIndex == -1)
//        return 0;
//    return *getPtr(offset);
}

qint16 CodePointer::toNextCode()
{
    return m_byteCodeStream->readInt16();
//    qint16 result = *getPtr();
//    m_currentIndex++;
//    return result;
}

int CodePointer::getPosition() const
{
    return m_byteCodeStream->pos() / 2;
//    return m_currentIndex;
}

void CodePointer::incPosition(int incValue)
{
    m_byteCodeStream->seek(m_byteCodeStream->pos() + incValue*2);
//    m_currentIndex += incValue;
}

void CodePointer::setPosition(int position)
{
    m_byteCodeStream->seek(position * 2);
//    m_currentIndex = position;
}

void CodePointer::setNullCode()
{
    m_isNullCode = true;
//    m_currentIndex = -1;
}

QString CodePointer::popString()
{
    int pos = m_byteCodeStream->pos();
//    QString str = QString::fromLocal8Bit(&m_byteCode.data()[pos + 2]);
    QString str = fromWindows1251(&m_byteCode.data()[pos + 2]);
    incPosition(getCode() + 1);

    return str;

//    return m_byteCodeStream->readString();
//    qint16* pointer = getPtr();
//    char* strData = (char*)(pointer + 1);
//    incPosition(*pointer + 1);
//    return QString::fromLocal8Bit(strData);
}

double CodePointer::getDouble()
{
    int pos = m_byteCodeStream->pos();
    double value = m_byteCodeStream->readDouble();
    m_byteCodeStream->seek(pos);

    return value;
}

int CodePointer::getInt32()
{
    int pos = m_byteCodeStream->pos();
    int value = m_byteCodeStream->readInt32();
    m_byteCodeStream->seek(pos);

    return value;
}

QString CodePointer::getString()
{
    int pos = m_byteCodeStream->pos();
//    return QString::fromLocal8Bit(&m_byteCode.data()[pos + 2]);
    return fromWindows1251(&m_byteCode.data()[pos + 2]);
}

//qint16* CodePointer::getPtr(int offset)
//{
//    return (qint16)(m_byteCode.data() + (m_byteCodeStream->pos() / 2));
////    qint16* ptr = (qint16*)(m_byteCode.data());
////    return (ptr + m_currentIndex + offset);
//}

}
