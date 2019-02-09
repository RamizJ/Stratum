#include "StreamItem.h"
#include "StreamManager.h"
#include "HandleCollection.h"
#include "VmGlobal.h"

#include <StDataGlobal.h>

#include <QDir>

using namespace StData;
using namespace std;

namespace StVirtualMachine {

StreamManager::StreamManager(QObject* parent) :
    QObject(parent)
{
    m_typeDictionary["NULL"] = StreamType::ST_FILE;
    m_typeDictionary["FILE"] = StreamType::ST_FILE;
    m_typeDictionary["MEMORY"] = StreamType::ST_MEMORY;
    m_typeDictionary["PIPE"] = StreamType::ST_PIPE;
    m_typeDictionary["NET"] = StreamType::ST_NET;
    m_typeDictionary["MIDI"] = StreamType::ST_MIDI;
    m_typeDictionary["SNDFX"] = StreamType::ST_SNDFX;
    m_typeDictionary["SERIAL"] = StreamType::ST_SERIAL;
    m_typeDictionary["LPT"] = StreamType::ST_LPT;
}

StreamManager::~StreamManager()
{
    m_streams.deleteAll();
}

int StreamManager::status(int handle)
{
    StreamItem* streamItem = m_streams.getItem(handle);
    if(streamItem != nullptr)
        return streamItem->status();

    return 0;
}

int StreamManager::createStream(const QString& device, const QString& name, const QString& flags)
{
    int handle = 0;
    QString streamTypeName = device.toUpper();
    if(m_typeDictionary.contains(streamTypeName))
    {
        StreamItem* streamItem = nullptr;
        StreamType streamType = m_typeDictionary.value(streamTypeName);
        switch(streamType)
        {
            case StreamManager::ST_NULL:
            case StreamManager::ST_PIPE:
            case StreamManager::ST_NET:
            case StreamManager::ST_MIDI:
            case StreamManager::ST_SNDFX:
            case StreamManager::ST_LPT:
                break;

            case StreamManager::ST_FILE:
                streamItem = new FileStreamItem(executedProject->projectDir(), name, flags);
                break;

            case StreamManager::ST_MEMORY:
                streamItem = new MemoryStreamItem(flags);
                break;

            case StreamManager::ST_SERIAL:
                break;
        }

        if(streamItem)
            handle = m_streams.insert(streamItem);
    }
    else
        throw invalid_argument(QString(tr("Ошибка создания потока. Некорректный флаг '%1'"))
                               .arg(streamTypeName).toStdString());

    return handle;
}

qint32 StreamManager::seek(int handle, qint32 pos)
{
    StreamItem* streamItem = m_streams.getItem(handle);
    if(streamItem != nullptr)
        return streamItem->seek(pos);

    return false;
}

qint64 StreamManager::pos(int handle)
{
    StreamItem* streamItem = m_streams.getItem(handle);
    if(streamItem != nullptr)
        return streamItem->pos();

    return 0;
}

int StreamManager::eof(int handle)
{
    StreamItem* streamItem = m_streams.getItem(handle);
    if(streamItem != nullptr)
        return streamItem->eof();

    return 0;
}

int StreamManager::getSize(int handle)
{
    StreamItem* streamItem = m_streams.getItem(handle);
    if(streamItem != nullptr)
        return streamItem->size();

    return 0;
}

int StreamManager::setWidth(int handle, int width)
{
    StreamItem* streamItem = m_streams.getItem(handle);
    if(streamItem != nullptr)
    {
        streamItem->setWidth(width);
        return 1;
    }

    return 0;
}

int StreamManager::readFloat(int handle, double& value)
{
    StreamItem* streamItem = m_streams.getItem(handle);
    if(streamItem != nullptr)
        return streamItem->readFloat(value);

    return 0;
}

QString StreamManager::readLine(int handle)
{
    StreamItem* streamItem = m_streams.getItem(handle);
    if(streamItem != nullptr)
        return streamItem->readLine();

    return "";
}

QString StreamManager::getLine(int handle, int size, char separator)
{
    StreamItem* streamItem = m_streams.getItem(handle);
    if(streamItem != nullptr)
        return streamItem->getLine(size, separator);

    return "";
}

int StreamManager::writeFloat(int handle, double value)
{
    StreamItem* streamItem = m_streams.getItem(handle);
    if(streamItem != nullptr)
        return streamItem->writeFloat(value);

    return 0;
}

int StreamManager::writeLine(int handle, const QString& str)
{
    StreamItem* streamItem = m_streams.getItem(handle);
    if(streamItem != nullptr)
        return streamItem->writeLn(str);

    return 0;
}

int StreamManager::copyBlock(int sourceHandle, int targetHandle, int from, int size)
{
    StreamItem* srcStreamItem = m_streams.getItem(sourceHandle);
    StreamItem* trgStreamItem = m_streams.getItem(targetHandle);

    if(srcStreamItem != nullptr && trgStreamItem != nullptr)
    {
        int lastPos = srcStreamItem->pos();
        srcStreamItem->seek(from);
        QString str = srcStreamItem->read(size);
        srcStreamItem->seek(lastPos);

//        QByteArray blockBytes = str.toLocal8Bit();
        QByteArray blockBytes = toWindows1251(str);
        return trgStreamItem->stream().writeRawData(blockBytes, blockBytes.length());
    }
    return 0;
}

int StreamManager::trunc(int handle)
{
    StreamItem* streamItem = m_streams.getItem(handle);
    if(streamItem != nullptr)
        return streamItem->trunc();

    return false;
}

int StreamManager::closeStream(int handle)
{
    StreamItem* streamItem = m_streams.take(handle);
    if(streamItem != nullptr)
    {
        int handle = streamItem->handle();
        delete streamItem;
        return handle;
    }
    return 0;
}

StreamItem* StreamManager::getStreamItem(int handle)
{
    return m_streams.getItem(handle);
}

void StreamManager::clear(Project* project)
{
    QList<StreamItem*> items = m_streams.items();

    for(StreamItem* streamItem : items)
        if(streamItem->project() == project || streamItem->project() == nullptr)
            m_streams.deleteItem(streamItem->handle());
}

void StreamManager::clearAll()
{
    m_streams.deleteAll();
}

}
