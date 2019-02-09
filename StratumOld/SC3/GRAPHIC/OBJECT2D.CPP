/*
Copyright (c) 1995  Virtual World
Module Name:
     object2d.cpp
Author:
     Alexander Shelemekhov
*/

#include <windows.h>
//#include "aarot.hpp" //для вращения bitmap
#include "space2d.h"
#include "space3d.h"
#include <math.h>
#include <mem.h>
#include <gwin2d3d\3drender.h>
#include "rotator3.h"
#include "_system.h"
#include "camera3d.h"

//************** 2d Objects *******************
void ChkRDibToUnload()
{
    C_TYPE i=0;PDib2d pd;
    while (i<TSpace2d::_dibs->count)
    {
        pd=(PDib2d)TSpace2d::_dibs->At(i);
        if(pd->IsZeroRef())
        {
            TSpace2d::_dibs->Free(pd);
        }
        else i++;
    }
};

void ChkRDoubleDibToUnload()
{C_TYPE i=0;PDoubleDib2d pd;
    while (i<TSpace2d::_ddibs->count)
    {pd=(PDoubleDib2d)TSpace2d::_ddibs->At(i);
        if (pd->IsZeroRef()){TSpace2d::_ddibs->Free(pd);}else i++;
    }
};

TObject2d::TObject2d():TObject()
{
    origin.x=0;origin.y=0;
    size.x=0;size.y=0;//ischanged=0;
    angle=0;
    alpha=1000;
}

TObject2d::TObject2d(POINT2D aorg)
{
    origin=aorg;
    size.x=0;
    size.y=0;
    angle=0;
    alpha=1000;
}

TObject2d::TObject2d(PStream ps,LONG _pos):TObject(ps,_pos)
{
    ps->ReadPOINT(&origin);
    ps->ReadPOINT(&size);
    owner=NULL;
    angle=0;
    alpha=1000;
    //ischanged=0;
};

TObject2d::~TObject2d()
{};

RECT  TObject2d::GetRect(int)
{
    RECT r;
    r.left=origin.x;r.top=origin.y;
    r.right=ceil(origin.x+size.x);
    r.bottom=ceil(origin.y+size.y);
    return r;
};

POINT2D  TObject2d::Move2d(POINT2D p)
{
    POINT2D op=origin;
    origin=p;
    return op;
    //origin.x+=delta.x;origin.y+=delta.y;
}

void  TObject2d::Rotate2d(POINT2D base,double f)
{
    POINT2D p,d;
    d.x=size.x/2;d.y=size.y/2;
    p.x=origin.x+d.x;
    p.y=origin.y+d.y;
    RotatePoint(p,base,f);
    origin.x=p.x-d.x;
    origin.y=p.y-d.y;
};

void  TObject2d::Store(PStream ps)
{
    TObject::Store(ps);
    ps->Write(&origin,sizeof(origin));
    ps->Write(&size,sizeof(size));
};

POINT2D TObject2d::SetSize2d(POINT2D s)
{
    POINT2D os=size;
    size=s;
    return os;
};

void MarkToolsToSave(PTools2d pt,int m)
{
    if (pt)
    {
        if (m)
            pt->flag|=128;
        else
            pt->flag&=127;
    }
}

void  TObject2d::SetState(WORD astate, ATTR_MODE op, pointer p )
{
    BOOL v1=((PSpace2d)p)->IsVisible(this);
    RECT r=GetRect();
    TObject::SetState(astate,op,p );
    BOOL v2=((PSpace2d)p)->IsVisible(this);
    if ( v1 != v2 )
    {
        ((PSpace2d)p)->AddRect(r);
        ((PSpace2d)p)->AddRect(GetRect());
        ((PSpace2d)p)->rectchange=1;
    }
}

int   TObject2d::GetDistToMouse(POINT2D p)
{
    double f=GetAngle();
    if (f==0.0)
    {
        if ((size.x==0)||(size.y==0))
            return 32760;

        if ((p.x>=origin.x) && (p.y>=origin.y) && (p.x<=origin.x+size.x) && (p.y<=origin.y+size.y))
            return 0;

        int x1=p.x-(origin.x+size.x);
        int y1=p.y-(origin.y+size.y);
        int x2=p.x-origin.x;
        int y2=p.y-origin.y;

        x1=(x1<0)?32760:x1;
        x2=(x2<0)?32760:x2;
        y1=(y1<0)?32760:y1;
        y2=(y2<0)?32760:y2;
        x1=min(x1,x2);
        y1=min(y1,y2);
        x1=max(x1,y1);
        return x1;
    }
    {
        RECT r=TObject2d::GetRect();
        HRGN rgn=GetRotateRectRGN(r,f);
        int pir=PtInRegion(rgn,p.x,p.y);
        ::DeleteObject(rgn);
        if (pir)return 0;else return 32760;
    }
};

BOOL TObject2d::Awaken(_SPACE*){return 1;};
void TObject2d::UpdateSize(int,void*){};
void TObject2d::StoreCoordinates(PStream ps)
{
    ps->WritePOINT(&origin);
    ps->WritePOINT(&size);
};
void TObject2d::LoadCoordinates(PStream ps)
{
    ps->ReadPOINT(&origin);
    ps->ReadPOINT(&size);
};


//*--------------------------------------------------------*
TLine_2d::TLine_2d(PPen2d apen,PBrush2d abrush, POINT2D * apoints,INT16 NumPoints,TSpace2d*space):TObject2d()
{
    arrows=NULL;
    pen=apen;
    brush=abrush;
    count=NumPoints;

    if (pen)   pen->IncRef(this);
    if (brush) brush->IncRef(this);
    if (NumPoints)
    {
        limit=GetLimit(count);
        points=new POINT2D[limit];
        _points=new POINT[limit];
        memcpy(points,apoints,NumPoints*sizeof(POINT2D));
        space->CRDtoLP(points,NumPoints);
        Update_Points();
    }
    else
    {
        points=NULL;
        _points=NULL;
    }
    UpdateSize();
}

//-------------
INT16 TLine_2d::GetLimit(INT16 count)
{
    if(count<10)return 10;
    if(count<32)return 32;
    if(count<64)return 64;
    return 50*ceil((int)(count)/50.0);
};

