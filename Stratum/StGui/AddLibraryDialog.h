#ifndef ADDLIBRTARYDIALOG_H
#define ADDLIBRTARYDIALOG_H

#include <QDialog>
#include <memory>

namespace Ui {
class AddLibraryDialog;
}

namespace StData {
class Project;
}

class AddLibraryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddLibraryDialog(QWidget *parent = 0);
    ~AddLibraryDialog();

    QString selectedPath();
    bool isRecursive();

    StData::Project* project() const {return m_project;}
    void setProject(StData::Project* project);

private slots:
    void chooseDirectory();
    void validateSelectedPath();
    void pathIsRelativeToggled(bool toggle);

private:
    Ui::AddLibraryDialog *ui;
    QString m_projectPathText;

    StData::Project* m_project;
};



#endif // ADDLIBRTARYDIALOG_H
