#include "object2d.h"
#include "space2d.h"

#include "space3d.h"
#include "camera3d.h"
#include <math.h>
#include "loadlib.h"
#include "_g_def.h"
#include "_system.h"

INT32 WINAPI Default3dMessage(INT16,INT32,INT32);

 TView3d_2d::TView3d_2d(POINT2D dor,POINT2D dsz,TSpace3d * ps,TCamera3d*_camera):TObject2d(dor)
  {BMPcolor=0;BMPmono=0;
	 space3d=ps;
	 space3d->views->Insert(this);
	 camera=_camera;
	 camera->views->Insert(this);
	 size=dsz;
	 imagesize.x=-1;
	 imagesize.y=-1;
	 items=new T3dObjects(20,20);
	 space2d=space3d->space2d;
	 UpdateCamera(FALSE,FALSE);
	 CreateObjects();
	 UpdateAllObjects();
    matrix.Set();
    UpdateCamera(TRUE,TRUE);
  };

 TView3d_2d::TView3d_2d(PStream ps,LONG _pos):TObject2d(ps,_pos)
  { BMPcolor=0;BMPmono=0;
	items=new T3dObjects(20,20);
	space3d=(PSpace3d)(void*)MAKELONG(ps->ReadWord(),0);
	camera=(TCamera3d*)MAKELONG(ps->ReadWord(),0);
	imagesize.x=-1;
	imagesize.y=-1;
  };

 void TView3d_2d::UpdateCamera(BOOL UpdObjects,BOOL UpdView){
   double sing,cosg,cosa,sina,cosb,sinb;
   double r=0;
   CAMERA3D &_cam=camera->camera;
   POINT3D viewp;
   if (_cam.poFlags&CAMERA_TARGET)
   {
	o_x=_cam.poDirection.x;
	o_y=_cam.poDirection.y;
	o_z=_cam.poDirection.z;
   }else{
	o_x=_cam.poOrg.x+_cam.poDirection.x;
	o_y=_cam.poOrg.y+_cam.poDirection.y;
	o_z=_cam.poOrg.z+_cam.poDirection.z;
   }
   viewp.x=_cam.poOrg.x-o_x;
   viewp.y=_cam.poOrg.y-o_y;
   viewp.z=_cam.poOrg.z-o_z;

// o_x=_cam.poOrg.x; // == ???
// o_y=_cam.poOrg.y;
// o_z=_cam.poOrg.z;

	r=sqrt( viewp.x*viewp.x+
		     viewp.y*viewp.y+
			  viewp.z*viewp.z );

   double alpha,beta;
   if (r==0.0){alpha=beta=0; }
	else{


//	 beta=asin(_cam.poDirection.y/r);
//	 alpha=GetAngleByXY(_cam.poDirection.z,_cam.poDirection.x);
    beta=asin(viewp.y/r);
    alpha=-GetAngleByXY(viewp.z,viewp.x);
	}

 double angle=0;

 POINT2D _size=size;
 POINT3D _scale;
 if (space2d){
   space2d->_LpToDpWO(&_size,1);
 }

 _scale.x=_size.x/_cam.poExtent.x;
 _scale.y=_size.y/_cam.poExtent.y;
 _scale.z=_cam.poExtent.z;

  if (_scale.x<_scale.y)
  {
   _scale.y=_scale.x;
  }else{
  _scale.x=_scale.y;
  }
 __scale=_scale.y;
  _scale.z*=_scale.y;
 _delta=camera->camera.poOffset;
 space2d->_LpToDpWO(&_delta,1);
BOOL calcangle=TRUE;
// Вычисляем коэффицменты
m1:
 offset.x=_size.x*camera->camera.poOffset.x;
 offset.y=_size.y*camera->camera.poOffset.y;

 perspective=(_cam.poFlags&CAMERA_PERSPECTIVE)!=0;
 focus=0;

 v43=-r;
 if (perspective){
  if (_cam.poFocus<=0)focus=1.0;else focus=_cam.poFocus;
  focus*=(r*_scale.z);
 }
 sing=sin(angle);cosg=cos(angle);
 cosa=cos(alpha);sina=sin(alpha);
 cosb=cos(beta) ;sinb=sin(beta);

 v11=cosa*cosg-sina*sinb*sing;
 v21=-cosb*sing;
 v31=sina*cosg+cosa*sinb*sing;

 v12=(cosa*sing+sina*sinb*cosg);
 v22=(cosb*cosg);
 v32=(sina*sing-cosa*cosg*sinb);

 v13=-sina*cosb;
 v23=sinb;
 v33=cosa*cosb;

// v43=0;

 v11*=_scale.x;
 v21*=_scale.x;
 v31*=_scale.x;

 v12*=-_scale.y;
 v22*=-_scale.y;
 v32*=-_scale.y;

 v13*=_scale.z;
 v23*=_scale.z;
 v33*=_scale.z;

if(calcangle){
 calcangle=FALSE;
 POINT3D up=_cam.poUp;
// up.x=up.y=0;
// up.z=100.0;
 v12*=-1.0;
 v22*=-1.0;
 v32*=-1.0;
 offset.x=offset.y=0;
 perspective=0;
 POINT3D _up;
 Perspective(&up,&_up);
 angle=-GetAngleByXY(_up.x,_up.y)+M_PI_2;
// angle=0;
 goto m1;
};


// Переворачиваем ось Y.


  inversecalc=0;
  UpdateRender();
  matrix.Set();
  matrix.Put(1,1,v11);
  matrix.Put(1,2,v12);
  matrix.Put(1,3,v13);

  matrix.Put(2,1,v21);
  matrix.Put(2,2,v22);
  matrix.Put(2,3,v23);

  matrix.Put(3,1,v31);
  matrix.Put(3,2,v32);
  matrix.Put(3,3,v33);

  matrix.Put(4,3,v43);
  matrix.Put(4,4,1.0);
	gr_float  poFar_Clip;   // - ближния отсекающая плоскость
	gr_float  poNear_Clip;  // - дяльняя отсекающая плоскость

  if(_cam.poFar_Clip > _cam.poNear_Clip){
   zclipvalid=1;
  }else zclipvalid=0;
  bounded=(_cam.poRenderType&RENDER_TYPEMASK)==RENDER_BOUNDED;
  if(UpdObjects) UpdateAllObjects();
  if(UpdView) UpdateRect(NULL);

};

 void TView3d_2d::UpdateRender(){
 MessageProc=0;
 if(camera->camera.poRenderName[0])
  {
  if (lstrcmpi(camera->camera.poRenderName,"Default")){
   (void*)MessageProc =GetRenderByName(camera->camera.poRenderName);
	}
  }
 if (!MessageProc)MessageProc=Default3dMessage;
 INT32 I_cam=(INT32)(& (camera->camera));
 (void*)Draw3dProc=(void*)MessageProc(RM3D_GETDRAWPROC,0,I_cam);
 calcfaces=MessageProc(RM3D_GETCAPS,caps3d_faces,    I_cam );
 calcpoints=MessageProc(RM3D_GETCAPS,caps3d_points,    I_cam );
 calcbounds=MessageProc(RM3D_GETCAPS,caps3d_bounds,    I_cam );

 calcfacecolor=MessageProc(RM3D_GETCAPS,caps3d_facecolor,    I_cam );
 calcpointcolor=MessageProc(RM3D_GETCAPS,caps3d_vertexcolor, I_cam );
 calcpointC=MessageProc(RM3D_GETCAPS,caps3d_vertexc,         I_cam );
 calcfaceABCD=MessageProc(RM3D_GETCAPS,caps3d_faceabcd,      I_cam );
 minimizerect=MessageProc(RM3D_GETCAPS,caps3d_minrect,  I_cam );
 wireframe = (camera->camera.poRenderType&RENDER_TYPEMASK)==RENDER_WIREFRAME;
};

 void TView3d_2d::Store(PStream ps)
  { TObject2d::Store(ps);
     C_TYPE index=0;
  if(space2d->spaces3d->fullstore){
   index=space2d->spaces3d->IndexOf(space3d);
  }else{
   for(C_TYPE i=0;i<space2d->spaces3d->count;i++)
    {
      PSpace3d sp=(PSpace3d)space2d->spaces3d->At(i);
       if(sp==space3d)break;
       if(space2d->spaces3d->IsNeedToSave(sp))index++;
    }
  }
  ps->WriteWord(index);

	 ps->WriteWord(camera->_HANDLE);
  };

 void TView3d_2d::NewCamera(TCamera3d*cam)
  { camera=cam;
	imagesize.x=-1;
	imagesize.y=-1;
	UpdateCamera(TRUE,TRUE);
  };
 TView3d_2d::~TView3d_2d()
  {
	if (HIWORD(space3d) && space3d->views)space3d->views->Delete(this);
	if (BMPcolor) DeleteObject(BMPcolor);
	if (BMPmono) DeleteObject(BMPmono);
	camera->views->Delete(this);
	BMPcolor=0;BMPmono=0;
	DeleteCollection(items);
  };
 RECT TView3d_2d::GetRect(int)
  {return TObject2d::GetRect(0);
  };
 void TView3d_2d::MarkToSave(int m)
  {TObject::MarkToSave(m);
	space3d->MarkToSave(m);
  };
 void TView3d_2d::SetState(WORD astate,ATTR_MODE op,pointer p){
  WORD oldstate=options;
  TObject2d::SetState(astate,op,p);
  if ((options&2) !=(oldstate&2))((PSpace2d)p)->AddObjectRect(this);
 };
 void TView3d_2d::Rotate2d(POINT2D p,double a)
  {
	TObject2d::Rotate2d(p,a);
  };
 void TView3d_2d::UpdateSize(int,void *)
 {
   UpdateCamera(TRUE);
 };
 void TView3d_2d::CreateObjects(){
  for(C_TYPE i=0;i<space3d->primary->count;i++)
   items->Insert(new _TObject3d((TObject3d*)space3d->primary->At(i)));
 };
 BOOL TView3d_2d::Awaken(_SPACE* p)
  {
	 space2d=(PSpace2d)p;
	 space3d=(PSpace3d)space2d->spaces3d->At(LOWORD(space3d));
	 if (space3d==NULL) return 0;
	 camera=(TCamera3d*)space3d->all->GetByHandle(LOWORD(camera));
	 if (camera==NULL) return 0;
	 camera->views->Insert(this);
	 space3d->views->Insert(this);
	 UpdateCamera();
	 CreateObjects();
	 UpdateAllObjects();
	 return 1;
  };
 POINT2D TView3d_2d::SetSize2d(POINT2D s)
  {POINT2D os=TObject2d::SetSize2d(s);
	 NewBuffers();
	 UpdateCamera(TRUE);
	 UpdateAllObjects();
	 return os;
  };

 void TView3d_2d::NewBuffers()
  {updaterect.left=0;updaterect.top=0;
  	POINT2D sz=size;space2d->_LpToDpWO(&sz,1);
	updaterect.right=sz.x;updaterect.bottom=sz.y;
	ischanged=1;
	if (BMPcolor) {DeleteObject(BMPcolor);BMPcolor=NULL;}
	if (BMPmono) {DeleteObject(BMPmono);BMPmono=NULL;}
   if (camera->camera.poRenderType&RENDER_NOBUFFER)return;

	POINT is;
	is=imagesize;
	HDC dc=GetDC(0);
	BMPcolor=CreateCompatibleBitmap(dc,is.x,is.y);
   HDC hDc=GetDC(0);
   HDC memdc=CreateCompatibleDC(hDc);
	SelectObject(memdc,BMPcolor);
   if (space2d->palette) SelectPalette(memdc,space2d->palette,0);

  HGDIOBJ b=CreateSolidBrush(camera->camera.poBackground);
  HGDIOBJ old=SelectObject(memdc,b);
   PatBlt(memdc,0,0,is.x,is.y,PATCOPY);
    SelectObject(memdc,old);
   DeleteObject(b);

   DeleteDC(memdc);
   ReleaseDC(0,hDc);
// !!!!!!!!!!!!!
	if (camera->camera.poRenderType&RENDER_TRANSPARENT){
	  BMPmono=CreateBitmap(is.x,is.y,1,1,NULL);
	}
  };
 void TView3d_2d::UpdateAllObjects(){
  for(C_TYPE i=0;i<items->count;i++)UpdateObject(i,UIV_ALL);
 };

 void TView3d_2d::UpdateObject(C_TYPE index,BYTE f){
   _TObject3d* o=(_TObject3d*)items->At(index);
   o->boundsinvalid=1;
   o->objflags=o->object->options;
   if(camera->camera.poRenderType&RENDER_ALLFACES)o->objflags|=STATE_ALLFACES;
   if(wireframe)o->objflags|=STATE_WIREFRAME; // говорим что проволочная
   if (calcpointcolor)o->flags|=O3D_CALCALLFACES;else
		o->flags&=~O3D_CALCALLFACES;

   if(f&UIV_MATRIX){
	o->UpdateMatrix(this);
	 if((space3d->localCS==o->object->_HANDLE) &&
	   (space3d->localType==LOCAL_COORDINATE)){
		space3d->UpdateCrd();
	 }
   }

   if((f&UIV_POINTS) && calcpoints){
	 o->PerspectiveAll();
   }
   if((f&UIV_POINTS) && calcbounds)o->UpdateBounds();

   if((f&UIV_PRIMITIVE)&& calcfaces)o->UpdateAllPrimitives();

   if (calcpointcolor)o->UpdatePoints(calcpointcolor);
   UpdateOrgSizeView(o);
 };

 void TView3d_2d::UpdateOrgSizeView(_TObject3d*o){
  if(calcpoints){
   TCollection *collection=o->points;
   C_TYPE pointcount=collection->count;

	 if (pointcount){
   POINT3Di m,org;

  // Perspective(o,0);
   _TPOINT3D* po=(_TPOINT3D*)collection->_At(0);
   org.x=po->_x;org.y=po->_y;org.z=po->_z;
   m=org;
   // первая

   for(C_TYPE i=1;i<pointcount;i++){
   //	Perspective(o,i);

	_TPOINT3D* po=(_TPOINT3D*)collection->_At(i);

	 if (po->_x<org.x)org.x=po->_x;else { if (po->_x>m.x)m.x=po->_x; }
	 if (po->_y<org.y)org.y=po->_y;else { if (po->_y>m.y)m.y=po->_y; }
	 if (po->_z<org.z)org.z=po->_z;else { if (po->_z>m.z)m.z=po->_z; }

   }

 	o->org=org;
	o->org.x/=256L;o->org.y/=256L;
	o->size.z=(m.z-org.z);

	o->size.x=m.x/256;
	o->size.y=m.y/256;
   o->size.x-=o->org.x;
   o->size.y-=o->org.y;
   if(o->objflags&STATE_POINTS){
     o->size.x+=6; o->size.y+=6;
     o->org.x-=3;  o->org.y-=3;
   }


   }else{
	o->org.y=o->org.z=o->org.x=o->size.x=o->size.y=o->size.z=0;
   }
  }else{
   POINT3D _o,s;
   o->GetBoundsRect(&_o,&s);
   o->org.x=_o.x;
   o->org.y=_o.y;
   o->org.z=_o.z;

   s.x-=_o.x;
   s.y-=_o.y;
   s.z-=_o.z;
   o->size.x=s.x;
   o->size.y=s.y;
   o->size.z=s.z;
  }
  o->flags&=~(O3D_ZFARCONFLICT|O3D_ZNEARCONFLICT|O3D_ZNOTINRECT);
  if(zclipvalid){
   double nc=-camera->camera.poNear_Clip*256;
   double fc=-camera->camera.poFar_Clip*256;
	if(o->org.z>=nc)o->flags|=O3D_ZNOTINRECT;else{
	if((o->org.z+o->size.z)<=fc)o->flags|=O3D_ZNOTINRECT;
	 else{
	  if(o->org.z<fc)o->flags|=O3D_ZFARCONFLICT;
	  if((o->org.z+o->size.z)>nc)o->flags|=O3D_ZNEARCONFLICT;
	}}
  } // проверка на попадание в Z
 };

 void TView3d_2d::UpdateRect(RECT * r,BYTE flags)
  {if (r)
	 {
	 if ( (r->left>=imagesize.x) || (r->right<0) ||
		  (r->top>=imagesize.y) ||(r->bottom<0))return;
	 if (r->left<0)r->left=0;
	 if (r->right>imagesize.x)r->right=imagesize.x;
	 if (r->bottom>imagesize.y)r->bottom=imagesize.y;
	 if (r->top<0)r->top=0;

   if (flags & UF_BUFFER){
	 if (ischanged)Rect_or_Rect(updaterect,*r);
				 else  {updaterect= (*r);ischanged=TRUE;}

      }
   if ((flags & UF_SCREEN) && space2d->IsVisible(this)){
	 POINT2D delta;
	 delta.x=origin.x;
	 delta.y=origin.y;


	 RECT u=(*r);
    space2d->_DpToLpWO((POINT*)&u,2);

	 u.left+=delta.x;u.right+=delta.x;
	 u.top+=delta.y;u.bottom+=delta.y;

    u.right++;
    u.bottom++;

	 space2d->AddRect(u);

	  if (options&2){ // Обновляем системную область
		u.left=origin.x;
		u.right=u.left+size.x;
		u.bottom=u.top=origin.y;
		POINT p;p.x=10;p.y=16;
		space2d->_DpToLpWO(&p,1);
		u.bottom+=p.y;
		space2d->AddRect(u);
	   };
	  }
	 }else
	  {
    if(flags&UF_BUFFER){
     updaterect.left=0;updaterect.top=0;
	  updaterect.right=size.x;updaterect.bottom=size.y;
     ischanged=1;
    }
	 if(flags&UF_SCREEN)space2d->AddObjectRect(this);
	  }
  //	space2d->AddRect(GetRect());

  };
