#ifndef PROJECTHIERARCHYWIDGET_H
#define PROJECTHIERARCHYWIDGET_H

#include <QMenu>
#include <QWidget>
#include <memory>

namespace Ui {
class ProjectHierarchyWidget;
}

namespace StData {
class Project;
class Class;
}

class EditObjectDialog;
class ObjectHierarchyModel;
class TabManager;

class ProjectHierarchyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectHierarchyWidget(QWidget *parent = 0);
    ~ProjectHierarchyWidget();

    void setProject(StData::Project* project);

private slots:
    void contextMenuRequested(QPoint point);
    void contextMenuRequested(const QModelIndex& index);
    void updateActionsState();
    void setFilter();

    void editClassSchema();
    void onEditClassSourceCode();
    void onEditObjectProperties();

private:
    Ui::ProjectHierarchyWidget *ui;
    QMenu* m_contextMenu;

    StData::Project* m_project;
    ObjectHierarchyModel* m_model;
};

#endif // PROJECTHIERARCHYWIDGET_H
