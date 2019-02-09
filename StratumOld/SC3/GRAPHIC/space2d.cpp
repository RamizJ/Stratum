/*
Copyright (c) 1995,1996  Virtual World
Module Name:
     space2d.cpp
Author:
     Alexander Shelemekhov
*/
#include <windows.h>
#include "space2d.h"
#include "space3d.h"
#include "meta.h"
#include <math.h>
#include "_system.h"
#include <mem.h>
#include "object3d.h"
#include "SPACE2D.H"
//#include <gwin2d3d\3drender.h>
#include "_SPACE.H"
#include "space2d.h"
#include "camera3d.h"
#include "_object.h"
#ifndef WIN32
#include <string.h>
#endif

#define IsToolOverflow(tool)\
    if (tool->count>=max_size)return 0;


#define CHECK_FOR_DUPLICATE(collect,tool,func)\
    if (State&SF_NOTEQUALTOOLS){\
    for(C_TYPE i = 0; i < collect->count; i++){\
    tool *t1=(tool *)collect->At(i);\
    if (func(t,t1)){\
    delete t;\
    return t1->_HANDLE;\
    } } }\
    return collect->InsertObject(t);

//#define GetGDIColor(_color) (palette? 0x02000000lu|_color:_color)

#define nTabPositions 15
//int TabStopPositions[nTabPositions]={50,100,150,200,250, 300,350,400,450,500, 550,600,650,700,750};


extern HPALETTE palette_256;
COLORREF TSpace2d::GetGDIColor(COLORREF _color)
{
    if(_color&0x02000000L)
    {
        _color=GetSysColor((int)((0xfffful)&_color));
    }
    return palette? 0x02000000lu|_color:_color;
};

//#define  _test_speed
//#include <string.h>
HBRUSH GetGrayBrush();
extern BYTE showtime3d;
static char * fileend ="End Of File <!>";

int isRectEmpty(RECT r){return (r.left>=r.right)||(r.top>=r.bottom);}

void Rect_or_Rect(RECT & r,RECT r1)
{
    if (isRectEmpty (r))	r=r1;
    if (isRectEmpty (r1))	r1=r;
    r.left=min(r.left,r1.left);r.right=max(r.right,r1.right);
    r.top=min(r.top,r1.top);r.bottom=max(r.bottom,r1.bottom);
};

void AddPointToRect(RECT & r,POINT2D p)
{
    r.left=min(r.left,p.x);
    r.right=max(r.right,p.x);
    r.top=min(r.top,p.y);
    r.bottom=max(r.bottom,p.y);
}

void AddPointToRect(RECT2D & r,POINT2D p)
{
    r.left=min(r.left,p.x);
    r.right=max(r.right,p.x);
    r.top=min(r.top,p.y);
    r.bottom=max(r.bottom,p.y);
}

int RectIntersect(RECT r,RECT r1)
{
    RECT t;
    if (isRectEmpty (r))return 0;
    if (isRectEmpty (r1))return 0;

    t.left=max(r.left,r1.left);
    t.top=max(r.top,r1.top);
    t.right=min(r.right,r1.right);
    t.bottom=min(r.bottom,r1.bottom);
    return  (t.left<t.right)&&(t.top<t.bottom);
};

void MovePoint(POINT2D & p,POINT2D& d)
{
    p.x+=d.x;
    p.y+=d.y;
}

void RotatePoint(POINT2D& p, POINT2D& base, double f)
{
    if (f!=0.0)
    {
        p.x -= base.x;
        p.y -= base.y;

        double s = sin(f);
        double c = cos(f);

        gr_float x = p.x * c - p.y * s;
        gr_float y = p.x * s + p.y * c;
        p.x = x + base.x;
        p.y = y + base.y;
    }
}

void RotateRect(RECT& r,double f)
{
    if (f!=0.0)
    {
        POINT2D p1,p2,p3,p4;
        p1.x=r.left;p1.y=r.top;
        p2.x=r.right;p2.y=r.top;
        p3.x=r.right;p3.y=r.bottom;
        p4.x=r.left;p4.y=r.bottom;
        RotatePoint(p2,p1,f);
        RotatePoint(p3,p1,f);
        RotatePoint(p4,p1,f);
        r.left=min(min(p1.x,p2.x),min(p3.x,p4.x));
        r.right=max(max(p1.x,p2.x),max(p3.x,p4.x))+1;
        r.top=min(min(p1.y,p2.y),min(p3.y,p4.y));
        r.bottom=max(max(p1.y,p2.y),max(p3.y,p4.y))+1;
    }
};

void RotateRect(RECT2D& r,double f)
{
    if (f!=0.0)
    {
        POINT2D p1,p2,p3,p4;
        p1.x=r.left;p1.y=r.top;
        p2.x=r.right;p2.y=r.top;
        p3.x=r.right;p3.y=r.bottom;
        p4.x=r.left;p4.y=r.bottom;
        RotatePoint(p2,p1,f);
        RotatePoint(p3,p1,f);
        RotatePoint(p4,p1,f);
        r.left=min(min(p1.x,p2.x),min(p3.x,p4.x));
        r.right=max(max(p1.x,p2.x),max(p3.x,p4.x))+1;
        r.top=min(min(p1.y,p2.y),min(p3.y,p4.y));
        r.bottom=max(max(p1.y,p2.y),max(p3.y,p4.y))+1;
    }
};

void GetRotateRectPoints(RECT2D r,double f,POINT2D*pp)
{
    pp[0].x=r.left; pp[0].y=r.top;
    pp[1].x=r.right;pp[1].y=r.top;
    pp[2].x=r.right;pp[2].y=r.bottom;
    pp[3].x=r.left; pp[3].y=r.bottom;
    RotatePoint(pp[1],pp[0],f);
    RotatePoint(pp[2],pp[0],f);
    RotatePoint(pp[3],pp[0],f);
};

void GetRotateRectPoints(RECT r,double f,POINT2D * pp)
{
    pp[0].x=r.left; pp[0].y=r.top;
    pp[1].x=r.right;pp[1].y=r.top;
    pp[2].x=r.right;pp[2].y=r.bottom;
    pp[3].x=r.left; pp[3].y=r.bottom;
    RotatePoint(pp[1],pp[0],f);
    RotatePoint(pp[2],pp[0],f);
    RotatePoint(pp[3],pp[0],f);
};

void GetRotateRectPoints(RECT r,double f,POINT * pp)
{
    pp[0].x=r.left; pp[0].y=r.top;
    pp[1].x=r.right;pp[1].y=r.top;
    pp[2].x=r.right;pp[2].y=r.bottom;
    pp[3].x=r.left; pp[3].y=r.bottom;

    POINT2D base;
    PntToPnt(base,pp[0]);
    for(int i=1;i<4;i++){
        POINT2D p2d;
        PntToPnt(p2d,pp[i]);
        RotatePoint(p2d,base,f);
        PntToPnt(pp[i],p2d);
    }
};

void _Line_(HDC dc,POINT p1,POINT p2)
{
    POINT p[2];p[0]=p1;p[1]=p2;
    Polyline(dc,p,2);
}

HWND  TSpace2d::SetHWND(HWND _hwnd)
{
    HWND o = hwnd;
    hwnd = _hwnd;

    for(C_TYPE i = 0; i < primary->count; i++)
    {
        TObject2d * o=(TObject2d *)primary->At(i);
        switch(o->WhoIsIt())
        {
            case 26:
            {
                TControl2d*c=(TControl2d*)o;
                c->SetParent(_hwnd);
            }break;
        }
    }
    return o;
};

void AddSlash(char *p)
{
    int l=lstrlen(p);
    if (l)
    {
        if (p[l-1]!='\\'){lstrcat(p,"\\");}
    }
};

int IsAbsolutePath(char *file){
    if (!file || ((*file)==0))return 0;
    while ((*file == ' ')&& *file)file++;
    int l=lstrlen(file);
    if(l>2){
        char * Lps=strstr(file,":");
        if(Lps){
            if(Lps[1]=='\\')return 1;
            return 3;
        }
        if((file[0]=='\\')&&(file[1]=='\\'))return 1;
    }
    return 0;
};
// Возвращает
// 1 - абсолютный
// 3 - несовсем но с уазанием на другой диск
// 0 - относительный путь

BOOL   GetAbsoluteName(char*base,char*relative,char*full){
    if(relative==NULL){ *full=0;return FALSE; }
    if(base)
        //	 char _buf[260];
        lstrcpy(full,base);
    AddSlash(full);
    char *_a=relative;
    while (*_a=='.' && _a[1]=='.'&& _a[2]=='\\'){
        _a+=3;
        char *ps=(full+lstrlen(full))-2;
        while ((ps-full)>0 && *ps!='\\')ps--;
        if(*ps=='\\')ps[1]=0;
    }
    lstrcat(full,_a);
    return TRUE;
};
int TSpace2d::GetFullPath(char* dst,char* fn){
    OFSTRUCT lpOpenBuff;
    if(fn && IsAbsolutePath(fn)){ // Это и так абсолютный путь
        lstrcpy(dst,fn);
        return (OpenFile(dst,&lpOpenBuff,OF_EXIST)>0);
    }

    if (txtPath){ // по пути на локальный каталог
        GetAbsoluteName(txtPath,fn,dst);
        if(OpenFile(dst,&lpOpenBuff,OF_EXIST)>0)return TRUE;
    }

    if (path){ // по пути на разделяемые ресурсы
        GetAbsoluteName(path,fn,dst);
        if(OpenFile(dst,&lpOpenBuff,OF_EXIST)>0)return TRUE;
    }
    if(GetPrivateProfileString("SUBST",fn,"",dst,256,_graphics_ini)){
        if(OpenFile(dst,&lpOpenBuff,OF_EXIST)>0)return TRUE;
    }
    // просто так
    if(OpenFile(fn,&lpOpenBuff,OF_EXIST)>0){ lstrcpy(dst,fn); return TRUE;   }

    return FALSE;
}
/*
int TSpace2d::GetFullPath(char* dst,char* fn)
 {int k=1;
    if (fn==NULL)return 0;
    if (path){lstrcpy(dst,path);lstrcat(dst,fn);} else lstrcpy(dst,fn);
reload:
    OemToAnsi(dst,dst);AnsiUpper(dst);AnsiToOem(dst,dst);

    OFSTRUCT lpOpenBuff;
    if(OpenFile(dst,&lpOpenBuff,OF_EXIST)>0)return TRUE;
    else
     switch(k){
      case 1:if(GetPrivateProfileString("SUBST",fn,"",dst,128,_graphics_ini) ) {k==2;goto  reload;}break;

     }

     if((k==1) && GetPrivateProfileString("SUBST",fn,"",dst,128,_graphics_ini) )
                  {k==2;goto  reload;}
     if(k && path)  {k=0;lstrcpy(dst,fn);goto  reload;}

    return FALSE;
 };
*/
void TSpace2d::SetHANDLE(HSP2D hsp)
{
    _HANDLE = hsp;
    for(C_TYPE i = 0; i < primary->count; i++)
    {
        TUserObject2d* po = (TUserObject2d*)primary->At(i);
        if (po->WhoIsIt()==25)
        {
            po->type->objproc(hsp,&(po->data),OM_SPACEVALID,NULL);
        }

        if (hwnd && po->WhoIsIt()==26)
        {
            ((TControl2d*)po)->SetParent(hwnd);
        }
    }
}
PDib2d TSpace2d::GetDib2d(HOBJ2D h,BOOL)
{
    return (PDib2d)dibs->GetByHandle(h);
};

HOBJ2D TSpace2d::CreateRDoubleDib2d(LPSTR FileName)
{
    PRefToDoubleDib2d pf=new TRefToDoubleDib2d(FileName,this);
    if (pf->tool) return ddibs->InsertObject(pf);
    delete pf;return 0;
};
BOOL TSpace2d::SetRDoubleDib2d(HOBJ2D t,LPSTR n,LPSTR o)
{
    PRefToDoubleDib2d pr=(PRefToDoubleDib2d)ddibs->GetByHandle(t);
    if (pr && pr->WhoIsIt()==111)
    {char ss[128];
        if(GetFullPath(ss,n)){
            PDoubleDib2d dib=GetRDoubleDib2d(ss,pr);
            if (dib){
                if (o)lstrcpy(o,pr->name);
                delete pr->name;pr->name=NewStr(n);
                if(pr->tool)pr->tool->DecRef(pr);pr->tool=dib;
                ChkRDoubleDibToUnload();
                ChkDoubleDib((PDoubleDib2d)pr);
                return TRUE;
            }}
    }
    return 0;
};

HOBJ2D TSpace2d::CreateRDib2d(LPSTR FileName)
{
    PRefToDib2d pf=new TRefToDib2d(FileName,this);
    if (pf->tool)
        return dibs->InsertObject(pf);
    delete pf;return 0;
};

int TSpace2d::SetRDib2d(HOBJ2D t,LPSTR n,LPSTR o)
{
    PRefToDib2d pr=(PRefToDib2d)dibs->GetByHandle(t);
    if (pr && pr->WhoIsIt()==ttREFTODIB2D)
    {char ss[256];
        if(GetFullPath(ss,n)){
            PDib2d dib=GetRDib2d(ss,pr);
            if (dib){
                if (o)lstrcpy(o,pr->name);
                if (pr->tool==dib){
                    pr->tool->DecRef(pr);
                }else{
                    delete pr->name;pr->name=NewStr(n);
                    if(pr->tool)pr->tool->DecRef(pr);pr->tool=dib;
                    ChkRDibToUnload();}
                ChkDib((PDib2d)pr);
                return 1;
            }}
    }
    return 0;
}

//*-------------------------------------------------------------------------*//
PDoubleDib2d TSpace2d::GetDoubleDib2d(HOBJ2D h,BOOL)
{
    return (PDoubleDib2d)ddibs->GetByHandle(h);
}

HOBJ2D TSpace2d::CreateAxis3d(POINT3D o,int s)
{
    PView3d_2d po=(PView3d_2d)GetObjByHANDLE(all->current);
    if (po && po->WhoIsIt()==otVIEW3D2D)
    {
        if (Axis3d){UpdateSpecObject((PObject2d)Axis3d);
            delete Axis3d;}
        Axis3d=new TAxis3d(o,s,po);
        UpdateSpecObject((PObject2d)Axis3d);
        return OID_AXIS3d;
    }return 0;
};

HOBJ2D TSpace2d::GetFrame3dCursor(POINT2D&pt)
{/*int x,y;
  const d=4;
 if (Axis3d)
    {x=(int)(Axis3d->tops[0].ix);
     y=(int)(Axis3d->tops[0].iy);
     if (((x-d)<pt.x)&&((x+d)>pt.x) && ((y-d)<pt.y)&&((y+d)>pt.y)  )
      return OID_AXIS3d;
    }
  if (Frame3d)
    {  long iz=0;INT16 j=-1;
    for (C_TYPE i=0;i<8;i++){
     x=(int)(Frame3d->tops[i].ix/tofloat);
     y=(int)(Frame3d->tops[i].iy/tofloat);
     if (((x-d)<pt.x)&&((x+d)>pt.x) && ((y-d)<pt.y)&&((y+d)>pt.y)  )
      if ((j==-1)||(Frame3d->tops[0].iz>iz))
        {iz=Frame3d->tops[0].iz;j=i;}
    }
    if (j!=-1)
    return (HOBJ2D)(OID_FRAME3d1+j);
    }*/
    return 0;
};

void TSpace2d::CRDtoLP(POINT2D FAR* lppt,int cPoints){
    if(crd_used){
        for(int i=0;i<cPoints;i++){
            double x=lppt[i].x;
            double y=lppt[i].y;
            lppt[i].x=x*crd_system.matrix[0][0]+y*crd_system.matrix[1][0]+crd_system.matrix[2][0];
            lppt[i].y=x*crd_system.matrix[0][1]+y*crd_system.matrix[1][1]+crd_system.matrix[2][1];
        }}
};
void TSpace2d::LPtoCRD(POINT2D FAR* lppt,int cPoints){
    if(crd_used){
        for(int i=0;i<cPoints;i++){
            double x=lppt[i].x;
            double y=lppt[i].y;
            lppt[i].x=x*mobr[0][0]+y*mobr[1][0]+mobr[2][0];
            lppt[i].y=x*mobr[0][1]+y*mobr[1][1]+mobr[2][1];
        }}
};

BOOL TSpace2d::SetCrd(CRDSYSTEM2D*c){
    BOOL rez=TRUE;
    crd_system.type=c->type;
    memset(crd_system.matrix,0,sizeof(crd_system.matrix));
    crd_system.center=c->center;
    crd_system.hObject=c->hObject;
    crd_system.matrix[2][2]=1;
    crd_system.matrix[0][2]=crd_system.matrix[1][2]=0;
    crd_used=TRUE;
    switch(c->type){
        case MATRIX_COORDINATE2D: memcpy(&crd_system.matrix,&(c->matrix),sizeof(crd_system.matrix));break;

        case XLYU_COORDINATE2D:crd_system.matrix[2][0]=c->center.x;
            crd_system.matrix[2][1]=c->center.y;
            crd_system.matrix[0][0]=-1;
            crd_system.matrix[1][1]=-1;
            break;
        case XLYD_COORDINATE2D:crd_system.matrix[2][0]=c->center.x;
            crd_system.matrix[2][1]=c->center.y;
            crd_system.matrix[0][0]=-1;
            crd_system.matrix[1][1]=1;
            break;
        case XRYD_COORDINATE2D:crd_system.matrix[2][0]=c->center.x;
            crd_system.matrix[2][1]=c->center.y;
            crd_system.matrix[0][0]=1;
            crd_system.matrix[1][1]=1;
            break;
        case XRYU_COORDINATE2D:crd_system.matrix[2][0]=c->center.x;
            crd_system.matrix[2][1]=c->center.y;
            crd_system.matrix[0][0]=1;
            crd_system.matrix[1][1]=-1;
            break;
        case MMTEXT_COORDINATE2D:crd_used=FALSE;break;
        default:rez=FALSE;
    }
    if(!rez){ crd_used=FALSE;crd_system.type=MMTEXT_COORDINATE2D; }
    // Вычисление обратной
    if(crd_used){
        double det=crd_system.matrix[0][0]*crd_system.matrix[1][1]-
                   crd_system.matrix[0][1]*crd_system.matrix[1][0];
        if(det){
            double det_1=1.0/det;
#define a crd_system.matrix
            mobr[0][0] = (  (a[1][1]*a[2][2] - a[2][1]*a[1][2] )*det_1);
            mobr[0][1] = (- (a[0][1]*a[2][2] - a[2][1]*a[0][2] )*det_1);
            mobr[0][2] = 0;//(  (a[0][1]*a[1][2] - a[1][1]*a[0][2] )*det_1);
            mobr[1][0] = (- (a[1][0]*a[2][2] - a[2][0]*a[1][2] )*det_1);
            mobr[1][1] = (  (a[0][0]*a[2][2] - a[2][0]*a[0][2] )*det_1);
            mobr[1][2] = 0;//(- (a[0][0]*a[1][2] - a[1][0]*a[0][2] )*det_1);
            mobr[2][0] = (  (a[1][0]*a[2][1] - a[2][0]*a[1][1] )*det_1);
            mobr[2][1] = (- (a[0][0]*a[2][1] - a[2][0]*a[0][1] )*det_1);
            mobr[2][2] = 1;//(  (a[0][0]*a[1][1] - a[1][0]*a[0][1] )*det_1);
#undef a
        }else crd_used=FALSE;
    }
    return rez;
};

BOOL TSpace2d::PushCrd(){
    if(!pushpop)pushpop=new TItems(5,5);
    if(pushpop->count<100){
        CRDSYSTEM2D * crd=new CRDSYSTEM2D;
        if(!crd_used)crd_system.type=MMTEXT_COORDINATE2D;
        memcpy(crd,&crd_system,sizeof(crd_system));
        pushpop->Insert(crd);
        return TRUE;
    } return FALSE;
};

BOOL TSpace2d::PopCrd(){
    if(pushpop && pushpop->count){
        CRDSYSTEM2D * c=(CRDSYSTEM2D * )pushpop->At(pushpop->count-1);
        pushpop->AtDelete(pushpop->count-1);
        SetCrd(c);
        delete c;
        return TRUE;
    }
    return FALSE;
};


BOOL TSpace2d::DeleteAxis3d()
{if (Axis3d){UpdateSpecObject((PObject2d)Axis3d);
        delete Axis3d;Axis3d=NULL;return TRUE;
    }
    return FALSE;
};


void TSpace2d::_DrawUser2d(HDC dc,TUserObject2d *po,RECT UpdRect){
    USERPAINT up;
    up.hDc=dc;
    up.paint=UpdRect;
    up.paintdc=wndupdaterect;

    if (po->type->flags&UF_MMTEXT){
        up._org=po->origin;
        up._size=po->size;
        _LpToDp(&(up._org),1);
        _LpToDpWO(&(up._size),1);
    }

    if (po->type->flags&UF_MMANIZATROPIC){ SetAnisotropic(dc);}

    po->type->objproc(_HANDLE,&(po->data),OM_PAINT,DWORD(&up));

    if (po->type->flags&UF_MMANIZATROPIC){ SetTextMode(dc); }

};

void TSpace2d::_DrawAxis3d(HDC dc,RECT)
{/*int i;
  POINT2D delta;
  PView3d_2d pv=Axis3d->view;
//  pv->SetXYZab(0);
  delta=pv->origin;
  delta.x+=pv->projector->poOffset.x;
  delta.y+=pv->projector->poOffset.y;
  _LpToDp(&delta,1);
  POINT p[2];
  p[0].x=(int)Axis3d->tops[0].ix+delta.x;
  p[0].y=(int)Axis3d->tops[0].iy+delta.y;
  HDC mdc=CreateCompatibleDC(dc);
  SelectObject(mdc,Axis3d->bitmap);
  BitBlt(dc,p[0].x,p[0].y,15,14,mdc,0,0,SRCINVERT);
  for(i=1;i<4;i++)
    {
     p[1].x=(int)Axis3d->tops[i].ix+delta.x;
     p[1].y=(int)Axis3d->tops[i].iy+delta.y;
     Polyline(dc,p,2);
     BitBlt(dc,p[1].x,p[1].y,15,14,mdc,i*16,0,SRCINVERT);
    }
  DeleteDC(mdc);
  */
};
BOOL FaceVisible(POINT2D&t1,POINT2D&t2,POINT2D&tL){
    double v1=t2.x-t1.x;
    double v2=t2.y-t1.y;
    double w1=tL.x-t1.x;
    double w2=tL.y-t1.y;
    return ((v1*w2-v2*w1)>0.0);
}
void TSpace2d::_DrawFrame3d(HDC dc,RECT)
{
    HGDIOBJ pen2=::CreatePen(PS_SOLID,2,RGB(0,255,0));
    HGDIOBJ pen=::CreatePen(PS_SOLID,1,RGB(0,255,0));
    HGDIOBJ pend=::CreatePen(PS_DOT,1,RGB(0,191,0));

    HGDIOBJ old=SelectObject(dc,pen);

    HGDIOBJ b=CreateSolidBrush(RGB(0,0,255));
    HGDIOBJ oldb=SelectObject(dc,b);

    POINT2D tops[8];
    //  HPEN pen3=::CreatePen(PS_DASH,1,RGB(255,255,255));
    //  HGDIOBJ old=SelectObject(dc,pen3);
    int i;
    PView3d_2d pv=Frame3d->view;
    POINT2D delta;
    //  pv->SetXYZab(0);
    delta=pv->origin;
    _LpToDp(&delta,1);
    for(i=0;i<8;i++){
        tops[i].x=Frame3d->tops[i].x+delta.x;
        tops[i].y=Frame3d->tops[i].y+delta.y;
    }
    RECT r;
    for(i=0;i<8;i++){

        if (i==0){r.left=tops[i].x;r.top=tops[i].y;
            r.right=r.left;r.bottom=r.top;
        }else{r.left=min(r.left,tops[i].x);
            r.right=max(r.right,tops[i].x);
            r.top=min(r.top,tops[i].y);
            r.bottom=max(r.bottom,tops[i].y); }
    }
    if (r.right-r.left<5){
        int m=(r.right+r.left)/2;
        r.right=m+3;r.left=m-3; }
    if (r.bottom-r.top<5){
        int m=(r.top+r.bottom)/2;
        r.bottom=m+3;r.top=m-3; }

    BOOL faces[6];
    faces[0]=FaceVisible(tops[0],tops[1],tops[3]);
    faces[1]=FaceVisible(tops[1],tops[5],tops[2]);
    faces[2]=FaceVisible(tops[5],tops[4],tops[6]);
    faces[3]=FaceVisible(tops[4],tops[0],tops[7]);
    faces[4]=FaceVisible(tops[3],tops[2],tops[7]);
    faces[5]=FaceVisible(tops[4],tops[5],tops[0]);

    typedef struct tagFRAME3Dfaces{
        BYTE p1,p2,f1,f2;
    }FRAME3Dfaces;

    static FRAME3Dfaces info[]={
        {0,1,0,5},
        {1,2,0,1},
        {2,3,0,4},
        {0,3,0,3},
        // Первая грань
        {5,4,2,5},
        {4,7,2,3},
        {7,6,2,4},
        {6,5,2,1},
        // Задняя грань
        {1,5,5,1},
        {2,6,4,1},
        {3,7,4,3},
        {0,4,5,2}
    };
    static  BYTE _tops[8][3]={
        {0,3,5},
        {0,1,5},
        {0,1,4},
        {0,4,3},

        {2,3,5},
        {2,1,5},
        {2,1,4},
        {2,4,3},

    };
    for(i=0;i<8;i++){
        if (faces[_tops[i][0]]||faces[_tops[i][1]]||faces[_tops[i][2]])
            //Rectangle(dc,tops[i].x-2,tops[i].y-2,tops[i].x+3,tops[i].y+3);
            PatBlt(dc,tops[i].x-2,tops[i].y-2,4,4,PATCOPY);
    };
    int mode=GetBkMode(dc);
    SetBkMode(dc, TRANSPARENT);

    for(i=0;i<sizeof(info)/sizeof(FRAME3Dfaces);i++){
        if (faces[info[i].f1]&&faces[info[i].f2])SelectObject(dc,pen2);
        else{
            if (faces[info[i].f1]||faces[info[i].f2])SelectObject(dc,pen);
            else SelectObject(dc,pend);
        }
        POINT p[2];
        PntToPnt(p[0],tops[info[i].p1]);
        PntToPnt(p[1],tops[info[i].p2]);
        Polyline(dc,p,2);
    }
    SetBkMode(dc,mode);
    SelectObject(dc,old);
    ::DeleteObject(pen);
    ::DeleteObject(pen2);
    ::DeleteObject(pend);

    SelectObject(dc,oldb);
    ::DeleteObject(b);
};