//-------------
TLine_2d::TLine_2d(PStream ps,LONG _pos) : TObject2d(ps,_pos)
{
    arrows=NULL;

    pen  =(PPen2d) (void*)MAKELONG(ps->ReadWord(),0);
    brush=(PBrush2d)(void*)MAKELONG(ps->ReadWord(),0);

    count =ps->ReadWord();
    limit=GetLimit(count);
    points=new POINT2D[limit];

    _points=new POINT[limit];
    ps->ReadPOINTs(points,count);

    if (ps->fileversion>0x0200)
    {
        BYTE _size;
        ps->Read(&_size,1);

        if (_size)
        {
            arrows=(ARROW2D*)new BYTE[_size];
            memset(arrows,0,_size);
            ps->Read(arrows,_size);
        }
    }
    Update_Points();
    //UpdateSize();
};

void TLine_2d::Update_Points()
{
    for(int i=0;i<count;i++)
    {
        _points[i].x=Round(points[i].x);
        _points[i].y=Round(points[i].y);
        //PntToPnt(_points[i],points[i]);
    }
};

TLine_2d::~TLine_2d()
{
    if (HIWORD(pen))   	{pen->DecRef(this);pen=0;}
    if (HIWORD(brush)) 	{brush->DecRef(this);brush=0;}
    if (arrows)					{delete[] arrows;arrows=0;}
    if (points)					{delete[] points;points=0;}
    if (_points)				{delete[] _points;_points=0;}
};

RECT TLine_2d::GetRect(int FromPaint)
{
    RECT r=TObject2d::GetRect();
    r.right++;r.bottom++;
    //if(r.left==r.right)r.right++;
    //if(r.bottom==r.top)r.bottom++;
    if ((owner)&&(owner->WhoIsIt()==4))
    {
        if(owner->items->At(0)==this)
        {
            r=((PObject2d)owner)->GetRect(); //? че к чему
        }
        else
        {
            if(FromPaint)
            {
                r.left=r.right=r.top=r.bottom=0;
                return r;
            }
        }
    }
    _UpRect(r);
    return r;
};

void TLine_2d::_UpRect(RECT & r)
{
    POINT2D d1,d2;
    d1.x=0;
    d1.y=0;
    d2 = d1;
    if (pen)
    {
        if( pen->width > 0 )
        {
            int d=pen->width/2;
            if(pen->width&0x1) d++;
            d1.x=d;d1.y=d;
        }
        else
        {
            d1.x=1;
            d1.y=1;
        }
        if(arrows)
        {
            gr_float l = max(fabs(arrows->Alength), fabs(arrows->Blength));
            d1.x+=l+1;	d1.y+=l+1;
        };
    }
    if(options & 2)
        d2 = r_size;

    d1.x=max(d1.x,d2.x);
    d1.y=max(d1.y,d2.y);

    r.left -= d1.x;
    r.top -= d1.y;
    r.right += d1.x;
    r.bottom += d1.y;
}

void TLine_2d::_UpRect(RECT2D & r)
{
    POINT2D d1,d2;
    d1.x=0;
    d1.y=0;
    d2=d1;
    if (pen)
    {
        if( pen->width > 0 )
        {
            int d = pen->width / 2;
            if(pen->width & 0x1)
                d++;

            d1.x=d;
            d1.y=d;
        }
        else
        {
            d1.x=1;
            d1.y=1;
        }
        if(arrows)
        {
            //gr_float l=max(fabs(arrows->Alength),fabs(arrows->Blength));
            //d1.x+=l+1;	d1.y+=l+1;
        };
    }
    if (options & 2)
        d2 = r_size;
    d1.x = max(d1.x,d2.x);
    d1.y = max(d1.y,d2.y);

    r.left -= d1.x;
    r.top -= d1.y;
    r.right += d1.x;
    r.bottom += d1.y;
}

void TLine_2d::SetState(WORD astate,ATTR_MODE op,pointer p)
{
    int old=options;
    RECT r=GetRect();
    TObject2d::SetState(astate,op,p );
    if (((old & 2) != (options &2)) &&((PSpace2d)p)->IsVisible(this))
    {
        ((PSpace2d)p)->AddRect(r);
        ((PSpace2d)p)->AddRect(GetRect());
    }
}

POINT2D TLine_2d::Move2d(POINT2D p)
{
    POINT2D op=TObject2d::Move2d(p);
    POINT2D delta;
    delta.x = p.x - op.x;
    delta.y = p.y - op.y;

    for(int i=0; i<count; i++)
        MovePoint(points[i],delta);

    Update_Points();
    return op;
}

void TLine_2d::Rotate2d(POINT2D base,double a)
{
    for(int i=0; i < count; i++)
        RotatePoint(points[i], base, a);

    //angle-=((double)1800*a)/M_PI;
    //Update_Points();
    UpdateSize();
}

void TLine_2d::UpdateSize(int,void* p)
{
    RECT2D r;
    Update_Points();
    r.left=points[0].x;
    r.top =points[0].y;
    r.right=r.left;
    r.bottom=r.top;
    //int i;
    for(int i=0;i<count;i++)
    {
        AddPointToRect(r,points[i]);
        //PntToPnt(_points[i],points[i]);
        _points[i].x=Round(points[i].x);
        _points[i].y=Round(points[i].y);
    }
    actualorigin.x=r.left;
    actualorigin.y=r.top;
    actualsize.x=r.right-r.left;
    actualsize.y=r.bottom-r.top;
    r_size.x=2;r_size.y=2;
    if (p)((PSpace2d)p)->_DpToLpWO(&r_size,1);
#ifdef NEWLINESIZE
    _UpRect(r);
    if(r.left<=0)
    {
        r.left++;
        if(r.right<=0) r.right++;
    }
    if(r.top<=0)
    {
        r.top++;
        if(r.bottom<=0) r.bottom++;
    }
    if (pen)
        if(pen->width&0x1)
        {	r.left++;	r.top++; 	}
#endif
    origin.x=r.left;
    origin.y=r.top;
    size.x=r.right-r.left; //+1
    size.y=r.bottom-r.top; //+1
};

RECT TLine_2d::GetUpdRect(int point)
{
    RECT r;
    r.left=_points[point].x;
    r.top =_points[point].y;
    r.right=r.left+1;
    r.bottom=r.top+1;
    if (point>0)
        AddPointToRect(r,points[point-1]);
    else
    {
        if ((brush)||((owner)&&(owner->WhoIsIt()==4)))
            AddPointToRect(r,points[count-1]);
    }
    if (point<count-1)
        AddPointToRect(r,points[point+1]);
    else
    {
        if ((brush)||((owner)&&(owner->WhoIsIt()==4)))
            AddPointToRect(r,points[0]);
    }
    _UpRect(r);
    if (!(options & 2))
    {r.left--;r.top--;r.right++;r.bottom++;}
    return r;
};

