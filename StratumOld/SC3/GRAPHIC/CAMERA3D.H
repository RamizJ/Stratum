/*
Copyright (c) 1995  Virtual World
Module Name:
	 camera3d.h
Author:
	 Alexander Shelemekhov
*/

class TCamera3d:public TObject{
public:
 CAMERA3D camera;
 TCollection* views;
 TCamera3d(CAMERA3D*);
 TCamera3d(PStream,LONG _pos);
 void Store(PStream ps);
 ~TCamera3d();
 virtual BOOL GetOrg_Size3d(POINT3D &,POINT3D &);
 virtual WORD WhoIsIt(){return 11;};
 void    Recalc(BOOL nb=FALSE);
 virtual void Transform3d(TMatrix&);
 virtual TMatrix2*GetMatrix(TMatrix2*);
 void GetTargetPoint3d(POINT3D&t);
 void SetTargetPoint3d(POINT3D&p);

 BOOL SetPoint(POINT3D&p,int i);
 BOOL GetPoint(POINT3D&p,int i);

};
