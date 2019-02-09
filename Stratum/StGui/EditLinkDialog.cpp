#include "EditLinkDialog.h"
#include "ui_EditLinkDialog.h"

#include "LinkGraphicsItem.h"

using namespace StSpace;

EditLinkDialog::EditLinkDialog(LinkGraphicsItem* item, QWidget *parent) :
    QDialog(parent, Qt::Window),
    ui(new Ui::EditLinkDialog),
    m_linkGraphicsItem(item)
{
    ui->setupUi(this);
    setWindowTitle(tr("Свойства связи"));
}

EditLinkDialog::~EditLinkDialog()
{
    delete ui;
}

