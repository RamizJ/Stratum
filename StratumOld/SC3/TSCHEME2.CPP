/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
            tscheme.cpp
Author:
            Alexander Shelemekhov
*/
#include "_tscheme.h"
#include <string.h>
#include "dllman.h"
#include "systab.h"
#include "hyper.h"
#include "password.h"

const WUVnewtexture=1;
const WUVchkC=2;
const WUVflipX=4;
const WUVflipY=8;
struct WUVMAP{
    HSP3D  hsp3d;
    HOBJ2D object3d,hframe;
    int    mapmode;
    double th,tv;
    double tminc,tmaxc;
    HOBJ2D texture;
    UINT32 flags;
    WUVMAP();
    WUVMAP(HSP3D,HOBJ2D,HOBJ2D,HOBJ2D);
};
BOOL GetFaceABCD(POINT3D *points,INT16* nums,INT16 count,double&a,double&b,double&c,double&d){
    POINT3D t[MAX_IN_PRIMITIVE];
    for(int j=0;j<count;j++){t[j]=points[nums[j]];}
    double r;

    double _v1=t[1].x-t[0].x;
    double _v2=t[1].y-t[0].y;
    double _w1=t[2].x-t[0].x;
    double _w2=t[2].y-t[0].y;
    double c1=t[1].z-t[2].z;
    double c2=t[0].z-t[2].z;
    double c3=t[0].z-t[1].z;

    c=_v1*_w2-_v2*_w1;
    a=(t[0].y*(c1) - t[1].y*(c2) + t[2].y*(c3));
    b=-(t[0].x*(c1)- t[1].x*(c2) + t[2].x*(c3));
    r=sqrt(a*a+b*b+c*c);

    if(r>0.0){
        a/=r;b/=r;c/=r;
        return TRUE;
    }
    return FALSE;
}

WUVMAP::WUVMAP(){
    memset(this,0,sizeof(WUVMAP));
};
WUVMAP::WUVMAP(HSP3D _hsp3d,HOBJ2D ht,HOBJ2D ho,HOBJ2D hf){
    memset(this,0,sizeof(WUVMAP));
    object3d=ho;
    hframe=hf;
    hsp3d   = _hsp3d;
    texture = ht;
    if(!SCGetScVarD("txtmap_pl_minC",tminc))tminc=-1;else flags|=WUVchkC;
    if(!SCGetScVarD("txtmap_pl_maxC",tmaxc))tmaxc=1;else   flags|=WUVchkC;
    if(!SCGetScVarD("txtmap_tilex",th))th=-1;
    if(!SCGetScVarD("txtmap_tiley",tv))tv=-1;

    int m=SCGetScVar("txtmap_type",0);
    if(m&256)flags |= WUVflipX;
    if(m&512)flags |= WUVflipY;

    if(texture){flags|=WUVnewtexture;
        if(th==-1 || tv==-1){
            MATERIAL3D m;
            memset(&m,0,sizeof(m));
            m.poSize=sizeof(m);
            char s[256];
            char s2[256];
            m.hfile=s;
            m.name=s2;
            if(!GetMaterial3d((HSP3D)hsp3d,texture,&m))return;
            if(th==-1)th=m.defXtile;
            if(tv==-1)tv=m.defYtile;
        }
    }
};

BOOL MakeTextureMap(WUVMAP&wuv){
    BOOL rez=FALSE;
    HSP3D  hsp3d=wuv.hsp3d;
    HOBJ2D object3d=wuv.object3d;

    if(wuv.mapmode==1){
        PushCrdSystem3d(hsp3d);
        CRDSYSTEM crd;
        crd.type=LOCAL_COORDINATE;
        crd.hObject=wuv.hframe;
        SetCrdSystem3d(hsp3d,&crd);
        double mat[16];
        HOBJ2D tmp=0;
        if(wuv.hframe==object3d){
            GetMatrix3d(hsp3d,wuv.hframe,mat);
            wuv.hframe=tmp=CreateObject3d(hsp3d);
            SetMatrix3d(hsp3d,tmp,mat);
            POINT3D o3,s3;
            GetObjectOrgSize3d(hsp3d,object3d,&o3,&s3);
            o3.x+=s3.x/2;o3.y+=s3.y/2;o3.z+=s3.z/2;

            PushCrdSystem3d(hsp3d);
            CRDSYSTEM crd;
            crd.type=LOCAL_COORDINATE;
            crd.hObject=object3d;
            SetCrdSystem3d(hsp3d,&crd);
            SetObjectBase3d(hsp3d,tmp,&o3);
            PopCrdSystem3d(hsp3d);

        }
        if(GetMatrix3d(hsp3d,wuv.hframe,mat)){

            rez=TRUE;
            wuv.mapmode=0;
            wuv.tmaxc=1;wuv.tminc=0;
            ROTATOR3D r={{0,0,0},{1,0,0},M_PI};
            wuv.flags|=WUVchkC;
            for(int i=0;i<4;i++){
                rez&=MakeTextureMap(wuv);
                RotateObject3d(hsp3d,wuv.hframe,&r);
                if(i==1){
                    r.Angle=M_PI_2;
                    wuv.tminc=cos(45.0*M_PI/180.0);
                    RotateObject3d(hsp3d,wuv.hframe,&r);
                    r.Angle=M_PI;
                }
            };

            r.Angle=M_PI_2;
            r.VctSize.y=1;r.VctSize.x=0;
            RotateObject3d(hsp3d,wuv.hframe,&r);
            MakeTextureMap(wuv);
            r.Angle=M_PI;
            RotateObject3d(hsp3d,wuv.hframe,&r);
            MakeTextureMap(wuv);

            SetMatrix3d(hsp3d,wuv.hframe,mat);
        }
        if(tmp)DeleteObject2d((HSP2D)hsp3d,tmp);
        PopCrdSystem3d(hsp3d);
        return rez;
    }
    INT16 c=GetNumPoints3d(hsp3d,object3d);
    INT16 pc=GetNumPrimitives3d(hsp3d,object3d);
    if(!(c && pc))return FALSE;

    POINT3D * points = new POINT3D[c];
    PushCrdSystem3d(hsp3d);
    CRDSYSTEM crd;
    crd.type=LOCAL_COORDINATE;
    crd.hObject=object3d;
    SetCrdSystem3d(hsp3d,&crd);
    if(!GetPoints3d(hsp3d,object3d,points,0,c))return FALSE;
    if(!LP3dtoLP3d(hsp3d,points,points,c,LP3D_CURRENT,LP3D_WORLD))return FALSE;
    crd.hObject=wuv.hframe;
    SetCrdSystem3d(hsp3d,&crd);
    if(!LP3dtoLP3d(hsp3d,points,points,c,LP3D_WORLD,LP3D_CURRENT))return FALSE;

    POINT3D origin,size;
    {
        POINT3D mx;
        POINT3D*pp=points;
        origin.x=pp->x;origin.y=pp->y;origin.z=pp->z;
        mx=origin;
        for(C_TYPE i=1;i<c;i++)
        {pp=points+i;
            if(pp->x<origin.x){origin.x=pp->x;}else{ if(pp->x>mx.x)mx.x=pp->x;}
            if(pp->y<origin.y){origin.y=pp->y;}else {if(pp->y>mx.y)mx.y=pp->y;}
            if(pp->z<origin.z){origin.z=pp->z;}else {if(pp->z>mx.z)mx.z=pp->z;}
        }
        size.x=mx.x-origin.x;
        size.y=mx.y-origin.y;
        size.z=mx.z-origin.z;
    }
    for(C_TYPE i=0;i<pc;i++){

        INT16 nums[MAX_IN_PRIMITIVE];
        PRIMITIVE3D p;
        POINT2D txt[MAX_IN_PRIMITIVE];
        if(GetPrimitives3d(wuv.hsp3d,wuv.object3d,&p,i,1,nums,sizeof(nums),0,0)){

            if(p.flags&PRIMITIVE_POLYGON){
                BOOL settexture=TRUE;
                switch(wuv.mapmode){
                    // planar
                    case 0:{
                        rez=TRUE;
                        BOOL ok=FALSE;
                        if(wuv.flags&WUVchkC){
                            double a,b,c,d;
                            if(GetFaceABCD(points,nums,p.count,a,b,c,d)){
                                settexture=ok=((c >= wuv.tminc)&&(wuv.tmaxc >=c));
                            }}else ok=TRUE;
                        if(ok){
                            for(int j=0;j<p.count;j++){
                                POINT3D * _p=points+nums[j];
                                txt[j].x=1-(_p->x-origin.x)/size.x;
                                txt[j].y=1-(_p->y-origin.y)/size.y;
                            }}
                    }break;
                        // end of planar
                        // cylinrical
                    case 2:{
                        rez=TRUE;
                        for(int j=0;j<p.count;j++){
                            POINT3D * _p=points+nums[j];

                            double a = GetAngleByXY2d(_p->x,_p->y);
                            if(a<0){ a+=2*M_PI;   }
                            txt[j].x=a;
                            txt[j].y=1-(_p->z-origin.z)/size.z;
                        }
                        BYTE f=0;
                        for(j=0;j<p.count;j++){
                            if(txt[j].x>(M_PI*3/2))f|=1;
                            if(txt[j].x<(M_PI/2))f|=2;
                        }
                        if((f&3)==3){
                            for(j=0;j<p.count;j++){
                                if (txt[j].x<(M_PI/2))txt[j].x+=2*M_PI;
                            }
                        }
                        for(j=0;j<p.count;j++){
                            txt[j].x=(txt[j].x/(2*M_PI));
                        }

                    }break;
                        // end cylinder
                        // sphere
                    case 3:{ // BOX
                        rez=TRUE;
                        for(int j=0;j<p.count;j++){
                            POINT3D * _p=points+nums[j];
                            double r=sqrt(_p->x*_p->x+_p->y*_p->y);
                            double a = GetAngleByXY2d(_p->x,_p->y);
                            if(a<0){
                                a+=2*M_PI;
                            }
                            double b = GetAngleByXY2d(_p->z,r);
                            txt[j].x=a;
                            txt[j].y=b/(M_PI);
                        }
                        BYTE f=0;
                        for(j=0;j<p.count;j++){
                            if(txt[j].x>(M_PI*3/2))f|=1;
                            if(txt[j].x<(M_PI/2))f|=2;
                        }
                        if((f&3)==3){
                            for(j=0;j<p.count;j++){
                                if (txt[j].x<(M_PI/2))txt[j].x+=2*M_PI;
                            }
                        }
                        for(j=0;j<p.count;j++){
                            txt[j].x=(txt[j].x/(2*M_PI));
                        }
                    }break;
                        // end sphere
                }; // switch
                if(settexture){
                    for(int j=0;j<p.count;j++){
                        if(wuv.flags&WUVflipX){
                            txt[j].x=1-txt[j].x;
                        }
                        if(wuv.flags&WUVflipY){
                            txt[j].y=1-txt[j].y;
                        }
                        txt[j].x*=wuv.th;
                        txt[j].y*=wuv.tv;
                    }


                    p.texturePOINTS=txt;
                    if(wuv.flags&WUVnewtexture)p.material=0;
                    if(p.material==0)p.material=wuv.texture;
                    p.flags|=PRIMITIVE_TEXTURED;
                    rez&=SetPrimitives3di(hsp3d,object3d,&p,&i,1);
                }
            }}
    }

    PopCrdSystem3d(hsp3d);
    delete points;
    return TRUE;
};
BOOL  ApplyTexture(HSP3D hsp3d,HOBJ2D object3d,HOBJ2D hframe,HOBJ2D texture,int flags,double th,double tv,double tminc,double tmaxc){
    if(flags&128)return ReplaceTexture3d(hsp3d,object3d,0,texture);
    WUVMAP w;
    w.hsp3d    = hsp3d;
    w.object3d = object3d;
    w.hframe   = hframe;
    w.texture  = texture;
    if(flags&256)w.flags|=WUVflipX;
    if(flags&512)w.flags|=WUVflipX;
    w.th=th<=0?1:th;w.tv=tv<=0?1:tv;
    w.tminc=tminc,w.tmaxc=tmaxc;
    w.mapmode=flags&3;
    w.tminc=tminc;
    w.tmaxc=tmaxc;
    if(w.tminc>-1)w.flags|=WUVchkC;
    if(w.tmaxc<1)w.flags|=WUVchkC;
    return MakeTextureMap(w);
};

