#include "VirtualMachine.h"
#include "VmGlobal.h"
#include "SpaceWindowItem.h"

#include <QTime>
#include <Class.h>
#include <LibraryCollection.h>
#include <Object.h>
#include <SpaceWidget.h>
#include <limits.h>
#include <VmCodes.h>

#include "VarFunctions.h"
#include "MathFunctions.h"
#include "StringFunctions.h"
#include "LogicBinaryFunctions.h"
#include "StreamFunctions.h"
#include "FileSystemFunctions.h"
#include "ArrayFunctions.h"
#include "MatrixFunctions.h"
#include "DialogFunctions.h"
#include "WindowFunctions.h"
#include "SpaceFunctions.h"
#include "SpaceObjectFunctions.h"
#include "ToolFunctions.h"
#include "PenFunctions.h"
#include "BrushFunctions.h"
#include "TextFunctions.h"
#include "PixmapFunctions.h"
#include "PolylineFunctions.h"
#include "GroupFunctions.h"
#include "SystemFunctions.h"
#include "MessageFunctions.h"
#include "ClassFunctions.h"
#include "ControlsFunctions.h"
#include "MultimediaFunctions.h"
#include "HyperJumpManager.h"

using namespace StData;

namespace StVirtualMachine {

VirtualMachine::VirtualMachine(QObject* parent) :
    IVirtualMachine(parent)
{
    codePointer = new CodePointer(this);
    valueStack = new VarValueStack(this);

    arrayManager = new ArrayManager(this);
    matrixManager = new MatrixManager(this);
    streamManager = new StreamManager(this);
    windowManager = new SpaceWindowManager(this);
    messageManager = new MessageManager(this);
    hyperJumpManager = new HyperJumpManager(this);

    //TODO перенести в SCore/Run
    m_keyStateObserver = new KeyStateObserver(this);
    m_keyStateObserver->start();
    environmentTime.start();

    virtualMachine = this;
    fontDataBase = &m_fontDatabase;

    connect(hyperJumpManager, &HyperJumpManager::hyperJumpOnProject,
            this, &VirtualMachine::hyperJumpOnProject);
}

void VirtualMachine::setProject(Project* project)
{
    valueStack->reset();
    contextStack.clear();

    m_project = project;
    if(m_project)
        LibraryCollection::setCurrentCollection(m_project->libraryCollection());
}

void VirtualMachine::initialize()
{
    operations.resize(VM_MAXIMUM_CODE);
    operations.fill(nullptr);

    setupVarFunctions();
    setupMathFunctions();
    setupStringFunctions();
    setupLogicBinaryFunctions();
    setupStreamFunctions();
    setupFileSystemFunctions();
    setupArrayFunctions();
    setupMatrixFunctions();
    setupDialogFunctions();
    setupWindowFunctions();
    setupSpaceFunctions();
    setupSpaceObjectFunctions();
    setupToolFunctions();
    setupPenFunctions();
    setupBrushFunctions();
    setupTextFunctions();
    setupTextureFunctions();
    setupPolylineFunctions();
    setupGroupFunctions();
    setupSystemFunctions();
    setupMessageFunctions();
    setupClassFunctions();
    setupControlFunctions();
    setupMultimediaFunctions();
}

void VirtualMachine::resetForProject(Project* project)
{
    valueStack->reset();
    contextStack.clear();

    matrixManager->deleteMinusMatrixes(project);
    streamManager->clear(project);
    messageManager->reset(project);
    windowManager->clear(project);

    executedObject = nullptr;
    currentIterationNumber = 0;
}

void VirtualMachine::reset()
{
    valueStack->reset();
    contextStack.clear();

    arrayManager->reset();
    matrixManager->deleteMinusMatrixes(m_project);
    streamManager->clearAll();
    windowManager->clearAll();

    executedObject = nullptr;
    currentIterationNumber = 0;
}

void VirtualMachine::doOneIteration()
{
    if(m_project == nullptr || m_project->rootObject() == nullptr)
        throw std::runtime_error(tr("Проект не проинициализирован").toStdString());

    bool mustStop = (execFlags & EF_MUSTSTOP) != 0;

    if(!mustStop)
    {
        executedProject = m_project;
        //executedProject->rootObject()->restoreState();

        execute(executedProject->rootObject());
        messageManager->processSendedMessages();

        executedProject->rootObject()->saveState();

        if(currentIterationNumber < ULLONG_MAX)
            currentIterationNumber++;

        //TODO перерисовывать только при необходимости.
        //Следить за изменяемыми элементами, перерисовывать только нужный квадрат
        for(SpaceWindowItem* wndItem : windowManager->windowItems())
            wndItem->spaceWidget()->updateViewport();
    }
    else
    {
        emit closeAllRequested();
        execFlags = 0;
    }
}

void VirtualMachine::savePlusMatrixes()
{
    if(matrixManager && m_project)
        matrixManager->saveAllPlusMatrixes(m_project->projectDir());
}

void VirtualMachine::deletePlusMatrixes()
{
    if(matrixManager && m_project)
        matrixManager->deletePlusMatrixes(m_project);
}

void VirtualMachine::deleteAllMatrixes()
{
    if(matrixManager && m_project)
        matrixManager->deleteAllMatrixes(m_project);
}

void VirtualMachine::loadProjectMatrixes()
{
    if(m_project)
    {
        QStringList matrixFileNameList = m_project->projectDir().entryList({"*.mat"}, QDir::Files);

        for(QString matrixFileName : matrixFileNameList)
        {
            QStringList matrixFileNameParts = matrixFileName.split('_');
            if(!matrixFileNameParts.isEmpty())
            {
                QString idStr = matrixFileNameParts.first();
                bool isConverted;
                int matrixId = idStr.toInt(&isConverted);
                if(isConverted)
                    matrixManager->load(matrixId, m_project->projectDir().absoluteFilePath(matrixFileName));
            }
        }
    }
}

void VirtualMachine::showProjectWindows(Project* project)
{
    //Разблокировать сообщения от мыши
    for(SpaceWindowItem* wndItem : windowManager->windowItems())
        if(wndItem->project() == project)
            wndItem->spaceWidget()->show();
}

void VirtualMachine::setJumpingState()
{
    execFlags |= EF_RUNNING;
}

void VirtualMachine::resetJumpingState()
{
    execFlags &= ~EF_RUNNING;
}

bool VirtualMachine::isJumpingState()
{
    return (execFlags & EF_RUNNING) != 0;
}

}
