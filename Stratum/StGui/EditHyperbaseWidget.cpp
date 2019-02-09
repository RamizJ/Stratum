#include "EditHyperbaseWidget.h"
#include "ui_EditHyperbaseWidget.h"

EditHyperbaseWidget::EditHyperbaseWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditHyperbaseWidget)
{
    ui->setupUi(this);
}

EditHyperbaseWidget::~EditHyperbaseWidget()
{
    delete ui;
}