BOOL  RemoveTexture3d(HSP3D hsp3d,HOBJ2D object3d){
    INT16 nums[MAX_IN_PRIMITIVE];
    PRIMITIVE3D p;
    INT16 pc=GetNumPrimitives3d(hsp3d,object3d);
    BOOL rez=FALSE;
    for(C_TYPE i=0;i<pc;i++){
        if(GetPrimitives3d(hsp3d,object3d,&p,i,1,nums,sizeof(nums),0,0)){
            if((p.flags&(PRIMITIVE_POLYGON|PRIMITIVE_TEXTURED))==
               (PRIMITIVE_POLYGON|PRIMITIVE_TEXTURED)){
                p.flags&=~PRIMITIVE_TEXTURED;
                p.material=0;
                rez|=SetPrimitives3di(hsp3d,object3d,&p,&i,1);
            }
        }
    }
    return rez;
}
BOOL  ReplaceTexture3d(HSP3D hsp3d,HOBJ2D object3d,HOBJ2D hold,HOBJ2D hnew){
    INT16 nums[MAX_IN_PRIMITIVE];
    POINT2D txt[MAX_IN_PRIMITIVE];
    PRIMITIVE3D p;
    INT16 pc=GetNumPrimitives3d(hsp3d,object3d);
    BOOL rez=FALSE;
    for(C_TYPE i=0;i<pc;i++){
        if(GetPrimitives3d(hsp3d,object3d,&p,i,1,nums,MAX_IN_PRIMITIVE,txt,MAX_IN_PRIMITIVE)){
            if((p.flags&(PRIMITIVE_POLYGON))==
               (PRIMITIVE_POLYGON)){
                if(hold==0 || (p.material==hold)){
                    p.material=hnew;
                    rez|=SetPrimitives3di(hsp3d,object3d,&p,&i,1);
                }
            }
        }
    }
    return rez;
}


#ifndef RUNTIME
void TSchemeWindow::Repair(int code)
{
m1:
    for(INT16 i=0;i<_class->childcount;i++){
        HOBJ2D h=_class->childs[i].handle;
        for(INT16 j=i+1;j<_class->childcount;j++){
            if(h==_class->childs[j].handle){
                INT16 cc=_class->childcount;
                RemoveChildren(_class,h,1,1);
                RemoveChildren(_class,h,1,1);
                if(_class->childcount<cc)goto m1;
            }
        }
    }
};

// автоматическое создание связей
void AddActiveLink(TCollection*items,PClass _class,HOBJ2D obj){
    INT16 id=_class->GetChildById(obj);
    if(id>-1){
        if( _class->childs[id].type->GetClass()->flags&CF_AUTOLINK){
            items->Insert(_class->childs+id);
        }
    }
}
BOOL isRectEmpty(RECT2D r){return (r.left>=r.right)||(r.top>=r.bottom);}
BOOL RectIntersect(RECT2D r,RECT2D r1)
{ RECT t;
    if (isRectEmpty (r))return 0;
    if (isRectEmpty (r1))return 0;
    t.left=max(r.left,r1.left);
    t.top=max(r.top,r1.top);
    t.right=min(r.right,r1.right);
    t.bottom=min(r.bottom,r1.bottom);
    return  (t.left<t.right)&&(t.top<t.bottom);
};
void MakeRect(RECT2D&r,POINT2D&org,POINT2D&size){
    r.left=org.x;r.top=org.y;
    r.right=org.x+size.x;r.bottom=org.y+size.y;
};

struct TLINKOBJ2D{
    RECT2D rect;
    HOBJ2D handle;
};

void TSchemeWindow::GetGroupLinkItems(HOBJ2D h,TCollection**p){
    if(GetObjectData2d(hsp,h,UD_VARS,NULL,0,0)){
        if((*p)==NULL)*p=new TCollection(10,10);
        TLINKOBJ2D*lo=new TLINKOBJ2D;
        (*p)->Insert(lo);
        lo->handle=h;
        POINT2D org,size;
        GetObjectOrgSize2d(hsp,h,&org,&size);
        MakeRect(lo->rect,org,size);
    }else{
        INT16 count=GetGroupItemsNum2d(hsp,h);
        for(INT16 i=0;i<count;i++){
            GetGroupLinkItems(GetGroupItem2d(hsp,h,i),p);
        }
    }
};
void  TSchemeWindow::AfterMove(){
#ifndef DEMO
    if(!_class)return; // Если нет класса
    if(!(object || __objects))return;
    TCollection * items =new TCollection (10,10);
    if(object){
        AddActiveLink(items,_class,object);
    }else{
        INT16 count=GetGroupItemsNum2d(hsp,__objects);
        for(INT16 i=0;i<count;i++){
            AddActiveLink(items,_class,GetGroupItem2d(hsp,__objects,i));
        }
    }

    RECT2D r; // обрамляющий прямоугольник
    for(C_TYPE i=0; i<items->count ;i++)
    {PChildInfo pci=(PChildInfo)items->At(i);
        POINT2D org,size;
        if(GetObjectOrgSize2d(hsp,pci->handle,&org,&size)){
            if(i){
                if(r.left>org.x)r.left=org.x;
                if(r.top>org.y)r.top=org.y;
                if(r.right<(org.x+size.x))r.right=(org.x+size.x);
                if(r.bottom<(org.y+size.y))r.bottom=(org.y+size.y);
            }else MakeRect(r,org,size);
        }
    }

    if(items->count){
        TCollection * pitems= new TCollection (10,10);

        for(C_TYPE i=0;i<_class->childcount;i++)
        {
            PChildInfo pci=_class->childs+i;
            if(pci->type->GetClass()->flags&CF_AUTOLINK){
                BOOL found=FALSE;
                for(C_TYPE j=0; j<items->count ;j++)
                {
                    if(items->At(j)==pci){found=TRUE;break;}
                }
                if(!found)pitems->Insert(pci);
            }
        }
        // сейчас можно проверить на выбывание

        for(C_TYPE j=0;j<pitems->count;j++){// цикл по всем объектам
            TCollection * i1=NULL;
            PChildInfo pci=PChildInfo(pitems->At(j));                         //pci->handle
            POINT2D org,size;
            GetObjectOrgSize2d(hsp,pci->handle,&org,&size);
            RECT2D r2;
            MakeRect(r2,org,size);
            if(RectIntersect(r,r2)){ // попадает в общий прямоугольник
                for(C_TYPE i=0;i<items->count;i++){ // цикл по перемещаемым объектам
                    TCollection * i2=NULL;
                    PChildInfo pc=PChildInfo(items->At(i));                            //pc->handle
                    if(!_class->GetLinkByHH(pc->handle,pci->handle)){
                        // Эти имиджи еще не связанны и можно попробовать их связать
                        GetObjectOrgSize2d(hsp,pc->handle,&org,&size);
                        RECT2D r1;
                        MakeRect(r1,org,size);
                        if(RectIntersect(r1,r2)){
                            // их прямоугольные оболочки пересекаются (pci->handle)

                            if(!i1)GetGroupLinkItems(pci->handle,&i1);
                            if(!i2)GetGroupLinkItems(pc->handle,&i2);
                            if(i1 && i2 && i1->count && i2->count){
                                for(C_TYPE k1=0;k1<i1->count;k1++){
                                    TLINKOBJ2D* pl1=(TLINKOBJ2D*)i1->At(k1);
                                    for(C_TYPE k2=0;k2<i2->count;k2++){
                                        TLINKOBJ2D* pl2=(TLINKOBJ2D*)i2->At(k2);
                                        if(RectIntersect(pl1->rect,pl2->rect)){
                                            PLinkInfo pl=new TLinkInfo;
                                            memset(pl,0,sizeof(TLinkInfo));
                                            pl->obj1Id=pci->handle;
                                            pl->obj2Id=pc->handle;

                                            pl->flags=LF_AUTOLINK;

                                            POINT2D pp[2];
                                            pp[1].x=(pl1->rect.left+pl1->rect.right)/2;
                                            pp[1].y=(pl1->rect.bottom+pl1->rect.top)/2;
                                            pp[0].x=(pl2->rect.left+pl2->rect.right)/2;
                                            pp[0].y=(pl2->rect.bottom+pl2->rect.top)/2;

                                            HOBJ2D hpen=CreatePen2d(hsp,PS_SOLID,0 ,RGB(0,0,255), R2_COPYPEN);
                                            pl->handle=CreatePolyline2d(hsp,hpen,0 ,pp,2);
                                            NewLinkCheck(pl,pl1->handle,pl2->handle);
                                            if((pl->count==0) || (!AddLink(_class,pl)))
                                            {
                                                FullDeleteObject2d(hsp,pl->handle);
                                            };
                                            delete pl;
                                        }
                                    }
                                }

                            }
                        }
                    }
                    if(i2){
                        for(C_TYPE k=0;k<i2->count;k++){ delete (i2->At(k)); }
                        DeleteCollection(i2);
                    }
                }
            } // попадает в общий прямоугольник
            if(i1){
                for(C_TYPE k=0;k<i1->count;k++){ delete (i1->At(k)); }
                DeleteCollection(i1);
            }
        }// цикл по всем объектам

        delete pitems;
        // _Message("CHECK for move");
    }else{
        // _Message("CHECK for move: None");
    }

    delete items;
#endif
};

void  TSchemeWindow::RenderFormat(UINT uFormat){
    DWORD e;
    HWND hwnd=GetClipboardOwner();
    if(IsClipboardFormatAvailable(CF_2D)){
        HANDLE hg=GetClipboardData(CF_2D);
        if(hg){
            char cp[260];GetTexturePath2d(hsp,cp);
            HSP2D __hsp=LdFromMemory2d(0,hg,cp);
            if(__hsp){
                HOBJ2D obj=GetCurrentObject2d(__hsp);
                INT16 t=GetObjectType2d(__hsp,obj);

                if(uFormat==CF_DIB){
                    if(t==otBITMAP2D){
                        CopyDib(GetDib2d(hsp,GetDibObject2d(hsp,object2d)),FALSE);
                    }
                }

                if(uFormat==CF_TEXT || uFormat==CF_RTF){

                    if(t==otTEXT2D){
                        HOBJ2D text=GetTextObject2d(__hsp,obj);
                        if(uFormat==CF_RTF){
                            TMemoryStream *ms=new TMemoryStream(0);
                            if(WriteRTF(__hsp,text,ms)){
                                ms->Truncate();
                                SetClipboardData(CF_RTF,ms->GetHANDLE());
                            }
                            delete ms;
                        }
                        if(uFormat==CF_TEXT){
                            TMemoryStream *ms=new TMemoryStream(0);
                            HOBJ2D htm=GetTextObject2d(hsp,text);
                            INT16 count=GetTextNumItems2d(hsp,htm);
                            LOGTEXT *lt=new LOGTEXT[count];
                            GetLogText2d(hsp,htm,lt,count);
                            for(int i=0;i<count;i++){
                                int l=GetLogString2d(hsp,lt[i].ltStringIndex,0,0);
                                char *str=new char[l+1];
                                GetLogString2d(hsp,lt[i].ltStringIndex,str,l+1);
                                ms->Write(str,l);
                                delete str;
                            }
                            delete lt;
                            ms->Truncate();
                            SetClipboardData(CF_TEXT,ms->GetHANDLE());
                            delete ms;
                        }

                    }
                }
            }}else{
#ifdef WIN32
            e= GetLastError();
#endif
        }
    }//IsClipboardFormatAvailable
};

