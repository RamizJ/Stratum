#undef _WINREG_
#define _SC_WINREG_
#include <windows.h>
#include <dir.h>
#include <string.h>
#include <dos.h>
#include <stdlib.h>

#include "class.h"
#include "twindow.h"

#include "../BC/bc_include_stratum/STRATUM.H"
#include <stratum\sc_types.h>
#include <stratum\sc_msg.h>
#include <stratum\sccall.h>
#include <scbuild.h>

#include "sc3.rh"
#include "stream.h"

#include "vmachine.h"
#include "winman.h"
#include "stream_m.h"
#include "tcntbar.h"
#include "math.h"
#include "video\scvideo.h"

#ifdef USEAUDIERE
#include "audio\scaudio.h"
#endif

#include "sc_ctl3.h"
#include "graphic\3dtools.h"
#include "scvars.h"
#include "_message.h"
#include "dialogs.h"

#include "registry.h"
#include "stdio.h"
#include "strings.rh"
#include "ballon.h"

#include "dllman.h"

#ifdef USE_OGRE
#include "OgreWrapperCommands.h"
#include "Root.h"
#pragma comment(lib, "OgreWrapper.lib")
#endif

#include "dynodata.h"

#define ID_TIMER 1

#ifdef PROTECT
#include "password.h"
#endif

#ifdef CHECKHEAP
void CheckFinalMemoryState();
#endif

//#define LOG_ON
BOOL paused=0;
BOOL dialog_active=0;
BOOL taskactive=0;
BYTE sc_interface=0;
UINT ballon_message;
UINT scwnd_message;
UINT hTimer;
int SCcodepage,SCcharset;
#ifdef DBE
int DBInitSQL();
void DoneSQL();
#endif

BYTE registed_copy=0; // хранится зарегистрированная копия.
int peridle=1;
char secondcode[13];
extern char *cnt_tpl;
char **__argv;
int __argc;
TLibrary extern *librarys;

BOOL (WINAPI *  Ctl3dColorChange)(void)=NULL;
BOOL (WINAPI *  Ctl3dAutoSubclass)(HANDLE)=NULL;
BOOL (WINAPI *  Ctl3dRegister)(HANDLE)=NULL;
BOOL (WINAPI *  Ctl3dUnregister)(HANDLE)=NULL;
void AddDirEx(TLibrary*,BOOL chk=TRUE,BYTE=0);

#ifdef WIN32
void RegIcon(char *ext,char*type,char *info,int num,int flags=0)
{
    SetValueData(HKEY_CLASSES_ROOT,ext,"",type);
    SetValueData(HKEY_CLASSES_ROOT,type,"",info);
    char s[256],s2[256];
    wsprintf(s,"%s\\DefaultIcon",type);
    wsprintf(s2,"%s\\sc200032.exe,%d",sc_path,num);
    SetValueData(HKEY_CLASSES_ROOT	,s,"",s2);
    if(flags&1)
    {
        wsprintf(s,"%s\\Shell\\open\\command",type);
        wsprintf(s2,"%s\\sc200032.exe \"%%1\"",sc_path);
        SetValueData(HKEY_CLASSES_ROOT,s,"",s2);
    }
    if(flags&2)
    {
        wsprintf(s,"%s\\Shell\\run",type);
        SetValueData(HKEY_CLASSES_ROOT,s,"","&Run project");
        lstrcat(s,"\\command");
        wsprintf(s2,"%s\\sc200032.exe /run \"%%1\"",sc_path);
        SetValueData(HKEY_CLASSES_ROOT,s,"",s2);
    }
}
#endif // WIN32

BOOL IsRegistered()
{
    return registed_copy;
};

#ifdef PROTECT
BOOL IsRegistered2()
{
    char s[64];
    GetSecondCode2(s);
    if(test1(s))
    {
        char ss[256];
        GetValueData("Register","Name",ss,sizeof(ss));
        GetValueData("Register","EMail",ss+lstrlen(ss),sizeof(ss)-lstrlen(ss));
        return test4(s,ss);
    }
};
#endif

void RegisterIcons()
{
#ifdef WIN32
    RegIcon(".spj","StratumProjectFile","Stratum Project File",5,3);
    RegIcon(".vdr","StratumGraphicFile","Stratum 2d graphics File",7,1);
    RegIcon(".cls","StratumClassFile","Stratum Class File",3);
    RegIcon(".stt","StratumVarFile","Stratum variables file",6);
    RegIcon(".mat","StratumMatrixFile","Stratum matrix file",4);
#endif // WIN32
};

