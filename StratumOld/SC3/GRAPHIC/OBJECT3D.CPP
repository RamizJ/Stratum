/*
Copyright (c) 1995  Virtual World
Module Name:
	 object3d.cpp
Author:
	 Alexander Shelemekhov
*/
//#define NOKERNEL         // KERNEL APIs and definitions
//#define NOGDI             GDI APIs and definitions
//#define NOUSER           // USER APIs and definitions
#define NOSOUND          // Sound APIs and definitions
#define NOCOMM           // Comm driver APIs and definitions
#define NODRIVERS        // Installable driver APIs and definitions
//#define OEMRESOURCE      // OEM Resource values
#define NONLS            // All NLS defines and routines
#define NOSERVICE        // All Service Controller routines, SERVICE_ equates, etc.
#define NOKANJI          // Kanji support stuff.
#define NOMINMAX         // min() and max() macros
#define NOLOGERROR       // LogError() and related definitions
#define NOPROFILER       // Profiler APIs
#define NOMEMMGR         // Local and global memory management
#define NOLFILEIO        //_l* file I/O routines
#define NOOPENFILE       // OpenFile and related definitions
//#define NORESOURCE       // Resource management
#define NOATOM           // Atom management
#define NOLANGUAGE       // Character test routines
//#define NOLSTRING        // lstr* string management routines
#define NODBCS           // Double-byte character set routines
#define NOKEYBOARDINFO   // Keyboard driver routines
#define NOGDICAPMASKS    // GDI device capability constants
#define NOCOLOR          // COLOR_* color values
//#define NOGDIOBJ         // GDI pens, brushes, fonts
#define NODRAWTEXT       // DrawText() and related definitions
#define NOTEXTMETRIC     // TEXTMETRIC and related APIs
#define NOSCALABLEFONT   // Truetype scalable font support
//#define NOBITMAP         // Bitmap support
#define NORASTEROPS      // GDI Raster operation definitions
#define NOMETAFILE       // Metafile support
#define NOSYSMETRICS     // GetSystemMetrics() and related SM_* definitions
#define NOSYSTEMPARAMSINFO //SystemParametersInfo() and SPI_* definitions
#define NOMSG            // APIs and definitions that use MSG structure
#define NOWINSTYLES      // Window style definitions
#define NOWINOFFSETS     // Get/SetWindowWord/Long offset definitions
#define NOSHOWWINDOW     // ShowWindow and related definitions
#define NODEFERWINDOWPOS  // DeferWindowPos and related definitions
#define NOVIRTUALKEYCODES // VK_* virtual key codes
#define NOKEYSTATES       // MK_* message key state flags
#define NOWH              // SetWindowsHook and related WH_* definitions
#define NOMENUS           // Menu APIs
#define NOSCROLL          // Scrolling APIs and scroll bar control
#define NOCLIPBOARD       // Clipboard APIs and definitions
#define NOICONS           // IDI_* icon IDs
#define NOMB              // MessageBox and related definitions
#define NOSYSCOMMANDS     // WM_SYSCOMMAND SC_* definitions
#define NOMDI             // MDI support
#define NOCTLMGR          // Control management and controls
#define NOWINMESSAGES     // WM_* window messages
#define NOHELP            // Help support

#include <windows.h>
#include "object3d.h"
#include "object2d.h"
#include "space3d.h"
#include "view3d.h"



#include <math.h>
#include <mem.h>



#include <gwin2d3d\3drender.h>

#include "rotator3.h"

#ifdef _check_for_double
	int TTriangle::_refcount=0;
	int TLine3d::_refcount=0;
#endif

void TRotator3d::rotate(POINT3D*points,int count){
double _x,_y,_z;
for(int i=0;i<count;i++){
  POINT3D *p=points+i;
 //  _x=p->x*r11+p->y*r21+p->z*r31+r41;
 //  _y=p->x*r12+p->y*r22+p->z*r32+r42;
 //  _z=p->x*r13+p->y*r23+p->z*r33+r43;

     _x=p->x*r.a[0][0]+p->y*r.a[1][0]+p->z*r.a[2][0]+r.a[3][0];
     _y=p->x*r.a[0][1]+p->y*r.a[1][1]+p->z*r.a[2][1]+r.a[3][1];
     _z=p->x*r.a[0][2]+p->y*r.a[1][2]+p->z*r.a[2][2]+r.a[3][2];

	p->x=_x;p->y=_y;p->z=_z;
  }
};

