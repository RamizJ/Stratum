#ifndef EDITLINKDIALOG_H
#define EDITLINKDIALOG_H

#include <QDialog>

namespace Ui {
class EditLinkDialog;
}

namespace StSpace {
class LinkGraphicsItem;
}

class EditLinkDialog : public QDialog
{
    Q_OBJECT

public:
    EditLinkDialog(StSpace::LinkGraphicsItem* item, QWidget *parent = 0);
    ~EditLinkDialog();

    StSpace::LinkGraphicsItem* linkGraphicsItem() const {return m_linkGraphicsItem;}
    void setLinkGraphicsItem(StSpace::LinkGraphicsItem* linkGraphicsItem) {m_linkGraphicsItem = linkGraphicsItem;}

private:
    Ui::EditLinkDialog *ui;
    StSpace::LinkGraphicsItem* m_linkGraphicsItem;
};

#endif // EDITLINKDIALOG_H
