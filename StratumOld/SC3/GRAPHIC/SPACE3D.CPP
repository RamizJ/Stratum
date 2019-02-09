/*
Copyright (c) 1995  Virtual World
Module Name:
     space3d.cpp
Author:
     Alexander Shelemekhov
*/


#include <windows.h>
#include "space3d.h"
#include "space2d.h"
#include "object2d.h"
#include "camera3d.h"
#include "view3d.h"
#include "rotator3.h"

#include <gwin2d3d\3drender.h>
#include <mem.h>
#include <math.h>
#include "_g_def.h"

struct TCRD{
    BYTE type;
    HOBJ2D hObject;
    double m[4][4];
};

BOOL TSpace3d::IsObjectVisible(PObject3d p){
    if (p->IsEmpty()) return 0;
    return TRUE;
};

//*---------- services -----------------*
void TSpace3d::Store(PStream ps){
    ps->WriteWord(_HANDLE);
    ps->Put(primary);
    ps->Put(all);
    if(materials->GetCountToSave()){
        //  ps->WriteWord(10);
        ps->Put(materials);
    }
    // ps->WriteWord(16);
};

HSP3D InsertSpace3d(PSpace3d ps3d);

TSpace3d::TSpace3d(PStream ps,LONG _pos):_SPACE(ps){
    materials=0;
    dataitems=NULL;
    all=NULL;primary=NULL;
    _HANDLE=ps->ReadWord();
    if(ps->fileversion>=0x0300)
    {
        while(_pos>ps->GetPos()){

            CHUNK _chunk;
            _chunk.InitRead(ps);
            PObject p=(PObject)(ps->AllocateObject(_chunk));
            if(p){
                switch(p->WhoIsIt()){
                    case otPRIMARYCOLLECTION:primary=(PPrimaryCollection)p;break;
                    case otOBJECTCOLLECTION:all    =(PObjectCollection)p;break;
                    case otDATAITEMS:dataitems=(TDataItems*)p;break;
                    case otMATERIALS3D:materials=(TMaterialls3d*)p;break;
                };
            }else{

            }
            _chunk.Check();
        }

    }else{

        primary=(PPrimaryCollection)ps->Get();
        all=(PObjectCollection)ps->Get();

        BOOL ok=TRUE;
        do{
            int a=ps->ReadWord();
            switch(a){
                case 10:{(void*)materials=ps->Get(1024);
                }break;
                case 16:ok=FALSE;break;
            }
        }while (ok);
    }
    //if (a!=16)_Error(0);
    if(materials){
        for(C_TYPE i=0;i<materials->count;i++){
            TMaterial3d* m=(TMaterial3d*)materials->At(i);
            m->space3d=this;
        }}
    for(C_TYPE i=0;i<all->count;i++)((PObject)all->At(i))->Awaken(this);
    _TSpace3d();
    InsertSpace3d(this);
    _Awaken();
    //a++;
};

//*-------------------------------------------------------------*//
BOOL TSpace3d::SetObjectColor3d(HOBJ2D h,COLOR3D c){
    PObject3d po=(PObject3d)all->GetByHandle(h);
    if (po)switch(po->WhoIsIt()){
        case 10:{
            po->SetColor3d(c);
            UpdateObject(po);
        }
            return TRUE;
        case  5:{for(C_TYPE i=0;i<((PGroup3d)po)->items->count;i++)
                SetObjectColor3d(PObject(((PGroup3d)po)->items->At(i))->_HANDLE,c);
        }
            return TRUE;
    }return FALSE;
};

BOOL TSpace3d::SetMatrix(HOBJ2D Object3d,void*buf){
    PObject3d po=(PObject3d)all->GetByHandle(Object3d);
    if(po->WhoIsIt()==10){
        UpdateObject(po,UOF_BUFFER);
        po->matrix.Set(*((TMatrix*)buf));
        po->UpdateInView();
        UpdateObject(po,UOF_BUFFER);
        return TRUE;
    }
    return FALSE;
};

INT16 TSpace3d::GetObjectColor3d(HOBJ2D h,COLOR3D&color){
    PObject3d po=(PObject3d)all->GetByHandle(h);
    if (po)switch(po->WhoIsIt()){
        case 10:{
            for(C_TYPE i=0;i<po->primitives->count;i++){
                PRIMITIVE3D *p=(PRIMITIVE3D *)po->primitives->At(i);
                if (!i){ color=p->color;}
                else
                { if ((p->color.rgb) != (color.rgb)) return 2;
                }
            }
        }return 1;
        case 5:{COLOR3D col;
            for(C_TYPE i=0;i<((PGroup3d)po)->items->count;i++){
                INT16 ret=GetObjectColor3d(PObject(((PGroup3d)po)->items->At(i))->_HANDLE,col);
                if (ret==2)return ret;
                if(!i){
                    color=col;
                }else{
                    if ((col.rgb) != (color.rgb))return 2;
                }
            }
        }return 1;
    }return 0;
};

BOOL TSpace3d::StoreColors3d(PStream,HOBJ2D h){
    PObject3d po=(PObject3d)all->GetByHandle(h);
    if (po){
        return TRUE;
    }
    return FALSE;
};

BOOL TSpace3d::LoadColors3d(PStream ps){
    HOBJ2D h=(HOBJ2D)ps->ReadWord();
    PObject3d po=(PObject3d)all->GetByHandle(h);
    if (po)
        switch(po->WhoIsIt()){
            case 10:{
            }return TRUE;
            case 5:{for(INT16 i=0;i<((PGroup3d)po)->items->count;i++)
                    if (LoadColors3d(ps)==0)return 0;
            }return 1;
        }return 0;
};
//*-------------------------------------------------------------*//
void TSpace3d::MarkToSave(int m)
{ needsave=m;
    int i;
    for(i=0;i<all->count;i++)((PObject)all->At(i))->MarkToSave(m);
}
PGroup TSpace3d::_CreateGroup()
{
    return (PGroup)new TGroup3d();
};

HOBJ2D TSpace3d::CreateObject3d()
{ PObject3d po=new TObject3d();
    po->space3d=this;
    HOBJ2D ho=InsertObject(po);
    if (ho){
        for(C_TYPE i=0;i<views->count;i++){
            TView3d_2d* v=(TView3d_2d*)views->At(i);
            C_TYPE index=v->items->Insert(new _TObject3d(po));
            v->UpdateObject(index,UIV_MATRIX);
        }
    }
    return ho;
}
BOOL IsMaterialEqual(TMaterial3d*m1,TMaterial3d*m2){
    if(m1->_m.flags!=m2->_m.flags)return FALSE;
    if(m1->_m.name!=m2->_m.name){
        if((m1->_m.name==NULL)||(m2->_m.name==NULL))return FALSE;
        if(lstrcmp(m1->_m.name,m2->_m.name))return FALSE;
    }
    if(m1->file!=m2->file){
        if((m1->file==NULL)||(m2->file==NULL))return FALSE;
        if(lstrcmp(m1->file,m2->file))return FALSE;
    }
    if(m1->_m.color.rgb!=m2->_m.color.rgb)return FALSE;

    if(m1->dib && m2->dib){
        if(!IsDibsEqual(m1->dib,m2->dib))return FALSE;
    }

    return TRUE;
};