TRotator3d::TRotator3d(ROTATOR3D*_r){
 rotator=*_r;
 double RHO,THETA,cal,sal,cph,sph,cth,sth,cph2,sph2,cth2,sth2,pi,cal1;
 cal=cos(_r->Angle);sal=sin(_r->Angle);
 cal1=1-cal;
 RHO=sqrt(_r->VctSize.x*_r->VctSize.x+_r->VctSize.y*_r->VctSize.y+
	 _r->VctSize.z*_r->VctSize.z);
 pi=4.0*atan(1.0);
 if(RHO==0.0){THETA=0.0;cph=1.0;sph=0.0;}else
	{  if(_r->VctSize.x==0.0)
		  THETA=(_r->VctSize.y>=0.0 ? 0.5*pi :1.5*pi);
		else
		{ THETA=atan(_r->VctSize.y/_r->VctSize.x);
		  if(_r->VctSize.x<0)THETA+=pi;
		}
		cph=_r->VctSize.z/RHO;sph=sqrt(1.0-cph*cph);
	}
	cth=cos(THETA);sth=sin(THETA);
	cph2=cph*cph;sph2=1.0-cph2;
	cth2=cth*cth;sth2=1.0-cth2;

   r.Set();

	r.a[0][0]=(cal*cph2+sph2)*cth2+cal*sth2;    // r11
	r.a[0][1]=sal*cph+cal1*sph2*cth*sth;        // r12
	r.a[0][2]=sph*(cph*cth*cal1-sal*sth);       // r13

	r.a[1][0]=sph2*cth*sth*cal1-sal*cph;        // r21
	r.a[1][1]=sth2*(cal*cph2+sph2)+cal*cth2;    // r22
	r.a[1][2]=sph*(cph*sth*cal1+sal*cth);       // r23

	r.a[2][0]=sph*(cph*cth*cal1+sal*sth);       // r31
	r.a[2][1]=sph*(cph*sth*cal1-sal*cth);       // r32
	r.a[2][2]=cal*sph2+cph2;                    // r33

	//r41=r->VctOrg.x-r->VctOrg.x*r11-r->VctOrg.y*r21-r->VctOrg.z*r31;
	//r42=r->VctOrg.y-r->VctOrg.x*r12-r->VctOrg.y*r22-r->VctOrg.z*r32;
	//r43=r->VctOrg.z-r->VctOrg.x*r13-r->VctOrg.y*r23-r->VctOrg.z*r33;

   r.a[3][0]=_r->VctOrg.x-_r->VctOrg.x*r.Get(1,1)-_r->VctOrg.y*r.Get(2,1)-_r->VctOrg.z*r.Get(3,1);
   r.a[3][1]=_r->VctOrg.y-_r->VctOrg.x*r.Get(1,2)-_r->VctOrg.y*r.Get(2,2)-_r->VctOrg.z*r.Get(3,2);
   r.a[3][2]=_r->VctOrg.z-_r->VctOrg.x*r.Get(1,3)-_r->VctOrg.y*r.Get(2,3)-_r->VctOrg.z*r.Get(3,3);

 };

double _disttoline(POINT3D* p1,POINT3D* p2,POINT3D* p){
double a,b,d,c;
 a=p2->y-p1->y; b=p1->x-p2->x;
 d=sqrt(a*a+b*b);
 if (d){
  c=-p1->x*p2->y+p1->y*p2->x;
  d=fabs(a*p->x+b*p->y+c)/d;
 }else{
   b=p2->y-p->y; a=p2->x-p->x;
   d=sqrt(a*a+b*b);
 }
 return d;
}

TObject3d::~TObject3d(){
 DeleteCollection(primitives);
 if (points)delete points;
 };

UINT32 TObject3d::IsIntersect(PObject po,UINT32 f){
 switch(po->WhoIsIt()){
  case  5:{
		 return ((PGroup3d)po)->IsIntersect(this,f);
		  }break;
  case 10:{
		  PObject3d p=(PObject3d)po;
		  POINT3D tops[8];
		  POINT3D _tops[8];
		  Get8PointsByOrgSize(&(p->origin),&(p->size),tops);
		  CRDSYSTEM crd;

		   crd.type=LOCAL_COORDINATE;
		   crd.hObject=_HANDLE;
		   space3d->PushCrd();
		   space3d->SetCrd(&crd);
		   space3d->LPtoCP(p,tops,_tops,8);
		   space3d->PopCrd();
		   POINT3D _min,_max;
		   _min=_max=_tops[0];
		   for(int i=1;i<8;i++){
			 if(_min.x>_tops[i].x)_min.x=_tops[i].x;else
			  {if(_max.x<_tops[i].x)_max.x=_tops[i].x;}
			 if(_min.y>_tops[i].y)_min.y=_tops[i].y;else
			  {if(_max.y<_tops[i].y)_max.y=_tops[i].y;}
			 if(_min.z>_tops[i].z)_min.z=_tops[i].z;else
			  {if(_max.z<_tops[i].z)_max.z=_tops[i].z;}
		   }
		  _min.x=max(_min.x,origin.x);
		  _min.y=max(_min.y,origin.y);
		  _min.z=max(_min.z,origin.z);

		  _max.x=min(_max.x,(origin.x+size.x));
		  _max.y=min(_max.y,(origin.y+size.y));
		  _max.z=min(_max.z,(origin.z+size.z));

		 return ((_min.x<_max.x) && (_min.y<_max.y) && (_min.z<_max.z));

		  }break;
 }
 return 0;
};