double sqr(double x){return x*x;}
void TView3d_2d::GetAxisPoints(_TObject3d*o,POINT3D* p){
 POINT3D data[4]={{0,0,0},{1.0,0,0},{0,1.0,0},{0,0,1.0}};
 if(o){
 /*
  BYTE _p=o->perspective;
  o->perspective=0;
  for(int i=0;i<4;i++)o->_Perspective(data+i,p+i);
  o->perspective=_p;
  double l[3];
  for(i=1;i<4;i++)l[i-1]=sqrt(sqr(p[i].x-p[0].x)+
                              sqr(p[i].y-p[0].y)+
                              sqr(p[i].z-p[0].z));
  double _l=l[0];
  for(i=1;i<3;i++){ if(_l<l[i])_l=l[i]; }
  if(_l==0.0)_l=1;
  for(i=0;i<3;i++){ l[i]=_l/l[i]; }

  data[1].x=l[0];
  data[2].y=l[1];
  data[3].z=l[2];*/
  for(int i=0;i<4;i++)o->_Perspective(data+i,p+i);

 }else{
  Perspective(data,p,4);
 }
/*
  double l=0;
  for(int i=1;i<1;i++){double _l=sqrt(sqr(p[i].x-p[0].x)+
                              sqr(p[i].y-p[0].y));
                    if(_l>l)l=_l;
   }
 double k=l>0?40*l/40:1.0;*/
 for(int i=1;i<4;i++){
  double l=sqrt(sqr(p[i].x-p[0].x)+sqr(p[i].y-p[0].y));
  if(l>0.000001){
  p[i].x=p[0].x+40*(p[i].x-p[0].x)/l;
  p[i].y=p[0].y+40*(p[i].y-p[0].y)/l;
  }else{
   p[i]=p[0];
  }
 }

};