HOBJ2D TSpace3d::PasteObject(PStream s,POINT3D * p)
{
    {LONG pos=s->GetPos();
        INT16 id=s->ReadWord();
        switch(id){
            case otSPACE3Df:{
                s->fileversion=s->ReadWord();
                int mver=s->ReadWord();
                if(mver>currentfileversion)return 0;
            }break;
            case otSPACE3D :{
                s->fileversion=0x0203;
                s->Seek(pos);
            }break;
            default:return 0;
        }
    }
    PSpace3d ps=(PSpace3d)s->Get(17459);
    if ((ps)&&(ps->all)&&(ps->all->current))
    {
        while(ps->materials->count){
            TMaterial3d*_m=(TMaterial3d*)ps->materials->At(0);
            for(C_TYPE j=0;j<materials->count;j++){
                TMaterial3d*_m1=(TMaterial3d*)materials->At(j);
                if(IsMaterialEqual(_m,_m1)){
                    for(C_TYPE k=0;k<ps->primary->count;k++){
                        PObject3d _po=(PObject3d)ps->primary->At(k);
                        for(C_TYPE z=0;z<_po->primitives->count;z++){
                            TPrimitive3d*_p=(TPrimitive3d*)_po->primitives->At(z);
                            if(_p->_mat==_m){_p->_mat->DecRef(0);_p->_mat=_m1;_p->_mat->IncRef(0);}
                        }
                    }
                    ps->materials->AtFree(0);_m=NULL;
                    break;
                }
            }
            if(_m){
                _m->space3d=this;
                materials->Insert(_m);
            }
            ps->materials->AtDelete(0);
        }

        PObject3d po=(PObject3d)ps->GetObjByHANDLE(ps->all->current);
        for(C_TYPE i=0;i<ps->all->count;i++){
            PObject3d _po=(PObject3d)ps->all->At(i);
            all->InsertObject(_po);
            switch(_po->WhoIsIt()){
                case 10:_po->space3d=this;break;
                case 11:;break;  //camera
                case 12:;break;  //light
            }
        }
        for(i=0;i<ps->primary->count;i++){
            PObject3d _po=(PObject3d)ps->primary->At(i);
            primary->Insert(_po);
            if(views->count){
                for(C_TYPE k=0;k<views->count;k++){
                    TView3d_2d* v=(TView3d_2d*)views->At(k);
                    C_TYPE index=v->items->Insert(new _TObject3d(_po));
                    v->UpdateObject(index,UIV_ALL);
                }
                UpdateObject(_po);
            }
        }
        ps->all->DeleteAll();
        ps->primary->DeleteAll();
        // if (p && po)po->Move3d(*p);

        delete ps;
        if (po){
            if(p)SetObjectBase3d(po->_HANDLE,p);
            return po->_HANDLE;}
    }
    return 0;
};

BOOL TSpace3d::GetObjectBase(PObject3d po,POINT3D * Base){
    if(po){
m1:
        if(po->WhoIsIt()==5){
            TGroup3d*g=(TGroup3d*)po;
            if(g->items->count==0)return 0;
            po=(PObject3d)g->items->At(0);
            goto m1;
        }
        POINT3D p={0,0,0};
        return LPtoCP(po,&p,Base,1);
    }
    return TRUE;
};

BOOL TSpace3d::SetObjectOrigin3d(HOBJ2D Object3d,POINT3D * Origin)
{PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if (po)
    {if ((po->WhoIsIt()==52)&&(Object3d !=OID_FRAME3d))
        { SetFrame3dPoint(Object3d-OID_FRAME3d1,*Origin);return TRUE; }
        UpdateObject(po);

        POINT3D pp;
        CPtoWP(Origin,&pp,1);

        POINT3D oorg;//=po->Move3d(pp);
        UpdateObject(po);
        (*Origin)=oorg;
        return TRUE;
    }
    return FALSE;
};
BOOL TSpace3d::SetObjectSize3d(HOBJ2D Object3d, POINT3D * Size )
{PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if (po)
    {
        UpdateObject(po);
        po->SetSize3d(*Size);
        UpdateObject(po);
        return TRUE;
    }
    return FALSE;
};
BOOL TSpace3d::TransformObjectPoints3d(HOBJ2D Object3d,double*m){
    PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if(po && po->WhoIsIt()==10){
        UpdateObject(po);
        TMatrix mm;
        mm.Mul(matrix.b,*((TMatrix*)(m)));
        POINT3D pp;
        for(INT16 i=0;i<po->pointcount;i++){
            mm.Mul(pp,po->points[i]);
            po->points[i]=pp;
        }
        po->UpdateSize();
        po->UpdateInView();
        UpdateObject(po);
        return TRUE;
    }
    return FALSE;
};
BOOL TSpace3d::Transform3d(HOBJ2D Object3d,double*m)
{
    PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if (po)
    { UpdateObject(po);
        TMatrix mm;
        mm.Mul(matrix.b,*((TMatrix*)(m)));
        mm.PreMul(matrix.a);

        po->Transform3d(mm);
        //    po->Transform3d(*((TMatrix*)m));
        UpdateObject(po);
        return TRUE;
    }
    return FALSE;
};
BOOL TSpace3d::SetObjectBase3d(HOBJ2D Object3d,POINT3D * Base){
    PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if (po)
    {if ((po->WhoIsIt()==52)&&(Object3d !=OID_FRAME3d))
        { SetFrame3dPoint(Object3d-OID_FRAME3d1,*Base);return TRUE; }
        UpdateObject(po,UOF_BUFFER);
        POINT3D pp,p1,p2;
        p1.x=p1.y=p1.z=0.0;
        GetObjectBase(po,&p1);
        CPtoWP(Base,&pp,1);
        CPtoWP(&p1,&p2,1);
        TMatrix m;
        m.Set();
        m.Put(4,1,-(p2.x-pp.x));
        m.Put(4,2,-(p2.y-pp.y));
        m.Put(4,3,-(p2.z-pp.z));

        po->Transform3d(m);

        UpdateObject(po,UOF_BUFFER);
        return TRUE;
    }
    return FALSE;
};

BOOL TSpace3d::RotateObject3d(HOBJ2D Object3d, ROTATOR3D * rotator )
{PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if (po)
    { UpdateObject(po,UOF_BUFFER);

        ROTATOR3D r=*rotator;
        CPtoWP(&(rotator->VctOrg),&(r.VctOrg),1);
        CStoWS(&(rotator->VctSize),&(r.VctSize),1);
        TRotator3d _r(&r);
        po->Transform3d(_r.r);

        /*
   ROTATOR3D r=*rotator;
   TRotator3d _r(&r);
   TMatrix mm;
    mm.Mul(matrix.b,(_r.r));
    mm.PreMul(matrix.a);
   po->Transform3d(mm);
  */
        UpdateObject(po,UOF_BUFFER);
        return TRUE;
    }
    return FALSE;
};


