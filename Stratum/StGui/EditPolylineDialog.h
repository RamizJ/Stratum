#ifndef EDITPOLYLINEDIALOG_H
#define EDITPOLYLINEDIALOG_H

#include <QDialog>

namespace Ui {
class EditPolylineDialog;
}

namespace StSpace {
class PolylineGraphicsItem;
}

class EditPolylineDialog : public QDialog
{
    Q_OBJECT

public:
    EditPolylineDialog(StSpace::PolylineGraphicsItem* item, QWidget *parent = 0);
    ~EditPolylineDialog();

    void accept();

    StSpace::PolylineGraphicsItem* polylineGraphicsItem() const {return m_polylineGraphicsItem;}
    void setPolylineGraphicsItem(StSpace::PolylineGraphicsItem* polylineGraphicsItem);

private:
    Ui::EditPolylineDialog *ui;
    StSpace::PolylineGraphicsItem* m_polylineGraphicsItem;
};

#endif // EDITPOLYLINEDIALOG_H
