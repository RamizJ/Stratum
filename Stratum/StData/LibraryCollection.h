#ifndef LIBRARYMANAGER_H
#define LIBRARYMANAGER_H

#include "stdata_global.h"

#include <QObject>
#include <QMap>

namespace StData {

class Library;
class ExtraLibrary;
class SystemLibrary;
class ProjectLibrary;

class Class;
class LibraryCollection;

class STDATASHARED_EXPORT LibraryCollection : public QObject
{
    Q_OBJECT
public:
    ~LibraryCollection();
    static void loadSystemLibraries();
    static void unloadSystemLibraries();
    static LibraryCollection* create();

    static QList<SystemLibrary*> systemLibraries() { return m_systemLibraries; }
    static Class* getSystemClassByName(const QString& className);
    static LibraryCollection* currentCollection();
    static void setCurrentCollection(LibraryCollection* libraryCollection);
    static Class* getClassByName(const QString className);
//    static LibraryCollection& instance();

    QList<ExtraLibrary*> extraLibraries() const { return m_extraLibraries; }
    ProjectLibrary* projectLibrary() const { return m_projectLibrary; }

    void addLibrary(Library* libraries);
//    void addProjectLibrary(ProjectLibrary* library);

    void addLibraries(QList<Library*> libraries);
    void addLibraries(QList<ExtraLibrary*> libraries);
    void addLibraries(QList<SystemLibrary*> libraries);

    void removeLibrary(Library* libraries);
    void removeProjectLibraries();

    QList<Library*> libraries() {return m_libraryCache.values();}
    Class* classByName(const QString& className);

    void clearDuplicated();

private:
    LibraryCollection(const QMap<QString, Class*>& classCache);
    void addClassesToCache(Library* library);

private:
    static LibraryCollection* m_currectCollection;
    QMap<QString, Library*> m_libraryCache;

    static QList<SystemLibrary*> m_systemLibraries;
    QList<ExtraLibrary*> m_extraLibraries;
    ProjectLibrary* m_projectLibrary;

    //TODO заменить на сортируемую коллекцию
    QMap<QString, Class*> m_classCache;
    static QMap<QString, Class*> m_systemCache;

    QList<Class*> m_duplicatedClasses;
};

}

#endif // LIBRARYMANAGER_H