BOOL TSpace3d::SetPoints3di( HOBJ2D Object3d,POINT3D * points,INT16 *nums,INT16 count )
{PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if (po)
    {
        int type=po->WhoIsIt();
        if ((type==10)&&po->ChkNewPoints(nums,count));
        {
            UpdateObject(po);
            for (C_TYPE i=0;i<count;i++) *(po->points+nums[i])=points[i];

            if (views->count){
                C_TYPE index=primary->IndexOf(po);
                for(C_TYPE i=0;i<views->count;i++){
                    TView3d_2d* v=(TView3d_2d*)views->At(i);
                    _TObject3d*p=(_TObject3d*)v->items->At(index);
                    for(INT16 j=0;j<count;j++){
                        p->Perspective(nums[j]);
                    }
                    v->UpdateOrgSizeView(p);
                }
            }
            po->UpdateSize();
            UpdateObject(po);
            return TRUE;
        };
        if (type==52){
            for(INT16 i=0;i<count;i++)if (nums[i]<0 || nums[i]>7 )return FALSE;
            for (i=0;count;i++)SetFrame3dPoint(nums[i],points[i]);
            return TRUE;
        }
        if (type==11){ // Camera3d
            for(INT16 i=0;i<count;i++)if (nums[i]<0)return FALSE;
            BOOL u=FALSE;
            for (i=0;i<count;i++)u|=((TCamera3d*)po)->SetPoint(points[i],nums[i]);
            if(u)((TCamera3d*)po)->Recalc();
            return TRUE;
        }

    }
    return FALSE;
};

BOOL TSpace3d::SetPoints3d( HOBJ2D Object3d,POINT3D * points,INT16 from,INT16 count)
{PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if (po){
        int type=po->WhoIsIt();
        if((type==10)&& (from>=0) && ((from+count)<=po->pointcount))
        {
            UpdateObject(po);
            for (C_TYPE i=0;i<count;i++)  *(po->points+i+from)=points[i];
            po->UpdateSize();

            if (views->count){
                C_TYPE index=primary->IndexOf(po);
                for(C_TYPE i=0;i<views->count;i++){
                    TView3d_2d* v=(TView3d_2d*)views->At(i);
                    _TObject3d*p=(_TObject3d*)v->items->At(index);
                    for(INT16 j=0;j<count;j++){
                        p->Perspective((INT16)(j+from));
                    }
                    v->UpdateOrgSizeView(p);
                    p->UpdateAllPrimitives();
                }
            }
            UpdateObject(po);
            return TRUE;
        }

        if ((type==52) && ((from>=0) && ((from+count)<8))){
            for (C_TYPE i=0;count;i++)SetFrame3dPoint(from+i,points[i]);
            return TRUE;
        }
        if (type==11){ // Camera3d
            for(INT16 i=0;i<count;i++)if (from<0)return FALSE;
            BOOL u=FALSE;
            for (i=0;i<count;i++)u|=((TCamera3d*)po)->SetPoint(points[i],from+i);
            if(u) ((TCamera3d*)po)->Recalc();
            return TRUE;
        }
    }
    return FALSE;
};

INT16 TSpace3d::AddPoints3d(HOBJ2D Object3d, POINT3D * points,INT16 count )
{PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if ((po)&&(po->WhoIsIt()==10)&&po->pointcount+count<max_size)
    { POINT3D *_points=new POINT3D[po->pointcount+count];
        int oldsize=po->pointcount*sizeof(POINT3D);
        if (oldsize)memcpy(_points,po->points,oldsize);
        memcpy(_points+po->pointcount,points,count*sizeof(POINT3D));
        po->pointcount+=count;
        if (po->points)delete po->points;
        po->points=_points;
        po->UpdateSize();
        if (views->count){
            C_TYPE index=primary->IndexOf(po);
            for(C_TYPE i=0;i<views->count;i++){
                TView3d_2d* v=(TView3d_2d*)views->At(i);
                _TObject3d*p=(_TObject3d*)v->items->At(index);
                for(C_TYPE k=0;k<count;k++){
                    C_TYPE j=p->points->Insert(new _TPOINT3D);
                    p->Perspective(j);
                }
                v->UpdateOrgSizeView(p);
            }}
        return po->pointcount;
    }
    return 0;
};

BOOL TSpace3d::DelPoints3di( HOBJ2D Object3d,INT16*nums,INT16 num){
    PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if ((po)&&(po->WhoIsIt()==10)&&num){
        for(INT16 i=0;i<num;i++){
            if (po->IsPointInUse(nums[i]))return FALSE;
        }
        for(i=0;i<num;i++)po->_DeletePoint(nums[i]);
        if(num==po->pointcount){
            delete (po->points);
            po->points=NULL;
        }else{
            POINT3D *_points=new POINT3D[po->pointcount-num];
            INT16 k=0;
            for(i=0;i<po->pointcount;i++){
                for(INT16 j=0;j<num;j++){
                    if (nums[i]==i)goto m1;
                }
                _points[k]=po->points[i];
                k++;
m1:;
            }
            delete (po->points);
            po->points=_points;
        }
        if (views->count){
            C_TYPE index=primary->IndexOf(po);
            for(C_TYPE i=0;i<views->count;i++){
                TView3d_2d* v=(TView3d_2d*)views->At(i);
                _TObject3d*p=(_TObject3d*)v->items->At(index);
                for(INT16 j=0;j>num;j++){
                    p->points->AtFree(nums[j]);
                    p->points->AtPut(nums[j],NULL);
                }
                p->points->Pack();
            }}
        po->pointcount-=num;
        po->UpdateSize();
    }
    return TRUE;
};

BOOL TSpace3d::DelPoints3d( HOBJ2D Object3d,INT16 from,INT16 count){
    PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if ((po)&&(po->WhoIsIt()==10) && from>=0 && ((from +count)<=po->pointcount) && count){
        for(INT16 i=0;i<count;i++){
            if (po->IsPointInUse((INT16)(from+i)))return FALSE;
        }
        if (count!=po->pointcount){
            POINT3D * _points=new POINT3D[po->pointcount-count];
            if(from)memcpy(_points,po->points,from*sizeof(POINT3D));
            int _c=po->pointcount-(from+count);
            if(_c>0)memcpy(_points+from,po->points+from+count,sizeof(POINT3D)*(_c));
        }else{
            delete po->points;
            po->points=NULL;
        }
        if (views->count){
            C_TYPE index=primary->IndexOf(po);
            for(C_TYPE i=0;i<views->count;i++){
                TView3d_2d* v=(TView3d_2d*)views->At(i);
                _TObject3d*p=(_TObject3d*)v->items->At(index);
                for(INT16 j=0;j>count;j++){
                    p->points->AtFree((INT16)(from));
                    p->points->AtDelete((INT16)(from));
                }
            }}
        po->pointcount-=count;
        po->UpdateSize();
    }
    return TRUE;
};