void TView3d_2d::GetAxisRect(_TObject3d*o,RECT*_r){
 POINT3D p[4];
 int delta=12;
 GetAxisPoints(o,p);
 RECT r;

 r.left=r.right=p[0].x;
 r.top=r.bottom=p[0].y;
 r.right+=delta;
 r.bottom+=delta;
 for(BYTE i=1;i<4;i++){
  if(r.left>p[i].x)r.left=p[i].x;
  if(r.right<(p[i].x+delta))r.right=p[i].x+delta;
  if(r.top>p[i].y)r.top=p[i].y;
  if(r.bottom<(p[i].y+delta))r.bottom=p[i].y+delta;
 }

 r.left--;
 r.top--;
 r.right++;
 r.bottom++;
*_r=r;
};


void TView3d_2d::DrawAxis3d(HDC hdc,_TObject3d*o,BYTE f){
 HDC bdc=CreateCompatibleDC(hdc);
 SelectObject(bdc,GetAxisBMP());
 POINT3D p[4];
 GetAxisPoints(o,p);
 POINT pp[2];
 pp[0].x=p[0].x;
 pp[0].y=p[0].y;
 for(BYTE i=1;i<4;i++)
   {
     pp[1].x=p[i].x;
     pp[1].y=p[i].y;
     Polyline(hdc,pp,2);
     BitBlt(hdc,pp[1].x+1,pp[1].y+1,8,11,bdc,16*i,0,SRCCOPY);
   }
 DeleteDC(bdc);
};