void scCALLBACK TimerProc(HWND hwnd, UINT msg, UINT idTimer, DWORD dwTime)
{
    static BYTE alredy_enter_this_proc = 0;
    if(alredy_enter_this_proc)
    {
        if(hTimer)
        {
            KillTimer(mainwindow->HWindow, ID_TIMER);
            hTimer = 0;
        }
    }
    else
    {
        alredy_enter_this_proc = 1;
        for(int i = 0; i < peridle; i++)
        {
            if (EXEC_FLAGS & EF_RUNNING)
                DoOneStep();
            else
                Stop();
        }

        SetLastSteps();
        alredy_enter_this_proc = 0;
        if(!hTimer)
        {
            if (EXEC_FLAGS & EF_RUNNING)
            {
                int mode = SCGetScVar("runtimer", 20); //определяем интервал одного цикла исполнения
                hTimer = SetTimer(mainwindow->HWindow, ID_TIMER, mode, TimerProc);
            }
        }
    }
}

void Run()
{
    EXEC_FLAGS|=EF_RUNNING;
    SCSetStatusText(0,"Running");
    int mode=SCGetScVar("run_mode",0);
#ifdef LOGON
    LogMessage("Зашел в Run");
#endif
    if(mode==0)
    {
        if(hTimer)
        {
            EXEC_FLAGS&=~EF_BYTIMER;
            KillTimer(mainwindow->HWindow,ID_TIMER);
            hTimer=0;
        }
    }
    if(mode==1)
    {
#ifdef LOGON
        LogMessage("  mode==1");
#endif
        EXEC_FLAGS |= EF_BYTIMER;
        mode=SCGetScVar("runtimer",20);	//определяем интервал одного цикла исполнения
        hTimer = SetTimer(mainwindow->HWindow,ID_TIMER,mode,TimerProc);
    }
    GetLastScError();
    if (mainwindow)
        PressControlButton(CM_RUN,1);
}

void Stop()
{
#ifdef LOGON
    LogMessage("Зашел в Stop");
#endif

    EXEC_FLAGS &= ~EF_RUNNING | EF_BYTIMER;

#ifdef LOGON
    LogMessage("SCSetStatusText");
#endif

    SCSetStatusText(0,"Paused");

#ifdef LOGON
    LogMessage("mainwindow");
#endif

    if(mainwindow)
        PressControlButton(CM_PAUSE, 1);

#ifdef LOGON
    LogMessage("releasecapture");
#endif

    winmanager->releasecapture();

#ifdef LOGON
    LogMessage("hTimer");
#endif
    if(hTimer)// && mainwindow && mainwindow->HWindow)
    {
#ifdef LOGON
        LogMessage("KillTimer");
#endif
        KillTimer(mainwindow->HWindow,ID_TIMER);
        hTimer = 0;
    }

#ifdef LOGON
    LogMessage("ShowCursor");
#endif
    while(ShowCursor(1) < 0);

#ifdef USEAUDIERE
#ifdef LOGON
    LogMessage("FreeAudio");
#endif
    FreeAudio();
#endif

#ifdef USE_OGRE
#ifdef LOGON
    LogMessage("Root_Destroy");
#endif
    Root_Destroy();
#endif

#ifdef DBE
    DoneSQL();
#endif

#ifdef LOGON
    LogMessage("Stop Finished");
#endif
};

int InitCompiler();
int DoneCompiler();
void InitMatrixs();
void DoneMatrixs();
int VideoLoad();
char * ini_file=NULL;
BOOL playing;
HACCEL akeys;
HWND hmain;
HINSTANCE hInstance;
HWND title;
HINSTANCE ctl3d=0;
HINSTANCE HResource=0;

char libpath[128];
TWindowManager *winmanager;

void _GetBallonData()
{
    POINT p;
    GetCursorPos(&p);
    HWND hw=WindowFromPoint(p);
    if (hw)
    {
        char s[256]="";
        BALLONSTRUCT bs;
        bs.bsPoint=p;
        bs.bsSize=MAX_BALLONSTR;
        bs.bsText=s;
        ScreenToClient(hw,&(bs.bsPoint));
        if(SendMessage(hw,ballon_message,0,(LPARAM)&bs) && s[0])
            SetBallonData(p.x,p.y,s);
        else
            SetBallonData(p.x,p.y,0);
    }
};

void SetPerfomance();

