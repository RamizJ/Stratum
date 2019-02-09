/*
Copyright (c) 1995  Virtual World
Module Name:
     object2d.cpp
Author:
     Alexander Shelemekhov
*/
#define __FLOAT__GR__
#include <windows.h>
#include "space2d.h"
#include "_system.h"

#define _WIN_HOOK_DYNAMIC
#include "winhook.h"
//-----------------------

class TControlData{
public:
    DWORD exstyle,dwStyle;
    char * classname;
    char * text;
    int sizex,sizey,id;
    TControlData(PStream);

    ~TControlData();
    void Store(PStream);
};

TControlData::TControlData(PStream ps){
    classname=ps->ReadStr();
    text=ps->ReadStr();
    ps->Read(&dwStyle,4);
    ps->Read(&exstyle,4);
    id=ps->ReadWord();
    sizex=ps->ReadWord();
    sizey=ps->ReadWord();
    ps->ReadWord();
};
void TControlData::Store(PStream ps){

    ps->WriteStr(classname);
    ps->WriteStr(text);
    ps->Write(&dwStyle,4);
    ps->Write(&exstyle,4);
    ps->WriteWord(id);
    ps->WriteWord(sizex);
    ps->WriteWord(sizey);
    ps->WriteWord(0);

};
TControlData::~TControlData(){
    if(classname)delete classname;
    if (text)delete text;
};

RECT TControl2d::GetRect(int){
    return realrect;
};
void TControl2d::CheckStyle(LONG& _new){
    _new&=~((long)WS_CLIPSIBLINGS);
    if(!lstrcmpi(windata->classname,"BUTTON")){
        if((_new&0x0f)==BS_AUTORADIOBUTTON)
        {
            _new&=~0x0fL;_new|=BS_RADIOBUTTON;
        }
    }else{
#ifdef WIN32
        //  if(!lstrcmpi(windata->classname,"EDIT")){
        if(_new & WS_BORDER){
            windata->exstyle|=WS_EX_CLIENTEDGE;
        }
        //  }
#endif
    }
};
void TControl2d::SetRect(){
    RECT r;
    GetWindowRect(HWindow,&r);
    ScreenToClient(space->hwnd,(POINT*)&r);
    ScreenToClient(space->hwnd,(POINT*)&(r.right));
    space->_DpToLp((POINT*)&r,2);
    realrect=r;
};

void TControl2d::Create(int x,int y){
    if (windata){
        POINT2D s,p;
        s=size;
        p.x=x;
        p.y=y;
        space->_LpToDpWO(&s,1);
        space->_LpToDp(&p,1);

        LONG style=windata->dwStyle;
        CheckStyle(style);
        HWindow=CreateWindowEx(windata->exstyle,windata->classname,windata->text,style,
                               p.x,p.y,s.x,s.y,space->hwnd,(HMENU)(windata->id),hInstance,NULL);
        SetRect();
        dwStyle=windata->dwStyle;
        if (HWindow){
            delete windata;
            windata=NULL;
            SendMessage(HWindow,WM_SETFONT,(WPARAM)GetStockObject(ANSI_VAR_FONT),0);
            SubClass();
        }
    }
};

LRESULT CALLBACK WinHook(t_pointer data,HWND, UINT &msg, WPARAM &wParam, LPARAM &lParam,BOOL & calldefault){
    TControl2d * c=(TControl2d *)data;
    return c->ReceiveMessage(msg,wParam,lParam,calldefault);
};

TControl2d::TControl2d(PStream ps,LONG _pos):TObject2d(ps,_pos){
    thunk=0;
    HWindow=0;
    windata=new TControlData(ps);
    // HWindow=CreateWindowEx(exstyle,classname,text,dwStyle,0,0,x,y,GR_WINDOW,(HMENU)id,hInstance,NULL);
};
TControl2d::~TControl2d(){
    if (windata)delete windata;
    if (thunk)ReleaseWindowHook(HWindow);
    thunk=0;
    if(HWindow)DestroyWindow (HWindow);
    HWindow=NULL;
};
void TControl2d::SetState(WORD astate,ATTR_MODE op,pointer p){
    BOOL v1=((PSpace2d)p)->IsVisible(this);
    TObject::SetState(astate,op,p );
    BOOL v2=((PSpace2d)p)->IsVisible(this);
    if (HWindow && v1!=v2)ShowWindow(HWindow,v2?SW_SHOWNA:SW_HIDE);
};

