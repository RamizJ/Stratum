/*
Copyright (c) 1995  Virtual World
Module Name:
         object2d.h
         object2d.cpp
Author:
         Alexander Shelemekhov
*/
#ifndef _object_2d_h_
#define _object_2d_h_

#include "tools2d.h"
#include <gwin2d3d\3drender.h>
#include <math.h>
#include "../../BC/bc_include_gwin2d3d/GWINTYPE.H"

const int OCmayrotate = 1;

void PntToPnt(POINT2D&po,POINT pi);
void PntToPnt(POINT&po,POINT2D& pi);
double _round(double x);
void ChkPnt(POINT2D &p);

//*--------- TObject2d ----------------*
class _SPACE;
class TCamera3d;

class TObject2d : public TObject
{
public:
    POINT2D origin;
    POINT2D size;
    INT16 angle;
    INT16 alpha;
    //RECT oldrect;
    //BOOL ischanged;

    TObject2d();
    TObject2d(POINT2D);
    TObject2d(PStream,LONG _pos);
    ~TObject2d();

    virtual RECT GetRect(int FromPaint=0);
    virtual POINT2D Move2d(POINT2D);
    virtual void Rotate2d(POINT2D,double);
    virtual void Store(PStream);
    virtual double GetAngle() {return -((M_PI)*((double)angle))/1800;};
    virtual double GetAlpha() {return (double)alpha/1000.0;};
    virtual void SetAlpha(gr_float v) {alpha = max(0,min(1000,v*1000));};
    virtual int  GetDistToMouse(POINT2D);
    virtual BOOL Awaken(_SPACE*);
    //virtual int SetOptimalScale(int mode=1){return 0;};
    virtual void UpdateSize(int copy=1,void * _p=NULL); /* Копировать результат в origin и
                                                                                                          size */
    virtual POINT2D GetOrigin2d(){return origin;};
    virtual POINT2D GetSize2d(){return size;};
    virtual POINT2D SetSize2d(POINT2D);
    virtual void StoreCoordinates(PStream);
    virtual void LoadCoordinates(PStream);
    virtual void SetState(WORD astate,ATTR_MODE op,pointer );
    int  GetObjectCaps(int);
};
typedef TObject2d * PObject2d;

class TGroup2d : public TGroup
{
public:
    TGroup2d():TGroup(){};
    TGroup2d(PStream ps,LONG _pos):TGroup(ps,_pos){};

    virtual RECT GetRect(int FromPaint=0);
    virtual POINT2D Move2d(POINT2D);
    virtual void Rotate2d(POINT2D,double);

    virtual POINT2D GetOrigin2d();
    virtual POINT2D GetSize2d();
    virtual POINT2D SetSize2d(POINT2D);

    virtual WORD WhoIsIt(){return 3;}
};
typedef TGroup2d * PGroup2d;

class TRGroup2d :public TGroup2d
{
public:
    TRGroup2d():TGroup2d(){};
    TRGroup2d(PStream ps,LONG _pos):TGroup2d(ps,_pos){};

    virtual RECT GetRect(int FromPaint=0);
    HRGN GetRegion();
    virtual WORD WhoIsIt(){return 4;};
};

typedef TRGroup2d * PRGroup2d;
class TSpace2d;

class TUserObject2d: public TObject2d
{
public:
    TSpace2d * space;
    USEROBJSTRUCT    data;
    USERTYPESTRUCT * type;

    TUserObject2d(USEROBJSTRUCT*,USERTYPESTRUCT*);
    TUserObject2d(PStream,LONG _pos);
    ~TUserObject2d();

    virtual POINT2D Move2d(POINT2D);
    virtual void Rotate2d(POINT2D,gr_float);
    virtual void Store(PStream);
    virtual int  GetDistToMouse(POINT2D);
    virtual BOOL Awaken(_SPACE*);
    virtual POINT2D SetSize2d(POINT2D);
    virtual WORD WhoIsIt(){return 25;}
};

class TControlData;

class TControl2d : public TObject2d
{
    char classname[64];
public:
    WNDPROC thunk;
    TControlData*windata;
    HWND HWindow;
    DWORD dwStyle,exflags;
    RECT realrect;
    BYTE listbox;
    TSpace2d * space;
    TControl2d(TSpace2d* space,HWND,INT16 x,INT16 y,DWORD,DWORD exflags);

    TControl2d(PStream,LONG _pos);
    ~TControl2d();
    virtual RECT GetRect(int FromPaint=0);
    void    SetRect();
    virtual void Store(PStream);
    virtual POINT2D Move2d(POINT2D);
    virtual BOOL Awaken(_SPACE*);
    virtual POINT2D SetSize2d(POINT2D);
    virtual WORD WhoIsIt(){return 26;};
    void    SubClass();
    void    SetParent(HWND);
    LRESULT ReceiveMessage(UINT &msg, WPARAM &wParam, LPARAM &lParam,BOOL & calldefault);
    virtual void SetState(WORD astate,ATTR_MODE,pointer );
    virtual void UpdateSize(int copy=1,void * _p=NULL);
    static TControl2d*  GetControl(HWND);
private:
    void CheckStyle(LONG&_new);
    void Create(int x,int y);
};

class TLine_2d : public TObject2d
{
public:
    POINT2D r_size,actualorigin,actualsize;
    ARROW2D * arrows;
    PPen2d pen;
    PBrush2d brush;
    LPPOINT2D points;
    LPPOINT _points;
    INT16 count, limit;