void IdleAction()
{
    BOOL wait=TRUE;

    // Video HANDLE
    if(paused)
    {
        ResumeAll();
        paused = 0;
    }
    if(playing)
    {
        VideoHandle();
        wait = FALSE;
    }
    // Video HANDLE

    if( (!(EXEC_FLAGS & EF_BYTIMER)) && Running() )
    {
        for(int i = 0; i < peridle; i++)
            if(EXEC_FLAGS & EF_RUNNING)
            {
                DoOneStep();
            }
        wait = FALSE;
        SetLastSteps();
    }
    int static refresh = 0;
    if ((wait) || (refresh > 5))
    {
        ControlIdle();
        SetPerfomance();

        if(taskactive)
            _GetBallonData();
        refresh=0;
    }

    if (wait)
        ::WaitMessage();
    else
        refresh++;
}

#ifdef PROTECT
BOOL IsCodeValid()
{
    return test1(secondcode) && test2(secondcode);
};

void GetSecondCode()
{
    char s[256];
    char s1[256];
    char *ps="\t\x17)\x1D\a7y\rззе\x03э\rпХейэйХнн\aлХэг\x03\x03Хпп\tпг\tеянзэбu7\x1DUoWEUESgEGП\tMgEAQk]Ei\0";  // 1
    // char *ps="4Д5”DЕ·\x14\x43\x43Sd“\x14\x03ТS3“3Т\x13\x13D#Т“cddТ\x03\x034\x03c4Sѓ\x13C“sЧЕ”Ц\aЖVЦVжGVF\x024\x16GVvц'–V7";//2
    // char *ps="CLSID\\{A445F9A0-5393-11D2-96FF-00C06C581497}\\Implemented Categories";
    char *_ps=s1;
    while(*ps)
    {
        *_ps= (((BYTE)*ps)^0x8e)>>1; //1
        _ps++;ps++;
    }
    *_ps=0;
    // GetValueData(HKEY_CLASSES_ROOT,"CLSID\\{A445F9A0-5393-11D2-96FF-00C06C581497}\\Implemented Categories","",s,sizeof(s));
    GetValueData(HKEY_CLASSES_ROOT,s1,"",s,sizeof(s));
    memset(s1,12,sizeof(s1)); // тут же удаляем
    lstrcpyn(secondcode,s+25,13);
    _ps=secondcode;
    int i=1;
    while(*_ps)
    {
        if(*_ps>'9')*_ps-=('A'-'9');
        *_ps=*_ps-(i%6);
        _ps++;
        i++;
    }
};

BOOL Register()
{
    int _try=1;
    BOOL ok;
again:
    ok=FALSE;
    registed_copy=0;
    GetSecondCode();
    if(_try)
    {
        _try=FALSE;
        DWORD id=(DWORD)&_try;
        TFCollection*f=dllmanager->LoadPlugin(
                   #ifdef WIN32
                           "sc_reg32.dll"
                   #else
                           "sc_reg16.dll"
                   #endif
                           ,id);
        if(f)
        {
            BOOL (SDKPROC * Reg)(char*);
            (UINT32)Reg=f->Procedure(SC_GETPROCADDRESS,(UINT32)"ScRegister",0);
            if(Reg)
            {
                if(Reg(ini_file)){ok=TRUE;}
            }
            dllmanager->FreeDLL(id);
            if(ok)goto again;
        }
    }
    if(IsCodeValid())
    {
        ok=TRUE;
    }
    if(ok)
    {
        registed_copy=1;
        int count=GetMenuItemCount(mainwindow->menu);
        HMENU hm=GetSubMenu(mainwindow->menu,count-1);
        count=GetMenuItemCount(hm);
        DeleteMenu (hm,count-3,MF_BYPOSITION);
    }
    return ok;
};
#endif

