//#include "space3d.h"
#include <gwin2d3d\gwintype.h>
#include <gwin2d3d\3drender.h>
#include <math.h>
#include <stdlib.h>
#include "_g_def.h"

typedef struct tagsi_item{
_TPRIMITIVE3D * _primitive;
 PRIMITIVE3D  *  primitive;
_TPOINT3D     **  points;
}si_item;

int _USERENTRY sicmp(const void*i1,const void*i2){
 double s1=((si_item*)i1)->_primitive->minz;
 double s2=((si_item*)i2)->_primitive->minz;
 if (s1<s2)return -1;
 if (s1>s2)return 1;
 return 0;
}
COLORREF GetGDIColor(COLORREF _color){
 if(_color&0x02000000L){
  _color=GetSysColor((int)((0xfffful)&_color));
 }
 return _color;
 };

void FillArea(PAINTSTRUCT3D*ps){
 int sizex=ps->srcPaint.right-ps->srcPaint.left;
 int sizey=ps->srcPaint.bottom-ps->srcPaint.top;
 HGDIOBJ b=CreateSolidBrush(GetGDIColor(ps->camera->poBackground));
 HGDIOBJ old=SelectObject(ps->hcolorDc,b);
 PatBlt(ps->hcolorDc,ps->srcPaint.left,ps->srcPaint.top,sizex,sizey,PATCOPY);
  SelectObject(ps->hcolorDc,old);
 DeleteObject(b);
}

// Встроенный драйвер отрисовки

BOOL _EXPORT DrawSpace3d_Points(PAINTSTRUCT3D*ps);
BOOL _EXPORT DrawSpace3d(PAINTSTRUCT3D*ps){
 BOOL wireframe=FALSE;
 int mode=(ps->camera->poRenderType &RENDER_TYPEMASK);
 switch(mode){
   case RENDER_DISABLED:return TRUE;
   case RENDER_WIREFRAME:wireframe=TRUE;break;
   case RENDER_BOUNDED:return 0;
   case RENDER_POINTS:return DrawSpace3d_Points(ps);
 };
 FillArea(ps);

 POINT pp[MAX_IN_PRIMITIVE];
 TIntObject3d**objects3d=ps->objects3d;

 long pcount=0;
/*
 for(i=0;i<ps->objcount;i++){
 _OBJECT3D * obj=ps->_objects[i];
   pcount+=obj->primitives->count;
 }
*/

 for(INT16 i=0;i<ps->objcount;i++){
  _OBJECT3D * obj=ps->_objects[i];
  if (!(obj->flags&O3D_NOTINRECT)){
	  _TPRIMITIVE3D ** _primitives=obj->primitives->items;
	for(INT16 j=0;j<obj->primitives->count;j++){
	 _TPRIMITIVE3D*_primitive=_primitives[j];
	  if (_primitive->flags.visible) {pcount++;}
	 }}
	};

 if (pcount){
#if !defined(WIN32)
 if( pcount*sizeof(si_item)>65500ul){
  return 0;
 }
#endif
 si_item * items=new si_item[pcount];
 int k=0;

 for(INT16 i=0;i<ps->objcount;i++){
  _OBJECT3D * obj=ps->_objects[i];
  if (!(obj->flags&O3D_NOTINRECT)){
  TIntObject3d*object3d=objects3d[i];

  _TPOINT3D**_points=(_TPOINT3D**)obj->points->items;
  _TPRIMITIVE3D ** _primitives=obj->primitives->items;
   PRIMITIVE3D ** primitives3d=object3d->primitives->items;

   for(INT16 j=0;j<obj->primitives->count;j++){
	 _TPRIMITIVE3D*_primitive=_primitives[j];
	  PRIMITIVE3D *primitive3d=primitives3d[j];
  if (_primitive->flags.visible)
  {
	 items[k]._primitive=_primitive;
	 items[k].primitive=primitive3d;
	 items[k].points=_points;
	 k++;
	} } }};
 if (k==pcount){
 if (k>1 && (ps->camera->poRenderType &RENDER_ZSORT))qsort(items,pcount,sizeof(si_item),sicmp);
 }
 for(i=0;i<pcount;i++){
	PRIMITIVE3D *primitive3d =items[i].primitive;
	_TPRIMITIVE3D*_primitive= items[i]._primitive;
	_TPOINT3D **_points=items[i].points;
	for(INT16 k=0;k<primitive3d->count;k++){
	 _TPOINT3D*point=_points[primitive3d->points[k]];
	 pp[k].x=point->_x/256;
	 pp[k].y=point->_y/256;
	 }
   COLORREF color=0xfffffful&_primitive->color.rgb;
   if ((!_primitive->flags.wireframe)&&(!wireframe))
   {
   HGDIOBJ c=CreateSolidBrush(color);
   HGDIOBJ old=SelectObject(ps->hcolorDc,c);
   HGDIOBJ oldpen=SelectObject(ps->hcolorDc,GetStockObject(NULL_PEN));
   Polygon(ps->hcolorDc,pp,primitive3d->count);
   SelectObject(ps->hcolorDc,old);
   SelectObject(ps->hcolorDc,oldpen);
   DeleteObject(c);
   }
	else {
	int count=primitive3d->count;
   if (primitive3d->flags&PRIMITIVE_POLYGON){
	  pp[count]=pp[0];
	  count++;
   }
		HGDIOBJ hpen = CreatePen(PS_SOLID,1,color);
		HGDIOBJ hpenOld = SelectObject(ps->hcolorDc, hpen);
		Polyline(ps->hcolorDc,pp,count);
		SelectObject(ps->hcolorDc, hpenOld);
		DeleteObject(hpen);

	 }
   }
  delete items;
 }
 return TRUE;
};

