#ifndef LIBRARIESMODEL_H
#define LIBRARIESMODEL_H

#include "TreeNode.h"

#include <memory>
#include <QAbstractItemModel>

namespace StData {
class Library;
class Project;
class Class;
}

class TreeNode;
class UiTreeNode;
typedef DataTreeNode<StData::Class*> ClassNode;
typedef DataTreeNode<StData::Library*> LibraryNode;

class LibrariesModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    static QString systemSectionName() {return "Стандартные библиотеки";}
    static QString projectSectionName() {return "Проект";}
    static QString additionalSectionName() {return "Библиотеки проекта";}

    explicit LibrariesModel(QObject *parent = 0);
    ~LibrariesModel();

    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex& child) const;
    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;

    QModelIndex findIndex(TreeNode* treeNode, const QModelIndex& parentIndex = QModelIndex());
    QModelIndex systemSectionIndex();
    QModelIndex projectSectionIndex();
    QModelIndex extraSectionIndex();

    StData::Project* project() {return m_project;}
    void setProject(StData::Project* project);

    void addLibrary(const QString& libraryPath, bool isRecursive);
    void removeItem(const QModelIndex& index);

    void setIsEmptyLibrariesHidden(bool isHidden);
    void setFilter(const QString& filter);

private:
    void setupHierarchy();
    void addClasses(StData::Library* library, TreeNode* parentNode);
    QIcon getIcon(StData::Class* cls) const;

private:
    StData::Project* m_project;

    TreeNode* m_rootNode;
    UiTreeNode* m_systemSectionNode;
    UiTreeNode* m_projectSectionNode;
    UiTreeNode* m_extraSectionNode;

    bool m_isEmptyLibraryHidden;
    QString m_filter;
};

#endif // LIBRARIESMODEL_H
