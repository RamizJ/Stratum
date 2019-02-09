/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
            srvwin.cpp
Author:
            Alexander Shelemekhov
*/
#include "class.h"
#include "twindow.h"
#include "sc3.rh"
#include "browser.h"
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include "records.h"
#include "stream.h"
#ifdef DBE
#include "dbengine.h"
#endif
#include "ballon.h"
#include "video\scvideo.h"
#include "statusba.h"
#include "scvars.h"
#ifdef CAN_EDIT_BITMAP
#include "bmpwnd.h"
#include "\sc3_add\bitedit\bmpedit.h"
#endif
#include "dialogs.h"
#include "strings.rh"
#define IDC_HLABEL 1
#define IDC_VLABEL 2
#define IDC_INFO   3
#include "msgcrk.h"
#include <string.h>
#include "dllman.h"
#include <stratum\stratum.h>
#include <stratum\sc_types.h>
#include <stratum\sc_msg.h>
#include "dialogs.rh"
#include "tpopup.h"
#include "systab.h"
extern HINSTANCE hInstance;
BOOL extern _ChoseColor(HWND hwnd,COLORREF & clr,char*);
HINSTANCE HHook=NULL;



#define _WIN_HOOK_DYNAMIC
#include <gwin2d3d\winhook.h>


void   SetDefaultMenuItem(HMENU hm,UINT cmd){
#ifdef WIN32
    MENUITEMINFO mii;
    memset(&mii,0,sizeof(mii));
    mii.cbSize=sizeof(mii);
    mii.fMask=MIIM_STATE;
    if(GetMenuItemInfo(hm,cmd,FALSE,&mii )){
        mii.fMask=MIIM_STATE;
        mii.fState|=MFS_DEFAULT;;
        SetMenuItemInfo(hm,cmd,FALSE,&mii);
    }
#endif
};

BOOL LoadHook();
#ifdef WIN32
WNDPROC( _stdcall * CreateWindowHook)(HWND hwnd,WNDHOOKPROC,t_pointer data)=NULL;
int    ( _stdcall * ReleaseWindowHook)(HWND)=NULL;
t_pointer ( _stdcall * GetWindowData)(HWND hwnd);
#else
WNDPROC( far _pascal * CreateWindowHook)(HWND hwnd,WNDHOOKPROC,t_pointer data)=NULL;
int    ( far _pascal * ReleaseWindowHook)(HWND)=NULL;
t_pointer ( far _pascal * GetWindowData)(HWND hwnd);
#endif

BOOL CreateWinHook(HWND hwnd,WNDHOOKPROC w,t_pointer data){
    if(LoadHook()){
        return (CreateWindowHook(hwnd,w,data)!=NULL);
    };
    return TRUE;
}

//*******************************************************************
//              Status Bar
//*******************************************************************

#define BC_EDIT  110
#define BC_COMBO 111


#define DR_LEFTTOP(mode)\
    PatBlt(hdc,x,y,sizex,1,mode);\
    PatBlt(hdc,x,y+1,1,sizey-1,mode);

#define DR_RIGHTBOTTOM(mode)\
    PatBlt(hdc,x,y+sizey-1,sizex,1,mode);\
    PatBlt(hdc,x+sizex-1,y,1,sizey,mode);



// BMP window
HGLOBAL HMakeCopy(HGLOBAL h){
    if(h){
        long size=GlobalSize(h);
        HGLOBAL _h=GlobalAlloc(GMEM_MOVEABLE,size);
#ifdef WIN32
        memcpy(GlobalLock(_h),GlobalLock(h),size);
#else
        hmemcpy(GlobalLock(_h),GlobalLock(h),size);
#endif
        GlobalUnlock(_h);
        GlobalUnlock(h);
        return _h;
    }
    return 0;
}

TBMPChild::TBMPChild(LPCREATESTRUCT l,HWND hwnd):TMDIChildWindow(l,hwnd){

    TFCollection*f=dllmanager->LoadPlugin(
                   #ifdef WIN32
                       "bitmap32.dll"
                   #else
                       "bitmap16.dll"
                   #endif
                       ,2);

    MODE=MODE_BMPEDIT;
    _client=CreateWindow(szBITBLASS,0,WS_CHILD|WS_VISIBLE,
                         0,0,640,480,HWindow,0,(HINSTANCE)hInstance,NULL);
};


void TBMPChild::EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode){
    switch(command){
        case CM_SCALEBOX:{
            SetFocus(HWindow);
            double sc=GetScaleBox();
            if(sc>0){
                sc=SendMessage(_client,BM_SETSCALE,sc/100,0);
                SetScaleBox(sc);
            };
        };
        default:
#ifdef WIN32
            SendMessage(_client,WM_COMMAND, MAKEWPARAM((UINT)(command),(UINT)(0)), (LPARAM)(HWND)(HWindow));
#else
            SendMessage(_client,WM_COMMAND,command,0);
#endif
    }
};

BOOL TBMPChild::CanClose(){
    BOOL modifyed=SendMessage(_client,BM_ISMODIFYED,0,0);
    if (modifyed){
        //int rez=MessageBox(HWindow,"ќбновить пространство?","–едактирование битовой карты.",MB_YESNOCANCEL);

        int rez=MSGBOX(HWindow,MAKEINTRESOURCE(IDS_MSG21),_MSG_QSC,MB_YESNOCANCEL|MB_ICONQUESTION);
        if (rez==IDYES){
            HGLOBAL x=HMakeCopy((HGLOBAL)SendMessage(_client,BM_GETDIB,1,0));
            HGLOBAL a=HMakeCopy((HGLOBAL)SendMessage(_client,BM_GETDIB,2,0));
            if (hsp && object2d){
                int t=GetObjectType2d(hsp,object2d);
                HOBJ2D hdib=0,hddib=0;
                switch(t){
                    case otBITMAP2D :hdib=GetDibObject2d(hsp,object2d);break;
                    case otDOUBLEBITMAP2D:hddib=GetDoubleDibObject2d(hsp,object2d);break;
                }
                if(t==otDOUBLEBITMAP2D)SetDoubleDib2d(hsp,hddib,x,a);
                else SetDib2d(hsp,hdib,x);
            }
        }
        if (rez==IDCANCEL)    return FALSE;
    }
    return TRUE;
};


LRESULT TBMPChild::WndProc(UINT message, WPARAM wParam,LPARAM lParam){
    switch(message){
        case BMN_NEWMODE:PressControlButton(wParam,lParam);return wParam;
        case BMN_MOUSEPOS:{
            int x=LOWORD(lParam);
            int y=HIWORD(lParam);
            char s[64];
            wsprintf(s,"%d:%d",x,y);
            SCSetStatusText(2,s);
        };return 0;
        case WM_SETFOCUS:{
            double s=SendMessage(_client,BM_GETSCALE,0,0);
            SetScaleBox(s);
        }return 0;
    }
    return TMDIChildWindow::WndProc(message,wParam,lParam);
};
int TBMPChild::IsCommandDisable(int command){
    if(command==CM_SCALEBOX)return 0;
    return SendMessage(_client,BM_CMDDISABLE,command,0);
};

BOOL TBMPChild::OpenBMP(HGLOBAL x,HGLOBAL a){
    SendMessage(_client,BM_SETDIBS,(WPARAM)HMakeCopy(x),(LPARAM)HMakeCopy(a));
};

// ** S T A R T    B A L L O N **
static BYTE ballonclosed=0;
static TBallonWindow* hballon=0;

void CreateBallonWindow(){
    HWND ballon=CreateWindow("SC_BALLON_WINDOW",NULL,
                             WS_POPUP|WS_BORDER,0,0,80,20,mainwindow->HWindow,NULL,(HINSTANCE)hInstance,NULL);
    hballon=(TBallonWindow*)GetWindowLong(ballon,0);
};
void SetBallonData(int x,int y,char*text){
    if(!ballonclosed){
        if (!hballon)CreateBallonWindow();
        if(hballon)hballon->SetData(x,y,text);
    }
};
LRESULT TBallonWindow::WndProc(UINT message,WPARAM wParam,LPARAM lParam){
    switch(message){
        case WM_TIMER:{
            if (htimer){KillTimer(HWindow,htimer);htimer=0;
                wait=TRUE;
                _GetBallonData();
                wait=FALSE;
            }};return 0;
        case WM_PAINT:{
            PAINTSTRUCT ps;
            HDC hdc=BeginPaint(HWindow,&ps);
            HGDIOBJ b=CreateSolidBrush(
              #ifdef WIN32
                          GetSysColor(COLOR_INFOBK	)
              #else
                          GetSysColor(COLOR_BTNHIGHLIGHT)
              #endif
                          );
            HGDIOBJ old=SelectObject(hdc,b);
            PatBlt(hdc,ps.rcPaint.left,ps.rcPaint.top,
                   ps.rcPaint.right-ps.rcPaint.left,
                   ps.rcPaint.bottom-ps.rcPaint.top,PATCOPY);
            SetBkMode(hdc,TRANSPARENT);

            SetTextColor(hdc,
             #ifdef WIN32
                         GetSysColor(COLOR_INFOTEXT)
             #else
                         GetSysColor(COLOR_BTNTEXT)
             #endif
                         );
            SelectObject(hdc,HFont);
            TextOut(hdc,4,0,text,lstrlen(text));
            SelectObject(hdc,old);
            DeleteObject(b);
            EndPaint(HWindow,&ps);
        };return 0;
            // case WM_MOUSEACTIVATE:
        case WM_MOUSEMOVE:{
            HideWindow();
        };return 0;
    }
    return TWindow::WndProc(message,wParam,lParam);
};


TBallonWindow::TBallonWindow(LPCREATESTRUCT l,HWND hwnd):TWindow(l,hwnd){
    *text=0;
    htimer=0;
    wait=show=FALSE;
    cursory=GetSystemMetrics(SM_CYCURSOR)/2;
};

TBallonWindow::~TBallonWindow(){
    if (htimer)KillTimer(HWindow,htimer);htimer=0;
    hballon=0;
    ballonclosed=1;
};

void TBallonWindow::HideWindow(){
    if (show){
        ShowWindow(HWindow,SW_HIDE);
        show=FALSE;
    }
};

void TBallonWindow::AdjustPos(){
    HDC dc=GetDC(0);
    SIZE size;
    HGDIOBJ old=SelectObject(dc,HFont);
    GetTextExtentPoint(dc,text,lstrlen(text),&size);
    SelectObject(dc,old);
    UINT flags;
    HWND  hwndInsertAfter=0;
    int szx=size.cx+8;
    int szy=size.cy+2;

    if (show){hwndInsertAfter=HWND_TOP;
        flags=SWP_SHOWWINDOW|SWP_NOACTIVATE;
        RECT wr;
        GetWindowRect(GetDesktopWindow(),&wr);

        if(wr.right<(x+szx+5)){x=wr.right-(szx+5); }
        if(wr.bottom<(y+szy)){y=y-32; }

    } else
        flags=SWP_HIDEWINDOW|SWP_NOZORDER;

    SetWindowPos(HWindow,hwndInsertAfter,x,y,szx,szy,flags);
    ReleaseDC(0,dc);
};
void TBallonWindow::SetData(int _x,int _y,char*_text){

    if(wait){
        if (_text){
            if ((abs(_x-x)<3) &&(abs(_y-(y-cursory))<3)){
                lstrcpyn(text,_text,sizeof(text));
                show=TRUE;
                AdjustPos();
            }
            return;
        }
    }
    x=_x;y=_y+cursory;
    if (show){
        BOOL tohide=!(_text && *_text);
        if (tohide || lstrcmp(text,_text)){
            if (!tohide){
                lstrcpyn(text,_text,sizeof(text));
                //	HideWindow();
                show=TRUE;
                AdjustPos();
                InvalidateRect(HWindow,0,0);
            }else{
                *text=0;
                HideWindow();
            }
        }
    }else{
        if (_text && *_text){
            //  if (htimer){
            //    KillTimer(HWindow,htimer);
            //   }
            lstrcpy(text,_text);

            if (!htimer) htimer=SetTimer(HWindow,10,400,NULL);
        } }
};
// END BALLON


class TStatus{
public:
    enum status_modes {SM_CENTER=1,SM_FRAME=2};
    int start,size,percent;
    int _size;
    INT16 id;
    INT16 bmpid;
    UINT32 flags;
    char text[256];
    TStatus(int ,INT16 _id=0,status_modes sm=SM_FRAME);
};

TStatus::TStatus(int s,INT16 _id,status_modes sm){
    bmpid=0;
    size=s;
    _size=s;
    start=0;
    text[0]=0;
    flags=sm;
    percent=-1;
    id=_id;
};
TStatusBar::DoneIcons(){
    if(icons){DeleteObject(icons);icons=0;}
};
TStatusBar::~TStatusBar(){
    DoneIcons();
    DoneItems();
    delete items;
};

void TStatusBar::EvRButtonDown(WORD modKeys,int x,int y){
    POINT point;
    {POINT _point;GetCursorPos(&_point);
        ScreenToClient(HWindow,&_point);
        point=_point;
    }
    INT32 mode=SCGetScVar("StatusMode",2);
    HMENU p= ScLoadPopup(MENU_STATUS);
    CheckMenuItem(p,(mode==1)?CM_STATUSTOP:CM_STATUSBOTTOM,MF_BYCOMMAND|MF_CHECKED);
    ClientToScreen(HWindow,&point);
    TSTRATUMMENU sm;
    sm.hWindow=HWindow;
    sm.hMenu=p;
    sm.hType=SCMENU_STATUSBAR;
    mainwindow->TrackPopupMenu(point,&sm);
};

void TStatusBar::DoneItems(){
    for(C_TYPE i=0;i<items->count;i++){
        TStatus*status=(TStatus*)items->At(i);
        delete status;
    }
    items->DeleteAll();
};


C_TYPE steps;
C_TYPE perfomance;

void TStatusBar::Rebuild(){
    DoneItems();
    items->Insert(new TStatus(80,0,TStatus::SM_FRAME|TStatus::SM_CENTER));
    items->Insert(new TStatus(0,0,0));
    items->Insert(new TStatus(100,0,TStatus::SM_FRAME|TStatus::SM_CENTER));
    items->Insert(new TStatus(30));
    items->Insert(new TStatus(30,STATUS_ERRICON));
    items->Insert(new TStatus(30));

    if(SCGetScVar("status_perfomance",0)){
        perfomance=items->Insert(new TStatus(40,STATUS_PERFOMANCE));
    }else perfomance=-1;

    if(SCGetScVar("status_steps",0))steps=items->Insert(new TStatus(50,STATUS_STEPS));
    else steps=-1;
    ReCalc();
};

TStatusBar::TStatusBar(LPCREATESTRUCT l,HWND h):TWindow(l,h){
    icons=0;
    simple=1;
    lstrcpy(simpletext,"Stratum 3.0");
    sizey=20;
    items=new TCollection(10,0);
    Rebuild();
    status=0;
};

void TStatusBar::EvMouseMove(WORD modKeys,int x,int y){
    if(status&1){
        POINT p;
        p.x=x;
        p.y=y;
        MapWindowPoints(HWindow,GetParent(HWindow),&p,1);
        SetWindowPos(GetParent(HWindow),0,0,0,p.x+offset.x,p.y+offset.y,SWP_NOMOVE|SWP_NOZORDER);
    }else{
        if(!IsZoomed(GetParent(HWindow))){
            if(x>(sizex-16))SetCursor(0,IDC_SIZENWSE);else SetCursor(0,IDC_ARROW);
        }
    }
};

void TStatusBar::EvLButtonDown(WORD modKeys,int x,int y){
    if((x>(sizex-16)) && (!IsZoomed(GetParent(HWindow))) ){
        status|=1;
        offset.x=x;
        offset.y=y;
        MapWindowPoints(HWindow,GetParent(HWindow),&offset,1);
        RECT r;
        GetWindowRect(GetParent(HWindow),&r);
        r.right-=r.left;r.bottom-=r.top;
        offset.x=r.right-offset.x;
        offset.y=r.bottom-offset.y;
        SetCapture(HWindow);
    }
};
void TStatusBar::EvLButtonUp(WORD modKeys,int x,int y){
    if(status&1){
        status&=~1;
        ReleaseCapture();
    }
};

