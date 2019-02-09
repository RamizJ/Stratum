#include "Array.h"
#include "ControlsFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"

#include <SpaceWidget.h>
#include <SpaceScene.h>
#include <Space.h>
#include <StandardTypes.h>

using namespace StData;
using namespace StSpace;

namespace StVirtualMachine {

void setupControlFunctions()
{
    operations[VM_CREATECONTROL2D] = createControl;
    operations[VM_ENABLECONTROL] = enableControl;
    operations[VM_SETCONTROLFOCUS] = setControlFocus;
    operations[VM_GETCONTROLTEXT] = getControlText;
    operations[VM_SETCONTROLTEXT] = setControlText;
    operations[VM_SETCONTROLFONT] = setControlFont;
    operations[GETCONTROLSTYLE2D] = getControlStyle;
    operations[SETCONTROLSTYLE2D] = setControlStyle;
    operations[VM_ISDLGBUTTONCHECKED] = isButtonChecked;
    operations[VM_CHECKDLGBUTTON] = setButtonChecked;

    operations[LBADDSTRING] = listAddItem;
    operations[LBINSERTSTRING] = listInsertItem;
    operations[LBGETSTRING] = listGetItem;
    operations[LBCLEARLIST] = listClear;
    operations[LBDELETESTRING] = listRemoveItem;
    operations[LBGETCOUNT] = listItemsCount;
    operations[LBGETCARETINDEX] = listCurrentIndex;
    operations[LBSETCARETINDEX] = listSetCurrentIndex;
    operations[LBFINDSTRING] = listFindItem;
    operations[LBFINDSTRINGEXACT] = listFindItemExact;
    operations[LBGETSELINDEX] = listCurrentIndex;
    operations[VM_LBGETSELINDEXS] = listSelectedIndexes;
    operations[LBSETSELINDEX] = listSetCurrentIndex;
}

void createControl()
{
    int id = valueStack->popDouble();
    double height = valueStack->popDouble();
    double width = valueStack->popDouble();
    double y = valueStack->popDouble();
    double x = valueStack->popDouble();

    int style = valueStack->popDouble();
    QString text = valueStack->popString();
    QString className = valueStack->popString();
    int spaceHandle = valueStack->popInt32();

    int controlHandle = 0;
    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
        controlHandle = w->spaceScene()->createControl(className, text, style, x, y, width, height, id);

    valueStack->pushInt32(controlHandle);
}

void enableControl()
{
    bool enabled = valueStack->popDouble();
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        double res = w->spaceScene()->setControlEnabled(controlHandle, enabled);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
}

void setControlFocus()
{
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        double res = w->spaceScene()->setControlFocus(controlHandle);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
}

void getControlText()
{
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        QString text = w->spaceScene()->getControlText(controlHandle);
        valueStack->pushString(text);
    }
    else
        valueStack->pushString("");
}

void setControlText()
{
    QString text = valueStack->popString();
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        bool res = w->spaceScene()->setControlText(controlHandle, text);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
}

void setControlFont()
{
    int fontHandle = valueStack->popInt32();
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        bool res = w->spaceScene()->setControlFont(controlHandle, fontHandle);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
}

void getControlStyle()
{
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        int res = w->spaceScene()->getControlStyle(controlHandle);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
}

void setControlStyle()
{
    int style = valueStack->popDouble();
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        bool res = w->spaceScene()->setControlStyle(controlHandle, style);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
}

void isButtonChecked()
{
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        bool res = w->spaceScene()->isButtonChecked(controlHandle);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
}

void setButtonChecked()
{
    int checkState = valueStack->popDouble();
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        bool res = w->spaceScene()->setButtonChecked(controlHandle, checkState);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
}

void listAddItem()
{
    QString itemText = valueStack->popString();
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        bool res = w->spaceScene()->listAddItem(controlHandle, itemText);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
}

void listInsertItem()
{
    int index = valueStack->popDouble();
    QString itemText = valueStack->popString();
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        bool res = w->spaceScene()->listInsertItem(controlHandle, index, itemText);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
}

void listGetItem()
{
    int index = valueStack->popDouble();
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        QString itemText = w->spaceScene()->listGetItem(controlHandle, index);
        valueStack->pushString(itemText);
    }
    else
        valueStack->pushString("");
}

void listClear()
{
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        bool res = w->spaceScene()->listClear(controlHandle);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
}

void listRemoveItem()
{
    int index = valueStack->popDouble();
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        bool res = w->spaceScene()->listRemoveItem(controlHandle, index);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
}

void listItemsCount()
{
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        int itemsCount = w->spaceScene()->listItemsCount(controlHandle);
        valueStack->pushDouble(itemsCount);
    }
    else
        valueStack->pushDouble(0.0);
}

void listCurrentIndex()
{
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        int currentIndex = w->spaceScene()->listCurrentIndex(controlHandle);
        valueStack->pushDouble(currentIndex);
    }
    else
        valueStack->pushDouble(0.0);
}

void listSelectedIndexes()
{
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        QList<int> selectedIndexes = w->spaceScene()->listSelectedIndexes(controlHandle);
        int arrayHandle = arrayManager->createArray();
        Array* array = arrayManager->getArray(arrayHandle);

        for(int index : selectedIndexes)
        {
            ArrayItem* item = new ArrayItem(StandardTypes::floatType());
            item->var()->setDouble(index);
            array->insertItem(item);
        }
        valueStack->pushInt32(arrayHandle);
    }
    else
        valueStack->pushInt32(0);
}

//void listSetSelectedIndex()
//{
//    int index = valueStack->popDouble();
//    int controlHandle = valueStack->popInt32();
//    int spaceHandle = valueStack->popInt32();

//    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
//    {
//        bool res = w->spaceScene()->listSetCurrentIndex(controlHandle, currentIndex);
//        valueStack->pushDouble(res);
//    }
//    else
//        valueStack->pushDouble(0.0);
//}

void listSetCurrentIndex()
{
    int currentIndex = valueStack->popDouble();
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        bool res = w->spaceScene()->listSetCurrentIndex(controlHandle, currentIndex);
        valueStack->pushDouble(res);
    }
    else
        valueStack->pushDouble(0.0);
}

void listFindItem()
{
    int startIndex = valueStack->popDouble();
    QString searchingItemText = valueStack->popString();
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        int searchingItemIndex = w->spaceScene()->listFindItem(controlHandle, searchingItemText, startIndex);
        valueStack->pushDouble(searchingItemIndex);
    }
    else
        valueStack->pushDouble(0.0);
}

void listFindItemExact()
{
    int startIndex = valueStack->popDouble();
    QString searchingItemText = valueStack->popString();
    int controlHandle = valueStack->popInt32();
    int spaceHandle = valueStack->popInt32();

    if(SpaceWidget* w = windowManager->getWidget(spaceHandle))
    {
        int searchingItemIndex = w->spaceScene()->listFindItemExact(controlHandle, searchingItemText, startIndex);
        valueStack->pushDouble(searchingItemIndex);
    }
    else
        valueStack->pushDouble(0.0);

}

}