BOOL  TSchemeWindow::GrpPressProc(HOBJ2D obj2d,HOBJ2D primary)
{
    if(IsReserved(obj2d))return FALSE;
    if(obj2d==0)return TRUE;
    if(obj2d==object)return TRUE;
    if(obj2d==object2d)return TRUE;

    INT16 type=0;
    if(obj2d == __frame || obj2d == __objects)
        type=1;

    if(!type)
        type=WhatObject(obj2d);

    if(object==0 && __frame==0 && __objects2d==0 && object2d==0)
        return FALSE;

    switch(type)
    {
        case 3:return FALSE;
        case 0: // 2d Object
        {
            if(__objects2d)
            {
                INT16 id=IsGroupContainObject2d(hsp,__objects2d,primary);
                if(id>0)
                {
                    id--;
                    DelGroupItem2d(hsp,__objects2d,GetGroupItem2d(hsp,__objects2d,id));
                    if(GetGroupItemsNum2d(hsp,__objects2d)==1)
                    {
                        FullDeleteObject2d(hsp,__objects2d);
                        __objects2d=0;
                    }
                }else{
                    AddGroupItem2d(hsp,__objects2d,obj2d);
                }
                CreateGrpFrame2d(); // Перестраиваем
            }else{
                __objects2d=CreateGroup2d(hsp,&obj2d,1);
                SetObjectName2d(hsp,__objects2d,"@__objects2d@");
                if(object2d){AddGroupItem2d(hsp,__objects2d,object2d);object2d=0;}
                CreateGrpFrame2d();
            }
        }break;
        case 1:
        case 2: // Imaje or var place
        {
            if(object && object!=obj2d){
                CreateGroupBy1Element(object);
                DeleteEditFrame2d(hsp);
                object=0;
            }
            if(__frame){
                INT16 fc=GetGroupItemsNum2d(hsp,__frame);
                if (((obj2d==__objects || obj2d==__frame)) && (fc>0)){
                    // Удаляем
                    INT16 id=0;
                    id=IsGroupContainObject2d(hsp,obj2d==__objects?__objects:__frame,primary);
                    if(id>0){
                        id--;
                        DelGroupItem2d(hsp,__objects,GetGroupItem2d(hsp,__objects,id));
                        HOBJ2D fi=GetGroupItem2d(hsp,__frame,id);
                        DelGroupItem2d(hsp,__frame,fi);
                        FullDeleteObject2d(hsp,fi);
                        if(fc==1){
                            DeleteObject2d(hsp,__frame);
                            DeleteObject2d(hsp,__objects);
                            __objects=__frame=0;
                        }
                    }
                }else{
                    // Добавляем
                    POINT2D _org,_size;
                    GetObjectOrgSize2d(hsp,obj2d,&_org,&_size);
                    HOBJ2D brush=GetBrushObject2d(hsp,GetGroupItem2d(hsp,__frame,0));
                    CreateGRPFrame(obj2d,brush,_org);
                }
            }else{
                object=obj2d;
                if(object2d){
                    CreateGroupBy1Element(object);
                    object=0;
                }else   CreateImageFrame();
            }
        }break;
    }
    // Пошли делать проверку
    if(__objects2d){
        if(!__frame && (object==0)){
            // Удаляем группу только если нет выбранных имиджей и
            // количество выбранных 2d меньше чем 2
            // Если есть один выбранный 2d объект  то делаем его текущим и переходим в sfbase
            if(GetGroupItemsNum2d(hsp,__objects2d)<3){ // Необходиость грохнуть 2d группу
                object2d=GetGroupItem2d(hsp,__objects2d,0);
                SetState(sfbase);
                SetCurrentObject2d(hsp,object2d);
                CreateEditFrame2d();
            }
        }else{
            if(object){
                CreateGroupBy1Element(object);
                object=0;
                DeleteEditFrame2d(hsp);
            }}
    }else{

        INT16 fc=GetGroupItemsNum2d(hsp,__frame);
        if(object2d && fc){
            __objects2d=CreateGroup2d(hsp,&object2d,1);
            SetObjectName2d(hsp,__objects2d,"@__objects2d@");
            object2d=0;
            CreateGrpFrame2d();
            DeleteEditFrame2d(hsp);
            fc=3;
        }
        // Проверяем на необходимость удалить группу
        if(fc<2){
            object=GetGroupItem2d(hsp,__objects,0);
            DeleteFrames();
            SetState(sfbase);
            CreateImageFrame();
        }
    }
    if(__objects || __objects2d){
        if(state!=sfrectbase){SetState(sfrectbase);
            DeleteEditFrame2d(hsp);
        }
    }else{
        if(state!=sfbase)SetState(sfrectbase);
    }
    return TRUE;
}
void TSchemeWindow::CreateGroupBy1Element(HOBJ2D obj)
{
    HOBJ2D brush=CreateBrush2d(hsp,BS_PATTERN,0,0,
                               CreateDib2dFromResource(hsp,HResource,MAKEINTRESOURCE(BM_MASKblue)),
                               R2_MASKPEN);

    __frame = CreateGroup2d(hsp, NULL,0);
    SetObjectName2d(hsp,__frame,"@__frame@");

    __objects=CreateGroup2d(hsp,&obj,1);
    SetObjectName2d(hsp,__objects,"@__objects@");

    POINT2D _org,_size;
    GetObjectOrgSize2d(hsp,object,&_org,&_size);
    CreateGRPFrame(object,brush,_org);
};

void __AddOrg2d(HSP2D hsp,HOBJ2D h,POINT2D&p,BOOL&set){
    if(h){
        POINT2D org;
        org.x=org.y=0;
        if(GetObjectOrgSize2d(hsp,h,&org,NULL)){
            if(set){
                if(org.x<p.x)p.x=org.x;
                if(org.y<p.y)p.y=org.y;
            }else{
                set=TRUE;
                p=org;
            }
        }else{
            _Message("gwin failed 001");
        }
    }
}
void TSchemeWindow::CreateGrpFrame2d(){
    if(__objects2d){
        TRestoreCrd crd(hsp);
        HOBJ2D  h= GetObjectByName2d(hsp,__objects2d,"$@#_frames2d_$@#");
        if(h)FullDeleteObject2d(hsp,h);

        HOBJ2D dib=CreateBrush2d(hsp,BS_PATTERN,0,0,
                                 CreateDib2dFromResource(hsp,HResource,MAKEINTRESOURCE(BM_MASKblue)),R2_MASKPEN); //BM_MASKgreen

        INT16 count=GetGroupItemsNum2d(hsp,__objects2d);
        HOBJ2D *items=new HOBJ2D[count];
        GetGroupItems2d(hsp,__objects2d,items,count);
        for(INT16 i=0;i<count;i++){

            POINT2D point[5];
            POINT2D _size,_org;
            GetObjectOrgSize2d(hsp,items[i],&_org,&_size);
            for(INT16 j=0;j<5;j++)point[j]=_org;
            point[1].x+=_size.x;point[2].x+=_size.x;
            point[2].y+=_size.y;point[3].y+=_size.y;
            items[i]=CreatePolyline2d(hsp,0,dib , point,5) ;
        }
        h=CreateGroup2d(hsp,items,count);
        SetObjectName2d(hsp,h,"$@#_frames2d_$@#");
        AddGroupItem2d(hsp,__objects2d,h);
        SetAttribute2d(hsp,h,STATE_HARDDONTSELECT|STATE_DONTSELECT,ATTRSET);
        delete items;
    }
};
void   TSchemeWindow::UnTrackLine(POINT2D&point2d,BOOL fromup){

    SetCursor(0,IDC_ARROW);
    ReleaseCapture();
    //SetAttribute2d(hsp,link,1,ATTRSET);
    HideObject2d(hsp,link);// Чтобы не выбрать линк вместо объекта
    HOBJ2D obj2d=GetObjectFromPoint2d (hsp,&point2d);
    ShowObject2d(hsp,link);
    BOOL okey=0;

    if (obj2d && object && (obj2d!=object)){
        int t1=WhatObject(obj2d),t2=WhatObject(object);

        if ((t1==1 || t1==2)&&(t2==1 || t2==2))okey=1;
    }
    if (okey)
    {PLinkInfo pl=new TLinkInfo;
        pl->varhandle=0;
        if (_class->GetChildById(object)>-1) pl->obj1Id=object;
        else {
            pl->obj1Id=0;
            pl->varhandle=object;
        }

        if (_class->GetChildById(obj2d)>-1) pl->obj2Id=obj2d;
        else {
            pl->obj2Id=0;
            pl->varhandle=obj2d;
        }

        pl->handle=link;
        BOOL clip=0;
        if(state==sfLClipTrack){
            LINKCLIP*lc=(LINKCLIP*)tmpdata;
            clip=TRUE;
            pl->items=lc->plink->items;
            pl->flags=lc->plink->flags;
            pl->count=lc->plink->count;
            SetObjectLayer2d(hsp,link,lc->layer);
            lc->plink->items=NULL;
            lc->plink->count=0;
            //				DoneLinkPaste(); вызывается из SetState
            SetState(sfbase);
            //if(tmpdata)delete tmpdata;
        }else{
            pl->flags=0;
            pl->count=0;
            pl->items=NULL;
            NewLinkCheck(pl);
        }
        if(!AddLink(_class,pl))
        {
            FullDeleteObject2d(hsp,link);
        };
        INT16 num;
        plink=_class->GetLinkById(link,&num);
        SetState(sfbase);
        if(!clip){if(fromup)OpenLinksDialog(link);}else _class->DeleteLink(pl);

        ObjectToBottom2d(hsp,link);
        _class->DeleteLink(pl);
        delete pl;
    }else{
        HOBJ2D pen=GetPenObject2d(hsp,link);
        DeleteObject2d(hsp,link);
        DeleteTool2d(hsp,PEN2D ,pen);
        object=0;
        link=0;
    }
    SwithcToBaseMode();

};
void TSchemeWindow::CreateGroupFromSelection(){
    if(__objects2d){
        HOBJ2D  h= GetObjectByName2d(hsp,__objects2d,"$@#_frames2d_$@#");
        FullDeleteObject2d(hsp,h);
        SetObjectName2d(hsp,__objects2d,"");
        HOBJ2D ho=__objects2d;
        __objects2d=0;
        DeleteFrames(hsp);
        PostCreateObject2d(ho);
    }
};
void TSchemeWindow::UnDrag(POINT2D&point2d)
{
    SetState(sfbase);

    BOOL first=FALSE,last=FALSE,ok=FALSE,rebuild=FALSE;
    if (editpoint){
        if (GetVectorNumPoints2d(hsp,link)==editpoint+1)last=TRUE;
    }else first=TRUE;
    if(first || last){
        HideObject2d(hsp,link);// Чтобы не выбрать линк вместо объекта
        HOBJ2D obj2d=GetObjectFromPoint2d (hsp,&point2d);
        ShowObject2d(hsp,link);
        if(obj2d){
            INT16  type=WhatObject(obj2d);
            if(type==1 || type ==2){
                HOBJ2D h1=plink->obj1Id,h2=plink->obj2Id,vh=plink->varhandle;
                if(type==2){
                    if(first)h1=0;
                    if(last)h2=0;
                    vh=obj2d;
                }else{
                    if(first){if(h1==0)vh=0; h1=obj2d; }
                    if(last){if(h2==0)vh=0; h2=obj2d; }
                }
                ok=((h1 && h2 && (vh==0)) || (h1 && (h2==0) && vh) || ((h1==0) && h2 && vh));
                if(ok){
                    if( (h1 != plink->obj1Id)|| (h2 != plink->obj2Id)){
                        plink->obj1Id=h1;
                        plink->obj2Id=h2;
                        plink->varhandle=vh;
                        ModifyLink(_class,link,plink,TRUE);
                    }else{
                        if(plink->varhandle!=vh){
                            plink->varhandle=vh;
                            _class->SetModify();
                        }
                    }
                }
            }}
        if(!ok)SetVectorPoint2d(hsp,link,editpoint,&lastpoint);
    }
    UpdateScrollers();
    CreateLinkFrame();
}

POINT2D TSchemeWindow::GetRectOrg2d(){
    BOOL set=FALSE;
    POINT2D p;
    //_Message("1");
    __AddOrg2d(hsp,__objects2d,p,set);
    //_Message("2");
    __AddOrg2d(hsp,__objects,p,set);
    //_Message("3");
    __AddOrg2d(hsp,object,p,set);
    return p;
};
void  TSchemeWindow::SetRectOrg2d(POINT2D&p){
    if(object){
        SetObjectOrg2d(hsp,object,&p);
    }else{
        HOBJ2D ho=CreateGroup2d(hsp,NULL,0);
        if(__objects2d)AddGroupItem2d(hsp,ho,__objects2d);
        if(__objects)AddGroupItem2d(hsp,ho,__objects);
        if(__frame)AddGroupItem2d(hsp,ho,__frame);

        SetObjectOrg2d(hsp,ho,&p);
        DeleteGroup2d(hsp,ho);
    }
};

void TSchemeWindow::SelectRectProc(){
    DeleteFrames(hsp);
    TRestoreCrd crd(hsp);
    POINT2D org,size;
    GetObjectOrgSize2d(hsp,OID_FRAME2d,&org,&size);
    if (size.x<0){ org.x+=size.x;size.x=-size.x;}
    if (size.y<0){ org.y+=size.y;size.y=-size.y;}
    __objects2d=CreateRectGroup2d(org,size,FALSE,TRUE);
    if(__objects2d){
        SetObjectName2d(hsp,__objects2d,"@__objects2d@");
        CreateGrpFrame2d();
    }

    if(_class){
        INT16 count=0;
        HOBJ2D dib=0;
        HOBJ2D lasth=0;
        HOBJ2D h=0;
        while((h=GetNextObject2d(hsp,h))!=0){
            INT16 type=WhatObject(h);
            if ((type==1 || type==2)){
                POINT2D _org,_size;
                GetObjectOrgSize2d(hsp,h,&_org,&_size);

                if (_org.x>=org.x && _org.y>=org.y &&
                    ((_org.x+_size.x) <= (org.x+size.x)) &&
                    ((_org.y+_size.y) <= (org.y+size.y))){
                    if (count==0){
                        dib=CreateBrush2d(hsp,BS_PATTERN,0,0,
                                          CreateDib2dFromResource(hsp,HResource,MAKEINTRESOURCE(BM_MASKblue)),R2_MASKPEN);
                        __frame=CreateGroup2d(hsp,NULL,0);
                        SetObjectName2d(hsp,__frame,"@__frame@");
                        __objects=CreateGroup2d(hsp,NULL,0);
                        SetObjectName2d(hsp,__objects,"@__objects@");
                        lasth=h;
                    }
                    CreateGRPFrame(h,dib,_org);
                    count++;
                }
            }}
    }
};

