#ifndef EDITPIXMAPDIALOG_H
#define EDITPIXMAPDIALOG_H

#include <QDialog>

namespace Ui {
class EditPixmapDialog;
}

namespace StSpace {
class PixmapGraphicsItem;
}

class EditPixmapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditPixmapDialog(StSpace::PixmapGraphicsItem* item, QWidget *parent = 0);
    ~EditPixmapDialog();

    StSpace::PixmapGraphicsItem* pixmapGraphicsItem() const {return m_pixmapGraphicsItem;}
    void setPixmapGraphicsItem(StSpace::PixmapGraphicsItem* pixmapGraphicsItem);

private:
    Ui::EditPixmapDialog *ui;
    StSpace::PixmapGraphicsItem* m_pixmapGraphicsItem;
};

#endif // EDITPIXMAPDIALOG_H