INT16 TSpace3d::DelUnusedPoints3d(HOBJ2D Object3d ,PStream ps)
{PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if ((po)&&(po->WhoIsIt()==10)){
    }
    return 0;
};

void AddPoint3dToBar3d(POINT3D & o,POINT3D & s,POINT3D* t){
    o.x=min(o.x,t->x);o.y=min(o.y,t->y);o.z=min(o.z,t->z);
    s.x=max(s.x,t->x);s.y=max(s.y,t->y);s.z=max(s.z,t->z);
}
/*
void TSpace3d::UpdateTriangle(PObject3d po,PTriangle pt)
{ if (po->options & 1)return;
TPoint3d o=*(pt->t1),s=o;
 AddPoint3dToBar3d(o,s,pt->t2);
 AddPoint3dToBar3d(o,s,pt->t3);
 s.x-=o.x;s.y-=o.y;s.z-=o.z;
 UpdateBar3d(&o,&s);
};
void TSpace3d::UpdateLine(PObject3d po,PLine3d pl)
{ if (po->options & 1)return;
  TPoint3d o=*(pl->t1),s=o;
  AddPoint3dToBar3d(o,s,pl->t2);
    s.x-=o.x;s.y-=o.y;s.z-=o.z;
 UpdateBar3d(&o,&s);
};
void TSpace3d::UpdatePoint(PObject3d po,PPoint3d p)
{ if (po->options & 1)return;
  TPoint3d o=*p,s=o;
  for (int i=0;i<po->triangles->count;i++){
  PTriangle pt=(PTriangle)po->triangles->At(i);
  if (pt->t1==p || pt->t2==p || pt->t3==p){
  if (pt->t1!=p) AddPoint3dToBar3d(o,s,pt->t1);
  if (pt->t2!=p) AddPoint3dToBar3d(o,s,pt->t2);
  if (pt->t3!=p) AddPoint3dToBar3d(o,s,pt->t3); }
  }
  for (i=0;i<po->lines->count;i++){
  PLine3d pl=(PLine3d)po->lines->At(i);
  if (pl->t1==p || pl->t2==p){
  if (pl->t1!=p) AddPoint3dToBar3d(o,s,pl->t1);
  if (pl->t2!=p) AddPoint3dToBar3d(o,s,pl->t2);}
  }
 s.x-=o.x;s.y-=o.y;s.z-=o.z;
 UpdateBar3d(&o,&s);
};
*/
//*--------------- 3d Collection -----------------*//
HOBJ2D  T3dCollection::GetHandle(pointer p)
{return ((PSpace3d)p)->_HANDLE;
};
void T3dCollection::SetHandle(pointer p,HOBJ2D h)
{ ((PSpace3d)p)->_HANDLE=h;
};
BOOL  T3dCollection::MayDelete(pointer p)
{return ((PSpace3d)p)->views->count==0;
};
BOOL T3dCollection::IsNeedToSave(pointer p)
{return ((PSpace3d)p)->needsave;
};
/*----------- End of 3d Collection -------------*/

BOOL TSpace3d::_DeleteObject3d(PObject po){
    int type=po->WhoIsIt();
    switch(type){
        case 5:{
            PGroup3d pg=(PGroup3d)po;
            for(C_TYPE i=0;i<pg->items->count;i++){
                _DeleteObject3d((PObject)pg->items->At(i));
            }
        }break;
        case 10:{
            C_TYPE index=-1;
            for(C_TYPE i=0;i<views->count;i++){
                TView3d_2d* v=(TView3d_2d*)views->At(i);
                for (C_TYPE j=0;j<v->items->count;j++){
                    _TObject3d*to=(_TObject3d*)v->items->At(j);
                    if(to->object==po){index=j;break;}
                }
                if(index>-1){
                    v->items->AtFree(index);
                    v->items->AtDelete(index);
                }
            }}break;
        case 11:

            break;
        case 12:
            break;

    };
    return TRUE;
}
BOOL IsSpacse3dEqual(PSpace3d ps1,PSpace3d ps2)
{if ((ps1->primary->count!=ps2->primary->count) ||
            (ps1->all->count!=ps2->all->count))return FALSE;
    INT16 i,t;
    PObject3d po1,po2;
    for(i=0;i<ps1->all->count;i++)
    {po1=(PObject3d)ps1->all->At(i);
        po2=(PObject3d)ps2->all->At(i);
        if (po1->_HANDLE!=po2->_HANDLE) return FALSE;
        t=po1->WhoIsIt();
        if (t!=po2->WhoIsIt())return FALSE;
        switch(t){
            case 10:
            {if (po1->pointcount!=po2->pointcount)return FALSE;
                if (po1->flag!=po2->flag)return FALSE;
                if (po1->primitives->count!=po2->primitives->count)return FALSE;
                for(i=0;i<po1->primitives->count;i++)
                {TPrimitive3d*pr1=(TPrimitive3d*)po1->primitives->At(i);
                    TPrimitive3d*pr2=(TPrimitive3d*)po2->primitives->At(i);
                    if ((pr1->count!=pr2->count)||
                            (pr1->flags!=pr2->flags)||
                            ((*((UINT32*)&(pr1->color)))!=(*((UINT32*)&(pr2->color))))
                            )return FALSE;
                    // Далее по точкам...

                }
            }break;
                // Not Primary
            case 11:  //Camera
            {
                BYTE *pc1=(BYTE*)&(((TCamera3d*)po1)->camera),
                        *pc2=(BYTE*)&(((TCamera3d*)po2)->camera);
                for(int j=0;j<sizeof(CAMERA3D);j++){
                    if (pc1[j]!=pc2[j]) return FALSE;
                }
            }break;
            case 12: //light
            {

            }break;
            case 5:
            {PGroup3d pg1=(PGroup3d)po1,pg2=(PGroup3d)po2;
                if (pg1->items->count!=pg2->items->count)return FALSE;
                for (INT16 j=0;j<pg2->items->count;j++)
                    if (((PObject)pg1->items->At(j))->_HANDLE!=
                            ((PObject)pg2->items->At(j))->_HANDLE)return FALSE;
            }break;
            default:_Error(EM_internal);
        }
    }
    return TRUE;
};
TSpace3d::TSpace3d():_SPACE(){
    // pushpop=new TItems(5,5);
    materials=0;
    _TSpace3d();
};
void TSpace3d::_TSpace3d()
{
    space2d=NULL;
    views=new TCollection(10,10);
    if(!materials)materials=new TMaterialls3d(10,2);
    ResetCrd();
};
//--------------------------------------------

