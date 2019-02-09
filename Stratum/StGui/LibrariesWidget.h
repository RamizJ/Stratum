#ifndef LIBRARIESWIDGET_H
#define LIBRARIESWIDGET_H

#include <QWidget>
#include <QMenu>
#include <memory>

namespace Ui {
class LibrariesWidget;
}

namespace StData {
class Project;
}

class LibrariesModel;
class LibraryPropertiesDialog;
class AddLibraryDialog;

class LibrariesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LibrariesWidget(QWidget *project = 0);
    ~LibrariesWidget();

    LibrariesModel* librariesModel() const;

//    void setLibraryManager(LibraryFactory* libraryManager);

public slots:
    void setProject(StData::Project* project);

private slots:
    void treeItemClicked(const QModelIndex& index);
    void updateActionsState();
    void addLibrary();
    void removeItem();
    void hideEmptyLibraries(bool isEmptyLibrariesHidden);
    void setFilter();
    void contextMenuRequested(const QPoint& point);

    void onPropertiesTriggered();
    void onOpenInFileManagerTriggered();

private:
    QString currentItemPath();

private:
    Ui::LibrariesWidget *ui;

    LibraryPropertiesDialog* m_libraryPropertiesDialog;
    AddLibraryDialog* m_addLibraryDialog;
    QMenu* m_contextMenu;

    LibrariesModel* m_librariesModel;
};

#endif // LIBRARIESWIDGET_H
