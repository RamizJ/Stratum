#include "MatrixEditor.h"
#include "Matrix.h"

#include <QDialogButtonBox>
#include <QLineEdit>
#include <QTableView>
#include <QVBoxLayout>

namespace StVirtualMachine {

MatrixEditModel::MatrixEditModel(QObject* parent) :
    QAbstractTableModel(parent),
    m_matrix(nullptr)
{}

int MatrixEditModel::rowCount(const QModelIndex& /*parent*/) const
{
    if(!m_matrix)
        return 0;

    return m_matrix->rowCount();
}

int MatrixEditModel::columnCount(const QModelIndex& /*parent*/) const
{
    if(!m_matrix)
        return 0;

    return m_matrix->columnCount();
}

QVariant MatrixEditModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if(!m_matrix || !index.isValid())
        return result;

    if(role == Qt::DisplayRole)
        return m_matrix->getCell(index.row(), index.column());

    return result;
}

bool MatrixEditModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(!m_matrix || !index.isValid())
        return false;

    if(role == Qt::EditRole)
        m_matrix->putCell(index.row(), index.column(), value.toDouble());

    return true;
}

Qt::ItemFlags MatrixEditModel::flags(const QModelIndex& index) const
{
    if(!m_matrix || !index.isValid())
        return QAbstractTableModel::flags(index);

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

void MatrixEditModel::setMatrix(Matrix* matrix)
{
    if(matrix == m_matrix)
        return;

    beginResetModel();
    m_matrix = matrix;
    endResetModel();
}


/*---------------------------------------------------------------------------------------------*/
MatrixEditor::MatrixEditor(QWidget* parent) :
    QDialog(parent, Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint),
    m_sourceMatrix(nullptr),
    m_editedMatrix(nullptr)
{
    setWindowModality(Qt::WindowModal);
    resize(500, 300);

    m_matrixModel = new MatrixEditModel(this);

    QTableView* tableView = new QTableView(this);
    tableView->setModel(m_matrixModel);
    tableView->setItemDelegate(new MatrixEditDelegate(this));

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                       QDialogButtonBox::Cancel, this);

    QVBoxLayout* verticalLayout = new QVBoxLayout(this);
    verticalLayout->addWidget(tableView);
    verticalLayout->addWidget(buttonBox);
    setLayout(verticalLayout);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &MatrixEditor::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &MatrixEditor::reject);
}

MatrixEditor::~MatrixEditor()
{
    if(m_editedMatrix)
        delete m_editedMatrix;
}

void MatrixEditor::setMatrix(Matrix* matrix)
{
    m_sourceMatrix = matrix;
    if(!m_sourceMatrix)
        return;

    if(m_editedMatrix)
        delete m_editedMatrix;

    m_editedMatrix = new Matrix(*matrix);
    m_matrixModel->setMatrix(m_editedMatrix);

    setWindowTitle(QString("Matrix: (%1) [(%2 - %3),(%4 - %5)]")
                   .arg(matrix->handle())
                   .arg(matrix->minRow())
                   .arg(matrix->rowCount())
                   .arg(matrix->minColumn())
                   .arg(matrix->columnCount()));
}

void MatrixEditor::accept()
{
    if(m_sourceMatrix)
        *m_sourceMatrix = *m_editedMatrix;

    QDialog::accept();
}


/*---------------------------------------------------------------------------------------------*/
MatrixEditDelegate::MatrixEditDelegate(QObject* parent) :
    QItemDelegate(parent)
{}

QWidget* MatrixEditDelegate::createEditor(QWidget* parent,
                                          const QStyleOptionViewItem& /*option*/,
                                          const QModelIndex& index) const
{
    if(!index.isValid())
        return nullptr;

    QLineEdit* editor = new QLineEdit(parent);
    editor->setValidator(new QDoubleValidator(editor));
    return editor;
}

void MatrixEditDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if(!index.isValid())
        return;

    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->setText(index.data().toString());
}

void MatrixEditDelegate::setModelData(QWidget* editor,
                                      QAbstractItemModel* model,
                                      const QModelIndex& index) const
{
    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    model->setData(index, lineEdit->text());
}

}
