#ifndef SPACEWIDGETITEM_H
#define SPACEWIDGETITEM_H

#include <HandleItem.h>
#include <QObject>
#include <QDir>
#include <QGraphicsProxyWidget>
#include <QWidget>

namespace StData {
class Project;
class Scheme;
class Space;
class Class;
}

namespace StSpace {
class SpaceWidget;
}

namespace StVirtualMachine {

class SpaceWindowItem : public QObject, public StData::HandleItem
{
    Q_OBJECT
public:
    SpaceWindowItem(const QString& windowName, SpaceWindowItem* parentItem,
                    StData::Space* space, const QString& style,
                    const QPoint& pos = QPoint(), const QSize& size = QSize(),
                    int handle = 0);
    ~SpaceWindowItem();

    QList<SpaceWindowItem*> childItems() const;

    StSpace::SpaceWidget* spaceWidget() const { return m_spaceWidget; }
    QString windowName() const { return m_windowName; }

    StData::Space* space() const { return m_space; }
    void setSpace(StData::Space* space);

    StData::Class* cls() const { return m_class; }
    void setClass(StData::Class* cls) { m_class = cls; }

    StData::Project* project() const { return m_project; }
    void setProject(StData::Project* project) { m_project = project; }

    void addChildWindow(SpaceWindowItem* childItem);
    bool isChildWindow() const;

    SpaceWindowItem* parentItem() const { return m_parentItem; }
    void setParentItem(SpaceWindowItem* parentItem);

    QGraphicsProxyWidget* proxyWidget() const { return m_proxyWidget; }
    void setProxyWidget(QGraphicsProxyWidget* proxyWidget) { m_proxyWidget = proxyWidget; }

    void move(int x, int y);
    void move(const QPoint& pos);
    void resize(int w, int h);
    void resize(const QSizeF& size);

    void setSpaceScale(double scale);


signals:
    void windowClosed();
    void windowDestroyed();

private slots:
    void removeChildItem();

private:
    void autoResize(int w, int h);

private:
    QString m_windowName;
    StData::Space* m_space;
    StData::Class* m_class;
    StData::Project* m_project;

    StSpace::SpaceWidget* m_spaceWidget;
    QGraphicsProxyWidget* m_proxyWidget;

    SpaceWindowItem* m_parentItem;
    QList<SpaceWindowItem*> m_childItems;

    bool m_isChildWindow;
    bool m_isAutoResized;
};

}

#endif // WINDOWITEM_H