HOBJ2D TSchemeWindow::CreateRectGroup2d(POINT2D &org,POINT2D &size,BOOL rgn,BOOL if1){
    if (size.x<0){ org.x+=size.x;size.x=-size.x; }
    if (size.y<0){ org.y+=size.y;size.y=-size.y; }
    HOBJ2D obj=0,objfirst=0;
    HOBJ2D _obj2d=0;
    while((obj=GetNextObject2d(hsp,obj))!=0){
        int type=GetObjectType2d(hsp,obj);

        if ( (rgn && type==otLINE2D) || !rgn){
            if ((obj!=_obj2d) && (GetObjectParent2d(hsp,obj)==0)){
                if (!WhatObject(obj) && obj!=__objects  && obj!= __frame ){
                    POINT2D _org,_size;
                    GetObjectOrgSize2d(hsp,obj,&_org,&_size);
                    if (_org.x>=org.x && _org.y>=org.y &&
                        ((_org.x+_size.x) <= (org.x+size.x)) &&
                        ((_org.y+_size.y) <= (org.y+size.y))){
                        if (_obj2d){
                            if (rgn){
                                RGNGROUPITEM rg;
                                rg.Object=obj;
                                rg.Rop=RGN_OR;
                                AddRgnGroupItem2d(hsp,_obj2d,(INT16)(GetGroupItemsNum2d(hsp,_obj2d)-1),&rg);
                            }else
                                AddGroupItem2d(hsp,_obj2d,obj);
                        }else
                        {
                            if (objfirst || if1){
                                if (rgn){
                                    RGNGROUPITEM rg[2];
                                    rg[0].Object=objfirst;
                                    rg[0].Rop=RGN_OR;
                                    rg[1].Object=obj;
                                    rg[1].Rop=RGN_OR;
                                    if(if1)    _obj2d=CreateRgnGroup2d(hsp,rg+1,1);
                                    else _obj2d=CreateRgnGroup2d(hsp,rg,2);

                                }
                                else {
                                    HOBJ2D items[2]={objfirst,obj};
                                    if(if1)_obj2d=CreateGroup2d(hsp,items+1,1);
                                    else  _obj2d=CreateGroup2d(hsp,items,2);
                                }

                            }else
                                objfirst=obj;
                        }
                    }   }
            }
        }}
    return  _obj2d;
};

void _AddVarItem(TCollection*items,
                 PClass _class1,
                 char *v1,char*v2){
    if(v1 && v2 && v1[0] && v2[0]){
        INT16 id1=_class1->GetVarIdByName(v1,FALSE);
        if(id1>-1){
            BOOL insert=TRUE;
            PLinkEntryInfo pl=new TLinkEntryInfo;
            pl->name1=NewStr(v1);
            pl->name2=NewStr(v2);
            items->Insert(pl);

        } }
};


void AddVarItems(TCollection*items,PClass _class1,char*s){
    char *ps=s;
    char *vn=s;
    char *lv=NULL;
    while(*ps){
        if(*ps==';'){
            *ps=0;
            _AddVarItem(items,_class1,vn,lv);
            vn=ps+1;}
        if(*ps==','){*ps=0;lv=ps+1;}
        ps++;
    }
    _AddVarItem(items,_class1,vn,lv);
};
void _AddVarItem2(TCollection*items, char *v1,char*v2,
                  PClass _class1=NULL,PClass _class2=NULL)
// добавляет элемент в финальную коллекцию c проверкой на дублирование
{
    if(v1 && v2 && v1[0] && v2[0]){
        for(C_TYPE i=0;i<items->count;i++){
            PLinkEntryInfo pl=(PLinkEntryInfo)items->At(i);
            if((!lstrcmpi(pl->name1,v1)) && (!lstrcmpi(pl->name2,v2)))return;

        }

        if(_class1 && _class1->GetVarIdByName(v1,FALSE)==-1)return;
        if(_class2 && _class2->GetVarIdByName(v2,FALSE)==-1)return;

        PLinkEntryInfo pl=new TLinkEntryInfo;
        pl->name1=NewStr(v1);
        pl->name2=NewStr(v2);
        items->Insert(pl);
    }

};

void _RemoveVarItem(PLinkEntryInfo pl){
    delete pl->name1;
    delete pl->name2;
    delete pl;

};
void RemoveVarItems(TCollection&items){
    if(items.count){
        for(C_TYPE i=0;i<items.count;i++){
            _RemoveVarItem((PLinkEntryInfo)items.At(i));
        }
        items.DeleteAll();
    }
}

void PreAddVars(TCollection&items,TCollection&final,PClass _class1,PClass _class2,BOOL reverse){
    C_TYPE i=0;
    while(i<items.count)
    {
        PLinkEntryInfo pl=(PLinkEntryInfo)items.At(i);
        INT16 id1=(reverse?_class2:_class1)->GetVarIdByName(pl->name1,FALSE);
        INT16 id2=(reverse?_class1:_class2)->GetVarIdByName(pl->name2,FALSE);
        if(id1>-1 && id2>-1){
            if(reverse)_AddVarItem2(&final,pl->name2,pl->name1);else
                _AddVarItem2(&final,pl->name1,pl->name2);
            _RemoveVarItem(pl);
            items.AtDelete(i);
            continue;
        }
        i++;
    }
}

HOBJ2D TSchemeWindow::GetMainLinkGroup(HOBJ2D ho){
    HOBJ2D hm=ho;
    while(ho){
        if(GetObjectData2d(hsp,ho,UD_VARS,NULL,0,0))hm=ho;
        ho=GetObjectParent2d(hsp,ho);
    }
    return hm;
};

void TSchemeWindow::NewLinkCheck(PLinkInfo pl,HOBJ2D h1,HOBJ2D h2){
    HOBJ2D hp1,hp2;
    if(!h1 && !h2){
        HideObject2d(hsp,pl->handle);
        POINT2D p[2];
        GetVectorPoints2d(hsp,pl->handle,p,2);
        h1=GetObjectFromPoint2d (hsp,p,&hp1);
        h2=GetObjectFromPoint2d (hsp,p+1,&hp2);
        ShowObject2d(hsp,pl->handle);
    }else{
        hp1=h1;h1=pl->obj1Id;
        hp2=h2;h2=pl->obj2Id;
    }
    PClass po1,po2;
    po1=NULL;po2=NULL;
    char *s1=NULL,*s2=NULL;
    if(pl->obj1Id){
        if((h1==pl->obj1Id) && hp1){
            if(pl->obj1Id){
                INT16 i=_class->GetChildById(pl->obj1Id);
                if(i>-1){
                    po1=_class->childs[i].type->GetClass();
                    hp1=GetMainLinkGroup(hp1);
                    INT16 _size=GetObjectData2d(hsp,hp1,UD_VARS,NULL,0,0);

                    if(_size){
                        s1=new char[_size+1];
                        s1[_size]=0;
                        GetObjectData2d(hsp,hp1,UD_VARS,s1,0,_size);
                    }
                }}else po1=_class;
        }}else po1=_class;
    if(pl->obj2Id){
        if( (h2==pl->obj2Id) && hp2){
            if(pl->obj2Id){
                INT16 i=_class->GetChildById(pl->obj2Id);
                if(i>-1){
                    po2=_class->childs[i].type->GetClass();
                    hp2=GetMainLinkGroup(hp2);
                    INT16 _size=GetObjectData2d(hsp,hp2,UD_VARS,NULL,0,0);
                    if(_size){
                        s2=new char[_size+1];
                        s2[_size]=0;
                        GetObjectData2d(hsp,hp2,UD_VARS,s2,0,_size);
                    }
                }}else po2=_class;
        }}else  po2=_class;
    if(!(po2 && po1))return;

    if(s1 || s2){
        TCollection items1(10,10);
        TCollection items2(10,10);

        if(s1)AddVarItems(&items1,po1,s1);
        if(s2)AddVarItems(&items2,po2,s2);

        TCollection items(10,10);
        PreAddVars(items1,items,po1,po2,FALSE);
        PreAddVars(items2,items,po2,po1,TRUE);

        if(items1.count && items2.count){
            for(C_TYPE i=0;i<items1.count;i++){
                PLinkEntryInfo pl1=(PLinkEntryInfo)items1.At(i);
                for(C_TYPE j=0;j<items2.count;j++){
                    PLinkEntryInfo pl2=(PLinkEntryInfo)items2.At(j);
                    if(!lstrcmpi(pl1->name2,pl2->name2)){
                        _AddVarItem2(&items,pl1->name1,pl2->name1,po1,po2);
                    }
                }
            }
        };

        RemoveVarItems(items1);
        RemoveVarItems(items2);

        if(items.count){
            pl->count=items.count;
            pl->items=new TLinkEntryInfo[items.count];
            for(C_TYPE i=0;i<items.count;i++){
                pl->items[i]=*((PLinkEntryInfo)(items.At(i)));
                delete items.At(i);
            }
            items.DeleteAll();
        }

        //
    }
    if(s1)delete s1;
    if(s2)delete s2;

};

struct LSTRUCT{
    HOBJ2D oldhandle;
    HOBJ2D handle;
    BOOL   varplace;
};
C_TYPE DefChar(unsigned char ch);
BOOL IsWorldChar(char ch){
    C_TYPE c=DefChar(ch);
    return c==1 || c==2;
}
BOOL _IsStrEqual(char *s1,char *s2,BOOL _substr,BOOL _case,BOOL _whole=0){
    if((s1==NULL) || (s2==NULL))return FALSE;
    if(lstrlen(s1)<1)return FALSE;
    if(_substr){
        if(_case){
            BOOL ok=FALSE;
            char * buf=NewStr(s1);
            AnsiUpper(buf);
            if(_whole){
                char *_ps=buf;
                int l=lstrlen(s2);
                while(1){
                    char *pos=strstr(_ps,s2);
                    if(pos==NULL)break;
                    char *endpos=pos+l;
                    if ((pos==buf || !IsWorldChar(*(pos-1))) && (*(endpos)==NULL || !IsWorldChar(*(endpos) ) ))
                    {
                        ok=TRUE;
                        break;
                    }
                    _ps=pos+1;
                }
            }else{     if(strstr(buf,s2))ok=TRUE;  }
            delete buf;
            return ok;
        }else {
            if(strstr(s1,s2))return TRUE;
        }
    }else{
        if(_case){
            if(!lstrcmpi(s1,s2))return TRUE;
        }else{
            if(!lstrcmp(s1,s2))return TRUE;
        }
    }
    return FALSE;
};
BOOL ClassCompare(PClass _child,PObject po,TChildInfo*ci,char *s){
    UINT32 mode=SCGetScVar("search_opt2",0);
    UINT32 vm=SCGetScVar("search_opt1",0);
    BOOL _case=!((vm&1)!=0);
    BOOL _whole=(vm&2)!=0;
    BOOL f=FALSE;
    if((mode&0x80)&&(ci)){
        int h;
        if(sscanf(s,"%d",&h)){
            if(ci->handle==h){f=TRUE;goto _ok;}
        }
    }
    if((mode&0x01) && (ci)&& (ci->name)){
        if(_IsStrEqual(ci->name,s,!_whole,_case)){f=TRUE;goto _ok;}
    }
    if(mode&0x04){  // Class Name
        if(_IsStrEqual(_child->name,s,!_whole,_case)){f=TRUE;goto _ok;}
    }
    if(mode&0x10){
        for(INT16 j=0;j<_child->varcount;j++){
            TVarInfo*vi=_child->vars+j;
            if(_IsStrEqual(vi->name,s,!_whole,_case)){f=TRUE;goto _ok;}
        }
    };
    if(po && (mode&0x20)){
        for(INT16 j=0;j<_child->varcount;j++){
            char buf[256];
            GetObjectVarA(po,j,buf);
            if(_IsStrEqual(buf,s,!_whole,_case)){f=TRUE;goto _ok;}
        }
    };
    if(mode&0x40){
        for(INT16 j=0;j<_child->varcount;j++){
            TVarInfo*vi=_child->vars+j;
            if(_IsStrEqual(vi->info,s,!_whole,_case)){f=TRUE;goto _ok;}
        }
        char* txt=GetClassInfo(_child);
        if(txt){
            if(_IsStrEqual(txt,s,!_whole,_case)){f=TRUE;}
            delete txt;
            if(f)goto _ok;
        }
    };

    if(mode&0x08){
        char* txt=GetClassText(_child);
        if(txt){
            if(_IsStrEqual(txt,s,1,_case,_whole)){f=TRUE;}
            delete txt;
        }}
_ok:
    return f;
}
void  TSchemeWindow::AddGotoMenu(HMENU m,HOBJ2D obj2d){

    if(obj2d==0){
        if(__objects2d || __objects)return ;
        if(object)obj2d=object;
        if(object2d)obj2d=object2d;
    }
    int _size=GetObjectData2d(hsp,obj2d,UD_HYPERKEY,NULL,0,0);
    if(_size){
        THyperKey key(hsp,obj2d,0);
        if(key.openmode==0 && lstrlen(key.target)){
            char cmd[64];
            LoadString(HResource,IDS_str224,cmd,sizeof(cmd));
            char s[256];
            wsprintf(s,cmd,key.target);
            AppendMenu(m, MF_ENABLED , CM_GOTOSHORTCUT ,s  );
        }
    }
};