void TObject3d::SetState(WORD astate,ATTR_MODE op,pointer p)
{WORD old=options,uo=0;
    //	RECT r=GetRect();
    TObject::SetState(astate,op,p );
    const mask1=0xffffu^(OF_HARDDONTSELECT|OF_DONTSELECT);
    if ( (old & mask1) != (mask1 & 0x1f))  uo=1;

    if((old& (STATE_BOUNDS|STATE_AXIS3D))!=(options& (STATE_BOUNDS|STATE_AXIS3D))){
        WORD _op=options;
        options|=STATE_BOUNDS|STATE_AXIS3D;
        ((PSpace3d)p)->UpdateObject(this,UOF_AXIS|UOF_BOUNDS);
        options=_op;
    }

    if (uo){
        WORD _op=options;
        options&=0xfffeu;//Чтоб был Visible, чтоб взять его область обновления

        const mask=STATE_EDIT3D|STATE_WIREFRAME|STATE_NOSHADED|STATE_ALLFACES|STATE_POINTS;

        if ( (old & mask) != (options & mask)){
            UpdateInView(UIV_PRIMITIVE);
        }
        if( (old&(~(STATE_BOUNDS|STATE_AXIS3D))) !=
                (options&(~(STATE_BOUNDS|STATE_AXIS3D)))
                )((PSpace3d)p)->UpdateObject(this);
        //      UpdateInView(TRUE);
        options=_op;
    }
};

HOBJ2D TSpace3d::CreateCamera3d(CAMERA3D*camera){
    TCamera3d* cam=new TCamera3d(camera);
    return all->InsertObject(cam);
};
BOOL   TSpace3d::SetCamera3d(HOBJ2D hCam,CAMERA3D*camera){
    TCamera3d* cam=(TCamera3d*)all->GetByHandle(hCam);
    if (cam && cam->WhoIsIt()==11){
        BOOL nb=(cam->camera.poRenderType&RENDER_TRANSPARENT)!=(camera->poRenderType&RENDER_TRANSPARENT);
        cam->camera=*camera;
        cam->Recalc(nb);
        return TRUE;
    }
    return FALSE;
};

HOBJ2D TSpace3d::CreateLight3d(LIGHT3D*l){
    TLight3d* light=new TLight3d(l);
    HOBJ2D hl=all->InsertObject(light);
    if (primary->count){
        UpdateAll();
    }
    return hl;
};
BOOL   TSpace3d::SetLight3d(HOBJ2D hl,LIGHT3D*l){
    TLight3d *light=(TLight3d*)GetObjByHANDLE(hl);
    if (light && light->WhoIsIt()==12){
        light->light=*l;
        UpdateAll();
        return TRUE;
    }
    return FALSE;
};

BOOL   TSpace3d::DelPrimitives3d(HOBJ2D Object3d,INT16* PrimNum,INT16 count){
    PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if ((po)&&(po->WhoIsIt()==10)){
        po->ChkNewPrimitives(PrimNum,count);
        for(INT16 i=0;i<count;i++){
            po->primitives->AtFree(PrimNum[i]);
            po->primitives->AtPut(i,NULL);
        }
        po->primitives->Pack();
        po->colorvalid=0;
        if (views->count){
            C_TYPE index=primary->IndexOf(po);
            for(i=0;i<views->count;i++){
                TView3d_2d* v=(TView3d_2d*)views->At(i);
                _TObject3d*p=(_TObject3d*)v->items->At(index);
                for(INT16 j=0;j>count;j++){
                    p->primitives->AtFree(PrimNum[j]);
                    p->primitives->AtPut(j,NULL);
                }
                p->primitives->Pack();
            }}
        return TRUE;
    }
    return FALSE;
};
INT16  TSpace3d::CreatePrimitives3d(HOBJ2D Object3d,PRIMITIVE3D*prim,INT16 num){
    PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if ((po)&&(po->WhoIsIt()==10)){
        for(INT16 i=0;i<num;i++){
            if (!po->ChkNewPoints(prim[i].points,prim[i].count))return FALSE;
            if(prim[i].flags&PRIMITIVE_TEXTURED && prim[i].texturePOINTS){
                for(INT16 j=0;j<prim[i].count;j++){

                    if(prim[i].texturePOINTS[j].x<0)   return FALSE;
                    if(prim[i].texturePOINTS[j].y<0)   return FALSE;
                    if(prim[i].texturePOINTS[j].x>100) return FALSE;
                    if(prim[i].texturePOINTS[j].y>100) return FALSE;

                } }	}

        for(i=0;i<num;i++){
            po->primitives->Insert(new TPrimitive3d(&(prim[i]),this));
        }
        po->colorvalid=0;
        if (views->count){
            C_TYPE index=primary->IndexOf(po);
            for(C_TYPE i=0;i<views->count;i++){
                TView3d_2d* v=(TView3d_2d*)views->At(i);
                _TObject3d*p=(_TObject3d*)v->items->At(index);
                for(INT16 j=0;j<num;j++){
                    C_TYPE k=p->primitives->Insert(new _TPRIMITIVE3D);
                    if (!v->calcpointcolor)v->UpdatePrimitive(p,k);
                    p->UpdatePrimitive(k);
                }
                //     if (calcpointcolor)o->UpdatePoints();
                if (v->calcpointcolor){
                    v->UpdateObject(index,UIV_PRIMITIVE);
                    UpdateObject(po);
                }
            }

        }
        return po->primitives->count;
    }
    return TRUE;
};

BOOL TSpace3d::SetPrimitive3d(PObject3d po,PRIMITIVE3D&prim,C_TYPE i,C_TYPE index){
    if(i<0 || i>=po->primitives->count)return FALSE;
    if (prim.points && (!po->ChkNewPoints(prim.points,prim.count)))return FALSE;
    if(prim.flags&PRIMITIVE_TEXTURED && prim.texturePOINTS){
        for(INT16 j=0;j<prim.count;j++){
            if(prim.texturePOINTS[j].x<0)   return FALSE;
            if(prim.texturePOINTS[j].y<0)   return FALSE;
            if(prim.texturePOINTS[j].x>100) return FALSE;
            if(prim.texturePOINTS[j].y>100) return FALSE;
        }
    }
    TPrimitive3d*primitive=(TPrimitive3d*)(po->primitives->At(i));
    primitive->Set(&prim,this);
    po->colorvalid=0;
    if(index>-1){
        for(C_TYPE k=0;k<views->count;k++){
            TView3d_2d* v=(TView3d_2d*)views->At(k);
            _TObject3d*p=(_TObject3d*)v->items->At(index);
            v->UpdatePrimitive(p,i);
            p->UpdatePrimitive(i);
            v->UpdatePrimitive(p,i);
        }}
    return TRUE;
};

BOOL TSpace3d::SetPrimitives3di(HOBJ2D Object3d,PRIMITIVE3D*prim,
                                INT16* items,INT16 count){
    PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if ((po)&&(po->WhoIsIt()==10)){
        C_TYPE index;
        if (views->count){
            index=primary->IndexOf(po);
        }else index=-1;
        BOOL rez=FALSE;
        for(INT16 i=0;i<count;i++){
            rez|=SetPrimitive3d(po,prim[i],items[i],index);
        }
        return rez;
    }
    return FALSE;
};

