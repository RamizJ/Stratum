#ifndef SPACEOBJECTFUNCTIONS_H
#define SPACEOBJECTFUNCTIONS_H

namespace StVirtualMachine {

extern void setupSpaceObjectFunctions();

extern void createObjectFromFile2d();
extern void deleteObject2d();
extern void getObjectType();

extern void getObjectName2d();
extern void setObjectName2d();

extern void getObject2dByName();

extern void getObjectOrg2dX();
extern void getObjectOrg2dY();
extern void setObjectOrg2d();

extern void getObjectWidth2d();
extern void getObjectHeight2d();
extern void getActualHeight2d();
extern void getActualWidth2d();
extern void getActualSize2d();
extern void setObjectSize2d();

extern void getObjectAngle2d();
extern void rotateObject2d();
extern void getObjectAlpha2d();
extern void setObjectAlpha2d();

extern void setShowObject2d();
extern void showObject2d();
extern void hideObject2d();

extern void getObjectFromPoint2d();
extern void getObjectFromPoint2dEx();
extern void getLastPrimary();

extern void isObjectsIntersect2d();

extern void getNextObject2d();
extern void getCurrentObject2d();
extern void setCurrentObject2d();

extern void lockObject2d();

extern void setObjectAttribute2d();
extern void getObjectAttribute2d();

extern void getBottomObject2d();
extern void getUpperObject2d();
extern void getLowerObject2d();
extern void getTopObject2d();
extern void getObjectFromZOrder2d();
extern void getZOrder2d();
extern void objectToBottom2d();
extern void objectToTop2d();
extern void setZOrder2d();
extern void swapObjects2d();

}


#endif // SPACEOBJECTFUNCTIONS_H