POINT2D TLine_2d::SetSize2d(POINT2D newSize)
{
    UpdateSize();
    double kx, ky;
#ifdef NEWLINESIZE
    POINT2D d;
    d.x=(size.x-actualsize.x);
    d.y=(size.y-actualsize.y);
    if(size.x && ns.x)kx=(ns.x-d.x)/(size.x-d.x);else kx=0.0;
    if(size.y && ns.y)ky=(ns.y-d.y)/(size.y-d.y);else ky=0.0;
#else
    if(size.x)
        kx=(newSize.x)/(size.x);
    else
        kx=0.0;

    if(size.y)
        ky = (newSize.y)/(size.y);
    else
        ky=0.0;
#endif

    for(int i=0;i<count;i++)
    {
        //points[i].x=(points[i].x-origin.x)*kx+origin.x;
        //points[i].y=(points[i].y-origin.y)*ky+origin.y;
        points[i].x = (points[i].x-actualorigin.x)*kx+actualorigin.x;  //изменил
        points[i].y = (points[i].y-actualorigin.y)*ky+actualorigin.y;  //изменил
    }
    Update_Points();
    POINT2D old=size;
    //size=ns;
    UpdateSize();
    return old;
};

void TLine_2d::MarkToSave(int m)
{
    TObject::MarkToSave(m);
    MarkToolsToSave(pen,m);
    MarkToolsToSave(brush,m);
    if(brush && brush->dib) MarkToolsToSave(brush->dib,m);
};

void TLine_2d::Store(PStream ps)
{
    TObject2d::Store(ps);
    if (pen)ps->WriteWord(pen->_HANDLE);			else ps->WriteWord(0);
    if (brush)ps->WriteWord(brush->_HANDLE);	else ps->WriteWord(0);
    ps->WriteWord(count);
    ps->WritePOINTs(points,count);
    BYTE s=arrows?sizeof(ARROW2D):0;
    ps->Write(&s,1);
    if (arrows)
    {
        ps->Write(arrows,sizeof(ARROW2D));
    }
};

int  TLine_2d::GetDistToMouse(POINT2D p)
{
    double d=5;
    if(pen)
        d += pen->width/2;

    //int d=TObject2d::GetDistToMouse(p);
    if (((origin.x-d)<=p.x)&&((origin.y-d)<=p.y)&&
        ((origin.x+size.x+d)>=p.x)&&((origin.y+size.y+d)>=p.y))
    {
        int i;
        BYTE _org=(owner)&&(owner->WhoIsIt()==4);
        if (((brush)||(_org))&&count>2)
        {
            HRGN rg;
            if (_org)
                rg = ((PRGroup2d)owner)->GetRegion();
            else
                rg = CreatePolygonRgn(_points,count,GetRgnCreateMode());

            int _hr = PtInRegion(rg, p.x, p.y);
            ::DeleteObject(rg);
            if(_hr)
                return 0;
        }
        {
            for(i = 0; i < count - 1; i++)
                if(PointNearLine(points[i],points[i+1],p,d-6))
                    return 0;

            if(brush)
                if(brush->style!=BS_NULL)
                    if(PointNearLine(points[0],points[count-1],p,d-6))
                        return 0;
        }
    }
    return 32700;
};

BOOL TLine_2d::Awaken(_SPACE* ps)
{
    PSpace2d sp=(PSpace2d)ps;
    if (pen)
    {
        pen =(PPen2d) sp->pens->GetByHandle((HOBJ2D)pen);
        if(pen) 	pen->IncRef(this);		else	return 0;
    }
    if (brush)
    {
        brush =(PBrush2d) sp->bruhs->GetByHandle((HOBJ2D)brush);
        if(brush)	brush->IncRef(this);	else 	return 0;
    }
    return 1;
};

WORD TLine_2d::WhoIsIt(){ return 20; };

void TLine_2d::StoreCoordinates(PStream ps)
{
    ps->WriteWord(count);
    ps->WritePOINTs(points,count);
};

void TLine_2d::LoadCoordinates(PStream ps)
{
    int i=ps->ReadWord();
    if (i!=count)
        _Error(EM_internal);

    ps->ReadPOINTs(points,count);
    Update_Points();
    UpdateSize();
};



//-------------------------------------------------
/*
class _TBMP2d:public TObject2d{
 POINT2D SrcOrigin;
 POINT2D SrcSize;
*/
_TBMP2d::_TBMP2d(POINT2D SrcOrg, POINT2D aSrcSize,POINT2D DstOrg,
                 POINT2D DstSize,PTools2d pt):TObject2d()
{
    SrcOrigin=SrcOrg;SrcSize=aSrcSize;
    origin=DstOrg; size=DstSize;angle=0;
    dib=pt;
};

_TBMP2d::_TBMP2d(PStream ps,LONG _pos):TObject2d(ps,_pos)
{
    ps->ReadPOINT(&SrcOrigin);
    ps->ReadPOINT(&SrcSize);
    angle=ps->ReadWord();
    dib=(PDib2d)(void*)MAKELONG(ps->ReadWord(),0);
};

void _TBMP2d::Store(PStream ps)
{
    TObject2d::Store(ps);
    ps->WritePOINT(&SrcOrigin);
    ps->WritePOINT(&SrcSize);
    ps->WriteWord(angle);
    ps->WriteWord(dib->_HANDLE);
};

_TBMP2d::~_TBMP2d()
{
    if (HIWORD(dib))dib->DecRef(this);
};

void _TBMP2d::MarkToSave(int m)
{
    TObject::MarkToSave(m);
    MarkToolsToSave(dib,m);
};

TBitmap_2d::TBitmap_2d(POINT2D SrcOrg, POINT2D aSrcSize,POINT2D DstOrg, POINT2D DstSize,PDib2d pd)
    :_TBMP2d(SrcOrg,aSrcSize,DstOrg,DstSize,pd)
{
    dib->IncRef(this);
}

void TBitmap_2d::Rotate2d(POINT2D p,double a)
{
#ifdef USECAIRO
    RotatePoint(origin,p,a);
    angle-=(1800.0*a)/M_PI;
#else
    TObject2d::Rotate2d(p,a);
#endif
}

RECT TBitmap_2d::GetRect(int FromPaint)
{
    RECT r=TObject2d::GetRect(FromPaint);
    if (angle)
    {
        RotateRect(r,GetAngle());
    }
    return r;
};