BOOL   TSchemeWindow::DoFind(BOOL start){
    char s[256];
    BOOL rezult=FALSE;
    DeleteFrames(hsp);
    DeleteEditFrame2d(hsp);

    UINT32 _mode=SCGetScVar("search_what_tofind",0);
    UINT32 vm=SCGetScVar("search_opt1",0);
    BOOL grp=(vm&SR_MAKEGROUP)!=0;
    BOOL _case=!((vm&SR_CASE)!=0);
    BOOL _whole=(vm&SR_WORDS)!=0;
    UINT32 mode=SCGetScVar("search_opt2",0);
    if(!SCGetScVarS("search_string",s))return FALSE;

    if(_case)AnsiUpper(s);

    HCURSOR hOld=::SetCursor(LoadCursor(0,IDC_WAIT));

    if(_mode==0){
        if(_class){
            HOBJ2D ff=0;
            HOBJ2D dib;
            INT16 i=0;
            if(!start && object)i=_class->GetChildById(object)+1;
            for(;i<_class->childcount;i++){

                TChildInfo * ci=_class->childs+i;
                PClass _child=ci->type->GetClass();
                HOBJ2D h=ci->handle;
                PObject po=NULL;
                if(_object){po=_object->_GetObject(h);}

                if(ClassCompare(_child,po,ci,s)){

                    if(grp){
                        if(ff){
                            POINT2D _org;
                            if(!__objects){
                                dib=CreateBrush2d(hsp,BS_PATTERN,0,0,
                                                  CreateDib2dFromResource(hsp,HResource,MAKEINTRESOURCE(BM_MASKblue)),R2_MASKPEN);
                                __frame=CreateGroup2d(hsp,NULL,0);
                                __objects=CreateGroup2d(hsp,NULL,0);

                                GetObjectOrgSize2d(hsp,ff,&_org,NULL);
                                CreateGRPFrame(ff,dib,_org);
                            }
                            GetObjectOrgSize2d(hsp,h,&_org,NULL);
                            CreateGRPFrame(h,dib,_org);

                        }else {ff=h;}
                    }else{
                        ff=h;
                        break;
                    }
                }
            }
            if(ff && (__objects==NULL)){
                object=ff;
                CreateImageFrame();
                rezult=TRUE;
            }
            if(!ff)rezult= FALSE;else{
                if(object||__objects)rezult= TRUE;
            }
        }}
    if(_mode==1)
    {
        if(_class && _class->linkcount){
            INT16 index=0;
            if(!(start || (link==0)))
            {if(!_class->GetLinkById(link,&index))return FALSE; index++;}


            while(index<_class->linkcount)
            {
                PLinkInfo pl=_class->link+index;
                for(int i=0;i<pl->count;i++)
                {

                    if(mode&0x80){
                        int h;
                        if(sscanf(s,"%d",&h)){
                            if(pl->handle==h){goto lnk1;}
                        }
                    }
                    if(_IsStrEqual(pl->items[i].name1,s,!_whole,_case)||
                       _IsStrEqual(pl->items[i].name2,s,!_whole,_case))
                    {
lnk1:
                        link=pl->handle;
                        CreateLinkFrame();
                        rezult=TRUE;
                        index=_class->linkcount;
                        break;
                    }
                }
                index++;
            }
            if(!rezult){
                DeleteLinkFrame();
                link=0;}
        }}
    if(_mode==2){
        BOOL in3d=FALSE;
        HSP2D _hsp=hsp;
        HOBJ2D ho=object2d;
        HOBJ2D obj=object2d;
        if(object2d && (GetObjectType2d(hsp,object2d)==otVIEW3D2D)){
            in3d=TRUE;_hsp=(HSP2D)hsp3d;obj=object=object3d;
        }


        if(start || (ho==0))ho=0;
        do{
            BOOL f=FALSE;
            ho=GetNextObject2d(_hsp,ho);
            if(!ho)break;
            if(hardmode == SCHEME_EDITOR && (!in3d))
            {HOBJ2D _h=ho,_h_;
                do{
                    _h_=GetObjectParent2d(_hsp,_h);
                    if(_h_)_h=_h_;
                }while (_h_);
                int type=WhatObject(_h);
                if (type!=0)continue;
            };
            if(mode&0x80){
                int h;
                if(sscanf(s,"%d",&h)){
                    if(ho==h){f=TRUE;goto ok_;}
                }
            }

            char _s[256];
            if(GetObjectName2d(_hsp,ho,_s,sizeof(_s))){
                if(_IsStrEqual(_s,s,!_whole,_case)){f=TRUE;goto ok_;}
            }
ok_:
            if(f && ho){
                if(in3d){
                    object3d=ho;
                    SetCurrentObject3d(hsp3d,object3d);
                    CreateEditFrame3d();
                }else{
                    object2d=ho;
                    SetCurrentObject2d(_hsp,object2d);
                    CreateEditFrame2d();
                }
                ho=0;
                rezult=TRUE;
            }
        }while(ho);
    }
    hOld= ::SetCursor(hOld);
    return rezult;
};

typedef struct MY_SMALL_RECT { // srct
    INT16 left;
    INT16 top;
    INT16 right;
    INT16 bottom;
} MYSMALL_RECT;

typedef struct {
    DWORD   key;
    WORD    hmf;
    MYSMALL_RECT bbox;
    WORD    inch;
    DWORD   reserved;
    WORD    checksum;
} PLACABLEMETAFILEHEADER;

BOOL   TSchemeWindow::Cm_PasteWMF(){
    BOOL rez=FALSE;
    if(OpenClipboard(HWindow)){
        HANDLE hg=GetClipboardData(CF_METAFILEPICT);
        METAFILEPICT*mf=(METAFILEPICT*)GlobalLock(hg);

        PLACABLEMETAFILEHEADER h;
        h.key = 0x9AC6CDD7L;
        h.hmf=(UINT16)0;
        h.inch=96;
        h.bbox.left  = 0;
        h.bbox.top   = 0;
        h.bbox.right = mf->xExt/24;
        h.bbox.bottom= mf->yExt/24;
        WORD * w=(WORD*)&h;

        if(h.bbox.right<96)h.bbox.right=96;
        if(h.bbox.bottom<96)h.bbox.bottom=96;


        for(int i=0;i<10;i++)h.checksum^=(w[i]);
        char s[256];{
            TDOSStream st(NULL,TDOSStream::stCreate);
            lstrcpy(s,st.GetFileName());
        };
        HMETAFILE hmf=CopyMetaFile(mf->hMF,s);
        DeleteMetaFile(hmf);
        GlobalUnlock(hg);
        {
            TDOSStream st(s,TDOSStream::stOpen);
            TDOSStream st1(NULL,TDOSStream::stCreate);
            st1.Write(&h,sizeof(h));
            st.CopyTo(&st1,0,st.GetSize());
            HSP2D _hsp=0;
            if(SimplyExchange(_hsp,st1.GetFileName(),"wmf",0)){
                HOBJ2D h=MakeSingleGroup(_hsp);
                if(h){
                    long size;
                    HGLOBAL b=CopyToMemory2d(_hsp,h,&size);
                    if(b){
                        HOBJ2D obj=PasteFromMemory2d(hsp,b,&GetPastePoint(),pasteflags|PFC_MOVEOBJECT);
                        if(obj){
                            INT16 t=GetObjectType2d(hsp,obj);
                            if(t==otBITMAP2D){
                                POINT2D size;
                                GetActualSize2d(hsp,obj,&size);
                                SetObjectSize2d(hsp,obj,&size);
                            }
                            rez=TRUE;
                        }
                        GlobalFree(b);
                        object2d=obj;
                        SetCurrentObject2d(hsp,object2d);
                        CreateEditFrame2d();

                    }
                }
            }
            if(_hsp)DeleteSpace2d(_hsp);
        };
        ScDeleteFile(s);
        CloseClipboard();
    };
    return rez;
};

BOOL   TSchemeWindow::ProcessClipboardFormat(UINT uf)
{
    if(CFormat_CLASS==uf)
    {
        if(_class)Cm_PasteClass();return TRUE;
    };

    if(CFormat_OBJ==uf)
    {
        if(_class)Cm_PasteObj();return TRUE;
    };

    if(CF_2D==uf)
    {
        Cm_Paste();return TRUE;
    }

    if(CF_RTF==uf)
    {
        return Cm_PasteRtf();
    }

    switch(uf)
    {
        case CF_DIB         :Cm_PasteDIB();       break;
        case CF_TEXT        :Cm_PasteTEXT();      break;
        case CF_METAFILEPICT:return Cm_PasteWMF();break;
        default:return FALSE;
    }
    return TRUE;
};

void   TSchemeWindow::CopyDib(HGLOBAL hg,BOOL empty){
    if(hg){
        long size=GlobalSize(hg);
        HGLOBAL hglb=GlobalAlloc(GMEM_MOVEABLE,size);
#ifdef WIN32
        memcpy(GlobalLock(hglb),GlobalLock(hg),size);
#else
        hmemcpy(GlobalLock(hglb),GlobalLock(hg),size);
#endif

        GlobalUnlock(hglb);
        GlobalUnlock(hg);
        if (OpenClipboard(HWindow)){
            if(empty) EmptyClipboard();
            SetClipboardData(CF_DIB,hglb);
            CloseClipboard();
        }
    }
};
void   TSchemeWindow::SaveDib(HGLOBAL){

};

