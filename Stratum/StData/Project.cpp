#include "Project.h"
#include "VarManager.h"
#include "Library.h"
#include "Log.h"
#include "Sc2000DataStream.h"
#include "Object.h"
#include "Class.h"
#include "ObjectInfo.h"
#include "ObjectStateLoader.h"
#include "VarManagerLoader.h"
#include "Log.h"
#include "LibraryCollection.h"
#include "ObjectVarsLinker.h"

#include <QFileInfo>
#include <QDebug>

namespace StData {

Project::Project(const QString& fileName, QObject *parent) :
    QObject(parent),
    m_fileInfo(fileName),
    m_rootClass(nullptr),
    m_rootObject(nullptr),
    m_projectLibrary(nullptr)
{
    m_varCollection = new ProjectVarCollection();
    m_projectLibrary = new ProjectLibrary(this);
    m_objectVarsLinker = new ObjectVarsLinker(this);

    m_libraryCollection = LibraryCollection::create();
//    m_libraryCollection = &LibraryCollection::instance();
}

Project::~Project()
{
//    LibraryCollection::instance().removeProjectLibraries();
    delete m_libraryCollection;
//    delete m_projectLibrary;
//    qDeleteAll(m_extraLibraries);
    if(m_rootObject)
        delete m_rootObject;
}

QDir Project::projectDir() const
{
    return m_projectLibrary->libraryDir();
}

Class* Project::getClassByName(const QString& className)
{
    return m_libraryCollection->classByName(className);
//    return LibraryCollection::instance().classByName(className);
}

void Project::toStartupState()
{
    if(!m_rootObject)
        return;

    m_rootObject->toStartupState();
    m_rootObject->saveState();
}

void Project::setupObjects()
{
    if(m_rootObject)
        delete m_rootObject;

    m_rootObject = Object::createByClass(this, m_rootClass);
    m_objectVarsLinker->setRootObject(m_rootObject);
}

void Project::setupLinks(bool saveVarData)
{
    m_objectVarsLinker->link(saveVarData);
}

void Project::setupVars()
{
    if(!m_rootObject)
        return;

    m_rootObject->toDefaultState();

    ObjectVarsLoader objVarsLoader;
    objVarsLoader.load(this, m_rootObject);

    m_rootObject->saveState();
    m_rootObject->saveStartupState();
}

bool Project::load()
{
    bool result = false;

    SystemLog::instance().info("Load project started");
    SystemLog::instance().info(QString("Project file: %1").arg(m_fileInfo.absoluteFilePath()));

    QFile projectFile(m_fileInfo.absoluteFilePath());
    if(projectFile.open(QFile::ReadOnly))
    {
        SystemLog::instance().info("Project file opened");

        St2000DataStream stream(&projectFile);
        qint16 code = stream.readInt16();
        if(code == ProjectCode)
        {
            SystemLog::instance().info("ProjectCode");
            m_projectLibrary->load();
            m_libraryCollection->addLibrary(m_projectLibrary);

            code = stream.readInt16();
            bool stopReadingFile = false;
            while(code != 0 && !stopReadingFile)
            {
                switch(code)
                {
                    case PasswordCode:
                        SystemLog::instance().error(QObject::tr("Protected project not supported"));
                        break;

                    case MainClassCode:
                    {
                        QString rootClassName = stream.readString();

                        //                        Class* rootClassInfo = classInfoByName(rootClassName);
                        m_rootClass = m_libraryCollection->classByName(rootClassName);
                        if(!m_rootClass)
                            SystemLog::instance().error(QString(QObject::tr("Class with name: %1 not existed"))
                                                  .arg(rootClassName));
                    }break;

                    case ProjectVarsCode:
                    {
                        ProjectVarsLoader projectVarsLoader;
                        projectVarsLoader.load(stream, varManager());

                        bool isLibsRecursive = true;
                        if(varManager()->contains("lib_norecurse"))
                            isLibsRecursive = (varManager()->getVarI("lib_norecurse") == 0);

                        if(isLibsRecursive)
                        {
                            auto extraLibraries = ExtraLibrary::loadLibraries(this,
                                                                           fileInfo().absolutePath(),
                                                                           isLibsRecursive);

                            m_libraryCollection->addLibraries(extraLibraries);
                        }

                        if(varManager()->contains("addlib"))
                        {
                            QStringList addLibPaths = varManager()->getVarS("addlib").split(';');

                            for(QString addLibPath : addLibPaths)
                            {
                                auto extraLibraries = ExtraLibrary::loadLibraries(this,
                                                                                  addLibPath,
                                                                                  isLibsRecursive); 
                                m_libraryCollection->addLibraries(extraLibraries);
                            }
                        }
                    }break;

                    case WatchCode:
                        SystemLog::instance().warning("variable watching not supported");
                        stopReadingFile = true;
                        break;
                }
                code = stream.readInt16();
            }
            setupObjects();
            setupLinks(false);
            setupVars();

            result = true;
        }
    }
    projectFile.close();

    return result;
}

}
