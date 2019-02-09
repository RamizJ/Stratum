#ifndef EDITGROUPDIALOG_H
#define EDITGROUPDIALOG_H

#include <QDialog>

namespace Ui {
class EditGroupDialog;
}

namespace StSpace {
class GroupGraphicsItem;
}

class GroupGraphicsItemModel;

class EditGroupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditGroupDialog(StSpace::GroupGraphicsItem* item, QWidget *parent = 0);
    ~EditGroupDialog();

    StSpace::GroupGraphicsItem* graphicsItemGroup() const {return m_graphicsItemGroup;}
    void setGraphicsItemGroup(StSpace::GroupGraphicsItem* graphicsItemGroup);

private:
    Ui::EditGroupDialog *ui;
    StSpace::GroupGraphicsItem* m_graphicsItemGroup;
    GroupGraphicsItemModel* m_groupDataModel;
};

#endif // EDITGROUPDIALOG_H