void TStatusBar::ReCalc(){
    int len=0;
    TStatus*ss=NULL;
    int i;
    for(i=0;i<items->count;i++){
        TStatus*status=(TStatus*)items->At(i);
        if (!status->_size)ss=status;
        len+=status->_size;
    }
    int _len=0;
    if (ss){
        ss->size=sizex-(len+19);
    }
    for(i=0;i<items->count;i++){
        TStatus*status=(TStatus*)items->At(i);
        status->start=_len;
        _len+=status->size;
    };
};
LRESULT TStatusBar::WndProc(UINT message, WPARAM wParam,LPARAM lParam){
    switch(message){
        case WM_PAINT:{
            HDC hdc;
            PAINTSTRUCT ps;
            hdc=BeginPaint(HWindow,&ps);
            Paint(hdc);
            EndPaint(HWindow,&ps);
        }return 0;
    }
    return TWindow::WndProc(message,wParam,lParam);
};
void TStatusBar::EvLButtonDblClk(WORD modKeys,int x,int y){
#ifndef RUNTIME
    TStatus* status=GetStatusByXY(x,y);
    if(status){
        switch(status->id){
            case STATUS_ERRICON:{if(status->bmpid)mainwindow->Cm_Message();break;}
        }}
#endif
};
TStatus* TStatusBar::GetStatusByXY(int x,int y){
    if(y>2 && y<sizey-2){
        for(int i=0;i<items->count;i++){
            TStatus*status=(TStatus*)items->At(i);
            if((x>status->start) && (x<(status->start+status->size)))return status;
        }}
    return NULL;
};

BOOL TStatusBar::GetBallonStr(BALLONSTRUCT *bs){
    TStatus*status= GetStatusByXY(bs->bsPoint.x,bs->bsPoint.y);
    if(status){
        switch(status->id){
            case STATUS_ERRICON:{
                switch(status->bmpid){
                    case 1:{
                        LoadString(HResource,IDS_str202,bs->bsText,bs->bsSize);
                        return TRUE;
                    };
                }
            };return FALSE;
            case STATUS_STEPS:{lstrcpy(bs->bsText,status->text);
                static char s1[40]="";
                if(!(s1[0])){
                    LoadString(HResource,IDS_str148,s1,sizeof(s1));
                }
                lstrcat(bs->bsText,s1);

            }break;
            case STATUS_PERFOMANCE:{lstrcpy(bs->bsText,status->text);
                static char s2[40]="";
                if(!(s2[0])){
                    LoadString(HResource,IDS_str149,s2,sizeof(s2));
                }
                lstrcat(bs->bsText,s2);
            }break;
            default:return FALSE;
        }
        return TRUE;
    }
    return FALSE;
};

BOOL NeedRedrawProgressBar(RECT&r,int oldpercent,int percent){
    BOOL f1=((oldpercent>=0) && (oldpercent<=1000));
    BOOL f2=((percent>=0) && (percent<=1000));
    if(f1!=f2)return TRUE;
    if(!f2)return FALSE;
    int sizex = (r.right-r.left);
    const int cell=10;
    int countnew=((double)percent*sizex)/(1000.0*cell);
    int countold=((double)oldpercent*sizex)/(1000.0*cell);
    return (countnew!=countold);
};
void DrawProgressBar(HDC hDc,RECT&_r,int percent){ //percent (0 - 1000)
    RECT r=_r;
    r.top++;r.bottom--;
    if(percent>1000)percent=1000;
    if(percent<0)percent=0;
    const int cell=10;
    const int delta=2;

    int sizex = (r.right-r.left);
    int count=((double)percent*sizex)/(1000.0*cell);
    int h=(r.bottom-r.top)-2;

    HGDIOBJ brush=CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION));
    HGDIOBJ old=SelectObject(hDc,brush);
    for(int i=0;i<count;i++){
        PatBlt(hDc,r.left+cell*i,r.top+1,cell-delta,h,PATCOPY);
    };

    SelectObject(hDc,Bface);
    i=r.left+cell*count;
    PatBlt(hDc,i,r.top+1,r.right-i,h,PATCOPY);
    DeleteObject(brush);

};

void TStatusBar::PaintIcon(HDC hdc,TStatus*status,int id,BOOL center){
    if(!icons)icons=ScLoadBitmap(HResource,"status_icons",isizex,isizey,1,RGB(255,0,255));
    HDC hDC=CreateCompatibleDC(hdc);
    SelectObject(hDC,icons);
    BitBlt(hdc,status->start+(center?((status->size-isizex)/2):4),4,isizex,isizex,hDC,0,id*isizex,SRCCOPY);
    DeleteDC(hDC);
};
void TStatusBar::Paint(HDC hdc){
    HGDIOBJ oldbrush=SelectObject(hdc,Bface);
    // PatBlt(hdc,0,2,sizex,sizey-2,PATCOPY);
    PatBlt(hdc,0,0,sizex,sizey,PATCOPY);
    if (simple){
        PaintFrame(hdc,0,sizex-18);
        DrawText(hdc,0,sizex-19,simpletext,0);
    }else{
        for(int i=0;i<items->count;i++){
            TStatus*status=(TStatus*)items->At(i);
            if(status->flags&TStatus::SM_FRAME)PaintFrame(hdc,status->start+1,status->size);
            if(status->bmpid){
                PaintIcon(hdc,status,status->bmpid-1,TRUE);
            }else{
                if(status->percent>-1){
                    DrawProgressBar(hdc,i);
                }else {
                    DrawText(hdc,status->start,status->size,status->text,status->flags&TStatus::SM_CENTER);
                    if(i==2) PaintIcon(hdc,status,1,FALSE);
                }
            }
        }}
    if(!IsZoomed(GetParent(HWindow))){
        HPEN pl,pd,po;
        pl=CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNHIGHLIGHT));
        pd=CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
        po=(HPEN)SelectObject(hdc,pl);
        int i=13;
        int j=0;
        while(i>0){
            POINT pp[2];
            pp[0].x=sizex-i; pp[0].y=sizey-2;
            pp[1].x=sizex-1;pp[1].y=sizey-i-1;
            switch(j){
                case 0:SelectObject(hdc,pl);j++;break;
                case 1:SelectObject(hdc,pd);j++;break;
                case 2:i--;j=0;break;
                    //   case 3:
            }
            //  if(j)
            Polyline(hdc,pp,2);
            i--;
        }
        SelectObject(hdc,po);
        DeleteObject(pl);
        DeleteObject(pd);
    }
    SelectObject(hdc,oldbrush);
};

TStatusBar extern *_StatusBar;

void SetPerfomance(){
    static DWORD lasttime=0;
    static DWORD tmp=0;
    static DWORD lastcount=0;
    if(perfomance>-1){
        DWORD time1;
        DWORD time=time1=GetTickCount();

        time=(time-lasttime);
        lasttime=time1;
        tmp+=time;
        if(tmp>1000){
            char text[32];
            double ps=(calcedsteps-lastcount)*1000.0/(double)tmp;
            if(ps<=0.0)lstrcpy(text,"0.0");else {
                sprintf(text,"%4.4g",ps);
            }
            _StatusBar->SetText(perfomance,text);
            lastcount=calcedsteps;
            tmp = 0;
        }
    }
};
void SetLastSteps()
{
    if(steps > -1)
    {
        char text[32];
        wsprintf(text,"%ld",calcedsteps);
        _StatusBar->SetText(steps,text);
    }
};

void TStatusBar::DrawProgressBar(HDC hdc,int i){
    TStatus * status=(TStatus*)items->At(i);
    PaintFrame(hdc,status->start+1,status->size);
    RECT r;
    r.left=status->start+4;
    r.right=(r.left+status->size)-8;
    r.top=7;
    r.bottom=r.top+13;
    ::DrawProgressBar(hdc,r,status->percent);
};
TStatus *TStatusBar::GetStatus(int id){
    TStatus*status=NULL;
    if(id<0){
        for(int i=0;i<items->count;i++){
            TStatus*_status=(TStatus*)items->At(i);
            if (status->id==id){status=_status;break;}
        }
    }else if(id<items->count)status=(TStatus*)items->At(id);
    return status;
};
void TStatusBar::SetText(int id,char*text){
    if (simple)InvalidateRect(HWindow,NULL,0);
    TStatus*status=GetStatus(id);
    if(!status)return;
    int oldpercent=status->percent;
    if(HIWORD(text)==0){

        status->percent=LOWORD(text);
        if(status->percent>1000){status->percent=-1;InvalidateRect(HWindow,NULL,0);return ;}
    }

    if ((status->percent>-1) || lstrcmp(text,status->text)){
        if(HIWORD(text))lstrcpy(status->text,text);
        if(!simple){
            HDC hdc=GetDC(HWindow);
            if(status->flags&TStatus::SM_FRAME)PaintFrame(hdc,status->start+1,status->size);
            if(status->percent>-1){
                RECT r;
                r.left=status->start+3;
                r.right=(r.left+status->size)-6;
                if(NeedRedrawProgressBar(r,oldpercent,status->percent)){
                    if(oldpercent==-1){
                        SelectObject(hdc,Bface);
                        PatBlt(hdc,status->start+1,0,status->size-2,sizey,PATCOPY);
                    }
                    DrawProgressBar(hdc,id);
                }
            }else	{
                DrawText(hdc,status->start,status->size,status->text,status->flags&TStatus::SM_CENTER);
                if(id==2) PaintIcon(hdc,status,1,FALSE);
            }
            ReleaseDC(HWindow,hdc);
        }
    }
    simple=FALSE;
};
void TStatusBar::SetIcon(int id,int _id)
{
    if(simple)
        InvalidateRect(HWindow,NULL,0);
    TStatus*status=GetStatus(id);
    if(!status)
        return;
    if(status->bmpid!=_id){
        status->bmpid=_id;
        HDC hdc=GetDC(HWindow);
        PaintFrame(hdc,status->start+1,status->size);
        if(status->bmpid)PaintIcon(hdc,status,status->bmpid-1,TRUE);else
        {
            if(status->percent>-1){
                DrawProgressBar(hdc,id);
            }else	DrawText(hdc,status->start,status->size,status->text,status->flags&TStatus::SM_CENTER);
        }

        ReleaseDC(HWindow,hdc);
    }
};

void TStatusBar::PaintFrame(HDC hdc,int x,int size,int dy){
    int sizex=size;
    int _sizey=sizey;
    sizey-=3+2*dy;
    int y=3+dy;
    HGDIOBJ oldbrush=SelectObject(hdc,Bdark);
    DR_LEFTTOP(PATCOPY)
            SelectObject(hdc,Blight);
    DR_RIGHTBOTTOM(PATCOPY)
            SelectObject(hdc,oldbrush);
    sizey=_sizey;
};
void TStatusBar::SetStatusMode(BOOL mode){
    if(mode==simple){
        simple=(!mode);
        InvalidateRect(HWindow,0,0);
    }
};
void TStatusBar::SetSimpleText(char*txt){
    simple=TRUE;
    lstrcpy(simpletext,txt);
    InvalidateRect(HWindow,0,0);
};
void TStatusBar::DrawText(HDC hdc,int x,int size,char*text,BOOL center){
    RECT r;
    r.left=x+2;r.right=(x+size)-6;
    r.top=4;r.bottom=sizey-2;

    HGDIOBJ oldbrush=SelectObject(hdc,Bface);
    PatBlt(hdc,r.left,r.top,size-5,r.bottom-r.top,PATCOPY);
    SelectObject(hdc,oldbrush);

    int len=lstrlen(text);
    if (len){
        SelectObject(hdc,GetStockObject(ANSI_VAR_FONT));
        SetBkMode(hdc,TRANSPARENT);


        if (center)SetTextAlign(hdc,TA_CENTER|TA_TOP);
        else SetTextAlign(hdc,TA_LEFT|TA_TOP);
        ExtTextOut(hdc,center?(x+size/2):(x+4),5,ETO_CLIPPED,&r,text,len,NULL);
    }
};

void TStatusBar::EvSize(WORD/* sizeType*/,int sx,int sy){
    sizex=sx;
    sizey=sy;
    ReCalc();
};


//--------------------------------
//******  Color Button  **********
//--------------------------------

TColorButton::TColorButton(LPCREATESTRUCT l,HWND h):TWindow(l,h){
    style=(BYTE)l->style;
    rgb=0;
    sizex=30,sizey=10;
    state=0;
    modify=FALSE;
};

TColorButton::~TColorButton(){
};

extern BYTE palette[];
LRESULT TColorButton::WndProc(UINT message, WPARAM wParam,LPARAM lParam){
    switch(message){
        case WM_SETFOCUS:{state|=1;Repaint();  }return 0;
        case WM_KILLFOCUS:{state&=!1;Repaint();}return 0;
        case COLORB_GETCOLOR:return rgb;
        case COLORB_ISCHANGE:return modify;
        case COLORB_SETCOLOR:{
            if (lParam!=rgb){
                rgb=lParam;
                modify=0;
                Repaint();
            }
        }return 0;
        case WM_KEYDOWN:{
            if (wParam==VK_SPACE){
                state|=2;
                Repaint();
                state&=(~2);
                InvalidateRect(HWindow,NULL,0);
                SetColor();
                return 0;
            }
        }break;
        case WM_PAINT:{HDC hdc;
            PAINTSTRUCT ps;
            hdc=BeginPaint(HWindow,&ps);
            Paint(hdc);
            EndPaint(HWindow,&ps);
        }return 0;
        case WM_GETDLGCODE:return DLGC_BUTTON;
    }
    return TWindow::WndProc(message,wParam,lParam);
};

void TColorButton::SetColor(BOOL mode){
    if(mode){
        RECT r;
        GetWindowRect(HWindow,&r);
        POINT p;p.x=r.left;p.y=r.bottom;
        int f=1;
        if(style&CBS_SYSCOLOR)f|=2;
        if(style&CBS_TRANSPARENT)f|=4;
        if(ChoseColor(HWindow,p,rgb,(DWORD)HWindow,f)){
            modify=TRUE;
            Repaint();
            SendMessage(GetParent(HWindow),COLORB_COLORCHANGE,(WPARAM)HWindow,
            #ifdef WIN32
                        GetWindowLong(HWindow,GWL_ID)
            #else
                        GetWindowWord(HWindow,GWW_ID)
            #endif
                        );
        }
    }else{

        char s[256];
        GetWindowText(HWindow,s,sizeof(s));

        if (_ChoseColor(HWindow,rgb,s)){
            modify=TRUE;
            Repaint();
            SendMessage(GetParent(HWindow),COLORB_COLORCHANGE,(WPARAM)HWindow,
            #ifdef WIN32
                        GetWindowLong(HWindow,GWL_ID)
            #else
                        GetWindowWord(HWindow,GWW_ID)
            #endif
                        );
        }
    }
};


void TColorButton::Paint(HDC hdc){

    //---------------------------------------
    BYTE _flags=2;
    if (state&2)_flags|=1;
    DrawShadowFrame(hdc,0,0,sizex,sizey,_flags);
    int dx=1;
    int dy=1;
    if (state&2){ dx++;dy++;}

    //----------------------
    COLORREF _rgb=rgb;
    if(_rgb&0x2000000L)_rgb=GetSysColor(_rgb&0xff);

    HBRUSH color=CreateSolidBrush(_rgb);
    RECT r;
    r.left=dx+sizex*1/3;
    r.top=dy+4;
    r.bottom=sizey-6+dy;
    r.right=(sizex-7)+dx;

    HGDIOBJ oldbrush=SelectObject(hdc,Bdark);
    PatBlt(hdc,r.left-1,r.top-1,1+r.right-r.left,1,PATCOPY);
    PatBlt(hdc,r.left-1,r.top,1,r.bottom-r.top,PATCOPY);

    PatBlt(hdc,r.left-5,r.top-1,1,r.bottom-r.top,PATCOPY); //  |

    SelectObject(hdc,Blight);
    PatBlt(hdc,r.left+1,r.bottom,r.right-r.left,1,PATCOPY);
    PatBlt(hdc,r.right,r.top,1,r.bottom-r.top,PATCOPY);

    PatBlt(hdc,r.left-4,r.top-1,1,r.bottom-r.top,PATCOPY); //  |

    SelectObject(hdc,color);
    PatBlt(hdc,r.left,r.top,r.right-r.left,r.bottom-r.top,PATCOPY);

    if (state&1){
        r.left-=2;r.right+=2;
        r.top-2;r.bottom+=2;
        DrawFocusRect(hdc,&r);
    }
    SelectObject(hdc,oldbrush);

    DeleteObject(color);
}
void TColorButton::Repaint(){
    HDC hdc=GetDC(HWindow);
    Paint(hdc);
    ReleaseDC(HWindow,hdc);
};
void TColorButton::EvSize(WORD/* sizeType*/,int sx,int sy){
    sizex=sx;
    sizey=sy;
};
void TColorButton::EvLButtonDown(WORD,int,int){
    state|=6;
    Repaint();
    SetCapture(HWindow);
};
void TColorButton::EvLButtonUp(WORD,int,int){
    if (state&4){
        int nc;
        nc=state&2;
        state&=(~6);
        ReleaseCapture();
        Repaint();
        if (nc)SetColor(style!=CBS_STDDLG);
    }
};
void TColorButton::EvMouseMove(WORD,int xPos,int yPos){
    POINT point;
    point.x=xPos;point.y=yPos;
    if (state&4){
        RECT r;
        r.left=r.top=0;
        r.bottom=sizey;r.right=sizex;
        if (PtInRect(&r,point)){
            if (!(state&2))state|=2;Repaint();
        }else{
            if (state&2)state&=~2;Repaint();
        }}
};

