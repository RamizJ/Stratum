#include "OutputWidget.h"
#include "ui_OutputWidget.h"

OutputWidget::OutputWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputWidget)
{
    ui->setupUi(this);

    m_outputModel = new OutputListModel(this);
    ui->listViewOutput->setModel(m_outputModel);

    connect(ui->toolButtonClear, SIGNAL(clicked()), m_outputModel, SLOT(clear()));

    connect(ui->toolButtonInfo, SIGNAL(toggled(bool)), m_outputModel, SLOT(setInfoVisibility(bool)));
    connect(ui->toolButtonWarnings, SIGNAL(toggled(bool)), m_outputModel, SLOT(setWarningVisibility(bool)));
    connect(ui->toolButtonErrors, SIGNAL(toggled(bool)), m_outputModel, SLOT(setErrorVisibility(bool)));

    m_outputModel->setInfoVisibility(ui->toolButtonInfo->isChecked());
    m_outputModel->setWarningVisibility(ui->toolButtonWarnings->isChecked());
    m_outputModel->setErrorVisibility(ui->toolButtonErrors->isChecked());
}

OutputWidget::~OutputWidget()
{
    delete ui;
}
