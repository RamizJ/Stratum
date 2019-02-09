#ifndef SPACEFUNCTIONS_H
#define SPACEFUNCTIONS_H

namespace StVirtualMachine {

extern void setupSpaceFunctions();

extern void getSpaceOrg2dX();
extern void getSpaceOrg2dY();
extern void setSpaceOrg2d();

extern void getSpaceScale2d();
extern void setSpaceScale2d();

extern void emptySpace2d();
extern void setCrdSystem2d();
extern void saveRectArea2d();

extern void getBkBrush2d();
extern void setBkBrush2d();

extern void lockSpace2d();

extern void getSpaceLayers2d();
extern void setSpaceLayers2d();

extern void copyToClipboard();
extern void pasteFromClipboard();

}

#endif // SPACEFUNCTIONS_H
