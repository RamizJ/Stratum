#ifndef QTMSGFILELOGGER_H
#define QTMSGFILELOGGER_H

#include <qlogging.h>

class QtMsgFileLogger
{
public:
    static void handleMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
    QtMsgFileLogger() {}
    static QString getTypeName(QtMsgType type);
};

#endif // QTMSGFILELOGGER_H
