#include "EditRasterTextDialog.h"
#include "ui_EditRasterTextDialog.h"

#include <QColorDialog>
#include <QStringListModel>
#include <SpaceItem.h>
#include <Tool2d.h>
#include <RasterTextGraphicsItem.h>

using namespace StData;
using namespace StSpace;

EditRasterTextDialog::EditRasterTextDialog(RasterTextGraphicsItem* item, QWidget *parent) :
    QDialog(parent, Qt::Window),
    ui(new Ui::EditRichTextDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Свойства"));

    setupFontSizeCombobox();
    setRichTextGraphicsItem(item);

    connect(ui->toolButtonTextColor, SIGNAL(clicked()), SLOT(onSelectTextColor()));
    connect(ui->toolButtonBackroundColor, SIGNAL(clicked()), SLOT(onSelectBackgroundColor()));
}

EditRasterTextDialog::~EditRasterTextDialog()
{
    delete ui;
}

void EditRasterTextDialog::setRichTextGraphicsItem(RasterTextGraphicsItem* richTextGraphicsItem)
{
    m_rasterTextGraphicsItem = richTextGraphicsItem;
    if(m_rasterTextGraphicsItem)
    {
        ui->textEditRichText->setDocument(m_rasterTextGraphicsItem->document());

        ui->widgetLocation->setSpaceItemPosition(m_rasterTextGraphicsItem->pos());
        ui->widgetLocation->setSpaceItemSize(m_rasterTextGraphicsItem->boundingRect().size());
        ui->widgetLocation->setSpaceItemAngle(m_rasterTextGraphicsItem->rotation());
        ui->widgetLocation->setSpaceItemLayer(m_rasterTextGraphicsItem->textItem()->layer());
    }
}

void EditRasterTextDialog::onSelectTextColor()
{
    QColorDialog::getColor(Qt::black, this, tr("Цвет текста"), QColorDialog::ShowAlphaChannel);
}

void EditRasterTextDialog::onSelectBackgroundColor()
{
    QColorDialog::getColor(Qt::white, this, tr("Цвет заливки текста"), QColorDialog::ShowAlphaChannel);
}

void EditRasterTextDialog::setupFontSizeCombobox()
{
    QList<int> fontSizeList = QFontDatabase::standardSizes();
    QStringList fontSizeStrList;
    for(int fontSize : fontSizeList)
        fontSizeStrList << QString::number(fontSize);

    ui->comboBoxPointSize->setModel(new QStringListModel(fontSizeStrList, this));
}
