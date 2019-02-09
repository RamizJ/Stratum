#include "ProjectHierarchyWidget.h"
#include "ui_ProjectHierarchyWidget.h"

#include <Class.h>
#include <Scheme.h>
#include <Space.h>
#include <Object.h>

#include "EditObjectDialog.h"
#include "ObjectHierarchyModel.h"
#include "TreeNode.h"
#include "EditObjectDialog.h"
#include "TabManager.h"
#include "GuiContext.h"

using namespace StData;

ProjectHierarchyWidget::ProjectHierarchyWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectHierarchyWidget)
{
    ui->setupUi(this);

    m_model = new ObjectHierarchyModel(this);
    ui->treeViewProjectHierarchy->setModel(m_model);

//    m_objectPropertyDialog = GuiHelper::instance().objectPropertiesDialog();
//    m_tabManager = GuiHelper::instance().tabManager();

    m_contextMenu = new QMenu(this);
    m_contextMenu->addAction(ui->actionObjectProperties);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(ui->actionEditSchema);
    m_contextMenu->addAction(ui->actionEditSourceCode);
    m_contextMenu->addAction(ui->actionEditImage);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(ui->actionShowOnSchema);

    //TODO вынести в MainWindow
//    m_objectPropertyDialog = new ObjectPropertiesDialog(this);

    connect(ui->treeViewProjectHierarchy, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequested(QPoint)));
    connect(ui->treeViewProjectHierarchy, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onEditObjectProperties()));
    connect(ui->lineEditFilter, SIGNAL(textEdited(QString)), this, SLOT(setFilter()));

    connect(ui->actionObjectProperties, SIGNAL(triggered()), this, SLOT(onEditObjectProperties()));
    connect(ui->actionEditSchema, SIGNAL(triggered()), this, SLOT(editClassSchema()));
    connect(ui->actionEditSourceCode, SIGNAL(triggered()), this, SLOT(onEditClassSourceCode()));
}

ProjectHierarchyWidget::~ProjectHierarchyWidget()
{
    delete ui;
}

void ProjectHierarchyWidget::setProject(Project* project)
{
    m_project = project;
    m_model->setProject(m_project);
    ui->treeViewProjectHierarchy->expandAll();
    updateActionsState();
}

void ProjectHierarchyWidget::contextMenuRequested(QPoint point)
{
    QModelIndex indexAtPoint = ui->treeViewProjectHierarchy->indexAt(point);
    if(!indexAtPoint.isValid())
        return;

    updateActionsState();
    m_contextMenu->exec(QCursor::pos());
}

void ProjectHierarchyWidget::contextMenuRequested(const QModelIndex& index)
{
    if(!index.isValid())
        return;

    updateActionsState();

    m_contextMenu->exec(QCursor::pos());
}

void ProjectHierarchyWidget::updateActionsState()
{
    QModelIndex currentIndex = ui->treeViewProjectHierarchy->currentIndex();
    UiClassObjectNode* objNode = NULL;
    bool isSchemaExists = false;
    if(currentIndex.isValid())
    {
        objNode = static_cast<UiClassObjectNode*>(currentIndex.internalPointer());
        isSchemaExists = (objNode->data()->cls()->scheme() != nullptr);
    }
    ui->actionObjectProperties->setEnabled(objNode != nullptr);
    ui->actionEditSourceCode->setEnabled(objNode != nullptr);
    ui->actionEditSchema->setEnabled(isSchemaExists);

    ui->actionShowOnSchema->setEnabled(false);
    ui->actionEditImage->setEnabled(false);
}

void ProjectHierarchyWidget::onEditObjectProperties()
{
    QModelIndex currentIndex = ui->treeViewProjectHierarchy->currentIndex();

    if(currentIndex.isValid())
    {
        TreeNode* treeNode = static_cast<TreeNode*>(currentIndex.internalPointer());
        if(UiClassObjectNode* objNode = dynamic_cast<UiClassObjectNode*>(treeNode))
            EditObjectDialog(objNode->data()).exec();
    }
}

void ProjectHierarchyWidget::setFilter()
{
    m_model->setFilter(ui->lineEditFilter->text());
    ui->treeViewProjectHierarchy->expandAll();
}

void ProjectHierarchyWidget::editClassSchema()
{
    QModelIndex currentIndex = ui->treeViewProjectHierarchy->currentIndex();

    if(currentIndex.isValid())
    {
        UiClassObjectNode* objNode = static_cast<UiClassObjectNode*>(currentIndex.internalPointer());
        GuiContext::instance().tabManager()->openSpace(objNode->data());
    }
}

void ProjectHierarchyWidget::onEditClassSourceCode()
{
    QModelIndex currentIndex = ui->treeViewProjectHierarchy->currentIndex();

    if(currentIndex.isValid())
    {
        UiClassObjectNode* objNode = static_cast<UiClassObjectNode*>(currentIndex.internalPointer());
        GuiContext::instance().tabManager()->openSourceCode(objNode->data()->cls());
    }
}