void DrawShadowLine(HDC hdc,int x,int y,int sizex,int sizey){
    BOOL h=(sizey==0);
    if(sizex==0)sizex=1;
    if(sizey==0)sizey=1;
    HGDIOBJ oldbrush=SelectObject(hdc,Bdark);
    PatBlt(hdc,x,y,sizex,sizey,PATCOPY);
    // SetPixel(hdc,x,y,0);
    if(h)y++;else x++;
    SelectObject(hdc,Blight);

    PatBlt(hdc,x,y,sizex,sizey,PATCOPY);
    SelectObject(hdc,oldbrush);

};

void DrawShadowFrame(HDC hdc,int x,int y,int sizex,int sizey,BYTE flags){
    HGDIOBJ oldbrush=SelectObject(hdc,Bdark);
    int dx=1;
    int dy=1;
    if (flags&1){ //Down
        if(flags & 4){ x--; y--;sizex++;sizey++; }
        dx++;
        dy++;
        if(!(flags&4)){
            DR_LEFTTOP(BLACKNESS)
        }
        PatBlt(hdc,x+1,y+1,sizex-1,1,PATCOPY);
        PatBlt(hdc,x+1,y+2,1,sizey-2,PATCOPY);
        SelectObject(hdc,Blight);

        if(flags & 4){
            x++; y++;sizex--;sizey--;
        }
        DR_RIGHTBOTTOM(PATCOPY)

                if (!(flags&4)){
            COLORREF rgb=GetSysColor(COLOR_BTNFACE);
            SetPixel(hdc,x+1,y+sizey-2,rgb);
            SetPixel(hdc,x+sizex-1,y+1,rgb);
        }

    }else {
        if(flags & 4){sizex++;sizey++;}
        PatBlt(hdc,x,y+sizey-2,sizex-1,1,PATCOPY);
        PatBlt(hdc,x+sizex-2,y,1,sizey-1,PATCOPY);

        SelectObject(hdc,Blight);
        if(flags&4){sizex-=2;sizey-=2;}
        DR_LEFTTOP(PATCOPY)
                if(flags & 4){sizex+=2;sizey+=2;}
        if(!(flags&4)){
            DR_RIGHTBOTTOM(BLACKNESS)
        }	}
    if (flags&2){
        SelectObject(hdc,Bface);
        PatBlt(hdc,x+dx,y+dy,sizex-3,sizey-3,PATCOPY);}
    SelectObject(hdc,oldbrush);
};
/****
 View dib

**///////////////////
void  TViewDIB::SetText()
{
    char s[256];
    wsprintf(s,"Source x:%d ,y:%d  cx:%d ,cy:%d, W:%d, H:%d",
             viewrect.left,viewrect.top,
             viewrect.right-viewrect.left,
             viewrect.bottom-viewrect.top,
             maxrect.right,maxrect.bottom);
    SetDlgItemText(GetParent(HWindow),IDC_SRCBMPTEXT,s);
};

TViewDIB::TViewDIB(LPCREATESTRUCT l,HWND h):TSpaceWindow(l,h)
{
    object=0;
    hsp=CreateSpace2d(HWindow,0);
    HOBJ2D hd=CreateDib2dFromResource(hsp,HResource,MAKEINTRESOURCE(BM_MASK));
    HOBJ2D hb=CreateBrush2d(hsp,BS_PATTERN,0,0,hd,R2_MASKPEN);
    SetBkBrush2d(hsp,hb);
    POINT2D p[4]={{0,0},{0,0},{0,100},{0,0}};
    bwin=CreatePolyline2d(hsp,0,hb,p,4);
    win=CreatePolyline2d(hsp,0,hb,p,4);
    SetObjectAlpha2d(hsp,win,0.0);
    SetObjectAlpha2d(hsp,bwin,0.0);
    RGNGROUPITEM r[2]={{RGN_DIFF,bwin},{RGN_DIFF,win}};
    mask=CreateRgnGroup2d(hsp,r,2);
    state=0;
    HOBJ2D hp=CreatePen2d(hsp,PS_SOLID,1,RGB(255,0,0));
    for(int i=0;i<4;i++)lines[i]=CreatePolyline2d(hsp,hp,0,p,2);

    vscroll=new TScroller(this,0,10,1,SB_VERT);
    hscroll=new TScroller(this,0,10,1,SB_HORZ);
    //hscroll->autohide=0;
    //vscroll->autohide=0;
};

BOOL IsVideoFrame2d(HSP2D hsp,HOBJ2D obj)
{
    int _type=GetObjectType2d(hsp,obj);
    if (_type==otUSEROBJECT2D && GetFrameProp2d(hsp,obj,NULL))return TRUE;
    return FALSE;
};

void TViewDIB::NewDib(HSP2D h,HOBJ2D _object,int type,HOBJ2D hdib)
{
    char _buf[260];
    if(GetTexturePath2d(h,_buf))
        SetTexturePath2d(hsp,_buf);
    BOOL frame=IsVideoFrame2d(h,_object);
    if (object)
    {
        if (frame)
        {
            HOBJ2D dib=GetDibObject2d(hsp,object);
            DeleteObject2d(hsp,object);
            DeleteTool2d(hsp,DIB2D,dib);
        }
        else
            DeleteObject2d(hsp,object);
        object=0;
    }
    if (frame)
    {
        long size;
        HGLOBAL hglb1=CopyToMemory2d(h,_object,&size);
        POINT2D pastepoint;
        pastepoint.x=pastepoint.y=10;
        object=PasteFromMemory2d(hsp,hglb1,&pastepoint,PFC_MOVEOBJECT);
        GlobalFree(hglb1);
        FRAMEPROP fp;
        GetFrameProp2d(hsp,object,&fp);
        fp.src.left=0;
        fp.src.top=0;
        fp.src.right=0xffff;
        fp.src.bottom=0xffff;
        fp.flags=fp.flags&(~V2_SYNC);
        SetFrameProp2d(hsp,object,&fp);
        GetFrameProp2d(hsp,object,&fp);
        POINT2D osize;osize.x=fp.src.right;osize.y=fp.src.bottom;
        SetObjectSize2d(hsp,object,&osize);
    }
    else
    {
        if (hdib==0)return;
        HGLOBAL hglb=type==DIB2D?GetDib2d(h,hdib):GetXorDib2d(h,hdib);
        {//копируем диб
            long size=GlobalSize(hglb);
            HGLOBAL hglb1=GlobalAlloc(GMEM_MOVEABLE,size);
#ifdef WIN32
            memcpy(GlobalLock(hglb1),GlobalLock(hglb),size);
#else
            hmemcpy(GlobalLock(hglb1),GlobalLock(hglb),size);
#endif
            GlobalUnlock(hglb);GlobalUnlock(hglb1);
            hglb=hglb1;
        }
        HOBJ2D dib=CreateDib2d(hsp,hglb);//создаем новый диб
        POINT2D p_0,p_1,p10;
        p_0.x=p_0.y=0;p_1.x=p_1.y=-1;p10.x=p10.y=10;
        object=CreateBitmap2d(hsp,dib,&p_0,&p_1,&p10,&p_1);//создаем новый битмаз
    }
    POINT2D size;
    GetObjectOrgSize2d(hsp,object,NULL,&size);
    ObjectToBottom2d(hsp,object);
    viewrect.left=0;viewrect.top=0;
    viewrect.bottom=size.y;viewrect.right=size.x;
    maxrect=viewrect;
    POINT2D p[4];
    GetVectorPoints2d(hsp,bwin,p,4);
    p[2].x=p[1].x=size.x+20;
    p[2].y=p[3].y=size.y+20;
    SetVectorPoints2d(hsp,bwin,p,4);
    GetVectorPoints2d(hsp,win,p,4);
    p[0]=TPoint(10,10);									p[1]=TPoint(10+size.x,10);
    p[2]=TPoint(10+size.x,10+size.y);		p[3]=TPoint(10,10+size.y);
    SetVectorPoints2d(hsp,win,p,4);
    GetVectorPoints2d(hsp,lines[0],p,2);
    p[0]=TPoint(10,0);									p[1]=TPoint(10,size.y+20);
    SetVectorPoints2d(hsp,lines[0],p,2);
    GetVectorPoints2d(hsp,lines[1],p,2);
    p[0]=TPoint(10+size.x,0);						p[1]=TPoint(10+size.x,size.y+20);
    SetVectorPoints2d(hsp,lines[1],p,2);
    GetVectorPoints2d(hsp,lines[2],p,2);
    p[0]=TPoint(0,10);									p[1]=TPoint(20+size.x,10);
    SetVectorPoints2d(hsp,lines[2],p,2);
    GetVectorPoints2d(hsp,lines[3],p,2);
    p[0]=TPoint(0,10+size.y);						p[1]=TPoint(20+size.x,10+size.y);
    SetVectorPoints2d(hsp,lines[3],p,2);
    // Need scroll bars ??
    {
        POINT scrollsize;
        scrollsize.x=GetSystemMetrics(SM_CXVSCROLL);
        scrollsize.y=GetSystemMetrics(SM_CYHSCROLL);
        RECT r;GetWindowRect(HWindow,&r);
        int sx=(r.right-r.left),sy=(r.bottom-r.top),
                xsize=maxrect.right-maxrect.left+20,
                ysize=maxrect.bottom-maxrect.top+20;
        int nv=0,nh=0;
        if (xsize>sx)nh=1;
        if (ysize>(sy-scrollsize.y*nh))nv=1;
        if (xsize>(sx-scrollsize.x*nv))nh=1;
        //ShowScrollBar(HWindow,SB_HORZ,nh);
        //ShowScrollBar(HWindow,SB_VERT,nv);
        hscroll->SetRange(0,xsize);//(xsize-(scrollsize.x*nv)));
        vscroll->SetRange(0,ysize);//(ysize-(scrollsize.y*nh)));
        //hscroll->Show(nh);
        //vscroll->Show(nv);
        //ShowScrollBar(HWindow,SB_HORZ,nh);
        //ShowScrollBar(HWindow,SB_VERT,nv);
        POINT2D org=TPoint(0,0);
        if (!nh)
        {
            org.x=-((sx-scrollsize.x*nv)-(size.x+20))/2;
        }
        if (!nv)
        {
            org.y=-((sy-scrollsize.y*nh)-(size.y+20))/2;
        };
        SetOrgSpace2d(hsp,&org);
    }
    SetText();
};

void TViewDIB::SetRect(RECT r){
    POINT2D p[4];
    GetVectorPoints2d(hsp,win,p,4);
    RECT r1=r;r1.left+=10;r1.top+=10;
    r1.right+=10;r1.bottom+=10;

    if(p[0].x!=r1.left  || p[0].y!=r1.top)
        SetVectorPoint2d(hsp,win,0,&TPoint(r1.left,r1.top));
    if(p[1].x!=r1.right || p[1].y!=r1.top)
        SetVectorPoint2d(hsp,win,1,&TPoint(r1.right,r1.top));
    if(p[2].x!=r1.right || p[2].y!=r1.bottom)
        SetVectorPoint2d(hsp,win,2,&TPoint(r1.right,r1.bottom));
    if(p[3].x!=r1.left  || p[3].y!=r1.bottom)
        SetVectorPoint2d(hsp,win,3,&TPoint(r1.left,r1.bottom));

    int n;
    GetVectorPoints2d(hsp,lines[0],p,2);
    n=10+r.left;
    if (n!=p[0].x){
        p[0].x=p[1].x=n;
        SetVectorPoints2d(hsp,lines[0],p,2);}

    GetVectorPoints2d(hsp,lines[1],p,2);
    n=10+r.right;
    if (n!=p[0].x){
        p[0].x=p[1].x=n;
        SetVectorPoints2d(hsp,lines[1],p,2);}

    GetVectorPoints2d(hsp,lines[2],p,2);
    n=10+r.top;
    if (n!=p[0].y){
        p[0].y=p[1].y=n;
        SetVectorPoints2d(hsp,lines[2],p,2);}

    GetVectorPoints2d(hsp,lines[3],p,2);
    n=10+r.bottom;
    if (n!=p[0].y){
        p[0].y=p[1].y=n;
        SetVectorPoints2d(hsp,lines[3],p,2);}
    viewrect=r;
    SetText();
};

void TViewDIB::EvLButtonDown(WORD/* modKeys*/,int xPos,int yPos)
{
    POINT2D point;point.x=xPos;point.y=yPos;
    DPtoLP2d(hsp,&point,1);
    select=GetObjectFromPoint2d(hsp,&point);
    if (select==lines[0])state=1;
    if (select==lines[1])state=2;
    if (select==lines[2])state=3;
    if (select==lines[3])state=4;
    if (select && select==object)state=5;
    if (state)
    {
        SetCapture(HWindow);
        original=viewrect;
        org=point;
        org.x-=10;org.y-=10;
    }
};

void TViewDIB::EvLButtonUp(WORD/* modKeys*/,int,int)
{
    //DPtoLP2d(hsp,&point,1);
    if (state>0){ReleaseCapture();state=0;}
};

void TViewDIB::GetSpaceInfo(char *s){
    LoadString(HResource,IDS_str150,s,sizeof(s));
};

void TViewDIB::EvMouseMove(WORD /*modKeys*/,int xPos,int yPos)
{
    POINT2D point;point.x=xPos;point.y=yPos;
    DPtoLP2d(hsp,&point,1);
    if (state){point.x-=10;point.y-=10;}
    RECT r=viewrect;
    switch (state)
    {
        case 0:
            select=GetObjectFromPoint2d(hsp,&point);
            if (select==lines[0]||select==lines[1])SetCursor(NULL,IDC_SIZEWE);
            else if (select==lines[2]||select==lines[3])SetCursor(NULL,IDC_SIZENS);
            else if (select && select==object/* && viewrect!=maxrect*/)SetCursor(NULL,IDC_SIZE);
            else SetCursor(NULL,IDC_ARROW);
            return;
        case 1:if (point.x<r.right)r.left=point.x;break;
        case 2:if (point.x>r.left)r.right=point.x;break;
        case 3:if (point.y<r.bottom)r.top=point.y;break;
        case 4:if (point.y>r.top)r.bottom=point.y;break;
            //  case 5:r=original;r+=(TSize(point-org));break;
        case 5:
        {
            r=original;
            r.left+=(point.x-org.x);
            r.right+=(point.x-org.x);
            r.top+=(point.y-org.y);
            r.bottom+=(point.y-org.y);
        }break;
            //  case 5:r.bottom=point.y;break;
    }
    if (r.left	<0)	r.left=0;
    if (r.right	<0)	r.right=0;
    if (r.top		<0)	r.top=0;
    if (r.bottom<0)	r.bottom=0;

    if (r.right>maxrect.right){r.right=maxrect.right;}
    if (r.left>maxrect.right){r.left=maxrect.right;}
    if (r.bottom>maxrect.bottom){r.bottom=maxrect.bottom;}
    if (r.top>maxrect.bottom){r.top=maxrect.bottom;}
    if (r.left!=viewrect.left ||
        r.right!=viewrect.right ||
        r.top!=viewrect.top 		||
        r.bottom !=viewrect.bottom  )SetRect(r);
};

