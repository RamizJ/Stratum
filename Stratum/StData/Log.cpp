#include "Log.h"

#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <memory>

namespace StData {

Log::Log(QObject* parent) :
    QObject(parent)
{
    m_textForType[MessageType::MT_Info] = "Info";
    m_textForType[MessageType::MT_Warning] = "Warning";
    m_textForType[MessageType::MT_Error] = "Error";
}

QString Log::info(QString message)
{
    return writeMessage(message, MessageType::MT_Info);
}

QString Log::warning(QString message)
{
    return writeMessage(message, MessageType::MT_Warning);
}

QString Log::error(QString message)
{
    return writeMessage(message, MessageType::MT_Error);
}

QString Log::messageFormat()
{
    return QString("[%1][%2]: %3").arg(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz"));
}

QString Log::textForType(MessageType msgType)
{
    return m_textForType.value(msgType);
}

QString Log::writeMessage(QString message, const MessageType& msgType)
{
    QString formatedMessage = messageFormat().arg(textForType(msgType)).arg(message);
    emit messageWrited(formatedMessage, msgType);
    return formatedMessage;
}

/*--------------------------------------------------------------------------------------------------------*/
SystemLog& SystemLog::instance()
{
    static SystemLog log;
    return log;
}

QString SystemLog::fileName() const
{
    //TODO поиск локации при невозможности сохранения в текущей (например в AppData)
    return "Log.txt";
}

void SystemLog::info(QString message, bool fileWritingEnabled)
{
    QString formattedMessage = m_log->info(message);
    if(fileWritingEnabled)
        writeToFile(formattedMessage);
}

void SystemLog::warning(QString message, bool fileWritingEnabled)
{
    QString formattedMessage = m_log->warning(message);
    if(fileWritingEnabled)
        writeToFile(formattedMessage);

}

void SystemLog::error(QString message, bool fileWritingEnabled)
{
    QString formattedMessage = m_log->error(message);
    if(fileWritingEnabled)
        writeToFile(formattedMessage);
}

SystemLog::SystemLog(QObject* parent) :
    QObject(parent),
    m_maxFileSize(5*1024*1024)
{
    m_log = new Log(this);
    connect(m_log, &Log::messageWrited, this, &SystemLog::messageWrited);
}

void SystemLog::writeToFile(const QString message)
{
    QFile file(fileName());
    if (file.open(QFile::Append | QFile::Text))
    {
        QTextStream textStream(&file);
        textStream << message;
        endl(textStream);
        file.close();

        //TODO подумать как лучше сделать, например создавать новый файл с уникальным именем или переимеовывать старый
        //удаление старого файла при превышении максимально заданного размера, создание нового при следующем сообщении
        if (file.size() > m_maxFileSize)
            file.remove();
    }
//    else
//        qCritical() << m_log->messageFormat()
//                       .arg(m_log->textForType(MessageType::Warning))
//                       .arg("log file not opened");
}

}
