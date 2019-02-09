#include "LibrariesModel.h"

#include "TreeNode.h"

#include <Project.h>
#include <Library.h>
#include <Class.h>
#include <Icon.h>
#include <LibraryCollection.h>
#include <IconManager.h>

#include <QDir>
#include <QFileInfo>
#include <QIcon>

using namespace StData;

LibrariesModel::LibrariesModel(QObject *parent) :
    QAbstractItemModel(parent),
    m_systemSectionNode(nullptr),
    m_projectSectionNode(nullptr),
    m_extraSectionNode(nullptr),
    m_isEmptyLibraryHidden(false),
    m_project(nullptr)
{
    m_rootNode= new TreeNode();
}

LibrariesModel::~LibrariesModel()
{
    if(m_rootNode)
        delete m_rootNode;
}

QModelIndex LibrariesModel::index(int row, int column, const QModelIndex& parent) const
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

QModelIndex LibrariesModel::parent(const QModelIndex& child) const
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

int LibrariesModel::rowCount(const QModelIndex& parent) const
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

int LibrariesModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

QVariant LibrariesModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if(index.isValid())
    {
        TreeNode *node = static_cast<TreeNode*>(index.internalPointer());
        UiTreeNode* sectionNode = dynamic_cast<UiTreeNode*>(node);
        LibraryNode* libraryNode = dynamic_cast<LibraryNode*>(node);
        ClassNode* classNode = dynamic_cast<ClassNode*>(node);

        if(role == Qt::DisplayRole)
        {
            if(libraryNode)
                result = libraryNode->data()->relativePath();
            else if(classNode)
                //                result = QString(tr("имя файла: %1")).arg(QFileInfo(cls->fileName()).fileName());
                result = classNode->data()->originalName();
            else if(sectionNode)
                result = sectionNode->text();
        }
        else if(role == Qt::DecorationRole)
        {
            if(classNode)
                result = getIcon(classNode->data());
            else
                result = QIcon("://LibraryFolder.png");
        }
        else if(role == Qt::TextColorRole)
        {
            if(libraryNode)
                result = QDir(libraryNode->data()->absolutePath()).exists() ?
                            QColor(Qt::black) :
                            QColor(Qt::gray);
        }
        else if(role == Qt::ToolTipRole)
        {
            if(libraryNode)
            {
                QString path = libraryNode->data()->absolutePath();
                result = QDir(path).exists() ?
                            path :
                            QString(tr("Библиотека не найдена: %1")).arg(path);
            }

            else if(classNode)
                result = classNode->data()->fileInfo().absoluteFilePath();
        }
    }
    return result;
}

Qt::ItemFlags LibrariesModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QModelIndex LibrariesModel::findIndex(TreeNode* treeNode, const QModelIndex& parentIndex)
{
    QModelIndex result;
    for(int i = 0; i < rowCount(parentIndex); i++)
    {
        QModelIndex currentIndex = index(i, 0, parentIndex);
        TreeNode* currentTreeNode = static_cast<TreeNode*>(currentIndex.internalPointer());
        if(treeNode == currentTreeNode)
            return currentIndex;

        result = findIndex(treeNode, currentIndex);
        if(result != QModelIndex())
            return result;
    }
    return result;
}

QModelIndex LibrariesModel::systemSectionIndex()
{
    return findIndex(m_systemSectionNode);
}

QModelIndex LibrariesModel::projectSectionIndex()
{
    return findIndex(m_projectSectionNode);
}

QModelIndex LibrariesModel::extraSectionIndex()
{
    return findIndex(m_extraSectionNode);
}

void LibrariesModel::setProject(Project* project)
{
    if(m_project == project)
        return;

    m_project = project;
    setupHierarchy();
}

void LibrariesModel::addLibrary(const QString& libraryPath, bool isRecursive)
{
    if(!m_project)
        return;

    beginResetModel();

    QList<ExtraLibrary*> libraries = ExtraLibrary::loadLibraries(m_project, libraryPath, isRecursive);
    for(Library* library : libraries)
        LibraryCollection::currentCollection()->addLibrary(library);

    setupHierarchy();

    endResetModel();
}