void TView3d_2d::DrawBoundedRect(HDC hdc,_TObject3d*O,COLORREF color,BOOL b){
 HGDIOBJ pen=CreatePen(PS_SOLID,0,color);
 HGDIOBJ old=SelectObject(hdc,pen);
  O->UpdateBounds();

typedef struct{
 BYTE a,b;
}BNDPNT;

BNDPNT nums[12]= {
 {0,1},
 {0,2},
 {2,3},
 {1,3},

 {4,5},
 {4,6},
 {6,7},
 {5,7},

 {0,4},
 {1,5},
 {3,7},
 {2,6}
};
POINT3D*_pp=O->bounds;
for(int i=0;i<12;i++){
 POINT p2[2];
if(b){
 double x1,y1,x2,y2;
 x1=_pp[nums[i].a].x;
 y1=_pp[nums[i].a].y;

 x2=_pp[nums[i].b].x;
 y2=_pp[nums[i].b].y;
 POINT p2[2];
 double dx=x2-x1,dy=y2-y1;
 const double k=0.2;
 p2[0].x=x1;
 p2[0].y=y1;

 p2[1].x=x1+dx*k;
 p2[1].y=y1+dy*k;
Polyline(hdc,p2,2);
 p2[0].x=x2;
 p2[0].y=y2;

 p2[1].x=x2-dx*k;
 p2[1].y=y2-dy*k;
Polyline(hdc,p2,2);

}else{
 p2[0].x=_pp[nums[i].a].x;
 p2[0].y=_pp[nums[i].a].y;
 p2[1].x=_pp[nums[i].b].x;
 p2[1].y=_pp[nums[i].b].y;
Polyline(hdc,p2,2);
}
 }

old=SelectObject(hdc,old);
 ::DeleteObject(pen);
};