UINT32 TGroup3d::IsIntersect(PObject po,UINT32 f){
for(C_TYPE i=0;i<items->count;i++){
 PObject3d _po=(PObject3d)items->At(i);
 switch(_po->WhoIsIt()){
  case  5:{
		 UINT32 T=((PGroup3d)_po)->IsIntersect(po,f);
		 if(T)return T;
		  }break;
  case 10:{
		  UINT32 T=_po->IsIntersect(po,f);
		  if(T)return T;
		  }break;
 }}
 return 0;
};

void TObject3d::Transform3d(TMatrix&_matrix){
 matrix.Mul(_matrix);
 UpdateInView();
};

void TGroup3d::Transform3d(TMatrix&matrix)
 {C_TYPE i;
  for(i=0;i<items->count;i++)((PObject)items->At(i))->Transform3d(matrix);
 };

TMatrix2*TGroup3d::GetMatrix(TMatrix2* m){
 if(items->count){
   return ((PObject)items->At(0))->GetMatrix(m);
 }
 return NULL;
};

BOOL TObject3d::GetOrg_Size3d(POINT3D & o,POINT3D &s)
 {
  if(pointcount){
   s=size;o=origin;
   return TRUE;
   }else{
    s.x=s.y=s.z=o.x=o.y=o.z=0.0;
    return FALSE;
   }
 };
void TObject3d::_DeletePoint(INT16 num){
 for(C_TYPE i=0;i<primitives->count;i++){
  TPrimitive3d*p=(TPrimitive3d*)primitives->At(i);
  for(INT16 j=0;j<p->count;j++){
	if (p->points[j]>num)p->points[j]--;
   }
 }
};

BOOL TObject3d::Awaken(_SPACE*s){
  space3d=(TSpace3d*)s;
  for(C_TYPE i=0;i<primitives->count;i++){
   TPrimitive3d* p=(TPrimitive3d*)primitives->At(i);
   if(p->material){
    if(space3d->materials){
	 (void*)(p->_mat)=space3d->materials->GetByHandle(p->material);
    }else (void*)(p->_mat)=NULL;
	 if(p->_mat){
	   p->_mat->IncRef(NULL);
	 } } }
  return TRUE;
};
/*
POINT3D TObject3d::SetSize3d(POINT3D p)
 {
	matrix.Size(p);
	UpdateInView();
	return p;
 };
*/
void TObject3d::UpdateSize(int,void *)
 {
	POINT3D *pp;
	POINT3D mx;
	if(pointcount){
	 pp=points;
	origin.x=pp->x;origin.y=pp->y;origin.z=pp->z;
	mx=origin;
	for(C_TYPE i=1;i<pointcount;i++)
	{pp=points+i;

	 if(pp->x<origin.x){origin.x=pp->x;}else{ if(pp->x>mx.x)mx.x=pp->x;}
	 if(pp->y<origin.y){origin.y=pp->y;}else {if(pp->y>mx.y)mx.y=pp->y;}
	 if(pp->z<origin.z){origin.z=pp->z;}else {if(pp->z>mx.z)mx.z=pp->z;}
	}
  size.x=mx.x-origin.x;
  size.y=mx.y-origin.y;
  size.z=mx.z-origin.z;
  }else
	{origin.x=0.0;origin.y=0.0;origin.z=0.0;
	 size=origin;
	}
 };
/*
POINT3D TObject3d::Move3d(POINT3D p3){

 matrix.MoveTo(p3);
 UpdateInView();
 return p3;
};
*/
TObject3d::TObject3d(){
   points=0;
   pointcount=0;
   primitives=new TPrimitives();
   colorvalid=0;
  };
TObject3d::TObject3d(PStream ps,LONG _pos):TObject(ps,_pos){
   colorvalid=0;
   points=0;
   flag=ps->ReadWord();
   primitives=new TPrimitives();
   pointcount=ps->ReadWord();
   if (pointcount){
	  points=new POINT3D[pointcount];
	  ps->Read(points,sizeof(POINT3D)*pointcount);
   }
	INT16 count=ps->ReadWord();
	for(C_TYPE i=0;i<count;i++)primitives->Insert(new TPrimitive3d(ps,NULL));
  if(ps->fileversion>0x0201)matrix.Load(*ps);
 int a=ps->ReadWord();
 if (a!=10)_Error(0);
 UpdateSize();
};
void TObject3d::Store(PStream ps){
 TObject::Store(ps);
 ps->WriteWord(flag);
 ps->WriteWord(pointcount);
 if(pointcount)ps->Write(points,sizeof(POINT3D)*pointcount);
 ps->WriteWord(primitives->count);
 for(C_TYPE i=0;i<primitives->count;i++)((TPrimitive3d*)primitives->At(i))->Store(ps);
 matrix.Store(*ps);
 ps->WriteWord(10);
};
TMatrix2*TObject3d::GetMatrix(TMatrix2*){return &matrix;};
TMatrix2*TObject::GetMatrix(TMatrix2*){return NULL;};
//-----------------------------------*

