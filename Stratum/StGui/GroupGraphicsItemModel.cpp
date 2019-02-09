#include "GroupGraphicsItemModel.h"

#include <SpaceItem.h>

using namespace StData;
//using namespace StSpace;

GroupGraphicsItemModel::GroupGraphicsItemModel(QObject* parent) :
    QAbstractItemModel(parent),
    m_groupItem(nullptr)
{}

GroupGraphicsItemModel::~GroupGraphicsItemModel()
{}

void GroupGraphicsItemModel::setGroupItem(GroupItem* groupItem)
{
    beginResetModel();
    {
        m_groupItem = groupItem;
    }
    endResetModel();
}

QModelIndex GroupGraphicsItemModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    SpaceItem* parentItem = parent.isValid() ?
                                static_cast<SpaceItem*>(parent.internalPointer()) :
                                m_groupItem;

    if(GroupItem* parentGroup = dynamic_cast<GroupItem*>(parentItem))
    {
        if(SpaceItem* childItem = parentGroup->itemAt(row))
            return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex GroupGraphicsItemModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    SpaceItem* childItem = static_cast<SpaceItem*>(child.internalPointer());
    GroupItem* parentItem = childItem->ownerItem();

    if(parentItem == m_groupItem)
        return QModelIndex();

    int row = parentItem->ownerItem()->indexOf(parentItem);
    return createIndex(row, 0, parentItem);
}

int GroupGraphicsItemModel::rowCount(const QModelIndex& parent) const
{
    if (!m_groupItem || parent.column() > 0)
        return 0;

    SpaceItem* parentItem = parent.isValid() ?
                                static_cast<SpaceItem*>(parent.internalPointer()) :
                                m_groupItem;

    if(GroupItem* parentGroup = dynamic_cast<GroupItem*>(parentItem))
        return parentGroup->itemsCount();

    return 0;
}

int GroupGraphicsItemModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

QVariant GroupGraphicsItemModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if(index.isValid())
    {
        SpaceItem* spaceItem = static_cast<SpaceItem*>(index.internalPointer());
        if(role == Qt::DisplayRole)
        {
            if(dynamic_cast<GroupItem*>(spaceItem))
                result = tr("Группа двумерных объектов.");

            else if(dynamic_cast<TextureItem2d*>(spaceItem))
                result = tr("Двумерная битовая карта.");

            else if(dynamic_cast<PolylineItem2d*>(spaceItem))
                result = tr("Двумерная полилиния.");

            else if(dynamic_cast<TextItem2d*>(spaceItem))
                result = tr("Двумерный текст.");

            else if(dynamic_cast<TextItem2d*>(spaceItem))
                result = tr("Неизвестный объект.");

            if(spaceItem)
                result = QString("%1 #%2 %3").arg(result.toString()).arg(spaceItem->handle()).arg(spaceItem->name());
        }
        else if(role == Qt::DecorationRole)
        {
            if(dynamic_cast<GroupItem*>(spaceItem))
                result = QIcon("://16/Group.png");

            else if(dynamic_cast<TextureItem2d*>(spaceItem))
                result = QIcon("://16/Image.png");

            else if(dynamic_cast<PolylineItem2d*>(spaceItem))
                result = QIcon("://16/Polyline.png");

            else if(dynamic_cast<TextItem2d*>(spaceItem))
                result = QIcon("://16/Text.png");
        }
    }
    return result;
}
