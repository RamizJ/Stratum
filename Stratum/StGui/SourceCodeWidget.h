#ifndef SOURCECODEWIDGET_H
#define SOURCECODEWIDGET_H

#include <QWidget>
#include <QFutureWatcher>

namespace Ui {
class SourceCodeWidget;
}

namespace StData {
class Class;
}

class SourceCodeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SourceCodeWidget(QWidget *parent = 0);
    ~SourceCodeWidget();

    void setClass(StData::Class* cls);

protected:
    void timerEvent(QTimerEvent*);

private slots:
    void startCompile();
    void compileFinished();

private:
    Ui::SourceCodeWidget *ui;
    QString m_compileStatusText;
    QFutureWatcher<bool> m_compileFutureWatcher;
    int m_timerId;

    StData::Class* m_class;
};

#endif // SOURCECODEWIDGET_H