void TObject3d::Bar3d(double x,double y,double z,COLORREF cl){
};
BOOL TObject3d::IsEmpty(){
return (pointcount==0);
};
//***************************************************************
//***************************************************************
//***************************************************************
int IsLineIntersect(double ax,double ay,double bx,double by,
			double cx,double cy,double dx,double dy,double _em,double &r){
 double b1=(bx-ax),b2=(dy-cy),b3=(by-ay),b4=(dx-cx);
 double d=b1*b2-b3*b4;

 if (fabs(d)>1e-10){
		double b5=(ay-cy),b6=(ax-cx);
		r=(b5*b4-b6*b2)/d;
		double s=(b5*b1-b6*b3)/d;
		if ((r>_em)&&(r<1-_em)&&(s>_em)&&(s<1-_em))
		return 1;
		else return 0;
	}else return 0;
 };
int On_1_Side(POINT3D*base,POINT3D*b1,POINT3D*_t1,POINT3D*_t2){
if (_t1==_t2)return 0;
//return 0;
double a,b;
 a=(b1->x-base->x)*(_t1->y-base->y) - (b1->y-base->y)*(_t1->x-base->x);

 b=(b1->x-base->x)*(_t2->y-base->y) - (b1->y-base->y)*(_t2->x-base->x);
	 if (SGNEqual(a,b))
	 return 1;
	 else return 0;
};
BOOL TObject3d::IsPointInUse(INT16 num)
 {C_TYPE i;
  for (i=0;i<primitives->count;i++)
	{TPrimitive3d*p=(TPrimitive3d*)primitives->At(i);
	 for(INT16 j=0;j<p->count;j++)if (p->points[j]==num)return TRUE;
	 return FALSE;
	}
 };

//3d

BOOL TObject::GetOrg_Size3d(POINT3D &,POINT3D &){};
POINT3D TObject::SetSize3d(POINT3D p){return p;};

// TGroup3d *--------------------------------*
void TObject3d::StoreCoordinates(PStream ps)
 {int i;
  ps->WriteWord(pointcount);
  if (pointcount)ps->Write(points,sizeof(POINT3D)*pointcount);
 };
void TObject3d::LoadCoordinates(PStream ps)
 {int i;
  if (ps->ReadWord()!=pointcount)_Error(EM_internal);
  if (pointcount)ps->Read(points,sizeof(POINT3D)*pointcount);
  UpdateSize();
 };
RECT TObject3d::GetRect(int)
 {RECT r;r.left=0;r.right=0;
  return r;
 };

RECT TGroup3d::GetRect(int)
 {RECT r;r.left=0;r.right=0;
  return r;
 };

BOOL TGroup3d::GetOrg_Size3d(POINT3D & o,POINT3D & s)
 {C_TYPE i;
  POINT3D _o,_s,_r;
  BOOL set=FALSE;
	for(i=0;i<items->count;i++)
	 {PObject po=(PObject)items->At(i);
	  if (set)
	  {po->GetOrg_Size3d(_o,_s);
		o.x=min(o.x,_o.x);
		o.y=min(o.y,_o.y);
		o.z=min(o.z,_o.z);
		_r.x=max(_r.x,_o.x+_s.x);
		_r.y=max(_r.y,_o.y+_s.y);
		_r.z=max(_r.z,_o.z+_s.z);
	  }else
		{if (po->GetOrg_Size3d(o,_s)){
		 _r.x=o.x+_s.x;_r.y=o.y+_s.y;_r.z=o.z+_s.z;
         set=TRUE;
         }
		}
	 }
	  s.x=_r.x-o.x;s.y=_r.y-o.y;s.z=_r.z-o.z;
    return set;
 };
/*
POINT3D TGroup3d::SetSize3d(POINT3D S)
 {
  POINT3D _o,_s;
  GetOrg_Size3d(_o,_s);
  for(C_TYPE i=0;i<items->count;i++)
	{PObject po=(PObject)items->At(i);
	 POINT3D _o1,_s1;po->GetOrg_Size3d(_o1,_s1);
	 _s1.x=S.x*_s1.x/_s.x;
	 _s1.y=S.y*_s1.y/_s.y;
	 _s1.z=S.z*_s1.z/_s.z;
	 _o1.x=_o.x+S.x*(_o1.x-_o.x)/_s.x;
	 _o1.y=_o.y+S.y*(_o1.y-_o.y)/_s.y;
	 _o1.z=_o.z+S.z*(_o1.z-_o.z)/_s.z;
	 po->SetSize3d(_s1);
	 po->Move3d(_o1);
	}
  return _o;
 };
*/
//*----------------------------------------------------------------------------*
TFrame3d::TFrame3d(POINT2D _ps,TView3d_2d * _v,PObject3d po)
 {view =_v;
  psize=_ps;
  object=po;
  object->GetOrg_Size3d(origin,size);
  flag=0;UpdateSize();
  _HANDLE=OID_FRAME3d;
 };
/*
POINT3D TFrame3d::Move3d(POINT3D org)
 {POINT3D _p=size;
  origin=org;
  flag|=2;
  UpdateSize();
  return _p;
 };
void TFrame3d::Rotate3d(TRotator3d*r)
 {
  flag|=1; rotator=(r->rotator); UpdateSize();
 };
*/
BOOL TFrame3d::GetOrg_Size3d(POINT3D & o,POINT3D & s)
 {o=origin;s=size;
  return TRUE;
 };
