/*
Copyright (c) 1995  Virtual World
Module Name:
	 _system3.cpp
Author:
	 Alexander Shelemekhov
*/

#include "_system.h"

#include <gwin2d3d\3drender.h>
#include <mem.h>
#include "camera3d.h"
#include "rotator3.h"
#include <math.h>
#define __GETSPACE3D PSpace2d ps2; \
  PSpace3d ps=Get3dSpace(hSpace3d,ps2);
#define SetModify ps2->modifyed=1;

extern PCollection spaces;
int pnpoly(int npol, long *xp, long *yp, long x, long y)
 {
	int i, j, c = 0;
	for (i = 0, j = npol-1; i < npol; j = i++) {
		if ((((yp[i]<=y) && (y<yp[j])) ||
		 ((yp[j]<=y) && (y<yp[i]))) &&
		 (x < (xp[j] - xp[i]) * (y - yp[i]) / (yp[j] - yp[i]) + xp[i]))
		  c = !c;
	  }
	 return c;
	}

PObject3d GetObj3d( HSP3D hSpace3d ,HOBJ2D obj,int type=0)
 {__GETSPACE3D
  if (ps) {
	 PObject3d po=(PObject3d)ps->GetObjByHANDLE(obj);
	if (po){
	 if (type)
		 {if (type==po->WhoIsIt()) return po;
		 }else return po;
	  }
  }
	return NULL;
 }
_EXTERNC INT16 _EXPORT GetNextRenderDriver3d(INT16 prev,LPSTR descr,int m){
if (prev){prev++;}else prev=1;
char s[256];
if (!descr){descr=s;m=255;}
int r=LoadString(hInstance,RENDER_TYPES+prev,descr,m);
if (!r){lstrcpy(descr,"");prev=0;}
return prev;
};

_EXTERNC int _EXPORT GetUsageSpace3d( HSP3D hSpace3d )
 {__GETSPACE3D
  if (ps)return ps->views->count;
  return 0;
 };

_EXTERNC int _EXPORT DeleteSpace3d( HSP3D hSpace3d )
 {__GETSPACE3D
  if (ps2 && ps && ps->views->count==0){
	if (ps2->record)
	{if (ps->views->count)return 0;
	 ps2->NewMetaRecord3d(mfdeletespace3d,ps->_HANDLE);
	 ps2->NewUndoRecord(mfloadspace3d);
	 ps2->meta->Put(ps);
	 ps2->CloseMetaRecord();
	}

 // INT16 i=spaces->IndexOf(ps);
 // spaces->AtPut(i,NULL);
  ps2->spaces3d->Delete(ps);
  delete ps;
  SetModify
  if (ps2->record)_Error(EM_internal);
  }
	return TRUE;
 };

_EXTERNC int _EXPORT DeleteEmptyObjects3d( HSP3D hSpace3d ) // return сколько удалили
 {__GETSPACE3D
  if(ps){
	 int h;
	 if (ps2->record) h= ps->DeleteEmptyObjects(ps2);
	  else h= ps->DeleteEmptyObjects(NULL);
	  if(h)SetModify
	  return h;
	 }
	return 0;
 };           
_EXTERNC INT32 _EXPORT GetNumPoints3d( HSP3D hSpace3d,HOBJ2D Object3d )
 { PObject3d po=GetObj3d(hSpace3d,Object3d,0);
  if(po)switch(po->WhoIsIt()){
  case 5:{
		  INT32 count=0;
		   PGroup pg=(PGroup)po;
		   for(C_TYPE i=0;i<pg->items->count;i++){
			count+=GetNumPoints3d(hSpace3d,((PObject)pg->items->At(i))->_HANDLE);
		   }
		  return count;
		 }
  case 10:return po->pointcount;
  case 11:return 5;
  case 52:return 8;
  case 53:return 1;
  }
	 return -1;
 };
_EXTERNC INT32 _EXPORT GetNumPrimitives3d(HSP3D hSpace3d,HOBJ2D Object3d )
 { PObject3d po=GetObj3d(hSpace3d,Object3d,0);
  if(po)switch(po->WhoIsIt()){
  case 5:{
		  INT32 count=0;
		   PGroup pg=(PGroup)po;
		   for(C_TYPE i=0;i<pg->items->count;i++){
			count+=GetNumPrimitives3d(hSpace3d,((PObject)pg->items->At(i))->_HANDLE);
		   }
		  return count;
		 }
  case 10:return po->primitives->count;
  }
   return -1;
 };



_EXTERNC BOOL _EXPORT GetPoints3di( HSP3D hSpace3d,HOBJ2D Object3d,
						   POINT3D * points,INT16*nums,INT16 count )
 { PObject3d po=GetObj3d(hSpace3d,Object3d,0);
	if (po && count)
	switch (po->WhoIsIt()){
	case 10:{
	  for(INT16 i=0;i<count;i++)
	  if (nums[i]>=0 && nums[i]<po->pointcount)
	  {	points[i]=po->points[nums[i]];  }

	 return TRUE;
	}
   case 11:{
     BOOL rez=FALSE;
	  for(INT16 i=0;i<count;i++)
	  if (nums[i]>=0)
	  { rez|=((TCamera3d*)po)->GetPoint(points[i],nums[i]);  }
	 return rez;
   }
	case 52:{
		  for(INT16 i=0;i<count;i++)
	  if (nums[i]>=0 && nums[i]<8)
		points[i]=((PFrame3d)po)->tops[nums[i]];
	 return TRUE;
	}
	case 53:
	{points[0]=((PAxis3d)po)->tops[0];
	 return TRUE;
	}}
  return FALSE;
 };
_EXTERNC BOOL _EXPORT GetPoints3d( HSP3D hSpace3d,HOBJ2D Object3d,
						   POINT3D * points,INT16 from,INT16 count )
 { PObject3d po=GetObj3d(hSpace3d,Object3d,0);
	if (po && count){
	INT16 t=po->WhoIsIt();
	switch (t){
	case 10:if ((from>=0) && ((from+count)<=po->pointcount)){
	  for(INT16 i=0;i<count;i++)points[i]=po->points[from +i];
	 return TRUE;
	}return FALSE;
   case 11:{
     BOOL rez=FALSE;
	  for(INT16 i=0;i<count;i++)
	  if (from>=0)
	  { rez|=((TCamera3d*)po)->GetPoint(points[i],from +i);  }
	 return rez;
   }
	case 52:if (from >=0 && from +count<=8){
	   for(INT16 i=0;i<count;i++)
		   points[i]=((PFrame3d)po)->tops[from +i];
	 return TRUE;
	}return FALSE;
	case 53:
	{points[0]=((PAxis3d)po)->tops[0];
	 return TRUE;
	}}}
  return FALSE;
 };