BOOL ScMessageLoop()
{
#ifdef PROTECT
    secondcode[1]=0;
#ifndef DEMO
    GetSecondCode();
    if(!IsCodeValid())
    {
        // сначала добавляем защиту
        int count=GetMenuItemCount(mainwindow->menu);
        HMENU hm=GetSubMenu(mainwindow->menu,count-1);
        count=GetMenuItemCount(hm);
        char s[256];
        int i=IDS_str227*2;
        LoadString(HResource,i/2,s,sizeof(s));
        InsertMenu(hm,count-2,MF_BYPOSITION|MF_ENABLED|MF_STRING,CM_REGISTER,s);
        if(!ReminderDialog(hmain))
        {
            DestroyWindow(hmain);
            return FALSE;
        }         //            12
        if(1)
        { // защита по времени
            HINSTANCE h=HResource;
            HResource=0;
            if(!GetValueData(HKEY_CLASSES_ROOT,"CLSID\\{E627A360-F6EB-11D2-977D-00C06C581497}\\Implemented Categories","",s,sizeof(s)))
            {
                lstrcpy(s,"{E627A300-F600-11D2-977D-00C06C581497}");
            }
            date d;
            getdate(&d);
            int t=(s[7]-'0')*10+(s[8]-'0');
            int daycount=(s[12]-'0')*10+(s[13]-'0');
            if(daycount<0)daycount=1;
            if(d.da_day!=t || daycount<1)
            {
                daycount++;
                if(daycount>30)
                {
                    MessageBox(mainwindow->HWindow,"Your TRIAL period is expired!\nPlease register your copy!","Stratum 2000",MB_OK|MB_ICONINFORMATION);
                    DestroyWindow(hmain);
                    return FALSE;
                }
                if(daycount>10)
                {
                    char s[512];
                    wsprintf(s,"Your TRIAL period will be expired in %d days!\nPlease register your copy!",30-daycount);
                    MessageBox(mainwindow->HWindow,s,"Stratum 2000",MB_OK|MB_ICONINFORMATION);
                }
                char ss[10];
                wsprintf(ss,"%02d",d.da_day);
                s[7]=ss[0];
                s[8]=ss[1];
                wsprintf(ss,"%02d",daycount);
                s[12]=ss[0];
                s[13]=ss[1];
                SetValueData(HKEY_CLASSES_ROOT,"CLSID\\{E627A360-F6EB-11D2-977D-00C06C581497}\\Implemented Categories","",s);
            }
            HResource=h;
        }
    }
    else
    {
        registed_copy=1;
    }
#endif
#endif

#ifdef DEMO
    //DemoMSG();
#endif

#ifdef RUNTIME
    if(!GetProject(NULL))
    {
        mainwindow->Cm_Load(NULL,TRUE);
        if(!GetProject(NULL))
        {
            DestroyWindow(hmain);
            return FALSE;
        }
    }
#endif

    MSG msg;
    BOOL fRetVal = TRUE;
    title=0;
    while (fRetVal)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                fRetVal = FALSE;
            //	 TranslateMessage(&msg);
            //	 TranslateAccelerator(hmain,akeys,&msg);
            //	 DispatchMessage(&msg);
            if(!TranslateMDISysAccel(mainwindow ? mainwindow->mdi : 0, &msg) &&
               !TranslateAccelerator(hmain,akeys, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            IdleAction();
        }
    }
    return TRUE;
}

HBITMAP bmp_folder=0;

void AddMsgStr(char *s,long t)
{
    char buf[256];
    if(!HIWORD(s))
    {
        LoadString(HResource,LOWORD(s),buf,sizeof(buf));
        s=buf;
    }
    char ss[256];
    double d=t;
    int t1=floor(d/1000.0);
    int t2=floor(1000.0*(d/1000.0-t1));
    char cmd[64];
    if (t1)
    {
        LoadString(HResource,IDS_str192,cmd,sizeof(cmd));
        wsprintf(ss,cmd,s,t1,t2);
    }
    else
    {
        LoadString(HResource,IDS_str193,cmd,sizeof(cmd));
        wsprintf(ss,cmd,s,t2);
    }
    _Message(ss);
}

extern  ScPostInit()
{
#ifdef PROTECT
    secondcode[1]=0;
#ifndef DEMO
    GetSecondCode();
    if(IsCodeValid())
    {
        registed_copy=1;
    }
#endif
#endif
    int mode=SCGetScVar("OnStartup",0);
    switch(mode){
        case 1:
        {
            if(mainwindow->recently->count)
            {
                char *_name=(char *)mainwindow->recently->At(0);
                char s[256];
                lstrcpy(s,_name);
                mainwindow->Cm_Load(s);
            }
        }break;
        case 2:
            AboutDialog(mainwindow->HWindow,2);
            if(!registed_copy)
            {
#ifdef PROTECT
                GetSecondCode();
                if(IsCodeValid())
#endif
                {registed_copy=1;}
            }
            break;
    }
}
extern BOOL noschemeonload;

