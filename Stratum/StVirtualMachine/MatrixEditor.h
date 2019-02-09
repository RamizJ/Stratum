#ifndef MATRIXEDITOR_H
#define MATRIXEDITOR_H

#include <QAbstractTableModel>
#include <QDialog>
#include <QItemDelegate>

namespace StVirtualMachine {

class Matrix;

class MatrixEditModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MatrixEditModel(QObject* parent = 0);

    Matrix* matrix() const { return m_matrix; }
    void setMatrix(Matrix* matrix);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex&) const;
    int columnCount(const QModelIndex&) const;
    QVariant data(const QModelIndex& index, int role) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role);
    Qt::ItemFlags flags(const QModelIndex& index) const;

private:
    Matrix* m_matrix;
};


/*---------------------------------------------------------------------------------------------*/
class MatrixEditor : public QDialog
{
    Q_OBJECT
public:
    MatrixEditor(QWidget* parent = 0);
    ~MatrixEditor();

    void setMatrix(Matrix* matrix);

    // QDialog interface
public slots:
    void accept();

private:
    MatrixEditModel* m_matrixModel;
    Matrix* m_sourceMatrix;
    Matrix* m_editedMatrix;
};


/*---------------------------------------------------------------------------------------------*/
class MatrixEditDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit MatrixEditDelegate(QObject* parent = 0);
    // QAbstractItemDelegate interface
public:
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
};

}

#endif // MATRIXEDITMODEL_H
