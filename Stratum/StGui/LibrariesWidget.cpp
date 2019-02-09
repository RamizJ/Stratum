#include "LibrariesWidget.h"
#include "ui_LibrariesWidget.h"

#include "TreeNode.h"

#include <Project.h>
#include <Library.h>
#include <Class.h>
#include <LibrariesModel.h>
#include <AddLibraryDialog.h>
#include <LibraryPropertiesDialog.h>
#include <PathManager.h>

#include <QDebug>
#include <QContextMenuEvent>
#include <QDesktopServices>

using namespace StData;

LibrariesWidget::LibrariesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LibrariesWidget)
{
    ui->setupUi(this);
    m_addLibraryDialog = new AddLibraryDialog(this);
    m_libraryPropertiesDialog = new LibraryPropertiesDialog(this);

    m_librariesModel = new LibrariesModel(this);
    ui->treeViewLibraries->setModel(m_librariesModel);

    ui->toolButtonAdd->setDefaultAction(ui->actionAddLibrary);
    ui->toolButtonAddClass->setDefaultAction(ui->actionAddClass);
    ui->toolButtonRemove->setDefaultAction(ui->actionRemove);

    m_contextMenu = new QMenu(this);
    m_contextMenu->addAction(ui->actionProperties);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(ui->actionOpenInFileManager);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(ui->actionAddClass);
    m_contextMenu->addAction(ui->actionRemove);

    //Connections
    {
        connect(ui->treeViewLibraries, SIGNAL(clicked(QModelIndex)), this, SLOT(treeItemClicked(QModelIndex)));
        connect(ui->actionAddLibrary, SIGNAL(triggered()), this, SLOT(addLibrary()));
        connect(ui->actionRemove, SIGNAL(triggered()), this, SLOT(removeItem()));
        connect(ui->toolButtonHideEmpty, SIGNAL(toggled(bool)), this, SLOT(hideEmptyLibraries(bool)));
        connect(ui->lineEditFilter, SIGNAL(textEdited(QString)), this, SLOT(setFilter()));

        connect(ui->treeViewLibraries, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequested(QPoint)));

        connect(ui->actionOpenInFileManager, SIGNAL(triggered()), this, SLOT(onOpenInFileManagerTriggered()));
        connect(ui->actionProperties, SIGNAL(triggered()), this, SLOT(onPropertiesTriggered()));
    }
    updateActionsState();
}

LibrariesWidget::~LibrariesWidget()
{
    delete ui;
}

LibrariesModel* LibrariesWidget::librariesModel() const
{
    return m_librariesModel;
}

void LibrariesWidget::setProject(Project* project)
{
    m_librariesModel->setProject(project);
    m_addLibraryDialog->setProject(project);

    updateActionsState();
}

void LibrariesWidget::treeItemClicked(const QModelIndex& /*index*/)
{
    updateActionsState();
}

void LibrariesWidget::updateActionsState()
{
    QModelIndex currentIndex = ui->treeViewLibraries->currentIndex();
    TreeNode* treeNode = nullptr;
    bool isSystemLibrary(false);
    bool isProjectLibrary(false);
    bool isExtraLibrary(false);
    bool isClass(false);

    if(currentIndex.isValid())
    {
        treeNode = static_cast<TreeNode*>(currentIndex.internalPointer());

        if(LibraryNode* libraryNode = dynamic_cast<LibraryNode*>(treeNode))
        {
            isSystemLibrary = (dynamic_cast<SystemLibrary*>(libraryNode->data()) != nullptr);
            isExtraLibrary = (dynamic_cast<ExtraLibrary*>(libraryNode->data())  != nullptr);
            isProjectLibrary = (currentIndex == m_librariesModel->projectSectionIndex());
        }
        isClass = (dynamic_cast<ClassNode*>(treeNode) != nullptr);
    }

    ui->toolButtonHideEmpty->setEnabled(m_librariesModel->project() != NULL);

    ui->actionAddLibrary->setEnabled(m_librariesModel->project() != NULL);
    ui->actionAddClass->setEnabled(isProjectLibrary || isExtraLibrary);
    ui->actionRemove->setEnabled(isExtraLibrary);

    ui->actionOpenInFileManager->setEnabled(isSystemLibrary || isExtraLibrary || isClass);
    ui->actionProperties->setEnabled(treeNode &&
                                     currentIndex != m_librariesModel->extraSectionIndex());
}