HBITMAP ScLoadBitmap(HINSTANCE hi,LPSTR res,int&bmpsizex,int&bmpsizey,BYTE flags,COLORREF bk){
    HBITMAP hb=LoadBitmap(hi,res);
    if (hb){
        BITMAP bm;
        GetObject(hb, sizeof(BITMAP), &bm);
        bmpsizex=bm.bmWidth,bmpsizey=bm.bmHeight;
        if(flags&1){

            HBITMAP hmask=CreateBitmap(bm.bmWidth,bm.bmHeight,1,1,NULL);
            HDC wdc,hDC,maskDC;
            wdc=GetDC(0);
            hDC=CreateCompatibleDC(wdc);
            maskDC=CreateCompatibleDC(wdc);
            SelectObject(maskDC,hmask);
            SelectObject(hDC,hb);
            if(flags&2){
                bk=GetPixel(hDC,0,0);
            }
            PatBlt(maskDC,0, 0, bmpsizex, bmpsizey, WHITENESS);
            SetBkColor(hDC,bk);  // btnface to white
            BitBlt(maskDC,0, 0, bmpsizex, bmpsizey,hDC,0,0, SRCCOPY);
            PatBlt(maskDC,0, 0, bmpsizex, bmpsizey,DSTINVERT);
            SetTextColor(hDC,0);
            SetBkColor(hDC,RGB(255,255,255));
            BitBlt(hDC,0, 0, bmpsizex, bmpsizey,maskDC,0,0,SRCAND);
            SetTextColor(hDC,0);
            SetBkColor(hDC,GetSysColor(COLOR_BTNFACE));
            PatBlt(maskDC,0, 0, bmpsizex, bmpsizey,DSTINVERT);
            BitBlt(hDC,0, 0, bmpsizex, bmpsizey,maskDC,0,0,SRCPAINT);

            DeleteDC(hDC);
            DeleteDC(maskDC);
            ReleaseDC(0,wdc);
        };

    }
    return hb;
};

TMBPView::TMBPView(LPCREATESTRUCT l,HWND h):TWindow(l,h){
    long style=l->style;
    hb=0;
    hglobal=0;
    flags=0;
    if(style&VB_DIB){
        flags|=VB_DIB;
    }else{
        char s[80];
        GetWindowText(HWindow,s,79);
        int id=atoi(s);

        HINSTANCE hi=(style&VB_EXE)?hInstance:HResource;
        LPCSTR _id;
        if (id)_id=MAKEINTRESOURCE(id);else _id=s;

        if(flags&VB_BUTTON)flags|=1;
        if(flags&VB_CHECKED)flags|=3;
        BYTE _flags=0;
        if(style & VB_TRANSPARENT)_flags|=1;
        hb=ScLoadBitmap(hi,(char*)_id,sizex,sizey,1,RGB(192,192,192));
        int _x=sizex, _y=sizey;
        GetWinSizeByClient(_x,_y);
        if(flags&1){
            _x+=3;_y+=3;
        };
        SetWindowPos(HWindow,0,0,0,_x,_y,SWP_NOMOVE|SWP_NOZORDER);
    }
};
void  TWindow::GetWinSizeByClient(int &_x,int &_y){
    /*

 long style   = GetWindowLong(HWindow,GWL_STYLE);//GWL_EXSTYLE
 long exstyle = GetWindowLong(HWindow,GWL_EXSTYLE);

 int dx=0,dy=0;
 if (style &WS_BORDER){
 dx=GetSystemMetrics(SM_CXBORDER);
 dy=GetSystemMetrics(SM_CYBORDER);
 }
 if (style &WS_DLGFRAME || exstyle &WS_EX_DLGMODALFRAME){
 dx=GetSystemMetrics(SM_CXDLGFRAME)+GetSystemMetrics(SM_CXBORDER);
 dy=GetSystemMetrics(SM_CYDLGFRAME)+GetSystemMetrics(SM_CYBORDER);
 }
 if (style &WS_CAPTION){
#ifdef WIN32
if(exstyle&WS_EX_TOOLWINDOW)
_y += GetSystemMetrics(SM_CYSMCAPTION);
else  _y+=GetSystemMetrics(SM_CYCAPTION);
#else
_y+=GetSystemMetrics(SM_CYCAPTION);
#endif
     // May be - SM_CYBORDER
}
_x=_x+2*dx;
_y=_y+2*dy;
*/
    RECT r;
    r.left=r.top=0;
    r.right=_x;r.bottom=_y;
    DWORD style=GetWindowLong(HWindow,GWL_STYLE);
#ifdef WIN32
    DWORD dwExStyle=GetWindowLong(HWindow,GWL_EXSTYLE);
    AdjustWindowRectEx(&r, style,FALSE,dwExStyle );
#else
    AdjustWindowRect(&r, style, FALSE);
#endif
    _x=r.right-r.left;
    _y=r.bottom-r.top;
};

void TMBPView::EvLButtonDown(WORD modKeys,int xPos,int yPos){
    if(flags&1){
        flags|=0x100;
        SetCapture(HWindow);
        Repaint(1);
    }
};
void TMBPView::EvLButtonUp(WORD modKeys,int xPos,int yPos){
    if(flags&0x100){
        ReleaseCapture();
        if(InWindow(xPos,yPos)){
            if(flags&2)flags^=0x400;
            SendCommand(GetParent(HWindow),HWindow,BN_CLICKED);
        }
        flags&=~0x300;
        Repaint(0);
    }
};
void TMBPView::EvMouseMove(WORD modKeys,int xPos,int yPos){
    if(flags&0x100){
        Repaint(InWindow(xPos,yPos));
    }
};

BOOL TMBPView::InWindow(int xPos,int yPos){
    if((xPos>=0)&&(yPos>=0)){
        if((xPos<=sizex+3)&&(yPos<=sizey+3))return TRUE;
    }
    return FALSE;
};

BOOL TMBPView::Repaint(BOOL p){
    if(((flags&0x200)!=0)!=p){
        if(p)flags|=0x200;else flags&=~0x200;
        InvalidateRect(HWindow,0,0);
    }
};
long NColors(WORD bitCount);
LRESULT TMBPView::WndProc(UINT message, WPARAM wParam,LPARAM lParam){
    switch(message){
        case BMP_SETDIB:{
            hglobal=(HGLOBAL)wParam;
            InvalidateRect(HWindow,0,0);
        }return 0;
        case BMP_GETDIB:{
            return (LPARAM)hglobal;
        }return 0;

        case WM_PAINT:{
            HDC hdc;
            PAINTSTRUCT ps;
            hdc=BeginPaint(HWindow,&ps);
            int _d=0;
            if(flags&1){_d+=1;
                DrawShadowFrame(hdc,0,0,sizex+3,sizey+3,(flags&0x100)!=0);
            }
            BOOL down=((flags&0x200)!=0)^((flags&0x400)!=0);
            if(down && flags&1)_d+=1;
            if(flags&VB_DIB){
                RECT r;
                GetClientRect(HWindow,&r);
                SelectObject(hdc,BGrayed);
                PatBlt(hdc,0,0,r.right,r.bottom,PATCOPY);
                if(hglobal){
                    BITMAPINFO *lpBitsInfo=(BITMAPINFO *)GlobalLock(hglobal);
                    int sx=lpBitsInfo->bmiHeader.biWidth;
                    int sy=lpBitsInfo->bmiHeader.biHeight;
                    int NumClrs = NColors(lpBitsInfo->bmiHeader.biBitCount);
                    int colorAlloc = (int)NumClrs * sizeof(RGBQUAD);
                    char*Bits = (char far*)lpBitsInfo + ((int)lpBitsInfo->bmiHeader.biSize + colorAlloc);
                    int _dx=_d,_dy=_d;
                    if(sx<r.right)_dx=(r.right-sx)/2;
                    if(sy<r.bottom)_dy=(r.bottom-sy)/2;
                    StretchDIBits(hdc,_dx,_dy,sx,sy,
                                  0,0,sx,sy,
                                  Bits,lpBitsInfo,DIB_RGB_COLORS,SRCCOPY);
                    GlobalUnlock(hglobal);
                }
            }else{
                HDC bdc=CreateCompatibleDC(hdc);
                SelectObject(bdc,hb);

                BitBlt(hdc,_d,_d,sizex,sizey,bdc,0,0,SRCCOPY);
                DeleteDC(bdc);
            }

            EndPaint(HWindow,&ps);
        }return 0;
    }
    return TWindow::WndProc(message,wParam,lParam);
};

TMBPView::~TMBPView(){
    if(!(flags&VB_DIB))DeleteObject(hb);
};

/**************************************************************

                             B R O W S E R

**************************************************************/
TCollumn::TCollumn(int _amin,int _amax,int atype){
    _min=_amin;
    _max=_amax;
    type=atype;
    current=_max;
};
TCollumn::TCollumn(TCollumn*c){
    memcpy(this,c,sizeof(TCollumn));
}
/*
BOOL CALLBACK TableDialogProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam){
 switch(message){
  case WM_INITDIALOG:
  case WM_SIZE:{
    RECT r;
    HWND edit=GetDlgItem(hwnd,IDC_TABLEEDIT);
    GetClientRect(hwnd,&r);
 SetWindowPos(edit,0,0,0,r.right,r.bottom,
    SWP_NOZORDER|SWP_SHOWWINDOW);
  }return TRUE;
  case WM_KEYDOWN:{
      switch(wParam){
  case VK_TAB:
  case VK_LEFT:
  case VK_RIGHT:
    SendMessage(GetParent(hwnd),WM_KEYDOWN,wParam,lParam);
  return TRUE;
  }
  }break;
 }
 return FALSE;
};*/


/*
WNDPROC oldeditproc=NULL;
LRESULT CALLBACK _export _TblEditProc( HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam){
LRESULT ret= oldeditproc(hwnd,message,wParam,lParam);
//return oldeditproc(hwnd,message,wParam,lParam);

if (message==WM_KEYDOWN){
 if (wParam==VK_TAB || wParam==VK_RETURN)
  {
    SendMessage(GetParent(hwnd),message,wParam,lParam);
    return 1;
  }
 if (wParam==VK_UP){
 SendMessage(GetParent(hwnd),message,wParam,lParam);
     return 0;
 }
 if (wParam==VK_PRIOR){SendMessage(GetParent(hwnd),message,wParam,lParam); return 0;}
 if (wParam==VK_NEXT){SendMessage(GetParent(hwnd),message,wParam,lParam); return 0;}
 if (wParam==VK_DOWN){
 SendMessage(GetParent(hwnd),message,wParam,lParam);
  return 0;
 }
};
if (message==WM_GETDLGCODE){
  ret|=DLGC_WANTTAB;
 }
 return ret;
};
*/

LRESULT CALLBACK _TblEditProc(t_pointer data,HWND hwnd, UINT &msg, WPARAM &wParam, LPARAM &lParam,BOOL & calldefault){
    calldefault=1;
    switch(msg){
        case WM_KEYDOWN:
            switch(wParam){
                case VK_ESCAPE:
                case VK_TAB:
                case VK_UP:
                case VK_PRIOR:
                case VK_NEXT:
                case VK_RETURN:
                case VK_DOWN:{
                    SendMessage(GetParent(hwnd),msg,wParam,lParam);
                    calldefault=0;
                    return 0;
                }
            }break;

        case WM_GETDLGCODE:{
            calldefault=0;
            return DLGC_WANTTAB|DLGC_WANTALLKEYS;
        }break;
    }
    return 0;
};


TBrowser::TBrowser(LPCREATESTRUCT l,HWND h):TWindow(l,h){
    items=new TCollection(10,10);
    top=0;
    left=0;
    activeX=0;
    activeY=0;
    delta=0;
    wholeXsize=0;
    font=(HFONT)GetStockObject(ANSI_VAR_FONT);
    notify=GetParent(HWindow);
    combo=0;
    edit=CreateWindow("Edit",NULL,
                      WS_BORDER|WS_CHILD|ES_AUTOHSCROLL|ES_LEFT|WS_VISIBLE,0,0,100,16,HWindow,
                      (HMENU)BC_EDIT,hInstance,0);
    /*
  oldeditproc=(WNDPROC)GetWindowLong(edit,GWL_WNDPROC);
  SetWindowLong(edit,GWL_WNDPROC,(LPARAM)_TblEditProc);
*/

    CreateWinHook(edit,_TblEditProc,NULL);

    SendMessage(edit,WM_SETFONT,(WPARAM)font,0);
    vscroll=new TScroller(this,0,10,CELL_H,SB_VERT);
    vscroll->flags|=SF_FIXTOP;
    hscroll=new TScroller(this,0,wholeXsize,1,SB_HORZ);
    vscroll->ChangeSize(l->cy);
    hscroll->ChangeSize(l->cx);
    vlabel=hlabel=0;
    SetRange(0);
    SetActive(0,0);
    activevalid=0;
};

void TBrowser::SetRange(long c){
    count=c;
    if (count<0)count=0;
    if(activeY<0)activeY=0;
    if (count==0){
        if (edit)ShowWindow(edit,SW_HIDE);
        if (combo)ShowWindow(combo,SW_HIDE);
        activeY=activeX=0;
    }else{
        if (activeY>=count)SetActive(count-1,activeX);
        //activeY=count-1;
    }
    if(vlabel)InvalidateRect(vlabel->HWindow,0,1);
    InvalidateRect(HWindow,0,1);
    vscroll->SetRange(0,c);
};

