#ifndef EDITRICHTEXTDIALOG_H
#define EDITRICHTEXTDIALOG_H

#include <QDialog>
#include <RasterTextGraphicsItem.h>

namespace Ui {
class EditRichTextDialog;
}
namespace StSpace {
class RasterTextGraphicsItem;
}

class EditRasterTextDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditRasterTextDialog(StSpace::RasterTextGraphicsItem* item, QWidget *parent = 0);
    ~EditRasterTextDialog();

    StSpace::RasterTextGraphicsItem* richTextGraphicsItem() const {return m_rasterTextGraphicsItem;}
    void setRichTextGraphicsItem(StSpace::RasterTextGraphicsItem* richTextGraphicsItem);

private slots:
    void onSelectTextColor();
    void onSelectBackgroundColor();

private:
    void setupFontSizeCombobox();

private:
    Ui::EditRichTextDialog *ui;

    StSpace::RasterTextGraphicsItem* m_rasterTextGraphicsItem;
};

#endif // EDITRICHTEXTDIALOG_H