void LibrariesModel::removeItem(const QModelIndex& index)
{
    if(!index.isValid() || !index.parent().isValid() || !m_project)
        return;

    TreeNode* treeNode = static_cast<TreeNode*>(index.internalPointer());
    LibraryNode* libraryNode = dynamic_cast<LibraryNode*>(treeNode);
    ClassNode* classNode = dynamic_cast<ClassNode*>(treeNode);

    bool isSystemLibrary = (dynamic_cast<SystemLibrary*>(libraryNode->data()) != nullptr);
    bool isExtraLibrary = (dynamic_cast<ExtraLibrary*>(libraryNode->data()) != nullptr);

    if(isExtraLibrary)
    {
        beginRemoveRows(index.parent(), index.row(), index.row());
        {
            LibraryCollection::currentCollection()->removeLibrary(libraryNode->data());
            delete libraryNode;
        }
        endRemoveRows();
    }
    else if(classNode && !isSystemLibrary)
    {
        //        beginRemoveRows(index.parent(), index.row(), index.row());
        //        m_project->removeClassInfo(classInfo);
        //        endRemoveRows();
    }
}

void LibrariesModel::setIsEmptyLibrariesHidden(bool isHidden)
{
    if(m_isEmptyLibraryHidden == isHidden)
        return;

    m_isEmptyLibraryHidden = isHidden;
    setupHierarchy();
}

void LibrariesModel::setFilter(const QString& filter)
{
    if(m_filter == filter)
        return;

    m_filter = filter;
    setupHierarchy();
}

void LibrariesModel::setupHierarchy()
{
    beginResetModel();
    {
        if(m_rootNode)
        {
            delete m_rootNode;
            m_rootNode = nullptr;
        }

        if(m_project)
        {
            m_rootNode = new TreeNode();
            m_systemSectionNode = new UiTreeNode(m_rootNode, LibrariesModel::systemSectionName());
            m_projectSectionNode = new UiTreeNode(m_rootNode, LibrariesModel::projectSectionName());
            m_extraSectionNode = new UiTreeNode(m_rootNode, LibrariesModel::additionalSectionName());

            auto createLibraryNode = [this](TreeNode* parentNode, Library* library)
            {
                if(m_isEmptyLibraryHidden && library->classList().isEmpty())
                    return;

                LibraryNode* treeNode = new LibraryNode(parentNode, library);
                addClasses(library, treeNode);

                if(!m_filter.isEmpty() && treeNode->childs().isEmpty())
                    delete treeNode;
            };

            for(Library* library : LibraryCollection::currentCollection()->libraries())
            {
                if(dynamic_cast<SystemLibrary*>(library))
                    createLibraryNode(m_systemSectionNode, library);

                if(dynamic_cast<ExtraLibrary*>(library))
                    createLibraryNode(m_extraSectionNode, library);

                if(dynamic_cast<ProjectLibrary*>(library))
                    addClasses(library, m_projectSectionNode);
            }
        }
    }
    endResetModel();
}

void LibrariesModel::addClasses(Library* library, TreeNode* parentNode)
{
    if(library == NULL || parentNode == NULL)
        return;

    for(Class* cls : library->classList())
    {
        bool isFilterPassed = m_filter.isEmpty() ||
                cls->fileInfo().baseName().toLower().contains(m_filter.toLower()) ||
                cls->originalName().toLower().contains(m_filter.toLower());

        if(isFilterPassed)
            new ClassNode(parentNode, cls);
    }
}

QIcon LibrariesModel::getIcon(Class* cls) const
{
    QIcon icon = (cls->icon() != nullptr) ?
                QIcon(QPixmap::fromImage(cls->icon()->image())) :
                IconManager::instance().iconByIndex(cls->iconFileName(), cls->defIcon());

    return icon;
}
