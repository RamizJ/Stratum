#include "PathManager.h"

#include <QCoreApplication>
#include <QDir>

PathManager::PathManager()
{
    m_appPath = QCoreApplication::applicationDirPath();
    m_appDir = QDir(m_appPath);

#ifdef Q_OS_ANDROID
    m_libPath = "assets:/library";
    m_templatePath = "assets:/template";
    m_iconsLibPath = "assets:/icons";
#else
    m_libPath = m_appDir.absoluteFilePath("library");
    m_templatePath = m_appDir.absoluteFilePath("template");
    m_iconsLibPath = m_appDir.absoluteFilePath("icons");
#endif

    m_libDir = QDir(m_libPath);
    m_templateDir = QDir(m_templatePath);
    m_iconsLibDir = QDir(m_iconsLibPath);
}

PathManager::~PathManager()
{}

PathManager& PathManager::instance()
{
    static PathManager pathManager;
    return pathManager;
}

QString PathManager::fromWinOsSeparators(const QString& path)
{
    QString result = path;
    result = result.replace("\\\\", "/");
    result = result.replace("\\", "/");
    return result;
}

QString PathManager::absolutePath(const QDir& dir, QString relativePath)
{
    if(relativePath.startsWith('\\'))
        relativePath = relativePath.remove(0, 1);

    QString path = fromWinOsSeparators(relativePath);
    QString absoluteFilePath = dir.absoluteFilePath(path);
    return QDir::cleanPath(absoluteFilePath);
}

QString PathManager::cleanPath(const QString &path)
{
    QString result = fromWinOsSeparators(path);
    return QDir::cleanPath(result);
}
