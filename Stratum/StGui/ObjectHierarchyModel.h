#ifndef OBJECTHIERARCHYMODEL_H
#define OBJECTHIERARCHYMODEL_H

#include <QAbstractItemModel>
#include <QIcon>
#include <memory>
#include <TreeNode.h>

namespace StData {
class Project;
class Object;
class Class;
}

class ObjectHierarchyModel : public QAbstractItemModel
{
public:
    ObjectHierarchyModel(QObject* parent = 0);
    ~ObjectHierarchyModel();

    StData::Project* project() const {return m_project;}
    void setProject(StData::Project* project);

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex& child) const;
    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex&parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex& index, int role) const;

public slots:
    void setFilter(const QString& filter);

private:
    void setupHierarchy();
//    void setupClassHierarchy(StCore::Class* parentClass, TreeItem* parentItem);
    void setupObjectHierarchy(StData::Object* parentObject, TreeNode* parentItem);
    QIcon getIcon(StData::Class* cls);

private:
    StData::Project* m_project;

    TreeNode* m_rootNode;
    QString m_filter;
    QIcon m_defClassIcon;
};

class UiClassObjectNode : public DataTreeNode<StData::Object*>
{
public:
    UiClassObjectNode(TreeNode* parent = nullptr, const QString& text = "",
                const QIcon icon = QIcon(), StData::Object* classObject = nullptr) :
        DataTreeNode(parent, classObject),
        m_text(text),
        m_icon(icon)
    {}

    QString text() const {return m_text;}
    void setText(const QString& text) {m_text = text;}

    QIcon icon() const {return m_icon;}
    void setIcon(const QIcon& icon) {m_icon = icon;}

private:
    QString m_text;
    QIcon m_icon;
};

#endif // OBJECTHIERARCHYMODEL_H
