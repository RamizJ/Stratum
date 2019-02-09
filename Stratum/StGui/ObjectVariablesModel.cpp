#include "ObjectVariablesModel.h"

#include <Class.h>
#include <ClassVarInfo.h>
#include <ObjectVar.h>
#include <Object.h>
#include <StandardTypes.h>
#include <StDataGlobal.h>

#include <QColor>
#include <QLineEdit>
#include <QPainter>

using namespace StData;

ObjectVariablesModel::ObjectVariablesModel(QObject* parent) :
    QAbstractTableModel(parent),
    m_classObject(NULL)
{
    m_columnNames << "Имя" << "Тип" << "Значение" << "По умолчанию" << "Описание";

    m_boldFont = QFont();
    m_boldFont.setBold(true);
}

ObjectVariablesModel::~ObjectVariablesModel()
{}

Object*ObjectVariablesModel::classObject() const {return m_classObject;}

int ObjectVariablesModel::rowCount(const QModelIndex& /*parent*/) const
{
    return m_varList.count();
}

int ObjectVariablesModel::columnCount(const QModelIndex& /*parent*/) const
{
    return m_columnNames.count();
}

QVariant ObjectVariablesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result;
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        result = m_columnNames[section];
    }
    return result;
}

QVariant ObjectVariablesModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(m_classObject == NULL || !index.isValid())
        return result;

    int row = index.row();
    if(row >= 0 && row < m_varList.count())
    {
        ObjectVar* var = m_varList[row];
        Class* varType = var->info()->varType();
        if(role ==  Qt::DisplayRole)
        {
            switch(index.column())
            {
                case Name:
                    result = var->info()->varName();
                    break;

                case Type:
                    result = varType->originalName();
                    break;

                case Value:
                    if(StandardTypes::isNumeric(var->info()->varType()))
                    {
                        if(StandardTypes::isHandle(varType))
                            result = QString("#%1").arg(var->handleValue());
                        else
                            result = var->toString();
                    }
                    else
                        result = var->toString();
                    break;

                case DefaultValue:
                    result = var->info()->defValue();
                    break;

                case Description:
                    result = var->info()->info();
                    break;
            }
        }
        else if(role == Qt::TextColorRole)
        {
            if(index.column() == Value)
            {
                QString defValue = var->info()->defValue();
                if(StandardTypes::isNumeric(varType))
                {
                    QString numDefData = defValue.isEmpty() ? "0" : defValue;
                    if(var->toString() != numDefData)
                        return QColor(Qt::blue);
                }
                else if(StandardTypes::isString(varType))
                    if(var->toString() != defValue)
                        return QColor(Qt::blue);
            }
        }
        else if(role == Qt::FontRole)
        {
//            if(index.column() == Value && StandardTypes::isHandle(var->varInfo()->varType()))
//                result = m_boldFont;
        }
        else if(role == Qt::DecorationRole && index.column() == Value)
        {
            if(StandardTypes::isColorRef(var->info()->varType()))
            {
                QColor color = intToColor(var->intValue());

                QPixmap pixmap = QPixmap(16, 16);
                pixmap.fill(color);
                QPainter painter(&pixmap);
                painter.setPen(Qt::black);
                painter.setBrush(Qt::NoBrush);
                painter.drawRect(0,0, 15,15);

                return QIcon(pixmap);
            }
        }
    }
    return result;
}

Qt::ItemFlags ObjectVariablesModel::flags(const QModelIndex& index) const
{
    return (Qt::ItemIsEditable | QAbstractTableModel::flags(index));
}

void ObjectVariablesModel::setClassObject(Object* object)
{
    beginResetModel();
    {
        m_varList.clear();

        m_classObject = object;
        if(m_classObject)
        {
            for(ObjectVar* var : m_classObject->variables())
                if(!(var->info()->flags() & ClassVarInfo::Local))
                    m_varList << var;
        }
    }
    endResetModel();
}


/*------------------------------------------------------------------------------------------------------*/
ObjectVarDelegate::ObjectVarDelegate(QObject* parent) :
    QItemDelegate(parent)
{}

QWidget* ObjectVarDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& /*option*/,
                                         const QModelIndex& /*index*/) const
{
    QLineEdit* lineEdit = new QLineEdit(parent);
    lineEdit->setReadOnly(true);
    return lineEdit;
}

void ObjectVarDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if(QLineEdit* lineEdit = dynamic_cast<QLineEdit*>(editor))
        lineEdit->setText(index.data().toString());
}
