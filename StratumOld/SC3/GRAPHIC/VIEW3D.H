/*
Copyright (c) 1995  Virtual World
Module Name:
	 view3d.h
Author:
	 Alexander Shelemekhov
*/
#ifndef G_VIEW3D_H_
#define G_VIEW3D_H_

#include "matrix.h"

#define LIGHT_COMPATIBLE 1

#define UF_SCREEN 1
#define UF_BUFFER 2

#define UF_ALL    3


class TView3d_2d : public TObject2d{
public:
 double v11,v12,v13,v21,v22,v23,v31,v32,v33,v43;
 double o11,o12,o13,o21,o22,o23,o31,o32,o33;

 TMatrix matrix;

 double det;
 double o_x,o_y,o_z,focus;
 BOOL perspective;

 unsigned calcpoints:1;
 unsigned calcfaces:1;
 unsigned calcbounds:1;
 unsigned bounded:1;

 unsigned calcfacecolor:1;
 unsigned calcpointcolor:2;
 unsigned calcpointC:1;
 unsigned calcfaceABCD:1;

 unsigned updateobjcet:1;// Обновлять весь объект при любых изменениях
 unsigned updateall:1;// Обновлять вообще все при любых изменениях

 unsigned inversecalc:1; // Уже посчитана обратная матрица
 unsigned minimizerect:1;// Желательно примменять минимизацию облсти рисования
 unsigned wireframe:1;   // Рисуется проволочная модель
 unsigned zclipvalid:1;  // - можно производить клип по Z координатам

 T3dObjects *items;
 TSpace3d * space3d;
 TSpace2d * space2d;
 HBITMAP BMPcolor;
 HBITMAP BMPmono;
 RECT    updaterect;// в логических, относительно начала
 POINT   imagesize;
 BOOL    ischanged;
 POINT2D   _delta;
 POINT2D   offset;
 double __scale;
 TCamera3d * camera;

 BOOL  (WINAPI * Draw3dProc)(PAINTSTRUCT3D *);
 INT32 (WINAPI * MessageProc)(INT16,INT32,INT32);

 TView3d_2d(POINT2D,POINT2D,TSpace3d*,TCamera3d*);
 TView3d_2d(PStream,LONG _pos);
 ~TView3d_2d();
 virtual RECT GetRect(int FromPaint=0);
 virtual void Rotate2d(POINT2D,gr_float);
 virtual void UpdateSize(int copy=1,void * _p=NULL);
 virtual void Store(PStream);
 virtual BOOL Awaken(_SPACE*);
 virtual WORD WhoIsIt(){return 24;};
 virtual POINT2D SetSize2d(POINT2D);
 virtual void MarkToSave(int);

 void UpdatePrimitive(_TObject3d*,C_TYPE i);
 void NewBuffers();
 void UpdateRect(RECT *,BYTE f=UF_ALL);
 void NewCamera(TCamera3d*);
 void CreateObjects(); // Создание объектов по новому
 void UpdateOrgSizeView(_TObject3d*);
 void UpdateAllObjects();
 void UpdateObject(C_TYPE i,BYTE f); // Обновление точек и размеров.
 POINT2D FromLP2d(POINT2D pt,BOOL yup=TRUE);
 BOOL IsCompatible(int);
 virtual void SetState(WORD astate,ATTR_MODE op,pointer );

 void  UpdateCamera(BOOL UpdObjects=FALSE,BOOL UpdView=FALSE);
 void  UpdateRender();


 void Perspective(POINT3D*in,_TPOINT3D*out);
 void Perspective(POINT3D*in,POINT3D*out);
 void Perspective(POINT3D*in,POINT3D*out,int count);
 //void Perspective(_TObject3d*,C_TYPE i);
 //void PerspectiveAllPoints(_TObject3d*o);

 void GetMatrix(_TObject3d*o);

 void UnPerspective(POINT3D*in,POINT3D*out);

 void CalcObr();
 void DrawBoundedRect(HDC,_TObject3d*o,COLORREF,BOOL);
 void DrawAxis3d(HDC,_TObject3d*o,BYTE flags);
 void GetAxisPoints(_TObject3d*o,POINT3D* p);
 void GetAxisRect(_TObject3d*o,RECT*_r);
};
HBITMAP GetAxisBMP();
typedef TView3d_2d * PView3d_2d;
#endif