HRGN GetRotateRectRGN(RECT r,double f)
{
    POINT pp[4];;
    GetRotateRectPoints(r,f,pp);
    return CreatePolygonRgn(pp,4,OPAQUE);
}

int PointNearLine(POINT2D &l1,POINT2D &l2,POINT2D &p, double width)
{
    //const d=3.0;
    //const pd=2.0;
    double d=max(width,3);
    double pd=d;
    RECT tr;
    tr.left=min(l1.x,l2.x);
    tr.right=max(l1.x,l2.x);
    //if (l1.x>l2.x) 	{tr.left=l2.x;tr.right=l1.x;}
    //else           	{tr.left=l1.x;tr.right=l2.x;}

    tr.top=min(l1.y,l2.y);
    tr.bottom=max(l1.y,l2.y);
    //if (l1.y>l2.y)  {tr.top=l2.y;tr.bottom=l1.y;}
    //else						{tr.top=l1.y;tr.bottom=l2.y;}

    if (((tr.left-d)<p.x)&&((tr.right+d)>p.x)&&
        ((tr.top-d)<p.y)&&((tr.bottom+d)>p.y))
    {
        if (l1.x-l2.x)
        {
            if (l1.y-l2.y)
            {
                double y,x;
                y=l1.y+((l2.y-l1.y)*((p.x-pd)-l1.x))/(l2.x-l1.x);
                if ( (y>=(p.y-pd))&&(y<=(p.y+pd)) )
                    return TRUE;
                y=l1.y+((l2.y-l1.y)*((p.x+pd)-l1.x)) /(l2.x-l1.x);
                if ( (y>=(p.y-pd))&&(y<=(p.y+pd)) )
                    return TRUE;
                x=l1.x+((l2.x-l1.x)*((p.y-pd)-l1.y))/(l2.y-l1.y);
                if ( (x>=(p.x-pd))&&(x<=(p.x+pd)) )
                    return TRUE;
                x=l1.x+((l2.x-l1.x)*((p.y+pd)-l1.y))/(l2.y-l1.y);
                if ((x>=(p.x-pd))&&(x<=(p.x+pd)))
                    return TRUE;
            }
            else
                return (((l1.y-d)<=p.y)&&((l1.y+d)>=p.y)); //горизонтальна
        }
        else
            return (((l1.x-d)<=p.x)&&((l1.x+d)>=p.x)); // вертикальна
    } //Внутри прямоугольника
    return 0;
};

PObject TSpace2d::GetObjByHANDLE(HOBJ2D h)
{
    if (h < OID_RESERVED)
        return _SPACE::GetObjByHANDLE(h);

    if (h == OID_RCENTER)
        return RCenter;

    if (h >= OID_FRAME2d1 && h <= OID_FRAME2d)
        return Frame;

    return NULL;
}

BOOL TSpace2d::DeleteObject2d(HOBJ2D h)
{
    PObject po = GetObjByHANDLE(h);
    if (po)
    {
        AddObjectRect((PObject2d)po);
        rectchange = 1;
        if((Frame) && Frame->object && (IsObjectInObject(po, Frame->object)))
            DeleteEditFrame2d();

        _DeleteObject(h);
        return 1;
    }
    return 0;
};

void TSpace2d::UpdateFrame(PObject2d po)
{
    if ((Frame)&&(IsObjectInObject(po,Frame->object)))
    {AddRect(Frame->GetRect());
        Frame->UpdateSize();
        AddRect(Frame->GetRect());
    }
};
//*-----------------------------------------------------*
TSpace3d * TSpace2d::Get3dSpace(HOBJ2D h){
    return (TSpace3d *)spaces3d->GetByHandle(LOBYTE(h));
}

void TSpace2d::UpdateSpecObject(PObject2d po)
{
    if (hwnd==0)
        return;

    int type=po->WhoIsIt();

    if( (ischanged) || (lock) || (type==52) || (type==53) || ((po==Frame) && (Frame->options & FRAME_MASK)))
        AddRect(po->GetRect());
    else
    {
        HDC dc=GetDC(hwnd);
        membitmaporg.x=0;membitmaporg.y=0;
        SetTextMode(dc);
        int rop=SetROP2(dc,R2_XORPEN);
        HPEN penold=(HPEN)SelectObject(dc,GetStockObject(WHITE_PEN));
        {
            POINT2D delta=org;
            INT16 _x=(delta.x);
            INT16 _y=(delta.y);
            delta.x=-(_x&7);
            delta.y=-(_y&7);
#ifdef WIN32
            SetBrushOrgEx(dc,delta.x,delta.y,NULL);
#else
            SetBrushOrg(dc,delta.x,delta.y);
#endif
        }
        switch (type)
        {
            case 50: _DrawFrame(dc,po->GetRect());break;
            case 51: _DrawRCenter(dc);break;
                //	case 52: _DrawFrame3d(dc,po->GetRect());break;
                //	case 53: _DrawAxis3d(dc,po->GetRect());break;
        }
        SetROP2(dc,rop);
        SelectObject(dc,penold);
        ReleaseDC(hwnd,dc);
    }
};
BOOL TSpace2d::Set3dViewCamera(HOBJ2D Object2d,HOBJ2D hCamera,BOOL _m)
{PView3d_2d pv=(PView3d_2d)GetObjByHANDLE(Object2d);
    if ( pv && pv->WhoIsIt()==24)
    {
        TCamera3d *cam=(TCamera3d *)pv->space3d->GetObjByHANDLE(hCamera);
        if (cam==0 || cam->WhoIsIt()!=11)return FALSE;
        if (cam==pv->camera)return TRUE;
        pv->UpdateRect(NULL);
        if (Axis3d)UpdateSpecObject((PObject2d)Axis3d);
        if (Frame3d)UpdateSpecObject((PObject2d)Frame3d);
        pv->imagesize.x-1;pv->imagesize.y=-1;
        if (_m)
        {NewMetaRecord(mfset3dviewparam);
            meta->WriteWord(Object2d);
            meta->WriteWord(hCamera);
            NewUndoRecord(mfset3dviewparam);
            meta->WriteWord(Object2d);
            meta->WriteWord(pv->camera->_HANDLE);
            CloseMetaRecord();
        }
        pv->camera->views->Delete(pv);
        cam->views->Insert(pv);
        pv->NewCamera(cam);
        if (Axis3d){Axis3d->UpdateSize();UpdateSpecObject((PObject2d)Axis3d);}
        if (Frame3d){Frame3d->UpdateSize();UpdateSpecObject((PObject2d)Frame3d);}
        return 1;
    }
    return 0;
};
//meta file
void TSpace2d::WriteLastRecord(long pp)
{mrecord.PrevRecord=pp;
    mrecord.RedoCode=0;
    mrecord.UndoCode=0;
    mrecord.UndoPos=0;
    mrecord.NextRecord=0;
    meta->Write(&mrecord,sizeof(META_RECORD));
};
void TSpace2d::NewMetaRecord(INT16 code)
{if ((record)&&(meta->status==0)&&((record_flag & 6)==0))
    {mrecord.NextRecord=meta->GetPos();
        mrecord.RedoCode=record_flag&1?(-code):(code);
        record_flag&=(0xfe);
        record_flag|=2;
        modifyed=1;
    }else _Error(EM_internal);
};
void TSpace2d::NewMetaRecord1(INT16 c,WORD w1){
    NewMetaRecord(c);
    meta->WriteWord(w1);
};
void TSpace2d::NewMetaRecord2(INT16 c,WORD w1,WORD w2){
    NewMetaRecord1(c,w1);
    meta->WriteWord(w2);
};

void TSpace2d::NewUndoRecord(INT16 code)
{if ((record)&&(meta->status==0)&&((record_flag & 6)==2))
    {mrecord.UndoCode=code;
        mrecord.UndoPos=meta->GetPos()-mrecord.NextRecord;
        record_flag |=6;
    }else _Error(EM_internal);
};
void TSpace2d::NewUndoRecord1(INT16 c,WORD w1){
    NewUndoRecord(c);
    meta->WriteWord(w1);
};
void TSpace2d::NewUndoRecord2(INT16 c,WORD w1,WORD w2){
    NewUndoRecord1(c,w1);
    meta->WriteWord(w2);

};

void TSpace2d::NewMetaRecord3d(INT16 code,UINT16 hsp)
{NewMetaRecord(code);
    meta->Write(&hsp,1);
};
void TSpace2d::NewUndoRecord3d(INT16 code,UINT16 hsp)
{NewUndoRecord(code);
    meta->Write(&hsp,1);
};

void TSpace2d::CloseMetaRecord()
{if ((record)&&(meta->status==0)&&((record_flag & 6)==6))
    {long p=meta->GetPos(),p1=mrecord.NextRecord-sizeof(META_RECORD);
        mrecord.NextRecord=p;
        meta->Seek(p1);
        meta->Write(&mrecord,sizeof(META_RECORD));
        meta->Seek(p);
        WriteLastRecord(p1);
        record_flag &=~6;
    }else _Error(EM_internal);
};
void TSpace2d::WriteMetaPoints(PStream){};
INT16  TSpace2d::DoRedo()
{if ((record)&&(meta->status==0))
    {if (mrecord.NextRecord)
        {INT16 rc=mrecord.RedoCode;
            ProcessMetaRecord((UINT16)abs(rc));
            meta->Seek(mrecord.NextRecord);
            meta->Read(&mrecord,sizeof(META_RECORD));
            return rc;
        }
        return 0;
    }else return (INT16)_Error(EM_internal);
};

INT16  TSpace2d::DoUndo()
{if ((record)&&(meta->status==0))
    {if (mrecord.PrevRecord)
        {INT16 uc;
            meta->Seek(mrecord.PrevRecord);
            meta->Read(&mrecord,sizeof(META_RECORD));
            long pos=meta->GetPos();
            meta->Seek(pos+mrecord.UndoPos);
            uc=mrecord.UndoCode;
            ProcessMetaRecord(uc);
            meta->Seek(pos);
            return uc;
        }return 0;
    }else return (INT16)_Error(EM_internal);
};

BOOL TSpace2d::CloseMetaFile()
{if (record && meta)
    {if (modifyed)meta->Truncate();
        delete meta;meta=NULL;record=0;
        return TRUE;
    }
    return FALSE;
};