POINT2D TView3d_2d::FromLP2d(POINT2D pt,BOOL yup)
 {POINT2D delta=origin;delta.x-=pt.x;delta.y-=pt.y;
  pt.x-=(origin.x);
  if (yup)
  pt.y=offset.y-(pt.y-origin.y);
  else pt.y-=(origin.y);
  space2d->_LpToDpWO(&pt,1);
  return pt;
 };


 void TView3d_2d::CalcObr(){
  det=v11*v22*v33+v21*v32*v13+v12*v23*v31-
  (v13*v22*v31+v12*v21*v33+v23*v32*v11);
  o11=(v22*v33-v32*v23)/det;
  o21=-(v12*v33-v32*v13)/det;
  o31=(v12*v23-v22*v13)/det;

  o12=-(v21*v33-v31*v23)/det;
  o22=(v11*v33-v31*v13)/det;
  o32=-(v11*v23-v13*v21)/det;

  o13=(v21*v32-v31*v22)/det;
  o23=-(v11*v32-v31*v12)/det;
  o33=(v11*v22-v21*v12)/det;
 };
 void  TView3d_2d::Perspective(POINT3D*in,POINT3D*out){
 double x0=in->x-o_x,y0=in->y-o_y,z0=in->z-o_z;
 double x,y,z;
  z=v13*x0+v23*y0+v33*z0+v43;
  x=v11*x0+v21*y0+v31*z0;
  y=v12*x0+v22*y0+v32*z0;

  if (perspective){ //STEP 2
   if(z>0){         //STEP 2.1
    double _z=z/focus+1;
    x=x*_z;
    y=y*_z;
   }else
   {                //STEP 2.2
   double _z=-z+focus;
   x=x*focus/_z;
   y=y*focus/_z;
   }
  }
  out->x=x+offset.x; out->y=y+offset.y; out->z=z; //STEP 1
 };
 void TView3d_2d::UnPerspective(POINT3D*in,POINT3D*out){
  if (!inversecalc){CalcObr();inversecalc=1;};
  double xx,yy,zz;
         xx=in->x-offset.x;
         yy=in->y-offset.y;
         zz=in->z;            //STEP 1

  if (perspective){     //STEP 2
   if(zz>0){             //STEP 2.1
    double _z= zz/focus+1;
     xx=xx/_z;
     yy=yy/_z;
   }else{               //STEP 2.2
   double _z=-zz+focus;
   xx=(xx*_z)/focus;
   yy=(yy*_z)/focus;
   }
  }
  zz-=v43;
  out->x=o11*xx+o12*yy+o13*zz+o_x;
  out->y=o21*xx+o22*yy+o23*zz+o_y;
  out->z=o31*xx+o32*yy+o33*zz+o_z;
 };

 void  TView3d_2d::Perspective(POINT3D*in,_TPOINT3D*out){
 double x0=in->x-o_x,y0=in->y-o_y,z0=in->z-o_z;
 double x,y,z;
  x=v11*x0+v21*y0+v31*z0;
  y=v12*x0+v22*y0+v32*z0;
  z=v13*x0+v23*y0+v33*z0+v43;
  out->__x=x;out->__y=y;
  if (perspective){
    if(z>0){
    double _z=z/focus+1;
    x=x*_z;
    y=y*_z;
   }  else
   {
   double _z=-z+focus;
   x=x*focus/_z;
   y=y*focus/_z;
   }
  }
   out->_x=(out->x=(x+offset.x))*256.0;
   out->_y=(out->y=(y+offset.y))*256.0;
   out->_z=(out->z=z)*256.0;
 };

 void  TView3d_2d::GetMatrix(_TObject3d*o){
  TMatrix om(o->object->matrix.a);
   om.a[3][0]-=o_x;
   om.a[3][1]-=o_y;
   om.a[3][2]-=o_z;
   o->matrix.Mul(om,matrix); // reverse
  // o->matrix.a[3][2]+=1000;

 };

