#ifndef EDITSCHEMEDIALOG_H
#define EDITSCHEMEDIALOG_H

#include <QDialog>
#include <QListWidget>

namespace Ui {
class EditSchemeDialog;
}

namespace StData {
class Space;
}

class EditSchemeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditSchemeDialog(StData::Space* space, QWidget *parent = 0);
    ~EditSchemeDialog();

    StData::Space* space2d() const {return m_space2d;}
    void setSpace2d(StData::Space* space2d);

private:
    void setupLayers();
    void addLayerItem(QListWidget* listWidget,  int i);

private:
    Ui::EditSchemeDialog *ui;
    StData::Space* m_space2d;
};

#endif // EDITSCHEMEDIALOG_H