INT16 TSpace2d::ProcessMetaRecord(INT16 code)
{
    /*
 INT16 w1,w2,w3,w4;
  TOOL_TYPE tt;
  BOOL b1;
  HOBJ2D obj;
  POINT2D point;
if (code<2000)
 switch(code)
  {
case  mfstartofgroup:
        while(1){
        w1=meta->ReadWord();
        if (w1==mfendofgroup)return 0;
        if (ProcessMetaRecord(w1)) return 1;
        }
// Tools2d
case  mfdeleteunusedtools:
        w1=meta->ReadWord();
        w3=DeleteUnusedTool((TOOL_TYPE)w1);
        return 0;
case  mfloadtools:
        {BYTE b;
        PToolsCollection pt;pt=GetToolsById((TOOL_TYPE)(meta->ReadWord()));
         if (pt){
         do
          {meta->Read(&b,1);
            if (b)
            {PTools2d p=(PTools2d)pt->GetItem(meta);
             if (p==NULL) break;else
              {if (!p->Awaken(this))
                 {delete p;break;}
              }
             pt->Insert(p);
            }
          }while (b);}else break;
         return 0;
        }
case  mfmerge:{
        POINT2D point;
        meta->ReadPOINT(&point);
        if(MergeWithSpace(meta,point,meta->ReadWord())>0)return 0;
        }break;
case  mfsetscale2d:
        {POINT2D _mul,_div;
        meta->ReadPOINT(&_mul);
        meta->ReadPOINT(&_div);
         if (SetScale2d(_mul,_div))return 0;
        }break;
case  mfdeletetool:{
        w1=meta->ReadWord();w2=meta->ReadWord();
        if (DeleteTool2d((TOOL_TYPE)(w1),w2))return 0;
        break;
        }
case  mfcreatetool:{
        PToolsCollection pt;pt=GetToolsById((TOOL_TYPE)meta->ReadWord());
        if (pt){
        PTools2d t;t=(PTools2d)pt->GetItem(meta);
         if ((t)&&(pt->IndexOf(t)<0))
          {int h=t->_HANDLE;
            if (!t->Awaken(this)){delete t;break;}
            if(pt->InsertObject(t)==h)return 0;
          }
         }
        break;
        }
// pen
case  mfcreatepen:{
         COLORREF rgb;
         INT16 Style,Width,Rop2;
         Style=meta->ReadWord();
         Width=meta->ReadWord();
         meta->Read(&rgb,sizeof(rgb));
         Rop2=meta->ReadWord();
         if (CreatePen(Style,Width,rgb,Rop2))return 0;
        break;
        }
case  mfsetpenstyle:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if (SetPenStyle(w1,w2))return 0;
        break;
case  mfsetpenwidth:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if (SetPenWidth(w1,w2))return 0;
        break;
case  mfsetpencolor:{
        obj=meta->ReadWord();
        COLORREF rgb;
        meta->Read(&rgb,sizeof(rgb));
        if (SetPenColor(obj,rgb))return 0;
        break;}
case  mfsetpenrop2:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if (SetPenRop2(w1,w2))return 0;
        break;
// brush
case  mfcreatebrush:{
         COLORREF rgb;
         INT16 Style,Hatch,Rop2,DibIndex;
         Style=meta->ReadWord();
         Hatch=meta->ReadWord();
         meta->Read(&rgb,sizeof(rgb));
         DibIndex=meta->ReadWord();
         Rop2=meta->ReadWord();
         if(CreateBrush(Style,Hatch,rgb,DibIndex,Rop2))return 0;
        break;}
case  mfsetbrushstyle:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if (SetBrushStyle(w1,w2))return 0;
        break;
case  mfsetbrushhatch:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if (SetBrushHatch(w1,w2))return 0;
        break;
case  mfsetbrushdibindex:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if (SetBrushDibIndex(w1,w2))return 0;
        break;
case  mfsetbrushcolor:{
        obj=meta->ReadWord();
        COLORREF rgb;
        meta->Read(&rgb,sizeof(rgb));
        if (SetBrushColor(obj,rgb))return 0;
        break;}
case  mfsetbrushrop2:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if (SetBrushRop2(w1,w2))return 0;
        break;
// dib
case  mfcreatedib:{
        PDib2d pd=(PDib2d)dibs->GetItem(meta);
        int h=pd->_HANDLE;
        if (dibs->InsertObject(pd)==h)return 0;
        break;}
case  mfsetdib:{
        w1=meta->ReadWord();
        PDib2d pd=(PDib2d)dibs->GetItem(meta);
        TDib2d d(meta,0);
        if (d.dib)
         {d.SetFreeDib(FALSE);
          if (SetDib(w1,d.dib)) return 0;
          d.SetFreeDib(TRUE);
        }
        break;};
case  mfrcreatedib:{
        char * name=meta->ReadStr();
        int h=CreateRDib2d(name);
        delete name;
        if (h)return 0;
        break;}
case  mfrsetdib:{
        w1=meta->ReadWord();
        char * name=meta->ReadStr();
        int h=SetRDib2d(w1,name,NULL);
        delete name;
        if (h)return 0;
        break;}
// doubledib
case  mfcreatedoubledib:{
        PDoubleDib2d pd=(PDoubleDib2d)ddibs->GetItem(meta);
        int h=pd->_HANDLE;
        if (ddibs->InsertObject(pd)==h)return 0;
        break;}
case  mfsetdoubledib:{
        w1=meta->ReadWord();
        TDoubleDib2d d(meta,0);
        if (d.dib)
         {d.SetFreeDib(FALSE);
          if (SetDoubleDib(w1,d.dib,d.andbitmap->dib)) return 0;
          d.SetFreeDib(TRUE);
        }
        break;};

case  mfrcreatedoubledib:{
        char * name=meta->ReadStr();
        int h=CreateRDoubleDib2d(name);
        delete name;
        if (h)return 0;
        break;}
case  mfrsetdoubledib:{
        w1=meta->ReadWord();
        char * name=meta->ReadStr();
        int h=SetRDoubleDib2d(w1,name,NULL);
        delete name;
        if (h)return 0;
        break;}

// font
case  mfcreatefont:{
//		LOGFONT2D LogFont;
//		meta->Read(&LogFont,sizeof(LOGFONT));
//		if(CreateFont2d(&LogFont))return 0;
        break;}
case  mfsetlogfont:{
//		LOGFONT LogFont;
//		obj=meta->ReadWord();
//		meta->Read(&LogFont,sizeof(LOGFONT));
//		if(SetLogFont2d(obj,(&LogFont)))return 0;
        break;}
// string
case  mfcreatestring:{
        char * s=meta->ReadStr();
        int h=(CreateString2d(s));
        delete s;
        if(h)return 0;
        break;
        }
case  mfsetlogstring:{
        obj=meta->ReadWord();
        char * s=meta->ReadStr();int h;
        h=SetLogString2d(obj,s);
        delete s;
        if(h)return 0;
        break;}
// text
case  mfcreatetext:{
        INT16 NumItems=meta->ReadWord();
        LOGTEXT * Text=new LOGTEXT[NumItems];
        meta->Read(Text,sizeof(LOGTEXT)*NumItems);
        HOBJ2D h=CreateText(Text,NumItems );
        delete Text;
        if (h)return 0;
        break;}
case  mfsetlogtext:{
        HOBJ2D index=meta->ReadWord();
        INT16 NumItems=meta->ReadWord();
        LOGTEXT * Text=new LOGTEXT[NumItems];
        meta->Read(Text,sizeof(LOGTEXT)*NumItems);
        BOOL h=SetLogText2d(index,Text,NumItems);
        delete Text;
        if (h)return 0;
        break;}
// group2d
case  mfcreategroup2d:{
        w1=meta->ReadWord();
        HOBJ2D * objects=new HOBJ2D[w1];
        meta->Read(objects,sizeof(HOBJ2D)*w1);
        w2=CreateGroup(objects,w1);
        delete objects;
        if (w2)return 0;
        break;}
case  mfdeletegroup2d:
        if (DeleteGroup(meta->ReadWord()))return 0;
        break;
case  mfdeletegroupitem2d:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if (DeleteFromGroup(w1,w2))return 0;
        break;
case  mfsetgroupitems2d:
        {w1=meta->ReadWord();
         w2=meta->ReadWord();
         HOBJ2D * objects=new HOBJ2D[w2];
         meta->Read(objects,sizeof(int)*w2);
         b1=SetGroup(w1,objects,w2);
         delete objects;
         if (b1)return 0;
         break;}
case  mfsetgroupitem2d:
        w1=meta->ReadWord();w2=meta->ReadWord();w3=meta->ReadWord();
        if(SetGroupItem(w1,w2,w3))return 0;
        break;
case  mfaddgroupitem2d:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if(AddToGroup(w1,w2))return 0;
        break;
// RGN group
case  mfcreatergngroup2d:{
        w1=meta->ReadWord();
        RGNGROUPITEM * objects=new RGNGROUPITEM[w1];
        meta->Read(objects,sizeof(RGNGROUPITEM)*w1);
        w2=CreateRgnGroup(objects,w1);
        delete objects;
        if (w2)return 0;
        break;}
case  mfsetrgngroupitems:
        {w1=meta->ReadWord();
         w2=meta->ReadWord();
         RGNGROUPITEM * objects=new RGNGROUPITEM[w2];
         meta->Read(objects,sizeof(RGNGROUPITEM)*w2);
         b1=SetRgnGroupItems(w1,objects,w2);
         delete objects;
         if (b1)return 0;
         break;}
case  mfsetrgngroupitem:{
        w1=meta->ReadWord();w2=meta->ReadWord();
        RGNGROUPITEM item;meta->Read(&item,sizeof(RGNGROUPITEM));
        if(SetRgnGroupItem(w1,w2,&item))return 0;}
        break;
case  mfaddrgngroupitem:{
        w1=meta->ReadWord();w2=meta->ReadWord();
        RGNGROUPITEM item;meta->Read(&item,sizeof(RGNGROUPITEM));
        if(AddRgnGroupItem(w1,w2,&item))return 0;}
        break;
case  mfcreate3dspace:
        if(CreateSpace3d())return 0;
        break;
case  mfloadspace3d:
        {PSpace3d ps=(PSpace3d)meta->Get(17459);
         if (ps){spaces3d->Insert(ps);return 0;}
         break;
        }
// Objects
case  mfloadobject__:{
        PObject po=__ReadObject(meta);
        if (po){AddObjectRect((PObject2d)po);return 0;}
        break;
        }
// Objects2d
// all 2d objects
case  mfdeleteobject2d:
        if (DeleteObject2d(meta->ReadWord()))return 0;
        break;
case  mfsetobjectorg2d:
        obj=meta->ReadWord();
        meta->ReadPOINT(&point);
        if (SetObjectOrg2d(obj,point))return 0;
        break;
case  mfsetobjectsize2d:
        obj=meta->ReadWord();
        meta->ReadPOINT(&point);
        if (SetObjectSize2d(obj,point))return 0;
        break;
case  mfrotateobject2d:
        obj=meta->ReadWord();
        ROTATOR2D rotator;
        meta->Read(&rotator,sizeof(ROTATOR2D));
        if (RotateObject2d(obj,&rotator))return 0;
        break;
case  mfcoordinates:{
        obj=meta->ReadWord();
        PObject2d po;
        po=(PObject2d)GetObjByHANDLE(obj);
        if (po){Lock();
        AddObjectRect(po);po->LoadCoordinates(meta);
        UpdateFrame(po);
        AddObjectRect(po);UnLock();return 0;}
        break;
        }
case mfloadobject2d:
        {
         break;
        }
case  mfsetbkbrush:
        if (SetBkBrush(meta->ReadWord())) return 0;
        break;
//case  mfsetlayercolor:break;

//case  mfsetcurrentobj2d:break;
case  mfsetattribute2d:{
        obj=meta->ReadWord();
        PObject2d po;
        po=(PObject2d)GetObjByHANDLE(obj);
        UINT16 Attr;Attr=meta->ReadWord();
        if (po){po->options=(po->options & 0x00ffu)||(Attr & 0xff00u);return 0;}
        break;
        }
//case  mfsetobject2dptr:break;

// polyline
case  mfcreatepolyline:{
        INT16 ip,ib,pc;
        POINT2D * points;
        ip=meta->ReadWord();
        ib=meta->ReadWord();
        pc=meta->ReadWord();
        points = new POINT2D[pc];
        meta->ReadPOINTs(points,pc);
        HOBJ2D h=CreatePolyline2d(ip,ib,points,pc);
        delete points;
        if (h)return 0;
        break;
        }
case  mfaddvectorpoint:{
        obj=meta->ReadWord();
        INT16 pn;POINT2D point;
        pn=meta->ReadWord();
        meta->ReadPOINT(&point);
        if (AddVectorPoint2d(obj,pn,point))return 0;
        break;
        }
case  mfdelvectorpoint:{
        POINT2D point;
        w1=meta->ReadWord();w2=meta->ReadWord();
        if (DelVectorPoint2d(w1,w2,point))return 0;
        }
        break;
case  mfsetvectorpoints:{
        w1=meta->ReadWord();w2=meta->ReadWord();
        POINT2D * points=new POINT2D[w2];
        meta->ReadPOINTs(points,w2);
        b1=SetVectorPoints2d(w1,points,w2);
        delete points;
        if (b1)return 0;
        break;
        }
case  mfsetvectorpoint:{
         obj=meta->ReadWord();
         INT16 pn;POINT2D point;
         pn=meta->ReadWord();
         meta->ReadPOINT(&point);
        if (SetVectorPoint2d(obj,pn,point))return 0;
        break;}
case  mfsetrgncreatemode:
        w1=meta->ReadWord();w2=meta->ReadWord();
         if(SetRgnCreateMode(w1,w2))return 0;
        break;
case  mfsetpenobject:
        w1=meta->ReadWord();w2=meta->ReadWord();
         if(SetVectorPen2d(w1,w2))return 0;
        break;
case  mfsetbrushobject:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if(SetVectorBrush2d(w1,w2))return 0;
        break;
// all bitmasps (double and single)
case  mfsetbitmapsrcrect:{
         obj=meta->ReadWord();
         POINT2D SrcOrg,SrcSize;
         meta->ReadPOINT(&SrcOrg);
         meta->ReadPOINT(&SrcSize);
         if (SetBitmapSrcRect(obj,SrcOrg,SrcSize))return 0;
        break;}
// bitmap
case  mfcreatebitmap:{
         POINT2D SrcOrg,SrcSize,DstSize,DstOrg;
         INT16 angle;
         HOBJ2D DibIndex;
         DibIndex=meta->ReadWord();
         meta->ReadPOINT(&SrcOrg);
         meta->ReadPOINT(&SrcSize);
         meta->ReadPOINT(&DstOrg);
         meta->ReadPOINT(&DstSize);
         angle=meta->ReadWord();
         if(CreateBitmap2d(DibIndex,SrcOrg,SrcSize,
                                DstOrg,DstSize,angle))return 0;
        break;}
case  mfsetdibobject:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if(SetDibObject2d(w1,w2))return 0;break;
// double
case  mfcreatedoublebitmap:{
         POINT2D SrcOrg,SrcSize,DstSize,DstOrg;
         INT16 angle;
         HOBJ2D DoubleDibIndex;
         DoubleDibIndex=meta->ReadWord();
         meta->ReadPOINT(&SrcOrg);
         meta->ReadPOINT(&SrcSize);
         meta->ReadPOINT(&DstOrg);
         meta->ReadPOINT(&DstSize);
         angle=meta->ReadWord();
         if(CreateDoubleBitmap2d(DoubleDibIndex,SrcOrg,SrcSize,
                                      DstOrg,DstSize,angle))return 0;
        break;}
case  mfsetdoubledibobject:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if(SetDoubleDibObject2d(w1,w2))return 0;break;
// text
case  mfcreatetextraster:{
         POINT2D Org,Size,Delta;
         INT16 DstAngle;
         HOBJ2D TextIndex;
         TextIndex=meta->ReadWord();
         meta->ReadPOINT(&Org);
         meta->ReadPOINT(&Size);
         meta->ReadPOINT(&Delta);
         DstAngle=meta->ReadWord();
         if(CreateTextRaster2d(TextIndex,Org,Size,
                                            Delta,DstAngle ))return 0;
    break;}
case  mfsettextobject:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if(SetTextObject2d(w1,w2))return 0;
        break;
case mfsettextdelta:
        w1=meta->ReadWord();
        POINT2D delta;
        meta->ReadPOINT(&delta);
        if (SetTextDelta2d(w1,delta))return 0;
// 3d projection
case  mfcreate3dprojection:{
         POINT2D DstOrg,DstSize;
         HSP3D hSpace3d=(HSP3D)meta->ReadWord();
         meta->ReadPOINT(&DstOrg);
         meta->ReadPOINT(&DstSize);
         HOBJ2D camera=meta->ReadWord();
         //(hSpace3d | (_HANDLE << 8))
         if(Create3dProjection2d(0, DstOrg,DstSize,camera))return 0;
        break;}
case  mfset3dspace2d:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if(Set3dSpace2d(w1,NULL))return 0;
        break;
// Z order
case  mfZtotop:
        if (ObjectToEnd2d(meta->ReadWord(),1))return 0;
        break;
case  mfZtobottom:
        if (ObjectToEnd2d(meta->ReadWord(),0))return 0;
        break;
case  mfZswap:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if(SwapObject(w1,w2))return 0;
        break;
case  mfZset:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if(SetZOrder(w1,w2))return 0;
        break;
/*
case  mfset3dviewparam:
        {INT16 w1=meta->ReadWord();
         PROJECTOR3D p;
         meta->Read(&p,sizeof(p.poSize));
            meta->Read(&p.poType,p.poSize-sizeof(p.poSize));
         if(Set3dViewProjector(w1,&p))return 0;
         break;
        }
 default:
    _Error(EM_metarecord);
 return 1;
 }else{

int sp3=0;
meta->Read(&sp3,1);
//sp3=sp3|_HANDLE<8;
PSpace3d space3d=NULL;
//(PSpace3d)spaces3d->GetByHandle(sp3);
if (space3d==NULL) {_Error(EM_processmetaerror);return 1;}
switch (code){
 // 3d

case  mfdeletespace3d:
 //		if (DeleteSpace3d(sp3))return 0;
        break;
case  mfaddpoint3d:
        {w1=meta->ReadWord();
         POINT3D p;meta->Read(&p,sizeof(p));
         if(space3d->AddPoint3d(w1,&p)>-1) return 0;
        }break;
case  mfdelpoint3d:
        w1=meta->ReadWord();
        w2=meta->ReadWord();
        if (space3d->DelPoint3d(w1,w2))return 0;
        break;
case  mfdeleteunusedpoint3d:{
        if (space3d->DelUnusedPoints3d(meta->ReadWord()))return 0;
        }
        break;
case  mfsetpoint3d:
        {w1=meta->ReadWord();
         w2=meta->ReadWord();
         POINT3D p;meta->Read(&p,sizeof(p));
         if(space3d->SetPoint3d(w1,w2,&p)) return 0;
        }break;
case  mfsetpoints3d:
        {w1=meta->ReadWord();
         w2=meta->ReadWord();
         POINT3D *p=new POINT3D[w2];
         meta->Read(p,sizeof(POINT3D)*w2);
         b1=space3d->SetPoints3d(w1,p,w2);
         delete p;
         if (b1)return 0;
        }break;
case mfloadpoint3d:
      {PObject3d po=(PObject3d)space3d->all->GetByHandle(meta->ReadWord());
        if (po){
        PPoint3d p=(PPoint3d)po->points->At(meta->ReadWord());
        space3d->UpdateObject(po);
        TPoint3d t(meta);p->x=t.x;p->y=t.y;p->z=t.z;
        po->UpdateSize();
        space3d->UpdateObject(po);
        return 0;
        }
      }break;
case mfloadpoints3d:
      {PObject3d po=(PObject3d)space3d->all->GetByHandle(meta->ReadWord());
        if (po){
        space3d->UpdateObject(po);
        w2=meta->ReadWord();
        for(INT16 i=0;i<w2;i++)
         {PPoint3d p=(PPoint3d)po->points->At(i);
          TPoint3d t(meta);p->x=t.x;p->y=t.y;p->z=t.z;
         }
        po->UpdateSize();space3d->UpdateObject(po);
        return 0;
        }
      }break;
case mfloadunusedpoints3d:
        {PObject3d po=(PObject3d)space3d->all->GetByHandle(meta->ReadWord());
         if (po){
         do
         {w2=meta->ReadWord();
          if (w2>-1)
            {po->points->AtInsert(w2,new TPoint3d(meta));
            }
         }while (w2>-1); return 0;}
        }break;
case  mfaddline3d:{
        w1=meta->ReadWord();
        LINE3D l;meta->Read(&l,sizeof(l));
        if (space3d->AddLine3d(w1,&l)>-1)return 0;
        }
        break;
case  mfdelline3d:
        w1=meta->ReadWord();
        w2=meta->ReadWord();
        if (space3d->DelLine3d(w1,w2))return 0;
        break;
case  mfsetline3d:
        {w1=meta->ReadWord();
         w2=meta->ReadWord();
         LINE3D l;meta->Read(&l,sizeof(l));
         if (space3d->SetLine3d(w1,w2,&l))return 0;
        }
        break;
case  mfsetlines3d:
        {w1=meta->ReadWord();
         w2=meta->ReadWord();
         LINE3D *l=new LINE3D[w2];
         meta->Read(l,sizeof(LINE3D)*w2);
         b1=space3d->SetLines3d(w1,l,w2);
         delete l;
         if (b1)return 0;
        }break;
case  mfaddtriangle3d:{
        w1=meta->ReadWord();
        TRIANGLE3D triangle;
        meta->Read(&triangle,sizeof(TRIANGLE3D));
        if(space3d->AddTriangle3d(w1,&triangle)>-1)return 0;
        break;}
case  mfdeltriangle3d:
        w1=meta->ReadWord();w2=meta->ReadWord();
        if (space3d->DelTriangle3d(w1,w2))return 0;
        break;
case  mfsettriangle3d:
        {w1=meta->ReadWord();
         w2=meta->ReadWord();
         TRIANGLE3D t;meta->Read(&t,sizeof(t));
         if (space3d->SetTriangle3d(w1,w2,&t))return 0;
        }
        break;
case  mfsettriangles3d:
        {w1=meta->ReadWord();
         w2=meta->ReadWord();
         TRIANGLE3D *t=new TRIANGLE3D[w2];
         meta->Read(t,sizeof(TRIANGLE3D)*w2);
         b1=space3d->SetTriangles3d(w1,t,w2);
         meta->Read(t,sizeof(TRIANGLE3D)*w2);
         delete t;
         if (b1)return 0;
        }break;
case  mfloadtriangle3d:{
        w1=meta->ReadWord();
        w2=meta->ReadWord();
        PObject3d po=(PObject3d)space3d->GetObjByHANDLE(w1);
        if (po){
            po->ReadTriangle(meta,w2);
            PTriangle pt=(PTriangle)po->triangles->At(w2);
            space3d->UpdateTriangle(po,pt);
        }
        break;}
case  mfloadtriangles3d:
        {w1=meta->ReadWord();
         w2=meta->ReadWord();
        PObject3d po=(PObject3d)space3d->GetObjByHANDLE(w1);
        if (po){
        space3d->UpdateObject(po);
        po->triangles->FreeAll();
        int i;
        for (i=0;i<w2;i++)po->ReadTriangle(meta);
        space3d->UpdateObject(po);
        }return 0;
        }
case  mfcreategroup3d:{
        w1=meta->ReadWord();
        HOBJ2D * objects=new HOBJ2D[w1];
        meta->Read(objects,sizeof(int)*w1);
        w2=space3d->CreateGroup(objects,w1);
        delete objects;
        if (w2)return 0;
        break;}
case  mfdeletegroup3d:
         if (space3d->DeleteGroup(meta->ReadWord()))return 0;
         break;
case  mfcreateobject3d:{
         int NumPoints,NumLines,NumTriangles;
         POINT3D * points=NULL;
         LINE3D  * lines=NULL;
         TRIANGLE3D  * triangles=NULL;
         NumPoints=meta->ReadWord();
         if (NumPoints){
         points=new POINT3D[NumPoints];meta->Read(points,sizeof(POINT3D)*NumPoints);
         }
         NumLines=meta->ReadWord();
         if (NumLines){
         lines=new LINE3D[NumLines];
         meta->Read(lines,sizeof(LINE3D)*NumLines);
         }
         NumTriangles=meta->ReadWord();
         if (NumTriangles){
         triangles=new TRIANGLE3D[NumTriangles];
         meta->Read(triangles,sizeof(TRIANGLE3D)*NumTriangles);
         }
         b1=space3d->CreateObject3d(points,NumPoints,lines,NumLines,
             triangles,NumTriangles );
         if (NumTriangles)delete triangles;
         if (NumLines)delete lines;
         if (NumPoints)delete points;
        if (b1)return 0;
        break;}
case  mfdeleteobject3d:
        if (space3d->DeleteObject3d(meta->ReadWord()))return 0;
        break;
case  mfsetcurrentobject3d:break;
case  mfsetobjectsize3d:{
         w1=meta->ReadWord();
         POINT3D size;meta->Read(&size,sizeof(POINT3D));
         if (space3d->SetObjectSize3d(w1,&size))return 0;
         break;
        }
case  mfrotateobject3d:{
        w1=meta->ReadWord();
        ROTATOR3D r3d;meta->Read(&r3d,sizeof(ROTATOR3D));
        if(space3d->RotateObject3d(w1,&r3d))return 0;
        break;}
case  mfsetobjectorg3d:{
        w1=meta->ReadWord();
        TPoint3d org(meta);
        if(space3d->SetObjectOrigin3d(w1,&org))return 0;
        break;
        }
case  mfdeleteemptyobjects3d:
        space3d->DeleteEmptyObjects();
        return 0;
case  mfloadobject3d:
        {PObject po=space3d->__ReadObject(meta);
          if (po){
          space3d->UpdateObject((PObject3d)po);
          return 0;
          }
        break;
        }
case mfloadcoordinates3d:{
 PObject3d po=(PObject3d)space3d->GetObjByHANDLE(meta->ReadWord());
 if (po){
 space3d->UpdateObject(po);
 po->LoadCoordinates(meta);
 space3d->UpdateObject(po);
 return 0;}
 break;
 }
case mfpasteobject3d:
 {POINT3D p3;POINT3D * pp=0;
    if (meta->ReadWord()){meta->Read(&p3,sizeof(p3));pp=&p3;}
    if (space3d->PasteObject(meta,pp)) return 0;
 }break;
case mfsetattribute3d:{
w1=meta->ReadWord();//object3d
w2=meta->ReadWord();//attr
w3=meta->ReadWord();//mode
PObject3d po=(PObject3d)space3d->GetObjByHANDLE(w1);
if(po){po->SetState(w2,(ATTR_MODE)w3,space3d);return 0;}
}break;
case mfsetcolor3d:{
w1=meta->ReadWord();
COLORREF c;meta->Read(&c,sizeof(c));
if (space3d->SetObjectColor3d(w1,c))return 0;
}break;
case mfloadcolor3d:if(space3d->LoadColors3d(meta))return 0;
                         break;
 default:
    _Error(EM_metarecord);
 return 1;
}
}
 char s[128];
 wsprintf(s,"Error while processing meta record.\n Meta Record code:%d",code);
 MessageBox(0,s,"Graphic Dll",MB_TASKMODAL|MB_ICONHAND);
 //	_Error(EM_processmetaerror);
 */
    return 1;
};
//*-----------------------------------------------*
TSpace2d::TSpace2d(PStream ps) //: _SPACE(ps)
{
#ifdef USECAIRO
    RenderEngine=1;
#else
    RenderEngine=0;
#endif
    hwnd=0;
    SetDefault();
    path = NewStr(currentpath);
    txtPath = NewStr(texturepath);

    long fpos = ps->GetPos();
    ps->fileversion=ps->ReadWord();
    int mver = ps->ReadWord();

    if (mver <= currentfileversion)
    {
        if(ps->fileversion >= 0x0300)
        {
            CHUNK chunk;
            chunk.InitRead(ps);

            ps->ReadPOINT(&org);
            ps->ReadPOINT(&scale_div);
            ps->ReadPOINT(&scale_mul);

            State=ps->ReadWord();
            BkBrush=(PBrush2d)(void*)MAKELONG(ps->ReadWord(),0);

            ps->Read(&layers,4);
            defaultflags=ps->ReadWord();

            while(chunk._pos>ps->GetPos() && (ps->status==0))
            {
                CHUNK _chunk;
                _chunk.InitRead(ps);
                PObject p=(PObject)(ps->AllocateObject(_chunk));
                if(p)
                {
                    switch(p->WhoIsIt())
                    {
                        case 1004: pens=(PPenCollection)p;break;
                        case 1005:bruhs=(PBrushCollection)p;break;
                        case 1006:dibs=(PDibCollection)p;break;
                        case 1007:ddibs=(PDoubleDibCollection)p;break;

                        case 1008:
                            fonts = (PFontCollection)p;
                            break;

                        case 1009:
                            strings=(PStringCollection)p;
                            break;

                        case 1010:
                            texts = (PTextCollection)p;
                            break;

                        case 1012:spaces3d=(P3dCollection)p;break;

                        case otPRIMARYCOLLECTION: primary = (PPrimaryCollection)p;break;
                        case otOBJECTCOLLECTION: all    =(PObjectCollection)p;break;
                        case otEDITFRAME2D: (PEditFrame)p;break;
                        case otROTATECENTER2D:(PRotateCenter)p;break;
                        case otFRAME3D       :break;
                        case otAXIS3D        :break;
                        case otDATAITEMS:dataitems=(TDataItems*)p;break;
                    };
                }
                else
                {
                    switch(_chunk.code)
                    {
                        case ctINFONAME :name=chunk.ReadStr();break;
                        case ctINFOAUTOR:author=chunk.ReadStr();break;
                        case ctINFOINFO :description=chunk.ReadStr();break;
                        case ctCRDSYSTEM:
                        {
                            CRDSYSTEM2D c;
                            if(_chunk.size!=sizeof(c))
                            {
                                memset(&c,0,sizeof(c));
                            }
                            ps->Read(&c,sizeof(c));
                            SetCrd(&c);
                        }break;
                    }
                }
                _chunk.Check();
            }
            AddCollection();
        }
        else
        {
            // чтение старого формата файла
            long toolspos, mainpos;
            ps->Read(&mainpos, sizeof(mainpos));
            mainpos += fpos;

            ps->Read(&toolspos, sizeof(toolspos));
            toolspos += fpos;

            ps->ReadPOINT(&org);
            ps->ReadPOINT(&scale_div);
            ps->ReadPOINT(&scale_mul);
            {
                char * _path = ps->ReadStr();
                if (_path)
                    delete _path;
            }
            State=ps->ReadWord();
            name = ps->ReadStr();
            author = ps->ReadStr();
            description=ps->ReadStr();

            RCenter = (PRotateCenter)ps->Get();
            Frame = (PEditFrame)ps->Get();
            BkBrush = (PBrush2d)(void*)MAKELONG(ps->ReadWord(),0);

            ps->Seek(mainpos);
            primary = (PPrimaryCollection) ps->Get();
            all =   (PObjectCollection)ps->Get();

            ps->Seek(toolspos);
            int cc=ps->ReadWord();
            int i;
            for(i = 0; i < cc; i++)
            {
                PObject p=(PObject)ps->Get();
                switch(p->WhoIsIt())
                {
                    case 1004:pens=(PPenCollection)p;break;
                    case 1005:bruhs=(PBrushCollection)p;break;
                    case 1006:dibs=(PDibCollection)p;break;
                    case 1007:ddibs=(PDoubleDibCollection)p;break;
                    case 1008:fonts=(PFontCollection)p;break;
                    case 1009:strings=(PStringCollection)p;break;
                    case 1010:texts=(PTextCollection)p;break;
                    case 1012:spaces3d=(P3dCollection)p;break;
                }
            }
            AddCollection();

            if (ps->fileversion > 0x0102)
            {
                ps->Read(&layers,4);
                defaultflags=ps->ReadWord();
                INT16 code=ps->ReadWord();
                while (code)
                {
                    switch(code)
                    {
                        case 1:
                        {
                            HOBJ2D h=ps->ReadWord();
                            PObject po = (PObject)all->GetByHandle(h);
                            po->dataitems = (TDataItems*)ps->Get(1022);
                        }break;

                        case 2:
                        {
                            dataitems = (TDataItems*)ps->Get(1022);
                        }break;

                        case 3:
                        {
                            INT16 size=ps->ReadWord();
                            CRDSYSTEM2D c;
                            if(size != sizeof(c))
                            {
                                memset(&c,0,sizeof(c));
                            }
                            ps->Read(&c,size);
                            SetCrd(&c);
                            //crd_used=TRUE;
                        }break;
                    };
                    code = ps->ReadWord();
                }
            }

            char * chk=NULL;
            chk = ps->ReadStr();
            if ((!lstrcmp(chk,fileend))&&(_NeedAwaken))
            {}
            else
                _Error(EM_invalidstatefile);

            if (chk)
                delete (chk);
        }

        TSpace2d::currentpath=NULL;
        if (!Awaken())
        {
            DeleteCollection(all);
            if (primary)
                primary->DeleteAll();
            DeleteCollection(primary);

            all=NULL;
            primary=NULL;
            _Error(EM_awakenerror);
        }

    }
    UpdateLowScale();
};

BOOL  TSpace2d::Awaken()
{
    C_TYPE i;
    if (!_Awaken())
        return 0;

    // Dib
    for(i=0;i<dibs->count;i++)
    {
        PDib2d pd=(PDib2d)dibs->At(i);
        if (!pd->Awaken(this))
            return 0;
    }
    // DoubleDib
    for(i=0;i<ddibs->count;i++)
    {
        PDoubleDib2d pd=(PDoubleDib2d)ddibs->At(i);if (!pd->Awaken(this))return 0; }
    // Brush
    for(i=0;i<bruhs->count;i++)
    {
        PBrush2d pb=(PBrush2d)bruhs->At(i);
        if (!pb->Awaken(this))
            return 0;
    }
    // Texts
    for(i=0;i<texts->count;i++)
    {PText2d pt=(PText2d)texts->At(i);if (!pt->Awaken(this))return 0; }

    for(i=0;i<all->count;i++)
        if (!((PObject2d)all->At(i))->Awaken(this))
            return 0;

    for(i=0;i<spaces3d->count;i++)((PSpace3d)spaces3d->At(i))->space2d=this;

    if (BkBrush)
    {
        BkBrush=(PBrush2d)bruhs->GetByHandle((HOBJ2D)BkBrush);
        if (BkBrush)BkBrush->IncRef(this);else return 0;
    }

    if (Frame){
        if(Frame->Awaken(this)) Frame->UpdateSize(0,this);
        else {
            _Error(EM_FrameError);
            delete Frame;Frame=NULL;
        }
    }
    if (RCenter)
        RCenter->UpdateSize(0,this);
    if (Frame3d)
    {if(Frame3d->Awaken(this))Frame3d->UpdateSize(0,this);
        else return 0;
    }
    if (Axis3d)
    {if(Axis3d->Awaken(this)) Axis3d->UpdateSize(0,this);
        else return 0;
    }
    UpdateSize();
    return 1;
};

void TSpace2d::UpdateSize()
{
    C_TYPE i;
    for(i=0;i<primary->count;i++)
    {
        if(((PObject2d)all->At(i))->WhoIsIt()==23)
        {
            //POINT2D size=CalcTextSize((PText_2d)((PObject2d)all->At(i)));
            //((PObject2d)all->At(i))->SetSize2d(size);
            //MessageBox(0,"asdfwe","awef",0);
            //SetObjectSize2d(0,size);
            ((PObject2d)all->At(i))->UpdateSize(0,this);
        }
        else
            ((PObject2d)all->At(i))->UpdateSize(0,this);
    }
    for(i=0;i<fonts->count;i++)
        ((TFont2d*)fonts->At(i))->tmValid=0;
    if (Frame)Frame->UpdateSize(0,this);
    if (RCenter)RCenter->UpdateSize(0,this);
    if (Frame3d)Frame3d->UpdateSize(0,this);
    if (Axis3d)Axis3d->UpdateSize(0,this);
};

void TSpace2d::_LpToDp(POINT2D FAR* lppt,int cPoints)
{
    int i;
    for (i=0;i<cPoints;i++)
    {
        lppt[i].x=(((lppt[i].x)*scale_mul.x)/scale_div.x)-org.x;
        lppt[i].y=(((lppt[i].y)*scale_mul.y)/scale_div.y)-org.y;
    }
};

void TSpace2d::_DpToLp(POINT2D FAR* lppt,int cPoints)
{
    int i;
    for (i=0;i<cPoints;i++)
    {
        lppt[i].x=((((lppt[i].x+org.x)*scale_div.x))/scale_mul.x);
        lppt[i].y=((((lppt[i].y+org.y)*scale_div.y))/scale_mul.y);
    }
}

void TSpace2d::_LpToDpWO(POINT2D FAR* lppt,int cPoints)
{
    int i;
    for (i=0;i<cPoints;i++){
        lppt[i].x=((((lppt[i].x)*scale_mul.x))/scale_div.x);
        lppt[i].y=((((lppt[i].y)*scale_mul.y))/scale_div.y);
    }
};
void TSpace2d::_DpToLpWO(POINT2D FAR* lppt,int cPoints)
{ int i;
    for (i=0;i<cPoints;i++){
        lppt[i].x=((((lppt[i].x)*scale_div.x))/scale_mul.x);
        lppt[i].y=((((lppt[i].y)*scale_div.y))/scale_mul.y);
    }
};

void TSpace2d::_LpToDp(POINT FAR* lppt,int cPoints)
{
    int i;
    for (i=0;i<cPoints;i++){
        lppt[i].x=(int)(((long)((long)(lppt[i].x)*(long)scale_mul.x))/(long)scale_div.x)-org.x;
        lppt[i].y=(int)(((long)((long)(lppt[i].y)*(long)scale_mul.y))/(long)scale_div.y)-org.y;
    }
};
void TSpace2d::_DpToLp(POINT FAR* lppt,int cPoints)
{ int i;
    for (i=0;i<cPoints;i++){
        lppt[i].x=(int)(((long)((long)(lppt[i].x+org.x)*(long)scale_div.x))/(long)scale_mul.x);
        lppt[i].y=(int)(((long)((long)(lppt[i].y+org.y)*(long)scale_div.y))/(long)scale_mul.y);
    }
};
void TSpace2d::_LpToDpWO(POINT FAR* lppt,int cPoints)
{
    int i;
    for (i=0;i<cPoints;i++){
        lppt[i].x=(int)(((long)((long)(lppt[i].x)*(long)scale_mul.x))/(long)scale_div.x);
        lppt[i].y=(int)(((long)((long)(lppt[i].y)*(long)scale_mul.y))/(long)scale_div.y);
    }
};
void TSpace2d::_DpToLpWO(POINT FAR* lppt,int cPoints)
{ int i;
    for (i=0;i<cPoints;i++){
        lppt[i].x=(int)(((long)((long)(lppt[i].x)*(long)scale_div.x))/(long)scale_mul.x);
        lppt[i].y=(int)(((long)((long)(lppt[i].y)*(long)scale_div.y))/(long)scale_mul.y);
    }
};
void TSpace2d::Store(PStream ps)
{
    long fpos=ps->GetPos();
    ps->WriteWord(ps->fileversion);//current file version
    ps->WriteWord(ps->fileversion);//minimum

    CHUNK chunk;
    chunk.InitWrite(ps,stSPACEDATA);

    ps->WritePOINT(&org);
    ps->WritePOINT(&scale_div);
    ps->WritePOINT(&scale_mul);
    //ps->WriteStr(path);
    ps->WriteWord(State);
    /*
    ps->WriteStr(name);
    ps->WriteStr(author);
    ps->WriteStr(description);
    */
    if ((BkBrush)&&(TCollection::fullstore))
        ps->WriteWord(BkBrush->_HANDLE);
    else ps->WriteWord(0);
    ps->Write(&layers,4);
    ps->WriteWord(defaultflags);

    if(RCenter && TCollection::fullstore)ps->Put(RCenter);
    if(Frame && TCollection::fullstore)ps->Put(Frame);

    if(primary->GetCountToSave())	ps->Put(primary);
    if(all->GetCountToSave())			ps->Put(all);
    if(pens->GetCountToSave())		ps->Put(pens);
    if(bruhs->GetCountToSave())		ps->Put(bruhs);
    if(dibs->GetCountToSave())		ps->Put(dibs);
    if(ddibs->GetCountToSave())		ps->Put(ddibs);
    if(fonts->GetCountToSave())		ps->Put(fonts);
    if(strings->GetCountToSave())	ps->Put(strings);
    if(texts->GetCountToSave())		ps->Put(texts);
    if(spaces3d->GetCountToSave())ps->Put(spaces3d);

    if (TCollection::fullstore)
    {
        if(dataitems)ps->Put(dataitems);
        if(crd_used)
        {
            CHUNK _chunk;_chunk.InitWrite(ps,ctCRDSYSTEM);
            ps->Write(&crd_system,sizeof(CRDSYSTEM2D));
            _chunk.DoneWrite();
        }
        CHUNK _chunk;_chunk.WriteStr(ps,ctINFONAME,name);
        _chunk.WriteStr(ps,ctINFOAUTOR,author);
        _chunk.WriteStr(ps,ctINFOINFO,description);
    }
    chunk.DoneWrite();
}


