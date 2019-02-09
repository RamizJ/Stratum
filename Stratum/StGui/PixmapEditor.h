#ifndef PIXMAPEDITOR_H
#define PIXMAPEDITOR_H

#include <QGraphicsScene>
#include <QWidget>

namespace Ui {
class PixmapEditor;
}

class PixmapEditor : public QWidget
{
    Q_OBJECT

public:
    explicit PixmapEditor(QWidget *parent = 0);
    ~PixmapEditor();

    QPixmap pixmap() const;
    void setPixmap(const QPixmap& pixmap);

private:
    Ui::PixmapEditor *ui;

    QGraphicsPixmapItem* m_pixmapGraphicsItem;

    QGraphicsScene* m_scene;
};

#endif // PIXMAPEDITOR_H