_EXTERNC INT32 _EXPORT GetNumSpacePrimitives3d( HSP3D hSpace3d )
 {__GETSPACE3D
	if(ps)
	{int i;long c=0;
	 for(i=0;i<ps->primary->count;i++)
	 {PObject3d po=(PObject3d)(ps->primary->At(i));
	  c=c+(long)po->primitives->count;
	 }
	 return c;
	}
  return -1;
 };

_EXTERNC INT32 _EXPORT GetNumSpacePoints3d( HSP3D hSpace3d )
 {__GETSPACE3D
	if(ps)
	{int i;long c=0;
	 for(i=0;i<ps->primary->count;i++)
	 {PObject3d po=(PObject3d)(ps->primary->At(i));
	  c=c+(long)po->pointcount;
	 }
	 return c;
	}
  return -1;
 };


_EXTERNC int _EXPORT GetNumPrimary3d( HSP3D hSpace3d )
 {__GETSPACE3D
	if(ps) return ps->primary->count;
  return -1;
 };
_EXTERNC INT16 _EXPORT GetNumSecondary3d( HSP3D hSpace3d )
 {__GETSPACE3D
	if(ps) return ps->all->count-ps->primary->count;
  return -1;
 };
_EXTERNC BOOL _EXPORT GetObjectBase3d( HSP3D hSpace3d,HOBJ2D Object3d, POINT3D * Base){
__GETSPACE3D
  if (ps) {
	 PObject3d po=(PObject3d)ps->GetObjByHANDLE(Object3d);
     if(po){
       return ps->GetObjectBase(po,Base);
      }}
 return FALSE;
};

_EXTERNC BOOL _EXPORT SetObjectBase3d( HSP3D hSpace3d,HOBJ2D Object3d, POINT3D * Base){
__GETSPACE3D
  if (ps){
    return ps->SetObjectBase3d(Object3d,Base);
   }
 return FALSE;
};

_EXTERNC BOOL _EXPORT GetObjectOrgSize3d( HSP3D hSpace3d,HOBJ2D Object3d, POINT3D * Origin, POINT3D * Size )
{PObject3d po=GetObj3d(hSpace3d,Object3d );
  if(po)
   {if ((po->WhoIsIt()==52)&&(Object3d!=OID_FRAME3d))
      {* Origin=((PFrame3d)po)->tops[Object3d-OID_FRAME3d1];
	      return 1;
      }
   POINT3D t3d,_t3d;

   if (po->GetOrg_Size3d(t3d,_t3d)){

    if (Origin)*Origin=t3d;
    if (Size)*Size=_t3d;
	 return TRUE;
   }

  };
  if (Origin) memset(Origin,0,sizeof(POINT3D));
  if (Size)   memset(Size,0,sizeof(POINT3D));
  return FALSE;
};


_EXTERNC HGLOBAL _EXPORT CopyToMemory3d(HSP3D hSpace3d,HOBJ2D Object3d,INT32 *size)
 {__GETSPACE3D
  HGLOBAL hg=0;
  if (ps)
  {
  	TMemoryStream mstream(FALSE);

    if (ps->_StoreObject(&mstream,Object3d)){
     	 mstream.Truncate();
       hg=mstream.GetHANDLE();
		 if (size)*size=mstream.GetSize();
       return hg;
    }else{
     hg=mstream.GetHANDLE();
    }

 }
  if(hg)GlobalFree(hg);
   return 0;
 }

_EXTERNC BOOL _EXPORT CopyToClipboard3d(HSP3D hSpace3d,HOBJ2D Object3d)
 {__GETSPACE3D
  if (ps)
  {BOOL h=FALSE;
	 if(OpenClipboard(ps2->hwnd)){
		EmptyClipboard();
		 HGLOBAL hg=CopyToMemory3d(hSpace3d,Object3d,NULL);
		 if (!SetClipboardData(CFormat_3D,hg))GlobalFree(hg);else h=TRUE;
		}
		CloseClipboard();
		return h;
	  }
  return FALSE;
 };
_EXTERNC HOBJ2D _EXPORT PasteFromMemory3d(HSP3D  hSpace3d,HGLOBAL hg,POINT3D * org)
 {__GETSPACE3D
  if (ps)
  {HOBJ2D h=0;
	 TMemoryStream mstream(hg);
	  if (!mstream.status)h=ps->PasteObject(&mstream,org);
		if(h){
		if (ps2->record)
		{ps2->NewMetaRecord3d(mfpasteobject3d,ps->_HANDLE);
		 if (org){ps2->meta->WriteWord(255);ps2->meta->Write(org,sizeof(POINT3D));}
			  else ps2->meta->WriteWord(0);
		 long l=mstream.GetPos();
		 mstream.CopyTo(ps2->meta,0,l);
		 ps2->NewUndoRecord3d(mfdeleteobject3d,ps->_HANDLE);
		 ps2->meta->WriteWord(h);
		 ps2->CloseMetaRecord();
		}SetModify}
	 return h;
  }
  return 0;
 };
_EXTERNC HSP2D  _EXPORT GetSpace2dBy3d(HSP3D hSpace3d){
 __GETSPACE3D
  if(ps)return ps->space2d->_HANDLE;
  return 0;
};
_EXTERNC HOBJ2D _EXPORT PasteFromClipboard3d(HSP3D  hSpace3d,POINT3D * org)
 {__GETSPACE3D
  if (ps&&IsClipboardFormatAvailable(CFormat_3D) && OpenClipboard(ps2->hwnd))
  {HOBJ2D h=PasteFromMemory3d(hSpace3d,GetClipboardData(CFormat_3D),org);
		CloseClipboard();
	 return h;
  }
  return 0;
 };
_EXTERNC INT16 _EXPORT GetObjectColor3d( HSP3D hSpace3d,HOBJ2D Object3d,COLOR3D*c)
{__GETSPACE3D
  if (ps)return ps->GetObjectColor3d(Object3d,*c);
  return -1;
};

