#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <memory>

#include <Log.h>
#include <Core.h>
#include <Project.h>
#include <Class.h>
#include <Scheme.h>
#include <IconManager.h>
#include <Library.h>
#include <Error.h>
#include <Settings.h>

#include "SpaceScene.h"
#include "TabManager.h"
#include "GuiContext.h"
#include "Run.h"

#include <PathManager.h>
#include <QDebug>

using namespace std;
using namespace StData;
using namespace StCore;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    installEventFilter(&m_resizeFilter);

    Settings::instance().setMainWindowSize(size());

    m_tabManager = new TabManager(ui->tabWidget, this);

    GuiContext::instance().setTabManager(m_tabManager);

    tabifyDockWidget(ui->dockWidgetLibraries, ui->dockWidgetProjectHierarchy);
    tabifyDockWidget(ui->dockWidgetProjectHierarchy, ui->dockWidgetInfo);

    setupDockActions();

    //    ui->dockWidgetOutput->hide();
    ui->dockWidgetWatch->hide();
    ui->dockWidgetInfo->hide();
    ui->tabWidget->hide();

    updateActionsState();

    //connections
    connect(ui->actionOpenProject, &QAction::triggered, this, &MainWindow::openProjectFile);
    connect(ui->actionCloseProject, &QAction::triggered, this, &MainWindow::closeProject);

    Run* run = Core::instance().run();
    connect(ui->actionRun, &QAction::triggered, this, &MainWindow::run);
    connect(ui->actionStop, &QAction::triggered, this, &MainWindow::stop);
    connect(ui->actionPause, &QAction::triggered, this, &MainWindow::pause);
    connect(ui->actionOneStep, &QAction::triggered, run, &Run::oneStep);

    //        connect(run, &Run::error, this, &MainWindow::runError);
    connect(run, &Run::vmStop, this, &MainWindow::onVmStop);
    connect(run, &Run::vmPause, this, &MainWindow::onVmPause);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadProject(const QString& fileName)
{
    openProject(fileName);
    updateActionsState();
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if(event->type() == QEvent::Resize)
    {
        QSize size = static_cast<QResizeEvent*>(event)->size();
        Settings::instance().setMainWindowSize(size);

        QString msg = QString("MainWnd resized to: %1x%2").arg(size.width()).arg(size.height());
        SystemLog::instance().info(msg);
    }

    return QObject::eventFilter(watched, event);
}

void MainWindow::showEvent(QShowEvent* /*event*/)
{

    QString msg = QString("MainWnd size: %1x%2").arg(size().width()).arg(size().height());
    SystemLog::instance().info(msg);

    Settings::instance().setMainWindowSize(size());
}

//void MainWindow::resizeEvent(QResizeEvent* event)
//{
//    QSize size = event->size();
//    Settings::instance().setMainWindowSize(size);

//    QString msg = QString("MainWnd resized to: %1x%2").arg(size.width()).arg(size.height());
//    SystemLog::instance().info(msg);
//}

void MainWindow::openProjectFile()
{
    //    Core& core = Core::instance();
    QString projectFileName;
    bool isFileDialogAccepted = false;

    QFileDialog dialog(this, Qt::Window);
    dialog.setWindowTitle(tr("Выберите файл проекта"));
    dialog.setDirectory(Settings::instance().lastProjectPath());
    dialog.setNameFilter("Stratum project (*.spj *.prj)");
    dialog.setFileMode(QFileDialog::ExistingFile);

    if(dialog.exec() == QFileDialog::Accepted && !dialog.selectedFiles().isEmpty())
    {
        isFileDialogAccepted = true;
        projectFileName = dialog.selectedFiles().first();
    }

    if(isFileDialogAccepted)
        openProject(projectFileName);

    updateActionsState();
}

void MainWindow::closeProject()
{
    m_tabManager->closeAllTabs();
    ui->widgetProjectHierarhy->setProject(nullptr);
    ui->widgetLibraries->setProject(nullptr);

    Core::instance().closeCurrentProject();

    updateActionsState();
}

