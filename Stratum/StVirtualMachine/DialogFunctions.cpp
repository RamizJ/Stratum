#include "DialogFunctions.h"

#include "VmCodes.h"
#include "VmGlobal.h"
#include "VmContext.h"

#include <StDataGlobal.h>

#include <QInputDialog>
#include <QAbstractButton>
#include <QFileDialog>
#include <QColorDialog>
#include <QMessageBox>

#define tr(str) QObject::tr(str)

//MessageBox buttons
#define MB_OK               0
#define MB_OKCANCEL         1
#define MB_ABORTRETRYIGNORE 2
#define MB_YESNOCANCEL      3
#define MB_YESNO            4
#define MB_RETRYCANCEL      5
#define MB_TYPEMASK         15

//#define MB_ICONHAND         16
#define MB_ICONSTOP         16 //MB_ICONHAND
#define MB_ICONQUESTION     32
#define MB_ICONEXCLAMATION  48
#define MB_ICONINFORMATION  64 //MB_ICONASTERISK
//#define MB_ICONASTERISK     64
#define MB_ICONMASK         240 //0x00F0

#define MB_DEFBUTTON1       0
#define MB_DEFBUTTON2       256 //0x0100
#define MB_DEFBUTTON3       512 //0x0200

//MessageBox result
#define OK      1
#define CANCEL  2
#define ABORT   3
#define RETRY   4
#define IGNORE  5
#define YES     6
#define NO      7

using namespace StData;