BOOL ScProcessCmdLine(char **_argv,int _argc)
{
    int pl=0;
    BOOL run=0;
    BOOL edit=0;
    char *state=NULL;
    int i=1;
    while (i<_argc)
    {
        char * s=_argv[i];
        if(s)
        {
            if(s[0]=='/'|| s[0]=='-')
            {
                s++;
                if (!strnicmp(s,"LIB=",4))
                {
                    char * ps=s+4;
                    if(*ps)
                    {
                        TLibrary*lib=new TLibrary(ps,librarys);
                        AddDirEx(lib,TRUE,0);
                    }
                }
                if (!lstrcmpi(s,"NOTOOLS"))mainwindow->ShowControl(0);
                if (!lstrcmpi(s,"NOSTATUS"))mainwindow->ShowStatus(0);
                if (!lstrcmpi(s,"NOMENU"))mainwindow->ShowSysMenu(0);
                if (!lstrcmpi(s,"RUN"))run=1;
                if (!lstrcmpi(s,"EDIT"))edit=1;

                if (!strnicmp(s,"DSK=",4))
                {
                    char * ps=s+4;
                    while(*ps && *ps==' ')ps++;
                    if(*ps)cnt_tpl=NewStr(ps);
                }
                if (!strnicmp(s,"STATE=",6)){state=s+6;}
            }
            else
            {
                if(pl==0 )
                {
                    char ss[260];
                    if(!IsAbsolutePath(s))
                    {
                        getcurdir(0,ss);
                        AddSlash(ss);
                        lstrcat(ss,s);
                        s=ss;
                    }
                    noschemeonload=1;
                    mainwindow->Cm_Load(s);pl=1;
                    noschemeonload=0;
                }
            }
        }
        i++;
    }
    PObject object;
    PClass _class;
    if(GetRootObject(&_class,&object))
    {
        if(_class)
        {
            _Message(_M_PROJECTLOAT);
        }
        if(state)
            LoadState(NULL,state);
#ifndef RUNTIME
        if (!run || edit)
            mainwindow->CreateSchemeWindow(_class->name,object);
#endif
        if (run)
        {
            _Message(_M_AUTORUN);
            Run();
        }
    }
}

extern void (SDKPROC * __WTS__)(char *s)=NULL;
//extern BOOL   (WINAPI * FrameSetPos2d)(HSP2D,HOBJ2D,long pos);

void WriteTitleStr(char *s)
{
    if(__WTS__) __WTS__(s);
};