_EXTERNC BOOL _EXPORT SetObjectColor3d( HSP3D hSpace3d,HOBJ2D Object3d,COLOR3D* cl )
{__GETSPACE3D
  if (ps)
  {if ((ps2->record)&&(Object3d<OID_RESERVED)&&
			  ps->all->GetByHandle(Object3d)>0)
		{ps2->NewMetaRecord3d(mfsetcolor3d,ps->_HANDLE);
		 ps2->meta->WriteWord(Object3d);
		 ps2->meta->Write(&cl,sizeof(COLORREF));
		 ps2->NewUndoRecord3d(mfloadcolor3d,ps->_HANDLE);
		 ps->StoreColors3d(ps2->meta,Object3d);
		 ps2->CloseMetaRecord();
	 }
	 int h=ps->SetObjectColor3d(Object3d,*cl);
	 if(h)SetModify
	 return h;
  }
  return 0;
};
_EXTERNC BOOL _EXPORT SetObjectOrg3d( HSP3D hSpace3d,HOBJ2D Object3d, POINT3D * Origin )
{__GETSPACE3D
  if (ps)
  {POINT3D org1=*Origin;POINT3D org2=*Origin;
	BOOL h=ps->SetObjectOrigin3d(Object3d,&org1);
	if((h)&&(Object3d<OID_RESERVED)){
	if (ps2->record)
		{ps2->NewMetaRecord3d(mfsetobjectorg3d,ps->_HANDLE);
		 ps2->meta->WriteWord(Object3d);
		 ps2->meta->Write(&Origin,sizeof(Origin));
		 ps2->NewUndoRecord3d(mfsetobjectorg3d,ps->_HANDLE);
		 ps2->meta->WriteWord(Object3d);
		 ps2->meta->Write(&org1,sizeof(org1));
		 ps2->CloseMetaRecord();
		}
	  SetModify}
	 return h;
  }
  return 0;
};
_EXTERNC BOOL _EXPORT SetObjectSize3d( HSP3D hSpace3d,HOBJ2D Object3d, POINT3D * Size )
{if (Size->x>=0 && Size->y>=0 && Size->z>=0){
 __GETSPACE3D
  if (ps)
  {if (ps2->record) // 1 meta
  {PObject3d po=(PObject3d)ps->GetObjByHANDLE(Object3d);
	if ((po)&&(Object3d<OID_RESERVED))
	{ps2->NewMetaRecord3d(mfsetobjectsize3d,ps->_HANDLE);
	 ps2->meta->WriteWord(Object3d);
	 ps2->meta->Write(Size,sizeof(POINT3D));
	 ps2->NewUndoRecord3d(mfloadcoordinates3d,ps->_HANDLE);
	 ps2->meta->WriteWord(Object3d);
	 po->StoreCoordinates(ps2->meta);
	 ps2->CloseMetaRecord();
	}
  }//2 meta
  BOOL h=ps->SetObjectSize3d(Object3d,Size);
  if(h&&(Object3d<OID_RESERVED))SetModify
 return h;
  }}
 return 0;
};
_EXTERNC BOOL _EXPORT RotatePoint3d(ROTATOR3D * r,POINT3D *p,INT16 count)
 {
  TRotator3d rotator(r);
  rotator.rotate(p,count);
  return TRUE;
 }
_EXTERNC BOOL _EXPORT TransformObjectPoints3d( HSP3D hSpace3d,HOBJ2D Object3d, gr_float*m){
 __GETSPACE3D
  if (ps)
  {
  int h=ps->TransformObjectPoints3d(Object3d,m);
  if(h&&(Object3d<OID_RESERVED))SetModify
   return h;
  }
 return 0;
};
_EXTERNC BOOL _EXPORT RotateObjectPoints3d( HSP3D hSpace3d,HOBJ2D Object3d,ROTATOR3D * rotator){
 __GETSPACE3D
  if (ps)
  {
 TRotator3d _r(rotator);
  int h=ps->TransformObjectPoints3d(Object3d,(double*)(&_r.r.a));
  if(h&&(Object3d<OID_RESERVED))SetModify
   return h;
  }
 return 0;
};



_EXTERNC BOOL _EXPORT TransformObject3d( HSP3D hSpace3d,HOBJ2D Object3d,gr_float* m){
 __GETSPACE3D
  if (ps)
  {if (ps){
  int h=ps->Transform3d(Object3d,m);
  if(h&&(Object3d<OID_RESERVED))SetModify
   return h;

   }
  }
 return 0;
};
_EXTERNC BOOL _EXPORT RotateObject3d( HSP3D hSpace3d,HOBJ2D Object3d, ROTATOR3D * rotator )
{__GETSPACE3D
  if (ps)
  {if ((ps2->record)&&(Object3d<OID_RESERVED))
  {PObject3d po=(PObject3d)ps->GetObjByHANDLE(Object3d);
	if (po)
	{ps2->NewMetaRecord3d(mfrotateobject3d,ps->_HANDLE);
	 ps2->meta->WriteWord(Object3d);
	 ps2->meta->Write(rotator,sizeof(ROTATOR3D));
	 ps2->NewUndoRecord3d(mfloadcoordinates3d,ps->_HANDLE);
	 ps2->meta->WriteWord(Object3d);
	 po->StoreCoordinates(ps2->meta);
	 ps2->CloseMetaRecord();
	}
  }
  int h=ps->RotateObject3d(Object3d,rotator);
  if(h&&(Object3d<OID_RESERVED))SetModify
 return h;
  }
 return 0;
};
// View/Edit 3D Object Projection

