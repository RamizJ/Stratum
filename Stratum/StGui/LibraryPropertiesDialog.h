#ifndef LIBRARYPROPERTIESDIALOG_H
#define LIBRARYPROPERTIESDIALOG_H

#include <QDialog>

namespace Ui {
class LibraryPropertiesDialog;
}

namespace StData {
class Library;
}

class LibraryPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LibraryPropertiesDialog(QWidget *parent = 0);
    ~LibraryPropertiesDialog();

    StData::Library* library() const {return m_library;}
    void setLibrary(StData::Library* library);

private:
    Ui::LibraryPropertiesDialog *ui;
    StData::Library* m_library;

    QString m_titleText;
    QString m_labelClassesCountText;
};

#endif // LIBRARYPROPERTIESDIALOG_H
