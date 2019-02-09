#include "EditGroupDialog.h"
#include "ui_EditGroupDialog.h"
#include "GroupGraphicsItemModel.h"

#include <GroupGraphicsItem.h>
#include <SpaceItem.h>
#include <SpaceItem.h>

using namespace StSpace;
using namespace StData;

EditGroupDialog::EditGroupDialog(GroupGraphicsItem* item, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditGroupDialog),
    m_graphicsItemGroup(item)
{
    ui->setupUi(this);

    setWindowTitle(tr("Свойства группы"));

    m_groupDataModel = new GroupGraphicsItemModel(this);
    m_groupDataModel->setGroupItem(item->groupItem());
    ui->treeViewGroupModel->setModel(m_groupDataModel);
}

EditGroupDialog::~EditGroupDialog()
{
    delete ui;
}

void EditGroupDialog::setGraphicsItemGroup(GroupGraphicsItem* graphicsItemGroup)
{
    m_graphicsItemGroup = graphicsItemGroup;

    if(m_graphicsItemGroup)
    {
        GroupItem* groupItem = m_graphicsItemGroup->groupItem();

        m_groupDataModel->setGroupItem(groupItem);

        ui->widgetLocation->setSpaceItemName(groupItem->name());
        ui->widgetLocation->setSpaceItemPosition(groupItem->origin());
        ui->widgetLocation->setSpaceItemSize(groupItem->size());
        ui->widgetLocation->setSpaceItemAngle(groupItem->angle());
        ui->widgetLocation->setSpaceItemLayer(groupItem->layer());
    }
}
