#include "EditPenWidget.h"
#include "ui_EditPenWidget.h"

#include <QStringListModel>
#include <QColorDialog>

EditPenWidget::EditPenWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditPenWidget)
{
    ui->setupUi(this);

    QStringList penStyleList;
    penStyleList << "NoPen" << "Solid" << "Dash" << "Dot" << "DashDot" << "DashDotDot";
    ui->comboBoxPenStyle->setModel(new QStringListModel(penStyleList, this));

    {
        connect(ui->comboBoxPenStyle, SIGNAL(currentIndexChanged(int)), SLOT(onPenStyleChanged(int)));
        connect(ui->toolButtonPenColor, SIGNAL(clicked()), SLOT(onChoosePenColor()));
        connect(ui->spinBoxPenWidth, SIGNAL(valueChanged(int)), SLOT(onPenWidthChanged(int)));
    }
}

EditPenWidget::~EditPenWidget()
{
    delete ui;
}

QPen EditPenWidget::pen() const
{
    return m_pen;
}

void EditPenWidget::setPen(const QPen& pen)
{
    m_pen = pen;

    ui->comboBoxPenStyle->setCurrentIndex(pen.style());

    QPixmap pixmap(ui->toolButtonPenColor->width()-2, ui->toolButtonPenColor->height()-2);
    pixmap.fill(m_pen.color());
    ui->toolButtonPenColor->setIcon(QIcon(pixmap));

    ui->spinBoxPenWidth->setValue(m_pen.width());
}

void EditPenWidget::onChoosePenColor()
{
    QColor color = QColorDialog::getColor(m_pen.color(), this, tr("Цвет кисти"), QColorDialog::ShowAlphaChannel);
    if(color.isValid())
    {
        m_pen.setColor(color);

        QPixmap pixmap(ui->toolButtonPenColor->width()-2, ui->toolButtonPenColor->height()-2);
        pixmap.fill(m_pen.color());
        ui->toolButtonPenColor->setIcon(QIcon(pixmap));
    }
}

void EditPenWidget::onPenStyleChanged(int styleIndex)
{
    m_pen.setStyle(static_cast<Qt::PenStyle>(styleIndex));
}

void EditPenWidget::onPenWidthChanged(int penWidth)
{
    m_pen.setWidthF(penWidth);
}