BOOL __TestObject3d(PView3d_2d pv,_TObject3d*po,POINT2D pt,INT16 &_prim,double &z){
	  BOOL in=0;
	  int x,y;
	  long _x,_y;
	  x=pt.x; y=pt.y;
	  _x=pt.x*256;
	  _y=pt.y*256;
	  long __x=pt.x;
	  long __y=pt.y;
	  if ((pv->space2d->IsVisible(po->object)) &&
			((po->object->options &OF_HARDDONTSELECT)==0)&&
			((po->object->options & OF_DONTSELECT)?(pv->space2d->State&SF_DONTSELECT):1)
		  ){

	 if ((pt.x>=po->org.x) && (pt.x<=(po->org.x+po->size.x)) &&
		 (pt.y>=po->org.y) && (pt.y<=(po->org.y+po->size.y)))
		  {
				PObject3d obj=po->object;
			for(C_TYPE j=0;j<po->primitives->count;j++)
			  {_TPRIMITIVE3D * r=(_TPRIMITIVE3D *)po->primitives->At(j);
				 if ( r->flags.visible ){
				 /*
				   if ((r->rect.left<=x)&&(r->rect.right<=x)&&
				   (r->rect.top<=y)&&(r->rect.bottom)  ) */
				   {

					PRIMITIVE3D*primitive=(PRIMITIVE3D*)obj->primitives->At(j);
					long xx[20];
					long yy[20];
					INT16 count=min(primitive->count,19);
					BYTE flags=0;
					 for(int k=0;k<count;k++){
					  _TPOINT3D * _point=(_TPOINT3D*)po->points->At(primitive->points[k]);
					  xx[k]=_point->_x;
					  yy[k]=_point->_y;

					  if(_point->_x>=_x)flags|=1;
					  if(_point->_x<=_x)flags|=2;
					  if(_point->_y>=_y)flags|=4;
					  if(_point->_y<=_y)flags|=8;

					 }
             if ((flags&0xf)==0xf){
				 if (primitive->flags&PRIMITIVE_POLYGON){
					if(pnpoly(count,xx,yy,_x,_y))
					{ m1:
					 {double zz=0;
                    if (po->GetPrimitiveZ(j,pt.x,pt.y,zz)){
					  if (in){
					   if (z<zz){  zz=z;_prim=j;}
					  }else { z=zz;_prim=j;in=TRUE; }
                      }
					 }
					}else{
					long minx,maxx,miny,maxy;
				   for(int k=0;k<count;k++){
					 _TPOINT3D * _point=(_TPOINT3D*)po->points->At(primitive->points[k]);
					 if(k==0){
					  minx=maxx=_point->_x;
					  miny=maxy=_point->_y;
					 }else{
					  minx=min(minx,_point->_x);
					  maxx=max(maxx,_point->_x);
					  miny=min(miny,_point->_y);
					  maxy=max(maxy,_point->_y);
					 }
				   }
				   if (((maxx-minx)<=512)||((maxy-miny)<=512))goto m1;
					 }
				   }else{
                // Тестирование линии
                int c=count-1;
                if(primitive->flags&PRIMITIVE_CLOSELINE)c++;

               for(int k=0;k<c;k++){
               _TPOINT3D * _point1=(_TPOINT3D*)po->points->At(primitive->points[k]);
               _TPOINT3D * _point2=(_TPOINT3D*)po->points->At(primitive->points[k==(count-1)?0:(k+1)]);
                POINT2D p[2];
                p[0].x=_point1->x;
                p[0].y=_point1->y;
                p[1].x=_point2->x;
                p[1].y=_point2->y;
                if(PointNearLine(p[0],p[1],pt)){
                   double zz=max(_point1->z,_point2->z);
                 if (in){
					   if (z<zz){  zz=z;_prim=j;}
					  }else { z=zz;_prim=j;in=TRUE; }
                      }

                 }


                   }
                 } // тест по прямоуголуьной оболочке примитва
               }
			 }}// по
		  }// тест по прямоуголуьным оболочкам
      } // тест по флагам
  return in;
}

HOBJ2D __Get_Obj_By_Point(PView3d_2d pv,POINT2D p,INT16*_primitive=NULL)
 {	  if (pv && pv->WhoIsIt()==24)
		{
		 C_TYPE i;
		 HOBJ2D obj=0;
		  INT16 _pprim=0;
		 double z=0.0;
		 //
		 POINT2D pt=pv->FromLP2d(p,FALSE);
		 i=pv->space2d->GetFrame3dCursor(pt);
//		 pt=pv->FromLP2d(p);
		 if (i)return i;
		 // Проверка на попадание в Frame3d и Axis3d
		 if ((p.x >=pv->origin.x &&
	  p.x <=pv->origin.x+pv->size.x && p.y >=pv->origin.y &&
	  p.y <=pv->origin.y+pv->size.y )){
		 PSpace3d ps3=pv->space3d;
		 PObject3d po;
		 for(i=0;i<pv->items->count;i++)
		  {//po=(PObject3d)ps3->primary->At(i);
		   _TObject3d*po=(_TObject3d*)pv->items->At(i);
		   PObject3d _po=(PObject3d)ps3->primary->At(i);
			double zz=0;
	  INT16 primitive;
	  if (__TestObject3d(pv,po,pt,primitive,zz)){
			if (zz>z || (obj==0)){obj=_po->_HANDLE;
				_pprim=primitive;
				z=zz;
			 }
			}
		  }}
	  if (obj && _primitive)*_primitive=_pprim;
	  return obj;
	   }// тест на попадание в Tview3d_2d
	return 0;
 }

_EXTERNC BOOL _EXPORT UpdateFromFrame3d(HSP2D hSpace2d)
 {PSpace2d ps=GetSpace(hSpace2d);
  if ((ps)&&(ps->Frame3d))
	{/* PFrame3d f=ps->Frame3d;
	int h=ps->Frame3d->view->space3d->_HANDLE | (hSpace2d<<8);
	int o=ps->Frame3d->object->_HANDLE;
	ps->Lock();
	 if (ps->Frame3d->flag & 1)
	  {RotateObject3d(h,o,&(ps->Frame3d->rotator));
	  }
	 if (ps->Frame3d->flag & 4)
	  {POINT3D p3=ps->Frame3d->size.GetPOINT3D();
	  SetObjectSize3d(h,o,&p3);
	  }
	 if (ps->Frame3d->flag & 2)
	  {POINT3D p3=ps->Frame3d->origin.GetPOINT3D();
		SetObjectOrg3d(h,o,&p3);
	  }
	ps->UpdateSpecObject((PObject2d)f);
	f->object->GetOrg_Size3d(f->origin,f->size);
	f->UpdateSize();
	f->flag=0;
	ps->UpdateSpecObject((PObject2d)f);
	ps->UnLock();
	*/
	}
  return 0;
 }