BOOL TBitmap_2d::Awaken(_SPACE* ps)
{
    dib=(((PSpace2d)ps)->GetDib2d((WORD)dib,1));
    if(!dib)return 0;
    dib->IncRef(this);
    return 1;
};

WORD TBitmap_2d::WhoIsIt(){ return 21;};

TDoubleBitmap_2d::TDoubleBitmap_2d(POINT2D SrcOrg, POINT2D aSrcSize,POINT2D DstOrg, POINT2D DstSize,
                                   PDoubleDib2d pd):_TBMP2d(SrcOrg,aSrcSize,DstOrg,DstSize,pd)
{
    dib->IncRef(this);
};


void TDoubleBitmap_2d::Rotate2d(POINT2D p,double a)
{
#ifdef USECAIRO
    RotatePoint(origin,p,a);
    angle-=(1800.0*a)/M_PI;
#else
    TObject2d::Rotate2d(p,a);
#endif
}

RECT TDoubleBitmap_2d::GetRect(int FromPaint)
{
    RECT r=TObject2d::GetRect(FromPaint);
    if (angle)
    {
        RotateRect(r,GetAngle());
    }
    return r;
};

int TDoubleBitmap_2d::GetDistToMouse(POINT2D p)
{
    if (TObject2d::GetDistToMouse(p)==0)
    {
        PDoubleDib2d db=(PDoubleDib2d)(dib->GetMainObject());
        if(!db)return 0;
        HBITMAP hb=db->andbitmap->bitmap;
        if (hb)
        {
            RotatePoint(p,origin,-GetAngle());
            HDC dc=CreateCompatibleDC(NULL);
            SelectObject(dc,hb);
            POINT2D pp=p;
            pp.x=SrcOrigin.x+(int)(((long)(p.x-origin.x))*((long)(SrcSize.x))/size.x);
            pp.y=SrcOrigin.y+(int)(((long)(p.y-origin.y))*((long)(SrcSize.y))/size.y);
            long pixel=GetPixel(dc,pp.x,pp.y);
            DeleteDC(dc);
            if ((pixel & 255)==0) return 0;
            return 32760;
        }
        return 0;
    }
    return 32760;
};


BOOL TDoubleBitmap_2d::Awaken(_SPACE* ps)
{
    dib=(((PSpace2d)ps)->GetDoubleDib2d((WORD)dib,1));
    if (dib){dib->IncRef(this); return 1;}
    return 0;
};

WORD TDoubleBitmap_2d::WhoIsIt(){return 22;};


TBrush2d::~TBrush2d()
{
    if (HIWORD(dib))
        dib->DecRef(this);
};


RECT  TGroup2d::GetRect(int)
{
    RECT r;r.left=0;r.right=0;r.top=0;r.bottom=0;
    C_TYPE i;
    for(i=0;i<items->count;i++)
        Rect_or_Rect(r,((TObject2d*)items->At(i))->GetRect());
    return r;
};

POINT2D  TGroup2d::Move2d(POINT2D p)
{
    C_TYPE i;
    POINT2D o = GetOrigin2d();
    for(i = 0; i < items->count; i++)
    {
        TObject2d* po = (TObject2d*)items->At(i);
        POINT2D p1 = po->GetOrigin2d();

        p1.x = p.x + (p1.x - o.x);
        p1.y = p.y + (p1.y - o.y);
        po->Move2d(p1);
    }
    return o;
};

void  TGroup2d::Rotate2d(POINT2D p,double f)
{
    for(C_TYPE i = 0; i < items->count; i++)
        ((TObject2d*)items->At(i))->Rotate2d(p,f);
};

POINT2D TGroup2d::GetOrigin2d()
{
    POINT2D p;
    p.x=0;
    p.y=0;
    if (items->count>0)
    {
        RECT2D r;
        p = ((TObject2d*)items->At(0))->GetOrigin2d();
        r.left = p.x;
        r.right = p.x;
        r.top = p.y;
        r.bottom = p.y;
        C_TYPE i;

        for(i = 1; i < items->count; i++)
            AddPointToRect(r,((TObject2d*)items->At(i))->GetOrigin2d());

        p.x=r.left;
        p.y=r.top;
    }
    return p;
};

POINT2D TGroup2d::GetSize2d()
{
    POINT2D p,p1;p.x=0;p.y=0;
    if (items->count>0)
    {
        RECT2D r,r1;
        TObject2d* po=(TObject2d*)items->At(0);
        p=po->GetOrigin2d();
        p1=po->GetSize2d();
        r.left=p.x;r.top=p.y;
        r.right=p.x+p1.x;r.bottom=p.y+p1.y;
        C_TYPE i;
        for(i=1;i<items->count;i++)
        {
            po=(TObject2d*)items->At(i);
            p=po->GetOrigin2d();
            p1=po->GetSize2d();
            r1.left=p.x;			r1.top=p.y;
            r1.right=p.x+p1.x;	r1.bottom=p.y+p1.y;
            r.left=min(r.left,r1.left);r.right=max(r.right,r1.right);
            r.top=min(r.top,r1.top);r.bottom=max(r.bottom,r1.bottom);
        }
        p.x=r.right-r.left;p.y=r.bottom-r.top;
    }
    return p;
}

POINT2D TGroup2d::SetSize2d(POINT2D p)
{
    POINT2D oldsize = GetSize2d();
    POINT2D oldorg = GetOrigin2d();

    if ((oldsize.x==p.x)&&(oldsize.y==p.y))
        return p;

    C_TYPE i;
    for(i=0;i<items->count;i++)
    {
        TObject2d* po=(TObject2d*)items->At(i);
        POINT2D oo=po->GetOrigin2d();
        POINT2D os=po->GetSize2d();
        POINT2D o,s;

        o.x = (oldorg.x+ ((oo.x-oldorg.x)*(p.x)/(oldsize.x)));
        o.y = (oldorg.y+ ((oo.y-oldorg.y)*(p.y)/(oldsize.y)));

        s.x=(os.x*(p.x)/(oldsize.x));
        s.y=(os.y*(p.y)/(oldsize.y));

        po->SetSize2d(s);
        po->Move2d(o);
    }
    return oldsize;
}

RECT TRGroup2d::GetRect(int)
{
    RECT r;r.left=0;r.right=0;r.top=0;r.bottom=0;
    C_TYPE i;
    for(i=0;i<items->count;i++){
        TObject2d* po=((TObject2d*)items->At(i));
        POINT2D p=po->GetOrigin2d();
        POINT2D p1=po->GetSize2d();
        RECT r1;r1.left=p.x;r1.top=p.y;r1.right=p.x+p1.x;r1.bottom=p.y+p1.y;
        Rect_or_Rect(r,r1);
    }
    r.right++;
    r.bottom++;
    return r;
};

