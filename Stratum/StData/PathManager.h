#ifndef PATHMANAGER_H
#define PATHMANAGER_H

#include "stdata_global.h"

#include <QDir>
#include <QString>

class STDATASHARED_EXPORT PathManager
{
public:
    ~PathManager();

    QString appPath() const { return m_appPath; }
    QString libPath() const { return m_libPath; }
    QString templatePath() const { return m_templatePath; }
    QString iconsLibPath() const { return m_iconsLibPath; }

    QDir appDir() const { return m_appDir; }
    QDir libDir() const { return m_libDir; }
    QDir templateDir() const { return m_templateDir; }
    QDir iconsLibDir() const { return m_iconsLibDir; }

public:
    static PathManager& instance();
    static QString fromWinOsSeparators(const QString& path);
    static QString absolutePath(const QDir& dir, QString relativePath);
    static QString cleanPath(const QString& path);


private:
    PathManager();

private:
    QString m_appPath;
    QString m_libPath;
    QString m_templatePath;
    QString m_iconsLibPath;

    QDir m_appDir;
    QDir m_libDir;
    QDir m_templateDir;
    QDir m_iconsLibDir;
};

#endif // PATHMANAGER_H
