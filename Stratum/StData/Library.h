#ifndef LIBRARY_H
#define LIBRARY_H

#include "stdata_global.h"

#include <QFileInfo>
#include <QDir>

namespace StData {

class Class;
class Project;

class STDATASHARED_EXPORT Library
{
public:
    Library() = default;
    virtual ~Library();

    QFileInfo fileInfo() const {return m_fileInfo;}

    QList<Class*> classList() const {return m_classList;}
    void addClass(Class* classInfo);
    void removeClass(Class* cls);

    QString absolutePath() {return m_fileInfo.absoluteFilePath();}
    QDir libraryDir() {return m_libraryDir;}

    virtual QString relativePath() = 0;
    virtual void setPath(QString path);

    QString originalPath() {return m_originalPath;}

    void load(bool classNameOnly = true);

//signals:
//    void classAdded(Class* cl);
//    void classRemoved(const QString& className);

protected:
    void setFileInfo(const QFileInfo& fileInfo);
    void setOriginalPath(const QString& originalPath) {m_originalPath = originalPath;}

private:
    QString m_originalPath;
    QFileInfo m_fileInfo;
    QDir m_libraryDir;

    QList<Class*> m_classList;
};

class STDATASHARED_EXPORT SystemLibrary : public Library
{
public:
    SystemLibrary() = default;

    QString relativePath();

public:
    static QList<SystemLibrary*> loadLibraries();

private:
    static QList<SystemLibrary*> loadSystemLibrary(const QFileInfo& libFileInfo);
};

class STDATASHARED_EXPORT ProjectLibrary : public Library
{
public:
    explicit ProjectLibrary(Project* project);

    QString relativePath() {return "";}

private:
    virtual void setPath(QString path) {Library::setPath(path);}

private:
    Project* m_project;
};

class STDATASHARED_EXPORT ExtraLibrary : public Library
{
public:
    explicit ExtraLibrary(Project* project);

    QString relativePath();
    void setPath(QString path);

    static QList<ExtraLibrary*> loadLibraries(Project* project,
                                              const QString& path,
                                              bool isRecursiveLoading);

private:
    Project* m_project;
};

}

#endif // LIBRARY_H
