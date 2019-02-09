#ifndef EDITPENWIDGET_H
#define EDITPENWIDGET_H

#include <QPen>
#include <QWidget>

namespace Ui {
class EditPenWidget;
}

class EditPenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditPenWidget(QWidget *parent = 0);
    ~EditPenWidget();

    QPen pen() const;
    void setPen(const QPen& pen);

private slots:
    void onChoosePenColor();
    void onPenStyleChanged(int styleIndex);
    void onPenWidthChanged(int penWidth);

private:
    Ui::EditPenWidget *ui;

    QPen m_pen;
};

#endif // EDITPENWIDGET_H
