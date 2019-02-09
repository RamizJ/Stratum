#include "TabManager.h"

#include "SpaceScene.h"
#include "SpaceView.h"
#include "SourceCodeEditor.h"
#include "EditObjectDialog.h"
#include "SourceCodeWidget.h"
#include "SpaceContextMenu.h"
#include "SpaceEditWidget.h"

#include <Class.h>
#include <Scheme.h>
#include <Space.h>
#include <Object.h>

#include <QTextEdit>
#include <QVBoxLayout>
#include <QTabBar>
#include <Core.h>
#include <Project.h>

using namespace StData;
using namespace StCore;

TabManager::TabManager(QTabWidget* tabWidget = 0, QObject *parent) :
    QObject(parent),
    m_tabWidget(tabWidget)
{
    m_closeTabAction = new QAction(tr("Закрыть"), this);
    m_closeTabAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
    m_tabWidget->addAction(m_closeTabAction);

    connect(m_tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(tabClosing(int)));
    connect(m_closeTabAction, SIGNAL(triggered()), this, SLOT(closeCurrentTab()));
}

TabManager::~TabManager()
{}

void TabManager::openSpace(Object* object)
{
    if(object)
    {
        QString className = object->cls()->originalName();
        try
        {
            Space* space = object->cls()->scheme()->getSpace();

            if(m_tabWidget->count() == 0)
                m_tabWidget->show();

            SpaceEditWidget* spaceEditWidget = m_spaceWidgetForClassName.value(className, nullptr);
            if(!spaceEditWidget)
            {
                spaceEditWidget = new SpaceEditWidget(space, object, m_tabWidget);

                m_tabWidget->addTab(spaceEditWidget, QIcon(":/Class.png"), className);
                m_spaceWidgetForClassName[className] = spaceEditWidget;
            }
            m_tabWidget->setCurrentWidget(spaceEditWidget);
        }
        catch(std::exception& ex)
        {
            QString msg = QString("При открытии пространства класса '%1' возникла ошибка:\n%2")
                          .arg(className)
                          .arg(ex.what());

            QMessageBox::critical(nullptr, tr("Ошибка"), msg);
        }
    }
}

void TabManager::openSourceCode(StData::Class* cls)
{
    if(cls)
    {
        if(m_tabWidget->count() == 0)
            m_tabWidget->show();

        QWidget* tabPageWidget;
        if(m_textWidgetForClassName.contains(cls->originalName()))
        {
            tabPageWidget = m_textWidgetForClassName[cls->originalName()];
        }
        else
        {
            tabPageWidget = createWidget();
            SourceCodeWidget* sourceCodeWidget = new SourceCodeWidget(tabPageWidget);
            sourceCodeWidget->setClass(cls);
            tabPageWidget->layout()->addWidget(sourceCodeWidget);

            m_tabWidget->addTab(tabPageWidget, QIcon(":/SaveToStt.png"), cls->originalName());

            m_textWidgetForClassName[cls->originalName()] = tabPageWidget;
        }
        m_tabWidget->setCurrentWidget(tabPageWidget);
    }
}

void TabManager::closeAllTabs()
{
    for(int i = 0; i < m_tabWidget->count(); i++)
        m_tabWidget->removeTab(i);

    qDeleteAll(m_textWidgetForClassName);
    qDeleteAll(m_spaceWidgetForClassName);

    m_textWidgetForClassName.clear();
    m_spaceWidgetForClassName.clear();

    m_tabWidget->hide();
}

void TabManager::tabClosing(int index)
{
    QWidget* tabPageWidget = m_tabWidget->widget(index);

    m_textWidgetForClassName.remove(m_tabWidget->tabText(index));
    m_spaceWidgetForClassName.remove(m_tabWidget->tabText(index));

    m_tabWidget->removeTab(index);
    tabPageWidget->deleteLater();

    if(m_tabWidget->count() == 0)
        m_tabWidget->hide();
}

void TabManager::closeCurrentTab()
{
    if(m_tabWidget->count() > 0)
        tabClosing(m_tabWidget->currentIndex());
}

QWidget* TabManager::createWidget()
{
    QWidget* tabPageWidget = new QWidget(m_tabWidget);

    QVBoxLayout* verticalLayout = new QVBoxLayout(tabPageWidget);
    verticalLayout->setMargin(2);

    tabPageWidget->setLayout(verticalLayout);
    return tabPageWidget;
}