_EXTERNC INT16 _EXPORT GetNearestEdge( HSP2D hSpace2d,HOBJ2D Pr3dObject2d,HOBJ2D object3d,
 POINT2D point,POINT3D*out){
/*
 PView3d_2d pv=(PView3d_2d)_GetObject2d(hSpace2d,Pr3dObject2d,24);
	if (pv){
	pv->SetXYZab(0);
	int _obj=0;if(object3d<0)object3d=0;
	PObject3d po;
	int in=0;double z=0;
	do {
	if (object3d)po=(PObject3d)(pv->space3d->all->GetByHandle(object3d));
	 else {po=(PObject3d)pv->space3d->primary->At(_obj);_obj++;}
	if (po && po->WhoIsIt()==10){
	POINT2D pt=pv->FromLP2d(point);
	po->Perspective(pv->space3d->cut_z);
	PPoint3d p1,p2;

	for(int j=0;j<po->lines->count;j++)
		{PLine3d pl=(PLine3d)po->lines->At(j);
		  if (PointNearLine(pl->t1->GetPOINT2D(),pl->t2->GetPOINT2D(),pt)){
		  double zz=GetLineZ(pl,pt);
		  if (zz>z || in==0){
				z=zz;
				out[0]=pl->t1->GetPOINT3D();
				out[1]=pl->t2->GetPOINT3D();
				in=1;
		  } }
		}
	for(j=0;j<po->triangles->count;j++)
		{PTriangle r=(PTriangle)po->triangles->At(j);
		  if ((r->flag & tfvisible)&&
				(r->c!=0.0))
				{
			 if (PointNearLine(r->t1->GetPOINT(),r->t2->GetPOINT(),pt))
					{p1=r->t1;p2=r->t2;goto m1;}
			 if (PointNearLine(r->t2->GetPOINT(),r->t3->GetPOINT(),pt))
					{p1=r->t2;p2=r->t3;goto m1;}
			 if (PointNearLine(r->t3->GetPOINT(),r->t1->GetPOINT(),pt))
					{p1=r->t3;p2=r->t1;goto m1;}
				continue;
				m1:
				double zz=-(r->a*pt.x+r->b*pt.y+r->d)/r->c;
				if (zz>z || in==0){
				z=zz;
				out[0]=p1->GetPOINT3D();
				out[1]=p2->GetPOINT3D();
				in=2;
				}
				}
			 } // по треугольникам
	}}while(object3d==0 && _obj<pv->space3d->primary->count);
	 return in;
	}*/
	return 0;
 }
_EXTERNC HOBJ2D _EXPORT GetObjectItem3dbyPoint2d( HSP2D hSpace2d,HOBJ2D Pr3dObject2d,HOBJ2D object3d,
  POINT2D *point ,INT16 *primitive){	
PView3d_2d pv=(PView3d_2d)_GetObject2d(hSpace2d,Pr3dObject2d,24);
	if (pv){
	  PObject3d po=(PObject3d)(pv->space3d->all->GetByHandle(object3d));
	  if (po && po->WhoIsIt()==10){
		 C_TYPE index=pv->space3d->primary->IndexOf(po);
		  _TObject3d*_po=(_TObject3d*)pv->items->At(index);
		 double z=0.0;
		 POINT2D pt=pv->FromLP2d(*point,FALSE);
		 return __TestObject3d(pv,_po,pt,*primitive,z);
	}
  }
 return 0;
 };
_EXTERNC HOBJ2D _EXPORT GetObject3dFromPoint2d( HSP2D hSpace2d,HOBJ2D Pr3dObject2d, POINT2D* _pt ,HOBJ2D * primary,INT16*item)
 {PView3d_2d pv=(PView3d_2d)_GetObject2d(hSpace2d,Pr3dObject2d,24);
	 if (pv){
	 POINT2D pt =*_pt;
	 pv->space2d->CRDtoLP(&pt);
	  HOBJ2D obj=__Get_Obj_By_Point(pv,pt,item);
	  if (primary)*primary=obj;
		 PSpace3d ps3=pv->space3d;
	  if ((obj)&&(obj<OID_RESERVED))
		return ps3->GetGrandOwner((PObject)ps3->all->GetByHandle(obj));
		else return obj;
	  }
	 return 0;
 }
_EXTERNC HOBJ2D _EXPORT GetSubObject3dFromPoint2d( HSP2D hSpace2d,HOBJ2D Pr3dObject2d,
	 HOBJ2D Parent3d,POINT2D *_pt,HOBJ2D * primary )
 {	PView3d_2d pv=(PView3d_2d)_GetObject2d(hSpace2d,Pr3dObject2d,24);
	 if (pv){
	 POINT2D pt =*_pt;
	 pv->space2d->CRDtoLP(&pt);

	 HOBJ2D obj=__Get_Obj_By_Point(pv,pt);
	  if (primary)*primary=obj;
	 if ((obj)&&(obj<OID_RESERVED))
	  {PSpace3d ps3=pv->space3d;
		PObject3d po=(PObject3d)ps3->all->GetByHandle(obj);
		while ((po->owner)&&(po->owner->_HANDLE!=Parent3d)) po=(PObject3d)po->owner;
		  if (po->owner)return po->_HANDLE;
		  return -po->_HANDLE;
	  }else return obj;
	 }
	 return 0;
 }
_EXTERNC INT16 _EXPORT GetPoint3dFromPoint2d( HSP2D hSpace2d,HOBJ2D Pr3dObject2d,
			 HOBJ2D PrObject3d, POINT2D *_pt,BYTE dst )
 {PView3d_2d pv=(PView3d_2d)_GetObject2d(hSpace2d,Pr3dObject2d,24);
  if (pv)
  {
   PSpace3d ps=pv->space3d;
   PObject3d po=(PObject3d)ps->GetObjByHANDLE(PrObject3d);
     if ((po)&&(po->WhoIsIt()==10)){
      C_TYPE index=ps->primary->IndexOf(po);
      if(index>-1){
       _TObject3d*_po=(_TObject3d*)pv->items->At(index);
       C_TYPE j=-1;
       double _d=dst;
	 POINT2D pt__ =*_pt;
	 pv->space2d->CRDtoLP(&pt__);
    POINT2D pt=pv->FromLP2d(pt__,FALSE);

       for(C_TYPE i=0;i<_po->points->count;i++){
         _TPOINT3D* _tp=(_TPOINT3D*)(_po->points->At(i));
         double d=sqrt((_tp->x-pt.x)*(_tp->x-pt.x)+(_tp->y-pt.y)*(_tp->y-pt.y));
         if(d<_d){
           _d=d;j=i;
         }
        }
      return j;
      }}
   return -1;
  }
 };


