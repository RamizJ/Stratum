/*
Copyright (c) 1996 TST
graphic library for windows support file

Module Name
	 funct_3d.h
*/

#include "tpoint3d.h"

class T3dSurfaceParameters :public TPoint3d{
public:
COLORREF color;
 T3dSurfaceParameters():TPoint3d(0,0,0){
 color=RGB(255,255,255);
}
};

int Create3dSurface(HSP3D hsp3d,int qx,int qy,int action(T3dSurfaceParameters&),
						  TPoint3d * Size,
						  TPoint3d * delta);

/* ===========================================================
qx - interval on by X
qy - interval on by Y
action - adress of procedure.
		 return Z value and Color by (x,y);
		 if action return zero
		 constructing of surface terminating
Size - size of output surface
delta - origin of output surface
============================================================  */

