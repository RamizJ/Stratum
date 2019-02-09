#ifndef OBJECTVARIABLESMODEL_H
#define OBJECTVARIABLESMODEL_H

#include <QAbstractTableModel>
#include <QItemDelegate>
#include <qfont.h>

namespace StData {
class Object;
class ClassVarInfo;
class ObjectVar;
}

class ObjectVariablesModel : public QAbstractTableModel
{
public:
    enum ColumnType {Name, Type, Value, DefaultValue, Description};

public:
    ObjectVariablesModel(QObject* parent = 0);
    ~ObjectVariablesModel();

    StData::Object* classObject() const;
    void setClassObject(StData::Object* object);

public:
    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex& index, int role) const;

    Qt::ItemFlags flags(const QModelIndex&index) const;

private:
    StData::Object* m_classObject;
    QList<StData::ObjectVar*> m_varList;

    QStringList m_columnNames;
    QFont m_boldFont;
};

class ObjectVarDelegate : public QItemDelegate
{
public:
    ObjectVarDelegate(QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex&) const;
    void setEditorData(QWidget* editor, const QModelIndex& index) const;
//    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // OBJECTVARIABLESMODEL_H