BOOL TSpace3d::SetPrimitives3d(HOBJ2D Object3d,PRIMITIVE3D*prim,
                               INT16 from,INT16 count){
    PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if ((po)&&(po->WhoIsIt()==10)){
        C_TYPE index;
        if (views->count){
            index=primary->IndexOf(po);
        }else index=-1;
        BOOL rez=FALSE;
        for(INT16 i=0;i<count;i++){
            rez|=SetPrimitive3d(po,prim[i],i+from,index);
        }
        return rez;
    }
    return FALSE;
};


INT32 TSpace3d::IsIntersect(HOBJ2D h1,HOBJ2D h2,UINT32 f){
    TObject3d*po1=(TObject3d*)GetObjByHANDLE(h1);
    TObject3d*po2=(TObject3d*)GetObjByHANDLE(h2);
    if(po1 && po2){
        if(po1==po2)return 1;
        if(po1->WhoIsIt()==5){
            return ((PGroup3d)po1)->IsIntersect(po2,f);
        }else{
            if(po1->WhoIsIt()==10){
                return po1->IsIntersect(po2,f);
            }} }
    return FALSE;
}
// Преобразование координат

void TSpace3d::UpdateCrd(){
    PObject3d po=(PObject3d)GetObjByHANDLE(localCS);
    if (po){
        matrix.Set(po->matrix.a);
        matrix.CalcObr();
    }else ResetCrd();
};

void TSpace3d::ResetCrd(){
    localCS=0;
    localType=0;
    matrix.ReSet();
    matrix.CalcObr();
};
BOOL TSpace3d::SetCrd(CRDSYSTEM*crd){
    localCS=0;
    localType=crd->type;
    switch(localType){
        case WORLD_COORDINATE:ResetCrd();return TRUE;
        case LOCAL_COORDINATE:{
            localCS=crd->hObject;
            PObject3d po=(PObject3d)all->GetByHandle(localCS);
            TMatrix2 m;
            TMatrix2* _m;
            if (po && ((_m=po->GetMatrix(&m))!=NULL)){
                matrix.Set(_m->a);
                matrix.CalcObr();
                return TRUE;
            }
        }break;
        case PARENT_COORDINATE:break;
        case VIEW_COORDINATE  :break;
        case MATRIX_COORDINATE:break;
    }
    ResetCrd();
    return FALSE;
};

BOOL TSpace3d::GetCrd(CRDSYSTEM*c){
    if(c){
        c->type=localType;
        c->hObject=localCS;
        memcpy(&(c->matrix),&(matrix.a),sizeof(c->matrix));
        return TRUE;
    } return FALSE;
};

BOOL TSpace3d::PushCrd(){
    if(!pushpop)pushpop=new TItems(5,5);
    if(pushpop->count<100){
        CRDSYSTEM * crd=new CRDSYSTEM;

        crd->type   =  localType;
        crd->hObject = localCS;

        memcpy(&crd->matrix,&matrix.a,sizeof(TMatrix));
        pushpop->Insert(crd);
        return TRUE;
    }
    return FALSE;
};

BOOL TSpace3d::PopCrd(){
    if(pushpop && pushpop->count){
        CRDSYSTEM * c=(CRDSYSTEM*)pushpop->At(pushpop->count-1);
        pushpop->AtDelete(pushpop->count-1);
        SetCrd(c);
        delete c;
        return TRUE;
    }
    return FALSE;
};
BOOL TSpace3d::LPtoCP(PObject po,POINT3D*src,POINT3D*dst,int count){
    TMatrix2 __m;
    TMatrix2* m=po->GetMatrix(&__m);
    if(m){
        for(int i=0;i<count;i++){
            POINT3D _d;
            m->a.Mul(_d,src[i]);

            matrix.b.Mul(dst[i],_d);
        }
        return TRUE;
    }
    return FALSE;
};
void TSpace3d::CPtoWP(POINT3D*src,POINT3D*dst,int count){
    for(int i=0;i<count;i++){
        matrix.a.Mul(dst[i],src[i]);
    }
};
void TSpace3d::CStoWS(POINT3D*src,POINT3D*dst,int count){
    for(int i=0;i<count;i++){
        matrix.a._Mul(dst[i],src[i]);
    }
};
void TSpace3d::WPtoCP(POINT3D*src,POINT3D*dst,int count){
    matrix.CalcObr();
    for(int i=0;i<count;i++){
        matrix.b.Mul(dst[i],src[i]);
    }
};
void TSpace3d::WStoCS(POINT3D*src,POINT3D*dst,int count){
    matrix.CalcObr();
    for(int i=0;i<count;i++){
        matrix.b._Mul(dst[i],src[i]);
    }
};
/*
FLOAT parameter x, y
 if(~x > 0)
  ret := arctan(~y/~x)
 else
  if (~x == 0)
    ret := pi/2*sgn(~y)
  else
    if (~y == 0)
      ret := pi*ed(-~x)
    else
      ret := arctan(~y/~x) + pi
    endif
  endif
 endif
RETURN ret
*/
double GetAngleByXY(double x,double y)
{
    double f=0.0;
    if (y==0) //защита от деления на 0.
    {
        if (x >= 0)
            f = 0.0;
        else
            f = M_PI;
    }
    else if (y > 0)
    {
        f = -atan ( x / y) + M_PI_2;
    }
    else
    {
        f = -atan ( x / y) + M_PI*1.5;
    }
    /*	double f,dy,dx;
    dx=fabs(x),dy=fabs(y);
  if(x!=0.0)
      f=atan(dy/dx);
    else
  {
      if (y!=0.0)
        f=(y>0.0) ? M_PI_2 : 2*M_PI-M_PI_2 ;
    else
        f=0.0;
      return f;
    }
  if (y>=0.0)
        f=(x>0.0)? f:M_PI-f;
  else
        f=(x>=0.0)? 2*M_PI-f:f+M_PI;*/
    return f;
}

/*
 COLORREF color;
 UINT32 flags;
 char   * file;
 TDib2d * dib;
*/

TMaterial3d::_setup(){
    userdata=0;
    defXtile = defYtile =0;
    _m.color.rgb=RGB(255,255,255);
    _m.flags=0;
    file=NULL;
    _m.name=NULL;
    exttype=NULL;
    _m.bitmap=NULL;
    dib=0;
    _m.emittance.rgb=RGB(255,255,255);
    _m.ambient=_m.diffuse=_m.specular=_m.emittance;
    _m.transparency=_m.shine=0;
};

extern "C" TMATERIAL3D * _EXPORT r3dGetMaterialTexture(PRIMITIVE3D*p)
{
    TPrimitive3d *_p=(TPrimitive3d *)p;
    if(!_p)return NULL;
    TMaterial3d*m=_p->_mat;
    if(m){
        if(m->_m.bitmap)return &(m->_m);
        if((m->_m.flags&TF_EXTERNALDATA) && m->exttype){
            if(!m->userdata)m->userdata=GetUserType(m->exttype);
            if(m->userdata){
                m->_m.bitmap=(LPBITMAPINFOHEADER)m->userdata->objproc((HSP2D)(m->space3d->_HSP3D),(USEROBJSTRUCT*)m->_HANDLE,OM_GETBITMAP,(UINT32)m->file);
            }
        }else{
            if(m->file){
                m->LoadDib();
            }}
        return &(m->_m);
    }
    return NULL;
}

