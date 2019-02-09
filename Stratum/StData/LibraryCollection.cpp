#include "LibraryCollection.h"

#include "Library.h"
#include "Class.h"

namespace StData {

QList<SystemLibrary*> LibraryCollection::m_systemLibraries;
QMap<QString, Class*> LibraryCollection::m_systemCache;
LibraryCollection* LibraryCollection::m_currectCollection;

LibraryCollection::LibraryCollection(const QMap<QString, Class*>& classCache) :
    QObject(),
    m_projectLibrary(nullptr)
{
    m_classCache = classCache;
}

LibraryCollection::~LibraryCollection()
{}

void LibraryCollection::loadSystemLibraries()
{
    m_systemLibraries = SystemLibrary::loadLibraries();

    for(SystemLibrary* sl : m_systemLibraries)
        for(Class* cl : sl->classList())
            m_systemCache[cl->cleanName()] = cl;
}

void LibraryCollection::unloadSystemLibraries()
{
    qDeleteAll(m_systemLibraries);
    m_systemLibraries.clear();
    m_systemCache.clear();
}

LibraryCollection* LibraryCollection::create()
{
    return new LibraryCollection(m_systemCache);
}

Class* LibraryCollection::getSystemClassByName(const QString& className)
{
    return m_systemCache.value(className.toLower(), nullptr);
}

LibraryCollection* LibraryCollection::currentCollection()
{
    return m_currectCollection;
}

void LibraryCollection::setCurrentCollection(LibraryCollection* libraryCollection)
{
    m_currectCollection = libraryCollection;
}

Class* LibraryCollection::getClassByName(const QString className)
{
    if(m_currectCollection)
        return m_currectCollection->classByName(className);

    return m_systemCache.value(className.toLower(), nullptr);
}

void LibraryCollection::addLibrary(Library* library)
{
    if(!m_libraryCache.contains(library->absolutePath()))
    {
        m_libraryCache[library->absolutePath()] = library;
        addClassesToCache(library);

        if(ExtraLibrary* extraLibrary = dynamic_cast<ExtraLibrary*>(library))
            m_extraLibraries.append(extraLibrary);

        else if(SystemLibrary* systemLibrary = dynamic_cast<SystemLibrary*>(library))
            m_systemLibraries.append(systemLibrary);

        else if(ProjectLibrary* projectLibrary = dynamic_cast<ProjectLibrary*>(library))
            m_projectLibrary = projectLibrary;
    }
}

void LibraryCollection::addLibraries(QList<Library*> libraries)
{
    for(Library* library : libraries)
        addLibrary(library);
}

void LibraryCollection::addLibraries(QList<ExtraLibrary*> libraries)
{
    for(ExtraLibrary* library : libraries)
        addLibrary(library);
}

void LibraryCollection::addLibraries(QList<SystemLibrary*> libraries)
{
    for(SystemLibrary* library : libraries)
        addLibrary(library);
}

void LibraryCollection::removeLibrary(Library* library)
{
    if(m_libraryCache.contains(library->absolutePath()))
    {
        m_libraryCache.remove(library->absolutePath());

        for(Class* cls : library->classList())
            m_classCache.remove(cls->cleanName());
    }
}

void LibraryCollection::removeProjectLibraries()
{
    for(int i = m_libraryCache.count() - 1; i >= 0; i--)
    {
        Library* lib = m_libraryCache.values().at(i);
        if(dynamic_cast<ExtraLibrary*>(lib) != nullptr ||
           dynamic_cast<ProjectLibrary*>(lib) != nullptr)
        {
            removeLibrary(lib);
        }
    }
}

Class* LibraryCollection::classByName(const QString& className)
{
    Class* cls = m_classCache.value(className.toLower(), nullptr);
    if(cls && !cls->isLoaded())
        cls->load();

    return cls;
}

void LibraryCollection::clearDuplicated()
{
    qDeleteAll(m_duplicatedClasses);
    m_duplicatedClasses.clear();
}

//LibraryCollection& LibraryCollection::instance()
//{
//    static LibraryCollection libCollection;
//    return libCollection;
//}

void LibraryCollection::addClassesToCache(Library* library)
{
    for(Class* cls : library->classList())
    {
        QString cleanName = cls->cleanName();
        if(!m_classCache.contains(cleanName))
            m_classCache[cleanName] = cls;
        else
            m_duplicatedClasses << cls;
    }
}

}