TSpace2d::TSpace2d(HWND w,LPSTR apath):_SPACE()
{
#ifdef USECAIRO
    RenderEngine=1;
#else
    RenderEngine=0;
#endif
    hwnd=w;
    SetDefault();
    AddCollection();
    path=NewStr(apath);
    UpdateLowScale();
}

void TSpace2d::SetDefault()
{
    activefont=NULL;
    _activefont=NULL;
    activefontangle=0;

    readonly=0;
    rectchange=0;
    dataitems=NULL;
    crd_used=FALSE;
    txtPath=NULL;
    State=0;
    org.x=0;
    org.y=0;
    lock=0;
    scale_mul.x=100;scale_mul.y=100;
    scale_div.x=100;scale_div.y=100;
    name=author=description=NULL;
    ischanged=0;modifyed=0;record=0;maxcolor=0;palette=0;
    record_flag=0;
    meta=NULL;filename=NULL;Frame=NULL;RCenter=NULL;BkBrush=NULL;
    path=NULL;
    Frame3d=NULL;Axis3d=NULL;
    _dragobj=-1;
    pens=NULL;bruhs=NULL;dibs=NULL;ddibs=NULL;
    fonts=NULL;strings=NULL;texts=NULL;
    spaces3d=NULL;
    layers=0;
};
void _MkTTosave(PToolsCollection pt)
{C_TYPE i;
    for (i=0;i<pt->count;i++)((PTools2d)pt->At(i))->flag&=(~rfmustsaved);
}
int TSpace2d::_StoreObject(PStream ps,HOBJ2D obj)
{
    C_TYPE i;
    PObject2d po;
    _MkTTosave(texts);
    _MkTTosave(bruhs);
    _MkTTosave(pens);
    _MkTTosave(dibs);
    _MkTTosave(ddibs);
    _MkTTosave(fonts);
    _MkTTosave(strings);
    for (i=0;i<spaces3d->count;i++)
        ((PSpace3d)spaces3d->At(i))->MarkToSave(0);
    for (i=0;i<all->count;i++)((PObject)all->At(i))->MarkToSave(0);
    //  for (i=0;i<all->count;i++)((PObject)all->At(i))->MarkToSave(0);
    po=(PObject2d)GetObjByHANDLE(obj);
    if (po)
    {po->MarkToSave(1);
        TCollection::fullstore=0;
        INT16 cur=all->current;
        all->current=obj;
        //ps->Put(this);

        ps->WriteWord(otSPACE2D);
        Store(ps);

        all->current=cur;
        TCollection::fullstore=1;
        return 1;
    }
    return 0;
};
HOBJ2D TSpace2d::MergeWithSpace(PStream ps,POINT2D delta,INT16 flags,int _m)
{
    TSpace2d::currentpath=path;
    TSpace2d::texturepath=txtPath;
    ChkPnt(delta);
    // PSpace2d ps2=(PSpace2d)ps->Get(17458);
    if(ps->ReadWord()!=otSPACE2D)
        return 0;

    PSpace2d ps2 = new TSpace2d(ps);
    if (ps2)
    {
        if (_m)
        {
            NewMetaRecord(mfmerge);
            meta->WritePOINT(&delta);
            meta->WriteWord(flags);
            meta->Put(ps2);
            NewUndoRecord(mfstartofgroup);
        }

        PObject2d po,cpo=NULL;
        C_TYPE i,j,k,l;

        if (ps2->all->current)
            cpo=(PObject2d)ps2->all->GetByHandle(ps2->all->current);

        //-------- Вставка объектов ------------------------///
        for(i = 0; i < ps2->all->count; i++)
            all->InsertObject(ps2->all->At(i));

        for (i=0; i < ps2->primary->count;i++)
        {
            po = (PObject2d)ps2->primary->At(i);
            int type=po->WhoIsIt();
            primary->Insert(po);
            switch(type)
            {
                case 24:
                    ((PView3d_2d)po)->space2d = this;
                    break;
                case 25:
                {
                    TUserObject2d*u=((TUserObject2d*)po);
                    u->data.handle=u->_HANDLE;
                    u->type->objproc(_HANDLE,&(u->data),OM_SPACEVALID,NULL);
                }break;

                case 26:
                    ((TControl2d*)po)->space = this;
                    if (hwnd)
                        ((TControl2d*)po)->SetParent(hwnd);
                    break;
            }
        };
        //-------- Объекты вставленны ----------------------///
        if(_m)
        {meta->WriteWord(mfdeleteobject2d);
            meta->WriteWord(cpo->_HANDLE);
        }
        //-------- Вставка Инструментов --------------------///
        // pen
        if (flags & PFC_PENS)
        {PPen2d p1,p2;
            for (i=0;i<ps2->pens->count;i++)
            {p1=(PPen2d)ps2->pens->At(i);j=0;
                while(j<pens->count && p1)
                {p2=(PPen2d)pens->At(j);
                    if (IsPensEqual(p1,p2))
                    {for (k=0;k<ps2->primary->count;k++)
                        {po=(PObject2d)ps2->primary->At(k);
                            if ((po->WhoIsIt()==20)&&(((PLine_2d)po)->pen==p1))
                            {((PLine_2d)po)->pen=p2;p2->IncRef(po);p1->DecRef(po);
                            }
                        }
                        delete p1;p1=NULL;
                    }else j++;
                }
                if (p1) // Если нет аналогов, то вставляем
                {
                    pens->InsertObject(p1);
                    if (_m)
                    {meta->WriteWord(mfdeletetool);
                        meta->WriteWord(1);
                        meta->WriteWord(p1->_HANDLE);
                    }
                }
            }
            ;}
        else
            for (i=0;i<ps2->pens->count;i++){
                PPen2d pp=(PPen2d)ps2->pens->At(i);
                pens->InsertObject(pp);
                if (_m)
                {meta->WriteWord(mfdeletetool);
                    meta->WriteWord(1);
                    meta->WriteWord(pp->_HANDLE);
                }
            }
        //dib
        if (flags & PFC_DIBS)
        {
            PDib2d p1,p2;
            for (i=0;i<ps2->dibs->count;i++)
            {
                p1=(PDib2d)ps2->dibs->At(i);
                j=0;
                while(j<dibs->count && p1)
                {
                    p2=(PDib2d)dibs->At(j);
                    if (IsDibsEqual(p1,p2))
                    {
                        for (k=0;k<ps2->primary->count;k++)
                        {
                            po=(PObject2d)ps2->primary->At(k);
                            if ((po->WhoIsIt()==21)&&(((PBitmap_2d)po)->dib==p1))
                            {
                                ((PBitmap_2d)po)->dib = p2;
                                p2->IncRef(po);p1->DecRef(po);
                            }
                        }
                        // Сейчас в brush`ах
                        for(k=0;k<ps2->bruhs->count;k++)
                        {
                            PBrush2d pb=(PBrush2d)ps2->bruhs->At(k);
                            if (pb->dib==p1)
                            {
                                pb->dib=p2;
                                p2->IncRef(pb);
                                p1->DecRef(pb);
                            }
                        }
                        delete p1;p1=NULL;
                    }else j++;
                }
                if (p1)
                {
                    dibs->InsertObject(p1);
                    if (_m)
                    {meta->WriteWord(mfdeletetool);
                        meta->WriteWord(3);
                        meta->WriteWord(p1->_HANDLE);
                    }
                }
            }
            ;}
        else
            for (i=0;i<ps2->dibs->count;i++)
            {PDib2d pd=(PDib2d)ps2->dibs->At(i);
                dibs->InsertObject(pd);
                if (_m)
                {meta->WriteWord(mfdeletetool);
                    meta->WriteWord(3);
                    meta->WriteWord(pd->_HANDLE);
                }
            };
        //ddib
        if (flags & PFC_DDIBS)
        {PDoubleDib2d p1,p2;
            for (i=0;i<ps2->ddibs->count;i++)
            {p1=(PDoubleDib2d)ps2->ddibs->At(i);j=0;
                while(j<ddibs->count && p1)
                {p2=(PDoubleDib2d)ddibs->At(j);
                    if (IsDDibsEqual(p1,p2))
                    {for (k=0;k<ps2->primary->count;k++)
                        {po=(PObject2d)ps2->primary->At(k);
                            if ((po->WhoIsIt()==22)&&(((PDoubleBitmap_2d)po)->dib==p1))
                            {((PDoubleBitmap_2d)po)->dib=p2;p2->IncRef(po);p1->DecRef(po);
                            }
                        }
                        delete p1;p1=NULL;
                    }else j++;
                }
                if (p1)
                {
                    ddibs->InsertObject(p1);
                    if (_m)
                    {meta->WriteWord(mfdeletetool);
                        meta->WriteWord(4);
                        meta->WriteWord(p1->_HANDLE);
                    }
                }
            }
            ;}
        else
            for (i=0;i<ps2->ddibs->count;i++){
                PDoubleDib2d pd=(PDoubleDib2d)ps2->ddibs->At(i);
                ddibs->InsertObject(pd);
                if (_m)
                {meta->WriteWord(mfdeletetool);
                    meta->WriteWord(4);
                    meta->WriteWord(pd->_HANDLE);
                }
            };
        //brush
        if (flags & PFC_BRUHS){
            PBrush2d p1,p2;
            for (i=0;i<ps2->bruhs->count;i++)
            {p1=(PBrush2d)ps2->bruhs->At(i);j=0;
                while(j<bruhs->count && p1)
                {p2=(PBrush2d)bruhs->At(j);
                    if (IsBrushsEqual(p1,p2))
                    {for (k=0;k<ps2->primary->count;k++)
                        {po=(PObject2d)ps2->primary->At(k);
                            if ((po->WhoIsIt()==20)&&(((PLine_2d)po)->brush==p1))
                            {((PLine_2d)po)->brush=p2;p2->IncRef(po);p1->DecRef(po);
                            }
                        }
                        delete p1;p1=NULL;
                    }else j++;
                }
                if (p1) // Если нет аналогов, то вставляем
                {
                    bruhs->InsertObject(p1);
                    if (_m)
                    {meta->WriteWord(mfdeletetool);
                        meta->WriteWord(2);
                        meta->WriteWord(p1->_HANDLE);
                    }
                }
            };}
        else
            for (i=0;i<ps2->bruhs->count;i++)
            {PBrush2d pb=(PBrush2d)ps2->bruhs->At(i);
                bruhs->InsertObject(pb);
                if (_m)
                {meta->WriteWord(mfdeletetool);
                    meta->WriteWord(2);
                    meta->WriteWord(pb->_HANDLE);
                }
            };
        //string
        if (flags & PFC_STRINGS){
            PString2d p1,p2;
            for (i=0;i<ps2->strings->count;i++)
            {p1=(PString2d)ps2->strings->At(i);j=0;
                while(j<strings->count && p1)
                {p2=(PString2d)strings->At(j);
                    if (IsStringsEqual(p1,p2))
                    {for (k=0;k<ps2->texts->count;k++)
                        { PText2d pt2=(PText2d)ps2->texts->At(k);
                            for(l=0;l<pt2->text->count;l++)
                            {PTextRecord ptr=(PTextRecord)pt2->text->At(l);
                                if (ptr->string==p1)
                                {ptr->string=p2;p2->IncRef(pt2);p1->DecRef(pt2);
                                }
                            }
                        }
                        delete p1;p1=NULL;
                    }else j++;
                }
                if (p1) // Если нет аналогов, то вставляем
                {
                    strings->InsertObject(p1);
                    if (_m)
                    {meta->WriteWord(mfdeletetool);
                        meta->WriteWord(6);
                        meta->WriteWord(p1->_HANDLE);
                    }
                }
            }
        }
        else
            for (i=0;i<ps2->strings->count;i++){
                PString2d ps=(PString2d)ps2->strings->At(i);
                strings->InsertObject(ps);
                if(_m)
                {meta->WriteWord(mfdeletetool);
                    meta->WriteWord(6);
                    meta->WriteWord(ps->_HANDLE);
                }
            }
        //fonts
        if (flags & PFC_FONTS){
            PFont2d p1,p2;
            for (i=0;i<ps2->fonts->count;i++)
            {p1=(PFont2d)ps2->fonts->At(i);j=0;
                while(j<fonts->count && p1)
                {p2=(PFont2d)fonts->At(j);
                    if (IsFontsEqual(p1,p2))
                    {for (k=0;k<ps2->texts->count;k++)
                        { PText2d pt2=(PText2d)ps2->texts->At(k);
                            for(l=0;l<pt2->text->count;l++)
                            {PTextRecord ptr=(PTextRecord)pt2->text->At(l);
                                if (ptr->font==p1)
                                {ptr->font=p2;p2->IncRef(pt2);p1->DecRef(pt2);
                                }
                            }}
                        delete p1;p1=NULL;
                    }else j++;
                }
                if (p1) // Если нет аналогов, то вставляем
                {
                    fonts->InsertObject(p1);
                    if (_m)
                    {meta->WriteWord(mfdeletetool);
                        meta->WriteWord(5);
                        meta->WriteWord(p1->_HANDLE);
                    }
                }
            }
        }
        else
            for (i=0;i<ps2->fonts->count;i++)
            {PFont2d pf=(PFont2d)ps2->fonts->At(i);
                fonts->InsertObject(pf);
                if(_m)
                {meta->WriteWord(mfdeletetool);
                    meta->WriteWord(5);
                    meta->WriteWord(pf->_HANDLE);
                }
            }
        //texts
        if (flags & PFC_TEXTS){
            PText2d p1,p2;
            for (i=0;i<ps2->texts->count;i++)
            {p1=(PText2d)ps2->texts->At(i);j=0;
                while(j<texts->count && p1)
                {p2=(PText2d)texts->At(j);
                    if (IsTextsEqual(p1,p2))
                    {for (k=0;k<ps2->primary->count;k++)
                        {po=(PObject2d)ps2->primary->At(k);
                            if ((po->WhoIsIt()==23)&&(((PText_2d)po)->text==p1))
                            {((PText_2d)po)->text=p2;p2->IncRef(po);p1->DecRef(po);
                            }
                        }
                        delete p1;p1=NULL;
                    }else j++;
                }
                if (p1) // Если нет аналогов, то вставляем
                {
                    texts->InsertObject(p1);
                    if (_m)
                    {meta->WriteWord(mfdeletetool);
                        meta->WriteWord(7);
                        meta->WriteWord(p1->_HANDLE);
                    }
                }
            }
            ;}
        else
            for (i=0;i<ps2->texts->count;i++){
                PText2d pt=(PText2d)ps2->texts->At(i);
                texts->InsertObject(pt);
                if(_m)
                {meta->WriteWord(mfdeletetool);
                    meta->WriteWord(7);
                    meta->WriteWord(pt->_HANDLE);
                }
            }
        //spaces3d
        if (flags & PFC_3D){
            PSpace3d p1,p2;
            for (i=0;i<ps2->spaces3d->count;i++)
            {p1=(PSpace3d)ps2->spaces3d->At(i);j=0;
                while(j<spaces3d->count && p1)
                {p2=(PSpace3d)spaces3d->At(j);
                    if (IsSpacse3dEqual(p1,p2))
                    {for (k=0;k<ps2->primary->count;k++)
                        {po=(PObject2d)ps2->primary->At(k);
                            if ((po->WhoIsIt()==24)&&(((PView3d_2d)po)->space3d==p1))
                            {PView3d_2d pv=(PView3d_2d)po;

                                pv->camera->views->Delete(pv);
                                pv->space3d->views->Delete(pv);

                                pv->space3d=p2;

                                pv->camera=(TCamera3d*)p2->all->GetByHandle(pv->camera->_HANDLE);
                                pv->camera->views->Insert(pv);

                                p2->views->Insert(po);
                                for(C_TYPE v=0;v<pv->items->count;v++){
                                    _TObject3d* o=(_TObject3d*)pv->items->At(v);
                                    o->object=(TObject3d*)p2->primary->At(v);
                                }
                            }
                        }
                        delete p1;p1=NULL;
                    }else j++;
                }
                if (p1) // Если нет аналогов, то вставляем
                {
                    spaces3d->InsertObject(p1);
                    p1->space2d=this;
                    if (_m)
                    {
                        meta->WriteWord(mfdeletespace3d);
                        meta->Write(&(p1->_HANDLE),1);
                    }
                }
            }
        }
        else
            for (i=0;i<ps2->spaces3d->count;i++){
                PSpace3d p3=(PSpace3d)ps2->spaces3d->At(i);
                spaces3d->InsertObject(p3);
                p3->space2d=this;
                if(_m)
                {meta->WriteWord(mfdeletespace3d);
                    meta->Write(&(p3->_HANDLE),1);
                }

            }

        //------------------ END TOOLS --------------------//

        ps2->all->DeleteAll();
        ps2->primary->DeleteAll();
        ps2->pens->DeleteAll();
        ps2->bruhs->DeleteAll();
        ps2->dibs->DeleteAll();
        ps2->ddibs->DeleteAll();
        ps2->fonts->DeleteAll();
        ps2->texts->DeleteAll();
        ps2->strings->DeleteAll();
        ps2->spaces3d->DeleteAll();
        delete ps2;
        if (cpo){
            Lock();
            if (flags & PFC_MOVEOBJECT) cpo->Move2d(delta);
            AddObjectRect(cpo);
            if (flags & PFC_SETCURRENT)
            {all->current=cpo->_HANDLE;
                if (flags & PFC_SETFRAME)
                {POINT2D sp;sp.x=0;sp.y=0; CreateEditFrame2d(sp,
                                                             FRAME_HASPOINTS| FRAME_HASLINES); }
            }
            UnLock();
            if (_m){
                meta->WriteWord(mfendofgroup);
                CloseMetaRecord();
            }
            rectchange=1;
            return cpo->_HANDLE;
        }else {Invalidate();return -1;}
    }

    return 0;
};
TSpace2d::~TSpace2d()
{
    CloseMetaFile();
    if (HIWORD(BkBrush)) BkBrush->DecRef(this);
    //while(all->count)_DeleteObject(all->GetNextHandle(0));
    DeleteCollection(all);all=0;
    if (primary){primary->DeleteAll();DeleteCollection(primary);primary=0;}
    if (Axis3d)delete Axis3d;
    if (Frame3d)delete Frame3d;
    Frame3d=NULL;Axis3d=NULL;
    DeleteCollection(texts);       texts=NULL; // must before strings and fonts
    DeleteCollection(bruhs);       bruhs=NULL; // must before dibs
    DeleteCollection(pens);        pens=NULL;
    DeleteCollection(dibs);        dibs=NULL;
    DeleteCollection(ddibs);       ddibs=NULL;
    DeleteCollection(fonts);       fonts=NULL;
    DeleteCollection(strings);		 strings=NULL;
    DeleteCollection(spaces3d);		 spaces3d=NULL;
    if(name) delete name;
    if(author) delete author;
    if(description) delete description;
    if(filename)delete filename;
    if(Frame)delete Frame;
    if(RCenter) delete RCenter;
    if(path) delete path;
    if(txtPath) delete txtPath;
};

void TSpace2d::AddCollection()
{
    if (primary==NULL)primary=new TPrimaryCollection(10,10);
    if (all==NULL)	all   =new TObjectCollection(10,10);

    if (pens==NULL) pens= new TPenCollection(10,10);
    if (bruhs==NULL) bruhs=new TBrushCollection (10,10);
    if (dibs==NULL) dibs=new TDibCollection (10,10);
    if (ddibs==NULL) ddibs=new TDoubleDibCollection(10,10);
    if (fonts==NULL) fonts=new TFontCollection (10,10);
    if (strings==NULL) strings=new TStringCollection (10,10);
    if (texts==NULL) texts=new TTextCollection (10,10);
    if (spaces3d==NULL) spaces3d=new T3dCollection(4,4);
};

void TSpace2d::AddRect(RECT r)
{
    if(hwnd)
    {
        if (ischanged)
            Rect_or_Rect(updaterect,r);
        else
        {
            updaterect=r;
            ischanged=1;
        }
        if (lock==0)
            SendPaint();
    }
};

void TSpace2d::AddObjectRect(PObject po)
{
    if ((po)&&(IsVisible((PObject2d)po)))
        AddRect(po->GetRect());
};

int  TSpace2d::Lock()
{
    lock++;
    return lock;
}

int  TSpace2d::UnLock()
{
    if(lock) // чтоб не ушло в минус
    {
        lock--;
        if ((lock==0)&&(ischanged))
            SendPaint();
    }
    return lock;
};

void TSpace2d::UpdateLowScale()
{
    lowscale=0;
    if(scale_div.x>scale_mul.x)lowscale=1;
    if(scale_div.y>scale_mul.y)lowscale=1;
};

BOOL TSpace2d::SetScale2d(POINT2D m,POINT2D d)
{
    if((floor(d.x)>0)&&(floor(d.y)>0)&&(floor(m.x)>0)&&(floor(m.y)>0))
    {
        if(((m.x/d.x)<=50)&&((m.x/d.x)>0.01)&&((m.y/d.y)<=50)&&((m.y/d.y)>0.01))
        {
            scale_mul = m;
            scale_div = d;
            UpdateLowScale();
            UpdateSize();
            Invalidate();
            return TRUE;
        }
    }
    return FALSE;
};

long  TSpace2d::SetBkBrush(HOBJ2D h)
{int old=0;
    PBrush2d pb;
    if (h){pb =(PBrush2d)bruhs->GetByHandle(h);
        if (pb==NULL)return 0; }else pb=NULL;

    if (BkBrush)
    {old=BkBrush->_HANDLE;BkBrush->DecRef(this);}
    BkBrush=pb;if (pb)BkBrush->IncRef(this);

    Invalidate();
    return old | 0x10000l;
};

void TSpace2d::Invalidate()
{
    ischanged |= 2;
    if (lock==0)
        SendPaint();
};

void TSpace2d::SendPaint()
{
    if(!ischanged) return;

    if (ischanged&2)
        GetClientRect(hwnd, &updaterect);
    else
    {
        _LpToDp((LPPOINT) &updaterect, 2);
        if ((scale_mul.x/scale_div.x) * scale_div.x != scale_mul.x)
            updaterect.right++;

        if ((scale_mul.y / scale_div.y) * scale_div.y != scale_mul.y)
            updaterect.bottom++;

        if (lowscale)
        {
            updaterect.left--;
            updaterect.top--;
            updaterect.right++;
            updaterect.bottom++;
        }
    }

    updaterect.left-=5;//hack для косяка с непрорисовкой линий при скроллировании окна
    updaterect.top-=5;
    updaterect.right+=5;
    updaterect.bottom+=5;

    if(hwnd)
        InvalidateRect(hwnd,&updaterect,1);
    ischanged=0;
};