_EXTERNC HOBJ2D _EXPORT CreateObject3d( HSP3D hSpace3d)
  {__GETSPACE3D
  if (ps&& (!ps->Overflow()))
	{HOBJ2D h=ps->CreateObject3d();
	 if(h){
	  if (ps2->record)
		{ps2->NewMetaRecord3d(mfcreateobject3d,ps->_HANDLE);
		 ps2->NewUndoRecord3d(mfdeleteobject3d,ps->_HANDLE);
		 ps2->meta->WriteWord(h);
		 ps2->CloseMetaRecord();
		}SetModify}
	 return h;
	}
	return 0;
  }
_EXTERNC HOBJ2D _EXPORT CreateCamera3d(HSP3D hSpace3d,CAMERA3D*Camera){
__GETSPACE3D
  if (ps&& (!ps->Overflow())){
	 return ps->CreateCamera3d(Camera);
	}
  return 0;
};
_EXTERNC BOOL  _EXPORT GetCamera3d(HSP3D hSpace3d,HOBJ2D hCamera,CAMERA3D*c){
 TCamera3d*po=(TCamera3d*)GetObj3d(hSpace3d,hCamera,11);
  if (po){
   *c=(po->camera);
   return TRUE;
  }
  return 0;
};

_EXTERNC BOOL  _EXPORT SetCamera3d(HSP3D hSpace3d,HOBJ2D hCam,CAMERA3D*Camera){
__GETSPACE3D
  if (ps){
	if(ps->SetCamera3d(hCam,Camera)){
     ps->_SetModify();
     return TRUE;
    }
	}
  return 0;
};
_EXTERNC HOBJ2D _EXPORT CreateLight3d(HSP3D hSpace3d,LIGHT3D*l){
__GETSPACE3D
  if (ps&& (!ps->Overflow())){
	 if(ps->CreateLight3d(l)){
      ps->_SetModify();
     return TRUE;
     }
	}
  return 0;
};
_EXTERNC BOOL   _EXPORT GetLight3d(HSP3D hSpace3d,HOBJ2D hLight,LIGHT3D*l){
 TLight3d*po=(TLight3d*)GetObj3d(hSpace3d,hLight,12);
  if (po){
   *l=(po->light);
  }
  return 0;
};
_EXTERNC BOOL   _EXPORT SetLight3d(HSP3D hSpace3d,HOBJ2D hLight,LIGHT3D*l){
__GETSPACE3D
  if (ps){
	return ps->SetLight3d(hLight,l);
	}
  return 0;
};

// ------------------------------------------------------------*
_EXTERNC HOBJ2D _EXPORT CreateBar3d(HSP3D hSpace3d,POINT3D *org,
	  POINT3D *size,COLORREF rgb)
 {__GETSPACE3D
	if (ps)
	{
	 SetModify;
	}

  return 0;
 }
_EXTERNC HOBJ2D _EXPORT CreateMaterial3d(HSP3D hSpace3d,MATERIAL3D*m){
 __GETSPACE3D
 if (ps) return ps->CreateMaterial3d(m);
  return 0;
};
_EXTERNC HOBJ2D _EXPORT GetMaterialByName3d(HSP3D hSpace3d,char*s){
 __GETSPACE3D
 if (ps && ps->materials){
  for (C_TYPE i=0;i<ps->materials->count;i++){
   TMaterial3d *m=(TMaterial3d *)ps->materials->At(i);
  if(m->_m.name && (!lstrcmpi(m->_m.name,s)))return m->_HANDLE;
  }
 }
  return 0;
};

_EXTERNC BOOL   _EXPORT SetMaterial3d(HSP3D hSpace3d,HOBJ2D hm,MATERIAL3D*m){
 __GETSPACE3D
 if (ps) return ps->SetMaterial3d(hm,m);
  return 0;
};
_EXTERNC BOOL   _EXPORT GetMaterial3d(HSP3D hSpace3d,HOBJ2D hm,MATERIAL3D*m){
 __GETSPACE3D
 if (ps) return ps->GetMaterial3d(hm,m);
  return 0;
};
/*
_EXTERNC BOOL   _EXPORT DeleteMaterial3d(HSP3D hSpace3d,HOBJ2D hm){
 __GETSPACE3D
 if (ps) return ps->DeleteMaterial3d(hm);
  return 0;
};
*/
//*------------------------------------------------------------*
_EXTERNC BOOL _EXPORT SetPoints3di( HSP3D hSpace3d,HOBJ2D Object3d,
						 POINT3D FAR* point,INT16 * nums,INT16 count){
  __GETSPACE3D
  if (ps) return ps->SetPoints3di(Object3d,point,nums,count);
  return FALSE;
  }

_EXTERNC BOOL  _EXPORT SetPoints3d( HSP3D hSpace3d,HOBJ2D Object3d, POINT3D FAR* points,
						 INT16 start,INT16 count ){
  __GETSPACE3D
 if (ps)return ps->SetPoints3d( Object3d,points,start,count );
   return FALSE;
  };
_EXTERNC INT16 _EXPORT AddPoints3d( HSP3D hSpace3d,HOBJ2D Object3d, POINT3D FAR* point,INT16 count){
 __GETSPACE3D
 if (ps)return  ps->AddPoints3d(Object3d, point,count);
   return FALSE;
}
_EXTERNC BOOL  _EXPORT DelPoints3di( HSP3D hSpace3d,HOBJ2D Object3d,INT16* PointsNum ,INT16 count){
 __GETSPACE3D
 if (ps)return ps->DelPoints3di(Object3d,PointsNum ,count);
   return FALSE;
}
_EXTERNC BOOL  _EXPORT DelPoints3d( HSP3D hSpace3d, HOBJ2D Object3d,INT16 from,INT16 count){
 __GETSPACE3D
 if (ps)return ps->DelPoints3d(Object3d,from,count);
   return FALSE;
};
_EXTERNC INT16 _EXPORT DelUnusedPoints3d( HSP3D hSpace3d,HOBJ2D Object3d )
{__GETSPACE3D
 INT16 h;
 if (ps){
  if (ps2->record)
  {ps2->NewMetaRecord3d(mfdeleteunusedpoint3d,ps->_HANDLE);
	ps2->meta->WriteWord(Object3d);
	ps2->NewUndoRecord3d(mfloadunusedpoints3d,ps->_HANDLE);
	ps2->meta->WriteWord(Object3d);
	  }
  h=ps->DelUnusedPoints3d(Object3d,ps2->meta);
  if(h)SetModify
  if (ps2->record) ps2->CloseMetaRecord();

  }
  return h;
};
_EXTERNC BOOL _EXPORT TransformPoint3d( HSP2D hSpace2d,HOBJ2D Obj2dIndex,POINT3D FAR* p,INT16 cPoints,BYTE f){
 PView3d_2d pv=(PView3d_2d)_GetObject2d(hSpace2d,Obj2dIndex,24);
 if (pv){
	 for (INT16 i=0;i<cPoints;i++){
     POINT3D out;
     if (f)pv->Perspective(p+i,&out,1);
     else pv->UnPerspective(p+i,&out);
     *(p+i)=out;
    }
  return TRUE;
 }
 return FALSE;
};