HOBJ2D TSpace3d::CreateMaterial3d(MATERIAL3D *m){
    if (materials->count>16000)return 0;
    TMaterial3d *_m = new TMaterial3d(m,this);
    //_m->space3d=this;
    return materials->InsertObject(_m);
};
BOOL   TSpace3d::DeleteMaterial3d(HOBJ2D hm){
    // return materials->_DeleteObject(hm);
};
BOOL   TSpace3d::UpdateTool(int w,HOBJ2D h,pointer p){
    if(w==9){
        TMaterial3d*_m=(TMaterial3d*)materials->GetByHandle(h);
        if(!_m)return FALSE;
        (pointer)_m->_m.bitmap=p;
        if(_m && !_m->IsZeroRef()){
            for(C_TYPE i=0;i<views->count;i++){
                TView3d_2d*v=(TView3d_2d*)views->At(i);
                //  if(v->camera.camera.flags&)
                v->UpdateCamera(FALSE,TRUE);
            } } }
};
BOOL   TSpace3d::SetMaterial3d(HOBJ2D h,MATERIAL3D *m){
    TMaterial3d*_m=(TMaterial3d*)materials->GetByHandle(h);
    if(_m){
        _m->Set(m);

        if(!_m->IsZeroRef()){
            for(C_TYPE i=0;i<views->count;i++){
                TView3d_2d*v=(TView3d_2d*)views->At(i);
                v->UpdateCamera(TRUE,TRUE);
            }
        };
        return TRUE;
    }
    return FALSE;
};
BOOL  TSpace3d::_StoreObject(PStream ps,HOBJ2D obj){
    if (obj==0)obj=all->current;
    PObject3d po=(PObject3d)GetObjByHANDLE(obj);
    if (po)
    {
        _MkTTosave(materials);
        for(C_TYPE i=0;i<all->count;i++)((PObject3d)all->At(i))->MarkToSave(0);
        po->MarkToSave(1);

        TCollection::fullstore=0;
        HOBJ2D cur=all->current;
        all->current=obj;
        ps->WriteWord(otSPACE3Df);
        ps->WriteWord(currentfileversion);
        ps->WriteWord(currentfileversion);
        ps->Put(this);
        all->current=cur;
        TCollection::fullstore=1;
        return (ps->status==NULL);
    }
};


BOOL   TSpace3d::GetMaterial3d(HOBJ2D h,MATERIAL3D *m){
    TMaterial3d*_m=(TMaterial3d*)materials->GetByHandle(h);
    if(_m){
        m->poSize=sizeof(MATERIAL3D);

        if(m->name){
            if(_m->_m.name){
                lstrcpy(m->name,_m->_m.name);
            }else *(m->name)=0;
        }
        if(m->hfile){
            if(_m->file){
                lstrcpy(m->hfile,_m->file);
            }else *(m->hfile)=0;
        }

        m->flags=_m->_m.flags;
        m->hdib=(_m->dib)?_m->dib->dib:NULL;
        m->defXtile = _m->defXtile;
        m->defYtile = _m->defYtile;

        m->ambient      = _m->_m.ambient;
        m->diffuse      = _m->_m.diffuse;
        m->specular     = _m->_m.specular;
        m->emittance    = _m->_m.emittance;
        m->shine        = _m->_m.shine;
        m->transparency = _m->_m.transparency;
        if(_m->exttype)lstrcpyn(m->exttype,_m->exttype,sizeof(m->exttype));
        if(m->defXtile==0)m->defXtile=1.0;
        if(m->defYtile==0)m->defYtile=1.0;

        return TRUE;
    }
    return FALSE;
};

TMaterial3d::TMaterial3d(){
    _setup();
};
void  TMaterial3d::Set(MATERIAL3D*m){

    _m.flags=m->flags;
    _m.color=m->color;
    defXtile = m->defXtile;
    defYtile = m->defYtile;
    if(userdata && _m.bitmap){
        userdata->objproc((HSP2D)(space3d->_HANDLE),(USEROBJSTRUCT*)(_HANDLE),OM_DESTROYTEXTURE,0);
    }
    userdata=0;
    _m.ambient      = m->ambient;
    _m.diffuse      = m->diffuse;
    _m.specular     = m->specular;
    _m.emittance    = m->emittance;
    _m.shine        = m->shine;
    _m.transparency = m->transparency;
    if(m->flags&TF_EXTERNALDATA){
        if(m->exttype[0]){
            exttype=NewStr(m->exttype);
        }
    }
    if(file){delete file;file=NULL;}
    if(_m.name){delete _m.name;_m.name=NULL;}
    if(m->hfile)file=NewStr(m->hfile);
    if(m->name)_m.name=NewStr(m->name);
    if(dib && ((dib->dib!=m->hdib)||m->hfile)){delete dib;dib=NULL;_m.bitmap=NULL;}

    if (!(m->flags&TF_FILE)){
        if(file)LoadDib();else {
            if((dib==NULL) ||( dib->dib!=m->hdib)) dib=new TDib2d(m->hdib);else
                _m.bitmap=(LPBITMAPINFOHEADER)dib->Info;
        }
    }

};

TMaterial3d::TMaterial3d(MATERIAL3D*m,TSpace3d *sp):TTools2d(){
    space3d=sp;
    _setup();
    Set(m);
};

TMaterial3d::TMaterial3d(PStream ps,LONG _pos):TTools2d(ps,_pos){
    _setup();
    ps->Read(&(_m.flags),4);
    BOOL ok=TRUE;
    do{
        UINT16 c=ps->ReadWord();
        switch(c){
            case 0:ok=0;break;
            case 1:ps->Read(&_m.color,4);break;
            case 2:file=ps->ReadStr();break;
            case 3:{
                dib = new TDib2d(ps,0);_m.bitmap=(LPBITMAPINFOHEADER)dib->Info;
            }break;
            case 4:_m.name=ps->ReadStr();break;
            case 5:ps->Read(&defXtile,sizeof(defXtile));break;
            case 6:ps->Read(&defYtile,sizeof(defYtile));break;
            case 7:{
                ps->Read(&_m.ambient,4);
                ps->Read(&_m.diffuse,4);
                ps->Read(&_m.specular,4);
                ps->Read(&_m.emittance,4);
            }break;
            case 8:{
                ps->Read(&(_m.shine),sizeof(_m.shine));
                ps->Read(&(_m.transparency),sizeof(_m.transparency));
            }break;
            case 9:{
                exttype=ps->ReadStr();
            }break;
            default:{
                CHUNK chunk;
                chunk.InitRead(ps);
                chunk.Check();
            }break;
        }
    }while (ok);

};
TMaterial3d::~TMaterial3d(){
    if(file)delete file;
    if(dib)delete dib;
    if(_m.name)delete _m.name;
    if(userdata && _m.bitmap){
        userdata->objproc((HSP2D)(space3d->_HANDLE),(USEROBJSTRUCT*)_HANDLE,OM_DESTROYTEXTURE,(UINT32)(space3d->_HSP3D));
    }
};
long NColors(WORD bitCount);
inline long ScanBytes(int pixWidth, int bitsPixel) {
    return (((long)pixWidth*bitsPixel+31) / 32) * 4;
}