    TLine_2d(PPen2d,PBrush2d, POINT2D * apoints,INT16 NumPoints,TSpace2d*);
    TLine_2d(PStream,LONG _pos);
    ~TLine_2d();

    virtual RECT GetRect(int FromPaint=0);
    virtual POINT2D Move2d(POINT2D);
    virtual void Rotate2d(POINT2D,double);
    virtual void UpdateSize(int copy=1,void * _p=NULL);
    virtual void Store(PStream);
    virtual int  GetDistToMouse(POINT2D);
    virtual POINT2D SetSize2d(POINT2D newSize);
    virtual BOOL Awaken(_SPACE*);
    virtual WORD WhoIsIt();
    int GetRgnCombineMode();
    int SetRgnCombineMode(int);
    int GetRgnCreateMode();
    RECT GetUpdRect(int point);
    virtual void MarkToSave(int);
    virtual void StoreCoordinates(PStream);
    virtual void LoadCoordinates(PStream);
    virtual void SetState(WORD astate,ATTR_MODE op,pointer );
    void Update_Points();
    void _UpRect(RECT & r);
    void _UpRect(RECT2D & r);
    INT16 GetLimit(INT16 count);
};
typedef TLine_2d * PLine_2d;

class TText_2d : public TObject2d
{
public:
    PText2d text;
    POINT2D Delta;

    TText_2d(POINT2D,POINT2D,PText2d);
    TText_2d(PStream,LONG _pos);
    ~TText_2d();

    virtual void Rotate2d(POINT2D,gr_float);
    virtual void UpdateSize(int copy=1,void * _p=NULL);
    virtual void Store(PStream);
    virtual BOOL Awaken(_SPACE*);
    virtual WORD WhoIsIt(){return 23;};
    virtual RECT GetRect(int FromPaint=0);
    virtual void MarkToSave(int);
    virtual void StoreCoordinates(PStream);
    virtual void LoadCoordinates(PStream);
};
typedef TText_2d * PText_2d;

class _TBMP2d:public TObject2d
{
public:
    POINT2D SrcOrigin;
    POINT2D SrcSize;
    PTools2d dib;
    _TBMP2d(POINT2D SrcOrg, POINT2D aSrcSize,POINT2D DstOrg, POINT2D DstSize,PTools2d);
    _TBMP2d(PStream,LONG _pos);
    ~_TBMP2d();
    virtual void Store(PStream);
    virtual WORD WhoIsIt(){return (WORD)_abstract();};
    virtual void MarkToSave(int);
};

class TBitmap_2d : public _TBMP2d
{
public:
    TBitmap_2d(POINT2D SrcOrg, POINT2D aSrcSize,POINT2D DstOrg, POINT2D DstSize,PDib2d pd);
    TBitmap_2d(PStream ps,LONG _pos):_TBMP2d(ps,_pos){};

    virtual void Rotate2d(POINT2D,double);
    virtual RECT GetRect(int FromPaint=0);
    virtual BOOL Awaken(_SPACE*);
    virtual WORD WhoIsIt();
};
typedef TBitmap_2d * PBitmap_2d;

class TDoubleBitmap_2d : public _TBMP2d
{
public:
    TDoubleBitmap_2d(POINT2D SrcOrg, POINT2D aSrcSize,POINT2D DstOrg, POINT2D DstSize,PDoubleDib2d pd);
    TDoubleBitmap_2d(PStream ps,LONG _pos):_TBMP2d(ps,_pos){};

    virtual void Rotate2d(POINT2D,double);
    virtual RECT GetRect(int FromPaint=0);
    virtual BOOL Awaken(_SPACE*);
    virtual WORD WhoIsIt();
    virtual int GetDistToMouse(POINT2D);
};

typedef TDoubleBitmap_2d * PDoubleBitmap_2d;
class TSpace3d;
class TSpace2d;
class _TObject3d;

class TEditFrame :public TObject2d
{
public:
    POINT2D psize;
    PObject2d object;
    POINT2D tops[4];
    POINT2D _origin,_size,_base,_rorg;
    double angle,_angle;

    TEditFrame(PObject2d po,POINT2D ps,int );
    TEditFrame(PStream ps,LONG _pos);
    virtual void UpdateSize(int copy=1,void * _p=NULL);
    virtual void Rotate2d(POINT2D,double);
    virtual double GetAngle() {return angle;};
    virtual void Store(PStream);
    virtual BOOL Awaken(_SPACE*);
    virtual WORD WhoIsIt(){return 50;};
    virtual POINT2D GetOrigin2d(){return _origin;};
    virtual POINT2D GetSize2d(){return _size;};
    void GetRECT2d(RECT2D&);
};

class TRotateCenter :public TObject2d
{
public:
    POINT2D center;
    POINT2D _size;
    TRotateCenter(POINT2D ps);
    TRotateCenter(PStream ps,LONG _pos);

    virtual WORD WhoIsIt(){return 51;}
    virtual void UpdateSize(int copy=1,void * _p=NULL);
    virtual void Store(PStream);
    virtual POINT2D Move2d(POINT2D);
    virtual POINT2D GetOrigin2d(){return center;}
};

typedef TRotateCenter * PRotateCenter;
typedef TEditFrame * PEditFrame;
RECT _GetObjectRect(PObject2d);
#endif //_object_2d_h_