_EXTERNC BOOL _EXPORT LP3dtoLP2d( HSP2D hSpace2d,HOBJ2D Obj2dIndex ,POINT2D FAR* lp2d,POINT3D FAR* lp3d,INT16 cPoints,BYTE flags)
 {PView3d_2d pv=(PView3d_2d)_GetObject2d(hSpace2d,Obj2dIndex,24);
	  if (pv)
		{int i;
//		 POINT2D delta=pv->camera->camera.poOffset;
		 POINT2D delta;
		 delta.x=pv->origin.x;delta.y=pv->origin.y;
		 for (i=0;i<cPoints;i++)
		  {POINT3D p;
			pv->Perspective(lp3d+i,&p,1);
			lp2d[i].x=p.x;
			lp2d[i].y=p.y;
	   if(!(flags&1)){
		if(pv->space2d)pv->space2d->_DpToLpWO(lp2d+i,1);
		}
		if(!(flags&2)){
			lp2d[i].x+=delta.x;
			lp2d[i].y+=delta.y;
		}

		 pv->space2d->LPtoCRD(lp2d+i);

		  }
		return cPoints;
		}
	return FALSE;
 };

_EXTERNC BOOL _EXPORT LP3dtoLP3d( HSP3D hSpace3d,POINT3D FAR* src,POINT3D FAR* dst,INT16 cPoints,BYTE srcf,BYTE dstf,HOBJ2D hview){
 __GETSPACE3D
 if (ps){
 PView3d_2d pv=NULL;
  if(hview){pv=(PView3d_2d)ps->space2d->all->GetByHandle(hview);
   if((pv==NULL)||(pv->WhoIsIt()!=24))return FALSE;
  }
  BYTE stype=srcf&0x7fu;
  BYTE dtype=dstf&0x7fu;
  BOOL ssize=srcf&0x80u;
  BOOL dsize=dstf&0x80u;

  for(INT16 i=0;i<cPoints;i++){
   POINT3D p;
  if(!ssize){
   switch(stype){
   case 0:p=src[i];break;
   case 1:return FALSE;
   case 2:ps->CPtoWP(src+i,&p,1);break;
   }}else{
   switch(stype){
     case 0:p=src[i];break;
     case 1:return FALSE;
	 case 2:ps->CStoWS(src+i,&p,1);break;
   }
  }
  if(!dsize){
   switch(dtype){
   case 0:dst[i]=p;break;
   case 1:return FALSE;
   case 2:ps->WPtoCP(&p,dst+i,1);break;
   case 3:if(pv)pv->Perspective(&p,dst+i,1);else return FALSE;
   }}else{
   switch(dtype){
     case 0:dst[i]=p;break;
     case 1:return FALSE;
     case 2:ps->WStoCS(&p,dst+i,1);break;
   }
  }

  }
  return TRUE;
 }
 return FALSE;
};

_EXTERNC INT16  _EXPORT CreatePrimitives3d(HSP3D hSpace3d,HOBJ2D Object3d,PRIMITIVE3D*prim,INT16 num){
 __GETSPACE3D
 if (ps)return ps->CreatePrimitives3d(Object3d,prim,num);
 return 0;
};
_EXTERNC INT16  _EXPORT CreatePrimitives3di(HSP3D hSpace3d,HOBJ2D Object3d,PRIMITIVE3D*mask,
				 INT16 *points,INT16 numpoints){
};
_EXTERNC BOOL   _EXPORT GetPrimitives3d(HSP3D hSpace3d,HOBJ2D Object3d,PRIMITIVE3D*p,
				INT16 from,INT16 count,
				INT16*pnums,INT16 pnumsize,
				POINT2D*txtPoints,INT16 p2size){
PObject3d po=GetObj3d(hSpace3d,Object3d,10);
if (po && from>=0 && po->primitives->count>=(count+from)){
 INT16 pcount,p2count;
 pcount=p2count=0;
 for(INT16 i=0;i<count;i++){
  TPrimitive3d*pr=(TPrimitive3d*)po->primitives->At(from+i);
  p[i].count=pr->count;
  p[i].flags=pr->flags;
  p[i].color=pr->color;
  p[i].material=pr->material;
  p[i].points=NULL;
  p[i].texturePOINTS=NULL;
  if (pr->count && (pcount+pr->count)<pnumsize && pnums){
   p[i].points=pnums+pcount;
   for(INT16 j=0;j<pr->count;j++){
	pnums[pcount]=pr->points[j];pcount++;
   }
  }
  if (pr->texturePOINTS && (p2count+pr->count)<p2size && txtPoints){
   p[i].texturePOINTS=txtPoints+p2count;
   for(INT16 j=0;j<pr->count;j++){
	txtPoints[p2count]=pr->texturePOINTS[j];p2count++;
   }
  }
 }
 return TRUE;
}
 return FALSE;
};
_EXTERNC BOOL _EXPORT DelPrimitives3d( HSP3D hSpace3d,HOBJ2D Object3d,INT16* PrimNum,INT16 count){
 __GETSPACE3D
 if (ps) return ps->DelPrimitives3d(Object3d,PrimNum,count);
 return FALSE;
};
_EXTERNC BOOL _EXPORT SetPrimitives3di(HSP3D hSpace3d,HOBJ2D Object3d,PRIMITIVE3D*mask,
				INT16* items,INT16 count){
   __GETSPACE3D
   if(ps)return ps->SetPrimitives3di(Object3d,mask,items,count);
   return FALSE;
 };
