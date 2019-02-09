#include "EditSchemeDialog.h"
#include "ui_EditSchemeDialog.h"

#include <Space.h>

EditSchemeDialog::EditSchemeDialog(StData::Space* space, QWidget *parent) :
    QDialog(parent, Qt::Window),
    ui(new Ui::EditSchemeDialog),
    m_space2d(space)
{
    ui->setupUi(this);

    setWindowTitle(tr("Параметры листа"));
}


EditSchemeDialog::~EditSchemeDialog()
{
    delete ui;
}

void EditSchemeDialog::addLayerItem(QListWidget* listWidget, int index)
{
    auto item = new QListWidgetItem(QString::number(index), listWidget);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(m_space2d->isLayerVisible(index) ? Qt::Checked : Qt::Unchecked);
}


void EditSchemeDialog::setSpace2d(StData::Space* space2d)
{
    if(m_space2d == space2d)
        return;

    m_space2d = space2d;
    if(m_space2d)
    {
        setupLayers();
    }
}

void EditSchemeDialog::setupLayers()
{
    for(int i = 0; i <= 15; i++)
        addLayerItem(ui->listWidgetGraphicsLayers, i);

    for(int i = 16; i <= 23; i++)
        addLayerItem(ui->listWidgetImageLayers, i);

    for(int i = 24; i <= 31; i++)
        addLayerItem(ui->listWidgetLinkLayers, i);
}
