#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

using namespace std;

namespace Ui {
class MainWindow;
}

class LibrariesModel;
class TabManager;
class AndroidFileDialog;

class ResizeFilter : public QObject
{
  Q_OBJECT
  public:
    ResizeFilter();
  protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *project = 0);
    ~MainWindow();

    void loadProject(const QString& fileName);

    bool eventFilter(QObject* watched, QEvent* event);

    // QWidget interface
protected:
    void showEvent(QShowEvent* event);


private slots:
    void openProjectFile();
    void closeProject();

    void run();
    void stop();
    void pause();
    void oneStep();

    void onVmStop();
    void onVmPause();

    void updateActionsState();

    void androidFileNameReady(QString fileName);

private:
    void setupDockActions();
    void openProject(const QString& filePath);


private:
    Ui::MainWindow *ui;

    TabManager* m_tabManager;
    ResizeFilter m_resizeFilter;

};


#endif // MAINWINDOW_H