BYTE inited=0;
BOOL ScInitCore(TSTRATUMINFO* info)
{
#ifdef LOGON
    LogMessage("ScInitCore start");
#endif
    if(inited)
        return TRUE;

    if(info && info->ScProc)
    {
        (UINT32)__WTS__ = info->ScProc(SC_GETPROCADDRESS,(UINT32)"WriteTitleStr",0);
        title=info->hMainWindow;
    }
#ifdef DLL
    lstrcpy(libpath,info->path);
#else
    GetPath(libpath,_argv[0]);
#endif

#ifdef LOGON
    {
        char s[999];
        lstrcpy(s,"Detect path: ");
        lstrcat(s,libpath);
        LogMessage(s);
    }
#endif
    sc_path =NewStr(libpath);
    SetCurrentDirectory(sc_path);
    lstrcpy(scheme_path, libpath);

    char s[999];
    if(!cnt_tpl)
    {
        lstrcpy(s,sc_path);
        lstrcat(s,"\\control.dsk");
        cnt_tpl = NewStr(s);
    }
    lstrcpy(s,libpath);
    lstrcat(scheme_path,"\\data\\icons\\");
    lstrcat(libpath,"\\library");
    lstrcat(s,"\\sc3.ini");
    ini_file = NewStr(s);

#ifdef NODLLRESOURCE
    HResource=hInstance;
#else
    char lib_[999]="";
    for(int i=0;i<__argc;i++)
    {
        if(!strnicmp(__argv[i],"/resource=",10))
        {
            char * ps=__argv[i]+10;
            while (*ps==' ')ps++;
            lstrcpy(lib_,ps);
        }
    }
    if(!lstrlen(lib_))
    {
        GetPrivateProfileString("preferences","resource","r1251s??.dll",lib_,sizeof(lib_),ini_file);
        char *ps=strstr(lib_,"??");
        if(ps)
        {
#ifdef WIN32
            *ps='3';ps++;*ps='2';
#else
            *ps='1';ps++;*ps='6';
#endif
        }
    }
    HResource=LoadLibrary(lib_);

#ifndef WIN32
    if(HResource<HINSTANCE_ERROR)	HResource=0;
#endif
    if(!HResource)
    {
        ShowWindow(info->hMainWindow, SW_HIDE);
        char msg[256];
        wsprintf(msg,"Unable to locate resource file: %s",lib_);
        MessageBox(0,msg,"Stratum loading ERROR",MB_OK|MB_ICONSTOP);
        return FALSE;
    }
#ifdef LOGON
    LogMessage("Check resource library version");
#endif
    //определяем версию библиотеки ресурсов (rXXXXsXX.dll)
    char test[64];
    LoadString(HResource,IDS_str201,test,sizeof(test));
    //Определяем версию библиотеки sccore32.dll
    char * _test=
        #ifdef RUNTIME
            "runtime";
#else
            "normal";
#endif
    if(lstrcmp(test,_test))
    {
        ShowWindow(info->hMainWindow,SW_HIDE);
        char s[256],s1[256];
        LoadString(HResource,IDS_str230,s1,sizeof(s1));
        wsprintf(s,s1,lib_,test,"sccore32.dll",_test);
        MessageBox(0,s,"Stratum loading ERROR",MB_OK|MB_ICONSTOP);
        return FALSE;
    }
#endif
    // определяем кодировку
    char spage[32];
    LoadString(HResource,IDS_str219,spage,sizeof(spage));
    sscanf(spage,"%d,%d",&SCcodepage,&SCcharset);

    if(!RegisterWindows())
    {
#ifdef LOGON
        LogMessage("Cant Register Windows classes");
#endif
        return 1;
    }
#ifdef LOGON
    LogMessage("Registering Windows classes");
#endif

#ifdef WIN32
    if(HResource==0)	return FALSE;
#else
    if(HResource<HINSTANCE_ERROR)	return FALSE;
#endif
    WriteTitleStr("Init resources...");
    LoadResources();
    WriteTitleStr("Init error handler...");
    InitErrorHandler();
#ifdef LOGON
    LogMessage("Init Error handler");
#endif
    CreateBrushes();
#ifdef LOGON
    LogMessage("Create solid brushes and fonts");
#endif
    ballon_message=RegisterWindowMessage("SC ballon message");
    scwnd_message=RegisterWindowMessage("SC identify message");
    InitScVars();

#ifdef LOGON
    LogMessage("Init main window");
#endif
    WriteTitleStr("Init main window...");
    hmain=CreateWindow(MainClassName,"",
                       WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_BORDER,
                       0,0,640,480,NULL,NULL,(HINSTANCE)hInstance,NULL);

#ifdef LOG_ON
    if(hmain)
        log.WriteDOSSTR("Create MainWindow");
    else
        log.WriteDOSSTR("Cant Create MainWindow");
#endif
    WriteTitleStr("Init video stub...");
    VIDEOINIT vi;
    vi.hwnd=mainwindow->HWindow;
    vi.playing = &playing;
    //  vi.brush=BGrayed;
    InitVideo2d(&vi);

#ifdef LOGON
    LogMessage("Init video stub");
#endif

    WriteTitleStr("Init class list...");
    _Message(_M_INIT);
    long tic = GetTickCount();
    _Message(_M_INITCLASSLIST);
    int classcount = InitClassList(libpath);
    long tic1=GetTickCount();
    char buf[64];
    LoadString(HResource,IDS_str105,buf,sizeof(buf));
    wsprintf(s,buf,classcount);

#ifdef LOGON
    LogMessage("Init class list done");
    LogMessage(s);
#endif
    AddMsgStr(s,tic1-tic);
    WriteTitleStr("Init virtual machine...");
    VM_Init();
#ifdef LOGON
    LogMessage("Virtual machine init");
#endif

    WriteTitleStr("Init matrixs...");
    InitMatrixs();
#ifdef LOGON
    LogMessage("Init matrixs");
#endif

    streams=new TStreamMan();
    winmanager=new TWindowManager();

    if (GetPrivateProfileInt("preferences", "ctl3d", 0, ini_file))
    {
        WriteTitleStr("Init ctl3d ...");
        ctl3d=LoadLibrary("CTL3DV2.dll");
#ifdef WIN32
        if (ctl3d)
#else
        if (HINSTANCE_ERROR<ctl3d)
#endif
        {
            (FARPROC)Ctl3dColorChange=GetProcAddress(ctl3d,"Ctl3dColorChange");
            (FARPROC)Ctl3dAutoSubclass=GetProcAddress(ctl3d,"Ctl3dAutoSubclass");
            (FARPROC)Ctl3dRegister=GetProcAddress(ctl3d,"Ctl3dRegister");
            (FARPROC)Ctl3dUnregister=GetProcAddress(ctl3d,"Ctl3dUnregister");
            if(Ctl3dColorChange && Ctl3dAutoSubclass && Ctl3dRegister && Ctl3dUnregister)
            {
                sc_interface|=1;
                Ctl3dRegister(hInstance);
                Ctl3dAutoSubclass(hInstance);
            }
#ifdef LOGON
            LogMessage("Init CTL3DV2");
#endif
        }
        else
        {
            ctl3d=0;
            _Message(_M_CTL3DLOADERROR);
#ifdef LOGON
            LogMessage("Unable to init CTL3DV2");
#endif
        }
    }
#ifdef LOGON
    LogMessage("Init app done");
#endif
    dyno_mem=new TMemVarManager();
    SetMainTitle(NULL);

    akeys=LoadAccelerators(HResource,MAKEINTRESOURCE(MAIN_MENU));

#ifdef LOGON
    LogMessage("Loading desktop");
#endif
    WriteTitleStr("Loading desktop...");
    mainwindow->LoadDesktop();
    UpdateSystemVars();
    ShowWindow(hmain,SW_SHOW);
    UpdateWindow(hmain);

    InitAddLib();
#ifdef DBE
    if(SCGetScVar("preload_idapi",0))
    {
#ifdef LOGON
        LogMessage("Loading IDAPI");
#endif
        WriteTitleStr("Loading IDAPI ...");
        DBInitSQL();
    }
#endif
    if(SCGetScVar("preload_video",0))
    {
#ifdef LOGON
        LogMessage("Loading Video");
#endif
        WriteTitleStr("Loading Video ...");
        VideoLoad();
    }
    if(SCGetScVar("preload_compiler",0))
    {
#ifdef LOGON
        LogMessage("Loading compiler");
#endif
        WriteTitleStr("Loading compiler ...");
        InitCompiler();
    }
    if(SCGetScVar("preload_3d",0))
    {
#ifdef LOGON
        LogMessage("Loading 3d tools");
#endif
        WriteTitleStr("Loading 3d tools ...");
        InitTools3d();
    }
#ifdef LOGON
    LogMessage("Loading plugins");
#endif
    WriteTitleStr("Loading plugins...");
    char *ps=new char[4096];
    GetPrivateProfileString(_plugins,NULL,"",ps,4096,ini_file);
#ifdef LOGON
    LogMessage("GetPrivateProfileString:");
    LogMessage(_plugins);
    LogMessage(ps);
#endif
    char *_ps=ps;
    while(*_ps)
    {
        char s[256];
        GetPrivateProfileString(_plugins,_ps,"",s,sizeof(s),ini_file);
#ifdef LOGON
        LogMessage("	GetPrivateProfileString:");
        LogMessage(_ps);
#endif
        if(s[0]=='1')
        {
#ifdef LOGON
            LogMessage("	start dllmanager->LoadPlugin:");
            LogMessage(s);
#endif
            wsprintf(s,"($plugins)%s",_ps);
            dllmanager->LoadPlugin(s,0);
#ifdef LOGON
            LogMessage("	end dllmanager->LoadPlugin:");
#endif
        }
        _ps=_ps+lstrlen(_ps)+1;
    }
    delete ps;

#ifdef USEAUDIERE
#ifdef LOGON
    LogMessage("Loading audio");
#endif
    WriteTitleStr("Loading audio...");
    InitAudioCore();
#endif

#ifdef LOGON
    LogMessage("Execute Stop()");
#endif
    Stop();
    //  SCSetStatusText(0,"Ready");
    SetStatusBeginText();
    inited=1; // Только теперь можно говороить о запуске

#ifdef LOCKEXITBUTTONOFWINDOW
#ifdef LOGON
    LogMessage("LOCKEXITBUTTONOFWINDOW");
#endif
    HMENU hMenuHandle;
    hMenuHandle = GetSystemMenu(hmain, FALSE);
    if (hMenuHandle)
        DeleteMenu(hMenuHandle, SC_CLOSE, MF_BYCOMMAND);
#endif
#ifdef RUNTIME
#ifdef LOGON
    LogMessage("Maximize main window");
#endif
    ShowWindow(hmain,SW_MAXIMIZE);
#endif

#ifdef LOGON
    LogMessage("ScInitCore end");
#endif
    return TRUE;
}

