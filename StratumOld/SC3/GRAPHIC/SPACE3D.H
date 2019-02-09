/*
Copyright (c) 1995  Virtual World
Module Name:
	 space3d.h
Author:
	 Alexander Shelemekhov
*/
#ifndef __SPACE3D_H_
#define __SPACE3D_H_
#include "_space.h"
#include "object3d.h"
 class TSpace2d;
const vfwaitcursor=1;

#define UOF_BOUNDS 1
#define UOF_AXIS   2
#define UOF_BUFFER 4
#define UOF_ALL    7


//------------// class TView3d //--------------//
 class TSpace3d:public _SPACE {
 public:
  HOBJ2D _HANDLE;
  HSP3D _HSP3D;
  TSpace2d * space2d;
  TCollection * views;
  RECT updaterect;
  int needsave;
  HPALETTE palette;

 TMaterialls3d*materials;
// transform matrix
 TMatrix2 matrix;
 HOBJ2D   localCS;
 UINT16   localType;

 TSpace3d();
 ~TSpace3d();
 TSpace3d(PStream,LONG _pos);

void _TSpace3d();// Зануляет все
// Преобразование координат

 void ResetCrd();
 void UpdateCrd();
 BOOL SetCrd(CRDSYSTEM*);
 BOOL GetCrd(CRDSYSTEM*);
 BOOL PushCrd();
 BOOL PopCrd();


 BOOL LPtoCP(PObject po,POINT3D*src,POINT3D*dst,int);

 void CPtoWP(POINT3D*src,POINT3D*dst,int);
 void CStoWS(POINT3D*src,POINT3D*dst,int);
 void WPtoCP(POINT3D*src,POINT3D*dst,int);
 void WStoCS(POINT3D*src,POINT3D*dst,int);

// Преобразование координат

void _SaveView(PStream);

void Store(PStream);
virtual PGroup _CreateGroup();
virtual PObject GetObjByHANDLE(HOBJ2D);
virtual WORD WhoIsIt(){return 17459;}
//*---------------------------------------------------------*
//   Objects
HOBJ2D CreateObject3d();
BOOL DeleteObject3d(HOBJ2D Object3d);
BOOL _DeleteObject3d(PObject po);
INT16 DeleteEmptyObjects(void *  _m=NULL);


BOOL  SetObjectOrigin3d(HOBJ2D Object3d,POINT3D * Origin);
BOOL  SetObjectBase3d(HOBJ2D Object3d,POINT3D * Base);

BOOL  SetObjectSize3d(HOBJ2D Object3d, POINT3D * Size );
BOOL  RotateObject3d(HOBJ2D Object3d, ROTATOR3D * rotator );
BOOL  Transform3d(HOBJ2D Object3d,double*matrix);
BOOL  SetMatrix(HOBJ2D Object3d,void*);

int   SetFrame3dPoint(int n,POINT3D);


// Points  **************************************************

BOOL SetPoints3di( HOBJ2D Object3d,POINT3D * points,INT16 *nums,INT16 count );
BOOL SetPoints3d(HOBJ2D Object3d, POINT3D * points,INT16 from,INT16 count );

INT16 AddPoints3d(HOBJ2D Object3d, POINT3D * point,INT16 count );
BOOL  DelPoints3di( HOBJ2D Object3d,INT16*,INT16 num);
BOOL  DelPoints3d( HOBJ2D Object3d,INT16 from,INT16 count);

INT16 DelUnusedPoints3d( HOBJ2D Object3d ,PStream ps=NULL);
BOOL  TransformObjectPoints3d(HOBJ2D Object3d,double*m);

HOBJ2D CreateCamera3d(CAMERA3D*);
BOOL   SetCamera3d(HOBJ2D,CAMERA3D*);

HOBJ2D CreateLight3d(LIGHT3D*l);
BOOL   SetLight3d(HOBJ2D,LIGHT3D*);
BOOL   GetObjectBase(PObject3d po,POINT3D*);

INT16  CreatePrimitives3d(HOBJ2D Object3d,PRIMITIVE3D*prim,INT16 num);
BOOL   DelPrimitives3d(HOBJ2D Object3d,INT16* PrimNum,INT16 count);
BOOL   SetPrimitives3di(HOBJ2D Object3d,PRIMITIVE3D*mask,
				INT16* items,INT16 count);
BOOL   SetPrimitives3d(HOBJ2D Object3d,PRIMITIVE3D*prim,
				INT16 from,INT16 count);
BOOL   SetPrimitive3d(PObject3d,PRIMITIVE3D&prim,C_TYPE i,C_TYPE index);
INT32 virtual IsIntersect(HOBJ2D ,HOBJ2D ,UINT32);
//---------------------------------------------------
void MarkToSave(int m);
HOBJ2D PasteObject(PStream,POINT3D *);

BOOL SetObjectColor3d(HOBJ2D,COLOR3D);
INT16 GetObjectColor3d(HOBJ2D,COLOR3D&);
BOOL StoreColors3d(PStream,HOBJ2D);
BOOL LoadColors3d(PStream);

void UpdateAll();
void UpdateObject(PObject3d,BYTE flags=UOF_ALL);
void UpdatePoint(PObject3d,INT16);
BOOL UpdateTool(int,HOBJ2D,pointer );
virtual void _SetModify();
//*---------------------------------------------------------*
BOOL ChkNewPrimitives(PObject3d,PRIMITIVE3D * ,INT16 Num);
//*---------------------------------------------------------*
BOOL  IsObjectVisible(PObject3d p);
//*---------------------------------------------------------*
HOBJ2D CreateMaterial3d(MATERIAL3D *);
BOOL   DeleteMaterial3d(HOBJ2D);
BOOL   SetMaterial3d(HOBJ2D,MATERIAL3D *);
BOOL   GetMaterial3d(HOBJ2D,MATERIAL3D *);
BOOL    _StoreObject(PStream ps,HOBJ2D obj);
};

typedef TSpace3d * PSpace3d;
BOOL IsSpacse3dEqual(PSpace3d,PSpace3d);
#endif
