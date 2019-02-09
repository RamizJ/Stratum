#include "Settings.h"

#ifdef Q_OS_ANDROID
    #define SETTINGS_PATH "assets:/Settings.ini"
#else
    #define SETTINGS_PATH "Settings.ini"
#endif

namespace StData {

#define SETTIGS_LOG_ENABLED "LogEnabled"
#define SETTIGS_LAST_PROJECT_PATH "LastProjectPath"
#define SETTIGS_LOG_FILE_PATH "LogFilePath"
#define SETTIGS_IS_AUTOSIZE_WINDOW "IsAutoSizeWindow"

Settings& Settings::instance()
{
    static Settings s;
    return s;
}

Settings::Settings() :
    m_settings(SETTINGS_PATH, QSettings::IniFormat),
    m_mainWindowSize(0,0)
{
    if(!m_settings.contains(SETTIGS_LOG_FILE_PATH))
        m_settings.setValue(SETTIGS_LOG_FILE_PATH, "Log.txt");

    if(!m_settings.contains(SETTIGS_IS_AUTOSIZE_WINDOW))
        m_settings.setValue(SETTIGS_IS_AUTOSIZE_WINDOW, false);

#ifdef _DEBUG
        m_settings.setValue(SETTIGS_LOG_ENABLED, true);
#else
    if(!m_settings.contains(SETTIGS_LOG_ENABLED))
    {
        m_settings.setValue(SETTIGS_LOG_ENABLED, false);

    }
#endif
}

bool Settings::isLogEnabled() const
{
    return m_settings.value(SETTIGS_LOG_ENABLED).toBool();
}

void Settings::setLogEnabled(bool isLogEnabled)
{
    m_settings.setValue(SETTIGS_LOG_ENABLED, isLogEnabled);
}

QString Settings::logFilePath() const
{
    return m_settings.value(SETTIGS_LOG_FILE_PATH).toString();
}

void Settings::setLogFilePath(const QString& logFdilePath)
{
    m_settings.setValue(SETTIGS_LOG_FILE_PATH, logFdilePath);
}

QString Settings::lastProjectPath() const
{
    return m_settings.value(SETTIGS_LAST_PROJECT_PATH).toString();
}

void Settings::setLastProjectPath(const QString path)
{
    m_settings.setValue(SETTIGS_LAST_PROJECT_PATH, path);
}

}