HRGN TRGroup2d::GetRegion()
{
    C_TYPE i;
    int combine_mode;
    if (items->count)
    {
        PLine_2d pl=(PLine_2d)(items->At(0));

        HRGN region=CreatePolygonRgn(pl->_points,
                                     pl->count,pl->GetRgnCreateMode());
        for(i=1; i < items->count; i++)
        {combine_mode=pl->GetRgnCombineMode();
            pl=(PLine_2d)(items->At(i));
            HRGN r1=CreatePolygonRgn(pl->_points,
                                     pl->count,pl->GetRgnCreateMode());
            HRGN hrgnDest = CreateRectRgn(0, 0, 0, 0);
            CombineRgn(hrgnDest, region, r1,combine_mode);
            DeleteObject(region);DeleteObject(r1);
            region=hrgnDest;
        }
        return region;
    }
    return 0;
};
int TLine_2d::GetRgnCombineMode()
{
    switch (options & 96)
    {
        case 32:
            return RGN_AND;

        case 64:
            return RGN_XOR;

        case 96:
            return RGN_DIFF;
    }
    return RGN_OR;
}

int TLine_2d::SetRgnCombineMode(int mode)
{
    options &= ~96;
    int old = GetRgnCombineMode();
    switch (mode)
    {
        case RGN_AND:
            options |=32;
            break;

        case RGN_XOR:
            options |=64;
            break;

        case RGN_DIFF:
            options|=96;
            break;

        case RGN_OR:
            break;
    }
    return old;
}

int TLine_2d::GetRgnCreateMode()
{
    if (options&128)
        return WINDING;
    return ALTERNATE;
}
//*---------------------------------------------------------*
/*
class TView3d_2d : public TObject2d{
 public:
 TSpace3d * space3d;
 TSpace2d * space2d;
 TPoint3d SrcOrg;
 TPoint3d SrcSize;
 double alpha;
 double beta;
 int render;
// PView3d view;
// XOR буффер
// AND буффер
*/

void TSpace3d::UpdateObject(PObject3d po,BYTE flags)
{
    INT16 i=po->WhoIsIt();
    if ((i==52)||(i==53))
    {
        if (views->count)
        {
            space2d->UpdateSpecObject((PObject2d)po);
        }return;
    }
    if ((i==10)||(i==5))
    {
        if (!space2d->IsVisible(po))
            return;

        if (i==5)
        {
            PGroup pg = (PGroup)po;
            for(INT16 j=0;j<pg->items->count;j++){
                UpdateObject((PObject3d)pg->items->At(j),flags);
            }
        }else{
            if (views->count){
                C_TYPE index=primary->IndexOf(po);
                for(C_TYPE i=0;i<views->count;i++){
                    TView3d_2d* v=(TView3d_2d*)views->At(i);
                    _TObject3d*p=(_TObject3d*)v->items->At(index);
                    RECT r;
                    if(v->bounded){
                        p->GetBoundsRect(NULL,NULL,&r);
                        v->UpdateRect(&r,UF_SCREEN|UF_BUFFER);
                    }else{
                        if(flags&UOF_BUFFER){
                            r.left=(p->org.x);
                            r.top=(p->org.y);
                            r.right=r.left+(p->size.x)+2;
                            r.bottom=r.top+(p->size.y)+2;
                            v->UpdateRect(&r);
                        }
                        if((po->options & STATE_BOUNDS) || (flags & UOF_BOUNDS))
                        {
                            p->GetBoundsRect(NULL,NULL,&r);
                            //InflateRect(&r,2,2);
                            v->UpdateRect(&r,UF_SCREEN);
                        }}
                    if((po->options&STATE_AXIS3D) || (flags&UOF_AXIS)){
                        v->GetAxisRect(p,&r);
                        v->UpdateRect(&r,UF_SCREEN);
                    }
                } }} }
};


PObject TSpace3d::GetObjByHANDLE(HOBJ2D h)
{if (h<OID_RESERVED)return _SPACE::GetObjByHANDLE(h);
    if ((h>=OID_FRAME3d1) && (h<=OID_FRAME3d) && (views->count!=0))
        return space2d->Frame3d;
    if ((h==OID_AXIS3d)&& (views->count!=0))return space2d->Axis3d;
    return NULL;
};

void TSpace3d::UpdateAll()
{
    C_TYPE i;
    for (i=0;i<views->count;i++)
    {
        PView3d_2d pv=(PView3d_2d)views->At(i);
        pv->UpdateRect(NULL);
    }
};


TText_2d::TText_2d(POINT2D org,POINT2D sz,PText2d pt)
{
    size=sz;
    origin=org;
    text=pt;
    text->IncRef(this);
    angle=0;
};

TText_2d::~TText_2d()
{
    if (HIWORD(text))
        text->DecRef(this);
};

void TText_2d::MarkToSave(int m)
{
    TObject::MarkToSave(m);
    MarkToolsToSave(text,m);
    for (C_TYPE i=0;i<text->text->count;i++)
    {
        PTextRecord pt=(PTextRecord)text->text->At(i);
        MarkToolsToSave(pt->string,m);
        MarkToolsToSave(pt->font,m);
    }
};

RECT TText_2d::GetRect(int FromPaint)
{
    RECT r=TObject2d::GetRect(FromPaint);
    if (angle)
    {
        RotateRect(r,GetAngle());
    }
    return r;
};

TText_2d::TText_2d(PStream ps, LONG _pos) :TObject2d(ps,_pos)
{
    text = (PText2d)(void*)MAKELONG(ps->ReadWord(),0);

    ps->ReadPOINT(&Delta);
    angle=ps->ReadWord();
};

void TText_2d::Rotate2d(POINT2D p,double a)
{
    // TObject2d::Rotate2d(p,a);
    RotatePoint(origin,p,a);
    angle -= ((double)1800*a) / M_PI;
    UpdateSize();
};

