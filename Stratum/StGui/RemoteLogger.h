#ifndef REMOTELOGGER_H
#define REMOTELOGGER_H

#include <QNetworkAccessManager>

class RemoteLogger
{
public:
    static void handleMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
    RemoteLogger() {}
    static QString getTypeName(QtMsgType type);

private:
    static QNetworkAccessManager m_networkAccessManager;
};

#endif // REMOTELOGGER_H
