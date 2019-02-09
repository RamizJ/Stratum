#ifndef SPACEITEMLOCATIONWIDGET_H
#define SPACEITEMLOCATIONWIDGET_H

#include <QWidget>

namespace Ui {
class SpaceItemLocationWidget;
}

class EditLocationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditLocationWidget(QWidget *parent = 0);
    ~EditLocationWidget();

    QString spaceItemName() const;
    void setSpaceItemName(const QString& name);

    QSizeF spaceItemSize() const;
    void setSpaceItemSize(const QSizeF& spaceItemSize);

    QPointF spaceItemPosition() const;
    void setSpaceItemPosition(const QPointF& pos);

    double spaceItemAngle() const;
    void setSpaceItemAngle(double angle);

    quint8 spaceItemLayer() const;
    void setSpaceItemLayer(quint8 layer);

    bool spaceItemIsSelectable() const;
    void setSpaceItemIsSelectable(bool isSelectable);

private:
    Ui::SpaceItemLocationWidget *ui;
};

#endif // SPACEITEMLOCATIONWIDGET_H
