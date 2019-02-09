/*
Copyright (c) 1995  Virtual World
Module Name:
	 _system.h
Author:
	 Alexander Shelemekhov
*/
#ifndef __system_h__
#define __system_h__
#include "space2d.h"
#include "space3d.h"
#include "meta.h"
#ifndef __GRAPH_DLL_TYPES
#include "_gr_sys.h"
#endif
//#include "gr_types.h"
#include "_g_def.h"
UINT extern CFormat_2D,CFormat_3D;
//#define _EXPORT far
PSpace2d GetSpace(HSP2D h);
PSpace3d Get3dSpace(HSP3D,PSpace2d &);
PObject2d _GetObject2d(HSP2D h,HOBJ2D o,int t=0);
PObject _GetObject(HSP2D h,HOBJ2D o,_SPACE** _ps=NULL);

BOOL LoadHook();
HINSTANCE extern HHook;
char extern _graphics_ini[256];

HBRUSH GetGrayBrush();
HBRUSH GetCrossBrush();
void PaintBmpProc(long org,long ul,long ur,long W,int&dx,int&cx);
#endif //__system_h__