BOOL ScDoneCore()
{
    if(!inited)return FALSE;
    inited=0;
    if (ctl3d)FreeLibrary(ctl3d);
#ifdef WIN32
    if (hRTFLib)FreeLibrary(hRTFLib);
#endif
#ifdef DBE
    DoneSQL();
#endif
    DoneClassList();
    delete(winmanager);
    DoneMatrixs();
    mainwindow=0;
    DoneCompiler();
    delete dyno_mem;
    VM_Done();
    if (ini_file){delete ini_file;ini_file=0;}
    if (streams)delete(streams);streams=0;
    if (bmp_folder)DeleteObject(bmp_folder);
    DoneErrorHandler();
    DoneBrushes();
    //  DoneVideo2d();
    Done3dTools();

#ifdef USEAUDIERE
    DoneAudioCore();
#endif

    delete sc_path;sc_path=NULL;
    FreeResources();
#if !defined(NODLLRESOURCE)
    FreeLibrary(HResource);
#endif
    if (spec_space)DeleteSpace2d(spec_space);
    DoneScVars();
    delete cnt_tpl;
#ifdef LOGON
    DoneLog();
#endif
#ifdef CHECKHEAP
    CheckFinalMemoryState();
#endif
    return TRUE;
}

#ifdef LOG_ON
extern BYTE logenable;
#endif

