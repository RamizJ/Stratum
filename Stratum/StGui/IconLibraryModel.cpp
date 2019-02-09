#include "IconLibraryModel.h"

#include <IconManager.h>
#include <QIcon>

using namespace StData;

IconLibraryModel::IconLibraryModel(QObject* parent) :
    QAbstractListModel(parent)
{}

IconLibraryModel::~IconLibraryModel()
{}

void IconLibraryModel::setIconSet(const QString& iconSetName)
{
    beginResetModel();
    {
        m_currentIconList = IconManager::instance().iconListByName(iconSetName);
    }
    endResetModel();
}

int IconLibraryModel::rowCount(const QModelIndex& /*parent*/) const
{
    return m_currentIconList.count();
}

QVariant IconLibraryModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if(index.isValid() && role == Qt::DecorationRole)
        result = m_currentIconList[index.row()];

    return result;
}