HOBJ2D TSchemeWindow::ConvertToOne(char*classname)
{
#ifdef PROTECTED
    if(!IsRegistered4())
    {
#ifdef WIN32
        PostMessage(mainwindow->HWindow,WM_COMMAND, MAKEWPARAM((UINT)(CM_REMIND),(UINT)(0)),0);
#else
        PostMessage(mainwindow->HWindow,WM_COMMAND,CM_REMIND,0);
#endif
        return 0;
    }
#endif
    HOBJ2D child=0;
    struct _TVP
    {
        TLinkInfo* link;      // копия
        TLinkInfo* _link;     // настоящая структура
        BYTE        first;
        HOBJ2D      objid;
        HOBJ2D      newobjid;  // дескриптор нового имиджа
        INT16       Nlink;     // номер линка в _class
    };
    struct _TVV
    {
        _TVP     *  l;
        TVarInfo * vi;
        pointer  addr;
        char     *name;
    };
    if(!__objects)return 0;

    if(MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG35) ,_MSG_WARNING,MB_YESNOCANCEL|MB_ICONQUESTION)!=IDYES)return 0;

    PClassListInfo pcli=_class->GetClassListInfo();
    PClass _Class=CreateNewClass(classname,NULL,NULL,pcli->library);
    UpdateClassWindows(UW_CREATE,UW_CLASS,_class->GetClassListInfo(),_class->GetClassListInfo()->library);
    if(!_Class)return 0;
    _Class->description=NewStr("This class was created automatically");
    int __close;
    TWaitCursor(HWindow);
    HSP2D hSpace=__GetClassScheme(_Class,__close);
    if (!hSpace)return 0;

    // Если есть объекты то записываем состояния
    TDOSStream *_st=NULL;
    if(_class->first){
        _st=new TDOSStream(NULL,TDOSStream::stCreate);
        INT16 count=GetGroupItemsNum2d(hsp,__objects);
        PObject po=_class->first;
        while(po){
            _st->WriteWord(0x1234);
            for(INT16 i=0;i<count;i++){
                HOBJ2D o=GetGroupItem2d(hsp,__objects,i);
                PObject _po=po->_GetObject(o);
                if(WhatObject(o)==1){
                    _st->WriteWord(o);
                    long p=_st->GetPos(); _st->WriteLong(0);
                    __SaveState(*_st,_po,3);
                    long _p=_st->GetPos();
                    _st->Seek(p);
                    _st->WriteLong(_p);
                    _st->Seek(_p);
                }
            }
            po=po->class_next;
        }
        _st->Seek(0);
    }

    // Если есть объекты то записываем состояния

    INT16 count;
    { // Убираем контактные площадки
reboot:
        count=GetGroupItemsNum2d(hsp,__objects);
        for(INT16 i=0;i<count;i++){
            HOBJ2D o=GetGroupItem2d(hsp,__objects,i); // было +1 GetGroupItem2d(hsp,__objects,i+1);
            if(WhatObject(o)==2){
                DelGroupItem2d(hsp,__objects,o);
                o=GetGroupItem2d(hsp,__frame,i);// было +1
                DelGroupItem2d(hsp,__frame,o);
                FullDeleteObject2d(hsp,o);
                goto reboot;
            }
        }
    }// Убираем контактные площадки
    HGLOBAL hg=_Copy();
    count=0;
    if (hg){
        LSTRUCT * items=NULL;
        POINT2D pastepoint={64,64};
        if(SelfPaste(hg,&pastepoint,_Class,NULL,hSpace,&items,count)){

            {
                TGrid*_grid=new TGrid(hSpace); _grid->SetGrid(); delete _grid;
            }

            //	  DeleteFrames();
            if (count>1){
                TCollection links(10,10);
                TProject *_project =new TProject();
                TProject *_tempprj=project;
                project=_project;
                _project->VarsMemory=new TMemoryManager(TMemoryManager::ZEROINIT);
                _project->VarsPointers=new TMemoryManager(TMemoryManager::ZEROINIT);
                TMemoryManager temp;

                PObject Object=CreateObjectByClass(_Class);
                Object->ClearVarPointers();
                Object->UpdateLinks(temp,*(_project->VarsMemory));
                project=_tempprj;

                for(INT16 i=0;i<_class->linkcount;i++){
                    HOBJ2D h1=_class->link[i].obj1Id,h2=_class->link[i].obj2Id;
                    if (h1==0)h1=_class->link[i].varhandle;
                    if (h2==0)h2=_class->link[i].varhandle;
                    BOOL p1=0,p2=0;
                    HOBJ2D nhobj1,nhobj2;
                    for(INT16 j=0;j<count;j++){
                        if (h1==items[j].oldhandle){p1=1;nhobj1=items[j].oldhandle;}
                        if (h2==items[j].oldhandle){p2=1;nhobj2=items[j].oldhandle;}
                    }

                    if (p1 || p2){
                        if (p1 && p2){
                        }else{
                            _TVP* vp  = new _TVP;
                            vp->first = p1!=0;
                            vp->link  = new TLinkInfo;
                            vp->_link = _class->link+i;
                            vp->link->Create(_class->link+i);
                            vp->objid = p1?nhobj1:nhobj2;
                            vp->newobjid=0;
                            vp->Nlink=i;
                            for(j=0;j<count;j++){
                                if(items[j].oldhandle==vp->objid){vp->newobjid=items[j].handle;break;}
                            }
                            links.Insert(vp);
                        }}
                }
                TCollection vars(10,10);
                if(links.count){
                    for(C_TYPE j=0;j<links.count;j++){
                        _TVP* vp  =(_TVP*)links.At(j);
                        PObject _po=Object->_GetObject(vp->newobjid);
                        for(INT16 k=0;k<vp->link->count;k++){
                            char ** vname=vp->first?(&(vp->link->items[k].name1)):(&(vp->link->items[k].name2));
                            PVarInfo pv;
                            pointer addr=_po->_GetVar(*vname,NULL,&pv);
                            for(C_TYPE jj=0;jj<vars.count;jj++){
                                _TVV * _pv=(_TVV *)vars.At(jj);
                                if(_pv->addr==addr){addr=0;
                                    if(lstrcmp(*vname,_pv->name)){
                                        delete *vname;
                                        *vname=NewStr(_pv->name);
                                    }
                                    break;
                                }
                            };
                            if(addr){
                                _TVV *_vv=new _TVV;
                                _vv->l=vp;
                                _vv->vi=pv;
                                _vv->name=NULL;
                                for(jj=0;jj<vars.count;jj++)
                                {
                                    _TVV * _pv=(_TVV *)vars.At(jj);
                                    if(!lstrcmpi(pv->name,_pv->name)){
                                        char ss[256];
                                        wsprintf(ss,"%s_a_%d_%d",_pv->name,vars.count,jj);
                                        _vv->name=NewStr(ss);
                                        break;
                                    }}
                                if(!_vv->name){
                                    _vv->name=NewStr(pv->name);
                                }
                                vars.Insert(_vv);
                            } } }
                    {
                        TMemoryStream _ms(TRUE);
                        _ms.WriteDOSSTR("//* auto generated *//");
                        for(C_TYPE jj=0;jj<vars.count;jj++)
                        {_TVV * _vv=(_TVV *)vars.At(jj);
                            char s[256];
                            wsprintf(s,"%s %s",_vv->vi->type->GetClassName(),_vv->name);
                            _ms.WriteDOSSTR(s);
                        }
                        _ms.WriteDOSSTR("//* auto generated *//");
                        _ms.WriteWord(0);
                        SetClassText(_Class,(char*)(_ms.base));
                    }
                    HOBJ2D varplace = CreateVarPlace(_Class,hSpace,8,8);
                    for(j=0;j<links.count;j++){
                        _TVP* vp  =(_TVP*)links.At(j);
                        HOBJ2D h1,h2;
                        if(vp->first){
                            h2=vp->newobjid;
                            h1=varplace;
                        }else{
                            h1=vp->newobjid;
                            h2=varplace;
                        }
                        for(INT16 k=0;k<vp->link->count;k++){
                            TLinkEntryInfo*le=vp->link->items+k;
                            char * n;
                            char ** _n,**__n;
                            if(vp->first){
                                n= le->name1;
                                _n=(&le->name2);
                                __n=&(vp->_link->items[k].name1);
                            }else{
                                n=le->name2;
                                _n=(&le->name1);
                                __n=&(vp->_link->items[k].name2);
                            }
                            for(INT16 k1=0;k1<vars.count;k1++){
                                _TVV * _vv=(_TVV *)vars.At(k1);
                                if(_vv->l==vp){
                                    if(!lstrcmpi(n,_vv->vi->name)){
                                        delete *_n;
                                        *_n=NewStr(_vv->name);
                                        delete *__n;
                                        *__n=NewStr(_vv->name);

                                        break;
                                    }
                                }
                            } };

                        HOBJ2D hlink=Vm_CreateLink(_Class->name,h1,h2);
                        INT16 num;
                        TLinkInfo *pl=_Class->GetLinkById(hlink,&num);
                        TLinkInfo _ltemp;_ltemp.Create(vp->link);
                        pl->count=_ltemp.count;
                        pl->items=_ltemp.items;
                        _ltemp.items=NULL;
                        _ltemp.count=0;
                    }
                    // Создали связи в новом имидже
                    // Теперь меняем в родителе
                }
                POINT2D org2d;
                {
                    POINT2D org,size;
                    if(GetObjectOrgSize2d(hsp,__objects,&org,&size)){
                        org2d.x=org.x+size.x/2;
                        org2d.y=org.y+size.y/2;
                    }else org2d=GetPastePoint();
                    AddChildren(_class,_Class->name,&child,org2d.x-16,org2d.y-16);
                };
                if(links.count){
                    for(C_TYPE j=0;j<links.count;j++){
                        _TVP* vp  =(_TVP*)links.At(j);
                        INT16 pnum=0;
                        if(vp->first){
                            vp->_link->obj1Id=child;
                        }else { vp->_link->obj2Id=child;
                            pnum=GetVectorNumPoints2d(hsp,vp->_link->handle)-1;
                        }
                        SetVectorPoint2d(hsp,vp->_link->handle,pnum,&org2d);
                    }

                    // now all delete
                    for(j=0;j<links.count;j++){
                        _TVP* vp  =(_TVP*)links.At(j);
                        _class->DeleteLink(vp->link);
                        delete vp->link;
                        delete vp;
                    }
                    for(C_TYPE jj=0;jj<vars.count;jj++)
                    {_TVV * _vv=(_TVV *)vars.At(jj);
                        if(_vv->name)delete _vv->name;
                        delete _vv;
                    }
                } // это 1 был ниже

                INT16 _count=GetGroupItemsNum2d(hsp,__objects);
                HOBJ2D *hitems=new HOBJ2D[_count];
                GetGroupItems2d(hsp,__objects,hitems,_count);
                DeleteFrames(hsp);
                for(i=0;i<_count;i++){
                    RemoveChildren(_class,hitems[i],TRUE,i==_count-1);
                }
                delete hitems;
                // all delete

                // 1 был здесь
                delete Object;
                TProject *_prj=projects;
                delete _project;
                projects=_prj;

            }
            // Чтение состояния
            if(_st){
                PObject po=_class->first;
                while(po){
                    _st->Seek(0);
                    if(_st->ReadWord()!=0x1234)goto failed;
                    PObject poc=po->_GetObject(child);
                    for(INT16 i=0;i<_Class->childcount;i++){
                        HOBJ2D _h=_st->ReadWord();
                        HOBJ2D h=0;
                        for(int j=0;j<count;j++)if(items[j].oldhandle==_h)h=items[j].handle;
                        if(h){
                            PObject _po=poc->_GetObject(h);
                            long p=_st->ReadLong();
                            if((po==NULL) || !__LoadState(*_st,_po))goto failed;
                            long _p=_st->GetPos();
                            if(_p!=p){
                                _st->Seek(p);
                            };
                        }else goto failed;
                    }
                    po=po->class_next;
                }
                goto f_ok;
failed:_Message("Conversion error!");
f_ok:
            }
            // Чтение состояния
        }

        if (items)delete items;

        GlobalFree(hg);
    } // Cant Copy
    if(_st){delete _st;}
    __CloseClassScheme(_Class,__close);
    return child;
};
void TSchemeWindow::Cm_PasteClass(){
    POINT2D pp=GetPastePoint();
    if(OpenClipboard(HWindow)){
        TMemoryStream ms(GetClipboardData(CFormat_CLASS));
        int command=ms.ReadWord();
        char s[256];
        while(ms.ReadStr(s,sizeof(s))){
            PClass __class=GetClassByName(s);
            HOBJ2D _obj=0;
            AddChildren(_class,__class->name,&_obj,pp.x,pp.y);
            object=_obj;
            SetState(sfbase);
            CreateImageFrame();
        }
        CloseClipboard();
    }
};
void TSchemeWindow::Cm_PasteObj(BOOL _2d_){
    if(IsClipboardFormatAvailable(CFormat_OBJ) && OpenClipboard(HWindow))
    {
        TRestoreCrd crd(hsp);
        HOBJ2D _h=0;
        BOOL _2d=_2d_ && IsClipboardFormatAvailable(CF_2D);
        SetState(sfbase);
        POINT2D pp=GetPastePoint();

        if(_2d){
            CloseClipboard();
            HOBJ2D obj=PasteFromClipboard2d(hsp,NULL,PFC_SETCURRENT|pasteflags);
            OpenClipboard(HWindow);
            HOBJ2D  h= GetObjectByName2d(hsp,obj,"$@#_frames2d_$@#");
            if(h){
                _h=obj;
                object2d=0;
            }else{
                FullDeleteObject2d(hsp,obj);
            }
        }

        _Paste(GetClipboardData(CFormat_OBJ),_2d?NULL:&pp,_2d);
        __objects2d=_h;

        if(_2d)TrackGroup(pp);
        if (__frame || __objects2d)SetState(sfrectbase);
        CloseClipboard();
    }
};

void TSchemeWindow::Cm_CopyObj(){
    if(OpenClipboard(HWindow)){
        EmptyClipboard();
        if(object || __objects || link){
            HGLOBAL hg=_Copy();
#ifdef WIN32
            if (hg)
#else
            if ( ((int)hg)>32)
#endif
            {
                if(!SetClipboardData(CFormat_OBJ,hg))GlobalFree(hg);
            }
        }
        if(__objects2d){
            HGLOBAL hg =lastinclipboard=CopyToMemory2d(hsp,__objects2d,NULL);
            if(hg){
                if(!SetClipboardData(CF_2D,hg))GlobalFree(hg);
            }
        }
        CloseClipboard();
    }
};
HGLOBAL TSchemeWindow::_Copy(){
    if (__objects==0 && object==0 && link==0)return NULL;
    TRestoreCrd crd(hsp);
    TMemoryStream st(FALSE);
    HOBJ2D * items;
    INT16 count;
    POINT2D _org;
    // Копирование связи
    if(link){
        INT16 id;
        PLinkInfo pl;
        if(pl=_class->GetLinkById(link,&id)){
            st.WriteWord(256);
            HOBJ2D pen=GetPenObject2d(hsp,link);

            LINKCLIP lc;
            memset(&lc,0,sizeof(lc));
            lc.width=GetPenWidth2d(hsp,pen);
            lc.color=GetPenColor2d(hsp,pen);
            lc.flags=pl->flags;
            lc.layer=GetObjectLayer2d(hsp,link);
            //   PClass _c;

            if(pl->obj1Id){
                INT16 id = _class->GetChildById(pl->obj1Id);
                if(id>-1){
                    lstrcpyn(lc.obj1,_class->childs[id].type->GetClassName(),sizeof(lc.obj1));
                }
            }else lstrcpyn(lc.obj1,_class->name,sizeof(lc.obj1));

            if(pl->obj2Id){
                INT16 id = _class->GetChildById(pl->obj2Id);
                if(id>-1){
                    lstrcpyn(lc.obj2,_class->childs[id].type->GetClassName(),sizeof(lc.obj2));
                }
            }else lstrcpyn(lc.obj2,_class->name,sizeof(lc.obj2));

            st.Write(&lc,sizeof(lc));
            _class->WriteLink(st,&(_class->link[id]));
            st.WriteWord(0);
        }
    }else{ // Имиджи
        if (__objects){
            count=GetGroupItemsNum2d(hsp,__objects);
            items=new HOBJ2D[count];
            GetGroupItems2d(hsp,__objects,items,count);
            GetObjectOrgSize2d(hsp,__objects,&_org,NULL);
        }else{
            items=new HOBJ2D[1];
            items[0]=object;
            GetObjectOrgSize2d(hsp,object,&_org,NULL);
            count=1;
        };
        {
            st.WriteWord(5);
            BOOL set=FALSE;
            POINT2D p;
            __AddOrg2d(hsp,__objects,p,set);
            __AddOrg2d(hsp,object,p,set);
            st.Write(&p,sizeof(p));
        }
        st.WriteWord(1);
        //	 st.WriteStr("CLASS");
        st.WriteWord(count);

        for(INT16 i=0;i<count;i++)
        { INT16 type=WhatObject(items[i]);

            INT16 n= _class->GetChildById(items[i]);
            if (n>-1){
                st.WriteStr(_class->childs[n].type->GetClassName());
                st.WriteStr(_class->childs[n].name);
            }else {
                if(type==2)st.WriteStr("#varplace#");else
                    st.WriteStr("");
            }
            st.WriteWord(GetObjectLayer2d(hsp,items[i]));
            st.WriteWord((INT16)items[i]);
            POINT2D org,size;GetObjectOrgSize2d(hsp,items[i],&org,&size);
            org.x-=_org.x;org.y-=_org.y;
            st.Write(&org,sizeof(org));
            st.Write(&size,sizeof(size));
        };

        BOOL wl=0;
        for( i=0;i<_class->linkcount;i++){
            HOBJ2D h1=_class->link[i].obj1Id,h2=_class->link[i].obj2Id,
                    vh=_class->link[i].varhandle;
            BOOL p1=0,p2=0,pv=0;
            for(INT16 j=0;j<count;j++){
                if (h1==items[j])p1=1;
                if (h2==items[j])p2=1;
                if (vh==items[j])pv=1;
            }
            if ((p1 && p2) || (pv && (p1 || p2))){
                HOBJ2D h=_class->link[i].handle;
                INT16    _count=GetVectorNumPoints2d(hsp,h);
                POINT2D *points=new POINT2D[_count];
                GetVectorPoints2d(hsp,h,points,_count);

                for(INT16 k=0;k<_count;k++){
                    points[k].x-=_org.x;
                    points[k].y-=_org.y;
                };
                if (!wl){
                    wl=1; st.WriteWord(2);
                    //		  st.WriteStr("LINK");
                }
                st.WriteWord(256);
                st.WriteWord(_count);
                st.Write(points,sizeof(POINT2D)*_count);
                delete points;

                HOBJ2D pen=GetPenObject2d(hsp,h);
                COLORREF rgb;
                rgb=GetPenColor2d(hsp,pen);
                INT16 width=GetPenWidth2d(hsp,pen);

                st.Write(&rgb,sizeof(COLORREF));
                st.WriteWord(width);
                st.WriteWord(GetObjectLayer2d(hsp,h));
                _class->WriteLink(st,&(_class->link[i]));
            }
        }

        if (wl)st.WriteWord(0);//end of links

        if (_object){
            BOOL setsave=FALSE;

            st.WriteWord(3);
            long _pos=st.GetPos();
            st.WriteLong(0);
            //	st.WriteStr("STATE");
            for(INT16 i=0;i<count;i++)
            {PObject po=_object->_GetObject(items[i]);
                if (po){
                    setsave|=po->IsSetExist();
                    st.WriteWord((INT16)items[i]);
                    __SaveState(st,po);
                }
            };
            long _pos2=st.GetPos();
            st.Seek(_pos);
            st.WriteLong(_pos2-(_pos+4));
            st.Seek(_pos2);

            if(setsave){
                st.WriteWord(4);
                _pos=st.GetPos();
                st.WriteLong(0);

                for(INT16 i=0;i<count;i++)
                {PObject po=_object->_GetObject(items[i]);
                    if (po && po->IsSetExist()){
                        st.WriteWord((INT16)items[i]);
                        //        long _pos=st.GetPos();
                        //	     st.WriteLong(0);
                        po->SaveSetState(st);
                        //        long _pos2=st.GetPos();
                        //        st.Seek(_pos);
                        //        st.WriteLong(_pos2);
                        //        st.Seek(_pos2);
                    }
                };

                st.WriteWord(0);
                long _pos2=st.GetPos();
                st.Seek(_pos);
                st.WriteLong(_pos2-(_pos+4));
                st.Seek(_pos2);
            }

        };
        delete items;

    }
    st.WriteWord(0);//end of block
    st.Truncate();
    return st.GetHANDLE();
};

