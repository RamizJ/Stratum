#ifndef LOG_H
#define LOG_H

#include "stdata_global.h"

#include <QObject>
#include <QMap>
#include <QFile>
#include <memory>

namespace StData {

enum MessageType {MT_Info, MT_Warning, MT_Error};

class STDATASHARED_EXPORT Log : public QObject
{
    Q_OBJECT
public:
//    enum MessageType {Info, Warning, Error};

public:
    explicit Log(QObject* parent = nullptr);

    virtual QString info(QString message);
    virtual QString warning(QString message);
    virtual QString error(QString message);

    virtual QString messageFormat();
    QString textForType(MessageType msgType);

signals:
    void errorMessageWrited(const QString& message);
    void warnigMessageWrited(const QString& message);
    void infoMessageWrited(const QString& message);

    void messageWrited(const QString& formattedMessage, const MessageType& msgType);

private:
    QString writeMessage(QString message, const MessageType& msgType);

private:
    QMap<MessageType, QString> m_textForType;
};

class STDATASHARED_EXPORT SystemLog : public QObject
{
    Q_OBJECT
public:
    QString fileName() const;

    virtual void info(QString message, bool fileWritingEnabled = false);
    virtual void warning(QString message, bool fileWritingEnabled = false);
    virtual void error(QString message, bool fileWritingEnabled = false);

public:
    static SystemLog& instance();

signals:
    void errorMessageWrited(const QString& message);
    void warnigMessageWrited(const QString& message);
    void infoMessageWrited(const QString& message);

    void messageWrited(const QString& formattedMessage, const MessageType& msgType);

private:
    explicit SystemLog(QObject* parent = nullptr);
    void writeToFile(const QString message);
	
private:
    QString m_fileName;
    qint64 m_maxFileSize;

    Log* m_log;
};

class STDATASHARED_EXPORT LogMessage
{
public:
    LogMessage(QString msg, MessageType msgType) :
        m_message(msg),
        m_messageType(msgType)
    {}

    MessageType messageType() const {return m_messageType; }
    void setMessageType(const MessageType& messageType) {m_messageType = messageType;}

    QString message() const {return m_message;}
    void setMessage(const QString& value) {m_message = value;}

private:
    QString m_message;
    MessageType m_messageType;
};


}
#endif // LOG_H