TBrowser::~TBrowser(){
    DeleteItems();
    delete items;
    //  DeleteObject(bitmap);
};
void TBrowser::DeleteItems(){
    TCollumn*col;
    for(int i=0;i<items->count;i++){
        col=(TCollumn*)items->At(i);
        delete col;
    }
    items->DeleteAll();
}
void TBrowser::Paint(HDC hdc,RECT&r){

    SelectObject(hdc,Blight);
    PatBlt(hdc,r.left,r.top,r.right-r.left,r.bottom-r.top,PATCOPY);
    SetBkMode(hdc,TRANSPARENT);
    if(!items->count)return;
    TCollumn *c=(TCollumn *)items->At((int)left);
    if (!c)return;
    int pos=(int)(c->start-delta+c->current);
    int j=(int)left;

    int _bottom=r.bottom/CELL_H+(int)top;
    if (_bottom>=count)
        _bottom=(int)((count-top)*CELL_H);else
        _bottom=r.bottom;

    if (j){
        c=(TCollumn *)items->At(j);
        if (!(c->start-delta))PatBlt(hdc,0,0,1,_bottom,BLACKNESS/*PATCOPY*/);
    }
    //  int _bottom=
    while
            (pos<r.right && j<items->count){
        c=(TCollumn *)items->At(j);
        pos=(int)(c->current+c->start-delta);
        PatBlt(hdc,pos,0,1,_bottom,BLACKNESS/*PATCOPY*/);
        j++;
    };

    {
        long i=top+r.top/CELL_H;
        int y=(r.top/CELL_H)*CELL_H;
        while(y<r.bottom && i<=count){
            PatBlt(hdc,r.left,y,r.right-r.left,1,BLACKNESS/*PATCOPY*/);
            y+=CELL_H;
            i++;
        };
    }
    long x,y,_y,_x;
    int _ileft=(int)left;
    _y=top+r.top/CELL_H;
    y=(_y-top)*CELL_H;
    c=(TCollumn *)items->At(_ileft);
    int waspaint=0;
    while(c->current+c->start-delta<r.left && _ileft<items->count){
        _ileft++;
        if(_ileft<items->count)c=(TCollumn *)items->At(_ileft);

    }
    if (_ileft<items->count){
        SelectObject(hdc,font);
        long _Lstart=c->start-delta;
        while(y<=r.bottom && _y<count){
            x=_Lstart;
            _x=_ileft;
            while (x<r.right && _x<items->count){
                TCollumn *c=(TCollumn *)items->At((int)_x);
                if (!waspaint){SendMessage(notify,TBN_BEGINDRAW,0,0);waspaint=1;}
                PaintCell(hdc,_x,_y,(int)x+3,(int)y+3);
                x+=c->current;
                _x++;
            }
            y+=CELL_H;_y++;
        }
    }
    if (waspaint)SendMessage(notify,TBN_ENDRAW,0,0);
};
void TBrowser::EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode){
    switch(command){
        case BC_COMBO:{
            switch(wNotifyCode){
                case CBN_SELENDOK:{
                    CELLINFO ci;
                    ci.message=1;
                    ci.y=activeY;ci.x=activeX;
                    char text[256];
                    GetWindowText(combo,text,255);
                    ci.text=text;
                    SendMessage(notify,TBN_SETTEXT,(WPARAM)HWindow,(LPARAM)&ci);
                }break;
            }
        }break;;
        case BC_EDIT:{
            switch(wNotifyCode){
                case EN_KILLFOCUS:{
                    SetData(0);
                }break;
            }
        }break;
        default:TWindow::EvCommand(command,hwndCtl,wNotifyCode);
    }
};
void TBrowser::EvLButtonDblClk(WORD modKeys,int xPos,int yPos){
    long y,x;
    if(GetCellByMouse(xPos,yPos,x,y)){
        CELLINFO ci;
        ci.x=x;
        ci.y=y;
        SendMessage(notify,TBN_DBLCLK,(WPARAM)HWindow,(LPARAM)&ci);
    }
}
LRESULT TBrowser::WndProc(UINT message, WPARAM wParam,LPARAM lParam){
    switch(message){
#ifdef WIN32
        case WM_HELP:HELP(GetWindowLong(HWindow,GWL_ID),0,0,HWindow);return 0;
#endif
        case WM_PARENTNOTIFY:{
            POINT point;
            int fwEvent = wParam;
            point.x = LOWORD(lParam);
            point.y = HIWORD(lParam);
            if(fwEvent==WM_RBUTTONDOWN){
                //ClientToScreen(HWindow,&point);
                EvRButtonDown(0,point.x,point.y);
            }
        }return 0;
        case TB_UPDATECELL:{
            CELLINFO*ci=((CELLINFO*)lParam);
            if((ci->x==activeX) && (ci->y==activeY)){
                SetActive(ci->y,ci->x,FALSE);
            }
            WndProc(TB_GETCELLRECT,0,(LPARAM)ci);
            InvalidateRect(HWindow,&(ci->rect),0);
        }return TRUE;
        case TB_GETCELLRECT:{
            CELLINFO*ci=((CELLINFO*)lParam);
            TCollumn *c=(TCollumn *)items->At(ci->x);
            ci->rect.left=(c->start-delta);
            ci->rect.right=ci->rect.left+c->current;
            ci->rect.top=((ci->y-top)*CELL_H);
            ci->rect.bottom=ci->rect.top+CELL_H;
            return TRUE;
        }break;
        case TB_GETCOLLUMN:{
            int i=(int)wParam;
            TCollumn *c=(TCollumn *)items->At(i);
            RECT& r=*(RECT*)lParam;;
            r.top=0;
            r.left=(c->start-delta);
            r.right=r.left+c->current;
            long _y=((count-top)*CELL_H);
            if(_y>sizey)r.bottom=sizey;else r.bottom=_y;
        };return TRUE;
        case TB_SETLABEL:{
            hlabel=(TBrowserLabel*)GetWindowLong((HWND)wParam,0);
            vlabel=(TBrowserLabel*)GetWindowLong((HWND)lParam,0);
            if (hlabel)hlabel->browser=this;
            if (vlabel)vlabel->browser=this;
        }return 0;
        case TB_ADDCOLLUMN:{
            TCollumn *c=new TCollumn((TCollumn*)lParam);
            if (((C_TYPE)wParam)>-1) items->AtInsert((C_TYPE)wParam,c);else
                items->Insert(c);
            ReCalc();
            SetActive(activeX,activeY);
        }return 0;
        case TB_RESETCOLLUMNS:{
            DeleteItems();
            delta=0;
        }break;
        case TB_SETACTIVE:{
            if(((CELLINFO*)lParam)->y<0){
                if (edit)ShowWindow(edit,SW_HIDE);
                if (combo)ShowWindow(combo,SW_HIDE);
            }else{
                SetActive(((CELLINFO*)lParam)->y,((CELLINFO*)lParam)->x,0);
            }
        }return 0;
        case TB_GETACTIVE:{
            ((CELLINFO*)lParam)->x=activeX;
            ((CELLINFO*)lParam)->y=activeY;
        }return 0;
        case TB_SETTEXT:{
            CELLINFO* ci=(CELLINFO*)lParam;
            if (activeX==ci->x && activeY==ci->y){
                activeY=-1;
            }else{
                TCollumn *cl=(TCollumn *)items->At((int)ci->x);
                RECT r;
                r.left = (int)(cl->start-delta);
                r.top = (int)((ci->y-top)*CELL_H);
                r.right=r.left+cl->current;
                r.bottom=r.top+CELL_H;
                InvalidateRect(HWindow,&r,1);
            }
        }return 0;
        case TB_SETRANGEY:SetRange(lParam);return 0;
        case TB_GETCOUNT:if(wParam)return items->count;
            else return count;
        case WM_SETFOCUS:if(!(flags&BF_INSETACTIVE)){if(subwindow && IsWindowVisible(subwindow))SetFocus(subwindow); else
                {
                    if(activevalid)SetActive(activeX,activeY);}
            }return 0;
        case WM_GETDLGCODE:return DLGC_WANTMESSAGE|DLGC_DEFPUSHBUTTON;
        case WM_KEYDOWN:{
            switch(wParam){
                case VK_TAB:{
                    long x=activeX,y=activeY;
                    if (GetAsyncKeyState(VK_SHIFT)&0x8000u)x--;else x++;
                    if (x<0){y--;x=items->count-1;}
                    if (x>=items->count){y++;x=0;}
                    SetActive(y,x);
                }return 0;
                case VK_RETURN:{
                    SendMessage(edit,EM_SETMODIFY,1,0);
                    SetData(TRUE);
                    SendMessage(edit,EM_SETMODIFY,0,0);
                };break;
                case VK_PRIOR:SetActive(activeY-vscroll->GetPage(),activeX);break;
                case VK_NEXT:SetActive(activeY+vscroll->GetPage(),activeX);break;
                case VK_LEFT:SetActive(activeY,activeX-1);break;
                case VK_RIGHT:SetActive(activeY,activeX+1);break;
                case VK_UP:SetActive(activeY-1,activeX);break;
                case VK_DOWN:SetActive(activeY+1,activeX);break;
                case VK_ESCAPE:{
                    SendMessage(GetParent(HWindow),message,wParam,lParam);
                    return TRUE;
                }
            }
            break;
        }
        case WM_PAINT:{
            HDC hdc;
            PAINTSTRUCT ps;
            hdc=BeginPaint(HWindow,&ps);
            Paint(hdc,ps.rcPaint);
            EndPaint(HWindow,&ps);
        }return 0;
    }
    return TWindow::WndProc(message,wParam,lParam);

};
void TBrowser::ReCalc(){
    long xx=0;
    for(int i=0;i<items->count;i++){
        TCollumn *c=(TCollumn *)items->At(i);
        c->start=xx;
        xx+=c->current;
        wholeXsize=c->current+c->start;
    }
    hscroll->SetRange(0,wholeXsize);
}
void TBrowser::ScrollTo(long*x,long*y){
    long odelta=delta;
    int oleft=(int)left;
    int otop=(int)top;
    if (y){
        top = *y;
    }
    ReCalc();
    if (x){
        for(int i=0;i<items->count;i++){
            TCollumn *c=(TCollumn *)items->At(i);
            if ((c->start+c->current)>*x){left=i;break;}
        }
        delta=(int)(*x);
    };
    if(vlabel && otop!=top) vlabel->Repaint();
    if(hlabel && (oleft!=left||odelta!=delta)) hlabel->Repaint();
};
void TBrowser::Cm_Down(){
};
void TBrowser::Cm_Up(){
};
void TBrowser::Cm_Insert(){
};
void TBrowser::PaintCell(HDC dc,long x,long y,int xpos,int ypos){
    RECT r;
    TCollumn *c=(TCollumn *)items->At((C_TYPE)x);
    r.left=(int)(c->start-delta);
    r.right=(int)(c->start+c->current-(delta+1));
    r.top=ypos-3;
    r.bottom=r.top+CELL_H-2;
    if(c->type&CF_DRAW)
    {
        CELLINFO cell;
        cell.message=1;
        cell.y=y;cell.x=x;
        cell.text=NULL;
        cell.rect=r;
        cell.hDC=dc;
        cell.ok=FALSE;
        SendMessage(notify,TBN_PAINTCELL,(WPARAM)HWindow,(LPARAM)&cell);
        if(cell.ok)return;
    }
    char s[257];
    SetTextColor(dc,GetStr(y,x,s));
    ExtTextOut(dc,xpos,ypos,ETO_CLIPPED,&r,s,lstrlen(s),NULL);
};

BOOL TBrowser::GetCellByMouse(int xPos,int yPos,LONG&x,LONG&y){
    y=top+yPos/CELL_H;x=-1;
    if(y>=count)return FALSE;
    for(long i=left;i<items->count;i++){
        TCollumn *c=(TCollumn *)items->At((int)i);
        if ((c->start-delta)<xPos&&(c->start+c->current-delta)>xPos)
        {x=i;break;}
    }
    return (x>-1);
};

void TBrowser::EvRButtonDown(WORD,int xPos,int yPos){
    long y,x;
    if(GetCellByMouse(xPos,yPos,x,y)){
        CELLINFO ci;
        ci.y=y;ci.x=x;
        ci.text="";
        ci.color=RGB(0,0,0);
        SetActive(y,x);
        SendMessage(notify,TBN_RBUTTON,(WPARAM)HWindow,(LPARAM)&ci);
    }
};

void TBrowser::EvLButtonDown(WORD/* modKeys*/,int xPos,int yPos){
    long y,x;
    if(GetCellByMouse(xPos,yPos,x,y)) SetActive(y,x);
};

void TBrowser::EvSize(WORD sizeType,int sx,int sy){
    TWindow::EvSize(sizeType,sx,sy);
    sizex=sx;sizey=sy;
};


void TBrowser::UpdateSize(){
    long _x,_y;
    TCollumn *c=(TCollumn *)items->At((int)activeX);
    _x=(c->start-delta);
    _y=((activeY-top)*CELL_H);
    if (_y<(-CELL_H))_y=-CELL_H;
    if (_y>2048l)_y=2048l;

    if (subwindow==edit){
        SetWindowPos(subwindow,0,(int)_x+1,(int)_y+1,(int)c->current-1,(int)CELL_H-1,
                     SWP_NOZORDER|SWP_SHOWWINDOW);
    }
    if (subwindow==combo){
        SetWindowPos(subwindow,0,(int)_x,(int)_y,(int)c->current,200,
                     SWP_NOZORDER|SWP_SHOWWINDOW);
    }
};
void TBrowser::SetActive(long y,long x,BOOL __notify){
    BYTE needupdate=0;
    flags|=BF_INSETACTIVE;
    if (items->count==0 || count==0l){
        ShowWindow(edit,SW_HIDE);
        if (combo)ShowWindow(combo,SW_HIDE);
        flags&=~BF_INSETACTIVE;return;
    }
    if (x<0)x=0;//items->count-1;
    if (x>=items->count)x=items->count-1;//0;
    if (y<0)y=y=0;//count-1;
    if (y>=count)y=count-1;//0;
    activevalid=1;
    {if((activeX!=x) || (activeY!=y))
        {if(!SetData()){
                flags&=~BF_INSETACTIVE;return;
            }}
    }
    BOOL achanged=(activeX!=x || activeY!=y);
    activeX=x;activeY=y;

    if (top>activeY){
        top=activeY;
        vscroll->SetPos(top);
        //if(vlabel)vlabel->Repaint();
        needupdate|=1;
    }
    if (activeY-top>((sizey/CELL_H)-1l)){
        top=(activeY-((long)sizey)/CELL_H)+1l;
        vscroll->SetPos(top);
        //	 if(vlabel)vlabel->Repaint();
        needupdate|=1;
    }
    TCollumn *c=(TCollumn *)items->At((int)activeX);
    if (activeX<left || c->start-delta<0){
        left=activeX;
        delta=(int)c->start;
        hscroll->SetPos(delta);
        //	 if(hlabel)hlabel->Repaint();
        needupdate|=2;
    };
    if (c->start+c->current-delta>sizex){
        delta=(INT16)(c->start+c->current-sizex);
        for(C_TYPE i=0;i<activeX;i++){
            TCollumn *cc=(TCollumn *)items->At(i);
            if (cc->start+cc->current>delta){ // was c вместо cc
                left=i;break;
            }
        }
        hscroll->SetPos(delta);
        //if(hlabel)hlabel->Repaint();
        needupdate|=2;
    };
    long _x,_y;
    c=(TCollumn *)items->At((int)x);
    _x=(c->start-delta);
    _y=((activeY-top)*CELL_H);
    if (_y<(-CELL_H))_y=-CELL_H;
    if (_y>2048l)_y=2048l;

    SelectEdit();

    CELLINFO cell;
    cell.message=1;
    cell.y=activeY;cell.x=activeX;
    cell.text=NULL;
    cell.rect.left=_x;
    cell.rect.top=_y;
    cell.rect.right=_x+c->current;
    cell.rect.bottom=_y+CELL_H;
    SendMessage(notify,TBN_GETEDITRECT,(WPARAM)HWindow,(LPARAM)&cell);

    if (subwindow==edit){
        SetWindowPos(subwindow,0,cell.rect.left+1,cell.rect.top+1,
                     (cell.rect.right-cell.rect.left)-1,(cell.rect.bottom-cell.rect.top)-1,
                     SWP_NOZORDER|SWP_SHOWWINDOW);

    }
    if (subwindow==combo){
        SetWindowPos(subwindow,0,cell.rect.left,cell.rect.top,(cell.rect.right-cell.rect.left),200,
                     SWP_NOZORDER|SWP_SHOWWINDOW);
    }
    SetFocus(subwindow);
    if(achanged && __notify)SendMessage(notify,TBN_ACTIVECHANGED,(WPARAM)HWindow,0);
    char s[256];
    GetStr(y,x,s);
    if (edit==subwindow)SendMessage(edit,EM_SETREADONLY,(c->type&CF_READONLY),0);
    SetWindowText(edit,s);

    SendMessage(edit,EM_SETMODIFY,0,0);
    SendMessage(edit,EM_SETSEL,0,-1); // выдел¤ем

    if(needupdate&2 && hlabel)hlabel->Repaint();
    if(needupdate&1 && vlabel)vlabel->Repaint();

    flags&=~BF_INSETACTIVE;
};


COLORREF TBrowser::GetStr(long y,long x,char*s){
    CELLINFO ci;
    ci.y=y;ci.x=x;
    ci.text=s;
    ci.color=RGB(0,0,0);
    ci.ok=TRUE;
    if (x>=0 && x<items->count && y>=0 && y<count){
        SendMessage(notify,TBN_GETTEXT,(WPARAM)HWindow,(LPARAM)&ci);
        if(!ci.ok ){if(!((y==0)&& (count==0)))SetRange(max(0,y));s[0]=0;}
    }else
        s[0]=0;
    return ci.color;
};

BOOL TBrowser::SetData(BOOL msg){
    if (subwindow==edit){
        if (!SendMessage(edit,EM_GETMODIFY,0,0))return 1;
    }else return 1;
    {
        CELLINFO ci;
        ci.message=1;
        ci.y=activeY;ci.x=activeX;
        char text[256];
        GetWindowText(edit,text,255);
        ci.text=text;
        SendMessage(notify,TBN_SETTEXT,(WPARAM)HWindow,(LPARAM)&ci);
        if (ci.ok)return TRUE;

        if (ci.message && msg)MSGBOX(HWindow,MAKEINTRESOURCE(IDS_str151),
                                     _MSG_ERROR,MB_OK|MB_ICONSTOP);
        return FALSE;
    }
};
void TBrowser::SelectEdit(){
    TCollumn *c=(TCollumn *)items->At((int)activeX);
    if (c->type&2)
    {
        long style=CBS_AUTOHSCROLL|WS_VSCROLL;
        if(c->type&1) style |=CBS_DROPDOWNLIST;else style|=CBS_DROPDOWN;
        if (!combo){ combo=CreateWindow("COMBOBOX",NULL,
                                        WS_BORDER|WS_CHILD|WS_VISIBLE|style ,0,0,100,200,HWindow,
                                        (HMENU)BC_COMBO,hInstance,0);
            SendMessage(combo,WM_SETFONT,(WPARAM)font,0);
        }
        subwindow=combo;
        SendMessage(combo,CB_RESETCONTENT,0,0);
        CELLINFO ci;
        ci.y=activeY;ci.x=activeX;
        SendMessage(notify,TBN_UPDATECOMBO,(WPARAM)combo,(LPARAM)&ci);
        ShowWindow(edit,0);
    } else
    {
        subwindow=edit;if(combo)ShowWindow(combo,0);
    }
};

