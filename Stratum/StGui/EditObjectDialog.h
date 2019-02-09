#ifndef OBJECTPROPERTIESDIALOG_H
#define OBJECTPROPERTIESDIALOG_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class EditObjectDialog;
}

namespace StData {
class Object;
}

class ObjectVariablesModel;
class IconLibraryModel;

class EditObjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditObjectDialog(StData::Object* object, QWidget *parent = 0);
    ~EditObjectDialog();

    StData::Object* classObject() const {return m_object;}
    void setClassObject(StData::Object* object);

private slots:
    void updateUiVariables();

private:
    Ui::EditObjectDialog *ui;

    StData::Object* m_object;
    ObjectVariablesModel* m_variablesModel;

    QString m_labelCreationDateText;
    QString m_dialogTitleText;

    IconLibraryModel* m_iconModel;
};

#endif // OBJECTPROPERTIESDIALOG_H
