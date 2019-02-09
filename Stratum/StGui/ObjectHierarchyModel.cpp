#include "ObjectHierarchyModel.h"

#include <Project.h>
#include <Class.h>
#include <ObjectInfo.h>
#include <Object.h>
#include <Icon.h>
#include <IconManager.h>

using namespace StData;

ObjectHierarchyModel::ObjectHierarchyModel(QObject* parent) :
    QAbstractItemModel(parent),
    m_project(NULL)
{
    m_rootNode = new TreeNode();
    m_defClassIcon = QIcon("://Class.png");
}

ObjectHierarchyModel::~ObjectHierarchyModel()
{}

void ObjectHierarchyModel::setProject(Project* project)
{
    if(m_project == project)
        return;

    m_project = project;
    setupHierarchy();
}

QModelIndex ObjectHierarchyModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeNode* parentNode;

    if (!parent.isValid())
        parentNode = m_rootNode;
    else
        parentNode = static_cast<TreeNode*>(parent.internalPointer());

    TreeNode* childNode = parentNode->childs().at(row);
    if (childNode)
        return createIndex(row, column, childNode);
    else
        return QModelIndex();
}

QModelIndex ObjectHierarchyModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    TreeNode* childNode = static_cast<TreeNode*>(child.internalPointer());
    TreeNode* parentNode = childNode->parent();

    if (parentNode == m_rootNode)
        return QModelIndex();

    int row = parentNode->parent()->childs().indexOf(parentNode);
    return createIndex(row, 0, parentNode);
}

int ObjectHierarchyModel::rowCount(const QModelIndex& parent) const
{
    TreeNode* parentNode;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentNode = m_rootNode;
    else
        parentNode = static_cast<TreeNode*>(parent.internalPointer());

    return parentNode == nullptr ? 0 : parentNode->childs().count();
}

int ObjectHierarchyModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

QVariant ObjectHierarchyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result;
    if(orientation == Qt::Horizontal && section == 0)
    {
        if(role == Qt::DisplayRole)
            return tr("Иерархия классов");
    }
    return result;
}

QVariant ObjectHierarchyModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if(index.isValid())
    {
        TreeNode* node = static_cast<TreeNode*>(index.internalPointer());

        if(UiClassObjectNode* classObjectNode = dynamic_cast<UiClassObjectNode*>(node))
        {
            if(!classObjectNode->data()->cls())
                return result;

            Object* object = classObjectNode->data();
            switch(role)
            {
                case Qt::DisplayRole:
                    result = object->cls()->originalName();
                    break;

                case Qt::DecorationRole:
                    result = classObjectNode->icon();
                    break;
            }
        }
    }
    return result;
}

void ObjectHierarchyModel::setFilter(const QString& filter)
{
    if(m_filter == filter)
        return;

    m_filter = filter;
    setupHierarchy();
}

void ObjectHierarchyModel::setupHierarchy()
{
    beginResetModel();
    {
        if(m_rootNode)
        {
            delete m_rootNode;
            m_rootNode = nullptr;
        }

        if(m_project != nullptr)
        {
            m_rootNode = new TreeNode();

            Object* rootObject = m_project->rootObject();
            if(rootObject && rootObject->cls())
            {
                TreeNode* treeNode = m_rootNode;
                Class* cls = rootObject->cls();
                if(cls->cleanName().contains(m_filter.toLower()) || m_filter.isEmpty())
                    treeNode = new UiClassObjectNode(m_rootNode, cls->originalName(),
                                                     getIcon(cls), rootObject);

                setupObjectHierarchy(rootObject, treeNode);
            }
        }
    }
    endResetModel();
}

void ObjectHierarchyModel::setupObjectHierarchy(Object* parentObject, TreeNode* parentNode)
{
    for(Object* childObject : parentObject->childObjects())
    {
        if(childObject && childObject->cls())
        {
            Class* cls = childObject->cls();
            TreeNode* treeNode = m_rootNode;

            if(m_filter.isEmpty() || cls->originalName().toLower().contains(m_filter.toLower()))
                treeNode = new UiClassObjectNode(parentNode, cls->originalName(), getIcon(cls), childObject);

            setupObjectHierarchy(childObject, treeNode);
        }
    }
}

QIcon ObjectHierarchyModel::getIcon(Class* cls)
{
    QIcon icon = cls->icon() ?
                QIcon(QPixmap::fromImage(cls->icon()->image())) :
                IconManager::instance().iconByIndex(cls->iconFileName(), cls->defIcon());

    return icon;
}
