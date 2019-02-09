#ifndef EDITHYPERBASEWIDGET_H
#define EDITHYPERBASEWIDGET_H

#include <QWidget>

namespace Ui {
class EditHyperbaseWidget;
}

class EditHyperbaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditHyperbaseWidget(QWidget *parent = 0);
    ~EditHyperbaseWidget();

private:
    Ui::EditHyperbaseWidget *ui;
};

#endif // EDITHYPERBASEWIDGET_H
