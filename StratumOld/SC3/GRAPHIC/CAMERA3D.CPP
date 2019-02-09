#include "object3d.h"
#include "camera3d.h"
#include "object2d.h"
#include "view3d.h"
#include <mem.h>
BOOL TCamera3d::SetPoint(POINT3D&p,int i){
 POINT3D * _p=NULL;
 switch(i){
   case 0:_p=&(camera.poOrg);break;
   case 1:_p=&(camera.poDirection);break;
   case 2:_p=&(camera.poUp);break;
   case 3:_p=&(camera.poExtent);break;
   case 4:if((camera.poOffset.x!=p.x)||(camera.poOffset.y!=p.y)){
            camera.poOffset.x=p.x;
            camera.poOffset.y=p.y;
            return TRUE;
         }break;
   case 5:if((camera.poNear_Clip!=p.x)||(camera.poFar_Clip!=p.y)){
            camera.poNear_Clip=p.x;
            camera.poFar_Clip=p.y;
            return TRUE;
         }break;
   }
 if(_p){
  if((_p->x!=p.x)||(_p->y!=p.y)||(_p->z!=p.z)){
  *_p=p;return TRUE;
  }
 }
 return FALSE;
};
BOOL TCamera3d::GetPoint(POINT3D&p,int i){
 POINT3D * _p=NULL;
 switch(i){
   case 0:_p=&(camera.poOrg);break;
   case 1:_p=&(camera.poDirection);break;
   case 2:_p=&(camera.poUp);break;
   case 3:_p=&(camera.poExtent);break;
   case 4:{
            p.x=camera.poOffset.x;
            p.y=camera.poOffset.y;
            p.z=0;
            return TRUE;
          }break;
   case 5:{
            p.x=camera.poNear_Clip;
            p.y=camera.poFar_Clip;
            return TRUE;
         }break;
   }
 if(_p){ p=*_p;return TRUE;  }
 return FALSE;
};

TCamera3d::TCamera3d(CAMERA3D *_camera){
 camera=*_camera;
 views=new TCollection(2,2);
};
TCamera3d::~TCamera3d(){
 views->FreeAll();
 delete views;
};
 TCamera3d::TCamera3d(PStream ps,LONG _pos):TObject(ps,_pos){
   int size=ps->ReadWord();
   memset(&camera,0,sizeof(CAMERA3D));
   ps->Read(&camera,size);
   views=new TCollection(2,2);
 };
 void TCamera3d::Store(PStream ps){
   TObject::Store(ps);
   ps->WriteWord(sizeof(CAMERA3D));
   ps->Write(&camera,sizeof(CAMERA3D));
 };

 void  TCamera3d::Recalc(BOOL nb){
   for(C_TYPE i=0;i<views->count;i++){
	TView3d_2d*v=(TView3d_2d*)views->At(i);
   if(nb)v->NewBuffers();
	v->UpdateCamera(TRUE,TRUE);
   }
 };

BOOL TCamera3d::GetOrg_Size3d(POINT3D &o,POINT3D &s){
  if (&s){  s.x=s.y=s.z=0; }
  if (&o){  o=camera.poOrg; }
  return TRUE;
 };

void TCamera3d::GetTargetPoint3d(POINT3D&p){
 if (camera.poFlags&CAMERA_TARGET){
    p=camera.poDirection;
 }else{
    p.x=camera.poDirection.x+camera.poOrg.x;
    p.y=camera.poDirection.y+camera.poOrg.y;
    p.z=camera.poDirection.z+camera.poOrg.z;
 }
};
void TCamera3d::SetTargetPoint3d(POINT3D&p){
 if (camera.poFlags&CAMERA_TARGET){
    camera.poDirection=p;
 }else{
    camera.poDirection.x=p.x-camera.poOrg.x;
    camera.poDirection.y=p.y-camera.poOrg.y;
    camera.poDirection.z=p.z-camera.poOrg.z;
 }
};

 void TCamera3d::Transform3d(TMatrix&m){
  POINT3D p,p1;
  m.Mul(p,camera.poOrg);
  camera.poOrg=p;
  m.Mul(p,camera.poUp);
  camera.poUp=p;
  GetTargetPoint3d(p);
  m.Mul(p1,p);
  SetTargetPoint3d(p1);
  Recalc();
 };

 TMatrix2*TCamera3d::GetMatrix(TMatrix2*m){
   m->ReSet();
   m->a.Put(3,1,camera.poOrg.x);
   m->a.Put(3,2,camera.poOrg.y);
   m->a.Put(3,3,camera.poOrg.z);
   m->bmustcalc=TRUE;
   m->CalcObr();
   return m;
 };