void TText_2d::UpdateSize(int,void*)
{

    /*	if (text->text->count)
    {
    TEXTMETRIC tm;
    PTextRecord pr=(PTextRecord)(text->text->At(0));
        HFONT _font=CreateFontIndirect(pr->font->font);
        HDC dc=GetDC(0);
        SelectObject(dc,_font);
        if(GetTextMetrics(dc,&tm))
    {
            size.x=tm.tmAveCharWidth*pr->string->StrLen()-tm.tmOverhang+100;
      size.y=tm.tmHeight + tm.tmExternalLeading;
      char s[1000];
      sprintf(s,"%g, %g",size.x,size.y);
        //MessageBox(0,s,"",0);
    }
        //long l=GetTextExtent(dc,pr->string->GetString(),pr->string->StrLen());
        //size.x=LOWORD(l);
    //size.y=HIWORD(l);
        ReleaseDC(0,dc);
    }
*/};

void TText_2d::Store(PStream ps)
{
    TObject2d::Store(ps);
    ps->WriteWord(text->_HANDLE);
    ps->WritePOINT(&Delta);
    ps->WriteWord(angle);
};

BOOL TText_2d::Awaken(_SPACE* ps)
{
    text=(PText2d)(((PSpace2d)ps)->texts->GetByHandle((HOBJ2D)text));
    if (text) text->IncRef(this);else return 0;
    return 1;
};

void TText_2d::StoreCoordinates(PStream ps)
{
    TObject2d::StoreCoordinates(ps);
    ps->WriteWord(angle);
};

void TText_2d::LoadCoordinates(PStream ps)
{
    TObject2d::LoadCoordinates(ps);
    angle=ps->ReadWord();
};

//---------------------------------------------------//
TEditFrame::TEditFrame(PObject2d po,POINT2D ps,int f):TObject2d()
{
    object=po;
    ps.x=8;ps.y=8;
    psize=ps;angle=0.0;_angle=0.0;
    _origin.x=0;_origin.y=0;_size.x=0;_size.y=0;
    _base.x=0;_base.y=0;
    _rorg.x=_rorg.y=0;
    for(int i=0;i<4;i++)
    {
        tops[i].x=tops[i].y=0;
    }
    options=(UINT16)f;
    if (po)
        UpdateSize();
};

void TEditFrame::GetRECT2d(RECT2D&r)
{
    if(object)
    {
        POINT2D Org,Size;
        Org=object->GetOrigin2d();
        Size=object->GetSize2d();
        r.left=Org.x;
        r.top=Org.y;
        r.right=r.left+Size.x;
        r.bottom=r.top+Size.y;
    }
    else
    {
        r.left=r.top=r.right=r.bottom=0;
    }
};

void TEditFrame::UpdateSize(int z,void * space)
{
    RECT2D r;
    if (object || z==15)
    {
        if (z!=15)
        {
            angle=object->GetAngle();
            //object?angle=object->GetAngle()/(4*M_PI):0;
        }
        //char s[1000];
        //wsprintf(s,"%d, %d",angle,object->WhoIsIt());
        //MessageBox(0,s,"",0);
        if (angle==0.0)
        {
            if (z==15)
            {
                r.left=_origin.x;
                r.top=_origin.y;
                r.right=r.left+_size.x;
                r.bottom=r.top+_size.y;
            }
            else
            {
                GetRECT2d(r);
            }//r= _GetObjectRect(object);
            //*--------------------------------------*
            r.right--;r.bottom--;
            tops[0].x=r.left;		tops[0].y=r.top;
            tops[1].x=r.right;	tops[1].y=r.top;
            tops[2].x=r.right;	tops[2].y=r.bottom;
            tops[3].x=r.left;		tops[3].y=r.bottom;
            _origin=tops[0];
            _size.x=(r.right-r.left)+1;
            _size.y=(r.bottom-r.top)+1;
            _angle=angle;
            _base=_origin;
            if(z!=15)
                _rorg=_base;
            r.top++;   r.bottom++;
        }
        else
        {
            if (z==15)
            {
                POINT2D newbase=_origin;
                RotatePoint(newbase,_base,angle);
                r.left=newbase.x;
                r.top=newbase.y;
                r.right=r.left+_size.x;
                r.bottom=r.top+_size.y;
            }
            else
            {
                _angle=angle;
                //r= _GetObjectRect(object);
                GetRECT2d(r);
                r.right--;	r.bottom--;
                _size.x=(r.right-r.left)+1;
                _size.y=(r.bottom-r.top)+1;
                _origin.x=r.left;_origin.y=r.top;
                _base=_origin;
                _rorg=_base;
            }
            GetRotateRectPoints(r,angle,tops);
            RotateRect(r,angle);
        }
    }
    else
    {
        r.left=_origin.x;
        r.top=_origin.y;
        r.right=r.left+_size.x;
        r.bottom=r.top+_size.y;
        r.right--;r.bottom--;
    }
    POINT2D s=psize;
    if (space)
        ((PSpace2d)space)->_DpToLpWO(&s,1);
    origin.x=r.left-s.x/2-3;
    origin.y=r.top-s.y/2-3;
    size.x=r.right-r.left+s.x+6;
    size.y=r.bottom-r.top+s.y+6;
};

TEditFrame::TEditFrame(PStream ps,LONG _pos):TObject2d(ps,_pos)
{
    object=(PObject2d)(void*)MAKELONG(ps->ReadWord(),0);
    ps->ReadPOINT(&psize);
    _origin.x=0;_origin.y=0;_size.x=1;_size.y=1;
};

void TEditFrame::Store(PStream ps)
{
    TObject2d::Store(ps);
    ps->WriteWord(object?object->_HANDLE:(INT16)-1);
    ps->WritePOINT(&psize);
};

void TEditFrame::Rotate2d(POINT2D p,double a)
{
    origin=_rorg;
    if (object==NULL || object->GetObjectCaps(OCmayrotate))
    {
        RotatePoint(origin,p,a-_angle);
        //angle-=((double)1800*a)/M_PI;
        angle=a;
    }
    else
        TObject2d::Rotate2d(p,a-_angle);
    _base=origin;
    _origin=origin;
    UpdateSize(15);
};

BOOL TEditFrame::Awaken(_SPACE* sp)
{
    if ((INT16)object<1)
    {
        object=NULL;
        return 1;
    }
    object=(PObject2d)((PSpace2d)sp)->GetObjByHANDLE((HOBJ2D)object);
    return object!=NULL;
};


TRotateCenter::TRotateCenter(POINT2D ps)
{
    center=ps;
    _size.x=10;_size.y=10;
    UpdateSize();
};

void TRotateCenter::Store(PStream ps)
{
    TObject2d::Store(ps);
    ps->WritePOINT(&center);
    ps->WritePOINT(&_size);
};