/*
POINT3D TFrame3d::SetSize3d(POINT3D _size)
 {POINT3D p=size;
  flag|=4;
  size=_size;return p;
 };
*/
TAxis3d::TAxis3d(POINT3D _ps,int as,TView3d_2d * aview)
 {origin=_ps;axisSize=as;view=aview;
  _HANDLE=OID_AXIS3d;
  UpdateSize();
  bitmap=LoadBitmap(hInstance,MAKEINTRESOURCE(BITMAP_1));
 };
TAxis3d::~TAxis3d()
 {DeleteObject(bitmap);
 };
/*
POINT3D TAxis3d::Move3d(POINT3D o)
 {POINT3D old=origin;origin=o;UpdateSize();
  return old;
 };

void TAxis3d::Rotate3d(TRotator3d *r)
 {
   r->rotate(&origin,1);
   UpdateSize();
 };
*/
BOOL TAxis3d::GetOrg_Size3d(POINT3D &o,POINT3D &s)
  {o=origin;s.x=0;s.y=0;s.z=0;
   return TRUE;
  };

BOOL TObject3d::ChkNewPoints(INT16 *pn,INT16 count){
 if(count<1 || count>MAX_IN_PRIMITIVE)return FALSE;
 for(INT16 i=0;i<count;i++){
   if (pn[i]<0 || pn[i]>=pointcount)return 0;
 }
 return TRUE;
};
BOOL TObject3d::GetSolidColor(COLOR3D&c){
if(!colorvalid){
if(primitives->count){
 long r,g,b,a;
 for(C_TYPE i=0;i<primitives->count;i++){
  PRIMITIVE3D *p=(PRIMITIVE3D *)primitives->At(i);
  if(i==0){
   r=p->color.c.r;
   g=p->color.c.g;
   b=p->color.c.b;
   a=p->color.c.a;
  }else{
   r+=p->color.c.r;
   g+=p->color.c.g;
   b+=p->color.c.b;
   a+=p->color.c.a;
  }
 }
color.c.r=r/primitives->count;
color.c.g=g/primitives->count;
color.c.b=b/primitives->count;
color.c.a=a/primitives->count;

colorvalid=1;

}else return FALSE;
}
c=color;
return TRUE;
};

void TObject3d::SetColor3d(COLOR3D _color){
 colorvalid=1;
 color=_color;
 for(C_TYPE i=0;i<primitives->count;i++){
  PRIMITIVE3D *p=(PRIMITIVE3D *)primitives->At(i);
  p->color=_color;
 };
 UpdateInView(UIV_PRIMITIVE);
};

 void TObject3d::MarkToSave(int m)
  {TObject::MarkToSave(m);
	for(C_TYPE i=0;i<primitives->count;i++){
    TPrimitive3d *_m=(TPrimitive3d *)primitives->At(i);
    if(_m->_mat)MarkToolsToSave(_m->_mat,m);
   }
  };

BOOL TObject3d::ChkNewPrimitives(INT16 *pn,INT16 count){
 for(INT16 i=0;i<count;i++){
   if (pn[i]<0 || pn[i]>=primitives->count)return 0;
  }
 return TRUE;
};
void TPrimitives::FreeItem(pointer p){
 delete (TPrimitive3d*)p;
};
void Get8PointsByOrgSize(POINT3D* origin,POINT3D* size,POINT3D *tops){
  for (int i=0;i<8;i++)
	 {tops[i]=*origin;if(i>3)tops[i].z+=size->z;
	  if (i & 2)     tops[i].y+=size->y;
	  if ((i+5) & 2) tops[i].x+=size->x;
  }
};

TPrimitive3d::TPrimitive3d(PRIMITIVE3D*p,TSpace3d*sp){
 memset(this,0,sizeof(TPrimitive3d));
 _Set(p,sp);
};
/*
Установка примитивов
Замена точек идет только если указатель на них не равен NULL
*/
BOOL TPrimitive3d::Set(PRIMITIVE3D*p,TSpace3d*sp){
 if ((p->points||(p->count==0)) && points)delete points;
 if ((p->texturePOINTS||
     (p->count==0)||
     ((flags&PRIMITIVE_TEXTURED)==0))&&texturePOINTS){delete texturePOINTS;texturePOINTS=NULL;}
 if (_mat){
  _mat->DecRef(NULL);
  _mat=NULL;
 }
 _Set(p,sp);
 return TRUE;
};

