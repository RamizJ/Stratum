#include "Run.h"

#include <Project.h>
#include <IVirtualMachine.h>

#include <HyperBase.h>
#include <Log.h>
#include <QDebug>

using namespace StData;
using namespace StInterfaces;

namespace StCore {

Run::Run(StInterfaces::IVirtualMachine* virtualMachine, QObject *parent) :
    QObject(parent),
    m_timerId(0),
    m_timerInterval(5),
    m_inProgress(false),
    m_currentProject(nullptr),
    m_jumpingProject(nullptr),
    m_virtualMachine(virtualMachine),
    m_iterationInProcess(false),
    m_mustStop(false)
{
    connect(m_virtualMachine, &IVirtualMachine::closeAllRequested, this, &Run::onCloseAll);
    connect(m_virtualMachine, &IVirtualMachine::stopRequested, this, &Run::pause);
    connect(m_virtualMachine, &IVirtualMachine::hyperJumpOnProject, this, &Run::hyperJumpOnProject);
}

Run::~Run()
{}

void Run::setCurrentProject(Project* currentProject)
{
    m_currentProject = currentProject;

    if(m_currentProject != nullptr)
        m_executedProjects << m_currentProject;
}

void Run::start()
{
    stopTimer();

    if(m_currentProject == nullptr)
        throw std::runtime_error(tr("Ошибка запуска проекта на выполнение. Проект не определен").toStdString());

    m_timerId = startTimer(m_timerInterval);
    m_inProgress = true;

    doOneIteration();
}

void Run::pause()
{
    stopTimer();
    emit vmPause();
}

void Run::stop()
{
    stopTimer();
    m_virtualMachine->reset();
    m_currentProject->toStartupState();
    emit vmStop();
}

void Run::oneStep()
{
    if(m_inProgress)
        stopTimer();

    doOneIteration();
}

void Run::timerEvent(QTimerEvent*)
{
    if(m_iterationInProcess)
        return;

    m_iterationInProcess = true;

    if(m_mustStop)
    {
        m_virtualMachine->resetForProject(m_currentProject);
        if(HyperKeyParamsPtr hyperKeyParams = m_currentProject->hyperKeyParams())
        {
            if(hyperKeyParams->autoClose() && !m_executedProjects.isEmpty())
            {
                m_mustStop = false;

                delete m_currentProject;

                m_currentProject = m_executedProjects.last();
                m_executedProjects.removeLast();

//                m_currentProject->toStartupState();
                m_virtualMachine->setProject(m_currentProject);

                //Блокировать сообщения от мыши
                if(hyperKeyParams->hideWindows())
                    m_virtualMachine->showProjectWindows(m_currentProject);
            }
        }
        else
        {
            stopTimer();
            m_mustStop = false;
            return;
        }
    }

    try {
        doOneIteration();
    }
    catch(std::runtime_error e) {
        SystemLog::instance().error(e.what());
        stop();
    }


    if(m_jumpingProject)
    {
        m_executedProjects << m_currentProject;
        m_currentProject = m_jumpingProject;

        m_virtualMachine->setProject(m_currentProject);

        m_jumpingProject = nullptr;
        m_virtualMachine->resetJumpingState();
    }

    m_iterationInProcess = false;
}

void Run::onCloseAll()
{
    m_mustStop = true;
}

void Run::hyperJumpOnProject(Project* project)
{
    if(project)
        m_jumpingProject = project;
}

void Run::stopTimer()
{
    if(m_timerId)
    {
        killTimer(m_timerId);
        m_timerId = 0;
        m_inProgress = false;
        m_iterationInProcess = false;
    }
}

void Run::doOneIteration()
{
    try
    {
        m_virtualMachine->doOneIteration();
    }
    catch(...)
    {
        stopTimer();
        throw;
    }
}

}
