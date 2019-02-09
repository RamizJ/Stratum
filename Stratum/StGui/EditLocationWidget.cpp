#include "EditLocationWidget.h"
#include "ui_EditLocationWidget.h"

EditLocationWidget::EditLocationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SpaceItemLocationWidget)
{
    ui->setupUi(this);
}

EditLocationWidget::~EditLocationWidget()
{
    delete ui;
}

QString EditLocationWidget::spaceItemName() const
{
    return ui->lineEditName->text();
}

void EditLocationWidget::setSpaceItemName(const QString& name)
{
    ui->lineEditName->setText(name);
}

QSizeF EditLocationWidget::spaceItemSize() const
{
    return QSizeF(ui->doubleSpinBoxWidth->value(), ui->doubleSpinBoxHeight->value());
}

void EditLocationWidget::setSpaceItemSize(const QSizeF& size)
{
    ui->doubleSpinBoxWidth->setValue(size.width());
    ui->doubleSpinBoxHeight->setValue(size.height());
}

QPointF EditLocationWidget::spaceItemPosition() const
{
    return QPointF(ui->doubleSpinBoxX->value(), ui->doubleSpinBoxY->value());
}

void EditLocationWidget::setSpaceItemPosition(const QPointF& pos)
{
    ui->doubleSpinBoxX->setValue(pos.x());
    ui->doubleSpinBoxY->setValue(pos.y());
}

double EditLocationWidget::spaceItemAngle() const
{
    return ui->doubleSpinBoxAngle->value();
}

void EditLocationWidget::setSpaceItemAngle(double angle)
{
    ui->doubleSpinBoxAngle->setValue(angle);
}

quint8 EditLocationWidget::spaceItemLayer() const
{
    return ui->comboBoxLayer->currentText().toInt();
}

void EditLocationWidget::setSpaceItemLayer(quint8 layer)
{
    ui->comboBoxLayer->setCurrentText(QString::number(layer));
}

bool EditLocationWidget::spaceItemIsSelectable() const
{
    return ui->checkBoxSelectable->isChecked();
}

void EditLocationWidget::setSpaceItemIsSelectable(bool isSelectable)
{
    ui->checkBoxSelectable->setChecked(isSelectable);
}