_EXTERNC BOOL _EXPORT SetPrimitives3d(HSP3D hSpace3d,HOBJ2D Object3d,PRIMITIVE3D*mask,
				INT16 from,INT16 count){
   __GETSPACE3D
   if(ps)return ps->SetPrimitives3d(Object3d,mask,from,count);
   return FALSE;
 };


_EXTERNC BOOL _EXPORT LP2dtoView(HSP2D hSpace2d,HOBJ2D Obj2dIndex,POINT2D*point2din,POINT2D*point2dout,INT16 count)
 {PView3d_2d pv=(PView3d_2d)_GetObject2d(hSpace2d,Obj2dIndex,24);
	  if (pv)
	 {for(INT16 i=0;i<count;i++){
	   pv->space2d->CRDtoLP(point2din+i);
	   point2dout[i]=pv->FromLP2d(point2din[i],FALSE);
	 }
	   return TRUE;
	}	return FALSE;
  };
/* LP2dtoLP3d
 сначала преобразуем базовую в видовые координаты STEP 0.
 и далее для каждой точки:
  1. преобразуем двухмерную в нормальную 2d систему координат STEP 1.
  2. преобразуем двухмерную в систему координат проекции      STEP 2.
  3. новая 3d точка с x,y от 2d а Z от базовой                STEP 3.

*/

_EXTERNC BOOL _EXPORT LP2dtoLP3d(HSP2D hSpace2d,HOBJ2D Obj2dIndex ,LP2DLP3D*lp,POINT2D*point2d,POINT3D*point3d,INT16 count)
 {PView3d_2d pv=(PView3d_2d)_GetObject2d(hSpace2d,Obj2dIndex,24);
	  if (pv)
	 {
		POINT3D _2d,_3d,_4d;
		_3d=lp->_base;
		pv->Perspective(&_3d,&_4d,1); //STEP 0.
	for(INT16 i=0;i<count;i++){
		POINT2D pt=point2d[i];
		pv->space2d->CRDtoLP(&pt);  // STEP 1.
		pt=pv->FromLP2d(pt,FALSE);  // STEP 2.
		_2d.x=pt.x;
		_2d.y=pt.y;
		_2d.z=_4d.z;                // STEP 3.
		pv->UnPerspective(&_2d,point3d+i);
      // проверка
      POINT3D _5d;
      pv->Perspective(point3d+i,&_5d,1);
      if((_5d.x-_2d.x)>1 ||(_5d.y-_2d.y)>1 ||(_5d.z-_2d.z)>1){
       pv->UnPerspective(&_2d,point3d+i);
      }
	 }

	   return TRUE;
	 }
	return FALSE;
 };



_EXTERNC BOOL _EXPORT SetCrdSystem3d(HSP3D hSpace3d,CRDSYSTEM*crd){
__GETSPACE3D
  if (ps){
   return ps->SetCrd(crd);
  }
 return FALSE;
};
_EXTERNC BOOL _EXPORT GetCrdSystem3d(HSP3D hSpace3d,CRDSYSTEM*crd){
__GETSPACE3D
  if (ps && crd){
   crd->type=ps->localType;
   crd->hObject=ps->localCS;
   memcpy(&(crd->matrix),&(ps->matrix.a.a),sizeof(TMatrix));
   return TRUE;
  }return FALSE;
};

_EXTERNC BOOL _EXPORT SetMatrix3d( HSP3D hSpace3d,HOBJ2D Object3d,void*buf){
__GETSPACE3D
 if(ps && buf){
  return ps->SetMatrix(Object3d,buf);
  return TRUE;
 }
 return FALSE;
};

_EXTERNC BOOL _EXPORT GetMatrix3d(HSP3D hSpace3d,HOBJ2D Object3d,void*buf){
  PObject3d po=GetObj3d(hSpace3d,Object3d );
  if(po){
   TMatrix2 __m;
   TMatrix2*m=po->GetMatrix(&__m);
   if(m){
    memcpy(buf,(m->a.a),sizeof(TMatrix));
    return TRUE;
   }
   return FALSE;
  }
 return FALSE;
}


_EXTERNC BOOL  _EXPORT MakeDefaultCamera(CAMERA3D* p,int mode){
 if (p){
 CAMERA3D &camera =*p;
 mode&=0x3f;
 if (!mode)mode=0x15;
 memset(&camera,0,sizeof(camera));
 camera.poSize=sizeof(camera);
 camera.poOrg.z=0;
 camera.poOrg.x=0;
 camera.poOrg.y=0;

 if (mode&1)camera.poOrg.x=1;
 if (mode&2)camera.poOrg.x=-1;
 if (mode&4)camera.poOrg.y=1;
 if (mode&8)camera.poOrg.y=-1;
 if (mode&16)camera.poOrg.z=1;
 if (mode&32)camera.poOrg.z=-1;

 camera.poOrg.z*=1000;
 camera.poOrg.x*=1000;
 camera.poOrg.y*=1000;

 camera.poOrg.z*=1000;
 camera.poOrg.x*=1000;
 camera.poOrg.y*=1000;

 camera.poUp.z=100;
 camera.poUp.x=0;
 camera.poUp.y=0;

 camera.poFocus=1;
 camera.poBackground=RGB(0,0,0);
 camera.poOffset.x=0.5;
 camera.poOffset.y=0.5;
 camera.poExtent.x=camera.poExtent.y=100;
 camera.poExtent.z=1.0;
 lstrcpy(camera.poRenderName,"Auto");
 //CAMERA_PERSPECTIVE|
 camera.poFlags=CAMERA_TARGET;
 camera.poRenderType=RENDER_FACETED|RENDER_ZSORT|RENDER_SHADED|RENDER_TEXTURED;

  return TRUE;
 }
 return FALSE;
};
_EXTERNC void _EXPORT __MUL_MAT4(void*src1,void*src2,void*dst){
 ((TMatrix*)dst)->Mul(*((TMatrix*)src1),*((TMatrix*)src2));
}
_EXTERNC void _EXPORT __INV_MAT4(void*src,void*dst){
  TMatrix*m1=(TMatrix*)dst;
  m1->Invert(*((TMatrix*)src));
}