void TControl2d::UpdateSize(int copy,void * _p)
{
    RECT r=TObject2d::GetRect();
    space->_LpToDp((LPPOINT)&r,2);
    if(HWindow)
        SetWindowPos(HWindow,0,
                     r.left,r.top,r.right-r.left,r.bottom-r.top,
                     SWP_NOZORDER|SWP_NOACTIVATE);
};

LRESULT TControl2d::ReceiveMessage(UINT &msg, WPARAM &wParam, LPARAM &lParam,BOOL & calldefault){
    switch(msg){
        /*
  case WM_ERASEBKGND:{

    calldefault=0;
  }return 1;
*/
        case WM_KEYDOWN:
        case WM_KEYUP:{
            struct {
                HWND   hwnd;
                HOBJ2D hObject;
                int    nVirtKey;
                long   lKeyData;
                BOOL   calldef;
            }t1;
            t1.hwnd=HWindow;
            t1.hObject=_HANDLE;
            t1.nVirtKey = (int) wParam;    // virtual-key code
            t1.lKeyData = lParam;          // key data
            t1.calldef=TRUE;
            int r=SendMessage(space->hwnd,WM_PARENTNOTIFY,msg,(LPARAM)&t1);
            calldefault=t1.calldef;
            return r;
        }
    };
    if (space->State&SF_HOOKMOUSE){
        calldefault=0;
        switch(msg){
            case WM_MOUSEMOVE:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONDBLCLK:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONDBLCLK:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_MBUTTONDBLCLK:
            {

                POINT p;
                p.x = LOWORD(lParam);  // horizontal position of cursor
                p.y = HIWORD(lParam);  // vertical position of cursor

                ClientToScreen(HWindow,&p);
                HWND parent=space->hwnd;
                ScreenToClient(parent,&p);

                lParam=MAKELONG(p.x,p.y);

                return SendMessage(parent,msg,wParam,lParam);
            }
            case WM_SETCURSOR:{
                // WORD nHittest = LOWORD(lParam);    /* hit-test code                */
                WORD wMouseMsg = HIWORD(lParam);   /* mouse-message number         */
                LPARAM l;l=MAKELONG(HTCLIENT,wMouseMsg);
                return SendMessage(space->hwnd,msg,(WPARAM)space->hwnd,l);

            }
                // case WM_CLOSE:if (GetParent(HWindow)!=GR_WINDOW){SetParent(GR_WINDOW);return 0;}break;
        };
    }
    calldefault=TRUE;
    return 0;
};

TControl2d::TControl2d(TSpace2d* _space,HWND hwnd,INT16 x,INT16 y,DWORD style,DWORD _exflags):TObject2d()
{
    windata=0;
    thunk=0;
    space=_space;
    dwStyle=style;
    exflags=_exflags;

    HWindow=hwnd;

    SubClass();
    SendMessage(hwnd,WM_SETFONT,(WPARAM)GetStockObject(ANSI_VAR_FONT),0);
    /*
  CheckStyle(style);
#ifdef WIN32
  SetWindowLong(hwnd,GWL_EXSTYLE,windata->exstyle);
#endif
  SetWindowLong(hwnd,GWL_STYLE,style);
*/
    origin.x=x;
    origin.y=y;
    Move2d(origin);
    RECT r;
    GetWindowRect(HWindow,&r);
    size.x=r.right-r.left;
    size.y=r.bottom-r.top;
    space->_DpToLpWO(&size,1);
};