void TRotateCenter::UpdateSize(int,void * space)
{
    POINT2D s=_size;
    if (space)
        ((PSpace2d)space)->_DpToLpWO(&s,1);
    origin.x=center.x-s.x-1;
    origin.y=center.y-s.y-1;
    size.x=s.x*2+2;
    size.y=s.y*2+2;
};

TRotateCenter::TRotateCenter(PStream ps,LONG _pos):TObject2d(ps,_pos)
{
    ps->ReadPOINT(&center);
    ps->ReadPOINT(&_size);
};

POINT2D TRotateCenter::Move2d(POINT2D p)
{
    POINT2D old=center;
    center=p;
    origin.x+=center.x-old.x;
    origin.y+=center.y-old.y;
    return old;
};

int TObject2d::GetObjectCaps(int i)
{
    int o=WhoIsIt();
    switch (i)
    {
        case OCmayrotate:
            if ((o==20)||(o==23)||(o==3)||(o==4))
                return 1;
            break;
    }
    return 0;
};

RECT _GetObjectRect(PObject2d po)
{POINT2D o=po->GetOrigin2d(),s=po->GetSize2d();RECT r;
    r.left=o.x;r.top=o.y;
    r.right=o.x+s.x;r.bottom=o.y+s.y;
    return r;
};
void TFrame3d::UpdateSize(int,void *)
{
    Get8PointsByOrgSize(&origin,&size,tops);
    int i;
    if (flag & 1)
    {
        TRotator3d rotator(rotator);
        rotator.rotate(tops,8);
    }
    POINT2D mi,ma,m;
    view->Perspective(tops,tops,8);
    for (i=0;i<8;i++)
    {
        if (i==0)
        {mi.x=tops[i].x;mi.y=tops[i].y;ma=mi;}else
        {m.x=tops[i].x;
            m.y=tops[i].y;
            mi.x=min(mi.x,m.x);mi.y=min(mi.y,m.y);
            ma.x=max(ma.x,m.x);ma.y=max(ma.y,m.y);
        }
    }
    rect.left=mi.x;rect.top=mi.y;
    rect.right=ma.x;rect.bottom=ma.y;

    rect.right+=3; rect.bottom+=3;
    rect.left-=2;rect.top-=2;
    view->space2d->_DpToLpWO(((POINT *)&rect),2);
    POINT2D delta=view->offset;
    delta=view->origin;
    rect.left+=delta.x;rect.right+=delta.x;
    rect.top+=delta.y;rect.bottom+=delta.y;
};

void TAxis3d::UpdateSize(int,void *)
{/*

  for(int i=0;i<4;i++)tops[i]=origin;
  tops[1].x+=100.0;
  tops[2].y+=100.0;
  tops[3].z+=100.0;
  tops[0].RealPerspective(0);
     tops[0]._y=-tops[0]._y;
  double x0=tops[0]._x,y0=tops[0]._y,z0=tops[0]._z;
  tops[0].Perspective();
     tops[0]._y=-tops[0]._y;
  tops[0].ix=Round(tops[0]._x);tops[0].iy=Round(tops[0]._y);
  rect.left=(int)tops[0].ix;rect.right=rect.left;
  rect.top=(int)tops[0].iy;rect.bottom=rect.top;
  for(i=1;i<4;i++){
  tops[i].RealPerspective(0);
     tops[i]._y=-tops[i]._y;
  double x=tops[i]._x-x0,y=tops[i]._y-y0,z=tops[i]._z-z0;
  tops[i].RealPerspective();
  double r=sqrt(x*x+y*y+z*z);
  if (r>1e-10){
    tops[i].ix=tops[0]._x+axisSize*x/r;
    tops[i].iy=tops[0]._y+axisSize*y/r;
  }else {tops[i].ix=0;tops[i].iy=0;}

  rect.left=min(rect.left,(int)(tops[i].ix));
  rect.right=max(rect.right,(int)(tops[i].ix));
  rect.top=min(rect.top,(int)(tops[i].iy));
  rect.bottom=max(rect.bottom,(int)(tops[i].iy));
  }
  POINT2D delta=view->projector->poOffset;
  rect.right+=10;
  rect.bottom+=15;
  view->space2d->_DpToLpWO(((POINT2D *)&rect),2);
  rect.left+=delta.x;rect.right+=delta.x;
  rect.top+=delta.y;rect.bottom+=delta.y;

  delta=view->origin;
  rect.left+=delta.x;rect.right+=delta.x;
  rect.top+=delta.y;rect.bottom+=delta.y;
  */
};
RECT TAxis3d::GetRect(int)
{ return rect;
};
RECT TFrame3d::GetRect(int)
{ return rect;
};
int TSpace3d::SetFrame3dPoint(int n,POINT3D p)
{PFrame3d f=space2d->Frame3d;
    if (f){
        space2d->UpdateSpecObject((PObject2d)f);
        POINT3D _o=f->origin,_s=_o;
        _s.x+=f->size.x;_s.y+=f->size.y;_s.z+=f->size.z;
        static  BYTE kn[8]={0,1,3,2,4,5,7,6};
        int k=kn[n & 7];
        if (k & 1)_s.x=p.x;else _o.x=p.x;
        if (k & 2)_s.y=p.y;else _o.y=p.y;
        if (k & 4)_s.z=p.z;else _o.z=p.z;
        f->origin=_o;f->size.x=_s.x-_o.x;f->size.y=_s.y-_o.y;f->size.z=_s.z-_o.z;
        f->UpdateSize();
        f->flag |= 6;
        space2d->UpdateSpecObject((PObject2d)f);
        return 1;
    }return 0;
};
//tools
BOOL TText2d::Awaken(_SPACE* sp)
{ C_TYPE j;
    for (j=0;j<text->count;j++)
    {PTextRecord pr=(PTextRecord)(text->At(j));
        pr->text2d=this;
        pr->font=(PFont2d)(((PSpace2d) sp)->fonts)->GetByHandle((HOBJ2D)pr->font);
        if (pr->font)pr->font->IncRef(this);else return 0;
        pr->string=(PString2d)(((PSpace2d) sp)->strings)->GetByHandle((HOBJ2D)pr->string);
        if (pr->string)pr->string->IncRef(this);else return 0;
    }
    return 1;
};
BOOL TBrush2d::Awaken(_SPACE* sp)
{if (dib)
    {dib=(PDib2d)((PSpace2d)sp)->dibs->GetByHandle((HOBJ2D)dib);
        if (dib==NULL)return 0;dib->IncRef(this);
    }return 1;
};