/*
 BOOL h;
 TBrowser* browser;*/

TBrowserLabel::TBrowserLabel(LPCREATESTRUCT l,HWND hw):TWindow(l,hw){
    browser=0;
    long style=GetWindowLong(HWindow,GWL_STYLE);
    h=(style&BLS_HORIZONTAL)!=0;
    flags=(style&BLS_RESIZE)!=0;
    if(style&BLS_MOUSENOTIFY)flags|=4;

    markcurrent=(style&BLS_MARKCURRENT)!=0;
    gettext=(style&BLS_USERTEXT)!=0;
    sizey=l->cy;
    sizex=l->cx;
};
int TBrowserLabel::GetFromMouse(int px){
    TCollumn *c;
    for(int i=(int)browser->left;i<browser->items->count;i++){
        c=(TCollumn *)(browser->items->At(i));
        int x=(int)((c->start+c->current)-browser->delta);
        if (px>x-3 && px<x+3){return i;}

    }
    return -1;
};
void TBrowserLabel::EvLButtonDown(WORD ,int xPos,int yPos){
    if (flags &1){
        int i=GetFromMouse(xPos);
        if (i>-1){
            flags|=2;
            drag=i;
            SetCapture(HWindow);
            SetCursor(HResource,IDC_RESIZEX);
            return;}
    }
    if(flags&4){
        if(h){

            for(int i=(int)browser->left;i<browser->items->count;i++){
                TCollumn *c=(TCollumn *)(browser->items->At(i));
                int x=(int)((c->start)-browser->delta);
                if(xPos>x && (x+c->current)>xPos){
                    char s[80];
                    TLABELINFO info(HWindow,s);
                    info.pos=i;
                    SendMessage(GetParent(HWindow),TBL_LBUTTON,0,(LPARAM)&info);
                    return;
                }
            }

        }else{

            int y=browser->top+yPos/CELL_H;
            if(y>=0 && y<browser->count){
                char s[80];
                TLABELINFO info(HWindow,s);
                info.pos=y;
                SendMessage(GetParent(HWindow),TBL_LBUTTON,0,(LPARAM)&info);
            }
        }
    }
};

void TBrowserLabel::EvLButtonUp(WORD ,int,int){
    if (flags &2){
        ReleaseCapture();
        SetCursor(0,IDC_ARROW);
        flags &=~2;
        // Ѕыло здесь
    }
};
void TBrowserLabel::EvMouseMove(WORD ,int xPos,int){
    if (flags &2){
        TCollumn *c=(TCollumn *)(browser->items->At(drag));
        long x=(xPos+browser->delta)-c->start;
        if (x>5){
            if (x>c->_max)x=c->_max;
            c->current=(int)x;
            browser->ReCalc();
            Repaint();
            // Ѕыло там
            InvalidateRect(browser->HWindow,0,1);
            browser->UpdateSize();
        }
    }else
        if (flags &1){
            int i=GetFromMouse(xPos);
            if (i>-1)SetCursor(HResource,IDC_RESIZEX);else SetCursor(0,IDC_ARROW);
        }
};

void TBrowserLabel::EvSize(WORD /*sizeType*/,int sx,int sy){
    sizex=sx;
    sizey=sy;
};
LRESULT TBrowserLabel::WndProc(UINT message, WPARAM wParam,LPARAM lParam){
    switch(message){
        case WM_PAINT:{
            HDC hdc;
            PAINTSTRUCT ps;
            hdc=BeginPaint(HWindow,&ps);
            Paint(hdc,(ps.rcPaint));
            EndPaint(HWindow,&ps);
        }return 0;
    }
    return TWindow::WndProc(message,wParam,lParam);
};
void TBrowserLabel::Paint(HDC hdc,RECT&r){
    PaintPanel(hdc,r);
};
void TBrowserLabel::DrawSText(HDC hdc,int x,int y,char*s,RECT *rect){
    SetTextColor(hdc,RGB(255,255,255));
    int l=lstrlen(s);
    if(rect) ExtTextOut(hdc,x+1,y+1,ETO_CLIPPED,rect,s,l,NULL);
    else TextOut(hdc,x+1,y+1,s,l);
    SetTextColor(hdc,RGB(0,0,0));
    if(rect) ExtTextOut(hdc,x,y,ETO_CLIPPED,rect,s,l,NULL);
    else TextOut(hdc,x,y,s,l);
};
void TBrowserLabel::PaintPanel(HDC hdc,RECT&r){
    //PatBlt(hdc,4,4,sizex-8,sizey-8,WHITENESS);
    HBRUSH face=CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    HGDIOBJ old=SelectObject(hdc,face);
    PatBlt(hdc,0,0,sizex,sizey,PATCOPY);
    SelectObject(hdc,old);
    DeleteObject(face);
    if (!browser)return;
    SetBkMode(hdc,TRANSPARENT);
    SelectObject(hdc,GetStockObject(ANSI_VAR_FONT));
    SetTextAlign(hdc,TA_CENTER|TA_TOP);
    TCollumn *c;
    HWND parent=NULL;
    if (gettext)parent=GetParent(HWindow);
    char s[80];
    TLABELINFO info(HWindow,s);
    if (h){
        RECT rect;rect.top=1;rect.bottom=sizey-1;
        for(int i=(int)browser->left;i<browser->items->count;i++){
            c=(TCollumn *)(browser->items->At(i));
            if ((c->start-browser->delta)>r.right)break;
            DrawShadowFrame(hdc,(int)c->start-browser->delta,0,(int)c->current,sizey,0);
            rect.left=(int)c->start-browser->delta;
            rect.right=rect.left+(int)c->current;
            if (gettext){info.pos=i;
                SendMessage(parent,TBL_GETTEXT,0,(LPARAM)&info);}	 else
                wsprintf(s,"%ld",i);
            DrawSText(hdc,(int)(c->start+c->current/2-browser->delta),2,s,&rect);
        }
    }else{
        for(long i=browser->top;i<browser->count;i++){
            //	 int style=0;
            //	 if (markcurrent && browser->activeX==i)style=1;
            long y=((i-browser->top)*CELL_H);
            if (y>r.bottom)break;
            DrawShadowFrame(hdc,0,(int)y,(int)sizex,(int)CELL_H,0);
            if (gettext){info.pos=i;
                SendMessage(parent,TBL_GETTEXT,0,(LPARAM)&info);}	else
                wsprintf(s,"%ld",i);
            DrawSText(hdc,sizex/2,(int)(((i-browser->top)*CELL_H)+2),s,NULL);
        } }
};
void TBrowserLabel::Repaint(){
    HDC hdc=GetDC(HWindow);
    RECT r;
    GetClientRect(HWindow,&r);
    PaintPanel(hdc,r);
    ReleaseDC(HWindow,hdc);
};
//********************************************************************
//                  T C u r s o r  W i n d o w
//********************************************************************
int DefChar(unsigned char ch);

class TBrFiled{
public:
    char * title;
    char * field;
    BOOL   readonly;
    int    w;
    int    num;

    TBrFiled(char *&ps);
    TBrFiled();
    char * ReadTitle(char *ps);
    int    ReadInt(char *&ps);
    ~TBrFiled();
};
int TBrFiled::ReadInt(char *&ps){
    long w=0;
    while(DefChar(*ps)==2){
        w=w*10+(*ps-'0');
        ps++;
    }
    return (int)w;
};
char * TBrFiled::ReadTitle(char *ps){
    if (*ps=='\''){
        ps++;
        char *_ps=ps;
        ps++;
        while(*ps && *ps!='\''){
            ps++;
        }
        if (_ps-ps){
            int l=(int)(ps-_ps)+1;
            title=new char[l];
            lstrcpyn(title,_ps,l);
            ps++;
            return ps;
        }
    }
    return NULL;
};
TBrFiled::TBrFiled(){
    memset(this,0,sizeof(TBrFiled));w=-1;
};

TBrFiled::TBrFiled(char *&_ps){
    memset(this,0,sizeof(TBrFiled));
    w=-1;
    char *ps=_ps;
    if (*ps && (DefChar(*ps)==1))ps++;
    while((DefChar(*ps)==1) ||(DefChar(*ps)==2))ps++;
    int l=(int)(ps-_ps)+1;
    field=new char[l];
    lstrcpyn(field,_ps,l);
    _ps=ps;
    while (*ps){
        if (*ps=='/'){
            ps++;
            if (*ps=='h'){
                ps++;
                if (*ps=='='){
                    ps++;
                    ps=ReadTitle(ps);
                    if (!ps)return;
                }
            }else{
                if (*ps=='r'){
                    readonly=TRUE;
                    ps++;
                } } }else {
            if (*ps==':'){
                ps++;
                w=ReadInt(ps);
            }else ps++;
        }
    }
};
TBrFiled::~TBrFiled(){
    if (title)delete title;
    if (field)delete field;
};

#ifdef DBE
TCursorWindow::TCursorWindow(LPCREATESTRUCT l,HWND h):TWindow(l,h)
{
    inmdi=0;
    table=0;
    int dx=50,dy=20;
    browser=CreateWindow("SC_Browser",NULL,WS_CHILD|WS_VISIBLE,
                         dx,dy,640,480,HWindow,NULL,(HINSTANCE)hInstance,NULL);

    hl=CreateWindow("SC_BLabel",NULL,
                    WS_CHILD|BLS_HORIZONTAL|BLS_RESIZE|BLS_USERTEXT|BLS_MOUSENOTIFY|WS_VISIBLE,
                    dx+1,0,640,dy,HWindow,(HMENU)IDC_HLABEL,(HINSTANCE)hInstance,NULL);

    vl=CreateWindow("SC_BLabel",NULL,BLS_USERTEXT|WS_CHILD|WS_VISIBLE,
                    0,dy,dx,480,HWindow,(HMENU)IDC_VLABEL,(HINSTANCE)hInstance,NULL);
    EvSize(0,l->cx,l->cy);
    if((l->style&CWF_NOMENU)==0){
        SendMessage(CreateWindow("BUTTON","Info...",BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE,
                                 0,0,dx,dy,HWindow,(HMENU)IDC_INFO,(HINSTANCE)hInstance,NULL),WM_SETFONT,
                    (WPARAM)GetStockObject(ANSI_VAR_FONT),0);
    }
    SendMessage(browser,TB_SETLABEL,(WPARAM)hl,(WPARAM)vl);
    items=new TCollection(10,10);
    searchdata=NULL;
};
TCursorWindow::~TCursorWindow(){
    if(table)DBDeAttachWindow(table,HWindow);
    DeleteItems();
    delete items;
    if(searchdata)delete searchdata;
};
int  TCursorWindow::IsCommandDisable(int command){
    switch(command){
        case CM_SEARCHAGAIN:if(searchdata && searchdata->scCanAgain)return 0;else return DM_CANAGAIN;
        case CM_PROPERTIES:
        case CM_PACK:
        case CM_FILESAVEAS:
        case CM_SEARCH:
        case CM_SORTAZ1:
        case CM_SORTZA1:
        case CM_SORTZA:
        case CM_SORTAZ:
        case CM_DELRECORD:DBGetCount(table)?0:200;
        case CM_APPEND:{
            CELLINFO ci;
            SendMessage(browser,TB_GETACTIVE,0,(LPARAM)&ci);
            return (ci.y>-1)?0:200;
        }
    }
    return TWindow::IsCommandDisable(command);
};

void TCursorWindow::EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode){
    switch(command){
        case CM_FILESAVEAS:if(table)SaveTableDialog(HWindow,table);break;
        case CM_PROPERTIES:if(table)TableDialog(HWindow,table);break;
        case CM_SEARCH:if(table){
                if(!searchdata){searchdata=new SEARCH_in_CURSOR;
                    memset(searchdata,0,sizeof(SEARCH_in_CURSOR));
                    searchdata->scCurrent=searchdata->scFromStart=searchdata->scDirection=TRUE;
                }
                if(TableSearchDialog(HWindow,searchdata)){
                    if(searchdata->scFromStart){
                        if(searchdata->scDirection)
                        {
                            CELLINFO ci;
                            ci.x=ci.y=0;
                            SendMessage(browser,TB_GETACTIVE,0,(LPARAM)&ci);
                            DBGoTop(table);
                        }else DBGoBottom(table);
                        searchdata->scCanAgain;
                    }
                }else return;
            };// no break;
        case CM_SEARCHAGAIN:if(table && searchdata){
                int fstart,fcount;
                CELLINFO ci;
                SendMessage(browser,TB_GETACTIVE,0,(LPARAM)&ci);

                if(searchdata->scCurrent){
                    fcount=1;
                    fstart=ci.x;
                }else{
                    fstart=0;
                    fcount=items->count;
                }
                char s[256];
                char key[256];
                lstrcpy(key,searchdata->scKey);
                if(!searchdata->scCase)
#ifdef WIN32
                    CharUpper(key);
#else
                    AnsiUpper(key);
#endif
                int mode=0;
                int i;
                if(searchdata->scCanAgain)goto m1;
                do{
                    for(i=0;i<fcount;i++){
                        if(DBGetField(table,((TBrFiled*)items->At(i+fstart))->num,s)){
                            if(!searchdata->scCase)
#ifdef WIN32
                                CharUpper(s);
#else
                                AnsiUpper(s);
#endif
                            BOOL ok=FALSE;
                            switch(searchdata->scFragment){
                                case 0:ok=!lstrcmp(s,key);break;
                                case 1:ok=(s==strstr(s,key));break;
                                case 2:{char *ps=strstr(s,key);
                                    if(ps && (ps-s)==lstrlen(ps))ok=TRUE;
                                }break;
                                case 3:ok=(NULL!=strstr(s,key));break;

                            }
                            if(ok){
                                searchdata->scCanAgain=TRUE;
                                ci.x=i+fstart;
                                SendMessage(browser,TB_SETACTIVE,0,(LPARAM)&ci);
                                return;
                            } } }

m1:
                    if(searchdata->scDirection)ci.y++;else ci.y--;
                }while(DBSkip(table,searchdata->scDirection?1:-1));
                MessageBox(HWindow,"Specified data not found","Information",MB_OK|MB_ICONINFORMATION);
            }break;
        case CM_DELRECORD:DBDeleteRecord(table,FALSE);break;
        case CM_APPEND:DBAppendRecord(table);break;
        case CM_SORTAZ:
        case CM_SORTZA:
        case CM_SORTAZ1:
        case CM_SORTZA1:if(table){
                int id;
                if((command==CM_SORTAZ1)||(command==CM_SORTZA1))id=lastfield;else{
                    CELLINFO ci;
                    SendMessage(browser,TB_GETACTIVE,0,(LPARAM)&ci);
                    id=ci.x;
                }
                if(id>=0 && id<items->count){
                    TBrFiled *f=(TBrFiled *)items->At((INT16)(id));
                    char s[256];
                    char **si=(char**)new UINT32[1];
                    si[0]=s;
                    wsprintf(s,"#%d",f->num);
                    if((command==CM_SORTAZ1)||(command==CM_SORTAZ))lstrcat(s,"/d");else lstrcat(s,"/a");
                    DBSort(table,0,1,si);
                    delete si;
                }
            }
    }
};

