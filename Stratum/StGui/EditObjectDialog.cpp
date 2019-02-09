#include "EditObjectDialog.h"
#include "ui_EditObjectDialog.h"

#include "ObjectVariablesModel.h"
#include "IconLibraryModel.h"
#include "GuiContext.h"

#include <Class.h>
#include <Object.h>
#include <IconManager.h>
#include <Compiler.h>
#include <CompileError.h>

#include <QDateTime>
#include <QMenu>
#include <QStringListModel>
#include <memory>
#include <QDebug>

using namespace StData;
using namespace StCompiler;

EditObjectDialog::EditObjectDialog(Object* object, QWidget *parent) :
    QDialog(parent, Qt::Window),
    ui(new Ui::EditObjectDialog)
{
    ui->setupUi(this);

    m_variablesModel = new ObjectVariablesModel(this);
    ui->tableViewVariables->setModel(m_variablesModel);
    ui->tableViewVariables->setItemDelegate(new ObjectVarDelegate(this));

    setClassObject(object);

    ui->tableViewVariables->setColumnWidth(ObjectVariablesModel::Name, 150);
    ui->tableViewVariables->setColumnWidth(ObjectVariablesModel::Value, 150);
    ui->tableViewVariables->setColumnWidth(ObjectVariablesModel::DefaultValue, 125);
    ui->tableViewVariables->horizontalHeader()->setSectionResizeMode(ObjectVariablesModel::Type,
                                                                     QHeaderView::ResizeToContents);
    ui->tableViewVariables->horizontalHeader()->setStretchLastSection(true);

    m_labelCreationDateText = ui->labelCreationDate->text();
    m_dialogTitleText = windowTitle();

    IconManager& iconManager = IconManager::instance();

    ui->comboBoxIconLibrary->setModel(new QStringListModel(iconManager.standardIconSets(), this));
    ui->comboBoxIconLibrary->setCurrentText(iconManager.defaultIconSetName());
    m_iconModel = new IconLibraryModel(this);
    m_iconModel->setIconSet(ui->comboBoxIconLibrary->currentText());

    ui->listViewIcons->setModel(m_iconModel);

    connect(ui->comboBoxIconLibrary, SIGNAL(currentTextChanged(QString)), m_iconModel, SLOT(setIconSet(QString)));
}

EditObjectDialog::~EditObjectDialog()
{
    delete ui;
}

void EditObjectDialog::setClassObject(Object* object)
{
    if(m_object == object)
        return;

    m_object = object;
    m_variablesModel->setClassObject(m_object);
    ui->widgetSourceCode->setClass(m_object->cls());

    updateUiVariables();
}

void EditObjectDialog::updateUiVariables()
{
    if(m_object == NULL)
        return;

    Class* cls = m_object->cls();

    QFileInfo fileInfo = cls->fileInfo();

    setWindowTitle(m_dialogTitleText.arg(cls->originalName()));

    ui->lineEditClassName->setText(cls->originalName());
    ui->lineEditClassFileName->setText(fileInfo.absoluteFilePath());
    ui->labelCreationDate->setText(m_labelCreationDateText.arg(fileInfo.created().toString("dd.MM.yyyy hh:mm")));
    ui->plainTextEditClassDescription->setPlainText(cls->description());
}