void MainWindow::run()
{
    ui->actionRun->setEnabled(false);
    ui->actionOneStep->setEnabled(false);
    ui->actionStop->setEnabled(true);
    ui->actionPause->setEnabled(true);

    Run* run = Core::instance().run();
    try
    {
        run->start();
    }
    catch(std::runtime_error& e)
    {
        QMessageBox::critical(this, tr("Ошибка"), e.what());
    }

    //    updateActionsState();
}

void MainWindow::stop()
{
    Core::instance().run()->stop();
    updateActionsState();
}

void MainWindow::pause()
{
    Core::instance().run()->stop();
    updateActionsState();
}

void MainWindow::oneStep()
{
    Core::instance().run()->oneStep();
    updateActionsState();
}

void MainWindow::onVmStop()
{
    ui->actionRun->setEnabled(true);
    ui->actionStop->setEnabled(false);
    ui->actionPause->setEnabled(false);
    ui->actionOneStep->setEnabled(true);
}

void MainWindow::onVmPause()
{
    ui->actionRun->setEnabled(true);
    ui->actionStop->setEnabled(true);
    ui->actionPause->setEnabled(false);
    ui->actionOneStep->setEnabled(true);
}

void MainWindow::setupDockActions()
{
    ui->menuDocWindows->addAction(ui->dockWidgetProjectHierarchy->toggleViewAction());
    ui->menuDocWindows->addAction(ui->dockWidgetInfo->toggleViewAction());
    ui->menuDocWindows->addAction(ui->dockWidgetLibraries->toggleViewAction());
    ui->menuDocWindows->addAction(ui->dockWidgetOutput->toggleViewAction());
    ui->menuDocWindows->addAction(ui->dockWidgetWatch->toggleViewAction());

    ui->menuToolbars->addAction(ui->toolBarGeneral->toggleViewAction());
    ui->menuToolbars->addAction(ui->toolBarModeling->toggleViewAction());
    ui->menuToolbars->addAction(ui->toolBarSchemaObjects->toggleViewAction());
    ui->menuToolbars->addAction(ui->toolBarShemaView->toggleViewAction());
    ui->menuToolbars->addAction(ui->dockWidgetWatch->toggleViewAction());
}


void MainWindow::updateActionsState()
{
    Project* currentProject = Core::instance().currentProject();
    Run* run = Core::instance().run();
    ui->toolBarModeling->setEnabled(currentProject != nullptr);
    ui->toolBarSchemaObjects->setEnabled(currentProject != nullptr);
    ui->toolBarShemaView->setEnabled(currentProject != nullptr);

    bool runEnabled = !run->inProgress() && currentProject && currentProject->rootObject();
    ui->actionRun->setEnabled(runEnabled);
    ui->actionOneStep->setEnabled(runEnabled);
    ui->actionStop->setEnabled(run->inProgress());
    ui->actionPause->setEnabled(run->inProgress());
}

void MainWindow::openProject(const QString& projectFileName)
{
    if(!projectFileName.isEmpty())
    {
        m_tabManager->closeAllTabs();
        ui->widgetLibraries->setProject(nullptr);
        ui->widgetProjectHierarhy->setProject(nullptr);

        try
        {
            Core::instance().openProject(projectFileName);

            Project* currentProject = Core::instance().currentProject();
            ui->widgetLibraries->setProject(currentProject);
            ui->widgetProjectHierarhy->setProject(currentProject);
            m_tabManager->openSpace(currentProject->rootObject());
        }
        catch(std::exception& ex)
        {
            m_tabManager->closeAllTabs();
            QMessageBox::critical(this, tr("Ошибка"), ex.what());
        }
    }
}

void MainWindow::androidFileNameReady(QString fileName)
{
    openProject(fileName);
    updateActionsState();
}


ResizeFilter::ResizeFilter() : QObject()
{}

bool ResizeFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Resize)
    {
        QResizeEvent* resizeEv = static_cast<QResizeEvent*>(event);
        qDebug() << resizeEv->size();
    }
    return QObject::eventFilter(obj, event);
}
