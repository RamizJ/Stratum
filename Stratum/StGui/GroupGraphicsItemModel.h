#ifndef GROUPGRAPHICSITEMMODEL_H
#define GROUPGRAPHICSITEMMODEL_H

#include "TreeNode.h"

#include <QAbstractItemModel>

namespace StData {
class GroupItem;
}

class GroupGraphicsItemModel : public QAbstractItemModel
{
public:
    GroupGraphicsItemModel(QObject* parent = 0);
    ~GroupGraphicsItemModel();

    StData::GroupItem* groupItem() const {return m_groupItem;}
    void setGroupItem(StData::GroupItem* groupItem);

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex& child) const;
    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;

private:
    StData::GroupItem* m_groupItem;
};

#endif // GROUPGRAPHICSITEMMODEL_H
