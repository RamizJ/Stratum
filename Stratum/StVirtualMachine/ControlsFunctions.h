#ifndef CONTROLSFUNCTIONS_H
#define CONTROLSFUNCTIONS_H

namespace StVirtualMachine {

extern void setupControlFunctions();

extern void createControl();
extern void enableControl();
extern void setControlFocus();

extern void getControlText();
extern void setControlText();
extern void setControlFont();

extern void getControlStyle();
extern void setControlStyle();

extern void isButtonChecked();
extern void setButtonChecked();

extern void listAddItem();
extern void listInsertItem();
extern void listGetItem();
extern void listClear();
extern void listRemoveItem();
extern void listItemsCount();
extern void listCurrentIndex();
extern void listSetCurrentIndex();
extern void listSelectedIndexes();
//extern void listSetSelectedIndex();
extern void listFindItem();
extern void listFindItemExact();

}

#endif // CONTROLSFUNCTIONS_H
