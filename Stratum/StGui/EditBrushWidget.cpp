#include "EditBrushWidget.h"
#include "ui_EditBrushWidget.h"

#include <QStringListModel>
#include <QColorDialog>

EditBrushWidget::EditBrushWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditBrushWidget)
{
    ui->setupUi(this);

    QStringList brushStyleList;
    brushStyleList << "NoBrush" << "Solid"
                   << "Dense1" << "Dense2" << "Dense3" << "Dense4" << "Dense5" << "Dense6" << "Dense7"
                   << "Horizontal" << "Vertical" << "Cross" << "BackDiagonal" << "FrontDiagonal" << "DiagonalCross"
                   << "LinearGradient" << "RadialGradient" << "ConicalGradient";
    ui->comboBoxBrushStyle->setModel(new QStringListModel(brushStyleList, this));

    //connections
    {
        connect(ui->toolButtonBrushColor, SIGNAL(clicked()), SLOT(onChooseBrushColor()));
        connect(ui->comboBoxBrushStyle, SIGNAL(currentIndexChanged(int)), SLOT(onBrushStyleChanged(int)));
        connect(ui->groupBoxBrush, SIGNAL(toggled(bool)), SLOT(onBrushEnabled(bool)));
    }
}

EditBrushWidget::~EditBrushWidget()
{
    delete ui;
}

void EditBrushWidget::setBrush(const QBrush& brush)
{
    m_brush = brush;

    ui->comboBoxBrushStyle->setCurrentIndex(m_brush.style());
    updateButtonIconColor();
}

void EditBrushWidget::onChooseBrushColor()
{
    QColor color = QColorDialog::getColor(m_brush.color(), this, tr("Цвет заливки"), QColorDialog::ShowAlphaChannel);
    if(color.isValid())
    {
        m_brush.setColor(color);
        updateButtonIconColor();
    }
}

void EditBrushWidget::onBrushStyleChanged(int styleIndex)
{
    m_brush.setStyle(static_cast<Qt::BrushStyle>(styleIndex));
}

void EditBrushWidget::onBrushEnabled(bool /*enabled*/)
{
//    if(!enabled)
//    {
//        m_brush = QBrush;
//    }
//    else
//    {
//        m_brush = QBrush(Qt::white, Qt::SolidPattern);
//    }
}

void EditBrushWidget::updateButtonIconColor()
{
    QPixmap pixmap(ui->toolButtonBrushColor->width()-2, ui->toolButtonBrushColor->height()-2);
    pixmap.fill(m_brush.color());
    ui->toolButtonBrushColor->setIcon(QIcon(pixmap));
}