namespace StVirtualMachine {

void setupDialogFunctions()
{
    operations[vm_INPUT_BOX] = inputBox;
    operations[MESSAGE_BOX] = messageBox;
    operations[V_SAVEDIALOG] = saveFileDialog;
    operations[V_LOADDIALOG] = openFileDialog;
    operations[CHOSECOLORDIALOG] = chooseColorDialog;
    operations[V_FOLDERDIALOG] = chooseFolderDialog;
}

void inputBox()
{
    QString text = valueStack->popString();
    QString labelText = valueStack->popString();
    QString title = valueStack->popString();

    QInputDialog inputDialog(nullptr, Qt::WindowCloseButtonHint);
    inputDialog.resize(300, inputDialog.height());

    inputDialog.setWindowTitle(title);
    inputDialog.setTextValue(text);
    inputDialog.setLabelText(labelText);

    inputDialog.setCancelButtonText("Отмена");

    if(inputDialog.exec())
        text = inputDialog.textValue();
    else
        text = "";

    valueStack->pushString(text);
}

void messageBox()
{
    uint style = valueStack->popDouble();
    QString title = valueStack->popString();
    QString text = valueStack->popString();

    QMessageBox messageBox;
    //Icon
    {
        if((style & MB_ICONINFORMATION) == MB_ICONINFORMATION)
            messageBox.setIcon(QMessageBox::Information);

        else if((style & MB_ICONEXCLAMATION) == MB_ICONEXCLAMATION)
            messageBox.setIcon(QMessageBox::Warning);

        else if((style & MB_ICONQUESTION) == MB_ICONQUESTION)
            messageBox.setIcon(QMessageBox::Question);

        else if((style & MB_ICONSTOP) == MB_ICONSTOP)
            messageBox.setIcon(QMessageBox::Critical);
    }

    //Buttons
    {
        if((style & MB_RETRYCANCEL) == MB_RETRYCANCEL)
        {
            messageBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
            if(style & MB_DEFBUTTON2)
                messageBox.setDefaultButton(QMessageBox::Cancel);

            messageBox.button(QMessageBox::Retry)->setText(tr("Повторить"));
            messageBox.button(QMessageBox::Cancel)->setText(tr("Отмена"));
        }
        else if((style & MB_YESNO) == MB_YESNO)
        {
            messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            if(style & MB_DEFBUTTON2)
                messageBox.setDefaultButton(QMessageBox::No);

            messageBox.button(QMessageBox::Yes)->setText(tr("Да"));
            messageBox.button(QMessageBox::No)->setText(tr("Нет"));
        }
        else if((style & MB_YESNOCANCEL) == MB_YESNOCANCEL)
        {
            messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            if(style & MB_DEFBUTTON2)
                messageBox.setDefaultButton(QMessageBox::No);

            else if(style & MB_DEFBUTTON3)
                messageBox.setDefaultButton(QMessageBox::Cancel);

            messageBox.button(QMessageBox::Yes)->setText(tr("Да"));
            messageBox.button(QMessageBox::No)->setText(tr("Нет"));
            messageBox.button(QMessageBox::Cancel)->setText(tr("Отмена"));
        }
        else if((style & MB_ABORTRETRYIGNORE) == MB_ABORTRETRYIGNORE)
        {
            messageBox.setStandardButtons(QMessageBox::Abort | QMessageBox::Retry | QMessageBox::Ignore);
            if(style & MB_DEFBUTTON2)
                messageBox.setDefaultButton(QMessageBox::Retry);

            else if(style & MB_DEFBUTTON3)
                messageBox.setDefaultButton(QMessageBox::Ignore);

            messageBox.button(QMessageBox::Abort)->setText(tr("Прервать"));
            messageBox.button(QMessageBox::Retry)->setText(tr("Повторить"));
            messageBox.button(QMessageBox::Ignore)->setText(tr("Пропустить"));
        }
        else if((style & MB_OKCANCEL) == MB_OKCANCEL)
        {
            messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            if(style & MB_DEFBUTTON2)
                messageBox.setDefaultButton(QMessageBox::Cancel);
            messageBox.button(QMessageBox::Cancel)->setText(tr("Отмена"));
        }
        else if((style & MB_OK) == MB_OK)
        {
            messageBox.setStandardButtons(QMessageBox::Ok);
        }
    }

    messageBox.setWindowTitle(title);
    messageBox.setText(text);


    VmContext::setIsMessageBoxRaised(true);
    int ret = messageBox.exec();
    VmContext::setIsMessageBoxRaised(false);


    switch(ret)
    {
        case QMessageBox::Ok:
            ret = OK;
            break;
        case QMessageBox::Cancel:
            ret = CANCEL;
            break;
        case QMessageBox::Yes:
            ret = YES;
            break;
        case QMessageBox::No:
            ret = NO;
            break;
        case QMessageBox::Retry:
            ret = RETRY;
            break;
        case QMessageBox::Ignore:
            ret = IGNORE;
            break;
        case QMessageBox::Abort:
            ret = ABORT;
            break;
    }
    valueStack->pushDouble(ret);
}

void openFileDialog()
{
    QString defaultFileName = valueStack->popString();
    QString filter = valueStack->popString();
    QString title = valueStack->popString();

    QFileInfo fileInfo(defaultFileName);
    if(fileInfo.isRelative())
        defaultFileName = executedProject->projectDir().absoluteFilePath(defaultFileName);

    valueStack->pushString(QFileDialog::getOpenFileName(nullptr, title, defaultFileName, filter));
}

void saveFileDialog()
{
    QString defaultFileName = valueStack->popString();
    QString filter = valueStack->popString();
    QString title = valueStack->popString();

    QFileInfo fileInfo(defaultFileName);
    if(fileInfo.isRelative())
        defaultFileName = executedProject->projectDir().absoluteFilePath(defaultFileName);

    valueStack->pushString(QFileDialog::getSaveFileName(nullptr, title, defaultFileName, filter));
}

void chooseColorDialog()
{
    QColor initColor(valueStack->popInt32());
    QString title = valueStack->popString();

    QColor color = QColorDialog::getColor(initColor, nullptr, title);
    if(!color.isValid())
        color = initColor;

    valueStack->pushInt32(colorToInt(color));
}

void chooseFolderDialog()
{
    /*quint32 flags = */valueStack->popDouble();
    QString dirName = valueStack->popString();
    QString title = valueStack->popString();

    if(dirName.isEmpty() || QDir(dirName).isRelative())
        dirName = executedProject->projectDir().absoluteFilePath(dirName);

    dirName = QFileDialog::getExistingDirectory(nullptr, title, dirName);
    valueStack->pushString(QDir::toNativeSeparators(dirName));
}


}