BOOL TSchemeWindow::SelfPaste(HGLOBAL hglobal,POINT2D* point,PClass _Class,PObject _Object,HSP2D hSpace,LSTRUCT**_items,INT16 &count){
    POINT2D pbuf;
    LSTRUCT*items=*_items;
    TMemoryStream st(hglobal);
    INT16 code=st.ReadWord();
    if(st.status){
        _Message("Error #1 in paste operations");
        return FALSE;}
    while (code){
        switch(code){
            case 256:{ // чтение одной связи
                LINKCLIP* lc=new LINKCLIP;
                st.Read(lc,sizeof(LINKCLIP));
                tmpdata=lc; //? must remove
                lc->plink=new TLinkInfo();
                _Class->ReadLink(st,lc->plink);
                SetCursor(HResource,IDC_NEWLINK); SetState(sfLClipBase); //? must remove
            }break;
            case 1:{ // чтение классов
                count=st.ReadWord();
                *_items=items=new LSTRUCT[count];
                for(INT16 i=0;i<count;i++){
                    char childname[128];
                    st.ReadStr(childname);    // Class name
                    if (childname[0])
                    {
                        char * s=NULL;
                        BOOL vp=!lstrcmpi(childname,"#varplace#");
                        if(!vp) s=st.ReadStr();    // Child name
                        INT16 layer=st.ReadWord();
                        items[i].oldhandle=(HOBJ2D)st.ReadWord(); // Old Handle
                        POINT2D org,size;
                        st.Read(&org,sizeof(org));       // Point
                        st.Read(&size,sizeof(size));
                        if(point)
                        {
                            org.x+=point->x;org.y+=point->y;
                        }
                        HOBJ2D num=0;
                        if(vp){
                            items[i].handle=CreateVarPlace(_Class,hSpace,org.x,org.y);
                            items[i].varplace=1;
                            num=items[i].handle;
                        }else{
                            items[i].varplace=0;
                            if(AddChildren(_Class,childname,&num,org.x,org.y)){
                                SetObjectLayer2d(hsp,num,layer);
                                INT16 _num=_Class->GetChildById(num);

                                items[i].handle=num;
                                _Class->childs[_num].name=s;
                                if(s)SetObjectName2d(hsp,num,s);
                                if(_Class->childs[_num].type->GetClass()->flags&CF_SIZEABLE){
                                    SetObjectSize2d(hSpace,num,&size);
                                }

                                //  size
                            }else goto m1;
                        }

                        //if (dib)
                        //CreateGRPFrame(num,dib,org);
                    }}
                ;}break;
            case 2:
            {
                code=st.ReadWord();
                while (code)
                {
                    INT16 _count=st.ReadWord();
                    POINT2D *points=new POINT2D[_count];
                    st.Read(points,sizeof(POINT2D)*_count);
                    INT16 k;
                    if(point)
                        for(INT16 k=0;k<_count;k++)
                        {
                            points[k].x+=point->x;
                            points[k].y+=point->y;
                        };
                    COLORREF color;
                    st.Read(&color,sizeof(COLORREF));
                    INT16 width=st.ReadWord();
                    INT16 layer=st.ReadWord();
                    HOBJ2D h=CreatePolyline2d(hSpace,CreatePen2d(hSpace,PS_SOLID,width,color,R2_COPYPEN),
                                              0 , points,_count);
                    SetObjectLayer2d(hSpace,h,layer);
                    ObjectToBottom2d(hSpace,h);
                    TLinkInfo li;
                    _Class->ReadLink(st,&li);
                    li.handle=h;
                    for(k=0;k<count;k++){
                        if (li.obj1Id==items[k].oldhandle){li.obj1Id=items[k].handle;break;}
                    };
                    for(k=0;k<count;k++){
                        if (li.obj2Id==items[k].oldhandle){li.obj2Id=items[k].handle;break;}
                    }
                    for(k=0;k<count;k++){
                        if (li.varhandle==items[k].oldhandle){li.varhandle=items[k].handle;break;}
                    };

                    delete points;
                    AddLink(_Class,&li);
                    li.Done();
                    code=st.ReadWord();
                }
                ;}break;
            case 3:{
                long _delta=st.ReadLong();
                if (_Object){
                    for(INT16 i=0;i<count;i++)
                    {PObject po=_Object->_GetObject(items[i].handle);
                        if (po){
                            HOBJ2D h=(HOBJ2D)st.ReadWord();
                            if (h==items[i].oldhandle) __LoadState(st,po); else goto m0;
                        } } }else{
                    st.Seek(st.GetPos()+_delta);
                }
                ;}break;
            case 4:
            {
                long _delta=st.ReadLong()+st.GetPos();
                if(_Object){
                    do{
                        HOBJ2D h=st.ReadWord();
                        HOBJ2D _h=0;
                        if(!h)break;
                        //        LONG pos=st.ReadLong();
                        PObject po=NULL;
                        for(INT16 j=0;j<count;j++){
                            if (h==items[j].oldhandle){_h=items[j].handle;break;}
                        }
                        po=_Object->_GetObject(_h);
                        if (po){
                            _LoadSetState(po,st);
                            po->GetProject()->VarChange();
                        }else {st.Seek(_delta);break;}
                    }while(st.status==0);
                }else st.Seek(_delta);
            }break;
            case 5:
            { // Перемещение в первоначальную точку
                st.Read(&pbuf,sizeof(pbuf));
                if(!point){
                    point=&pbuf;
                }
            };break;
            default:
                _Message("Error #2 in paste operations");
                break;
        }
        code=st.ReadWord();
    }
m0:

m1:
    return TRUE;
}

void  TSchemeWindow::_Paste(HGLOBAL hglobal,POINT2D* point,BOOL allwaysgrp){
    INT16 count=0;
    LSTRUCT * items=NULL;
    if(SelfPaste(hglobal,point,_class,_object,hsp,&items,count)){
        DeleteFrames();
        if (count>1 || (allwaysgrp && count>0)){
            __frame=CreateGroup2d(hsp,NULL,0);
            __objects=CreateGroup2d(hsp,NULL,0);
            HOBJ2D dib=CreateBrush2d(hsp,BS_PATTERN,0,0,
                                     CreateDib2dFromResource(hsp,HResource,MAKEINTRESOURCE(BM_MASKblue)),R2_MASKPEN);

            for(int i=0;i<count;i++){
                POINT2D org;
                GetObjectOrgSize2d(hsp,items[i].handle,&org,NULL);
                CreateGRPFrame(items[i].handle,dib,org);
            }
        }else{
            if (count==1){
                object=items[0].handle;
                CreateImageFrame();
            }}
    }
    if (items)delete items;
};

BOOL CreateBar3d(HSP3D hsp3d,HOBJ2D obj,double sizex,double sizey,double sizez,POINT3D *org,
                 COLORREF c1,COLORREF c2,BYTE flag){
    COLOR3D color,color2;
    color.rgb=c1;color2.rgb=c2;

    POINT3D aa[8];
    sizey/=2.0;sizex/=2.0;sizez/=2.0;

    aa[0].x=aa[3].x=aa[4].x=aa[7].x=-sizex;
    aa[1].x=aa[5].x=aa[2].x=aa[6].x=sizex;
    aa[0].y=aa[1].y=aa[4].y=aa[5].y=-sizey;
    aa[2].y=aa[3].y=aa[6].y=aa[7].y=sizey;
    aa[0].z=aa[1].z=aa[2].z=aa[3].z=sizez;
    aa[4].z=aa[5].z=aa[6].z=aa[7].z=-sizez;
    INT16 c=GetNumPoints3d(hsp3d,obj);
    if(org){
        for(int i=0;i<8;i++){
            aa[i].x+=org->x;
            aa[i].y+=org->y;
            aa[i].z+=org->z;
        }
    }

    AddPoints3d(hsp3d,obj,aa,8);

    PRIMITIVE3D p[6];
    memset(&p,0,sizeof(p));

    INT16 npoints[16]={
        0,1,2,3,
        4,5,6,7,
        0,4,1,5,
        2,6,3,7
    };
    for(int i=0;i<16;i++){npoints[i]+=c;}
    for(i=0;i<6;i++){p[i].flags=PRIMITIVE_POLYLINE;p[i].color=i>1?color2:color;}
    p[0].flags|=PRIMITIVE_CLOSELINE;
    p[1].flags|=PRIMITIVE_CLOSELINE;

    p[0].count=p[1].count=4;
    p[2].count=p[3].count=p[4].count=p[5].count=2;

    p[0].points=npoints;
    p[1].points=npoints+4;
    p[2].points=npoints+8;
    p[3].points=npoints+10;
    p[4].points=npoints+12;
    p[5].points=npoints+14;
    if(flag){
        CreatePrimitives3d(hsp3d,obj,p,1);
        CreatePrimitives3d(hsp3d,obj,p+2,2);
    }else
        CreatePrimitives3d(hsp3d,obj,p,6);
}

BOOL CreateCylinder3d(HSP3D hsp3d,HOBJ2D obj,double R,double H,COLORREF c1,COLORREF c2){
    COLOR3D color,color2;
    color.rgb=c1;color2.rgb=c2;
    int count=16;
    INT16 c=GetNumPoints3d(hsp3d,obj);
    POINT3D *aa=new POINT3D[count*2];
    for(int i=0;i<count;i++)
    {
        double a=2*M_PI*((double)i/(double)count);
        aa[i].x=aa[i+count].x=R*cos(a);
        aa[i].y=aa[i+count].y=R*sin(a);
        aa[i].z=H/2;aa[i+count].z=-H/2;
    };
    AddPoints3d(hsp3d,obj,aa,count*2);
    delete aa;
    for(i=0;i<count;i++)
    {
        PRIMITIVE3D p[3];
        memset(&p,0,sizeof(p));
        INT16 npoints[6];
        npoints[0]=i+c;npoints[4]=i+c+count;
        npoints[2]=i+c;npoints[3]=i+count+c;

        if(i==count-1){
            npoints[1]=c;
            npoints[5]=c+count;
        }else{
            npoints[1]=npoints[0]+1;
            npoints[5]=npoints[4]+1;
        }

        for(int j=0;j<3;j++)
        {
            p[j].count=2;
            p[j].flags=PRIMITIVE_POLYLINE;
            p[j].points=npoints+j*2;
            p[j].color=j==1?color2:color;
        }
        CreatePrimitives3d(hsp3d,obj,p,3);
    }
}

