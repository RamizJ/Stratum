#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include "Log.h"


namespace Ui {
class MainWindow;
}

class AndroidFileDialog;
class AndroidFileDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setText(const QString& text);
    void addText(const QString& text, const StData::MessageType& msgType);
    void addMessage(const QString& text, const StData::MessageType& type);
    void initialize();

    // QWidget interface
protected:
    void showEvent(QShowEvent*);
    void resizeEvent(QResizeEvent* event);

private slots:
    void openFile();
    void openFileAccepted(const QString& fileName);
    void onVmStop();

private:
    void run(const QString& projectFileName);



private:
    Ui::MainWindow *ui;
    std::shared_ptr<AndroidFileDialog> m_fileDialog;
    bool m_isInitialized;
};

#endif // MAINWINDOW_H