void TPrimitive3d::_Set(PRIMITIVE3D*p,TSpace3d*sp){
 flags=p->flags;
 color=p->color;

 if(p->points){
  count=p->count;
  points=new INT16[count];
  memcpy(points,p->points,2*count);
 }

 material=p->material;
 if(material){
  (void*)(_mat)=sp->materials->GetByHandle(material);
  if(_mat){
   _mat->IncRef(NULL);
  }
 }
  if ((flags&PRIMITIVE_TEXTURED)&&(p->texturePOINTS)){
   texturePOINTS=new POINT2D[count];
   memcpy(texturePOINTS,p->texturePOINTS,sizeof(POINT2D)*count);
   };
};
TPrimitive3d::TPrimitive3d(PStream ps,TSpace3d*sp){
 _mat=NULL;
 count=ps->ReadWord();
 material=ps->ReadWord();
 flags=ps->ReadWord();
 ps->Read(&color,sizeof(COLOR3D));
 points=new INT16[count];
 ps->Read(points,sizeof(INT16)*count);
 if (flags&PRIMITIVE_TEXTURED ){
 texturePOINTS=new POINT2D[count];
 ps->Read(texturePOINTS,sizeof(POINT2D)*count);
 }else texturePOINTS=NULL;
};

void TPrimitive3d::Store(PStream ps){
 ps->WriteWord(count);
 ps->WriteWord(material);
 ps->WriteWord(flags);
 ps->Write(&color,sizeof(COLOR3D));
 ps->Write(points,sizeof(INT16)*count);
 if (flags&PRIMITIVE_TEXTURED )ps->Write(texturePOINTS,sizeof(POINT2D)*count);
};

TPrimitive3d::~TPrimitive3d(){
 if (points)delete points;
 if (texturePOINTS)delete texturePOINTS;
 if (_mat){
  _mat->DecRef(NULL);
 }
};


 _TObject3d::_TObject3d(TObject3d*o){
  object=o;
  objflags=object->options;
  points=new TItems(max(10,object->pointcount),20);
  primitives=new TItems(max(10,object->primitives->count),20);
  org.y=org.z=org.x=size.x=size.y=size.x=0;
  flags=0;
  for(C_TYPE i=0;i<object->pointcount;i++){
   points->Insert(new _TPOINT3D);
  }
  for(i=0;i<object->primitives->count;i++){
   primitives->Insert(new _TPRIMITIVE3D);
  }
  boundsinvalid=1;
 };

 _TObject3d::~_TObject3d(){
   DeleteCollection(points);
   DeleteCollection(primitives);
 };
  void TObject3d::UpdateInView(BYTE f){
   if (space3d->views->count){
	 C_TYPE index=space3d->primary->IndexOf(this);
	 for(C_TYPE i=0;i<space3d->views->count;i++){
		TView3d_2d* v=(TView3d_2d*)space3d->views->At(i);
//		_TObject3d*p=(_TObject3d*)v->items->At(index);
//     p->boundsinvalid=1;
	  v->UpdateObject(index,f);
  }}
 };

 void _TObject3d::UpdateMatrix(TView3d_2d*view){
  view->GetMatrix(this);

  offset=view->offset;
  perspective=view->perspective;
  focus=view->focus;
 };


void _TObject3d::GetBoundsRect(POINT3D *_p1,POINT3D *_p2,RECT*r){
 UpdateBounds();
 POINT3D p1,p2;
 p1.x=p2.x=bounds[0].x;
 p1.y=p2.y=bounds[0].y;
 p1.z=p2.z=bounds[0].z;

 for(int i=1;i<8;i++){
  if(bounds[i].x<p1.x)p1.x=bounds[i].x;else{ if(p2.x<bounds[i].x)p2.x=bounds[i].x;}
  if(bounds[i].y<p1.y)p1.y=bounds[i].y;else{ if(p2.y<bounds[i].y)p2.y=bounds[i].y;}
  if(bounds[i].z<p1.z)p1.z=bounds[i].z;else{ if(p2.z<bounds[i].z)p2.z=bounds[i].z;}
 }

if(_p1)*_p1=p1;
if(_p2)*_p2=p2;
if(r){
r->left=p1.x-2;
r->right=p2.x+1;
r->top=p1.y-1;
r->bottom=p2.y+1;
}
};


