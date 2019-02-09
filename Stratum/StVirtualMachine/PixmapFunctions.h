#ifndef PIXMAPFUNCTIONS_H
#define PIXMAPFUNCTIONS_H

namespace StVirtualMachine {

extern void setupTextureFunctions();

extern void createDib2d();
extern void createRDib2d();
extern void createDoubleDIB2d();
extern void createRefDoubleDib2d();

extern void createBitmap2d();
extern void createDoubleBitmap2d();

extern void getDibObject2d();
extern void setDibObject2d();

extern void getBitmapSrcRect2d();
extern void setBitmapSrcRect2d();

extern void getDibPixel2d();
extern void setDibPixel2d();

extern void getDDibObject2d();
extern void setDDibObject2d();

}

#endif // PIXMAPFUNCTIONS_H
