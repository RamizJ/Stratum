#include "Sc2000DataStream.h"
#include "StDataGlobal.h"

#include <QPointF>
#include <QSizeF>
#include <QVector>

namespace StData {

St2000DataStream::St2000DataStream() :
    QDataStream(),
    m_fileVersion(0)
{
    setByteOrder(LittleEndian);
}

St2000DataStream::St2000DataStream(QIODevice* device) : QDataStream(device)
{
    setByteOrder(LittleEndian);
}

St2000DataStream::St2000DataStream(QByteArray* byteArray, QIODevice::OpenMode flags) : QDataStream(byteArray, flags)
{
    setByteOrder(LittleEndian);
}

St2000DataStream::St2000DataStream(const QByteArray& byteArray) : QDataStream(byteArray)
{
    setByteOrder(LittleEndian);
}

qint64 St2000DataStream::pos() const
{
    return device()->pos();
}

bool St2000DataStream::seek(qint64 pos)
{
    return device()->seek(pos);
}

bool St2000DataStream::isOkStatus() const
{
    return (status() == QDataStream::Ok);
}

qint8 St2000DataStream::readInt8()
{
    qint8 result;
    *this >> result;
    return result;
}

qint16 St2000DataStream::readInt16()
{
    qint16 result;
    *this >> result;
    return result;
}

qint32 St2000DataStream::readInt32()
{
    qint32 result;
    *this >> result;
    return result;
}

quint8 St2000DataStream::readUint8()
{
    quint8 result;
    *this >> result;
    return result;
}

quint16 St2000DataStream::readUint16()
{
    quint16 result;
    *this >> result;
    return result;
}

quint32 St2000DataStream::readUint32()
{
    quint32 result;
    *this >> result;
    return result;
}

qreal St2000DataStream::readDouble()
{
    qreal result;
    *this >> result;
    return result;
}

QPointF St2000DataStream::readPointF()
{
    qreal x, y;
    *this >> x >> y;

    return QPointF(x, y);
}

QSizeF St2000DataStream::readSizeF()
{
    qreal w, h;
    *this >> w >> h;

    return QSizeF(w, h);
}

qreal St2000DataStream::readAngle()
{
    qint16 a;
    *this >> a;

    qreal angle = -a / 10.0;
    return angle;
}

QString St2000DataStream::readString()
{
    QString result;

    qint16 count = readInt16();
    if(count > 0)
    {
        char* buffer = new char[count];
        int bytesRead = (*this).readRawData(buffer, count);

        if(bytesRead == count)
        {
            result = StData::fromWindows1251(buffer, count);
//            result = QString::fromLocal8Bit(buffer, count);
        }

        delete buffer;
    }

    return result;
}

QString St2000DataStream::readString(qint16 count)
{
    QString result;

    int size = readInt16();
    size = qMin(size, count - 1);

    if(size > 0)
    {
        char* buffer = new char[size];
        int bytesRead = this->readRawData(buffer, size);

        if(bytesRead == size)
        {
            result = StData::fromWindows1251(buffer, size);
        }

        delete buffer;
    }
    return result;
}

void St2000DataStream::skipString()
{
    skipRawData(readInt16());
}

QByteArray St2000DataStream::readBytes()
{
    QByteArray result;

    qint16 bufferSize = readInt16();
    if(bufferSize > 0)
    {
        char* buffer = new char[bufferSize];
        int bytesRead = readRawData(buffer, bufferSize);

        if(bytesRead == bufferSize)
            result = QByteArray(buffer, bufferSize);

        delete buffer;
    }

    return result;
}

QByteArray St2000DataStream::readBytes(int count)
{
    QByteArray result;

    if(count > 0)
    {
        char* buffer = new char[count];
        int bytesRead = readRawData(buffer, count);

        if(bytesRead != -1)
            result = QByteArray(buffer, bytesRead);

        delete buffer;
    }

    return result;
}

void St2000DataStream::readFileVersion()
{
    m_fileVersion = readInt16();
}

void St2000DataStream::writeInt8(qint8 data)
{
    *this << data;
}

void St2000DataStream::writeInt16(qint16 data)
{
    *this << data;
}

void St2000DataStream::writeInt32(qint32 data)
{
    *this << data;
}

void St2000DataStream::writeDouble(double data)
{
    *this << data;
}

void St2000DataStream::writeString(const QString& str)
{
//    QByteArray data = str.toLocal8Bit();
    QByteArray data = toWindows1251(str);
    this->writeInt16(data.length());
    this->writeRawData(data.constData(), data.length());
}

}