void _TObject3d::UpdateBounds(){
 if(boundsinvalid){
 POINT3D o,s;
 POINT3D p;
 o=object->origin;
 s=object->size;
   for(int i=0;i<8;i++){
        if(i&1)p.x=o.x+s.x;else p.x=o.x;
        if(i&2)p.y=o.y+s.y;else p.y=o.y;
        if(i&4)p.z=o.z+s.z;else p.z=o.z;
         _Perspective(&p,bounds+i);
   }
  boundsinvalid=0;
 }
};
void _TObject3d::_Perspective(POINT3D*in,POINT3D*out){
  POINT3D dst;
  matrix.Mul(dst,*in);
 if (perspective){
   if(dst.z>0){
    double _z=dst.z/focus+1;
    dst.x=dst.x*_z;
    dst.y=dst.y*_z;
   }  else
   {
    double _z=-dst.z+focus;
    dst.x=dst.x*focus/_z;
    dst.y=dst.y*focus/_z;
   }
   }
   out->x=dst.x+offset.x;
   out->y=dst.y+offset.y;
   out->z=dst.z;
 };
 void _TObject3d::Perspective(C_TYPE i){
  POINT3D dst;
  matrix.Mul(dst,object->points[i]);
  _TPOINT3D* out=(_TPOINT3D*)points->At(i);

  out->__x=dst.x;
  out->__y=dst.y;

  if (perspective){
   if(dst.z>0){
    double _z=dst.z/focus+1;
    dst.x=dst.x*_z;
    dst.y=dst.y*_z;
   }  else
   {
    double _z=-dst.z+focus;
    dst.x=dst.x*focus/_z;
    dst.y=dst.y*focus/_z;
   }

  }
   out->_x=(out->x=(dst.x+offset.x))*256.0;
   out->_y=(out->y=(dst.y+offset.y))*256.0;
   out->_z=(out->z=dst.z)*256.0;
 };

 void _TObject3d::PerspectiveAll(){
   for(C_TYPE i=0;i<points->count;i++)Perspective(i);
 };
 BOOL _TObject3d::GetPrimitiveZ(C_TYPE index,double x,double y,double &z){
  PRIMITIVE3D *  t=(PRIMITIVE3D*)object->primitives->At(index);
  _TPRIMITIVE3D * _t=(_TPRIMITIVE3D*)primitives->At(index);
  POINT3D p[2];
  int j=0;
  for(INT16 i=0;i<t->count;i++){
   INT16 n2;
   if (i==t->count-1)n2=0;else n2=i+1;
   _TPOINT3D*_p1=(_TPOINT3D*)points->At(t->points[i]);
   _TPOINT3D*_p2=(_TPOINT3D*)points->At(t->points[n2]);
      if (_p2->y>_p1->y){ _TPOINT3D *_pn=_p1;_p1=_p2;_p2=_pn; }
    if(_p1->y==y){
      p[j++]=*((POINT3D*)_p1);
        }else{
        if(_p2->y==y){
      p[j++]=*((POINT3D*)_p2);
        }else{
         if ((_p1->y<y)&&(_p2->y>y))
            {
             p[j].x=_p1->x+(y-_p1->y)*(_p2->x-_p1->x)/(_p2->y-_p1->y);
             p[j].z=_p1->z+(y-_p1->y)*(_p2->z-_p1->z)/(_p2->y-_p1->y);
             j++;
            }
          }
       }

    if (j==2)break;
  }
  if (j==2){
    if (p[0].x>p[1].x){POINT3D pb=p[0];p[0]=p[1];p[1]=pb;}

    if (x<=p[0].x){z=p[0].z;return TRUE;}
    if (x>=p[1].x){z=p[1].z;return TRUE;}

    z = p[0].z+(p[1].z-p[0].z)*(x-p[0].x)/(p[1].x-p[0].x);
  }else{
    z=((_TPOINT3D*)points->At(t->points[0]))->z;
  }
  return TRUE;
 };