void LibrariesWidget::addLibrary()
{
    //    QPoint point = ui->toolButtonAdd->mapToGlobal(QPoint(ui->toolButtonAdd->rect().center().x(), ui->toolButtonAdd->rect().bottom() + 9));
    //    point.setX(point.x() - m_addLibraryDialog->rect().center().x());
    //    m_addLibraryDialog->move(point);

    if(m_addLibraryDialog->exec() == QDialog::Accepted)
    {
        m_librariesModel->addLibrary(m_addLibraryDialog->selectedPath(), m_addLibraryDialog->isRecursive());
        ui->treeViewLibraries->expand(m_librariesModel->projectSectionIndex());
        ui->treeViewLibraries->expand(m_librariesModel->extraSectionIndex());
    }
}

void LibrariesWidget::removeItem()
{
    QModelIndex removedIndex = ui->treeViewLibraries->currentIndex();
    m_librariesModel->removeItem(removedIndex);
}

void LibrariesWidget::hideEmptyLibraries(bool isEmptyLibrariesHidden)
{
    m_librariesModel->setIsEmptyLibrariesHidden(isEmptyLibrariesHidden);

    if(!ui->lineEditFilter->text().isEmpty())
        ui->treeViewLibraries->expandAll();
    else
    {
        ui->treeViewLibraries->expand(m_librariesModel->projectSectionIndex());
        ui->treeViewLibraries->expand(m_librariesModel->extraSectionIndex());
    }
}

void LibrariesWidget::setFilter()
{
    m_librariesModel->setFilter(ui->lineEditFilter->text());

    if(!ui->lineEditFilter->text().isEmpty())
        ui->treeViewLibraries->expandAll();
    else
    {
        ui->treeViewLibraries->expand(m_librariesModel->projectSectionIndex());
        ui->treeViewLibraries->expand(m_librariesModel->extraSectionIndex());
    }
}

void LibrariesWidget::contextMenuRequested(const QPoint& point)
{
    QModelIndex indexAtPoint = ui->treeViewLibraries->indexAt(point);
    if(!indexAtPoint.isValid())
        return;

    updateActionsState();

    QPoint globalPoint = ui->treeViewLibraries->mapToGlobal(point);
    m_contextMenu->exec(globalPoint);
}

void LibrariesWidget::onPropertiesTriggered()
{
    QModelIndex currentItemIndex = ui->treeViewLibraries->currentIndex();
    if(currentItemIndex.isValid())
    {
        TreeNode* treeNode = static_cast<TreeNode*>(currentItemIndex.internalPointer());
        //        Class* cls = qobject_cast<Class*>(treeItem->data());

        if(LibraryNode* libraryNode = dynamic_cast<LibraryNode*>(treeNode))
        {
            m_libraryPropertiesDialog->setLibrary(libraryNode->data());
            m_libraryPropertiesDialog->show();
        }
        //        else if(cls)
        //            result = cls->library()->absolutePath();
    }
}

void LibrariesWidget::onOpenInFileManagerTriggered()
{
    QString path = currentItemPath();
    if(!path.isEmpty())
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

QString LibrariesWidget::currentItemPath()
{
    QString result;
    QModelIndex currentIndex = ui->treeViewLibraries->currentIndex();

    if(currentIndex.isValid())
    {
        TreeNode* treeNode = static_cast<TreeNode*>(currentIndex.internalPointer());
        UiTreeNode* sectionNode = dynamic_cast<UiTreeNode*>(treeNode);
        LibraryNode* libraryNode = dynamic_cast<LibraryNode*>(treeNode);
        ClassNode* classNode = dynamic_cast<ClassNode*>(treeNode);

        if(libraryNode)
            result = libraryNode->data()->absolutePath();
        else if(classNode)
            result = classNode->data()->library()->absolutePath();

        else if(sectionNode && sectionNode->text() == LibrariesModel::systemSectionName())
            result = PathManager::instance().appPath();

        else if(sectionNode && sectionNode->text() == LibrariesModel::projectSectionName())
            result = m_librariesModel->project()->fileInfo().absolutePath();
    }
    return result;
}