HOBJ2D TSpace2d::CreateEditFrame3d(POINT2D SizerSize )
{if (SizerSize.x==0)SizerSize.x=8;
    if (SizerSize.y==0)SizerSize.y=8;
    if (all->current>0)
    {PView3d_2d po=(PView3d_2d)GetObjByHANDLE(all->current);;
        if (po && po->WhoIsIt()==24)
        {PSpace3d ps3=po->space3d;
            if (ps3->all->current>0){
                PObject3d p3=(PObject3d)ps3->GetObjByHANDLE(ps3->all->current);
                if (p3)
                {if (Frame3d){UpdateSpecObject((PObject2d)Frame3d);delete Frame3d;}

                    Frame3d = new TFrame3d(SizerSize,po,p3);
                    UpdateSpecObject((PObject2d)Frame3d);
                    //AddRect(Frame3d->GetRect());
                    return OID_FRAME3d;
                }
            }}
    }
    return 0;
};
BOOL TSpace2d::DeleteEditFrame3d()
{ if (Frame3d)
    {
        // AddRect(Frame3d->GetRect());
        UpdateSpecObject((PObject2d)Frame3d);
        delete Frame3d;Frame3d=NULL;
        return TRUE;
    }
    return 0;
};
HOBJ2D TSpace2d::CreateEditFrame2d(POINT2D SizerSize ,int f)
{if (SizerSize.x==0)SizerSize.x=8;
    if (SizerSize.y==0)SizerSize.y=8;
    if (all->current>0 || f & FRAME_NOOBJECT)
    {
        PObject2d po=(PObject2d)GetObjByHANDLE(all->current);
        if (po || f&FRAME_NOOBJECT){
            if (Frame)
            {if (Frame->object==po)return 1;
                UpdateSpecObject(Frame);
                delete Frame;Frame=NULL;
            }
            Frame = new TEditFrame(po,SizerSize,f);
            Frame->UpdateSize(0,this);
            UpdateSpecObject(Frame);
            return OID_FRAME2d;
        }
    }
    return 0;
};
BOOL TSpace2d::DeleteEditFrame2d()
{ if (Frame)
    {
        UpdateSpecObject(Frame);
        delete Frame;Frame=NULL;
    }
    return 0;
};
HOBJ2D TSpace2d::CreateRotateCenter2d(POINT2D& center )
{if (RCenter)
    {if ((RCenter->center.x==center.x)&&
         (RCenter->center.y==center.y)) return 0;
        UpdateSpecObject(RCenter);
        RCenter->center=center;
    }else
    {RCenter=new TRotateCenter(center);
    }
    RCenter->UpdateSize(0,this);
    UpdateSpecObject(RCenter);
    return OID_RCENTER;
};
BOOL TSpace2d::DeleteRotateCenter2d()
{ if (RCenter)
    {UpdateSpecObject(RCenter);
        delete RCenter;RCenter=NULL;
        return TRUE;
    }
    return FALSE;
};
double __GetAngle(POINT2D& delta,POINT2D& Origin)
{double f,dy,dx;
    dx=(double)(Origin.x-delta.x);
    dy=(double)(Origin.y-delta.y);
    if (dx<0)dx=-dx;
    if (dy<0)dy=-dy;
    if(Origin.x-delta.x)
        f=atan((double)(dy/(dx)));
    else {
        if (Origin.y-delta.y)
            f=(Origin.y-delta.y)>0 ? M_PI_2 : - M_PI_2 ;
        else f=0.0;
        return f;
    }
    if ((Origin.y-delta.y)>0){
        f=(Origin.x>delta.x)? f:M_PI-f;
    }else{
        f=(Origin.x>delta.x)? M_PI+M_PI_2+(M_PI_2-f):f+M_PI;
    }
    return f;
}
BOOL TSpace2d::SetObjectOrg2d(HOBJ2D Object2d, POINT2D &Origin )
{
    TObject2d * po=(TObject2d * )GetObjByHANDLE(Object2d);
    if(po)
    {
        CRDtoLP(&Origin);
        WORD objtype = po->WhoIsIt();
        POINT2D o = po->GetOrigin2d();
        if (o.x == Origin.x && o.y == Origin.y)
            return 1;

        int spec = ((objtype == 50)||(objtype == 51));
        if (spec)
            UpdateSpecObject(po);
        else
        {
            Lock();
            AddObjectRect(po);
            rectchange=1;
        }
        if (objtype == 50)
        {
            POINT2D pp=Origin;
            if (RCenter)
                /*
Begin Rotate Rotate Rotate Rotate Rotate Rotate Rotate Rotate
*/
            {POINT2D delta=RCenter->center;
                Frame->UpdateSize();
                POINT2D p=GetFramePoint(Object2d-(OID_FRAME2d1-1));
                double f1=__GetAngle(delta,Origin);
                double f2=__GetAngle(delta,p);
                Frame->Rotate2d(RCenter->center,f1-f2+Frame->_angle);

                /*
End Rotate Rotate Rotate Rotate Rotate Rotate Rotate Rotate
*/
            }else{
                POINT2D s=Frame->_size,o=Frame->_origin;
                if (Frame->angle!=0.0)RotatePoint(pp,Frame->_origin,-Frame->angle);
                switch(Object2d-(OID_FRAME2d1-1))
                {
                    case 1:s.y=o.y+s.y-pp.y;o.y=pp.y;
                        s.x=o.x+s.x-pp.x;o.x=pp.x;break;
                    case 2:s.y=o.y+s.y-pp.y;o.y=pp.y;
                        s.x=pp.x-o.x;break;
                    case 3:s.x=pp.x-o.x;s.y=pp.y-o.y;break;
                    case 4:s.y=pp.y-o.y;
                        s.x=o.x+s.x-pp.x;o.x=pp.x;break;
                    case 5:s.y=o.y+s.y-pp.y;o.y=pp.y;break;
                    case 6:s.x=pp.x-o.x;break;
                    case 7:s.y=pp.y-o.y;break;
                    case 8:s.x=o.x+s.x-pp.x;o.x=pp.x;break;
                }
                if (((s.x>1)&&(s.y>1))||Frame->object==NULL)
                {Frame->_size=s;Frame->_origin=o;}
                Frame->UpdateSize(15,this);
            }
        } {
            POINT2D oldp=po->Move2d(Origin);
            UpdateFrame(po);
            Origin=oldp;
        }
        //POINT2D oldp=po->Move(Origin);
        if (spec)UpdateSpecObject(po);else{
            AddObjectRect(po);
            UnLock();
        }

        return 1;
    }
    return 0;
};

BOOL TSpace2d::SetObjectSize2d(HOBJ2D Object2d, POINT2D &Size )
{
    TObject2d * po=(TObject2d*)GetObjByHANDLE(Object2d);
    if(po)
    {
        rectchange=1;
        Lock();
        AddObjectRect(po);
        POINT2D oldp=po->SetSize2d(Size);
        UpdateFrame(po);
        AddObjectRect(po);
        UnLock();
        Size=oldp;
        return 1;
    }
    return 0;
};

BOOL   TSpace2d::RotateObject2d(HOBJ2D Object2d, ROTATOR2D * rotator )
{
    TObject2d * po = (TObject2d*)GetObjByHANDLE(Object2d);
    if(po)
    {
        rectchange = 1;
        Lock();

        AddObjectRect(po);
        po->Rotate2d(rotator->center, rotator->angle);
        UpdateFrame(po);
        //po->UpdateSize();
        AddObjectRect(po);

        UnLock();
        return 1;
    }
    return 0;
};

BOOL TSpace2d::SetObjectAlpha2d(HOBJ2D Object2d, gr_float alpha /*0.0 .. 1.0*/)
{
    TObject2d * po=(TObject2d * )GetObjByHANDLE(Object2d);
    if(!po) return 0;
    Lock();
    AddObjectRect(po);
    po->SetAlpha(alpha);
    UnLock();
    return 1;
};

void TSpace2d::SetAnisotropic(HDC dc)
{
    SetMapMode(dc,MM_ANISOTROPIC);
#ifdef WIN32
    SetWindowExtEx(dc,scale_div.x,scale_div.x,NULL);
    SetViewportExtEx(dc,scale_mul.x,scale_mul.y,NULL);
    SetViewportOrgEx(dc,-(membitmaporg.x+org.x),-(membitmaporg.y+org.y),NULL);
    SetWindowOrgEx(dc,0,0,NULL);
#else
    SetWindowExt(dc,scale_div.x,scale_div.x);
    SetViewportExt(dc,scale_mul.x,scale_mul.y);
    SetViewportOrg(dc,-(membitmaporg.x+org.x),-(membitmaporg.y+org.y));
    SetWindowOrg(dc,0,0);
#endif
    //  SetWindowOrg(dc,org.x,org.y);
};

void TSpace2d::SetTextMode(HDC dc)
{
    SetMapMode(dc,MM_TEXT);
#ifdef WIN32
    SetWindowOrgEx(dc,0,0,NULL);
    SetViewportOrgEx(dc,-membitmaporg.x,-membitmaporg.y,NULL);
#else
    SetWindowOrg(dc,0,0);
    SetViewportOrg(dc,-membitmaporg.x,-membitmaporg.y);
#endif
};

void TSpace2d::View(HDC dc,RECT UpdRect)
{
    if(RenderEngine==0)
    {
        PObject2d po;
        for (C_TYPE i=0;i<primary->count;i++)
        {
            po= (PObject2d)primary->At(i);
            if (IsVisible(po) && (RectIntersect(UpdRect,po->GetRect(1))))
            {
                switch(po->WhoIsIt())
                {
                    case 20:_DrawPolyLine(dc,(PLine_2d)po);
                        break;
                    case 21:_DrawBitmap(dc,(PBitmap_2d)po,(PDib2d)(((PBitmap_2d)po)->dib->GetMainObject()),SRCCOPY);
                        break;
                    case 22:_DrawDoubleBitmap(dc,(PDoubleBitmap_2d)po);
                        break;
                    case 23:_DrawText(dc,(PText_2d)po, UpdRect);
                        break;
                    case 24:_DrawSpace3d(dc,(PView3d_2d)po,UpdRect);
                        break;
                    case 25:_DrawUser2d(dc,(TUserObject2d*)po,UpdRect);
                }
            }
        }
    }

    if(activefont){
        DeleteObject(_activefont);
        activefont=NULL;_activefont=NULL;activefontangle=0;
    }

    if(!(paint_flags&PF_NOFRAMES)){//draw frame/Rcenter
        int drawFrame=(Frame)&&(RectIntersect(UpdRect,Frame->GetRect(1)));
        int drawCenter=(RCenter)&&(RectIntersect(UpdRect,RCenter->GetRect(1)));
        int drawFrame3d=(Frame3d !=NULL)&&(RectIntersect(UpdRect,Frame3d->GetRect(1)));
        int drawAxis3d=(Axis3d!=NULL)&&(RectIntersect(UpdRect,Axis3d->GetRect(1)));

        if (drawFrame || drawCenter){
            int rop=SetROP2(dc,R2_XORPEN);
            HGDIOBJ penold=SelectObject(dc,GetStockObject(WHITE_PEN));
            if (drawFrame) _DrawFrame(dc,UpdRect);
            if (drawCenter)_DrawRCenter(dc);
            SelectObject(dc,penold);
            SetROP2(dc,rop);}
        if(drawFrame3d ||drawAxis3d){
            if (drawFrame3d)_DrawFrame3d(dc,UpdRect);
            if (drawAxis3d)_DrawAxis3d(dc,UpdRect);
        }
    }
};

void _MRect(HDC dc,POINT c,int d=1)
{/* POINT2D aPoints[5];
    aPoints[0].x = c.x-d;aPoints[0].y =c.y-d;
    aPoints[1].x = c.x+d;aPoints[1].y =c.y-d;
    aPoints[2].x = c.x+d;aPoints[2].y =c.y+d;
    aPoints[3].x = c.x-d;aPoints[3].y =c.y+d;
    aPoints[4].x = c.x-d;aPoints[4].y =c.y-d;
    Polyline(dc,aPoints,5);*/

    PatBlt(dc,c.x-d,c.y-d,d*2+1,d*2+1,DSTINVERT);

};

/* 1------5------2
     |             |
     8             6
     |             |
     4------7------3
*/
void TSpace2d::_DrawFrame(HDC dc,RECT)
{if (Frame->size.x==0 || Frame->size.y==0)return;

    HGDIOBJ hbold=SelectObject(dc,GetStockObject(NULL_BRUSH));
    POINT2D ps=Frame->psize;
    POINT2D pp[5];int i;
    POINT2D mp;
    for (i=0;i<4;i++)pp[i]=Frame->tops[i];
    pp[4]=pp[0];
    _LpToDp(pp,5);
    POINT _pp[5];
    for (i=0;i<5;i++)PntToPnt(_pp[i],pp[i]);

    if(Frame->options&FRAME_MASK){
        HBRUSH hb=GetGrayBrush();
        SelectObject(dc,hb);
        UnrealizeObject(hb);
        COLORREF clrref=SetTextColor(dc,RGB(0,0,128));
        int oldr2=SetROP2(dc,R2_MASKPEN);
        HGDIOBJ oldpen=SelectObject(dc,GetStockObject(NULL_PEN));
        _pp[1].x++;_pp[2].x++;
        _pp[2].y++;_pp[3].y++;
        Polygon(dc,_pp,5);
        SetTextColor(dc,clrref);
        SetROP2(dc,oldr2);
        SelectObject(dc,oldpen);
        SelectObject(dc,hbold);
        return;
    }
    ps.x=ps.x/2;ps.y=ps.y/2;
    if (Frame->options&FRAME_HASLINES){

        if (Frame->options&FRAME_DASHED){
            if(Frame->angle==0.0){
                HBRUSH hg=GetGrayBrush();
                UnrealizeObject(hg);
                HGDIOBJ old= SelectObject(dc,hg);

                PatBlt(dc,_pp[0].x,_pp[0].y,_pp[1].x-_pp[0].x,1,PATINVERT);
                PatBlt(dc,_pp[0].x,_pp[2].y,_pp[1].x-_pp[0].x,1,PATINVERT);

                PatBlt(dc,_pp[1].x,_pp[1].y+1,1,(_pp[2].y-_pp[1].y)-2,PATINVERT);
                PatBlt(dc,_pp[0].x,_pp[1].y+1,1,(_pp[2].y-_pp[1].y)-2,PATINVERT);

                SelectObject(dc,old);
            }else{
                HGDIOBJ np=::CreatePen(PS_DOT,0,RGB(255,255,255));
                HGDIOBJ oldpen=SelectObject(dc,np);
                int obk=SetBkMode(dc,TRANSPARENT);
                Polyline(dc,_pp,5);
                SetBkMode(dc,obk);
                SelectObject(dc,oldpen);
                ::DeleteObject(np);
            }
        }else Polyline(dc,_pp,5);}
    if (Frame->options&FRAME_HASPOINTS)
        for(i=0;i<4;i++){
            _MRect(dc,_pp[i],3);
            POINT _mp;
            _mp.x=(pp[i].x+pp[i+1].x)/2;
            _mp.y=(pp[i].y+pp[i+1].y)/2;
            _MRect(dc,_mp,3);
        }
    SelectObject(dc,hbold);
};
void TSpace2d::_DrawRCenter(HDC dc)
{
    HGDIOBJ hbold=SelectObject(dc,GetStockObject(NULL_BRUSH));
    POINT2D ps=RCenter->center;
    _LpToDp(&ps,1);
    Ellipse(dc,ps.x-RCenter->_size.x,ps.y-RCenter->_size.y,
            ps.x+RCenter->_size.x+1,ps.y+RCenter->_size.y+1);
    SelectObject(dc,GetStockObject(BLACK_BRUSH));
    Ellipse(dc,ps.x-2,ps.y-2,ps.x+3,ps.y+3);
    SelectObject(dc,hbold);
};

//---------------------------------------------------
TPRNTEXT * TSpace2d::InsertPRNitem(HDC dc,PTextRecord ptr,char * str,int len)
{
    TPRNTEXT * item= new TPRNTEXT;
    item->string=str;
    if (len==-1)item->len=lstrlen(str);else item->len=len;
    if (item->len>0 && item->string[item->len-1]=='\r')item->len--;
    //  if (item->len>1 && item->string[item->len-1]=='\r')item->len--;
    item->pt=ptr;
    HFONT _font=CreateFontIndirect(ptr->font->font);
    HGDIOBJ old=SelectObject(dc,_font);
#ifdef WIN32
    SIZE p;
    if (item->len){
        GetTextExtentPoint32(dc,item->string,item->len,&p);
        item->size.x=p.cx;}else
    {
        GetTextExtentPoint32(dc,"A",1,&p);
        item->size.x=0;
    }
    item->size.y=p.cy;
#else
    long l;
    if (item->len){
        l=GetTextExtent(dc,item->string,item->len);
        item->size.x=LOWORD(l);}
    else {l=GetTextExtent(dc,"A",1);item->size.x=0;}
    item->size.y=HIWORD(l);
#endif
    /*
  TEXTMETRIC tm;
  GetTextMetrics(dc,&tm);
  item->ascent=tm.tmAscent;
*/
    if(!ptr->font->tmValid)ptr->font->GetTM(dc);
    item->ascent=ptr->font->tmAscent;
    SelectObject(dc,old);
    ::DeleteObject(_font);
    tmpCollection->Insert(item);
    return item;
};

INT16 TSpace2d::PrintPRNString(HDC dc,HDC mdc,PText_2d pt,POINT2D origin,INT16 deltay,POINT2D & _s)
{
    if(tmpCollection->count)
    {
        INT16 i,maxy=0,sizex=0;
        TPRNTEXT * item;
        int ascent=0;
        for(i=0;i<tmpCollection->count;i++)
        {
            item=(TPRNTEXT *)tmpCollection->At(i);
            maxy=max(maxy,item->size.y);
            sizex+=item->size.x;
            if(ascent<item->ascent)ascent=item->ascent;
        }
        deltay=deltay+maxy;
        origin.y+=deltay;
        _s.x=max(_s.x,sizex);
        _s.y=origin.y-pt->origin.y;
        origin.y+=ascent-maxy;
        RotatePoint(origin,pt->origin,pt->GetAngle());
#ifdef WIN32
        MoveToEx(dc,origin.x,origin.y,NULL);
#else
        MoveTo(dc,origin.x,origin.y);
#endif
        SetTextAlign(dc,TA_LEFT|TA_BASELINE|TA_UPDATECP);
        if (mdc){
#ifdef WIN32
            MoveToEx(mdc,origin.x,origin.y,NULL);
#else
            MoveTo(mdc,origin.x,origin.y);
#endif
            SetTextAlign(mdc,TA_LEFT|TA_BASELINE|TA_UPDATECP);
        }
        for(i=0;i<tmpCollection->count;i++)
        {
            item =(TPRNTEXT *) tmpCollection->At(i);
            if ((item->len)&&
                // Проверка по выходу за границы по Y.
                //                (origin.y-maxy<pt->origin.y+pt->size.y)&&
                //                (origin.y>pt->origin.y)&&
                // Проверка по выходу за границу по X.
                (origin.x+item->size.x > pt->origin.x)&&
                (origin.x < pt->origin.x+pt->size.x)
                ){
                HGDIOBJ _font=(HGDIOBJ)_CreateFont(item->pt->font,pt->angle);
                HGDIOBJ old=SelectObject(dc,_font);
                HGDIOBJ oldmfont;

                int bkmode=((item->pt->ltBgColor & 0x01000000l)!=0l) ? 0 : OPAQUE;
                int oldbk=0;
                if(bkmode)
                {
                    SetBkColor(dc,GetGDIColor(item->pt->ltBgColor));
                    oldbk=SetBkMode(dc,OPAQUE);
                    if(mdc)SetBkMode(mdc,OPAQUE);
                }
                else
                {
                    oldbk=SetBkMode(dc,TRANSPARENT);
                    if(mdc)SetBkMode(mdc,TRANSPARENT);
                }

                SetTextColor(dc,GetGDIColor(item->pt->ltFgColor));
                TextOut(dc,0,0,item->string,item->len);
                if(mdc)
                {
                    oldmfont=SelectObject(mdc,_font);
                    TextOut(mdc,0,0,item->string,item->len);
                    SelectObject(mdc,oldmfont);
                }
                origin.x+=item->size.x;
                SelectObject(dc,old);
                //	 DeleteObject(_font);
                SetBkMode(dc,oldbk);
            }
            delete item;
        }
        tmpCollection->DeleteAll();
    }
    return deltay;
};
//----------
POINT2D TSpace2d::_DrawText(HDC dc,PText_2d ptext,RECT UpdRect)
{
    BOOL _print=(paint_flags&PF_TOPRINTER);
    BOOL makergn=TRUE;
    if(_print)
    {
        POINT2D _s = CalcTextSize(ptext);
        if(_s.x<=ptext->size.x+4 && _s.y<=ptext->size.y+4)
            makergn=FALSE;
    }
    POINT2D _size;_size.x=0;_size.y=0;
    if (ptext->text->text->count)
    {
        HRGN rgn=NULL;
        HDC cdc=dc,mdc=0;
        HBITMAP cbm,mbm,oldcolor,oldmono;
        RECT r;
        int needdraw=1,_H,_W,notrotate;

        if (ptext->angle!=0)
        {
            double f,f1;
            f1=modf(ptext->GetAngle()/(M_PI_2),&f);
            f1=fabs(f1);
            if (f1>0.5)f1=1-f1;
            notrotate=f1<((1.0/900.0)*M_PI_2);
        }
        else
            notrotate=1;

        if (notrotate)
        {
            SetAnisotropic(dc);
            if(makergn)
            {
                r.left=ptext->origin.x;
                r.top=ptext->origin.y;
                r.right=r.left+ptext->size.x;
                r.bottom=r.top+ptext->size.y;

                _LpToDp((LPPOINT) &r,2);
                RotateRect(r,ptext->GetAngle());
                rgn=CreateRectRgn(r.left,r.top,r.right,r.bottom);
                OffsetRgn(rgn,-membitmaporg.x,-membitmaporg.y);
                SelectClipRgn(dc,rgn);
            }
        }
        else
        {
            if(_print)
                SetAnisotropic(dc);

            if (!(ptext->options&0x80) && !_print)
            {
                r.left=ptext->origin.x;
                r.top=ptext->origin.y;
                r.right=r.left+ptext->size.x;
                r.bottom=r.top+ptext->size.y;
                POINT tops [4];
                GetRotateRectPoints(r,ptext->GetAngle(),tops);
                RotateRect(r,ptext->GetAngle());
                r.left=max(r.left,UpdRect.left);
                r.right=min(r.right,UpdRect.right)+1;
                r.top=max(r.top,UpdRect.top);
                r.bottom=min(r.bottom,UpdRect.bottom)+1;
                rgn=CreatePolygonRgn(tops,4,ALTERNATE);
                needdraw=RectInRegion(rgn,&r);
                if (needdraw)
                {
                    HRGN rgnDest,rgn2;
                    rgnDest = CreateRectRgn(0, 0, 0, 0);
                    rgn2 = CreateRectRgn(r.left,r.top,r.right,r.bottom);
                    CombineRgn(rgnDest,rgn2,rgn, RGN_XOR);
                    DeleteObject(rgn);DeleteObject(rgn2);
                    rgn=rgnDest; // Это регион
                    _LpToDp((LPPOINT)&r,2);
                    _H=r.bottom-r.top;
                    _W=r.right-r.left;
                    cbm=CreateCompatibleBitmap(dc,_W,_H);
                    mbm=CreateBitmap(_W,_H,1,1,NULL);
                    cdc=CreateCompatibleDC(dc);
                    mdc=CreateCompatibleDC(dc);
                    oldcolor=(HBITMAP)SelectObject(cdc,cbm);
                    oldmono=(HBITMAP)SelectObject(mdc,mbm);
                    PatBlt(cdc,0,0,_W,_H,BLACKNESS);
                    PatBlt(mdc,0,0,_W,_H,WHITENESS);
                    SetTextColor(mdc,RGB(0,0,0));
                    SetBkColor(mdc,RGB(0,0,0));
                    POINT2D oldorg=membitmaporg;
                    membitmaporg.x=r.left;membitmaporg.y=r.top;
                    SetAnisotropic(cdc);
                    SetAnisotropic(mdc);
                    membitmaporg=oldorg;
                }
            }
        }
        if (needdraw)
        {
            INT16 deltay=0,i;
            POINT2D origin=ptext->origin;
            origin.x-=ptext->Delta.x;origin.y-=ptext->Delta.y;

            tmpCollection=new TCollection(5,5);
            PLOGTextCollection text=ptext->text->text;
            for (i=0;i<text->count;i++)
            {
                PTextRecord ptr=(PTextRecord)text->At(i);
                if (ptr->string->flags&1)
                {
                    char * p=ptr->string->string;
                    if (p)
                    {
                        char * sbegin=p;
                        {
                            while (*p!=0)
                            {
                                if (*p=='\n')
                                {
                                    InsertPRNitem(dc,ptr,sbegin,(p-sbegin));sbegin=p;sbegin++;
                                    INT16 __dy=PrintPRNString(cdc,mdc,ptext,origin,deltay,_size);
                                    deltay=__dy;
                                }
                                p++;
                            }
                            InsertPRNitem(cdc,ptr,sbegin);
                        }
                    }
                }
                else
                {
                    InsertPRNitem(dc,ptr,ptr->string->string);
                }
            }
            PrintPRNString(cdc,mdc,ptext,origin,deltay,_size);
            delete tmpCollection;
            if (notrotate||_print)
            {
                if(makergn)SelectClipRgn(dc,0);
                SetTextMode(dc);
                SetBkColor(dc,RGB(255,255,255));
                SetTextColor(dc,0);
            }
            else
            {
                if(_print)
                    SetTextMode(dc);
                if (!(ptext->options&0x80) && !_print)
                {
                    //*************************
                    HGDIOBJ cbr=SelectObject(cdc,GetStockObject(BLACK_BRUSH));
                    HGDIOBJ mbr=SelectObject(mdc,GetStockObject(WHITE_BRUSH));
                    PaintRgn(cdc,rgn);
                    PaintRgn(mdc,rgn);
                    SelectObject(cdc,cbr);SelectObject(mdc,mbr);
                    //*************************
                    //POINT2D origin=pt->origin;_LpToDp(&origin,1);
                    SetMapMode(cdc,MM_TEXT);SetMapMode(mdc,MM_TEXT);

                    SetViewportOrgEx(cdc,0,0,NULL);
                    SetViewportOrgEx(mdc,0,0,NULL);
                    SetWindowOrgEx(cdc,0,0,NULL);
                    SetWindowOrgEx(mdc,0,0,NULL);
                    BitBlt(dc,r.left,r.top,_W,_H,mdc,0,0,SRCAND);
                    BitBlt(dc,r.left,r.top,_W,_H,cdc,0,0,SRCINVERT);
                    SelectObject(cdc,oldcolor);SelectObject(mdc,oldmono);
                    DeleteDC(cdc); DeleteDC(mdc);
                    ::DeleteObject(cbm);::DeleteObject(mbm);
                }
            }
        } //needdraw
        if(rgn)
            DeleteObject(rgn);
    }
    return _size;
};

POINT2D TSpace2d::CalcTextSize(PText_2d pt)
{
    WORD _paint_flags = paint_flags;
    paint_flags = 0;
    HDC dc=CreateCompatibleDC(0);
    HBITMAP hb=CreateCompatibleBitmap(dc,16,16);
    HGDIOBJ ob=SelectObject(dc,hb);
    INT16 _a=pt->angle;pt->angle=0;
    POINT2D _d=pt->Delta;pt->Delta.x=0;pt->Delta.y=0;
    POINT2D _membitmaporg=membitmaporg;
    RECT _updaterect=updaterect;
    membitmaporg.x=0;membitmaporg.y=0;

    updaterect.left=0;
    updaterect.top=0;
    updaterect.right=15;
    updaterect.bottom=15;

    POINT2D _s = _DrawText(dc, pt, updaterect);

    pt->angle=_a;
    pt->Delta=_d;
    membitmaporg=_membitmaporg;
    updaterect=_updaterect;

    SelectObject(dc,ob);
    DeleteDC(dc);
    ::DeleteObject(hb);
    paint_flags=_paint_flags;

    return _s;
};
//---------------------------------------------------
void FillArea(PAINTSTRUCT3D*ps);

