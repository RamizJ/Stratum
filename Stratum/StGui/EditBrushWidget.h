#ifndef EDITBRUSHWIDGET_H
#define EDITBRUSHWIDGET_H

#include <QWidget>

namespace Ui {
class EditBrushWidget;
}

class EditBrushWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditBrushWidget(QWidget *parent = 0);
    ~EditBrushWidget();

    QBrush brush() const {return m_brush;}
    void setBrush(const QBrush& brush);

    void updateButtonIconColor();

private slots:
    void onChooseBrushColor();
    void onBrushStyleChanged(int styleIndex);
    void onBrushEnabled(bool enabled);

private:
    Ui::EditBrushWidget *ui;

    QBrush m_brush;
};

#endif // EDITBRUSHWIDGET_H