/*
 void TView3d_2d::Perspective(_TObject3d*o,C_TYPE i){
   POINT3D * pi=o->object->points+i;
   _TPOINT3D* po=(_TPOINT3D*)o->points->At(i);
   Perspective(pi,po);
 };

 void TView3d_2d::PerspectiveAllPoints(_TObject3d*o){
   POINT3D * pi=o->object->points;
   _TPOINT3D* po;
   TCollection*_points=o->points;
   C_TYPE pcount=_points->count;

   for(C_TYPE i=0;i<pcount;i++){
	 po=(_TPOINT3D*)_points->_At(i);
	 Perspective(pi,po);
     pi++;
    }
 };
*/

void TView3d_2d::Perspective(POINT3D*in,POINT3D*out,int count){
 for(int i=0;i<count;i++){
  Perspective(in+i,out+i);
 }
};

void TView3d_2d::UpdatePrimitive(_TObject3d*o,C_TYPE i){
 PRIMITIVE3D* p=(PRIMITIVE3D*)o->object->primitives->At(i);
 long rleft,rright,rtop,rbottom;

 for(INT16 j=0;j<p->count;j++){
  _TPOINT3D* point=(_TPOINT3D*)(o->points->At(p->points[j]));
  if (j==0){
	rleft=rright=point->_x;
	rtop=rbottom=point->_y;
  }else{
   if (rleft>point->_x)rleft=point->_x; else{if (rright<point->_x)rright=point->_x; }
   if (rtop>point->_y)rtop=point->_y; else{if (rbottom<point->_y)rbottom=point->_y; }
  }
 }
 RECT r;
 r.left=rleft/256l;r.right=rright/256l;
 r.top=rtop/256l;r.bottom=rbottom/256l;
 r.right++;r.bottom++;
 UpdateRect(&r);
};