void TCursorWindow::DeleteItems(){
    for(int i=0;i<items->count;i++){
        TBrFiled*f=(TBrFiled*)items->At(i);
        delete f;
    }
    items->DeleteAll();

    SendMessage(browser,TB_RESETCOLLUMNS,0,0);
}
void TCursorWindow::EvSize(WORD,int sx,int sy){
    int dx=50,dy=20;
    SetWindowPos(browser,0,0,0,sx-dx,sy-dy,SWP_NOZORDER|SWP_NOMOVE);
    SetWindowPos(hl,0,0,0,sx-dx,dy,SWP_NOZORDER|SWP_NOMOVE);
    SetWindowPos(vl,0,0,0,dx,sy-dy,SWP_NOZORDER|SWP_NOMOVE);
};
LRESULT TCursorWindow::WndProc(UINT message, WPARAM wParam, LPARAM lParam){
    switch(message){
        case WM_COMMAND:{
            EV_COMMANDsimply
                    switch(command){
        case IDC_INFO: TSTRATUMMENU sm;
                    sm.hWindow=HWindow;
                    sm.hType=SCMENU_TABLE;
                    POINT point;
                    GetCursorPos(&point);
                    TrackPopupMenuR(point,MENU_TABLE,&sm);

                    //TableDialog(HWindow,table);break;
            };
        }break;
        case TBL_LBUTTON:{
            TLABELINFO* info=(TLABELINFO*)lParam;
            lastfield=info->pos;
            POINT pos;
            GetCursorPos(&pos);
            TSTRATUMMENU sm;
            sm.hWindow=HWindow;
            sm.hType=SCMENU_CURSORTITLE;
            mainwindow->TrackPopupMenuR(pos,SCMENU_CURSORTITLE,&sm);

        }return 0;
        case TB_CURSORCHANGE:{
            TCursorChangeInfo *c=(TCursorChangeInfo *)lParam;
            switch(c->code){
                case U_REMOVE:{
                    table=0;
                    if(inmdi){
                        SendMessage(mainwindow->GetMDI(),WM_MDIDESTROY,(WPARAM)GetParent(HWindow),0);
                    }else{
                        // DestroyWindow(HWindow);
                        SetTable(0,0);
                    }
                }break;
                case U_RECORD:{
                    //     InvalidateRect(browser,NULL,0);
                    CELLINFO ci;
                    SendMessage(browser,TB_GETACTIVE,0,(LPARAM)&ci);
                    if(ci.y==c->y){ SendMessage(browser,TB_UPDATECELL,0,(LPARAM)&ci); }
                    ci.y=c->y;
                    SendMessage(browser,TB_GETCELLRECT,0,(LPARAM)&ci);
                    ci.rect.left=0;
                    ci.rect.right=4096;
                    InvalidateRect(browser,&(ci.rect),0);
                };break;
                case U_INSERT:{
                    SendMessage(browser,TB_SETRANGEY,-1,DBGetCount(table));
                    CELLINFO ci;
                    SendMessage(browser,TB_GETACTIVE,0,(LPARAM)&ci);
                    ci.y=c->y;
                    SendMessage(browser,TB_SETACTIVE,0,(LPARAM)&ci);
                }break;
                case U_APPEND:{
                    CELLINFO ci;
                    SendMessage(browser,TB_GETACTIVE,0,(LPARAM)&ci);
                    long _count=SendMessage(browser,TB_GETCOUNT,0,0);
                    long count=DBGetCount(table);
                    SendMessage(browser,TB_SETRANGEY,-1,count);
                    ci.y=_count;
                    SendMessage(browser,TB_SETACTIVE,0,(LPARAM)&ci);
                }break;
                case U_CURPOS:{
                    CELLINFO ci;
                    SendMessage(browser,TB_GETACTIVE,0,(LPARAM)&ci);
                    ci.y=c->y;
                    SendMessage(browser,TB_SETACTIVE,0,(LPARAM)&ci);

                }break;
                case U_DELETE:{
                    CELLINFO ci;
                    SendMessage(browser,TB_GETACTIVE,0,(LPARAM)&ci);
                    ci.y=-1;
                    SendMessage(browser,TB_SETACTIVE,0,(LPARAM)&ci);
                    SendMessage(browser,TB_SETRANGEY,-1,DBGetCount(table));
                };
                default:{
                    SendMessage(browser,TB_SETRANGEY,-1,DBGetCount(table));
                    //  InvalidateRect(HWindow,0,0);
                }
            }
        }return 0;
        case WM_SETFOCUS:SetFocus(browser);return 0;
        case TBN_BEGINDRAW:DBBeginDraw(table);return 0;
        case TBN_ENDRAW:DBEndDraw(table);return 0;
        case TBL_GETTEXT:{
            TLABELINFO* info=(TLABELINFO*)lParam;
            if (info){
                if(info->id==IDC_HLABEL){
                    if (info->pos<items->count){
                        TBrFiled *f=(TBrFiled *)items->At((INT16)(info->pos));
                        lstrcpy(info->text,f->title);
                    }else lstrcpy(info->text,"Error!");
                }
                if(info->id==IDC_VLABEL){
                    wsprintf(info->text,"%ld",info->pos+1);
                }
                return 0;
            }
        }break;
        case TBN_ACTIVECHANGED:{
            CELLINFO ci;
            SendMessage(browser,TB_GETACTIVE,0,(LPARAM)&ci);
            DBSkipTo(table,ci.y);
        }break;
        case TBN_SETTEXT:{
            CELLINFO* ci=(CELLINFO*)lParam;
            if (ci && ci->x<items->count){
                TBrFiled *f=(TBrFiled *)items->At((int)ci->x);
                //  DBGoTop(table);
                DBSkipTo(table,ci->y);
                int res=DBSetField(table,(INT16)f->num,ci->text);
                ci->ok=(res!=0);
                if(!ci->ok){
                    ci->message=FALSE;
                    char s[256];
                    DBGetTextError(DBGetLastError(),s);
                    MessageBox(HWindow,s,"Error",MB_OK|MB_ICONSTOP);
                }
                return TRUE;
            }
        };
        case TBN_GETTEXT:{
            CELLINFO* ci=(CELLINFO*)lParam;
            if (ci && ci->x<items->count){
                TBrFiled *f=(TBrFiled *)items->At((int)ci->x);
                ci->ok=DBDraw(table,ci->y,f->num-1,ci->text,ci->color);
            }
        }return 0;
    }
    return TWindow::WndProc(message,wParam,lParam);
};

void TCursorWindow::SetTable(int t,char *p){

    if(table && t!=table){
        DBDeAttachWindow(table,HWindow);
    }
    table=t;
    if(table)DBAttachWindow(table,HWindow);
    DeleteItems();
    if(table){
        if (p==0 || lstrlen(p)==0){
m1:
            int count = DBFieldCount(table);
            for(int i=0;i<count;i++){
                TBrFiled*f=new TBrFiled();
                f->num=i+1;
                items->Insert(f);
            }
        }else{
            char *ps=p;
            char *_ps=ps;
            if(!strncmpi(ps,"table=",6))
            {
                int _table=atol(ps+6);
                if(IsTableExist(_table))
                {
                    DBGoTop(_table);
                    int index=0;
                    do
                    {
                        char field[256]="";
                        BOOL readonly=FALSE;
                        if(DBGetField(_table,"Field",field))
                        {
                            TBrFiled*f=new TBrFiled();
                            items->Insert(f);
                            f->field=NewStr(field);
                            f->num=DBGetFId(table,field);
                            DBGetField(_table,"Header",field);
                            f->title=NewStr(field);
                            double rez;
                            if(DBGetField(_table,"ReadOnly",&rez))
                            {
                                if(rez)f->readonly=1;
                            }
                            if(DBGetField(_table,"Width",&rez))
                            {
                                if(rez)f->w=rez;
                            }
                            index++;
                        }else break;
                    }while (DBSkip(_table,1));

                }else goto m1;

            }else{
                while(TRUE){
                    if (*ps==',' || !(*ps)){
                        char s[256];
                        lstrcpyn(s,_ps,(int)(ps-_ps)+1);
                        TBrFiled*f=new TBrFiled(s);
                        if (f->field && ((f->num=DBGetFId(table,f->field))>0)){
                            items->Insert(f);
                        }else delete f;
                        _ps=ps+1;
                    }
                    if (*ps)ps++;else break;
                }} }

        for(int i=0;i<items->count;i++){
            TBrFiled*f=(TBrFiled*)items->At(i);
            if (f->w<0)f->w=GetFieldLen(f->num);
            if (!f->title){
                char t[256];
                DBFieldName(table,(INT16)(f->num),t);
                f->title=NewStr(t);
            }
            int flags=CF_EDIT;
            if(DBIsFieldReadOnly(table,f->num)||(f->readonly))flags=CF_READONLY;
            TCollumn c=TCollumn(0,f->w*3,flags);
            c.current=f->w;
            SendMessage(browser,TB_ADDCOLLUMN,-1,LPARAM(&(c)));
        };
    }
    SendMessage(browser,TB_SETRANGEY,-1,DBGetCount(table));
    SendMessage(browser,TB_SETLABEL,(WPARAM)hl,(WPARAM)vl);
    InvalidateRect(HWindow,0,0);
};

int TCursorWindow::GetFieldLen(int id){
    int m= DBGetFLen(table,(INT16)id);
    if(m>400)m=400;
    return m;
};
#endif //dbe

HBITMAP  h_maks_bitmap=0; // Ѕитмара дл¤ кисти

void CreateBrushes(){
    Bface=CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    Blight=CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));
    Bdark=CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
    h_maks_bitmap=LoadBitmap(HResource,MAKEINTRESOURCE(BM_MASK));
    BGrayed=CreatePatternBrush(h_maks_bitmap);
    char s[32];
    LoadString(HResource,IDS_str120,s,sizeof(s));

    HFont = CreateFont(14, 0, 0, 0, 0, /* specify pt size    */
                       0, 0, 0, SCcharset, 0, 0, 0, 0,s );

};// создает кисти

void DoneBrushes(){
    DeleteObject(Blight);
    DeleteObject(Bdark);
    DeleteObject(Bface);
    DeleteObject(BGrayed);
    DeleteObject(h_maks_bitmap);
    DeleteObject(HFont);
};

HBRUSH Blight=0,Bdark=0,Bface=0,BGrayed=0;
HFONT HFont=0;
//*----------------------------------------------------------*
// ѕомещает все объекты из пространства в одну группу
HOBJ2D MakeSingleGroup(HSP2D hsp, BOOL* created)
{
    if (created)
        *created = 0;
    TCollection items(100,50);
    HOBJ2D obj=0;

    while ((obj=GetNextObject2d(hsp,obj)) != (HOBJ2D)0)
    {
        if (!GetObjectParent2d(hsp,obj))
            items.Insert((void*)obj);
    }
    switch(items.count)
    {
        case 0:
            return 0;

        case 1:
            return (HOBJ2D)(items.At(0));

        default:
        {
            HOBJ2D* _items = new HOBJ2D[items.count];
            for(INT16 i=0;i<items.count;i++)
                _items[i]=(HOBJ2D)(items.At(i));

            HOBJ2D ret = CreateGroup2d(hsp, _items, items.count);

            if (created)
                *created = TRUE;

            delete _items;

            return ret;
        }
    }
}

void DeleteAllUnusedTool2d(HSP2D hsp){
    // ¬ажен пор¤док !! (»з за иерархии инструментов)
    DeleteUnusedTools2d(hsp,TEXT2D);
    DeleteUnusedTools2d(hsp,FONT2D);
    DeleteUnusedTools2d(hsp,STRING2D);
    DeleteUnusedTools2d(hsp,PEN2D);
    DeleteUnusedTools2d(hsp,BRUSH2D);
    DeleteUnusedTools2d(hsp,DIB2D);
    DeleteUnusedTools2d(hsp,DOUBLEDIB2D);
    DeleteUnusedTools2d(hsp,SPACE3D);
    //
}

double GetScaleSpace2d(HSP2D hsp){
    POINT2D _m,_d;
    if(GetScaleSpace2d(hsp,&_m,&_d))  return _m.x/_d.x;
    return 1.0;
};


//---------------
BOOL LoadHook(){
    if(!HHook){
#ifdef WIN32
        HHook=LoadLibrary2d("whook_32.dll");
#else
        HHook=LoadLibrary2d("whook_16.dll");
#endif

        if (HHook){
            (FARPROC)CreateWindowHook  = GetProcAddress(HHook,"CreateWindowHook");
            (FARPROC)ReleaseWindowHook = GetProcAddress(HHook,"ReleaseWindowHook");
            (FARPROC)GetWindowData = GetProcAddress(HHook,"GetWindowData");
            _Message("WinHook loaded");

        }else {
            HHook=NULL;
        }
    }
    return (HHook!=0);
}
TCollection * popuptools=NULL;

extern "C" BOOL _SDKPROC SCDestoyToolPopup(int id){
    if(!popuptools)return FALSE;
    for(C_TYPE i=0;i<popuptools->count;i++){
        TPopupTool*tp =(TPopupTool*)popuptools->At(i);
        if(tp->id==id){
            tp->refcount--;
            if(tp->refcount<1){
                return DestroyWindow(tp->HWindow);
            }
        }
    }
    return FALSE;
};

extern "C" BOOL _SDKPROC SCToolPopupVar(int id,int vn,BOOL set,void*d){
    if(!popuptools)return FALSE;
    for(C_TYPE i=0;i<popuptools->count;i++){
        TPopupTool*tp =(TPopupTool*)popuptools->At(i);
        if((tp->id==id) || (id==0)){
            SendMessage(tp->tool,set?WM_SETTOOLVAR:WM_GETTOOLVAR,vn,(LPARAM)d);
        }
    }
    return FALSE;
}
BOOL TPopupTool::GetPosVars(char*prefix,char*sx,char*sy){
    if(prefix && lstrlen(prefix)){
        wsprintf(sx,"tp_%s_x",prefix);
        wsprintf(sy,"tp_%s_y",prefix);
        return TRUE;
    }
    return FALSE;
};
extern "C" HWND _SDKPROC SCCreateToolPopup(TSTRATUMTOOLBAR*tp){
    if(popuptools){
        for(C_TYPE i=0;i<popuptools->count;i++){
            TPopupTool*_tp =(TPopupTool*)popuptools->At(i);
            if(_tp->id==tp->id){
                _tp->refcount++;
                return _tp->HWindow;
            }}
    };
    HWND wnd=0;
    if(!(tp->flags&16)){
        wnd=CreateWindowEx(
        #ifdef WIN32
                WS_EX_TOOLWINDOW
        #else
                0
        #endif
                ,"SC_TOOL_POPUP",tp->title,WS_POPUP |WS_THICKFRAME|WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU,
                tp->x,tp->y,tp->cx,tp->cy,mainwindow->HWindow,0,(HINSTANCE)hInstance,NULL);

        TPopupTool*t=(TPopupTool*)GetWindowLong(wnd,0);

        t->name  = NewStr(tp->title);
        t->style = tp->flags;
        t->id    = tp->id;
        lstrcpy(t->prefix,tp->prefix);
        if(tp->buttons){
            HWND hwnd=SCCreatePropertySheet(wnd,"",tp->buttons,0,tp->hTemplate,tp->sTemplate);
            t->SetChild(hwnd,1);
        }
        char sx[64],sy[64];
        if(TPopupTool::GetPosVars(tp->prefix,sx,sy)){
            if(tp->x==CW_USEDEFAULT)tp->x=SCGetScVar(sx,-1024);
            if(tp->y==CW_USEDEFAULT)tp->y=SCGetScVar(sy,-1024);
        }
        RECT tr,r,dr;
        GetWindowRect(mainwindow->HWindow,&r);
        GetWindowRect(wnd,&tr);GetWindowRect(GetDesktopWindow(),&dr);

        tr.right  -= tr.left;
        tr.bottom -= tr.top;

        if(tp->x==-1024){
            tp->x=r.right-tr.right;
        }
        if(tp->y==-1024){
            tp->y=r.bottom-tr.bottom;
        }
        if((tp->x+tr.right)  > dr.right  )tp->x=dr.right-tr.right;
        if((tp->y+tr.bottom) > dr.bottom )tp->y=dr.bottom-tr.bottom;
        SetWindowPos(wnd,0,tp->x,tp->y,0,0,SWP_NOZORDER|SWP_NOSIZE);
        if(tp->flags&1){
            ShowWindow(wnd,SW_SHOW);
        }}
    if(tp->flags&2){
        HMENU hm=GetSubMenu(mainwindow->menu,2);
        //CheckMenuItem(hm,id,MF_BYCOMMAND|MF_UNCHECKED);
        InsertMenu(hm,GetMenuItemCount(hm)-2,MF_BYPOSITION|MF_STRING|MF_CHECKED,tp->id,tp->title);
    }
    return wnd;

};