void TControl2d::SubClass(){
    if (LoadHook()){
        thunk=CreateWindowHook(HWindow,WinHook,this);
        GetClassName(HWindow,classname,sizeof(classname));
        if(!lstrcmpi(classname,"listbox"))
        {
            listbox=1;
        }
    }
}
void  TControl2d::SetParent(HWND _hwnd){
    if (_hwnd){
        POINT2D _org=origin;
        POINT2D _size=size;
        space->_LpToDp(&_org,1);
        space->_LpToDpWO(&_size,1);
        if (windata){
            Create(origin.x,origin.y);
        }else{
            SetWindowPos(HWindow,0,_org.x,_org.y,_size.x,_size.y,SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);
            ::SetParent(HWindow,_hwnd);
        }
        //ShowWindow(HWindow,SW_SHOW);
    }else{
        ShowWindow(HWindow,SW_HIDE);
        ::SetParent(HWindow,GetDesktopWindow());
    }
};
BOOL TControl2d::Awaken(_SPACE*sp){
    space=(TSpace2d*)sp;
    if (space->hwnd)SetParent(space->hwnd);
    return (HWindow!=0 || windata);
};
void  TControl2d::Store(PStream ps){
    TObject2d::Store(ps);
    if (windata)windata->Store(ps);else
    {
        char classname[256];
        GetClassName(HWindow,classname,sizeof(classname));
        ps->WriteStr(classname);
        int tl=SendMessage(HWindow,WM_GETTEXTLENGTH,0,0);

        if (tl){
            char *s=new char [tl+1];
            GetWindowText(HWindow,s,tl+1);
            ps->WriteStr(s);
            delete s;
        }else ps->WriteStr(NULL);

        ps->Write(&dwStyle,4);
        DWORD style=GetWindowLong(HWindow,GWL_EXSTYLE);
        ps->Write(&style,4);
#ifdef WIN32
        ps->WriteWord(GetWindowLong(HWindow,GWL_ID));
#else
        ps->WriteWord(GetWindowWord(HWindow,GWW_ID));
#endif
        RECT r;
        GetWindowRect(HWindow,&r);
        ps->WriteWord(r.right-r.left);
        ps->WriteWord(r.bottom-r.top);
        ps->WriteWord(0);}
};
POINT2D TControl2d::Move2d(POINT2D p){
    POINT2D p1=p;
    space->_LpToDp(&p1,1);
    SetWindowPos(HWindow,0,p1.x,p1.y,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
    SetRect();
    return TObject2d::Move2d(p);
};
POINT2D TControl2d::SetSize2d(POINT2D p){
    POINT2D p1=p;
    space->_LpToDpWO(&p1,1);
    SetWindowPos(HWindow,0,0,0,p1.x,p1.y,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
    SetRect();
    return TObject2d::SetSize2d(p);
};

_EXTERNC HINSTANCE WINAPI LoadLibrary2d(char*name);
BOOL LoadHook(){
    BYTE static trytoload=1;
    if (HHook)return TRUE;
    if (trytoload){
        trytoload=0;
#ifdef WIN32
        HHook=LoadLibrary2d("whook_32.dll");
#else
        HHook=LoadLibrary2d("whook_16.dll");
#endif
        if (!HHook){HHook=NULL;return FALSE;}
        (FARPROC)CreateWindowHook  = GetProcAddress(HHook,"CreateWindowHook");
        (FARPROC)ReleaseWindowHook = GetProcAddress(HHook,"ReleaseWindowHook");
        (FARPROC)GetWindowData = GetProcAddress(HHook,"GetWindowData");
        return TRUE;
    }
    return FALSE;
};
TControl2d* TControl2d::GetControl(HWND hw){
    TControl2d*c=(TControl2d*)GetWindowData(hw);
    return c;
};
HINSTANCE HHook=NULL;
#ifdef WIN32
WNDPROC( _stdcall * CreateWindowHook)(HWND hwnd,WNDHOOKPROC,t_pointer data)=NULL;
int    ( _stdcall * ReleaseWindowHook)(HWND)=NULL;
t_pointer ( _stdcall * GetWindowData)(HWND hwnd);
#else
WNDPROC( far _pascal * CreateWindowHook)(HWND hwnd,WNDHOOKPROC,t_pointer data)=NULL;
int    ( far _pascal * ReleaseWindowHook)(HWND)=NULL;
t_pointer ( far _pascal * GetWindowData)(HWND hwnd);
#endif

_EXTERNC BOOL  _EXPORT GetControlId2d(HWND control,HSP2D*hsp,HOBJ2D *id){
    if (GetWindowData){
        TControl2d*c=(TControl2d*)GetWindowData(control);
        if (hsp)*hsp=0;
        if (id)*id=0;
        if(c){
            if (hsp)*hsp=c->space->_HANDLE;
            if (id)*id=c->_HANDLE;
            return TRUE;
        }}
    return FALSE;
};