BOOL TMaterial3d::LoadDib(){
    char *_p=space3d->space2d->txtPath;
    BOOL _try=TRUE;
    char s[256];
    if(_p){
        wsprintf(s,"%s\\%s",_p,file);
        _p=s;
    }else _p=file;
m1:
    //	  TDOSStream st(_p,TDOSStream::stOpenRead);
    dib =ImportRasterImage(_p,0,0);
    if(dib){

        //		dib=new TDib2d(&st,0);
        if(dib->Info){
            if(
                    !((dib->Info->bmiHeader.biBitCount==24)||
                      (dib->Info->bmiHeader.biBitCount== 8))
                    )
            {

                LPBITMAPINFOHEADER pi=(LPBITMAPINFOHEADER)(dib->Info);
                long perline=ScanBytes(pi->biWidth,pi->biBitCount);
                long _perline=ScanBytes(pi->biWidth,24);
                long sizeimage=_perline*pi->biHeight;
                long _size=sizeimage+sizeof(BITMAPINFO);
                long clrsize;
                if(pi->biClrUsed)clrsize=pi->biClrUsed*4; else clrsize= 4*NColors(pi->biBitCount);

                HGLOBAL block=GlobalAlloc(GMEM_MOVEABLE,_size);
                LPBITMAPINFOHEADER info=(LPBITMAPINFOHEADER) GlobalLock(block);
                info->biSize=sizeof(BITMAPINFOHEADER);
                info->biWidth  = pi->biWidth;
                info->biHeight = pi->biHeight;
                info->biPlanes=1;
                info->biBitCount=24;
                info->biCompression=BI_RGB;
                info->biSizeImage=_size;
                info->biXPelsPerMeter=300;
                info->biYPelsPerMeter=300;
                info->biClrUsed=NColors((INT16)24);
                info->biClrImportant=NColors((INT16)24);
                for(int j=0;j<pi->biHeight;j++){
                    BYTE *  ps = (((char*)pi)   + sizeof(BITMAPINFO)) +  perline*j + clrsize;
                    BYTE * _ps = (((char*)info) + sizeof(BITMAPINFO)) + _perline*j;
                    for(int x=0;x<pi->biWidth;x++){
                        RGBQUAD c;
                        switch(pi->biBitCount){
                            case 1:{BYTE _p = ps[x/8];
                                c=dib->Info->bmiColors[(_p&(1<<(7-(x&0x07))))!=0];
                            }break;
                            case 4:{int index = x/2;
                                index = (x&1)?((ps[index])&0x0f):((ps[index])&0xf0);
                                c = dib->Info->bmiColors[index];
                            }break;
                            case 8:c=dib->Info->bmiColors[(int)*ps];ps++;break;
                        }

                        *_ps=c.rgbGreen; _ps++;
                        *_ps=c.rgbRed;   _ps++;
                        *_ps=c.rgbBlue;  _ps++;

                    }
                }

                GlobalUnlock(block);
                delete dib;
                dib =new TDib2d(block);
            }
            _m.bitmap=(LPBITMAPINFOHEADER)(dib->Info);
            if(!(_m.flags&TF_FILE)){
                delete file;file=NULL;
            }
        }else { delete dib;dib=NULL; }
    }else{
        if(space3d->space2d->txtPath){
            if(_try){
                _try=FALSE;
                _p=file;
                goto m1;
            }
        }
    }
};
void TMaterial3d::Store(PStream ps){
    TTools2d::Store(ps);
    ps->Write(&_m.flags,4);
    //  ps->WriteWord(1);
    //  ps->Write(&_m.color,4);

    if(_m.flags&TF_FILE){
        if(file){
            ps->WriteWord(2);
            ps->WriteStr(file);
        }}else{
        if(dib){
            ps->WriteWord(3);
            dib->Store(ps);
        }}

    if(_m.name){
        ps->WriteWord(4);
        ps->WriteStr(_m.name);
    };
    if((defXtile !=0) && (defXtile !=1.0)){
        ps->WriteWord(5);
        ps->Write(&(defXtile),sizeof(defXtile));
    }
    if((defYtile !=0) && (defYtile !=1.0)){
        ps->WriteWord(6);
        ps->Write(&(defYtile),sizeof(defYtile));
    }

    ps->WriteWord(7);
    ps->Write(&_m.ambient,4);
    // ps->Write(&_m.color,4);
    ps->Write(&_m.diffuse,4);
    ps->Write(&_m.specular,4);
    ps->Write(&_m.emittance,4);

    if(_m.shine ||  _m.transparency){
        ps->WriteWord(8);
        ps->Write(&(_m.shine),sizeof(_m.shine));
        ps->Write(&(_m.transparency),sizeof(_m.transparency));
    }
    if(exttype){
        ps->WriteWord(9);
        ps->WriteStr(exttype);
    }

    ps->WriteWord(0);
};


INT16 TSpace3d::DeleteEmptyObjects(void * _m_)
{C_TYPE h=0,i=0;
    PObject3d po;
    for (i=0;i<primary->count;i++)if (((PObject3d)primary->At(i))->IsEmpty())h++;
    if (h){
        if (_m_)
        {
            /*
     PSpace2d _m = (PSpace2d) _m_;
      _m->NewMetaRecord3d(mfdeleteemptyobjects3d,_HANDLE);
      _m->NewUndoRecord3d(mfstartofgroup,_HANDLE);
      for (i=0;i<primary->count;i++){
      po=(PObject3d)primary->At(i);
     if (po->IsEmpty())
         { _m->meta->WriteWord(mfloadobject3d);
            _m->meta->Write(&_HANDLE,1);
            _m->__WriteObject(_m->meta,po);
         }
      }
      _m->meta->WriteWord(mfendofgroup);
      _m->CloseMetaRecord();
*/
        }
        i=0;
        while(i<primary->count)
        {po=(PObject3d)primary->At(i);
            if (po->IsEmpty())
            { primary->AtDelete(i);
                delete po;
                h++;
            }else i++;
        }
    }
    return h;
};
void TSpace3d::_SetModify(){space2d->modifyed=TRUE;};


int TSpace3d::DeleteObject3d(HOBJ2D Object3d)
{PObject3d po=(PObject3d)GetObjByHANDLE(Object3d);
    if (po)
    { UpdateObject(po,UOF_BUFFER);
        _DeleteObject3d(po);
        if ((space2d->Frame3d)&&(IsObjectInObject(po,space2d->Frame3d->object)))
            space2d->DeleteEditFrame3d();
        return _DeleteObject(Object3d);
    }
    return 0;
};

