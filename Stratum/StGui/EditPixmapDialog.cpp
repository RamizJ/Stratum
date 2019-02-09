#include "EditPixmapDialog.h"
#include "ui_EditPixmapDialog.h"

#include <PixmapGraphicsItem.h>

#include <SpaceItem.h>

using namespace StSpace;

EditPixmapDialog::EditPixmapDialog(PixmapGraphicsItem* item, QWidget *parent) :
    QDialog(parent, Qt::Window),
    ui(new Ui::EditPixmapDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Свойства изображения"));

    setPixmapGraphicsItem(item);
}

EditPixmapDialog::~EditPixmapDialog()
{
    delete ui;
}

void EditPixmapDialog::setPixmapGraphicsItem(PixmapGraphicsItem* pixmapGraphicsItem)
{
    m_pixmapGraphicsItem = pixmapGraphicsItem;

    if(m_pixmapGraphicsItem)
    {
        ui->widgetPixmapEditor->setPixmap(m_pixmapGraphicsItem->pixmap());

        ui->widgetLocation->setSpaceItemPosition(m_pixmapGraphicsItem->pos());
        ui->widgetLocation->setSpaceItemSize(m_pixmapGraphicsItem->boundingRect().size());
        ui->widgetLocation->setSpaceItemAngle(m_pixmapGraphicsItem->rotation());
        ui->widgetLocation->setSpaceItemLayer(m_pixmapGraphicsItem->pixmapItem()->layer());
    }
}
