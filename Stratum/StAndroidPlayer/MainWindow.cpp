#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "AndroidFileDialog.h"
#include "AndroidFunctions.h"

#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <Settings.h>
#include <QDebug>
#include <qevent.h>

#include "VmLog.h"
#include "Core.h"
#include "Run.h"
#include "Log.h"


using namespace StData;
using namespace StCore;
using namespace StVirtualMachine;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_isInitialized(false)
{
    ui->setupUi(this);

    m_fileDialog = std::make_shared<AndroidFileDialog>();
    connect(m_fileDialog.get(), &AndroidFileDialog::existingFileNameReady, this, &MainWindow::openFileAccepted);

#ifndef QT_DEBUG
    ui->centralWidget->setVisible(false);
#else
    connect(&SystemLog::instance(), &SystemLog::messageWrited, this, &MainWindow::addText);
    connect(&VmLog::instance(), &VmLog::messageWrited, this, &MainWindow::addMessage);
#endif

    connect(Core::instance().run(), &Run::vmStop, this, &MainWindow::onVmStop);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize()
{
    if(m_isInitialized)
        return;

    Settings::instance().setMainWindowSize(size());
    QString msg = QString("Wnd size: %1x%2").arg(size().width()).arg(size().height());
    //SystemLog::instance().info(msg);
    addText(msg, MessageType::MT_Info);

//    QString projectFileName = AndroidFunctions::getOpeningFileName();
//    if(!projectFileName.isEmpty())
//    {
//        run(projectFileName);
//    }
//    else
//    {
//        if(!m_fileDialog->provideExistingFileName())
//        {
//            addText("Show file dialog failed");
//        }
//    }

    QString projectFileName = "assets:/8/project.spj";
    run(projectFileName);

    m_isInitialized = true;
}

void MainWindow::openFile()
{}

void MainWindow::openFileAccepted(const QString& fileName)
{
    run(fileName);
}

void MainWindow::run(const QString& projectFileName)
{
    try
    {
        Core::instance().openProject(projectFileName);
        Core::instance().run()->start();
    }
    catch(std::runtime_error& e)
    {
        addText(e.what(), MessageType::MT_Info);
    }
}

void MainWindow::setText(const QString& text)
{
    ui->label->setText(text);
}

void MainWindow::addText(const QString& text, const MessageType& msgType)
{
    if(msgType != MessageType::MT_Error && msgType != MessageType::MT_Warning)
        setText(ui->label->text() + "\n" + text);
}

void MainWindow::addMessage(const QString& text, const StData::MessageType& type)
{
    setText(ui->label->text() + "\n" + text);
}

void MainWindow::showEvent(QShowEvent* /*event*/)
{
    QSize sz = size();
    Settings::instance().setMainWindowSize(sz);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QSize sz = event->size();
    Settings::instance().setMainWindowSize(sz);
}

void MainWindow::onVmStop()
{
    addText("quit", MessageType::MT_Info);
    close();
}
