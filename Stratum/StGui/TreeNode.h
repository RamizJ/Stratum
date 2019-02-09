#ifndef TREENODE
#define TREENODE

#include <QList>
#include <QIcon>
#include <memory>

#include <QDebug>

using namespace std;

class TreeNode
{
public:
    TreeNode(TreeNode* parent = nullptr) :
        m_parent(nullptr)
    {
        setParent(parent);
    }

    virtual ~TreeNode()
    {
        qDeleteAll(m_childs);
    }

    TreeNode* parent() const {return m_parent;}
    void setParent(TreeNode* newParent)
    {
        if(m_parent == newParent)
            return;

        if(m_parent)
            m_parent->removeChild(this);

        newParent->addChild(this);
        m_parent = newParent;
    }

    QList<TreeNode*> childs() const {return m_childs; }

    void addChild(TreeNode* childNode)
    {
        if(!m_childs.contains(childNode))
        {
            m_childs << childNode;
            childNode->setParent(this);
        }
    }

    void removeChild(TreeNode* node) {m_childs.removeOne(node);}

private:
    TreeNode* m_parent;
    QList<TreeNode*> m_childs;
};

class UiTreeNode : public TreeNode
{
public:
    UiTreeNode(TreeNode* parent = nullptr, QString text = "", QIcon icon = QIcon()) :
        TreeNode(parent),
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

template<class T>
class DataTreeNode : public TreeNode
{
public:
    DataTreeNode(TreeNode* parent, T data) :
        TreeNode(parent),
        m_data(data)
    {}

    ~DataTreeNode()
    {}

    T data() const {return m_data;}
    void setData(const T& data) {m_data = data;}

private:
    T m_data;
};

#endif // TREENODE
