#ifndef SETTINGS_H
#define SETTINGS_H

#include "stdata_global.h"

#include <QSettings>
#include <QSizeF>

namespace StData {

class STDATASHARED_EXPORT Settings
{
public:
    static Settings& instance();

    bool isLogEnabled() const;
    void setLogEnabled(bool isLogEnabled);

    QString logFilePath() const;
    void setLogFilePath(const QString& logFdilePath);

    QString lastProjectPath() const;
    void setLastProjectPath(const QString path);

    QSize mainWindowSize() const {return m_mainWindowSize;}
    void setMainWindowSize(const QSize& size) { m_mainWindowSize = size; }

private:
    Settings();

private:
    QSettings m_settings;
    QSize m_mainWindowSize;
};

}

#endif // SETTINGS_H