BOOL TSpace2d::ChkObjects3dtoFit(PAINTSTRUCT3D&ps,RECT&upd,BOOL _min)
{long left,right,top,bottom;
    left=upd.left;
    right=upd.right;
    top=upd.top;
    bottom=upd.bottom;
    RECT minrect;
    BOOL minset=FALSE;
    for(C_TYPE i=0;i<ps.objcount;i++){
        _OBJECT3D*obj=ps._objects[i];
        long _right=obj->org.x+obj->size.x;
        long _bottom=obj->org.y+obj->size.y;


        if ((!IsVisible((PObject3d)obj->object))||(obj->org.x>right)||((_right)<left)
            || (obj->org.y>bottom)||((_bottom)<top)|| (obj->flags&O3D_ZNOTINRECT))
            obj->flags|=O3D_NOTINRECT;else
        {
            obj->flags&=~O3D_NOTINRECT;
            if (
                (obj->org.x>=left)&&((_right)<=right)
                &&(obj->org.y>=top)&&((_bottom)<=bottom))
            {obj->flags|=O3D_FULLINRECT;
            }else  obj->flags&=~O3D_FULLINRECT;
            if(_min){
                if(!minset){
                    minrect.left=obj->org.x;
                    minrect.top=obj->org.y;
                    minrect.right=_right;
                    minrect.bottom=_bottom;
                    minset=TRUE;
                }else{
                    if(obj->org.x<minrect.left)minrect.left=obj->org.x;
                    if(obj->org.y<minrect.top)minrect.top=obj->org.y;
                    if(minrect.right<_right) minrect.right=_right;
                    if(minrect.bottom<_bottom) minrect.bottom=_bottom;
                }
            }
        }}
    if(ps.objcount && _min){
        BYTE f=0;
        minrect.bottom++;
        minrect.right++;
        //  minrect.left--;
        //  minrect.top--;

        if(upd.left<minrect.left)f|=1;else minrect.left=upd.left;
        if(upd.top<minrect.top)f|=2;else minrect.top=upd.top;
        if(upd.bottom>minrect.bottom)f|=4;else minrect.bottom=upd.bottom;
        if(upd.right>minrect.right)f|=8;else minrect.right=upd.right;
        if(f){

            HGDIOBJ b=CreateSolidBrush(ps.hmonoDc?0:GetGDIColor(ps.camera->poBackground));
            HGDIOBJ old=SelectObject(ps.hcolorDc,b);

            if(f&1)PatBlt(ps.hcolorDc,upd.left,upd.top,minrect.left-upd.left,upd.bottom-upd.top,PATCOPY);
            if(f&2)PatBlt(ps.hcolorDc,minrect.left,upd.top,minrect.right-minrect.left,minrect.top-upd.top,PATCOPY);
            if(f&4)PatBlt(ps.hcolorDc,minrect.left,minrect.bottom,minrect.right-minrect.left,upd.bottom-minrect.bottom,PATCOPY);
            if(f&8)PatBlt(ps.hcolorDc,minrect.right,upd.top,upd.right-minrect.right,upd.bottom-upd.top,PATCOPY);
            SelectObject(ps.hcolorDc,old);
            DeleteObject(b);
            if(ps.hmonoDc){
                SelectObject(ps.hmonoDc,GetStockObject(WHITE_BRUSH));
                if(f&1)PatBlt(ps.hmonoDc,upd.left,upd.top,minrect.left-upd.left,upd.bottom-upd.top,PATCOPY);
                if(f&2)PatBlt(ps.hmonoDc,minrect.left,upd.top,minrect.right-minrect.left,minrect.top-upd.top,PATCOPY);
                if(f&4)PatBlt(ps.hmonoDc,minrect.left,minrect.bottom,minrect.right-minrect.left,upd.bottom-minrect.bottom,PATCOPY);
                if(f&8)PatBlt(ps.hmonoDc,minrect.right,upd.top,upd.right-minrect.right,upd.bottom-upd.top,PATCOPY);
            }
            upd=minrect;
            ps.srcPaint=upd;
        }
    }
    if (_min && (!minset))return FALSE;
    return TRUE;
}


void TSpace2d::_DrawSpace3d(HDC dc,PView3d_2d pv,RECT& UpdRect)
{
    BOOL _print=(paint_flags&PF_TOPRINTER);
    POINT2D sz=pv->size;
    POINT2D _sz=sz;
    POINT2D org=pv->origin;
    _LpToDpWO(&_sz,1);
    if(!_print)
    {sz=_sz;}
    _LpToDp(&org,1);
    if ((pv->camera->camera.poRenderType&RENDER_TYPEMASK)==RENDER_DISABLED){
        HGDIOBJ brush=GetGrayBrush();
        HGDIOBJ old=SelectObject(dc,brush);
        UnrealizeObject(brush);
        PatBlt(dc,org.x,org.y,sz.x,sz.y,PATCOPY);
        SelectObject(dc,old);
        return;
    }
    HCURSOR oldcursor=0;
    POINT prev;

    if (pv->camera->camera.poRenderType&RENDER_WAITCURSOR)oldcursor=SetCursor(LoadCursor(0,IDC_WAIT));
    long time_start;
    if(pv->options&2){  time_start=GetTickCount(); }

    HRGN directlip=0;

    HDC memdc=NULL,monodc=NULL;
    HGDIOBJ hbmpcolor=NULL,hbmpmono=NULL;
    //	BOOL buffer=!(pv->camera->camera.poRenderType&RENDER_NOBUFFER) && !_print;
    BOOL buffer=!(pv->camera->camera.poRenderType&RENDER_NOBUFFER) || _print;
    if(buffer){
        if (pv->imagesize.x!=sz.x || pv->imagesize.y!=sz.y)
        {PntToPnt(pv->imagesize,sz);
            pv->NewBuffers();
        }
        if (pv->BMPcolor){
            if(_print){
                HDC _dc= GetDC(0);
                memdc=CreateCompatibleDC(_dc);
                ReleaseDC(0,_dc);
            }else{
                memdc=CreateCompatibleDC(dc);
            }
            hbmpcolor=SelectObject(memdc,pv->BMPcolor);
            if (palette) SelectPalette(memdc,palette,0);
        }
        if (pv->BMPmono){
            monodc=CreateCompatibleDC(dc);
            hbmpmono=SelectObject(monodc,pv->BMPmono);
        }
    }else{
        // Direct Draw(not in buffer)
        PntToPnt(pv->imagesize,sz);
        int x,y,x2,y2;

        x=org.x-membitmaporg.x;
        y=org.y-membitmaporg.y;
        x2=org.x+sz.x-membitmaporg.x;
        y2=org.y+sz.y-membitmaporg.y;

        directlip=CreateRectRgn(x,y,x2,y2);
        SetViewportOrgEx(dc,-(membitmaporg.x-org.x),-(membitmaporg.y-org.y),&prev);

        SelectClipRgn(dc,directlip);
        memdc=dc;
    }
    if (pv->ischanged || !buffer){

        RECT upd=pv->updaterect;
        //	 _LpToDpWO((POINT*)(&upd),2);
        if(buffer){
            upd.left=max(0,upd.left);
            upd.top=max(0,upd.top);
            upd.right=min(pv->imagesize.x,upd.right);
            upd.bottom=min(pv->imagesize.y,upd.bottom);
        }else{
            POINT2D pp,pp1;
            pp.x=UpdRect.left;
            pp.y=UpdRect.top;
            pp.x-=pv->origin.x;
            pp.y-=pv->origin.y;
            if(!_print)_LpToDpWO(&pp,1);

            upd.left=max(0,pp.x);
            upd.top=max(0,pp.y);

            pp.x=UpdRect.right;
            pp.y=UpdRect.bottom;
            pp.x-=pv->origin.x;
            pp.y-=pv->origin.y;
            if(!_print)_LpToDpWO(&pp,1);

            upd.right=min(sz.x,pp.x);
            upd.bottom=min(sz.y,pp.y);

        }
        HRGN cliprgn;

        POINT2D delta=pv->camera->camera.poOffset;
        if(!_print)_LpToDpWO(&delta,1);
        PAINTSTRUCT3D ps;

        memset(&ps,0,sizeof(ps));
        ps.zclipvalid=pv->zclipvalid;
        ps.hcolorDc=memdc;
        ps.hmonoDc=monodc;
        ps.srcPaint=upd;
        ps.offset=pv->offset;
        ps.camera=&(pv->camera->camera);
        ps._objects=(_OBJECT3D**)(pv->items->items);
        ps.objects3d=(TIntObject3d**)(pv->space3d->primary->items);
        ps.objcount=pv->items->count;

        if(ChkObjects3dtoFit(ps,upd,pv->minimizerect && !(pv->bounded))){
            int notfull=((upd.bottom-upd.top<pv->imagesize.y)||(upd.right-upd.left<pv->imagesize.x));
            if (notfull && buffer)
            {cliprgn=CreateRectRgn(upd.left,upd.top,upd.right,upd.bottom);
                if (memdc)SelectClipRgn(memdc,cliprgn);
                if (monodc)SelectClipRgn(monodc,cliprgn);
            }

            if((pv->Draw3dProc==NULL) || (pv->Draw3dProc(&ps)==0)){
                FillArea(&ps);
                for(C_TYPE i=0;i<pv->items->count;i++){
                    _TObject3d*obj=(_TObject3d*)pv->items->At(i);
                    if(!(obj->flags&(O3D_NOTINRECT|O3D_ZNOTINRECT))){
                        //if((obj->flags&O3D_NOTINRECT)==0)
                        COLOR3D color;
                        if(!obj->object->GetSolidColor(color))color.rgb=RGB(255,255,255);
                        pv->DrawBoundedRect(memdc,obj,color.rgb,FALSE);
                    }
                }
            }

            if (notfull && buffer)
            {
                if (memdc) SelectClipRgn(memdc,0);
                if (monodc) SelectClipRgn(monodc,0);
                DeleteObject(cliprgn);
            }
        }else FillArea(&ps);
        pv->ischanged=0;
    }

    if(buffer){
        if(_print){
            //	  StretchBlt(dc,org.x,org.y,_sz.x,_sz.y,memdc, 0,0, sz.x,sz.y,SRCCOPY);

            long sizeimage=ScanBytes(sz.x,24)*sz.y;
            long _size=sizeimage+(sizeof(BITMAPINFO));

            HGLOBAL block=GlobalAlloc(GMEM_MOVEABLE,_size);

            LPBITMAPINFOHEADER info=(LPBITMAPINFOHEADER) GlobalLock(block);
            info->biSize=sizeof(BITMAPINFOHEADER);
            info->biWidth=sz.x;
            info->biHeight=sz.y;
            info->biPlanes=1;
            info->biBitCount=(INT16)24;
            info->biCompression=BI_RGB;
            info->biSizeImage=_size;
            info->biXPelsPerMeter=300;
            info->biYPelsPerMeter=300;
            info->biClrUsed=0;
            info->biClrImportant=0;

            char *bits=((char*)info)+sizeof(BITMAPINFOHEADER);
            GetDIBits(memdc,pv->BMPcolor,0,sz.y,bits,(LPBITMAPINFO)info,DIB_RGB_COLORS);

            GlobalUnlock(block);

            StretchDIBits(dc,org.x,org.y,_sz.x,_sz.y,
                          0,0,sz.x,sz.y,
                          bits, (BITMAPINFO*)info, DIB_RGB_COLORS, SRCCOPY);

            GlobalFree(block);
        }else{
            // to display
            PntToPnt(sz,pv->imagesize);
            if (monodc){
                BitBlt(dc,org.x,org.y,sz.x,sz.y,monodc,0,0,SRCAND);
                BitBlt(dc,org.x,org.y,sz.x,sz.y,memdc,0,0,SRCINVERT);
            }else  {
                BitBlt(dc,org.x,org.y,sz.x,sz.y,memdc,0,0,SRCCOPY);
            }

        }
        if (monodc)DeleteDC(monodc);
        if (memdc)DeleteDC(memdc);
    }else{
        SelectClipRgn(dc,0);
        DeleteObject(directlip);

        SetViewportOrgEx(dc,prev.x,prev.y,NULL);
        memdc=0;
    }

    SetViewportOrgEx(dc,-(membitmaporg.x-org.x),-(membitmaporg.y-org.y),&prev);

    for(C_TYPE i=0;i<pv->items->count;i++){
        _TObject3d*obj=(_TObject3d*)pv->items->At(i);
        if(obj->object->options&STATE_BOUNDS)
            pv->DrawBoundedRect(dc,obj,RGB(255,255,255),TRUE);
        if(obj->object->options&STATE_AXIS3D)
            pv->DrawAxis3d(dc,obj,0);
    }
    SetViewportOrgEx(dc,prev.x,prev.y,NULL);

    if(pv->options&2){
        // Рисуем отладочную информацию.
        long time_end=GetTickCount();
        time_end-=time_start;
        RECT r;
        r.left=org.x;r.top=org.y;
        r.right=org.x+sz.x;
        r.bottom=org.y+sz.y;
        SelectObject(dc,GetStockObject(ANSI_VAR_FONT));
        char s[128];
        wsprintf(s,"Space3d (draw in %ld ms) :%s",time_end,pv->camera->camera.poRenderName);
        ExtTextOut(dc,org.x,org.y,ETO_CLIPPED,&r,s,lstrlen(s),0);
    }
    if (pv->camera->camera.poRenderType&RENDER_WAITCURSOR)SetCursor(oldcursor);
    //  if(_print)SetTextMode(dc);
};

void TSpace2d::_DrawDoubleBitmap(HDC dc,PDoubleBitmap_2d _pb)
{
    PDoubleDib2d pd=(PDoubleDib2d)_pb->dib->GetMainObject();
    if(pd)
    {
        pd->andbitmap->filename=pd->filename;
        _DrawBitmap(dc,(PBitmap_2d)_pb,pd->andbitmap,SRCAND);
        _DrawBitmap(dc,(PBitmap_2d)_pb,pd,SRCINVERT);
        pd->andbitmap->filename=NULL;
    }else{
        _DrawBmpStub(dc,_pb);
    }
};

extern HBITMAP noimage_bmp;
void TSpace2d::_DrawBmpStub(HDC dc,PObject2d pb)
{
    if(!noimage_bmp)noimage_bmp=LoadBitmap(hInstance,"NO_IMAGE");
    POINT dstsize,origin;
    PntToPnt(origin,pb->origin);
    PntToPnt(dstsize,pb->size);
    _LpToDpWO(&dstsize,1);
    dstsize.x--;dstsize.y--;
    _LpToDp(&origin,1);
    // HGDIOBJ hold=SelectObject(dc,GetStockObject())
    HPEN hp=::CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
    HGDIOBJ old=SelectObject(dc,hp);
    POINT p[3];
    p[0].x=origin.x;            p[0].y=origin.y+dstsize.y-1;
    p[1].x=origin.x;            p[1].y=origin.y;
    p[2].x=origin.x+dstsize.x;	p[2].y=origin.y;
    Polyline(dc,p,3);

    HPEN hp1=::CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNFACE));
    SelectObject(dc,hp1);
    p[0].x=origin.x;            p[0].y=origin.y+dstsize.y;
    p[1].x=origin.x+dstsize.x;  p[1].y=origin.y+dstsize.y;
    p[2].x=origin.x+dstsize.x;  p[2].y=origin.y+1;
    Polyline(dc,p,3);

    SelectObject(dc,old);
    ::DeleteObject(hp1);::DeleteObject(hp);
    HDC mdc=CreateCompatibleDC(dc);
    SelectObject(mdc,noimage_bmp);
    int sizex=14,sizey=16;
    sizex=min(sizex,dstsize.x-7);
    sizey=min(sizey,dstsize.y-7);
    if(sizex>0 && sizey>0)
    {
        BitBlt(dc,origin.x+7,origin.y+7,sizex,sizey,mdc,0,0,SRCCOPY);
    }
    DeleteDC(mdc);
    TToolRef*tr=(TToolRef*)((_TBMP2d*)pb)->dib;;
    if(tr && tr->name)
    {
        HGDIOBJ old=SelectObject(dc,GetStockObject(ANSI_VAR_FONT));
        char s[300];
        wsprintf(s," File : ( %s ) not found ",tr->name);
        RECT r;
        r.left=origin.x;r.top=origin.y;
        r.right=origin.x+dstsize.x-1;
        r.bottom=origin.y+dstsize.y-1;
        ExtTextOut(dc,origin.x+7,origin.y+26,ETO_CLIPPED ,&r,s,lstrlen(s),NULL);
        SelectObject(dc,old);
    }
};

void TSpace2d::_DrawBitmap(HDC dc,PBitmap_2d pb,PDib2d dib,DWORD rop)
{
    if(dib)
    {
        POINT DIBsize,origin;
        PntToPnt(origin,pb->origin);
        POINT dstsize;
        PntToPnt(dstsize,pb->size);
        _LpToDpWO(&dstsize,1);
        DIBsize.x=dib->W;DIBsize.y=dib->H;
        //	if (!dib->filename)_LpToDpWO(&DIBsize,1);
        _LpToDp(&origin,1);
        BOOL cb=( ((long)(dib->perline)*(dib->H)<1024*1024)
                  &&((paint_flags&PF_NOCACHEDIB)==0) );

        if (((DIBsize.x!=dib->imagesize.x)||(DIBsize.x!=dib->imagesize.x)||
             (dib->bitmap==NULL))	&& cb)
        {
            dib->imagesize=DIBsize;
            DIBtoBITMAP(dib,dc);
        }

        if (dib->bitmap && cb)
        {
            HDC mdc=CreateCompatibleDC(dc);
            HGDIOBJ hbmpold=SelectObject(mdc,dib->bitmap);
            if (palette) SelectPalette(mdc,palette,0);
            POINT srcsize; PntToPnt(srcsize,pb->SrcSize);
            POINT srcorg; PntToPnt(srcorg,pb->SrcOrigin);

            if(srcorg.x>dib->W)srcorg.x=0;if(srcorg.y>dib->H)srcorg.y=0;
            if((srcsize.x+srcorg.x)>dib->W)srcsize.x=dib->W-srcorg.x;
            if((srcsize.y+srcorg.y)>dib->H)srcsize.y=dib->H-srcorg.y;
            /*
      if (!(dib->filename)){
                         _LpToDpWO(&srcsize,1);
                                 _LpToDpWO(&srcorg,1);
                                }
*/
            StretchBlt(dc,origin.x,origin.y, dstsize.x,dstsize.y,mdc,
                       srcorg.x,srcorg.y, srcsize.x,srcsize.y,rop);

            SelectObject(mdc,hbmpold);
            DeleteDC(mdc);
        }else {
            POINT srcsize; PntToPnt(srcsize,pb->SrcSize);
            POINT srcorg; PntToPnt(srcorg,pb->SrcOrigin);

            if((srcsize.x+srcorg.x)>dib->W)srcsize.x=dib->W-srcorg.x;
            if((srcsize.y+srcorg.y)>dib->H)srcsize.y=dib->H-srcorg.y;

            if (srcsize.y!=dib->H)	 srcorg.y=((dib->H-srcorg.y)-srcsize.y);

            StretchDIBits(dc,origin.x,origin.y,dstsize.x,dstsize.y,
                          srcorg.x,srcorg.y,srcsize.x,srcsize.y,
                          dib->Bits,dib->Info,DIB_RGB_COLORS,rop);

        }
    }else{
        _DrawBmpStub(dc,pb);
    }
};

void _DrawArrow(HDC dc,POINT2D&_p2,POINT2D&_p1,double length,double angle,BOOL f,PLine_2d pl){
    POINT2D p1,p2;
    double a=__GetAngle(_p2,_p1);
    p1=_p1;
    p1.x+=length;
    RotatePoint(p1,_p1,a);
    p2=p1;
    RotatePoint(p1,_p1,M_PI+angle);
    RotatePoint(p2,_p1,M_PI-angle);
    POINT p[4];
    PntToPnt(p[0],p1);
    PntToPnt(p[1],_p1);
    PntToPnt(p[2],p2);
    if (f && pl->pen){
        HGDIOBJ b=CreateSolidBrush(pl->pen->color);
        HGDIOBJ old=SelectObject(dc,b);
        Polygon(dc,p,3);
        SelectObject(dc,old);
        DeleteObject(b);
    }else Polyline(dc,p,3);
}
void TSpace2d::_DrawArrows(HDC dc,PLine_2d pl){
    if(pl->arrows && pl->count){
        if (pl->arrows->Alength!=0){
            _DrawArrow(dc,pl->points[pl->count-2],pl->points[pl->count-1],
                    pl->arrows->Alength,pl->arrows->Aangle,pl->arrows->flags&1,pl);
        }
        if (pl->arrows->Blength!=0){
            _DrawArrow(dc,pl->points[1],pl->points[0],
                    pl->arrows->Blength,pl->arrows->Bangle,pl->arrows->flags&2,pl);
        }
    }
};



void TSpace2d::_DrawPolyLine(HDC dc,PLine_2d pl)
{
    int r2;
    int oldfill;
    BYTE flag=0;
    if(pl->pen && pl->pen->style!=PS_NULL)flag|=1;
    if(pl->brush && pl->brush->style!=BS_NULL)flag|=2;
    if (flag==0)return ;

    SetAnisotropic(dc);
    if ((pl->owner)&&(pl->owner->WhoIsIt()==4))
    {if(pl->owner->items->At(0)==pl) // Чертится группа регионов
        {HGDIOBJ bo,b;
            if (pl->brush){b=_CreateBrush(pl->brush);
                bo=SelectObject(dc,b);
                HRGN rgn=((PRGroup2d)pl->owner)->GetRegion();
                if (pl->brush->rop2!=R2_COPYPEN) r2=SetROP2(dc,pl->brush->rop2);
                PaintRgn(dc,rgn);
                if (pl->brush->rop2!=R2_COPYPEN) SetROP2(dc,r2);
                ::DeleteObject(rgn);
                SelectObject(dc,bo);
                ::DeleteObject(b);
            }
        }}else
    {HGDIOBJ p,b,po,bo;
        if ( flag&1/*pl->pen*/) {p=(HGDIOBJ)_CreatePen(pl->pen);po=SelectObject(dc,p);

        }
        if (flag&2 /*pl->brush*/){b=(HGDIOBJ)_CreateBrush(pl->brush);
            bo=SelectObject(dc,b);

        }
        if (flag&1/*pl->pen*/){
            if (flag&2/*pl->brush*/) {
                if (pl->brush->rop2==pl->pen->rop2)
                {
                    if (pl->brush->rop2!=R2_COPYPEN) r2=SetROP2(dc,pl->brush->rop2);
                    oldfill=SetPolyFillMode(dc,pl->GetRgnCreateMode());
                    Polygon(dc,pl->_points,pl->count);
                    SetPolyFillMode(dc,oldfill);
                    if (pl->brush->rop2!=R2_COPYPEN)       SetROP2(dc,r2);
                }else
                {r2=SetROP2(dc,pl->brush->rop2);
                    HGDIOBJ hp001;
                    hp001=SelectObject(dc,GetStockObject(NULL_PEN));
                    oldfill=SetPolyFillMode(dc,pl->GetRgnCreateMode());
                    Polygon(dc,pl->_points,pl->count);
                    SetPolyFillMode(dc,oldfill);
                    SelectObject(dc,hp001);
                    SetROP2(dc,pl->pen->rop2);
                    Polyline(dc,pl->_points,pl->count);
                    // Если не замкнута
                    POINT pp[2];
                    pp[1]=pl->_points[0];
                    pp[0]=pl->_points[pl->count-1];
                    if (pp[0].x!=pp[1].x || pp[0].y!=pp[1].y)	Polyline(dc,pp,2);
                    SetROP2(dc,r2);
                }
            }
            else {
                if (pl->pen->rop2!=R2_COPYPEN)r2=SetROP2(dc,pl->pen->rop2);
                Polyline(dc,pl->_points,pl->count);
                _DrawArrows(dc,pl);
                if (pl->pen->rop2!=R2_COPYPEN) SetROP2(dc,r2);
            }
        }else
        {HGDIOBJ hp;hp=SelectObject(dc,GetStockObject(NULL_PEN));
            if (pl->brush->rop2!=R2_COPYPEN)
                r2=SetROP2(dc,pl->brush->rop2);
            oldfill=SetPolyFillMode(dc,pl->GetRgnCreateMode());
            Polygon(dc,pl->_points,pl->count);
            SetPolyFillMode(dc,oldfill);
            if (pl->brush->rop2!=R2_COPYPEN)SetROP2(dc,r2);
            SelectObject(dc,hp);
        }
        if (flag&1/*pl->pen*/) {
            SelectObject(dc,po);DeleteObject(p);
        };
        if (flag&2/*pl->brush*/) {
            SelectObject(dc,bo);DeleteObject(b);
        }
    }
    if (pl->options & STATE_POLYEDGE)
    {
        r2 = SetROP2(dc,R2_XORPEN);
        HPEN pen= ::CreatePen(PS_DOT,0,RGB(255,255,255));
        HGDIOBJ old = SelectObject(dc,pen);
        Polyline(dc,pl->_points, pl->count );
        SelectObject(dc,old);
        DeleteObject(pen);
        SetROP2(dc,r2);
    }
    SetTextMode(dc);

    if (pl->options & STATE_EDITING)
    {
        POINT c;
        SelectObject(dc,GetStockObject(WHITE_PEN));

        r2 = SetROP2(dc,R2_XORPEN);
        for(int i = 0; i < pl->count; i++)
        {
            c=pl->_points[i];
            _LpToDp(&c,1);
            _MRect(dc,c);
        }
        SetROP2(dc,r2);
    }
}

