#ifndef SC2000DATASTREAM_H
#define SC2000DATASTREAM_H

#include "stdata_global.h"

#include <QDataStream>
#include <qglobal.h>

namespace StData {

class STDATASHARED_EXPORT St2000DataStream : public QDataStream
{
public:
    St2000DataStream();
    explicit St2000DataStream(QIODevice* device);
    St2000DataStream(QByteArray* byteArray, QIODevice::OpenMode flags);
    St2000DataStream(const QByteArray& byteArray);

    qint64 pos() const;
    bool seek(qint64 pos);
    bool isOkStatus() const;

    qint8 readInt8();
    qint16 readInt16();
    qint32 readInt32();

    quint8 readUint8();
    quint16 readUint16();
    quint32 readUint32();

    qreal readDouble();

    QPointF readPointF();
    QSizeF readSizeF();

    qreal readAngle();

    QString readString();
    QString readString(qint16 count);
    void skipString();

    QByteArray readBytes();
    QByteArray readBytes(int count);

    qint16 fileVersion() const {return m_fileVersion;}
    void readFileVersion();

    void writeInt8(qint8 data);
    void writeInt16(qint16 data);
    void writeInt32(qint32 data);
    void writeDouble(double data);
    void writeString(const QString& str);

private:
    qint16 m_fileVersion;
};

}

#endif // SC2000DATASTREAM_H