BOOL TView3d_2d::IsCompatible(int){
 return 0;
};

 int  TRenders3d::Compare(pointer key1,pointer key2){
   return lstrcmpi((char*)key1,(char*)key2);
 };
 void TRenders3d::FreeItem(pointer p){
  RENDERLIB*r=(RENDERLIB*)p;
  FreeLibrary(r->lib);
  //MessageBeep(-1);
  delete r;
 };
 RENDERLIB * TRenders3d::GetLib(char*r3d){
   C_TYPE index;
   if (Search(r3d,index))return (RENDERLIB*)At(index);
   char s[256];
//   MessageBox(0,_graphics_ini,"Graphics library",MB_OK|MB_TASKMODAL);
   GetPrivateProfileString(
#ifdef WIN32
   "RENDER3D_v32"
#else
   "RENDER3D_v16"
#endif
   ,r3d,"",s,sizeof(s),_graphics_ini);
   if (lstrlen(s)){
	char *ps=s;
	char lib[256]="";
	char proc[128]="";
	while(*ps && *ps!=',')ps++;
	lstrcpyn(lib,s,(ps-s)+1);

	if (*ps==',')lstrcpy(proc,ps+1);else lstrcpy(proc,"RenderProc3d");
	HINSTANCE hlib=LoadLibrary(lib);
#ifdef WIN32
 if (hlib)
#else
 if (hlib > HINSTANCE_ERROR)
#endif
   {
 INT32  (WINAPI*MessageProc)(INT16,INT32,INT32);
 (FARPROC)MessageProc=GetProcAddress(hlib,proc);
  if (!MessageProc){  FreeLibrary(hlib);  return NULL; }
   RENDERLIB*r=new RENDERLIB;
			 r->lib=hlib;
	 r->MessageProc=MessageProc;
	 lstrcpy(r->Name,r3d);
	  r->MessageProc(RM3D_INITDRIVER,(INT32)hInstance,0);

	 Insert(r);
	 return r;
   }else{
	char ss[256];
	wsprintf(ss,"3d driver (%s) load error",s);
	MessageBox(0,ss,"Graphics library",MB_OK|MB_TASKMODAL);
   }
  }
  return NULL;
 };
TRenders3d * _renders=NULL;

_EXTERNC BOOL _EXPORT Driver3dDialog(HWND hwnd,char*r3d){
 INT32 (WINAPI * MessageProc)(INT16,INT32,INT32);
 (void*)MessageProc =GetRenderByName(r3d);
 if (!MessageProc)MessageProc=Default3dMessage;
 if(MessageProc){
   return MessageProc(RM3D_DRIVERSETUP,(UINT32)hwnd,0);
 }
}


void *GetRenderByName(char*r3d){
 char s[256];
  if(!lstrcmpi(r3d,"Auto")){
   if(GetPrivateProfileString(
#ifdef WIN32
   "RENDER3D_v32"
#else
   "RENDER3D_v16"
#endif
   ,"Auto","",s,sizeof(s),_graphics_ini))r3d=s;
  }
 if (!_renders)_renders=new TRenders3d();
 RENDERLIB*r=_renders->GetLib(r3d);
 if (r){
  return r->MessageProc;
 }
 return NULL;
};
void Done3dSystem(){
if (_renders)DeleteCollection(_renders);
};