BOOL CreateSphere3d(HSP3D hsp3d,HOBJ2D obj,double R,COLORREF c1,COLORREF c2){
    COLOR3D color,color2;
    color.rgb=c1;color2.rgb=c2;
    int count=16;
    POINT3D *aa=new POINT3D[count];
    int _c=8;
    for(int j=0;j<_c;j++){
        double f=M_PI*((double)j/(double)_c);

        INT16 c=GetNumPoints3d(hsp3d,obj);

        for(int i=0;i<count;i++)
        {
            double a=2*M_PI*((double)i/(double)count);
            aa[i].x = (R*cos(a))*cos(f);
            aa[i].z = R*sin(a);
            aa[i].y= (R*cos(a))*sin(f);
        };
        AddPoints3d(hsp3d,obj,aa,count);
        for(i=0;i<count;i++)
        {
            PRIMITIVE3D p;
            memset(&p,0,sizeof(p));
            INT16 npoints[2];
            npoints[0]=i+c;
            npoints[1]=i+c+1;

            if(i==count-1){ npoints[1]=c;  }

            p.count=2;
            p.flags=PRIMITIVE_POLYLINE;
            p.points=npoints;
            p.color=color;
            CreatePrimitives3d(hsp3d,obj,&p,1);
        }
    }
    delete aa;
};

BOOL  TSchemeWindow::CreateTextureFrame(int mode){
    if(htmpframe){DeleteObject2d((HSP2D)hsp3d,htmpframe);htmpframe=0;}
    POINT3D Origin ,Size;
    if(!GetObjectOrgSize3d(hsp3d,object3d,&Origin ,&Size))return FALSE;

    htmpframe=CreateObject3d(hsp3d);
    double matrix[16];
    GetMatrix3d(hsp3d,object3d,matrix);
    POINT3D o3,s3;
    GetObjectOrgSize3d(hsp3d,object3d,&o3,&s3);
    o3.x+=s3.x/2;o3.y+=s3.y/2;o3.z+=s3.z/2;
    SetMatrix3d(hsp3d,htmpframe,matrix);

    PushCrdSystem3d(hsp3d);
    CRDSYSTEM crd;
    crd.type=LOCAL_COORDINATE;
    crd.hObject=object3d;
    SetCrdSystem3d(hsp3d,&crd);
    SetObjectBase3d(hsp3d,htmpframe,&o3);


    crd.type=LOCAL_COORDINATE;
    crd.hObject=htmpframe;
    SetCrdSystem3d(hsp3d,&crd);

    COLORREF c1 = RGB(0,255,255);
    COLORREF c2 = RGB(255,146,36);

    switch(mode){
        case 0:CreateBar3d(hsp3d,htmpframe,Size.x*1.1,Size.y*1.1,Size.z*1.1,NULL,c1,c2,1);break;
        case 1:CreateBar3d(hsp3d,htmpframe,Size.x*1.1,Size.y*1.1,Size.z*1.1,NULL,c1,c2,0);break;
        case 2:CreateCylinder3d(hsp3d,htmpframe,Size.x*1.4/2,Size.z*1.1,c1,c2);break;
        case 3:CreateSphere3d(hsp3d,htmpframe,Size.z*1.3/2,c1,c2);break;
        case 256:{


        }
    }
    char s[64];
    wsprintf(s,"%d",mode);
    SetObjectName2d((HSP2D)hsp3d,htmpframe,s);
    PopCrdSystem3d(hsp3d);
};
HOBJ2D TSchemeWindow::GetObject3d(int mode){
    switch(mode){
        case 0:if(addmode3d && htmpframe )return htmpframe;
        default: return object3d;
    }
};

BOOL  TSchemeWindow::ApplyTexture(){
    WUVMAP wuv(hsp3d,GetCurrentTool2d((HSP2D)hsp3d,TEXTURE3D),object3d,htmpframe);
    char s[64];
    GetObjectName2d((HSP2D)hsp3d,htmpframe,s,sizeof(s));
    wuv.mapmode=s[0]-'0';
    return MakeTextureMap(wuv);
};

void   TSchemeWindow::CurrChanged(){
    HSP2D _hsp=hsp;
    UINT32 d2;
    if(state<sfbase3d){
        d2=object2d;
        if(hardmode==SCHEME_EDITOR){
            if(object)d2 = object| (1<<16);
            if(link)d2   = link  | (2<<16);
            if(__objects)d2=__objects|((1+256)<<16);
        }
    }else{
        _hsp=(HSP2D)hsp3d;
        d2=object3d|(4<<16);
    }
    dllmanager->ForEach(SC_CUROBJ2DCHANGED,(UINT32)_hsp,d2);
};
BOOL  TSchemeWindow::CreateToolPopup(int id){
    TSTRATUMTOOLBAR sb;
    sb.id=id;
    sb.x=CW_USEDEFAULT;
    sb.y=CW_USEDEFAULT;
    sb.cx=0;
    sb.cy=0;
    sb.hTemplate=HResource;
    sb.sTemplate="Prop_ToolBar";
    sb.flags=1;
    switch(id){

        case CM_MODIFY3D:{
            char s[64];
            LoadString(HResource,IDS_str197,s,sizeof(s));
            sb.title   = s;//"Текстура";
            sb.buttons = new TPropertyButton(IDS_str198,HResource,"Texture_Map",0);
            sb.buttons->Procedure=TxtToolProc;
            TPropertyButton *b= new TPropertyButton(IDS_str199,HResource,"Primitive_Vertexes",0);
            b->Procedure=VertToolProc;
            b->hsp=(HSP2D)hsp3d;
            b->iuser1=object3d;
            sb.buttons->Insert(b);
            b= new TPropertyButton(IDS_str200,HResource,"Primitive_Edit",0);
            b->Procedure=PrimToolProc;
            b->hsp=(HSP2D)hsp3d;
            b->iuser1=object3d;

            sb.buttons->Insert(b);
            lstrcpy(sb.prefix,"obj_txt_map");

        }break;
        default:return FALSE;
    }
    return SCCreateToolPopup(&sb)!=NULL;
};

void  TSchemeWindow::DestroyToolPopup(int id){
    SCDestoyToolPopup(id);
};

BOOL TSchemeWindow::NewMovePoint3d(POINT2D&_pp,POINT3D&_newpoint,BOOL mvobj){
    // на вход
    // base - базовая точка
    // _pp
    // на выход
    // _newpoint - новая точка
    switch(submode3d){
        case 0:{ // in View coordinates
            LP2DLP3D lp;
            memset(&lp,0,sizeof(lp));
            lp._base=base;
            POINT3D _org3d;
            if(LP2dtoLP3d(hsp,object2d,&lp,&_pp,&_org3d,1)){
                //SetObjectBase3d(hsp3d,object3d,&_org3d);
                _newpoint=_org3d;
                return TRUE;
            }
        }break;
            // on the Axis
        case 4:
        case 2:
        case 1:{ // in Axis

            POINT3D _b;
            if(mvobj) GetObjectBase3d(hsp3d,GetObject3d(),&_b);else _b= _newpoint;

            {
                POINT3D newbase=_b;
                POINT3D nbw;
                POINT2D p2d;

                LP3dtoLP3d(hsp3d,&_b,&nbw,1,LP3D_CURRENT,LP3D_WORLD);
                // Из текущих в мировые
                LP3dtoLP2d(hsp,object2d,&p2d,&nbw,1);
                // Теперь координаты проекции

                double d=sqrt((_pp.x-p2d.x)*(_pp.x-p2d.x)+(_pp.y-p2d.y)*(_pp.y-p2d.y));
                double step=100.0;
                POINT2D op=p2d;

                do{

                    switch(submode3d){
                        case 1:_b.x+=step;break;
                        case 2:_b.y+=step;break;
                        case 4:_b.z+=step;break;
                    }

                    LP3dtoLP3d(hsp3d,&_b,&nbw,1,LP3D_CURRENT,LP3D_WORLD);
                    LP3dtoLP2d(hsp,object2d,&p2d,&nbw,1);

                    double d1=sqrt((_pp.x-p2d.x)*(_pp.x-p2d.x)+(_pp.y-p2d.y)*(_pp.y-p2d.y));

                    if ((abs(op.x-p2d.x)<0.5)&&(abs(op.y-p2d.y)<0.5) && (abs(d1-d)<0.2))break;
                    if(d1>d){
                        step=-(step/2.0);
                    }
                    d=d1;
                    op=p2d;
                }while (TRUE);

                //SetObjectBase3d(hsp3d,GetObject3d(),&_b);
                _newpoint=_b;
                return TRUE;
            }
        }break;
        case 3:
        case 6:
        case 5:
        {

            POINT3D _org3d;
            //  int _mode=2;
            //  HOBJ2D object2d=hmove;
            //  HOBJ2D object3d=object3d;

            POINT2D _p=_pp;
            //        _p.x=pp.x+ddelta.x;
            //        _p.y=pp.y+ddelta.y;
            // _p - новые координаты центра объекта в координатах пространства
            POINT3D _b,_b_;

            if(GetObjectBase3d(hsp3d,GetObject3d(),&_b)){
                _b_=_b;
                TransformPoint3d(hsp,object2d,&_b_,1,1);
                // Перевод базы в систему координат проекции

                POINT2D po;
                LP2dtoView(hsp,object2d,&_p,&po,1);
                POINT3D _p2d;
                _p2d.x=po.x;
                _p2d.y=po.y;
                _p2d.z=_b_.z;

                POINT3D points[3],_points[3];
                for(int i=0;i<3;i++){
                    _points[i]=_b;
                    // 0 x-y  3
                    // 1 y-z  6
                    // 2 z-x  5

                    switch(submode3d){
                        case 3:
                            if(i==1)_points[i].x+=100;
                            if(i==2)_points[i].y+=100;
                            break;
                        case 6:
                            if(i==1)_points[i].z+=100;
                            if(i==2)_points[i].y+=100;
                            break;
                        case 5:
                            if(i==1)_points[i].x+=100;
                            if(i==2)_points[i].z+=100;
                            break;
                    }

                }
                if(!LP3dtoLP3d(hsp3d,_points,points,3,LP3D_CURRENT,LP3D_VIEW,object2d))return FALSE;
                double a,b,c,d;
                c= points[0].x*(points[1].y - points[2].y)
                        -points[1].x*(points[0].y - points[2].y)
                        +points[2].x*(points[0].y - points[1].y);

                d=-(points[0].x*(points[1].y*points[2].z  -  points[2].y*points[1].z)-
                        points[1].x*(points[0].y*points[2].z  -  points[2].y*points[0].z)+
                        points[2].x*(points[0].y*points[1].z  -  points[1].y*points[0].z));

                a=  points[0].y*(points[1].z-points[2].z)
                        -points[1].y*(points[0].z-points[2].z)
                        +points[2].y*(points[0].z-points[1].z);
                b=-(points[0].x*(points[1].z-points[2].z)
                        -points[1].x*(points[0].z-points[2].z)
                        +points[2].x*(points[0].z-points[1].z));

                double r=sqrt(a*a+b*b+c*c);
                if (c<0){r=-r;}
                if (r!=0){
                    a=a/r;b=b/r;c=c/r;d=d/r;

                    if(fabs(c)>0) _p2d.z=-(d+a*_p2d.x+b*_p2d.y)/c;

                    TransformPoint3d(hsp,object2d,&_p2d,1,0);
                    // Перевод обратно в мировую систему координат
                    POINT3D _pcd; //
                    LP3dtoLP3d(hsp3d,&_p2d,&_pcd,1,LP3D_WORLD,LP3D_CURRENT,0);
                    // Перевод в текущую систему координат
                    //SetObjectBase3d(hsp3d,GetObject3d(),&_pcd);
                    _newpoint=_pcd;
                    return TRUE;
                }}

        }break;
    }
    return FALSE;
}
void TSchemeWindow::GetNewImagePos(){
    RECT r;
    GetClientRect(HWindow,&r);
    if(_class){
        int sx=max(r.right/40,2);
        int sy=max(r.right/40,2);
        BYTE *buf=new BYTE[sx*sy];
        memset(buf,0,sx*sy);
        for(int i=0;i<_class->childcount;i++){
            TChildInfo*ci=_class->childs+i;
            int x=(ci->x-hscroll->GetPos())/40;
            int y=(ci->y-vscroll->GetPos())/40;
            if(x>=0 && y>=0 && x<sx && y<sy){buf[y*sx+x]=1;}
        }
        for(int y=0;y<sy;y++)
            for(int x=0;x<sx;x++){
                if(buf[y*sx+x]==0){
                    _mousepos.x=x*40 - hscroll->GetPos()%40;
                    _mousepos.y=y*40 - vscroll->GetPos()%40;
                    DPtoLP2d(hsp,&_mousepos,1);
                    Snap(_mousepos);
                    y=sy;
                    break;
                }
            }
        delete buf;
    }else{
        _mousepos.x=r.right;_mousepos.y=r.bottom;
        _mousepos.x/=2;_mousepos.y/=2;
        DPtoLP2d(hsp,&_mousepos,1);
        Snap(_mousepos);
    }
};
#endif // runtime


