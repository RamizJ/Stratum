#include "Core.h"
#include "Run.h"
#include <Settings.h>

#include <Project.h>
#include <LibraryCollection.h>
#include <Library.h>
#include <Class.h>
#include <Object.h>
#include <Compiler.h>
#include <IVirtualMachine.h>
#include <VirtualMachine.h>
#include <StandardTypes.h>
#include <IconManager.h>
#include <Error.h>

using namespace std;
using namespace StData;
using namespace StCompiler;
using namespace StVirtualMachine;
using namespace StInterfaces;

namespace StCore
{

Core::Core() :
    m_currentProject(nullptr)
{
    m_virtualMachine = new VirtualMachine(this);
    m_compiler = new Compiler(this);
    m_run = new Run(m_virtualMachine, this);
//    m_settings = new Settings(this);
}

Core::~Core()
{
    LibraryCollection::unloadSystemLibraries();
}

Core& Core::instance()
{
    static Core core;
    return core;
}

void Core::initialize()
{
    IconManager::instance().loadStandardIcons();

    LibraryCollection::loadSystemLibraries();
    StandardTypes::initialize(LibraryCollection::systemLibraries());

    if(!m_compiler->initialize())
        throw runtime_error(tr("Не удалось проинициализировать компилятор").toStdString());

    m_virtualMachine->initialize();
}

void Core::openProject(const QString& projectFileName)
{
    closeCurrentProject();

    m_currentProject = new Project(projectFileName);
    LibraryCollection::setCurrentCollection(m_currentProject->libraryCollection());

    if(!m_currentProject->load())
        throw runtime_error(tr("Не удалось загрузить проект").toStdString());

    m_run->setCurrentProject(m_currentProject);
    m_virtualMachine->setProject(m_currentProject);
    m_virtualMachine->loadProjectMatrixes();
//    m_settings->setLastProjectPath(projectFileName);
    Settings::instance().setLastProjectPath(projectFileName);
}

void Core::saveProject()
{}

void Core::closeCurrentProject()
{
    if(m_run->inProgress())
        m_run->stop();

    if(m_currentProject != nullptr)
    {
        m_virtualMachine->savePlusMatrixes();
        m_virtualMachine->deleteAllMatrixes();
        m_run->setCurrentProject(nullptr);
        m_virtualMachine->setProject(nullptr);

        delete m_currentProject;
        m_currentProject = nullptr;
    }
}

}