TPopupTool::TPopupTool(LPCREATESTRUCT l,HWND h):TWindow(l,h){
    name=NULL;
    style=0;
    id=0;
    refcount=1;
    size.x=l->cx;
    size.y=l->cy;
    prefix[0]=0;
    if(!popuptools)popuptools=new TCollection(2,2);
    popuptools->Insert(this);
};
TPopupTool::~TPopupTool(){
    if(popuptools){
        C_TYPE index=popuptools->IndexOf(this);
        if(index>-1)popuptools->AtDelete(index);
        if(!(popuptools->count)){delete popuptools;popuptools=NULL;}
    }
    if(name)delete name;
    if(style&2){
        HMENU hm=GetSubMenu(mainwindow->menu,2);
        CheckMenuItem(hm,id,MF_BYCOMMAND|MF_UNCHECKED);
    }
};
void TPopupTool::EvSize(WORD sizeType,int sx,int sy){
    SetWindowPos(tool,0,0,0,sx,sy,SWP_SHOWWINDOW|SWP_NOZORDER|SWP_NOMOVE);
    if(style&4) size.x=sx;
    if(style&8) size.y=sy;
};
void TPopupTool::SetChild(HWND h,int f){
    tool=h;
    if(f&1){
        RECT cr;
        GetClientRect(h,&cr);
        int x=cr.right,y=cr.bottom;
        GetWinSizeByClient(x,y);
        size.x=x;
        size.y=y;
        SetWindowPos(HWindow,0,0,0,x,y,SWP_NOZORDER|SWP_NOMOVE);
    }
    RECT r;
    GetClientRect(HWindow,&r);
    SetWindowPos(tool,0,0,0,r.right,r.bottom,((f&2)?SWP_SHOWWINDOW:0)|SWP_NOZORDER);
};
LRESULT TPopupTool::WndProc(UINT message, WPARAM wParam, LPARAM lParam){
    switch(message){
        case WM_MOVE:{
            int xPos = (int) LOWORD(lParam);    // horizontal position
            int yPos = (int) HIWORD(lParam);    // vertical position
            char sx[64],sy[64];
            if(TPopupTool::GetPosVars(prefix,sx,sy)){
                SCSetScVarI(sx,xPos);
                SCSetScVarI(sy,yPos);
            }
        }break;
        case WM_GETMINMAXINFO:{
            MINMAXINFO FAR* lpmmi = (MINMAXINFO FAR*) lParam;
            if(!(style&4))	{
                lpmmi->ptMaxSize.x=lpmmi->ptMaxTrackSize.x=lpmmi->ptMinTrackSize.x=size.x;
            }
            if(!(style&8)){
                lpmmi->ptMaxSize.y=lpmmi->ptMaxTrackSize.y=lpmmi->ptMinTrackSize.y=size.y;
            }
            return 0;
        }break;

        case WM_MOUSEACTIVATE:{
            SendMessage(mainwindow->HWindow,message,wParam,lParam);
            HWND p=GetLastActivePopup(mainwindow->HWindow);
            if(p && p!=HWindow){
                TWindow*w=(TWindow*)SendMessage(p,scwnd_message,0,0);
                if(!w){
                    return MA_NOACTIVATEANDEAT;
                } }
        } break;

    };
    return TWindow::WndProc(message,wParam,lParam);
};

// Property Button
/// TPropertyButton
char * NewIntStr(char*_title){
    char *title;
    if(HIWORD(_title)){
        title=NewStr(_title);
    }else {
        char s[256];
        if(!LoadString(HResource,LOWORD(_title),s,sizeof(s)))lstrcpy(s,"**");
        title=NewStr(s);
    }
};
void LoadStr(int i,char*s){
    if(!LoadString(HResource,i,s,256))s="**";
};
TPropertyButton::TPropertyButton(char*_title,HINSTANCE h,LPCSTR _templ,int hc){
    memset(this,0,sizeof(TPropertyButton));
    title=NewIntStr(_title);
    hInstance=h;
    flags=PSP_DATAVALID;
    ((LPCSTR)templ)=_templ;
    Procedure=NULL;
    helpctx=hc;
};
TPropertyButton::TPropertyButton(int _title,HINSTANCE h,LPCSTR _templ,int hc){
    memset(this,0,sizeof(TPropertyButton));
    title=NewIntStr((char*)_title);
    hInstance=h;
    flags=PSP_DATAVALID;
    ((LPCSTR)templ)=_templ;
    Procedure=NULL;
    helpctx=hc;
};
void TPropertyButton::Insert(TPropertyButton*_next){
    if (next)next->Insert(_next);else
    {_next->prev=this;
        next=_next;
    }
};
TPropertyButton::~TPropertyButton(){
    if (next){delete next;next=NULL;}
    if (title)delete title;
};

TPropertySheet::TPropertySheet(LPCREATESTRUCT l,HWND h):TWindow(l,h){
    first=0;
    active=0;
    height=21;
    focused=0;
    style=l->style&0x7;
    autodelay=(l->style&PSS_DESTROYPAGES)!=0;
    lines=0;
    items=NULL;
    RECT r;
    GetClientRect(HWindow,&r);
    sizex=r.right; sizey=r.bottom;
};
TPropertySheet::~TPropertySheet(){
    if(items){
        for(C_TYPE i=0;i<items->count;i++){
            TCollection*c=(TCollection*)items->At(i);
            delete c;
        }
        delete items;
    }
    if (autodelay && first)delete first;
};
void TPropertySheet::SetActive(TPropertyButton*na){
    SendMessage(GetParent(HWindow),PSN_PAGESELECT,0,(LPARAM)na);
};
void TPropertySheet::EvLButtonDown(WORD,int xPos,int yPos){
    if(style!=PSS_NORMAL)return;
    TPropertyButton*pb=first;
    POINT point;point.x=xPos;point.y=yPos;
    //  int id=0;
    while(pb){
        if (PtInRect(&(pb->rect),point)){
            SetFocus(HWindow);
            if (pb!=active){
                SetActive(pb);
                return;
            }
        }
        pb=pb->next;//id++;
    };
};
LRESULT TPropertySheet::WndProc(UINT message, WPARAM wParam,LPARAM lParam){
    switch(message){
        case PST_GETACTIVEPAGE:return (LRESULT)active;
        case PST_GETALLPAGES:return (LRESULT)first;
        case PST_SETACTIVEPAGE:{active=(TPropertyButton*)lParam;
            if(items && items->count>1){
                for(C_TYPE i=0;i<items->count;i++){
                    TCollection*c=(TCollection*)items->At(i);
                    for(C_TYPE j=0;j<c->count;j++){
                        TPropertyButton*p=(TPropertyButton*)c->At(j);
                        if(p==active){
                            if(i!=items->count-1){
                                items->Swap(i,items->count-1);
                            }
                            goto m1;
                        }}}
m1:
                for(i=0;i<items->count;i++){
                    TCollection*c=(TCollection*)items->At(i);
                    for(C_TYPE j=0;j<c->count;j++){
                        TPropertyButton*p=(TPropertyButton*)c->At(j);
                        p->rect.top    =  i*21;
                        p->rect.bottom =p->rect.top+21;
                        p->line=i;
                    }
                }
            }
            switch(style){
                case PSS_NORMAL:Repaint();break;
                case PSS_COMBOLIST:{
                    TPropertyButton*p=active;
                    int i=0;
                    while(p->prev){p=p->prev;i++;}
                    SendDlgItemMessage(GetParent(HWindow),IDC_COMBOPAGES,CB_SETCURSEL,i,0);
                }break;
            }
            return 0;
        }
        case WM_SETFOCUS:focused=1;if(style==PSS_NORMAL)Repaint();return 0;
        case WM_KILLFOCUS:focused=0;if(style==PSS_NORMAL)Repaint();return 0;
        case WM_GETDLGCODE:return DLGC_WANTARROWS;
        case WM_KEYDOWN:{
            switch(wParam){
                case VK_LEFT:if (active->prev)SetActive(active->prev);else{
                        TPropertyButton*pb=first;
                        while(pb->next)pb=pb->next;
                        SetActive(pb);
                    }
                    return 0;
                case VK_RIGHT:{if (active->next)SetActive(active->next);
                    else SetActive(first);return 0;
                }
            }
        case VK_ESCAPE:
            case VK_RETURN:SendMessage(GetParent(HWindow),message,wParam,lParam);break;
            }break;
        case WM_SIZE:{
            RECT r;
            GetClientRect(HWindow,&r);
            sizex=r.right;
            sizey=r.bottom;
        }return 0;
        case WM_PAINT:{
            HDC hdc;
            PAINTSTRUCT ps;
            hdc=BeginPaint(HWindow,&ps);
            Paint(hdc,(ps.rcPaint));
            EndPaint(HWindow,&ps);
        }return 0;
    }
    return TWindow::WndProc(message,wParam,lParam);
};
void TPropertySheet::SetButtons(TPropertyButton*b,int _sizex){
    active=first=b;
    if(style==PSS_NORMAL){
        if(!items)items=new TCollection(10,10);
        HDC hdc=GetDC(HWindow);
        SelectObject(hdc,HFont);
        TPropertyButton*pb=first;
        int x=2;
        TCollection*_items=new TCollection(4,2);
        items->Insert(_items);
        while(pb){
            pb->rect.left=x;
            SIZE tsize;
            GetTextExtentPoint(hdc,pb->title,lstrlen(pb->title),&tsize);
            pb->sizex=tsize.cx;
            if(x+tsize.cx>(_sizex-16)){    //-(4+16)
                _items=new TCollection(4,2);
                items->Insert(_items);
                pb->rect.left=x=2;
            };
            _items->Insert(pb);
            x+=tsize.cx+16;
            pb->rect.right=x;
            if(!pb->next && items->count>1){
                pb->rect.right=_sizex-25;
            }
            pb->rect.top=2;
            pb->rect.bottom=height;
            pb=pb->next;
        };
        lines=items->count;
        height=height*lines;
        ReleaseDC(HWindow,hdc);
    }
    if(style==PSS_COMBOLIST){
        HWND list=GetDlgItem(GetParent(HWindow),IDC_COMBOPAGES);
        TPropertyButton*pb=first;
        while(pb){
            SendMessage(list,CB_ADDSTRING,0,(LPARAM)(pb->title));
            pb=pb->next;
        }
        SendMessage(list,CB_SETCURSEL,0,0);
        lines = height = 0;
    }
};
void TPropertySheet::Repaint(){
    HDC hdc=GetDC(HWindow);
    HGDIOBJ oldbrush=SelectObject(hdc,Bface);
    PatBlt(hdc,0,0,sizex-1,height+1,PATCOPY);
    SelectObject(hdc,oldbrush);
    PaintButtons(hdc,NULL);
    ReleaseDC(HWindow,hdc);
};
void TPropertySheet::Paint(HDC hdc,RECT&r){
    if(style==PSS_NORMAL){
        PaintButtons(hdc,&r);
        SelectObject(hdc,Blight);
        PatBlt(hdc,0,height,1,sizey-height,PATCOPY);
        PatBlt(hdc,0,sizey-1,sizex,1,BLACKNESS);
        PatBlt(hdc,sizex-1,height,1,sizey,BLACKNESS);

        //HBRUSH dark=CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
        //	HGDIOBJ oldbrush=SelectObject(hdc,dark);
        SelectObject(hdc,Bdark);

        PatBlt(hdc,1,sizey-2,sizex-2,1,PATCOPY);
        PatBlt(hdc,sizex-2,height+1,1,sizey-2,PATCOPY);
    }else{
        DrawShadowFrame(hdc,0,0,sizex,sizey,0);
    }


    //	SelectObject(hdc,oldbrush);
    //	DeleteObject(dark);
};
void TPropertySheet::PaintButtons(HDC hdc,RECT*){
    if (first==NULL || active==NULL )return;
    HPEN wight= CreatePen(PS_SOLID	,0,GetSysColor(COLOR_BTNHIGHLIGHT));
    HPEN dark = CreatePen(PS_SOLID	,0,GetSysColor(COLOR_BTNSHADOW));
    HPEN black= CreatePen(PS_SOLID	,0,0);
    HGDIOBJ old=SelectObject(hdc,wight);
    for(C_TYPE i=0;i<items->count;i++){

        TCollection*row=(TCollection*)items->At(i);
        int x=active->rect.left-2;
        MoveToEx(hdc,0,height,NULL);

        if(i==items->count-1){
            SelectObject(hdc,wight);
            if (x){ LineTo(hdc,x,height); }//________
            LineTo(hdc,x,active->rect.top);               // |
            LineTo(hdc,active->rect.left,active->rect.top-2); //  /
            LineTo(hdc,active->rect.right,active->rect.top-2);  //-----------

            SelectObject(hdc,black);
            LineTo(hdc,active->rect.right+2,active->rect.top);
            LineTo(hdc,active->rect.right+2,height);
            /*  \ *//*  | */

            SelectObject(hdc,wight);

            LineTo(hdc,sizex,height); //____________________
            MoveToEx(hdc,active->rect.right+1,active->rect.top,NULL);
            LineTo(hdc,active->rect.right+1,height);
            SelectObject(hdc,dark);
            MoveToEx(hdc,active->rect.right+1,active->rect.top,NULL);
            LineTo(hdc,active->rect.right+1,height);
        }
        TPropertyButton*pb;
        if(i){

            HGDIOBJ oldbrush=SelectObject(hdc,Bface);
            for(C_TYPE j=0;j<row->count;j++){
                pb=(TPropertyButton*)row->At(j);
                int x=pb->rect.left;
                int sx=pb->rect.right-pb->rect.left;

                if(pb->next==active && pb->line==active->line)sx-=2;
                if(pb->prev==active && pb->line==active->line){x+=3;sx-=3;}

                PatBlt(hdc,x,pb->rect.top,
                       sx,pb->rect.bottom-pb->rect.top,PATCOPY);
                PatBlt(hdc,x+1,pb->rect.top-1,
                       sx-2,1,PATCOPY);
            }
            SelectObject(hdc,oldbrush);

        }

        SelectObject(hdc,wight);

        for(C_TYPE j=0;j<row->count;j++){
            pb=(TPropertyButton*)row->At(j);
            if (pb!=active){
                POINT pp[4];
                pp[0].x=pb->rect.left;pp[0].y=height-1;
                pp[1].x=pb->rect.left;pp[1].y=pb->rect.top+2;
                pp[2].x=pb->rect.left+2;pp[2].y=pb->rect.top;
                pp[3].x=pb->rect.right-2;pp[3].y=pb->rect.top;
                if ((pb->prev==active)&&(pb->line==active->line)){
                    pp[2].x+=2;
                    Polyline(hdc,pp+2,2);}else
                    Polyline(hdc,pp,4);
            }
        }

        for(j=0;j<row->count;j++){
            pb=(TPropertyButton*)row->At(j);

            if (pb!=active && (pb->next==NULL || pb->line!=active->line || pb->next!=active) ){

                SelectObject(hdc,black);
                POINT pp[4];
                pp[0].x=pb->rect.right-2;pp[0].y=pb->rect.top+1;
                pp[1].x=pb->rect.right-1;pp[1].y=pb->rect.top+2;
                pp[2].x=pp[1].x;pp[2].y=height;
                Polyline(hdc,pp,3);
                SelectObject(hdc,dark);
                pp[1].x--;
                pp[2].x--;
                Polyline(hdc,pp+1,2);
            }
        };

        SelectObject(hdc,black);
        SetTextAlign(hdc,TA_CENTER |TA_TOP);
        SetBkMode(hdc,TRANSPARENT);
        SelectObject(hdc,HFont);
        SetTextColor(hdc,GetSysColor(COLOR_BTNTEXT));
        for(j=0;j<row->count;j++){
            pb=(TPropertyButton*)row->At(j);
            int y=pb->rect.top+3;
            if (pb==active){
                y-=2;
                TextOut(hdc,-1+(pb->rect.right+pb->rect.left)/2,y,pb->title,lstrlen(pb->title));

            }
            TextOut(hdc,(pb->rect.right+pb->rect.left)/2,y,	pb->title,lstrlen(pb->title));
            if (pb==active && focused){
                SIZE tsize;
                GetTextExtentPoint(hdc,pb->title,lstrlen(pb->title),&tsize);
                int _x=tsize.cx;
                int _y=tsize.cy;
                RECT fr;
                fr.left=(((pb->rect.right+pb->rect.left)/2)-_x/2)-5;
                fr.right=(((pb->rect.right+pb->rect.left)/2)+_x/2)+5;
                fr.top=y;
                fr.bottom=y+_y+2;
                DrawFocusRect(hdc,&fr);
            }
        }
    }
    SelectObject(hdc,old);
    DeleteObject(black);
    DeleteObject(dark);
    DeleteObject(wight);
};


