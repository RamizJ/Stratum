#include "RemoteLogger.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>
#include <QNetworkRequest>
#include <QNetworkReply>

QNetworkAccessManager RemoteLogger::m_networkAccessManager;

void RemoteLogger::handleMessage(QtMsgType type, const QMessageLogContext& /*context*/, const QString& msg)
{
    QJsonObject obj;
    obj.insert("message", msg);
    obj.insert("type", getTypeName(type));
    QByteArray data = QJsonDocument(obj).toJson();

    QNetworkRequest req(QUrl("http://stratum.ac.ru/androidlog/writelog.php"));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    req.setHeader(QNetworkRequest::ContentLengthHeader,data.length());

    /*QNetworkReply *reply = */m_networkAccessManager.post(req, data);

//    QEventLoop loop;
//    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
//    loop.exec();
}

QString RemoteLogger::getTypeName(QtMsgType type)
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
