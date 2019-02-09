#ifndef STREAMMANAGER_H
#define STREAMMANAGER_H

#include "ErrorProvider.h"
#include <QDataStream>
#include <QDir>
#include <QMap>
#include <HandleCollection.h>

namespace StData {
class Project;
}

namespace StVirtualMachine {

class StreamItem;
typedef StData::HandleCollection<StreamItem> StreamItemCollection;

class StreamManager : public QObject, public StData::ErrorHolder
{
    Q_OBJECT
public:
    enum StreamType {ST_NULL=1, ST_FILE, ST_MEMORY, ST_PIPE, ST_NET, ST_MIDI, ST_SNDFX, ST_SERIAL, ST_LPT};

public:
    explicit StreamManager(QObject* parent = 0);
    ~StreamManager();

    int createStream(const QString& device, const QString& name, const QString& flags);

    int status(int handle);

    qint32 seek(int handle, qint32 pos);
    qint64 pos(int handle);
    int eof(int handle);
    int getSize(int handle);
    int setWidth(int handle, int width);

    int readFloat(int handle, double& value);
    QString readLine(int handle);
    QString getLine(int handle, int size = 255, char separator = '\n');

    int writeFloat(int handle, double value);
    int writeLine(int handle, const QString& str);
    int copyBlock(int sourceHandle, int targetHandle, int from, int size);
    int trunc(int handle);

    int closeStream(int handle);

    StreamItem* getStreamItem(int handle);

    void clear(StData::Project* project);
    void clearAll();

private:
    StreamItemCollection m_streams;
    QMap<QString, StreamType> m_typeDictionary;
};

}

#endif // STREAMMANAGER_H