/*
 Для обновления примитива
 flags -
 objflags -
   STATE_WIREFRAME - если этот объект рисуется проволочным или режим отрисовки такой.

Определение видимости по перспективным координатам.
Если перспектива то
 определяем


*/

 void _TObject3d::UpdatePrimitive(C_TYPE i){
  PRIMITIVE3D *  t=(PRIMITIVE3D*)object->primitives->At(i);
  _TPRIMITIVE3D * _t=(_TPRIMITIVE3D*)primitives->At(i);

  BYTE wire=0;
  if (objflags&(STATE_EDIT3D|STATE_WIREFRAME)){ _t->flags.visible=1; wire=1; };

  if ((t->count>2)&&(t->flags&PRIMITIVE_POLYGON)){
// проверка железной видимости
  BYTE v=(t->flags&PRIMITIVE_ALWAYS)||(objflags&STATE_ALLFACES);
// проверка железной видимости
//  wire=0;
  _TPOINT3D* t1 =(_TPOINT3D*)points->At(t->points[0]);
  _TPOINT3D* t2 =(_TPOINT3D*)points->At(t->points[1]);
//  _TPOINT3D* t3 =(_TPOINT3D*)points->At(t->points[2]);
  _TPOINT3D* tL =(_TPOINT3D*)points->At(t->points[t->count-1]);
  float a,b,c,d;
 if(v)goto m1;
 if(perspective){ // это толко определение видимости при перспективе
 // Нужно определить видимость
  float _v1=t2->_x-t1->_x;
  float _v2=t2->_y-t1->_y;
  float _w1=tL->_x-t1->_x;
  float _w2=tL->_y-t1->_y;
  c=_v1*_w2-_v2*_w1;
  v=(c<0.0);
  if(!(v || (flags&O3D_CALCALLFACES))){_t->flags.visible=0;return;}
 }
m1:
   { // считается нормальный C для рассчета освещения
    float _v1=t2->__x-t1->__x;
    float _v2=t2->__y-t1->__y;
    float _w1=tL->__x-t1->__x;
    float _w2=tL->__y-t1->__y;
     c=_v1*_w2-_v2*_w1;
     v|=(c<0.0);
   }
final:
  _t->flags.visible=v;
  c=fabs(c);

//  if (v || (flags&O3D_CALCALLFACES))
  {

    float c1=t2->z-tL->z;
    float c2=t1->z-tL->z;
    float c3=t1->z-t2->z;

	a=(t1->y*(c1) - t2->y*(c2) + tL->y*(c3));
	b=-(t1->x*(c1)-t2->x*(c2) + tL->x*(c3));

   float r=sqrt(a*a+b*b+c*c);

	if (c<0.0){r=-r;}
    long _c;
	if (r!=0.0){
	 //a=a/r;b=b/r;
//     c=c/r;
    _c=65536L*(c/r);
     //d=d/r;
	 }else _c=65536L;
   COLOR3D color=t->color;
   color.c.r=HIWORD(_c*color.c.r);
   color.c.g=HIWORD(_c*color.c.g);
   color.c.b=HIWORD(_c*color.c.b);

   _t->c=_c;
   _t->color=color;
   //_t->a=a;_t->b=b;_t->c=c;_t->d=d;

   _t->minz=_t->maxz=t1->_z;

   for(INT16 j=1;j<t->count;j++){
	_TPOINT3D* tt =(_TPOINT3D*)points->At(t->points[j]);
	if (tt->_z<_t->minz)_t->minz=tt->_z;else
	 { if (tt->_z>_t->maxz)_t->maxz=tt->_z;
	 }
   }
  }
	}else { // примитив сам по себе линии
         _t->color=t->color;
			_t->flags.visible=1;
         wire=1;
	    }
	  _t->flags.wireframe=wire;
 };
 void _TObject3d::UpdateAllPrimitives(){
  C_TYPE pcount=primitives->count;
  for(C_TYPE i=0;i<pcount;i++)UpdatePrimitive(i);
 };

 void _TObject3d::UpdatePoints(BYTE mode){
  C_TYPE pointcount=points->count;
  for(C_TYPE i=0;i<pointcount;i++)
	((_TPOINT3D*)points->_At(i))->count=0;

  C_TYPE pcount=primitives->count;

  if(mode==1){
  for(i=0;i<pcount;i++){
  PRIMITIVE3D *  t=(PRIMITIVE3D*)object->primitives->_At(i);
  _TPRIMITIVE3D * _t=(_TPRIMITIVE3D*)primitives->_At(i);
  if (_t->flags.visible || (flags&O3D_CALCALLFACES)){
   for(INT16 j=t->count-1;j>-1;j--){
    COLOR3D tc=_t->color;
	_TPOINT3D* point =(_TPOINT3D*)points->At(t->points[j]);
	if (point->count==0){
	  point->color=tc;
	}else{
	int count=point->count;
	int count_1=count+1;
     COLOR3D cc=point->color;
	 cc.c.r=(count*cc.c.r+tc.c.r)/(count_1);
	 cc.c.g=(count*cc.c.g+tc.c.g)/(count_1);
	 cc.c.b=(count*cc.c.b+tc.c.b)/(count_1);
     point->color=cc;
	}
	point->count++;
   }}}
   }else{
    for(i=0;i<pcount;i++){
  PRIMITIVE3D *  t=(PRIMITIVE3D*)object->primitives->_At(i);
  _TPRIMITIVE3D * _t=(_TPRIMITIVE3D*)primitives->_At(i);
  if (_t->flags.visible || (flags&O3D_CALCALLFACES)){
       for(INT16 j=t->count-1;j>-1;j--){

	_TPOINT3D* point =(_TPOINT3D*)points->At(t->points[j]);
	if (point->count==0) point->c=_t->c;
	                  else point->c+=_t->c;
	point->count++;
   }
    }
   }// for
   for(C_TYPE i=0;i<pointcount;i++){
	_TPOINT3D* point=((_TPOINT3D*)points->_At(i));
     if(point->count)point->c/=point->count;
   }
  }
 };

 TLight3d::TLight3d(LIGHT3D*l):TObject(){
  light=*l;
 };
 TLight3d::~TLight3d(){
 };
 TLight3d::TLight3d(PStream ps,LONG _pos):TObject(ps,_pos){
   int l=ps->ReadWord();
   memset(&(light),0,sizeof(light));
   light.lSize=sizeof(light);
   ps->Read(&(light.lType),l-2);
 };

 void TLight3d::Store(PStream ps){
  TObject::Store(ps);
  ps->WriteWord(sizeof(light));
  ps->Write(&light.lType,sizeof(light)-2);
 };

BOOL TLight3d::GetOrg_Size3d(POINT3D &o,POINT3D &s){
  if (&s){  s.x=s.y=s.z=0; }
  if (&o){  o=light.lOrg; }
  return TRUE;
 };

  POINT3D TLight3d::Move3d(POINT3D org){
  POINT3D old=light.lOrg;
  light.lOrg=org;
  for(C_TYPE i=0;i<space3d->views->count;i++){
   TView3d_2d*v=(TView3d_2d*)space3d->views->At(i);
   if (v->IsCompatible(LIGHT_COMPATIBLE)) v->UpdateRect(NULL);
  }
   return old;
 };

 void T3dObjects::FreeItem(pointer p){
  delete (_TObject3d*)p;
 };
/*
void TCamera3d::Perspective(POINT3D*in,POINT3D*out,int count){

};
*/
//Michele Mercier
