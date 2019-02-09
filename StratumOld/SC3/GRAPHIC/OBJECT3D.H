/*
Copyright (c) 1995  Virtual World
Module Name:
	 object3d.h
Author:
	 Alexander Shelemekhov
*/

#ifndef __3dobject_h
#define __3dobject_h
#include "object.h"
#include <gwin2d3d\3drender.h>
#include "matrix.h"
//#include <windows.h>
#define MAXLONG     0x7FFFFFFFL
#define _e 0.05

inline int SGN(double f){
 if (f<0) return -1;
 if (f>0) return 1;
 return 0;
};
inline int SGNEqual(double f1,double f2){
 int a=SGN(f1);int b=SGN(f2);
 return ((a==b)||(a==0)||(b==0));
};

//флаги для Object3d
const ofvisible=128u;
const ofsolid=1;
const ofdefault=ofsolid;
class TObject3d;
typedef TObject3d * PObject3d;

	/*  object3d */

class TSpace3d;
class TLight3d:public TObject{
public:
 TSpace3d *space3d;
 LIGHT3D light;
 TLight3d(LIGHT3D*);
 ~TLight3d();
 TLight3d(PStream,LONG _pos);
 void Store(PStream ps);
 POINT3D Move3d(POINT3D);
 virtual WORD WhoIsIt(){return 12;};
 virtual BOOL GetOrg_Size3d(POINT3D &,POINT3D &);
};

class TPrimitives:public TCollection{
public:
TPrimitives():TCollection(10,10){};
virtual void FreeItem(pointer);
};
class TMaterial3d;
class TPrimitive3d:public tagPRIMITIVE3D{
public:
TMaterial3d * _mat;
TPrimitive3d(PRIMITIVE3D*,TSpace3d*);
TPrimitive3d(PStream,TSpace3d*);
~TPrimitive3d();
BOOL Set(PRIMITIVE3D*,TSpace3d*);
void _Set(PRIMITIVE3D*,TSpace3d*);
void Store(PStream ps);
};
#define UIV_PRIMITIVE 1
#define UIV_POINTS    2
#define UIV_MATRIX    4
#define UIV_ALL       0xff

class TObject3d:public TObject {
public:
POINT3D origin,size;
INT16     pointcount;
POINT3D * points;
TCollection *primitives;
TMatrix2 matrix;
//-----------------------
WORD flag;
TSpace3d * space3d;
COLOR3D color;
unsigned colorvalid:1;


TObject3d();
TObject3d(PStream,LONG _pos);
~TObject3d();
virtual BOOL GetOrg_Size3d(POINT3D &,POINT3D &);
void Store(PStream ps);
virtual void UpdateSize(int copy=1,void * _p=NULL);
BOOL   IsPointInUse(INT16);
//---------------------------------
void Bar3d(double x,double y,double z,COLORREF cl);
int  IsEmpty();
virtual WORD WhoIsIt(){return 10;};
void _DeletePoint(INT16);
//-----------------------------------*
void WritePoint(PStream,int);
BOOL ReadPoint(PStream,int i=-1);
virtual void StoreCoordinates(PStream);
virtual void LoadCoordinates(PStream);
virtual RECT GetRect(int FromPaint=0);
virtual void SetState(WORD astate,ATTR_MODE op,pointer );
virtual void Transform3d(TMatrix&);
virtual TMatrix2*GetMatrix(TMatrix2*);
void    SetColor3d(COLOR3D color);
BOOL ChkNewPoints(INT16 *,INT16 count);
BOOL ChkNewPrimitives(INT16 *,INT16 count);
void UpdateInView(BYTE f=UIV_ALL);
BOOL virtual Awaken(_SPACE*s);
BOOL GetSolidColor(COLOR3D&);
UINT32 IsIntersect(PObject,UINT32);
virtual void MarkToSave(int);
};
// for UpdateIn View

class TGroup3d : public TGroup
{public:
 TGroup3d():TGroup(){};
 TGroup3d(PStream ps,LONG _pos):TGroup(ps,_pos){};
 virtual RECT GetRect(int FromPaint=0);
 virtual BOOL GetOrg_Size3d(POINT3D &,POINT3D &);
 virtual WORD WhoIsIt(){return 5;};
 virtual void Transform3d(TMatrix&);
 virtual TMatrix2*GetMatrix(TMatrix2*);
 UINT32 IsIntersect(PObject,UINT32);
};

class TView3d_2d;
class TFrame3d :public TObject{
  public:
  POINT3D origin,size;
  TObject3d * object;
  TView3d_2d * view;
  POINT2D psize;
  POINT3D tops[9];
  RECT rect;
ROTATOR3D rotator;
BYTE flag;
TFrame3d(POINT2D _ps,TView3d_2d *,PObject3d po);
virtual WORD WhoIsIt(){return 52;};
virtual void UpdateSize(int copy=1,void * _p=NULL);
virtual RECT GetRect(int FromPaint=0);
//virtual POINT3D Move3d(POINT3D);
//virtual void Rotate3d(TRotator3d*r);
virtual BOOL GetOrg_Size3d(POINT3D &,POINT3D &);
//virtual POINT3D SetSize3d(POINT3D);
};

class TAxis3d :public TObject{
  public:
  POINT3D origin;
  TView3d_2d * view;
  POINT3D tops[4];
  RECT rect;
  int axisSize;
  HBITMAP bitmap;
TAxis3d(POINT3D _ps,int,TView3d_2d *);
~TAxis3d();
virtual WORD WhoIsIt(){return 53;};
virtual void UpdateSize(int copy=1,void * _p=NULL);
virtual RECT GetRect(int FromPaint=0);
//virtual POINT3D Move3d(POINT3D);
//virtual void Rotate3d(TRotator3d*);
virtual BOOL GetOrg_Size3d(POINT3D &,POINT3D &);
};

typedef TFrame3d * PFrame3d;
typedef TGroup3d * PGroup3d;
typedef TAxis3d * PAxis3d;
int  IsLineIntersect(double ax,double ay,double bx,double by,
			double cx,double cy,double dx,double dy,double _em,double &r);
int On_1_Side(POINT3D base,POINT3D b1,POINT3D _t1,POINT3D _t2);
inline int _intersect(long a,long b,long c,long d){
 if (a<=c)return b>c;else return d>a;
 };


class _TObject3d{
 public:
  TObject3d*object;
  POINT3Di  org;
  POINT3Di  size;
  UINT16    objflags;
  UINT32    flags;
  TItems*   points;
  TItems*   primitives;

// non visible in render

  TMatrix   matrix;
  double focus;
  POINT2D offset;
  BYTE perspective;
  BYTE boundsinvalid;
  POINT3D bounds[8];

 _TObject3d(TObject3d*);
~_TObject3d();
 void UpdatePrimitive(C_TYPE i);
 void Perspective(C_TYPE i);

 void _Perspective(POINT3D*in,POINT3D*out);
 void PerspectiveAll();
 void UpdateMatrix(TView3d_2d*);

 void UpdatePoints(BYTE mode);// 1 - calc color/calc C
 void UpdateAllPrimitives();
 void UpdateBounds();
 void GetBoundsRect(POINT3D*,POINT3D*,RECT*r=NULL);
 BOOL GetPrimitiveZ(C_TYPE i,double x,double y,double &z);
};


void    Get8PointsByOrgSize(POINT3D*,POINT3D*,POINT3D *);
//BOOL    MakeDefaultCamera(CAMERA3D * p);
double  GetAngleByXY(double x,double y);

#endif //__3dobject_h
