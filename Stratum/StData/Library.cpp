#include "Library.h"

#include "Project.h"
#include "Class.h"
#include "PathManager.h"

#include <QDebug>

namespace StData {

Library::~Library()
{
    qDeleteAll(m_classList);
//    for(Class* classInfo : m_classList)
//        removeClass(classInfo);
}

void Library::addClass(Class* cls)
{
    if(cls != nullptr)
        m_classList.append(cls);
}

void Library::removeClass(Class* cls)
{
    m_classList.removeOne(cls);
    delete cls;
}

void Library::setPath(QString path)
{
    setOriginalPath(path);
    setFileInfo(QFileInfo(path));
}

void Library::load(bool classNameOnly)
{
    QFileInfoList classFileInfoList = libraryDir().entryInfoList(QDir::Files);
    for(QFileInfo classFileInfo : classFileInfoList)
    {
        if(classFileInfo.suffix().compare("cls", Qt::CaseInsensitive) != 0)
            continue;

        Class* cl = new Class(classFileInfo, this);

        if(classNameOnly)
            cl->loadName();
        else
            cl->load();
        addClass(cl);
    }
}

void Library::setFileInfo(const QFileInfo& fileInfo)
{
    m_fileInfo = fileInfo;
    m_libraryDir = QDir(m_fileInfo.absoluteFilePath());
}

QList<SystemLibrary*> SystemLibrary::loadLibraries()
{
    QList<SystemLibrary*> result;

    QDir sysLibDir = PathManager::instance().libDir();
    QFileInfoList subDirFileInfoList = sysLibDir.entryInfoList(QDir::Dirs);
    for(QFileInfo subDirFileInfo : subDirFileInfoList)
    {
        if(subDirFileInfo.fileName() == "." || subDirFileInfo.fileName() == "..")
            continue;

        result += loadSystemLibrary(subDirFileInfo);
    }
    return result;
}

QList<SystemLibrary*> SystemLibrary::loadSystemLibrary(const QFileInfo& libFileInfo)
{
    QList<SystemLibrary*> result;

    if(!libFileInfo.exists())
        return result;

    SystemLibrary* library = new SystemLibrary();
    library->setPath(libFileInfo.absoluteFilePath());
    result.append(library);
    library->load();

    QFileInfoList subDirFileInfoList = library->libraryDir().entryInfoList(QDir::Dirs);
    for(QFileInfo subDirFileInfo : subDirFileInfoList)
    {
        if(subDirFileInfo.fileName() == "." || subDirFileInfo.fileName() == "..")
            continue;

        result += loadSystemLibrary(subDirFileInfo);
    }

    return result;
}

QString SystemLibrary::relativePath()
{
    QString result = PathManager::instance().libDir().relativeFilePath(fileInfo().absoluteFilePath());
    return result;
}

ProjectLibrary::ProjectLibrary(Project* project) :
    m_project(project)
{
    setPath(m_project->fileInfo().absolutePath());
}

ExtraLibrary::ExtraLibrary(Project* project) :
    m_project(project)
{}

QString ExtraLibrary::relativePath()
{
    QString result;
    if(m_project)
        result = m_project->fileInfo().absoluteDir().relativeFilePath(fileInfo().absoluteFilePath());

    return result;
}

void ExtraLibrary::setPath(QString path)
{
    setOriginalPath(path);
    if(m_project)
    {
        if(!path.isEmpty() && !path.isEmpty() && path.at(0) == '\\')
            path.insert(0, '.');

        QDir projectDir = m_project->projectDir();
        path = PathManager::fromWinOsSeparators(path);

        QString cleanPath = QDir::cleanPath(projectDir.absoluteFilePath(path.simplified()));
        setFileInfo(QFileInfo(cleanPath));
    }
}

QList<ExtraLibrary*> ExtraLibrary::loadLibraries(Project* project, const QString& path, bool isRecursiveLoading)
{
    QList<ExtraLibrary*> result;

    ExtraLibrary* library = new ExtraLibrary(project);
    library->setPath(path);
    if(path != project->fileInfo().absolutePath())
    {
        library->load();
        result.append(library);
    }

    if(isRecursiveLoading)
    {
        QFileInfoList subDirs = library->libraryDir().entryInfoList(QDir::Dirs);
        for(QFileInfo subDir : subDirs)
        {
            if(subDir.fileName() == "." || subDir.fileName() == "..")
                continue;

            result += loadLibraries(project, subDir.absoluteFilePath(), isRecursiveLoading);
        }
    }

    return result;
}

}
