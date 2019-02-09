#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QAction>
#include <QMap>
#include <QObject>
#include <QTabWidget>

namespace StData {
class Class;
class Object;
}

class SpaceContextMenu;
class SpaceEditWidget;

class TabManager : public QObject
{
    Q_OBJECT
public:
    explicit TabManager(QTabWidget* tabWidget, QObject *parent = 0);
    ~TabManager();

    QTabWidget* tabWidget() const {return m_tabWidget;}
    void setTabWidget(QTabWidget* tabWidget) {m_tabWidget = tabWidget;}

    void openSpace(StData::Object* cls);
    void openSourceCode(StData::Class* cls);

public slots:
    void closeAllTabs();
    void tabClosing(int index);
    void closeCurrentTab();

private:
    QWidget* createWidget();

private:
    QTabWidget* m_tabWidget;
    QMap<QString, SpaceEditWidget*> m_spaceWidgetForClassName;
    QMap<QString, QWidget*> m_textWidgetForClassName;

    QAction* m_closeTabAction;

//    SpaceContextMenu* m_spaceSceneManager;
};

#endif // TABMANAGER_H
