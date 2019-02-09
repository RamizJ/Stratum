#undef STRATUM_INTERNAL

#include "../BC/bc_include_gwin2d3d/GWINFUNC.H"
#include "../BC/bc_include_stratum/STRATUM.H"
#include "../BC/bc_include_stratum/SC_MSG.H"
#include "MESSAGES.H"
#include "dos.h"
#include "../BC/bc_include_stratum/SCCALL.H"

//#include <gwin2d3d\gwinfunc.h>
//#include <stratum\stratum.h>
//#include <stratum\sc_msg.h>
//#include "messages.h"
//#include <dos.h>
//#include <stratum\sccall.h>
#ifndef WIN32
extern "C"
{
HINSTANCE SDKPROC ldprocedure(char*path,int code);
};
#else
#include "loadcore.cpp"
#endif

HDC tdc=0;
INT16 bmpw,bmph;
HGDIOBJ tbrush=0;

void WriteTitleStr(char *s)
{
    PatBlt(tdc,1,bmph-15,bmpw-2,14,PATCOPY);
    TextOut(tdc,bmpw/2,bmph-15,s,lstrlen(s));
};

HWND InitTitle(HINSTANCE hInstance)
{
    HDC _dc=GetDC(0);
    char * _tn;
    if (GetDeviceCaps(_dc,BITSPIXEL)>8)
        _tn="TITLE256";
    else
        _tn="TITLE16";
    ReleaseDC(0,_dc);
    HBITMAP _title=LoadBitmap(hInstance,_tn);
    BITMAP bm;
    if(GetObject(_title,sizeof(bm),&bm))
    {
        bmpw=bm.bmWidth; bmph=bm.bmHeight;
        int wh=bmph;
        RECT r;
        GetClientRect(GetDesktopWindow(),&r);
        HWND title=CreateWindowEx(WS_EX_TOPMOST,"STATIC","",WS_POPUP,
                                  0,0,bm.bmWidth,wh,NULL,NULL,(HINSTANCE)hInstance,NULL);
        SetWindowPos(title,HWND_TOP,(r.right-bm.bmWidth)/2,(r.bottom-wh)/2,bm.bmWidth,wh,
                     SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW	);
        tdc=GetDC(title);
        HDC dc=CreateCompatibleDC(tdc);
        SelectObject(dc,_title);
        BitBlt(tdc,0,0,bm.bmWidth,bm.bmHeight,dc,0,0,SRCCOPY);
        DeleteDC(dc);
        DeleteObject(_title);
        SetTextAlign(tdc,TA_CENTER|TA_TOP);
        SelectObject(tdc,GetStockObject(ANSI_VAR_FONT));
        SelectObject(tdc,GetStockObject(LTGRAY_BRUSH));
        COLORREF rgb=GetPixel(tdc,bmpw/2,bmph-15);
        SetTextColor(tdc,rgb?RGB(0,0,0):RGB(255,255,255));
        SetBkColor(tdc,rgb);
        tbrush=CreateSolidBrush(rgb);
        SelectObject(tdc,tbrush);
        WriteTitleStr("Loading core...");
        return title;
    }
    return 0;
}

void DoneTitle(HWND title)
{
    ReleaseDC(title,tdc);
    DestroyWindow(title);
    DeleteObject(tbrush);
}

UINT32 _SDKPROC ScDllProc(UINT16 msg, UINT32 p1, UINT32 p2)
{
    switch(msg)
    {
        case SC_GETPROCADDRESS:
        {
            char *name=(char *)p1;
            if(!lstrcmpi(name,"WriteTitleStr"))
                return(DWORD)WriteTitleStr;
        }break;
    }
    return NULL;
}

BOOL GetPath(char* file, char* path)
{
    int l=lstrlen(file);
    if (l)
    {
        while(file[l]!='\\'&& l>0)
            l--;

        if (l)
        {
            for(int i=0;i<l;i++)
                path[i]=file[i];

            path[l]=0;
            return TRUE;
        }
    }
    lstrcpy(path,"");
    return FALSE;
};

#include "msgcrk.h"

BOOL RegisterProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    //_STDHELP(Project_creating_Dialog)
    HWND  hwndCtl;
    INT16 wNotifyCode;
    INT16 command;
    switch(msg){
        case WM_INITDIALOG:{};return TRUE;
        case WM_COMMAND:
        {
            EV_COMMANDnoVar
                    switch (command){
        case IDOK:
                {
                    EndDialog(hwnd,TRUE);
                    return TRUE;
                }
                case IDCANCEL:
                    EndDialog(hwnd,0);
                    return TRUE;
            }
        }
            break;
    }
    return FALSE;
};

