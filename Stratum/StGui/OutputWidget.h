#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include <QWidget>

#include "OutputListModel.h"

namespace Ui {
class OutputWidget;
}

class OutputWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OutputWidget(QWidget *project = 0);
    ~OutputWidget();

private:
    Ui::OutputWidget *ui;

    OutputListModel* m_outputModel;
};

#endif // OUTPUTWIDGET_H
