#include "StreamItem.h"
#include "VmLog.h"
#include "VmGlobal.h"

#include <QDataStream>
#include <Sc2000DataStream.h>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QTextCodec>
#include <PathManager.h>
#include <StDataGlobal.h>

using namespace StData;
using namespace std;

namespace StVirtualMachine {

StreamItem::StreamItem(qint16 handle) :
    HandleItem(handle),
    m_width(8)
{
    m_project = executedProject;
}

StreamItem::~StreamItem()
{}

int StreamItem::eof()
{
    return stream().device()->atEnd();
}

int StreamItem::status()
{
    return stream().status();
}

qint64 StreamItem::seek(qint64 pos)
{
    if(pos < 0)
        throw invalid_argument(QString("Ошибка позиционирования в потоке: Seek(%1)").arg(pos).toStdString());

    return stream().seek(pos);
}

qint64 StreamItem::size()
{
    return stream().device()->size();
}

QString StreamItem::read(int size)
{
//    return QString::fromLocal8Bit(stream().readBytes(size));
    return fromWindows1251(stream().readBytes(size));
}

int StreamItem::readFloat(double& value)
{
    int result = 1;
    switch(width())
    {
        case 0:
        case 1:
        {
            qint8 n;
            stream() >> n;
            value = n;
        }break;

        case 2:
        {
            quint16 n;
            stream() >> n;
            value = n;
        }break;

        case 3:
        {
            qint16 n;
            stream() >> n;
            value = n;
        }break;

        case 4:
        {
            quint32 n;
            stream() >> n;
            value = n;
        }break;

        case 5:
        {
            qint32 n;
            stream() >> n;
            value = n;
        }break;

        case 6:
        {
            float n;
            stream() >> n;
            value = n;
        }break;

        case 8:
            stream() >> value;
            break;

        case 9:
            long double n;
            stream().readRawData((char*)&n, 10);
            value = (double)n;
            break;

        default:
            result = 0;
            break;
    }
    return result;
}

QString StreamItem::readLine()
{
    QString str = getLine();
    if(!str.isEmpty())
    {
        for(int i = 0; i < str.length(); i++)
        {
            char ch = str.at(i).toLatin1();
            if(ch == 10 || ch == 13)
                return str.left(i);
        }
    }
    return str;
}

QString StreamItem::readAll()
{
    int pos = stream().pos();
    stream().seek(0);
    QByteArray array = stream().readBytes(size());
    stream().seek(pos);

    return QString::fromLatin1(array);
}

QString StreamItem::getLine(int size, char separator)
{
//    QTextCodec* win1251Codec = QTextCodec::codecForName("windows-1251");

    qint64 pos = stream().device()->pos();

    QByteArray bytes = stream().readBytes(size);
    if(bytes.isEmpty())
        return "";

    for(int i = 0; i < bytes.count(); i++)
    {
        if(bytes.at(i) == separator)
        {
            stream().device()->seek(pos + i + 1);
            return fromWindows1251(bytes, i + 1);
//            return QString::fromLocal8Bit(bytes, i + 1);
        }
    }
    stream().device()->seek(pos + bytes.count());

    return fromWindows1251(bytes);
//    return QString::fromLocal8Bit(bytes);
}

bool StreamItem::writeFloat(double value)
{
    int result = 1;
    switch(width())
    {
        case 0:
        {
            quint8 n = value;
            //stream().writeRawData((char*)&n, 1);
            stream() << n;
        }break;

        case 1:
        {
            qint8 n = value;
            stream() << n;
        }break;

        case 2:
        {
            quint16 n = value;
            stream() << n;
        }break;

        case 3:
        {
            qint16 n = value;
            stream() << n;
        }break;

        case 4:
        {
            quint32 n = value;
            stream() << n;
        }break;

        case 5:
        {
            qint32 n = value;
            stream() << n;
        }break;

        case 6:
        {
            float n = value;
            stream() << n;
        }break;

        case 8:
            stream() << value;
            break;

        case 9:
        {
            long double n = value;
            stream().writeRawData((char*)&n, 10);
        }break;

        default:
            result = 0;
            break;
    }

    flush();

    result = (stream().status() == QDataStream::Ok);
    return result;
}

bool StreamItem::writeLn(const QString& str)
{
    if(stream().status() != QDataStream::Ok)
        return false;

//    QByteArray bytes = str.toLocal8Bit();
    QByteArray bytes = toWindows1251(str);
    stream().writeRawData(bytes, bytes.count());
    stream().writeInt16(0x0a0d);

    return (stream().status() == QDataStream::Ok);
}

bool StreamItem::write(const QString& str, int count)
{
    if(stream().status() != QDataStream::Ok)
        return false;

//    QByteArray bytes = str.toLocal8Bit();
    QByteArray bytes = toWindows1251(str);
    stream().writeRawData(bytes, count == -1 ? bytes.count() : count);

    bool result = (stream().status() == QDataStream::Ok);
    return result;
}

qint64 StreamItem::pos()
{
    return stream().pos();
}

void StreamItem::close()
{
    stream().device()->close();
}





/*---------------------------------------------------------------------------------*/
MemoryStreamItem::MemoryStreamItem(const QString openModeStr) :
    StreamItem(),
    m_openMode(0)
{
    if(openModeStr.contains("CREATE", Qt::CaseInsensitive))
    {
        m_byteArray.clear();
        m_openMode = QIODevice::ReadWrite | QIODevice::Truncate;
    }
    else
    {
        if(openModeStr.contains("READONLY", Qt::CaseInsensitive))
            m_openMode = QIODevice::ReadOnly;

        else if(openModeStr.contains("WRITEONLY", Qt::CaseInsensitive))
            m_openMode = QIODevice::WriteOnly;

        else
            m_openMode = QIODevice::ReadWrite;
    }

    m_buffer.setBuffer(&m_byteArray);
    m_buffer.open(m_openMode);
    m_stream.setDevice(&m_buffer);
}

MemoryStreamItem::~MemoryStreamItem()
{
    m_buffer.close();
}

int MemoryStreamItem::trunc()
{
    bool result = true;
    int pos = m_stream.pos();
    {
        m_buffer.close();
        m_byteArray = m_byteArray.left(pos);
        result &= m_buffer.open(m_openMode);
        result &= m_stream.seek(pos);
        m_buffer.open(m_openMode);
    }
    return result;
}

void MemoryStreamItem::flush()
{}

/*---------------------------------------------------------------------------------*/
FileStreamItem::FileStreamItem(const QDir& projectDir, const QString& fileName, const QString& additionalInfo) :
    StreamItem()
{
    QString filePath = PathManager::absolutePath(projectDir, fileName);
//    QString filePath = fileName;
//    if(QFileInfo(fileName).isRelative())
//        filePath = projectDir.absoluteFilePath(fileName);

    if(QFileInfo(filePath).isDir())
        throw runtime_error(QString(QObject::tr("Ошибка создания потока ввода-вывода. Путь '%1' является директорией")
                                    .arg(filePath)).toStdString());


    if(additionalInfo.contains("CREATE", Qt::CaseInsensitive))
    {
        if(m_file.exists())
            m_file.resize(0);
        m_openMode = QIODevice::ReadWrite | QIODevice::Truncate;
    }
    else
    {
        if(!QFile::exists(filePath))
            throw runtime_error(QString(QObject::tr("Ошибка создания потока. Файл не существует: '%1'"))
                                .arg(QDir::toNativeSeparators(filePath)).toStdString());

        if(additionalInfo.contains("READWRITE", Qt::CaseInsensitive))
            m_openMode = QIODevice::ReadWrite;

        else if(additionalInfo.contains("WRITEONLY", Qt::CaseInsensitive))
            m_openMode = QIODevice::WriteOnly;

        else
            m_openMode = QIODevice::ReadOnly;
    }

    m_file.setFileName(filePath);
    if(m_file.open(m_openMode))
        m_stream.setDevice(&m_file);
    else
        throwLogicError(QString(QObject::tr("Ошибка создания потока. Не удалось открыть файл '%1' в режиме %2"))
                        .arg(QDir::toNativeSeparators(filePath))
                        .arg(additionalInfo));
    seek(0);
}


FileStreamItem::~FileStreamItem()
{
    m_file.close();
}

int FileStreamItem::trunc()
{
    bool result = false;

    qint64 pos = m_stream.pos();
    m_file.close();

    QByteArray bytes;
    if(m_file.open(QIODevice::ReadOnly))
    {
        bytes = stream().readBytes(pos);
        m_file.close();
        if(m_file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            stream().writeRawData(bytes.data(), bytes.length());
            result = stream().status() == QDataStream::Ok;
            m_file.close();
        }
    }
    else
        result = false;

    result &= m_file.open(m_openMode);
    m_file.seek(pos);
    return result;
}

void FileStreamItem::flush()
{
    m_file.flush();
}

void FileStreamItem::throwLogicError(const QString& errorText)
{
    throw std::logic_error(errorText.toStdString());
}

}