PDib2d __GetRDib2d(char * s,_OBJECT*obj)
{
    C_TYPE i;PDib2d pd;
    for (i=0;i<TSpace2d::_dibs->count;i++)
    {
        pd=(PDib2d)TSpace2d::_dibs->At(i);
        if (lstrcmpi(pd->filename,s)==0){pd->IncRef(obj);return pd;}
    }

    BOOL dbl=FALSE;
    pd = ImportRasterImage(s,NULL,&dbl);
    if(dbl && pd){delete pd;pd=NULL;}
    if(pd)
    {
        TSpace2d::_dibs->Insert(pd);
        pd->filename=NewStr(s);
        pd->IncRef(obj);
    }
    return pd;
};

PDib2d GetRDib2d(char * s,_OBJECT*obj)
{
    PDib2d pd=__GetRDib2d(s,obj);
    if(!pd)
    {
        char dst[256];
        GetPrivateProfileString("SUBST","*.bmp","",dst,sizeof(dst),_graphics_ini);
        pd=__GetRDib2d(dst,obj);
    }
    return pd;
}

PDoubleDib2d __GetRDoubleDib2d(char * s,_OBJECT*obj)
{C_TYPE i;PDoubleDib2d pd;
    for (i=0;i<TSpace2d::_ddibs->count;i++)
    {pd=(PDoubleDib2d)TSpace2d::_ddibs->At(i);
        if (lstrcmpi(pd->filename,s)==0){pd->IncRef(obj);return pd;}
    }
    TDOSStream ds(s,TDOSStream::stOpenRead);
    if (ds.status)return 0;
    pd=new TDoubleDib2d(0,&ds);
    if (pd==NULL)return 0;
    if (pd->dib==NULL){delete pd;return NULL;}
    TSpace2d::_ddibs->Insert(pd);
    pd->filename=NewStr(s);
    pd->IncRef(obj);
    return pd;
};
PDoubleDib2d GetRDoubleDib2d(char * s,_OBJECT*obj)
{
    PDoubleDib2d pd=__GetRDoubleDib2d(s,obj);
    if(!pd){
        char dst[256];
        GetPrivateProfileString("SUBST","*.dbm","",dst,sizeof(dst),_graphics_ini);
        pd=__GetRDoubleDib2d(dst,obj);
    }
    return pd;
}
void __LoadError(char *n,char *fp,char *p){
    char s[256];
    wsprintf(s,"Unable to load bitmap %s \nPath: %s\nFull name: %s\n",n,p,fp);
    MessageBox(0,s,"Graphic Dll",MB_TASKMODAL|MB_ICONHAND);
}
BOOL TRefToDoubleDib2d::Awaken(_SPACE* ps)
{char ss[256]="";
    //if
    ((PSpace2d)ps)->GetFullPath(ss,name);
    tool=GetRDoubleDib2d(ss,this);
    /*
      if (tool)return 1;
    __LoadError(name,ss,((PSpace2d)ps)->path);
    return 0;
*/
    return TRUE;
}
BOOL TRefToDib2d::Awaken(_SPACE* ps)
{
    char ss[256];
    //if(
    ((PSpace2d)ps)->GetFullPath(ss,name);
    tool=GetRDib2d(ss,this);
    /*
      if (tool)return 1;
    __LoadError(name,ss,((PSpace2d)ps)->path);
    return 0;
*/
    return TRUE;
}

TUserObject2d::TUserObject2d(USEROBJSTRUCT *us,USERTYPESTRUCT* ut){
    data=*us;
    data.type=NewStr(data.type);
    type=ut;
    //  if (ut->usersize)data.data=new BYTE[ut->usersize];
    origin.x=data.x;origin.y=data.y;
    size.x=data.sizex;size.y=data.sizey;
}

TUserObject2d::TUserObject2d(PStream ps,LONG _pos) : TObject2d(ps,_pos)
{
    data.type = ps->ReadStr();
    data.sizex = size.x;
    data.sizey = size.y;
    data.x = origin.x;
    data.y = origin.y;

    type = GetUserType(data.type);
    data.ptr = 0;
    data.data = 0;
    data.handle = _HANDLE;

    USERSTREAM us;
    us.data=0;
    us.size=0;
    us.flags=0;

    ps->Read(&(us.size),4);
    if (us.size)
    {
        us.data = new char[(int)us.size];
        ps->Read(us.data,(int)us.size);
    }
    if (type)
    {
        type->objproc(0, &(data), OM_LOAD, (DWORD)&us);
        if (us.flags&1)
            type->objproc(0,&(data),OM_POSTLOAD,(DWORD)&us);
    }
    if (us.size)
        delete us.data;
};
TUserObject2d::~TUserObject2d(){
    if (type){
        type->objproc(space->_HANDLE,&(data),OM_DESTROY,NULL);
    }
    delete data.type;
};
void TUserObject2d::Store(PStream ps){
    TObject2d::Store(ps);
    ps->WriteStr(data.type);
    USERSTREAM us;us.data=0;us.size=0;us.flags=0;
    type->objproc(space->_HANDLE,&(data),OM_SAVE,(DWORD)&us);
    ps->Write(&(us.size),4);
#ifdef WIN32
    if (us.size)ps->Write((const void*)us.data,us.size);
#else
    if (us.size)ps->Write((const void _huge*)us.data,us.size);
#endif
    if (us.flags&1){
        type->objproc(space->_HANDLE,&(data),OM_POSTSAVE,(DWORD)&us);
    }
};

POINT2D TUserObject2d::Move2d(POINT2D p)
{
    p=TObject2d::Move2d(p);
    data.x=origin.x;
    data.y=origin.y;
    return p;
};

void TUserObject2d::Rotate2d(POINT2D p,double a)
{
    TObject2d::Rotate2d(p,a);
    data.x=origin.x;
    data.y=origin.y;
    //angle-=((double)1800*a)/M_PI;
};

int  TUserObject2d::GetDistToMouse(POINT2D p)
{
    return TObject2d::GetDistToMouse(p);
};

BOOL TUserObject2d::Awaken(_SPACE*ps)
{
    space=(TSpace2d*)ps;
    if (type)
    {
        return (int)type->objproc(space->_HANDLE,&(data),OM_AWAKEN,NULL);
    }
    return 0;
};

POINT2D  TUserObject2d::SetSize2d(POINT2D p)
{
    p=TObject2d::SetSize2d(p);
    data.sizex=size.x;
    data.sizey=size.y;
    return p;
};
/*
 HWND HWindow;
 WORD exflags;*/


long Round(double f)
{
    return f+0.5;
}

