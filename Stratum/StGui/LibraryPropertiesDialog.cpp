#include "LibraryPropertiesDialog.h"
#include "ui_LibraryPropertiesDialog.h"

#include "Library.h"

#include <QFileInfo>
#include <QPushButton>

using namespace StData;

LibraryPropertiesDialog::LibraryPropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LibraryPropertiesDialog)
{
    ui->setupUi(this);

    m_titleText = windowTitle();
    m_labelClassesCountText = ui->labelLibraryClassesCount->text();

    ui->buttonBox->button(QDialogButtonBox::Close)->setText(tr("Закрыть"));
}

LibraryPropertiesDialog::~LibraryPropertiesDialog()
{
    delete ui;
}

void LibraryPropertiesDialog::setLibrary(Library* library)
{
    m_library = library;

    if(m_library)
    {
        ui->lineEditAbsolutePath->setText(m_library->absolutePath());
        ui->lineEditRelatedPath->setText(m_library->relativePath());
        ui->labelLibraryClassesCount->setText(m_labelClassesCountText.arg(m_library->classList().count()));

        setWindowTitle(m_titleText.arg(m_library->fileInfo().baseName()));
    }
}