BOOL WINAPI DrawSpace3d_Points(PAINTSTRUCT3D*ps){
 FillArea(ps);

 for(INT16 i=0;i<ps->objcount;i++){
  _OBJECT3D * obj=ps->_objects[i];
  if (!(obj->flags&O3D_NOTINRECT)){
  _TPOINT3D**_points=(_TPOINT3D**)obj->points->items;
	int count=obj->points->count;
	for(int k=0;k<count;k++){
	 _TPOINT3D*_point=_points[k];
	 SetPixel(ps->hcolorDc,_point->_x/256,_point->_y/256,_point->color.rgb);
	}
   }
 };
return TRUE;
};


INT32 _EXPORT Default3dMessage(INT16 msg,INT32 lp1,INT32 lp2){
 switch(msg){
  case RM3D_PAINT      :{
	   DrawSpace3d((PAINTSTRUCT3D*)lp2);
	   return TRUE;
	}break;
  case RM3D_GETCAPS    :{
	  CAMERA3D*camera=(CAMERA3D*)lp2;
	 int type=camera->poRenderType&RENDER_TYPEMASK;
	  switch(lp1){
	case  caps3d_facecolor  :
							 if (type==RENDER_WIREFRAME)return 1;
							 if (type==RENDER_FACETED)return 1;
							 return 0;
	case  caps3d_vertexcolor:
							 if (type==RENDER_POINTS)return 1;
							 return 0;
	case  caps3d_faceabcd   :return 0;
	case  caps3d_vertexc    :return 0;
	case  caps3d_version    :return 0x100;
	case  caps3d_drawonDIB  :return 0;
	case  caps3d_drawonDC   :return TRUE;
   case  caps3d_points     :
   case  caps3d_faces      :return(type!=RENDER_BOUNDED);
   case  caps3d_bounds     :return(type==RENDER_BOUNDED);


	  }
	}break;
  case RM3D_DRIVERSETUP:{
   HWND parent=(HWND)lp1;
   MessageBox(parent,"Стандартный алгоритм отрисовки.\n(Может осуществяться только предварительная соритровка по Z, никаких проверок на коллизии не производится).\n\nCopyright 1996-1997 Alexander Shelemekhov\n\nfor Statum Computer",
   "Std driver info",MB_OK|MB_ICONINFORMATION);

  }return 0;

  case RM3D_GETDRAWPROC:{
	 CAMERA3D*camera=(CAMERA3D*)lp2;
	 int type=camera->poRenderType&RENDER_TYPEMASK;
	 if (type==RENDER_POINTS)return (INT32)DrawSpace3d_Points;
    if (type==RENDER_BOUNDED)return NULL;
	 return (INT32)DrawSpace3d;
	}break;
  }
 return 0;
};


