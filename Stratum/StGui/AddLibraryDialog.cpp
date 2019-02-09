#include "AddLibraryDialog.h"
#include "ui_AddLibraryDialog.h"

#include <Project.h>
#include <Library.h>

#include <QFileDialog>
#include <QPushButton>

using namespace StData;

AddLibraryDialog::AddLibraryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddLibraryDialog),
    m_project(NULL)
{
    ui->setupUi(this);

    m_projectPathText = ui->labelProjectFolder->text();

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Добавить"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Отменить"));

    connect(ui->toolButtonChooseDirectory, SIGNAL(clicked()), this, SLOT(chooseDirectory()));
    connect(ui->lineEditDirectory, SIGNAL(textChanged(QString)), this, SLOT(validateSelectedPath()));
    connect(ui->checkBoxIsRelative, SIGNAL(toggled(bool)), this, SLOT(pathIsRelativeToggled(bool)));

    validateSelectedPath();
}

AddLibraryDialog::~AddLibraryDialog()
{
    delete ui;
}

void AddLibraryDialog::setProject(Project* project)
{
    m_project = project;
    if(m_project)
        ui->labelProjectFolder->setText(m_projectPathText.arg(m_project->fileInfo().absolutePath()));
}

QString AddLibraryDialog::selectedPath()
{
    return ui->lineEditDirectory->text();
}

bool AddLibraryDialog::isRecursive()
{
    return ui->checkBoxIsRecursive->isChecked();
}

void AddLibraryDialog::chooseDirectory()
{
    if(!m_project)
        return;

    QString directoryPath = QFileDialog::getExistingDirectory(this, tr("Выберите папку библиотеки"), m_project->fileInfo().absolutePath());
    if(ui->checkBoxIsRelative->isChecked())
    {
        QString cleanPath = QDir::cleanPath(directoryPath.simplified());
        QString relativePath = m_project->fileInfo().absoluteDir().relativeFilePath(cleanPath);

        ui->lineEditDirectory->setText(relativePath);
    }
    else
        ui->lineEditDirectory->setText(directoryPath);
}

void AddLibraryDialog::validateSelectedPath()
{
    if(m_project)
    {
        //TODO вставка нескольких библиотек

        ExtraLibrary extraLibrary(m_project);
        extraLibrary.setPath(ui->lineEditDirectory->text());

        bool okEnabled = !ui->lineEditDirectory->text().isEmpty() && extraLibrary.libraryDir().exists();
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(okEnabled);
    }
    else
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

void AddLibraryDialog::pathIsRelativeToggled(bool isRelative)
{
    if(m_project && !ui->lineEditDirectory->text().isEmpty())
    {
        ExtraLibrary extraLibrary(m_project);
        extraLibrary.setPath(ui->lineEditDirectory->text());
        ui->lineEditDirectory->setText(isRelative ? extraLibrary.relativePath() : extraLibrary.absolutePath());
    }
}
