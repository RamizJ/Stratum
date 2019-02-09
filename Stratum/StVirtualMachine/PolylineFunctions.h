#ifndef POLYLINEFUNCTIONS_H
#define POLYLINEFUNCTIONS_H

namespace StVirtualMachine {

extern void setupPolylineFunctions();

extern void createLine2d();
extern void createPolyLine2d();

extern void getVectorNumPoints2d();
extern void addPoint2d();
extern void delPoint2d();

extern void getPenObject2d();
extern void setPenObject2d();

extern void getBrushObject2d();
extern void setBrushObject2d();

extern void getVectorPoint2dX();
extern void getVectorPoint2dY();
extern void setVectorPoint2d();

extern void getRgnCreateMode();
extern void setRgnCreateMode();

extern void setLineArrows2d();

}

#endif // POLYLINEFUNCTIONS_H