void TSpace2d::DIBtoBITMAP(PDib2d pb, HDC dc)
{
    if (pb->bitmap)
        pb->FreeBitmap();

    int mono=(pb->_HANDLE==0) && (pb->NumClrs==2);

    if (mono)
        pb->bitmap = CreateBitmap(pb->imagesize.x, pb->imagesize.y, 1,1, NULL);
    else
        pb->bitmap = CreateCompatibleBitmap(dc, pb->imagesize.x, pb->imagesize.y);

    if (pb->bitmap)
    {
        HDC mdc = CreateCompatibleDC(dc);
        HGDIOBJ hbmpold = SelectObject(mdc,pb->bitmap);
#ifdef WIN32
        SetStretchBltMode(mdc,COLORONCOLOR);//HALFTONE
#else
        SetStretchBltMode(mdc,STRETCH_DELETESCANS);
#endif
        if (palette && !mono) SelectPalette(mdc,palette,0);
        StretchDIBits(mdc,0,0,pb->imagesize.x,pb->imagesize.y,
                      0,0,pb->W,pb->H,pb->Bits,pb->Info,DIB_RGB_COLORS,SRCCOPY);
        SelectObject(mdc,hbmpold);
        DeleteDC(mdc);
    }
}

PGroup TSpace2d::_CreateGroup()
{
    return (PGroup) new TGroup2d();
}

HOBJ2D TSpace2d::Create3dProjection2d(HSP3D hSpace3d,POINT2D& DstOrg,POINT2D& DstSize,
                                      HOBJ2D hCamera)
{
    PSpace2d space2d;
    PSpace3d ps3=::Get3dSpace(hSpace3d,space2d);
    if(ps3 && space2d==this)
    {
        TCamera3d *cam=(TCamera3d *)ps3->GetObjByHANDLE(hCamera);
        if (cam==0 || cam->WhoIsIt()!=11)return FALSE;
        PView3d_2d pv=new TView3d_2d(DstOrg,DstSize,ps3,cam);
        AddObjectRect(pv);
        return  InsertObject(pv);
    }
    return 0;
}

INT16 TSpace2d::SetRgnCreateMode(HOBJ2D rgn,INT16 mode)
{PLine_2d pl=(PLine_2d)GetObjByHANDLE(rgn);
    if ((pl==NULL)||(pl->WhoIsIt()!=20))return 0;
    INT16 old=(INT16)pl->GetRgnCreateMode();
    if (mode==WINDING)pl->options|=128;
    if (mode==ALTERNATE)pl->options&=~128;
    if(pl->brush)AddObjectRect(pl);
    return old;
};
DWORD TSpace2d::SetVectorPen2d(HOBJ2D VectorObj2d,HOBJ2D apen)
{PLine_2d pl=(PLine_2d)GetObjByHANDLE(VectorObj2d);
    if ((pl==NULL)||(pl->WhoIsIt()!=20))return 0;
    if  ((pl->pen && pl->pen->_HANDLE==apen)||
         (pl->pen==NULL && apen==0))return 0;

    Lock();
    AddObjectRect(pl);
    int old=0;PPen2d pp;
    if (apen){
        pp=(PPen2d)pens->GetByHandle(apen);
        if (pp==NULL)return 0;
        pp->IncRef(pl);
    } else pp=NULL;
    if(pl->pen){pl->pen->DecRef(pl);old=pl->pen->_HANDLE;}
    pl->pen=pp;AddObjectRect(pl);
    UnLock();
    return old | 0x10000l;
};
DWORD TSpace2d::SetVectorBrush2d(HOBJ2D VectorObj2d,HOBJ2D abrush)
{PLine_2d pl=(PLine_2d)GetObjByHANDLE(VectorObj2d);
    if ((pl==NULL)||(pl->WhoIsIt()!=20))return 0;
    if  ((pl->brush && pl->brush->_HANDLE==abrush)||
         (pl->brush==NULL && abrush==0))return 0;
    AddObjectRect(pl);
    HOBJ2D old=0;PBrush2d pb;
    if (abrush){
        pb=(PBrush2d)bruhs->GetByHandle(abrush);
        if (pb==NULL)return 0;
        pb->IncRef(pl);
    } else pb=NULL;
    if(pl->brush){pl->brush->DecRef(pl);old=pl->brush->_HANDLE;}
    pl->brush=pb;AddObjectRect(pl);
    return old | 0x10000l;
};

BOOL TSpace2d::SetVectorPoints2d(HOBJ2D VectorObj2d,POINT2D * points,INT16 NumPoints )
{
    PLine_2d pl=(PLine_2d)GetObjByHANDLE(VectorObj2d);
    if ((pl==NULL)||(pl->WhoIsIt()!=20))return FALSE;
    if ((NumPoints>16000) || (NumPoints<0))return FALSE;
    AddObjectRect(pl);
    //????
    if (pl->_points){delete pl->_points;pl->_points=NULL;}
    if (pl->points){delete pl->points;pl->points=NULL;}
    INT16 limit=pl->GetLimit(NumPoints);
    pl->points=new POINT2D[limit];
    memcpy(pl->points,points,sizeof(POINT2D)*limit);
    pl->_points=new POINT[limit];
    pl->count=NumPoints;
    CRDtoLP(pl->points,NumPoints);
    pl->Update_Points();

    pl->UpdateSize();AddObjectRect(pl);
    rectchange=1;
    return TRUE;
};
BOOL TSpace2d::SetVectorPoint2d(HOBJ2D VectorObj2d,INT16 pointNum,POINT2D & _point )
{
    PLine_2d pl=(PLine_2d)GetObjByHANDLE(VectorObj2d);
    if ((pl==NULL)||(pl->WhoIsIt()!=20))return FALSE;
    if ((pointNum>=pl->count)||(pointNum<0))return FALSE;
    POINT2D p=pl->points[pointNum];
    POINT2D point=_point;
    CRDtoLP(&point);
    pl->UpdateSize();
    if (p.x==point.x && p.y==point.y)return TRUE;
    BOOL visible=IsVisible(pl);
    if (visible)AddRect(pl->GetUpdRect(pointNum));
    pl->points[pointNum]=point;
    PntToPnt(pl->_points[pointNum],pl->points[pointNum]);
    point=p;
    // UpdateRect
    pl->UpdateSize();
    if (visible)AddRect(pl->GetUpdRect(pointNum));
    rectchange=1;
    return TRUE;
};
BOOL TSpace2d::DelVectorPoint2d(HOBJ2D VectorObj2d,
                                INT16 pointNum,POINT2D & point )
{
    PLine_2d pl=(PLine_2d)GetObjByHANDLE(VectorObj2d);

    if ((pl==NULL)||(pl->WhoIsIt()!=20))
        return FALSE;

    if(pointNum < 0)
        pointNum = pl->count-1;

    if ((pointNum >= pl->count) || (pointNum<0) || (pl->count==1))
        return FALSE;

    if (IsVisible(pl))AddRect(pl->GetUpdRect(pointNum));
    point=pl->points[pointNum];
    //	pl->points->AtDelete(pointNum);
    //	if (pl->_points){delete pl->_points;pl->_points=NULL;}
    /*
    POINT2D *_p=new POINT2D[pl->count-1];
     C_TYPE i,j;i=j=0;
     while (i<pl->count){ if (i!=pointNum){_p[j]=pl->points[i];j++;}  i++;}
     delete pl->points;pl->points=_p;
     pl->count--;
     pl->_points=new POINT[pl->count];
*/  for(C_TYPE i=pointNum+1;i<pl->count;i++){
        pl->points[i-1]=pl->points[i];
        pl->_points[i-1]=pl->_points[i];
    }
    pl->count--;
    //	 pl->Update_Points();
    pl->UpdateSize();
    rectchange=1;
    return TRUE;
};
BOOL TSpace2d::SetVectorArrows2d(HOBJ2D Object2d,ARROW2D*a){
    PLine_2d pl=(PLine_2d)GetObjByHANDLE(Object2d);
    if ((pl==NULL)||(pl->WhoIsIt()!=20))return FALSE;
    if (pl->arrows){delete pl->arrows;pl->arrows=NULL;
        AddObjectRect(pl);
    }
    if (a && (a->Alength || a->Blength)){
        pl->arrows=new ARROW2D;
        *(pl->arrows)= *a;
        AddObjectRect(pl);
    }
    return TRUE;
};
INT16 TSpace2d::AddVectorPoint2d(HOBJ2D VectorObj2d,INT16 pointNum,POINT2D& _point )
{
    PLine_2d pl=(PLine_2d)GetObjByHANDLE(VectorObj2d);
    if ((pl==NULL)||(pl->WhoIsIt()!=20))return 0;
    if (pointNum==-1)pointNum=pl->count;
    if ((pointNum>pl->count)||(pointNum<0))return 0;
    // UpdateRect
    POINT2D point=_point;
    CRDtoLP(&point);
    POINT2D *points=pl->points;
    if(pl->count>=pl->limit){
        INT16 limit=pl->GetLimit(pl->count+1);
        pl->limit=limit;
        POINT2D *_p=new POINT2D[limit];
        //memset(_p,0,sizeof(POINT2D)*pl->count); //???
        for(int i=pointNum;i<pl->count;i++){
            _p[i+1]=points[i];
        }
        for(i=0;i<pointNum;i++){
            _p[i]=points[i];
        }
        _p[pointNum]=point;
        if (pl->points){delete pl->points;pl->points=NULL;}
        if (pl->_points){delete pl->_points;pl->_points=NULL;}
        pl->points=_p;
        pl->count++;
        pl->_points=new POINT[limit];
        pl->Update_Points();
    }else{
        POINT *_points=pl->_points;
        for(int i=pl->count;i>pointNum;i--){
            points[i]=points[i-1];
            _points[i]=_points[i-1];
        }
        pl->count++;
        points[pointNum]=point;
        _points[i].x=Round(points[i].x);
        _points[i].y=Round(points[i].y);
    }
    if(_point.x<pl->origin.x ||
       _point.y<pl->origin.y ||
       _point.x>(pl->origin.x+pl->size.x) ||
       _point.y>(pl->origin.y+pl->size.y)) pl->UpdateSize();
    if (IsVisible(pl))AddRect(pl->GetUpdRect(pointNum));
    rectchange=1;
    return pl->count;
}

HOBJ2D TSpace2d::CreateRgnGroup(RGNGROUPITEM * items,INT16 NumItems )
{
    C_TYPE i;
    if(!ChkFutureRGroupMember(items,NumItems,0))
        return 0;

    PRGroup2d pg=new TRGroup2d();
    HOBJ2D h=all->InsertObject(pg);

    for(i=0;i<NumItems;i++)
        AddToGroup(h,items[i].Object);

    for(i=0; i<pg->items->count; i++)
    {
        PLine_2d pl=(PLine_2d) pg->items->At(i);
        pl->SetRgnCombineMode(items[i].Rop);
        AddRect(pl->GetRect());// не ObjectRect
    }
    return h;
}

BOOL TSpace2d::AddRgnGroupItem(HOBJ2D group2d,INT16 ItemNum,RGNGROUPITEM * Object2d )
{PRGroup2d obj=(PRGroup2d)GetObjByHANDLE(group2d);
    PLine_2d pl1=(PLine_2d)GetObjByHANDLE(Object2d->Object);
    if ((obj)&&(obj->WhoIsIt()==4)&&(ItemNum>-1)
        &&(ItemNum<=obj->items->count)&&(pl1)&&(pl1->WhoIsIt()==20))
    {AddObjectRect(pl1);AddObjectRect(obj);
        obj->items->AtInsert(ItemNum,pl1);
        pl1->owner=obj;AddObjectRect(obj);
        return TRUE;
    }
    return FALSE;
};
BOOL TSpace2d::SetRgnGroupItem(HOBJ2D group2d,INT16 ItemNum,RGNGROUPITEM * Object2d )
{
    PRGroup2d obj = (PRGroup2d)GetObjByHANDLE(group2d);
    PLine_2d pl1 = (PLine_2d)GetObjByHANDLE(Object2d->Object);

    if ((obj)&&(obj->WhoIsIt()==4)&&(ItemNum>-1) &&
        (ItemNum<obj->items->count)&&(pl1)&&(pl1->WhoIsIt()==20))
    {
        PLine_2d pl = ((PLine_2d)obj->items->At(ItemNum));
        if (pl1->owner==NULL)
        {
            AddObjectRect(obj);
            AddObjectRect(pl1);
            pl->owner=NULL;
            AddObjectRect(pl1);
            pl1->owner=obj;
            pl1->SetRgnCombineMode(Object2d->Rop);
            obj->items->AtPut(ItemNum,pl1);
            AddObjectRect(obj);
            return pl->_HANDLE;
        }
        else
            if (pl1==pl)
            {
                AddObjectRect(obj);
                pl->SetRgnCombineMode(Object2d->Rop);
                AddObjectRect(obj);
                return pl->_HANDLE;
            }
    }
    return 0;
}
BOOL TSpace2d::SetRgnGroupItems(HOBJ2D group,RGNGROUPITEM * Objects2d,INT16 NumObjects )
{PRGroup2d obj=(PRGroup2d)GetObjByHANDLE(group);
    if ((obj)&&(obj->WhoIsIt()==4)&&(NumObjects>-1))
    {
        if(!ChkFutureRGroupMember(Objects2d,NumObjects,group))return 0;
        HOBJ2D * objects;
        objects=new HOBJ2D[NumObjects];
        C_TYPE i;
        for (i=0;i<NumObjects;i++)objects[i]=Objects2d[i].Object;
        RECT r=obj->GetRect();
        BOOL h=_SPACE::SetGroup(group,objects,NumObjects);
        if (h)
        {Lock();
            AddRect(r);
            for (i=0;i<NumObjects;i++)
                ((PLine_2d)obj->items->At(i))->SetRgnCombineMode(Objects2d[i].Rop);
            AddObjectRect(obj);
            UnLock();
        }
        delete objects;
        return h;
    }
    return 0;
};

BOOL TSpace2d::GetRgnGroupItems(HOBJ2D group2d,RGNGROUPITEM * Objects2d,INT16 MaxObjects )
{PRGroup2d obj=(PRGroup2d)GetObjByHANDLE(group2d);
    if ((obj)&&(obj->WhoIsIt()==4))
    {C_TYPE i,j;j=min(MaxObjects,obj->items->count);
        for(i=0;i<j;i++)
        {PLine_2d pl=((PLine_2d)obj->items->At(i));
            Objects2d[i].Object=pl->_HANDLE;
            Objects2d[i].Rop=pl->GetRgnCombineMode();
        }
        return obj->_HANDLE;
    }
    return 0;
};

BOOL TSpace2d::GetRgnGroupItem(HOBJ2D group2d,INT16 ItemNum,RGNGROUPITEM * Object2d )
{PRGroup2d obj=(PRGroup2d)GetObjByHANDLE(group2d);
    if ((obj)&&(obj->WhoIsIt()==4)&&(ItemNum>-1)
        &&(ItemNum<obj->items->count))
    {
        PLine_2d pl=((PLine_2d)obj->items->At(ItemNum));
        Object2d->Object=pl->_HANDLE;
        Object2d->Rop=(INT16)pl->GetRgnCombineMode();return TRUE;
    }
    return FALSE;
};
BOOL TSpace2d::ChkFutureRGroupMember(RGNGROUPITEM * Objects2d,INT16 NumObjects,HOBJ2D owner )
{INT16 i;PObject obj;
    for(i=0;i<NumObjects;i++)
    { obj=GetObjByHANDLE(Objects2d[i].Object);
        if ((obj==NULL)||(obj->WhoIsIt()!=20)) return 0;
        if ((obj->owner!=NULL)&&(owner==0))    return 0;
        if ((obj->owner!=NULL)&&(obj->owner->_HANDLE!=owner))
            return 0;
    }
    return TRUE;
}
C_TYPE TSpace2d::DeleteFromGroup(HOBJ2D group,HOBJ2D object)
{ PGroup2d grp=(PGroup2d)GetObjByHANDLE(group);
    PObject2d obj=(PObject2d)GetObjByHANDLE(object);
    RECT r;
    if(grp && grp->WhoIsIt()==4)  r=grp->GetRect();
    C_TYPE h=_SPACE::DeleteFromGroup(group,object);
    if ((h)&&(grp->WhoIsIt()==4)){
        Rect_or_Rect(r,obj->GetRect());
        Rect_or_Rect(r,grp->GetRect());
        AddRect(r);
    }
    return h;
}

BOOL TSpace2d::DeleteGroup(HOBJ2D g)
{
    PGroup2d grp = (PGroup2d)GetObjByHANDLE(g);
    if (grp->WhoIsIt() == 4)
    {
        C_TYPE j = grp->items->count;
        pointer* p = new pointer[j];
        Lock();
        AddObjectRect(grp);

        for(int i = 0; i < j; i++)
            p[i] = grp->items->At(i);

        if ((Frame)&&(IsObjectInObject(grp,Frame->object)))
            DeleteEditFrame2d();

        BOOL h = _SPACE::DeleteGroup(g);

        if (h)
        {
            for(i = 0; i < j; i++)
                AddObjectRect((PObject2d)p[i]);
        }
        UnLock();
        delete p;
        return h;
    }
    return _SPACE::DeleteGroup(g);
};

HOBJ2D TSpace2d::SetDibObject2d(HOBJ2D Object2d,HOBJ2D DibIndex )
{
    PBitmap_2d obj=(PBitmap_2d)GetObjByHANDLE(Object2d);
    PDib2d pd=GetDib2d(DibIndex,1);
    if (pd && obj && obj->WhoIsIt()==21)
    {//pd=(PDib2d)(pd->GetMainObject());
        HOBJ2D h=obj->dib->_HANDLE;	obj->dib->DecRef(obj);
        obj->dib=pd;	pd->IncRef(obj);
        AddObjectRect(obj);
        return h;
    }
    return 0;
};

HOBJ2D TSpace2d::SetDoubleDibObject2d(HOBJ2D Object2d,HOBJ2D DoubleDibIndex )
{PDoubleBitmap_2d obj=(PDoubleBitmap_2d)GetObjByHANDLE(Object2d);
    PDoubleDib2d pd=GetDoubleDib2d(DoubleDibIndex,1);
    if (pd && obj && obj->WhoIsIt()==22)
    {//pd=(PDoubleDib2d)(pd->GetMainObject());
        HOBJ2D h=obj->dib->_HANDLE;obj->dib->DecRef(obj);
        obj->dib=pd;pd->IncRef(obj);
        AddObjectRect(obj);
        return h;
    }
    return 0;
};
BOOL TSpace2d::SetTextDelta2d(HOBJ2D Object2d,POINT2D &p){
    PText_2d obj=(PText_2d)GetObjByHANDLE(Object2d);
    if (obj && obj->WhoIsIt()==23){
        POINT2D po=obj->Delta;
        obj->Delta=p;p=po;
        AddObjectRect(obj);
        return 1;
    }return 0;
};
HOBJ2D TSpace2d::SetTextObject2d(HOBJ2D Object2d,HOBJ2D TextIndex )
{PText_2d obj=(PText_2d)GetObjByHANDLE(Object2d);
    PText2d pt=(PText2d)texts->GetByHandle(TextIndex);
    if (pt && obj && obj->WhoIsIt()==23)
    {HOBJ2D h=obj->text->_HANDLE;obj->text->DecRef(obj);
        obj->text=pt;pt->IncRef(obj);
        AddObjectRect(obj);
        return h;
    }
    return 0;
};
HSP3D TSpace2d::Set3dSpace2d(HOBJ2D Object2d, HSP3D hSpace3d )
{/*PView3d_2d pv=(PView3d_2d)GetObjByHANDLE(Object2d);
    PSpace3d ps3=NULL;//(PSpace3d)spaces3d->GetByHandle(hSpace3d & 255);
    if ((ps3)&&(pv)&&(pv->WhoIsIt()==24))
     {int h=pv->space3d->_HANDLE;
      pv->space3d->views->Delete(pv);
      pv->space3d=ps3;ps3->views->Insert(pv);
      pv->UpdateRect(NULL);
      return h;
     } */
    return 0;
};
BOOL  TSpace2d::SetBitmapSrcRect(HOBJ2D Object2d ,POINT2D & o,POINT2D & s)
{
    _TBMP2d* obj=(_TBMP2d*)GetObjByHANDLE(Object2d);
    if (obj)
    {
        int type=obj->WhoIsIt();
        if(type==21 || type==22)
        {
            PDib2d dib=(PDib2d)((PBitmap_2d)obj)->dib->GetMainObject();
            if(!dib) return FALSE;
            POINT2D oldO = obj->SrcOrigin, oldS = obj->SrcSize;
            POINT2D mx;
            if(type==21)
            {
                mx.x=dib->W;
                mx.y=dib->H;
            }
            else
                if(type==22)
                {
                    mx.x=dib->W;
                    mx.y=dib->H;
                }
                else return 0;

            if ((o.x>mx.x)||(o.y>mx.y))return 0;
            if (o.x<0)o.x=0;
            if (o.y<0)o.y=0;
            if ((o.x+s.x)>mx.x)s.x=mx.x-o.x;
            if ((o.y+s.y)>mx.y)s.y=mx.y-o.y;
            obj->SrcOrigin=o;
            obj->SrcSize=s;
            AddObjectRect(obj);
            o=oldO;s=oldS;
            return TRUE;
        }
    }
    return FALSE;
};

C_TYPE TSpace2d::ObjectToEnd2d(HOBJ2D Object2d ,C_TYPE top)
{
    pointer p=GetObjByHANDLE(Object2d);
    if (p)
    {
        C_TYPE i=primary->IndexOf(p);
        if (i>-1)
        {
            primary->AtDelete(i);
            if (top)
                primary->Insert(p);
            else
                primary->AtInsert(0,p);
            AddObjectRect((PObject2d)p);
            return (C_TYPE)(i+1);
        }
    }
    return 0;
};

BOOL TSpace2d::SwapObject(HOBJ2D First2d,HOBJ2D Second2d )
{ pointer p1=GetObjByHANDLE(First2d);
    pointer p2=GetObjByHANDLE(Second2d);
    if ((p1)&&(p2))
    { C_TYPE i1=primary->IndexOf(p1);
        C_TYPE i2=primary->IndexOf(p2);
        if ((i1>-1)&&(i2>-1))
        { primary->Swap(i1,i2);
            Lock();
            AddObjectRect((PObject2d)p1);
            AddObjectRect((PObject2d)p2);
            UnLock();
            return 1;}
    }
    return 0;
};

C_TYPE TSpace2d::SetZOrder(HOBJ2D Object2d, C_TYPE ZOrder)
{
    pointer p = GetObjByHANDLE(Object2d);
    C_TYPE z = primary->IndexOf(p);

    if ((z > -1) && (ZOrder > 0) && (ZOrder <= primary->count))
    {
        primary->Delete(p);
        primary->AtInsert((C_TYPE)(ZOrder-1),p);
        AddObjectRect((PObject2d)p);
        return (C_TYPE)(z+1);
    }
    return 0;
};

C_TYPE TSpace2d::GetZOrder(HOBJ2D Object2d )
{
    C_TYPE z=GetPrimaryOrder(Object2d);
    if (z>-1) return (C_TYPE)(z+1);return 0;
}
HOBJ2D TSpace2d::CreatePen(INT16 Style,INT16 Width, COLORREF rgb,INT16 Rop2 )
{
    IsToolOverflow(pens);
    TPen2d * t = new TPen2d(rgb, Style, Width, Rop2, 0);
    CHECK_FOR_DUPLICATE(pens, TPen2d, IsPensEqual)

}

HOBJ2D TSpace2d::CreateBrush(INT16 Style,INT16 Hatch,
                             COLORREF rgb,HOBJ2D DibIndex,INT16 Rop2 )
{IsToolOverflow(dibs)
            PDib2d pd=NULL;
    if (DibIndex){
        pd=GetDib2d(DibIndex,1);
        if (pd==NULL)return 0;
    }
    if((Style==BS_SOLID)&&(pd==NULL))
    { Style==BS_SOLID;
    }

    TBrush2d *t=new TBrush2d(Style,rgb,pd,Hatch,Rop2);
    CHECK_FOR_DUPLICATE(bruhs,TBrush2d,IsBrushsEqual)
};

HFONT  TSpace2d::_CreateFont(PFont2d pf,INT16 angle)
{
    if(activefont==pf && angle==activefontangle)
        return _activefont;

    if(_activefont)DeleteObject(_activefont);

    activefontangle=angle;
    activefont=pf;
    int old=pf->font2d.lfEscapement;
    pf->font2d.lfEscapement=angle;
    _activefont=::CreateFontIndirect(pf->font);
    pf->font2d.lfEscapement=old;
    return _activefont;
};

HPEN  TSpace2d::_CreatePen(PPen2d pp)
{
    return ::CreatePen(pp->style,pp->width,GetGDIColor(pp->color));
};

HBRUSH  TSpace2d::_CreateBrush(PBrush2d pb)
{
    if (pb->dib)
    {PDib2d dib=(PDib2d)(pb->dib->GetMainObject());
        if(dib){
            HBRUSH h=CreateDIBPatternBrush(dib->dib,DIB_RGB_COLORS);
            if(h)UnrealizeObject(h);
            return h;
        }
    }
    LOGBRUSH lb;

    if(pb->color&0x01000000L){
        lb.lbHatch = 0;
        lb.lbStyle = BS_NULL;
        lb.lbColor = 0;
    }else{
        if ((pb->style==BS_SOLID)&&(palette))
        {
            COLORREF cl=GetGDIColor(pb->color);
            int nColor = GetNearestPaletteIndex(palette,cl);
            return  CreateSolidBrush(PALETTEINDEX(nColor));
        }
        lb.lbHatch = pb->hatch;
        lb.lbStyle = pb->style;
        lb.lbColor = GetGDIColor(pb->color);
    }

    return CreateBrushIndirect(&lb);
};