int WinMain(HINSTANCE hInstance,HINSTANCE,LPSTR,int nCmdShow)
{
    HWND old=FindWindow("STRATUM_FRAME_30",NULL);
    // Ищем уже запущенную версию Stratum'а
    if(old)
    {
#ifdef LOG_ON
        logenable=0;
#endif

#ifdef WIN32
        if(!SendMessage(old,WM_GETSCINFO,1,0))
#endif
        {
            if (IsIconic(old))
                ShowWindow(old,SW_RESTORE);

#ifdef WIN32
            SetForegroundWindow(old);
#endif
            return FALSE;
        }
    }
    /*
    BOOL   (SDKPROC *ScInitCore)(TSTRATUMINFO*)=NULL;
    void   (SDKPROC *ScDoneCore)()=NULL;
    BOOL   (SDKPROC *ScProcessCmdLine)(char **_argv,int _argc)=NULL;
    void   (SDKPROC *ScMessageLoop)()=NULL;
    void   (SDKPROC *ScPostInit)()=NULL;
    void   (SDKPROC *ScVersion)(TSTRATUMVERSION*info)=NULL;
*/
//    UINT32 (SDKPROC *_ScDllProc)(UINT16  msg,UINT32 p1,UINT32 p2)=NULL;

    TSTRATUMINFO info;
    info.size=sizeof(info);
    info.version=MAKELONG(3,1);
    info.hInstance=hInstance;

    //setup title
    HWND title=0;
    BOOL showtitle=1;
    for(int i=1;i<_argc;i++)
        if(!lstrcmpi(_argv[i],"/notitle"))
        {
            showtitle=0;
            break;
        }
    if(showtitle)
        title=InitTitle(hInstance);

    info.hMainWindow=title;
    info.ScProc=ScDllProc;

    char s[256];
    GetPath(_argv[0], s);
    info.path = s;

    //Загрузка score.dll
    HINSTANCE hc = ldprocedure(info.path, 1);
    if(!hc)
    {
        //MessageBox(0,info.path,"awef",0);
        //MessageBox(0,_argv[0],"awef",0);
        DoneTitle(title);
        MessageBox(0,
           #ifdef WIN32
                   "Unable to locate sccore32.dll"
           #else
                   "Unable to locate sccore16.dll"
           #endif
                   ,"Stratum loading ERROR",MB_OK|MB_ICONSTOP);
        return -1;
    }
    //	(FARPROC)ScInitCore       = GetProcAddress(hc,"ScInitCore");
    //	(FARPROC)ScDoneCore       = GetProcAddress(hc,"ScDoneCore");
    //	(FARPROC)ScProcessCmdLine = GetProcAddress(hc,"ScProcessCmdLine");
    //	(FARPROC)ScMessageLoop    = GetProcAddress(hc,"ScMessageLoop");
    //	(FARPROC)ScPostInit       = GetProcAddress(hc,"ScPostInit");
    //	(FARPROC)ScVersion        = GetProcAddress(hc,"SCVersion");
    (FARPROC)_ScDllProc=GetProcAddress(hc,"ScDllProc");

    TSTRATUMVERSION _info;
    _info.size=sizeof(_info);

    if(_ScDllProc)
    {
        if(_ScDllProc(SC_GETVERSION,(UINT32)&_info,0))
        {
            //ScVersion(&_info);
            //определяем версию среды
            char s[20]="";
            if(_info.flags & svfPROF){lstrcpy(s," PROFESSIONAL ");}
            if(_info.flags & svfDEMO){lstrcat(s," DEMO "); }
            if(_info.flags & svfRUNTIME){lstrcat(s," RUNTIME "); }
            if(!(s[0]))
                lstrcpy(s," STANDARD ");

            if(s)
            {
                //Формируем название версии среды в верхнем правом углу окна загрузки
                int l=lstrlen(s);
                SIZE sz;
                HFONT hf=CreateFont(15,0,0,0,700,1,0,0,0, 0,0,0,0,"Arial"  );
                HFONT hf1=(HFONT)SelectObject(tdc,hf);
                GetTextExtentPoint(tdc,s,l,&sz);
                SetTextColor(tdc,RGB(240,240,255));
                SetBkColor(tdc,RGB(0,0,0));
                int m=SetBkMode(tdc,OPAQUE);
                TextOut(tdc,bmpw-(sz.cx/2+5),2,s,l);
                SetBkMode(tdc,m);
                SetTextColor(tdc,RGB(255,255,255));
                SetBkColor(tdc,RGB(0,0,0));
                SelectObject(tdc,hf1);
                DeleteObject(hf);
            }
        }
        //	BOOL load=ScInitCore(&info);
        _ScDllProc(SC_ARGUMENTS, _argc, (UINT32)_argv);
        BOOL load = _ScDllProc(SC_INITLIBRARY,(UINT32)&info,0);
        if(load)
        {
            //		SetStatusProgressBar(1,1000);
            WriteTitleStr("Loading done ...");
            DoneTitle(title);
            title=0;
            if( _ScDllProc(SC_SENDCMD,SC_CMD_USER,(UINT32)&"enteridle"))
                _ScDllProc(SC_DONELIBRARY,0,0);
            //			ScPostInit();
            //			ScMessageLoop();
            //			ScDoneCore();
        }
    }
    ldprocedure(NULL,2);
    return 0;
}
