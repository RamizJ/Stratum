#ifndef STREAMITEM_H
#define STREAMITEM_H

#include <HandleItem.h>

#include <memory>
#include <QBuffer>
#include <QDir>
#include <QTextStream>
#include <Sc2000DataStream.h>

namespace StData {
class Project;
class St2000DataStream;
}

namespace StVirtualMachine {

class StreamItem : public StData::HandleItem
{
public:
    explicit StreamItem(qint16 handle = 0);
    virtual ~StreamItem();

    int width() const {return m_width;}
    void setWidth(int width) { m_width = width; }

    StData::Project* project() const { return m_project; }
    void setProject(StData::Project* project) { m_project = project; }

public:
    virtual StData::St2000DataStream& stream() = 0;
    virtual int trunc() = 0;

    virtual int eof();
    virtual int status();
    virtual qint64 seek(qint64 pos);
    virtual qint64 pos();
    virtual qint64 size();

    virtual QString read(int size);
    virtual int readFloat(double& value);
    virtual QString readLine();
    virtual QString readAll();
    virtual QString getLine(int size = 255, char separator = '\n');

    virtual bool write(const QString& str, int count = - 1);
    virtual bool writeFloat(double value);
    virtual bool writeLn(const QString& str);

    virtual void close();

protected:
    virtual void flush() = 0;

private:
    int m_width;
    StData::Project* m_project;
};

class MemoryStreamItem : public StreamItem
{
public:
    MemoryStreamItem(const QString openModeStr);
    ~MemoryStreamItem();

    // StreamItem interface
public:
    StData::St2000DataStream& stream() { return m_stream; }
    int trunc();

protected:
    void flush();

private:
    QBuffer m_buffer;
    QByteArray m_byteArray;
    StData::St2000DataStream m_stream;    
    QIODevice::OpenMode m_openMode;
};

class FileStreamItem : public StreamItem
{
public:
    FileStreamItem(const QDir& projectDir, const QString& fileName, const QString& additionalInfo);
    ~FileStreamItem();

public:
    StData::St2000DataStream& stream() { return m_stream; }
    int trunc();

protected:
    void flush();

private:
    void throwLogicError(const QString& errorText);

private:
    QFile m_file;
    StData::St2000DataStream m_stream;
    QIODevice::OpenMode m_openMode;
};

}

#endif // STREAMITEM_H