DWORD TSpace2d::SetPenStyle(HOBJ2D index,INT16 Style )
{ PPen2d pp=(PPen2d)pens->GetByHandle(index);
    if (pp){
        INT16 i=pp->style;
        INT16 j=(Style==PS_SOLID)&&(i!=PS_SOLID)&&(pp->width>0);
        if (!j) ChkPen(pp);
        /* На тот случай, если карандаш становится толще за счет того что он
     изменил стиль
 */	  pp->style=Style;
        if (j) ChkPen(pp);
        return i |0x10000l;
    }else return 0;
};
DWORD TSpace2d::SetPenWidth(HOBJ2D index,INT16 Width )
{ PPen2d pp=(PPen2d)pens->GetByHandle(index);
    if (pp){
        INT16 i=pp->width;
        if (i>Width)ChkPen(pp);
        pp->width=Width;
        if (i<=Width)ChkPen(pp); // Обновление в самом толстом случае
        return i |0x10000l;
    }else return 0;
};
DWORD TSpace2d::SetPenRop2(HOBJ2D index,INT16 Rop2 )
{ PPen2d pp=(PPen2d)pens->GetByHandle(index);
    if (pp){
        int i=pp->rop2;
        pp->rop2=Rop2;
        ChkPen(pp);
        return i |0x10000l;
    }else return 0;
};
COLORREF TSpace2d::SetPenColor(HOBJ2D index, COLORREF rgb )
{ PPen2d pp=(PPen2d)pens->GetByHandle(index);
    if (pp){
        COLORREF i=pp->color;
        pp->color=rgb;
        ChkPen(pp);
        return i |0x1000000l;
    }else return 0;
};

DWORD TSpace2d::SetBrushStyle(HOBJ2D index,INT16 Style )
{
    PBrush2d pb=(PBrush2d)bruhs->GetByHandle(index);
    if (pb)
    {
        INT16 i=pb->style;
        pb->style=Style;
        ChkBrush(pb);
        return i |0x10000l;
    }
    else
        return 0;
};

DWORD TSpace2d::SetBrushDibIndex(INT16 index,HOBJ2D DibIndex )
{ PBrush2d pb=(PBrush2d)bruhs->GetByHandle(index);
    if (pb){
        INT16 i=0;
        if(pb->dib) i=pb->dib->_HANDLE;
        PDib2d pd=NULL;
        if (DibIndex) pd=GetDib2d(DibIndex,1);
        if ((pd==NULL)&&(DibIndex))return 0l;
        if(pb->dib)pb->dib->DecRef(pb);
        pb->dib=pd;
        if(pb->dib)pb->dib->IncRef(pb);
        ChkBrush(pb);
        return i |0x10000l;
    }else return 0l;
};
DWORD TSpace2d::SetBrushHatch(HOBJ2D index,INT16 Hatch )
{ PBrush2d pb=(PBrush2d)bruhs->GetByHandle(index);
    if (pb){
        int i=pb->hatch;
        pb->hatch=Hatch;
        ChkBrush(pb);
        return i |0x10000l;
    }else return 0;
};
COLORREF  TSpace2d::SetBrushColor(HOBJ2D index, COLORREF rgb )
{ PBrush2d pb=(PBrush2d)bruhs->GetByHandle(index);
    if (pb){
        COLORREF i=pb->color;
        pb->color=rgb;
        ChkBrush(pb);
        return i |0x1000000l;
    }else return 0;
};
DWORD TSpace2d::SetBrushRop2(HOBJ2D index, INT16 Rop2 )
{ PBrush2d pb=(PBrush2d)bruhs->GetByHandle(index);
    if (pb){
        int i=pb->rop2;
        pb->rop2=Rop2;
        ChkBrush(pb);
        return i |0x10000l;
    }else return 0;
};

DWORD TSpace2d::SetBrushPoints(HOBJ2D index,double x0,double y0,double x1,double y1)
{
    PBrush2d pb=(PBrush2d)bruhs->GetByHandle(index);
    if(!pb) return 0;

    pb->gradient_x0=x0;
    pb->gradient_y0=y0;
    pb->gradient_x1=x1;
    pb->gradient_y1=y1;

    ChkBrush(pb);
    return 1;
};

DWORD TSpace2d::SetBrushColors(HOBJ2D index,COLORREF* colors,int colors_count)
{
    PBrush2d pb=(PBrush2d)bruhs->GetByHandle(index);
    if(!pb) return 0;

    if(pb->gradient_colors)delete[] pb->gradient_colors;
    pb->gradient_colors_count=colors_count;
    pb->gradient_colors=colors;

    ChkBrush(pb);
    return 1;
};

HOBJ2D TSpace2d::CreateString2d(LPSTR String )
{
    IsToolOverflow(strings);
    TString2d* t = new TString2d(String);
    CHECK_FOR_DUPLICATE(strings, TString2d, IsStringsEqual)
}

BOOL TSpace2d::SetLogString2d(HOBJ2D index, LPSTR String )
{PString2d ps=(PString2d)strings->GetByHandle(index);
    if (ps){
        if(lstrcmp(ps->string,String)){
            //	 ChkString(ps);
            if (ps->string){delete ps->string;ps->string=NULL;}
            ps->string=NewStr(String);
            ps->UpdateFlags();
            ChkString(ps);
        }
        return TRUE;
    }
    return 0;
};

HOBJ2D TSpace2d::CreateFont2d(LOGFONT2D * LogFont )
{
    IsToolOverflow(fonts);
    TFont2d *t = new TFont2d(LogFont);
    CHECK_FOR_DUPLICATE(fonts, TFont2d, IsFontsEqual);
};

BOOL TSpace2d::SetLogFont2d(HOBJ2D index, LOGFONT2D * Logfont )
{
    PFont2d pf=(PFont2d)fonts->GetByHandle(index);
    if (pf)
    {
        ChkFont(pf);
        pf->font2d=(*Logfont);
        pf->tmValid=FALSE;
        //ChkFont(pf);
        return TRUE;
    }
    return FALSE;
}

BOOL TSpace2d::CheckNewText(LOGTEXT * Text,INT16 NumItems )
{
    int i;
    for(i=0;i<NumItems;i++)
    {
        if (!fonts->GetByHandle(Text[i].ltFontIndex))return 0;
        if (!strings->GetByHandle(Text[i].ltStringIndex))return 0;
    }
    return 1;
}

void TSpace2d::__CrTXD_7gS(PText2d pt,LOGTEXT * Text,INT16 NumItems )
{
    C_TYPE i;
    for(i=0;i<NumItems;i++)
    {
        PTextRecord pr=new TextRecord(&Text[i]);
        pr->text2d=pt;
        pr->font=(PFont2d)fonts->GetByHandle(Text[i].ltFontIndex);
        pr->string=(PString2d)strings->GetByHandle(Text[i].ltStringIndex);
        pr->font->IncRef(pt);
        pr->string->IncRef(pt);
        pt->text->Insert(pr);
    }
}

HOBJ2D TSpace2d::CreateText(LOGTEXT * Text,INT16 NumItems )
{
    IsToolOverflow(texts)
            if(!CheckNewText(Text,NumItems ))
            return 0;
    PText2d t=new TText2d();
    __CrTXD_7gS(t,Text,NumItems );
    CHECK_FOR_DUPLICATE(texts,TText2d,IsTextsEqual)
};

BOOL TSpace2d::SetLogText2d(HOBJ2D index, LOGTEXT* Text,INT16 NumItems,INT16 _m)
{
    INT16 i;
    if(!CheckNewText(Text,NumItems ))return 0;
    PText2d pt=(PText2d)texts->GetByHandle(index);
    if (pt)
    {
        if (_m)
        {
            NewMetaRecord(mfsetlogtext);
            meta->WriteWord(index);
            meta->WriteWord(NumItems);
            meta->Write(Text,NumItems*sizeof(LOGTEXT));
            NewUndoRecord(mfsetlogtext);
            meta->WriteWord(index);
            meta->WriteWord(pt->text->count);
            for(i=0;i<pt->text->count;i++)
                pt->text->PutItem(meta,pt->text->At(i));
            CloseMetaRecord();
        }
        pt->text->FreeAll();
        __CrTXD_7gS(pt,Text,NumItems );
        ChkText(pt);
        return TRUE;
    }
    return FALSE;
};

BOOL TSpace2d::ChkLOGTEXT(LOGTEXT * Text)
{if (!fonts->GetByHandle(Text->ltFontIndex))return FALSE;
    if (!strings->GetByHandle(Text->ltStringIndex))return FALSE;
    return TRUE;
};

/* Все ChkXXX - проверяют какой-либо инструмент, и если
 он используется объектом, то  соответствующая
 область на экране обновляется*/
void TSpace2d::ChkPen(PPen2d p)
{
    if (p){
        TRef2d*r=p->_RefObject;
        while(r){
            AddObjectRect((TObject*)(r->object));
            r=r->next;
        }
    }
    /*
  PPen2d pp=(PPen2d)pens->GetByHandle(index);
     if ((pp)&&(!pp->IsZeroRef())){
      for(C_TYPE i=0;i<primary->count;i++)
        {PLine_2d pl=(PLine_2d)primary->At(i);
          if(pl->WhoIsIt()==20)
            if (pl->pen==pp)
            AddObjectRect(pl);
        }
     }*/
};

void TSpace2d::ChkBrush(PBrush2d p)
{
    if(!p)return;

    TRef2d*r=p->_RefObject;
    while(r)
    {
        if(r->type==otLINE2D)
            AddObjectRect((TObject*)(r->object));
        else {Invalidate();return;}
        r=r->next;
    }

    /*
  PBrush2d pp=(PBrush2d)bruhs->GetByHandle(index);
     if ((pp)&&(!pp->IsZeroRef())){
      if (pp==BkBrush){Invalidate();return;}
      for(C_TYPE i=0;i<primary->count;i++)
        {PLine_2d pl=(PLine_2d)primary->At(i);
          if(pl->WhoIsIt()==20)
            if (pl->brush==pp)AddObjectRect(pl);
        }
     }
 */
};
void TSpace2d::ChkDib(PDib2d p)
{
    if (p)
    {
        TRef2d*r=p->_RefObject;
        while(r)
        {
            if(r->type==otBITMAP2D)AddObjectRect((TObject*)(r->object));
            if(r->type==ttBRUSH2D)ChkBrush((PBrush2d)(r->object));
            r=r->next;
        }
    }

    /*
 PDib2d pp=GetDib2d(index);
     if ((pp)&&(!pp->IsZeroRef()))
     {C_TYPE i;
      for(i=0;i<primary->count;i++) //Check to Bitmaps2d
        {PBitmap_2d pb=(PBitmap_2d)primary->At(i);
         if ((pb->WhoIsIt()==21)&&(pb->dib==pp))
          {AddObjectRect(pb);}
        }
      for(i=0;i<bruhs->count;i++)
        {PBrush2d pb=(PBrush2d)bruhs->At(i);
         if ((pb->dib==pp))ChkBrush(pb->_HANDLE);
        }// Check for brushs what used this dib
     }
  */
};

void TSpace2d::ChkDoubleDib(PDoubleDib2d p)
{
    if (p){
        TRef2d*r=p->_RefObject;
        while(r){
            AddObjectRect((TObject*)(r->object));
            r=r->next;
        }
    }


    /*
 PDoubleDib2d pp=GetDoubleDib2d(index);
     if ((pp)&&(!pp->IsZeroRef()))
     {C_TYPE i;
      for(i=0;i<primary->count;i++) //Check to Bitmaps2d
        {PDoubleBitmap_2d pd=(PDoubleBitmap_2d)primary->At(i);
         if ((pd->WhoIsIt()==22)&&(pd->dib==pp))
          {AddObjectRect(pd);
          }
        }
    }
*/
};
void TSpace2d::ChkString(PString2d ps)
{
    if (ps){
        TRef2d*r=ps->_RefObject;
        while(r){
            ChkText((PText2d)r->object);
            r=r->next;
        }
    }


    /*PString2d ps=(PString2d)strings->GetByHandle(index);
     if ((ps)&&(!ps->IsZeroRef()))
     {C_TYPE i;PText2d pt;
      for (i=0;i<texts->count;i++)
        {pt=(PText2d)texts->At(i);
          C_TYPE j;
          for (j=0;j<pt->text->count;j++){
          PTextRecord ptr=(PTextRecord)pt->text->At(j);
          if (ptr->string==ps)
            {ChkText(pt->_HANDLE);j=pt->text->count;}
            }
        }
     }
   */
};
void TSpace2d::ChkFont(PFont2d pf)
{
    if (pf){
        TRef2d*r=pf->_RefObject;
        while(r){
            ChkText((PText2d)r->object);
            r=r->next;
        }
    }

    /*
 PFont2d pf=(PFont2d)fonts->GetByHandle(index);
     if ((pf)&&(!pf->IsZeroRef()))
     {C_TYPE i;PText2d pt;
      for (i=0;i<texts->count;i++)
        {pt=(PText2d)texts->At(i);
          C_TYPE j;
          for (j=0;j<pt->text->count;j++){
          PTextRecord ptr=(PTextRecord)pt->text->At(j);
          if (ptr->font==pf)
            {ChkText(pt);j=pt->text->count;}
            }
        }
     }
    */
};
void TSpace2d::ChkText(PText2d pt)
{
    if (pt){
        TRef2d*r=pt->_RefObject;
        while(r){
            AddObjectRect((TObject*)(r->object));
            r=r->next;
        }
    }
    /*
 PText2d pt=(PText2d)texts->GetByHandle(index);
     if ((pt)&&(!pt->IsZeroRef())){
      C_TYPE i;
      for(i=0;i<primary->count;i++)
        {PText_2d px=(PText_2d)primary->At(i);
          if(px->WhoIsIt()==23)
            if (px->text==pt)AddObjectRect(px);
        }
     }
  */
};
INT16 TSpace2d::DeleteUnusedTool(TOOL_TYPE what,PStream ps)
{ PToolsCollection pt=GetToolsById(what);
    if(pt==NULL)return 0;
    return pt->DeleteUnused(ps);
};

int TSpace2d::DeleteTool2d(TOOL_TYPE what,HOBJ2D index)
{
    PToolsCollection pt=GetToolsById(what);
    if(pt==NULL)return 0;
    return pt->_DeleteObject(index);
};

PToolsCollection TSpace2d::GetToolsById(TOOL_TYPE t)
{
    switch (t){
        case 1:return pens;
        case 2:return bruhs;
        case 3:return dibs;
        case 4:return ddibs;
        case 5:return fonts;
        case 6:return strings;
        case 7:return texts;
            //			 case 8:return (PToolsCollection)spaces3d;
    };
    return NULL;
}

HOBJ2D TSpace2d::CreatePolyline2d(HOBJ2D Pen,HOBJ2D Brush,POINT2D * points, INT16 NumPoints )
{
    if((NumPoints>500)||(NumPoints<1) || ((Pen==0) && (Brush==0)))
        return 0;

    PBrush2d pb = NULL;
    PPen2d   pp = NULL;

    if (Pen)
    {
        pp = (PPen2d)pens->GetByHandle(Pen);
        if (pp==NULL)
            return 0;
    }
    if (Brush)
    {
        pb = (PBrush2d)bruhs->GetByHandle(Brush);
        if (pb==NULL)
            return 0;
    }

    PLine_2d pl= new TLine_2d(pp, pb, points, NumPoints, this);
    //CRDtoLP(&Origin);
    // MK_FP(points[i].x,points[i].y);
    rectchange=1;
    AddObjectRect(pl);
    return InsertObject(pl);
}


HOBJ2D TSpace2d::CreateBitmap2d(HOBJ2D DibIndex, POINT2D& SrcOrg, POINT2D& SrcSize,
                                POINT2D& DstOrg, POINT2D& DstSize,INT16/* angle */)
{
    PDib2d pd=GetDib2d(DibIndex,1);
    if(pd)
    {
        PDib2d dib=(PDib2d)pd->GetMainObject();
        if(dib)
        {
            if (SrcSize.x<0)SrcSize.x=dib->W;
            if (SrcSize.y<0)SrcSize.y=dib->H;
            if (DstSize.x<0)DstSize.x=SrcSize.x;
            if (DstSize.y<0)DstSize.y=SrcSize.y;
        }
        POINT2D _DstOrg=DstOrg;
        CRDtoLP(&_DstOrg);
        PBitmap_2d pb=new TBitmap_2d(SrcOrg,SrcSize,_DstOrg,DstSize,pd);
        AddObjectRect(pb);
        rectchange=1;
        return InsertObject(pb);
    }
    return 0;
};


HOBJ2D TSpace2d::CreateDoubleBitmap2d(HOBJ2D DibIndex, POINT2D& SrcOrg, POINT2D& SrcSize,
                                      POINT2D& DstOrg, POINT2D& DstSize,INT16 /*angle*/)
{
    PDoubleDib2d pd = GetDoubleDib2d(DibIndex,1);
    if(pd)
    {
        PDoubleDib2d dib=(PDoubleDib2d)pd->GetMainObject();
        if(dib)
        {
            if (SrcSize.x<0)SrcSize.x=dib->W;
            if (SrcSize.y<0)SrcSize.y=dib->H;
            if (DstSize.x<0)DstSize.x=SrcSize.x;
            if (DstSize.y<0)DstSize.y=SrcSize.y;
        }
        POINT2D _DstOrg=DstOrg;
        CRDtoLP(&_DstOrg);
        PDoubleBitmap_2d pb= new TDoubleBitmap_2d(SrcOrg,SrcSize,_DstOrg,DstSize,pd);
        AddObjectRect(pb);
        rectchange=1;
        return InsertObject(pb);
    }
    return 0;
};
HOBJ2D TSpace2d::CreateUserObject2d(USEROBJSTRUCT *us,USERTYPESTRUCT*ut){

    POINT2D p;
    p.x=us->x;
    p.y=us->y;
    POINT2D _p=p;
    CRDtoLP(&p);
    TUserObject2d*o=new TUserObject2d(us,ut);
    us->x=_p.x;
    us->y=_p.y;
    o->space=this;
    AddObjectRect(o);
    o->data.handle=InsertObject(o);
    rectchange=1;
    return o->data.handle;
};



HOBJ2D TSpace2d::CreateControl2d(char const* lpszClassName,
                                 char const* lpszWindowName,
                                 DWORD dwStyle,
                                 int x,  int y,  int nWidth,
                                 int nHeight,INT16 id,  DWORD ExdwStyle)
{
    if (hwnd)
    {
        dwStyle|=0x04000000L;
        POINT2D p,ps;
        p.x=x;p.y=y;
        CRDtoLP(&p);
        x=p.x;y=p.y;

        DWORD exstyle=0;
        if(!lstrcmpi(lpszClassName,"BUTTON"))
        {
            if((dwStyle&0x0f) == BS_AUTORADIOBUTTON)
            {
                dwStyle&=~0x0fL;
                dwStyle |= BS_RADIOBUTTON;
            }
        }
        else
        {
#ifdef WIN32
            if(dwStyle & WS_BORDER)
            {
                exstyle |= WS_EX_CLIENTEDGE;
            }
#endif
        }

        ps.x=nWidth;ps.y=nHeight;
        _LpToDp(&p,1);
        _LpToDpWO(&ps,1);

        HWND control = CreateWindowEx(exstyle,lpszClassName, lpszWindowName, dwStyle,
                                    p.x, p.y, ps.x, ps.y,hwnd,(HMENU)id,hInstance,NULL);
        if (control)
        {
            TControl2d*c = new TControl2d(this,control,x,y,dwStyle,ExdwStyle);
            rectchange=1;
            return InsertObject(c);
        }
    }
    return 0;
}

HOBJ2D TSpace2d::CreateTextRaster2d(HOBJ2D TextIndex,POINT2D& Org, POINT2D& Size,
                                    POINT2D& Delta,INT16 DstAngle  )
{
    PText2d pt = (PText2d)texts->GetByHandle(TextIndex);
    if (pt)
    {
        POINT2D _Org=Org;
        CRDtoLP(&_Org);
        PText_2d pt2=new TText_2d(_Org,Size,pt);
        pt2->Delta = Delta;
        pt2->angle = DstAngle;

        if (Size.x<0 || Size.y<0)
        {
            POINT2D s = CalcTextSize(pt2);
            if (Size.x < 0)
                pt2->size.x = s.x;
            if (Size.y<0)
                pt2->size.y = s.y;
        }
        AddObjectRect(pt2);
        //        pt2->UpdateSize();
        rectchange=1;
        return InsertObject(pt2);
    }
    return 0;
}

//******************** Dib2d ******************
HOBJ2D TSpace2d::CreateDib(HGLOBAL Dib )
{
    IsToolOverflow(dibs);
    PDib2d pd=new TDib2d(Dib);
    if (pd->dib)
        return dibs->InsertObject(pd);
    else
    {
        delete pd;
        return 0;
    }
}

int TSpace2d::SetDib(HOBJ2D DibIndex, HGLOBAL Dib )
{
    PDib2d pd =GetDib2d(DibIndex);
    if (pd==NULL)return 0;
    pd=(PDib2d)pd->GetMainObject();
    if(pd)
    {
        ChkDib(pd);
        int r=pd->SetDib(Dib);
        if (r)ChkDib(pd);
        return r;
    }
    else	return FALSE;
}

//*************** DoubleDib2d *****************
// DoubleDib2d
HOBJ2D TSpace2d::CreateDoubleDib(HGLOBAL XorDib,HGLOBAL AndDib )
{
    IsToolOverflow(ddibs);
    PDoubleDib2d pd=new TDoubleDib2d(XorDib, AndDib);
    if (pd->dib)
    {
        return ddibs->InsertObject(pd);
    }
    else
    {
        delete pd;
        return 0;
    }
};
BOOL TSpace2d::SetDoubleDib(HOBJ2D DoubleDibIndex,HGLOBAL XorDib, HGLOBAL AndDib )
{PDoubleDib2d pd =GetDoubleDib2d(DoubleDibIndex);
    if (pd==NULL)return 0;
    pd=(PDoubleDib2d)pd->GetMainObject();
    if(!pd)return FALSE;
    ChkDoubleDib(pd);
    int r=pd->SetDibs(XorDib,AndDib);
    if (r)ChkDoubleDib(pd);
    return r;
};
HOBJ2D TSpace2d::GetFrameCursor(POINT2D& p)
{if (Frame)
    {
        POINT2D pp[5];INT16 i,j=0;
        POINT2D mp,cp;mp.y=32000;mp.x=32000;
        for (i=0;i<4;i++)pp[i]=Frame->tops[i];
        pp[4]=pp[0];
        POINT2D ps=Frame->psize;
        _DpToLpWO(&ps,1);ps.x=ps.x/2;ps.y=ps.y/2;
        int xx,yy;
        for(i=0;i<8;i++){
            if (i<4)cp=pp[i];else
            {cp.x=(pp[i-4].x+pp[i-3].x)/2;cp.y=(pp[i-4].y+pp[i-3].y)/2;}
            xx=cp.x-p.x;if (xx<0)xx=-xx;
            yy=cp.y-p.y;if (yy<0)yy=-yy;
            if ((xx<=mp.x)&&(yy<=mp.y)){mp.x=xx;mp.y=yy;j=i+1;};
        }
        if (j)
            if ((mp.x>ps.x)||(mp.y>ps.y))j=0;
        return (HOBJ2D)j;
    }
    return 0;
};

POINT2D TSpace2d::GetFramePoint(int d)
{POINT2D p;p.x=0;p.y=0;
    if (Frame)
    {if (d<5)return Frame->tops[d-1];
        p.x=(Frame->tops[d-5].x+Frame->tops[d-4].x)/2;
        p.y=(Frame->tops[d-5].y+Frame->tops[d-4].y)/2;
    }
    return p;
};
PSpace3d TSpace2d::CreateSpace3d()
{if (spaces3d->count>16384)return 0;
    PSpace3d ps3=new TSpace3d();
    ps3->space2d=this;
    spaces3d->InsertObject(ps3);
    return ps3;
};
/*
int TSpace2d::DeleteSpace3d(HSP3D h)
 {PSpace3d ps3=(PSpace3d)spaces3d->GetByHandle(LOBYTE(h));
  if (ps3->views->count) return 0;
     spaces3d->Free(ps3);
    return h;
 };
*/

INT32 TSpace2d::IsIntersect(HOBJ2D h1,HOBJ2D h2,UINT32 f)
{
    TObject2d*po1=(TObject2d*)GetObjByHANDLE(h1);
    TObject2d*po2=(TObject2d*)GetObjByHANDLE(h2);
    if(po1 && po2)
    {
        RECT r1=po1->GetRect(0);
        RECT r2=po2->GetRect(0);
        if(RectIntersect(r1,r2))
            return 1;
    }
    return FALSE;
};

BYTE TSpace2d::_NeedAwaken=1;
PCollection TSpace2d::_dibs=new TCollection(2,1);
PCollection TSpace2d::_ddibs=new TCollection(2,1);
char* TSpace2d::currentpath=NULL;

DWORD TSpace2d::SetSpaceParam(UINT command, DWORD data)
{
    switch(command)
    {
        case SP_SETLAYERS:
            layers = data;
            Invalidate();
            break;

        case SP_GETLAYERS:
            return layers;

        case SP_SETDEFFLAGS:
            defaultflags = data;
            break;

        case SP_GETDEFFLAGS:
            return defaultflags;

        case SP_SETSPACEFLAG:
        {
            WORD st = State;
            State = (WORD)data;

            if ((st & SF_GRID) != (State & SF_GRID))
                Invalidate();
            return st;
        }
        case SP_GETSPACEFLAG:
            return State;

        case SP_SETREADONLY:
            readonly=data;
            return 1;

        case SP_GETREADONLY:
            return readonly;

        default:
            return 0;
    };
    modifyed=TRUE;
    return 1;
}
//*---------------------------------------------------------*

WORD TSpace2d::paint_flags=0;