char* sc_path=NULL;

BOOL SubDone()
{
#ifdef WIN32
    if(inited)
    {
        inited=FALSE;
        if(SaveAllModifyClasses(FALSE))
        {
            if(MessageBox(0,"Classes was changed!\n Are you want to save it? If saving will finished correctly then rename all files *._cl to *.cls",
                          "Stratum filed",MB_ICONHAND|MB_YESNO)==IDYES)
            {
                SaveAllModifyClasses(TRUE);
            }
        }
        if(dllmanager){delete dllmanager;dllmanager=NULL;}
    }
#endif
    return TRUE;
};

#if defined(WIN32)
//#pragma argsused
BOOL WINAPI DllEntryPoint( HINSTANCE hinstDll,
                           DWORD fdwRreason,
                           LPVOID plvReserved)
#else /* not flat model  */
int FAR _pascal LibMain( HINSTANCE _hInstance,
                         WORD ,
                         WORD wHeapSize,
                         LPSTR  )
#endif /* __FLAT */
{
#ifdef  WIN32

    switch(fdwRreason){
        case DLL_PROCESS_ATTACH:{
            ::hInstance=hinstDll;
            return TRUE;// Indicate that the DLL was initialized successfully.
        }
        case DLL_PROCESS_DETACH:{

            SubDone();
            return TRUE;
        }
    }
    return FALSE;
#else
    if ( wHeapSize != 0 ) UnlockData( 0 );
    ::hInstance=_hInstance;
    return TRUE;
#endif

}
#if !defined(WIN32)
int CALLBACK WEP(int nExitType){
    //if (nExitType==WEP_FREE_DLL)
    SubDone();
    return 1;
}
#endif

extern "C" void * _SDKPROC SCNew(int size){
    return new char[size];
};
extern "C" void _SDKPROC SCDelete(void*p){
    delete p;
};
extern "C" void _SDKPROC SCAddSlash(char*s){
    AddSlash(s);
}

BOOL  SCVersion(TSTRATUMVERSION*info)
{
    info->size=sizeof(TSTRATUMVERSION);
    info->hVersion=3;
    info->lVersion=0;
    info->flags=
        #ifdef DEMO
            svfDEMO;
#elif defined DBE
            svfPROF;
#else
            0;
#endif
#ifdef WIN32
    info->flags|=svfWIN32;
#endif
#ifdef PROTECT
    info->flags|=svfPROTECT;
#endif
#ifdef RUNTIME
    info->flags|=svfRUNTIME;
#endif
    info->codepage=SCcodepage;
    info->build=__SC_BUILD__;
    lstrcpy(info->internalname,"Stratum Computer 3.0 for Windows");
    lstrcpy(info->date,__DATE__);
    return TRUE;
};

extern "C" UINT32 ScDllProc(UINT16 msg, UINT32 p1, UINT32 p2)
{
    switch(msg)
    {
        case  SC_INITLIBRARY:
            return ScInitCore((TSTRATUMINFO*)p1);

        case  SC_DONELIBRARY:
            return ScDoneCore();

        case  SC_GETVERSION:
            if(p1)
                SCVersion((TSTRATUMVERSION*)p1);
            else
                return FALSE;

        case  SC_ARGUMENTS:
        {
            __argv=(char**)p2;
            __argc=p1;
            return TRUE;
        }
        case  SC_SENDCMD:
            switch(p1)
            {
                case SC_CMD_USER:
                {
                    if(!lstrcmpi("enteridle",(char*)p2))
                    {
                        ScProcessCmdLine(__argv,__argc);
                        ScPostInit();
                        return ScMessageLoop();
                    }
                }return FALSE;
            }return FALSE;

        case  SC_GETPROCADDRESS:
        {
            char *name=(char *)p1;
            if(!lstrcmpi(name,"ScMessage"))return (DWORD)SCMessage;
            if(!lstrcmpi(name,"SCGetObject"))return (DWORD)SCGetObject;
            if(!lstrcmpi(name,"SCGetObjectProject"))return (DWORD)SCGetObjectProject;
            if(!lstrcmpi(name,"SCGetObjectByFullName"))return (DWORD)SCGetObjectByFullName;
            if(!lstrcmpi(name,"SCGetFullObjectName"))return (DWORD)SCGetFullObjectName;
            if(!lstrcmpi(name,"SCInstallCallBack"))return (DWORD)SCInstallCallBack;
            if(!lstrcmpi(name,"SCUnInstallCallBack"))return (DWORD)SCUnInstallCallBack;
            if(!lstrcmpi(name,"SCInsertString"))return (DWORD)SCVm_InsertString;
        }break;
    }
    return NULL;
}

