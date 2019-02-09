#include "QtMsgFileLogger.h"

#include <Core.h>
#include <QFile>
#include <QTextStream>
#include <Settings.h>

using namespace StData;

void QtMsgFileLogger::handleMessage(QtMsgType type, const QMessageLogContext& /*context*/, const QString& msg)
{
    QFile logFile(Settings::instance().logFilePath());
    if(logFile.open(QIODevice::Append))
    {
        QTextStream stream(&logFile);
        stream << getTypeName(type) << ": " << msg << endl;

        logFile.flush();
    }
}

QString QtMsgFileLogger::getTypeName(QtMsgType type)
{
    switch (type)
    {
        case QtDebugMsg: return "Debug";
        case QtWarningMsg: return "Warning";
        case QtCriticalMsg: return "Critical";
        case QtFatalMsg: return "Fatal";
        case QtInfoMsg: return "Info";
    }
    return "Undefined";
}
