#include "HyperJumpManager.h"
#include "SystemFunctions.h"
#include "VirtualMachine.h"
#include "VmGlobal.h"

#include <HyperBase.h>
#include <SpaceGraphicsItem.h>
#include <SpaceItem.h>
#include <SpaceScene.h>
#include <SpaceWidget.h>
#include <QProcess>
#include <memory>
#include <PathManager.h>
#include <LibraryCollection.h>

#include <QDebug>

using namespace StSpace;
using namespace StData;

//typedef std::shared_ptr<HyperKeyParams> HyperKeyParamsPtr;

namespace StVirtualMachine {

#define CM_CLEARALL "CM_CLEARALL"
#define CM_PREVPAGE "CM_PREVPAGE"
#define CM_NEXTPAGE "CM_NEXTPAGE"

HyperJumpManager::HyperJumpManager(VirtualMachine* virtualMachine) :
    QObject(virtualMachine),
    m_virtualMachine(virtualMachine)
{}

void HyperJumpManager::registerWindow(StSpace::SpaceWidget* widget)
{
    if(!m_registeredWindows.contains(widget))
    {
        m_registeredWindows << widget;
        for(QGraphicsItem* graphicsItem : widget->spaceScene()->items())
            if(auto spaceGraphicsItem = dynamic_cast<SpaceGraphicsItem*>(graphicsItem))
                registerItem(widget, spaceGraphicsItem);
    }
}

void HyperJumpManager::registerItem(SpaceWidget* widget, SpaceGraphicsItem* item)
{
    if(item->spaceItem()->actualHyperKey())
    {
        if(!m_registeredItems.contains(item))
        {
            m_registeredItems << item;
            m_spaceWidgetForItem[item->spaceItem()] = widget;
            item->activateHyperJump();

            connect(item->spaceItem(), &SpaceItem2d::hyperJumpInvoked,
                    this, &HyperJumpManager::hyperJumpInvoked);
        }
    }
}

void HyperJumpManager::reset()
{}

void HyperJumpManager::hyperJumpInvoked()
{
    SpaceItem* spaceItem = static_cast<SpaceItem*>(sender());
    if(SpaceItemHyperKeyPtr key = spaceItem->actualHyperKey())
    {
        switch(key->openMode())
        {
            case SpaceItemHyperKey::OM_RunApp:
                runApp(key->target());
                break;

            case SpaceItemHyperKey::OM_OpenProject:
                runProject(key);
                break;

            case 4:     //Системная команда
                if(QString::compare(key->target(), CM_CLEARALL, Qt::CaseInsensitive) == 0)
                {
                    execFlags |= EF_MUSTSTOP;
                }
                else if(QString::compare(key->target(), CM_PREVPAGE, Qt::CaseInsensitive) == 0)
                {
                    //пока просто закрываем проект
                    execFlags |= EF_MUSTSTOP;
                }
                else if(QString::compare(key->target(), CM_NEXTPAGE, Qt::CaseInsensitive) == 0)
                {
                    qCritical() << "CM_NEXTPAGE not implemented";
                }

                break;

            case 3:
                break;

            default:    //Открытие окна
            {
                QString windowName = key->windowName();
                if(windowManager->getWindowItem(windowName) == nullptr)
                {
                    if(windowName.isEmpty())
                        if(SpaceWidget* spaceWidget = m_spaceWidgetForItem.value(spaceItem))
                            windowName = spaceWidget->windowName();

                    if(!windowName.isEmpty())
                    {
                        QString contentSource = key->target();
                        windowManager->openSchemeWindow(windowName, contentSource, "WS_BYSPACE");
                    }
                }
            }break;
        }
    }
}

void HyperJumpManager::runApp(const QString& fileName)
{
    QProcess* process = new QProcess();
    QObject::connect(process, SIGNAL(finished(int)), process, SLOT(deleteLater()));
    process->setWorkingDirectory(executedProject->projectDir().absolutePath());

    process->start(fileName, QProcess::ReadOnly);
}

void HyperJumpManager::runProject(const StData::SpaceItemHyperKeyPtr& key)
{
    if(!key) return;

    Project* oldProject = executedProject;
    bool isHyperjumpExecuted = false;

    m_virtualMachine->setJumpingState();

    QString projectFileName = PathManager::absolutePath(oldProject->projectDir(), key->target());
    if(!key->params().isEmpty())
    {
        LibraryCollection* oldCollection = LibraryCollection::currentCollection();

        HyperKeyParamsPtr hyperKeyParams = std::make_shared<HyperKeyParams>(key->params());
        Project* project = new Project(projectFileName);
        LibraryCollection::setCurrentCollection(project->libraryCollection());

        project->setHyperKeyParams(hyperKeyParams);
        if(project->load())
        {
            if(hyperKeyParams->hideWindows())
            {
                for(SpaceWindowItem* wndItem : windowManager->windowItems())
                    wndItem->spaceWidget()->hide();
            }

            //            if(hyperKeyParams->autoRun())
            //            {
            //                emit hyperJumpOnProject(project);
            //            }
            emit hyperJumpOnProject(project);
            isHyperjumpExecuted = true;
        }
        else
        {
            LibraryCollection::setCurrentCollection(oldCollection);
            delete project;
        }
    }

    if(!isHyperjumpExecuted)
        m_virtualMachine->resetJumpingState();
}

void HyperJumpManager::openSpaceWindow()
{
}

}
