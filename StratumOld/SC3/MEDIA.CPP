/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
      media.cpp
Author:
      Alexander Shelemekhov
*/

#include "windows.h"
#include "mmsystem.h"
#include "class.h"
#include "media.h"
#include "errors.h"
#include "video\vid_type.h"
#include <gwin2d3d\gwinfunc.h>
#include "graphic\_g_def.h"
#include "video\scvideo.h"
#include "graphic\3dtools.h"
#include <mem.h>
#include "stream.h"
#include "twindow.h"
#include "grtools.h"
#include <stratum\sccall.h>
#include "dllman.h"
#include <stratum\stratum.h>
#include <stratum\sc_types.h>
#include <stratum\sc_msg.h>
#include "msgcrk.h"
#include "strings.rh"
JOYCAPS  jcaps;




void DrawShadowFrame(HDC hdc,int x,int y,int sizex,int sizey,BYTE flags);

void InitInput()
{
    int numj = joyGetNumDevs();
    if (numj > 0)
    {
        joyGetDevCaps(JOYSTICKID1, &jcaps, sizeof(jcaps));
    }
}

void InitMedia()
{
}

int PlayWAV(char* lpszSoundName, UINT uFlags)
{
    if (lpszSoundName && *lpszSoundName==0)lpszSoundName=NULL;
    return sndPlaySound(lpszSoundName,uFlags);
}

double GetJoyX(int id){
    JOYINFO ji;
    joyGetPos(id,&ji);
    return   (((double)(ji.wXpos-jcaps.wXmin))/((double)jcaps.wXmax-(double)jcaps.wXmin));
};
double GetJoyY(int id){
    JOYINFO ji;
    joyGetPos(id,&ji);
    return (((double)(ji.wYpos-jcaps.wYmin))/((double)jcaps.wYmax-(double)jcaps.wYmin));
};
double GetJoyZ(int id){
    JOYINFO ji;
    joyGetPos(id,&ji);
    return   (((double)(ji.wZpos-jcaps.wZmin))/((double)jcaps.wZmax-(double)jcaps.wZmin));
};
BYTE   GetJoyBtn(int id){
    JOYINFO ji;
    joyGetPos(id,&ji);
    return (BYTE)ji.wButtons;
};
DWORD mcirez=0;

double GetLastMCIrez(){
    DWORD _mcirez=mcirez;mcirez=0;
    return _mcirez;
};

DWORD V_mcisendstring(char *cmd, char* ret, int)
{
    if (ret)
        *ret = 0;
    DWORD mcirez = mciSendString(cmd, ret, ret ? 255 : 0, 0);

    if (mcirez)
    {
        char s[1024];
        mciGetErrorString(mcirez,s,sizeof(s));
        _Message(s);
    }
    return mcirez;
};

UINT _mcigeterrorestring(double error,char*buf){
    return mciGetErrorString(error,buf,255);
};

void PauseAll(){
    aviaudioMessage(0,WM_SCV_PAUSEALL,1,0);
};
void ResumeAll(){
    aviaudioMessage(0,WM_SCV_PAUSEALL,0,0);
};

HV2D GetNextVideo(HV2D hv){
    return (HV2D)aviaudioMessage(0,WM_SCV_GETNEXTVIDEO,hv,0);
}
BOOL VideoInfoDialog(HWND hwnd,HV2D hv){
    return aviaudioMessage(0,(INT16)WM_SCV_VIDEODIALOG,(LONG)hwnd,(LPARAM)hv);
};
void CloseAllVideos(){
    aviaudioMessage(0,(INT16)WM_SCV_VIDEOCLOSEALL,0,0);
};
BOOL CanCloseAllVideos(){
    return (BOOL)aviaudioMessage(0,(INT16)WM_SCV_canVIDEOCLOSEALL,0,0);
};



#define VIDEO_LAST 16
#ifndef VIDEO_LINK

BOOL trytoload=1;
HINSTANCE hinstvideo=NULL;
HINSTANCE  hinst3d  =NULL;
VIDEOINIT videoinit;

BOOL VideoAvailable2d(){
    return hinstvideo!=0;
};
#ifdef WIN32
int  (WINAPI * _InitVideo2d)(VIDEOINIT*)=NULL;
//void (WINAPI * _DoneVideo2d)()=NULL;
#else
int  (far  _pascal * _InitVideo2d)(VIDEOINIT*)=NULL;
//void (far  _pascal * _DoneVideo2d)()=NULL;
#endif
void AddMsgStr(char *s,long t);
int VideoLoad()
{
#ifdef LOGON
    LogMessage("VideoLoad start");
#endif

    DWORD tic,tic1;
    tic=GetTickCount();

    TFCollection*f=dllmanager->LoadPlugin(
                   #ifdef WIN32
                       "scvid_32.DLL"
                   #else
                       "scvid_16.DLL"
                   #endif
                       ,2);
    if(f){
        hinstvideo=f->hdll;
#ifdef LOGON
        if(hinstvideo)
        {
            LogMessage("scvid_32.dll loaded");
        }
        else
        {
            LogMessage("scvid_32.dll NOT loaded");
        }
#endif
        (FARPROC)OpenVideo=GetProcAddress(hinstvideo,"OpenVideo");
        (FARPROC)CloseVideo=GetProcAddress(hinstvideo,"CloseVideo");
        (FARPROC)_InitVideo2d=GetProcAddress(hinstvideo,"InitVideo2d");
        //(FARPROC)_DoneVideo2d=GetProcAddress(hinstvideo,"DoneVideo2d");
        (FARPROC)CreateVideoFrame2d=GetProcAddress(hinstvideo,"CreateVideoFrame2d");
        (FARPROC)VideoSetPos=GetProcAddress(hinstvideo,"VideoSetPos");
        (FARPROC)GetVideoProp=GetProcAddress(hinstvideo,"GetVideoProp");
        (FARPROC)SetFrameProp2d=GetProcAddress(hinstvideo,"SetFrameProp2d");
        (FARPROC)FrameSetPos2d=GetProcAddress(hinstvideo,"FrameSetPos2d");
        (FARPROC)VideoPlay2d=GetProcAddress(hinstvideo,"VideoPlay2d");
        (FARPROC)VideoPause2d=GetProcAddress(hinstvideo,"VideoPause2d");
        (FARPROC)VideoResume2d=GetProcAddress(hinstvideo,"VideoResume2d");
        (FARPROC)VideoStop2d=GetProcAddress(hinstvideo,"VideoStop2d");
        (FARPROC)VideoHandle=GetProcAddress(hinstvideo,"VideoHandle");
        (FARPROC)aviaudioMessage=GetProcAddress(hinstvideo,"aviaudioMessage");
        (FARPROC)GetFrameProp2d=GetProcAddress(hinstvideo,"GetFrameProp2d");
        (FARPROC)BeginWriteVideo2d=GetProcAddress(hinstvideo,"BeginWriteVideo2d");
        (FARPROC)WriteVideoFrame2d=GetProcAddress(hinstvideo,"WriteVideoFrame2d");

        if(!(OpenVideo&&CloseVideo&&_InitVideo2d&&GetFrameProp2d&&
             //_DoneVideo2d&&
             CreateVideoFrame2d&&VideoSetPos&&GetVideoProp&&
             SetFrameProp2d&&FrameSetPos2d&&VideoPlay2d&&VideoPause2d&&
             VideoResume2d&&VideoStop2d&&VideoHandle&&aviaudioMessage))
        {
#ifdef LOGON
            LogMessage("Not all video functions supported");
#endif
            _Message("Not all video functions supported");
            return 0;
        };
        int rez= _InitVideo2d(&videoinit);
        tic1=GetTickCount();
        AddMsgStr(MAKEINTRESOURCE(IDS_str229),tic1-tic);
        return rez;
    }
    return 0;
};
BOOL Done3dTools(){
    if(hinst3d){
        FreeLibrary(hinst3d);
        return TRUE;
    }
    return FALSE;
};

BOOL InitTools3d(){
    if (hinst3d)return TRUE;
    static BYTE trytoload3d=TRUE;
    if (trytoload3d){
        trytoload3d=0;
#ifdef WIN32
        hinst3d = LoadLibrary("sc3d__32.DLL");
#else
        hinst3d = LoadLibrary("sc3d__16.DLL");
#endif
#ifdef WIN32
        if (hinst3d) {
#else
        if (hinst3d > HINSTANCE_ERROR) {
#endif

            (FARPROC)Import3DS=GetProcAddress(hinst3d,"Import3DS");
            (FARPROC)MakeFace3d=GetProcAddress(hinst3d,"MakeFace3d");

            (FARPROC)MakeCylinder3d=GetProcAddress(hinst3d,"MakeCylinder3d");
            (FARPROC)MakeSphere3d  =GetProcAddress(hinst3d,"MakeSphere3d");
            (FARPROC)MakeTore3d    =GetProcAddress(hinst3d,"MakeTore3d");
            (FARPROC)MakeBar3d     =GetProcAddress(hinst3d,"MakeBar3d");
            (FARPROC)MakeGrid3d    =GetProcAddress(hinst3d,"MakeGrid3d");

            if (!(Import3DS&&MakeFace3d&&MakeGrid3d&&MakeBar3d&&MakeSphere3d&&MakeCylinder3d))
            {
                FreeLibrary(hinst3d);
                hinst3d=0;
                _Message("Not all 3d tools functions supported by SC3d16(32).dll");
                return 0;
            };
            return TRUE;
        }else {
            hinst3d=0;
#ifdef WIN32
            _Message("SC3d32.dll not found");
#else
            _Message("SC3d16.dll not found");
#endif
        } }
    return 0;
};

// Заглушка видео фрейма
LRESULT scCALLBACK StubVideo2d(HSP2D hsp,USEROBJSTRUCT*u,UINT command,DWORD lParam){
    if (VideoLoad()){
        USERTYPESTRUCT ut;
        ut.name="VIDEO2D";
        ut.objproc=0;
        ut.flags=0;
        ut.usersize=0;
        RegisterUserType2d(&ut);
        return ut.objproc(hsp,u,command,lParam);
    }
    BYTE * f=(BYTE *)u->ptr;
    switch (command){
        case OM_PAINT:{
            USERPAINT * up=(USERPAINT*)lParam;
            RECT r;
            r.left=up->_org.x;r.top=up->_org.y;
            r.right=r.left+up->_size.x;
            r.bottom=r.top+up->_size.y;
            // HGDIOBJ hbrOld = SelectObject(up->hDc,BGrayed);
            DrawShadowFrame(up->hDc,r.left,r.top,r.right-r.left,r.bottom-r.top,2);
            UINT old=SetTextAlign(up->hDc,TA_CENTER|TA_BASELINE);
            SelectObject(up->hDc,GetStockObject(ANSI_VAR_FONT));
            //	PatBlt(up->hDc,r.left,r.top,r.right-r.left,r.bottom-r.top,PATCOPY);

            ExtTextOut(up->hDc,(r.right+r.left)/2,(r.bottom+r.top)/2,
                       ETO_CLIPPED,&r,"Video not available",19,NULL);
            SetTextAlign(up->hDc,old);
            // SelectObject(up->hDc,hbrOld);
        } break;
        case OM_LOAD:{
            USERSTREAM * us=(USERSTREAM *)lParam;
            f=new BYTE[(int)(us->size+2)];
            memcpy(f+2,us->data,(int)us->size);
            *((INT16 *)f)=(INT16)us->size;
            u->ptr=f;
        }return 1;

        case OM_SAVE:{
            USERSTREAM * us=(USERSTREAM *)lParam;
            us->size=*((INT16 *)f);
            us->data=f+2;
        }return 1;
        case OM_AWAKEN:return 1;
        case OM_DESTROY:{
            if (f)delete f;
            return 0;
        }
    }
    return 0;
}

//////////////////////////////////////
/////  Заглушки  Заглушки  Заглушки
//////////////////////////////////////

HV2D WINAPI s_OpenVideo(char*file,UINT16 flags)
{
#ifdef LOGON
    LogMessage("Start open videofile:");
    LogMessage(file);
#endif
    if (VideoLoad())
    {
        HV2D res = OpenVideo(file,flags);//возвращает индекс видео в коллекции открытых видеофайлов (индекс начинается с 4096)
#ifdef LOGON
        if(res)
            LogMessage("Videofile opened");
        else
            LogMessage("Videofile NOT opened");
#endif
        return res;
    }
    return 0;
};

int  WINAPI s_CloseVideo(HV2D){
    return 0;
};
int  WINAPI s_GetVideoProp(HV2D hv,VIDEOPROP*v){
    if (VideoLoad())return GetVideoProp(hv,v);
    return 0;
};
BOOL WINAPI s_GetFrameProp2d(HSP2D hv,HOBJ2D obj,FRAMEPROP*f){
    if (VideoLoad())return GetFrameProp2d(hv,obj,f);
    return 0;
};
BOOL  WINAPI s_SetFrameProp2d(HSP2D,HOBJ2D,FRAMEPROP*){return FALSE;};

HOBJ2D WINAPI s_CreateVideoFrame2d(HSP2D,HV2D ,gr_float,gr_float,gr_float,gr_float,RECT *,DWORD)
{
    return FALSE;
};

BOOL WINAPI s_VideoSetPos(HV2D,long){return 0;};
BOOL WINAPI s_FrameSetPos2d(HSP2D,HOBJ2D,long){return 0;};
BOOL WINAPI s_VideoHandle(){return 0;};

BOOL WINAPI s_VideoPlay2d(HSP2D,HOBJ2D ,long ,long,long,UINT32){return 0;};
BOOL WINAPI s_VideoPause2d(HSP2D,HOBJ2D){return 0;};
BOOL WINAPI s_VideoResume2d(HSP2D,HOBJ2D){return 0;};
BOOL WINAPI s_VideoStop2d(HSP2D,HOBJ2D ){return 0;};
HV2D WINAPI s_BeginWriteVideo2d(HSP2D _hsp,char*file,int x,int y,int cx,int cy,char*f){
    if (VideoLoad())return BeginWriteVideo2d(_hsp,file,x,y,cx,cy,f);
    return 0;
};
BOOL WINAPI s_WriteVideoFrame2d(HV2D hv){return 0;};

LRESULT WINAPI s_aviaudioMessage(HWND, unsigned,LONG, LONG){return 0;};


HV2D (WINAPI * OpenVideo)(char far*,UINT16 flags)=s_OpenVideo;
BOOL (WINAPI * CloseVideo)(HV2D)=s_CloseVideo;
HOBJ2D(WINAPI * CreateVideoFrame2d)(HSP2D,HV2D ,gr_float x,gr_float y,gr_float sizex,gr_float sizey,RECT * src,DWORD flags)=s_CreateVideoFrame2d;
BOOL (WINAPI * VideoSetPos)(HV2D,long pos)=s_VideoSetPos;
BOOL (WINAPI * GetVideoProp)(HV2D,VIDEOPROP*)=s_GetVideoProp;
BOOL (WINAPI * SetFrameProp2d)(HSP2D,HOBJ2D,FRAMEPROP*)=s_SetFrameProp2d;
BOOL (WINAPI * FrameSetPos2d)(HSP2D,HOBJ2D,long pos)=s_FrameSetPos2d;
BOOL (WINAPI * VideoPlay2d)(HSP2D,HOBJ2D ,long start,long end,long rate,UINT32 flags)=s_VideoPlay2d;
BOOL (WINAPI * VideoPause2d)(HSP2D,HOBJ2D)=s_VideoPause2d;
BOOL (WINAPI * VideoResume2d)(HSP2D,HOBJ2D)=s_VideoResume2d;
BOOL (WINAPI * VideoStop2d)(HSP2D,HOBJ2D)=s_VideoStop2d;
BOOL (WINAPI * VideoHandle)()=s_VideoHandle;
LRESULT (WINAPI * aviaudioMessage)(HWND hwnd, unsigned msg,LONG wParam, LONG lParam)=s_aviaudioMessage;
BOOL (WINAPI * GetFrameProp2d)(HSP2D,HOBJ2D,FRAMEPROP*)=s_GetFrameProp2d;
HV2D (WINAPI * BeginWriteVideo2d)(HSP2D _hsp,char*file,int x,int y,int cx,int cy,char*f)=s_BeginWriteVideo2d;
BOOL (WINAPI * WriteVideoFrame2d)(HV2D hv)=s_WriteVideoFrame2d;


HV2D (WINAPI * Import3DS)(HSP3D,char*filename)=NULL;
BOOL (WINAPI * MakeFace3d)(HSP3D hsp3d,HOBJ2D,POINT3D * points,INT16 count,ROTATOR3D * r,
                           POINT3D * delta,INT16 asteps,COLOR3D color,MATERIAL3D *m,
                           POINT3D *org,UINT32 flags)=NULL;
HOBJ2D (WINAPI * MakeCylinder3d)(HSP3D hsp3d,HOBJ2D old,COLOR3D color,double R1,double R2,double L,INT16 c1,INT16 c2);
HOBJ2D (WINAPI * MakeSphere3d)(HSP3D hsp3d,HOBJ2D old,COLOR3D color,double R,INT16 c1,INT16 c2);
HOBJ2D (WINAPI * MakeTore3d)(HSP3D hsp3d,HOBJ2D old,COLOR3D color,double r1,double r2,INT16 pcount,INT16 c2);
HOBJ2D (WINAPI * MakeBar3d)(HSP3D hsp3d,COLOR3D color,double sizex,double sizey,double sizez);
HOBJ2D (WINAPI * MakeGrid3d)(HSP3D hsp3d,double size,int count,COLORREF color);


BOOL InitVideo2d(VIDEOINIT* vi)
{
    videoinit=*vi;
    USERTYPESTRUCT ut;
    ut.name="VIDEO2D";
    ut.objproc=StubVideo2d;
    ut.flags=UF_MMTEXT;
    ut.usersize=0;
    int reg=RegisterUserType2d(&ut);
    return reg;
};


#else

BOOL VideoAvailable2d(){return TRUE;};
#endif

#include "filetype.h"

long ScanBytes(int pixWidth, int bitsPixel)
{
    return (((long)pixWidth*bitsPixel+31) / 32) * 4;
}

long NColors(WORD bitCount)
{
    if (bitCount == 1 || bitCount == 4 || bitCount == 8)
        return 1 << bitCount;
    else
        if (bitCount == 24)
            return 0;
    return -1;
}

BOOL IsURL(char*file);
UINT GetFileType(char*s,int)
{
    if(IsURL(s))
        return FILE_URL;
    UINT ft=GetFileType2d(s,NULL);
    if(ft)
        return ft;
    TDOSStream st(s, TDOSStream::stOpenRead);
    if(st.status)return 0;

    LONG _id=st.ReadLong();
    switch(LOWORD(_id))
    {
        case CLASS_SIGNATURE:return FILE_CLS;
        case PRJ_SIGNATURE:return FILE_PRJ;
    }
    switch(_id)
    {
        case 'Rar!':return FILE_RAR;
    }
    { //MAT //stt
        st.Seek(0);
        char s[80];
        st.ReadStr(s,40);
        if (!lstrcmp(s,"MATRIX FILE."))return FILE_MAT;
        if (!lstrcmp("SC Scheme Variables",s))return FILE_STT;
    }
    return 0;
}

BOOL _EXPORT __ProgressProc(int p)
{
    SCSetStatusProgressBar(1,p);
    return TRUE;
}

HOBJ2D ImportBMP(HSP2D hsp, char *s, int type, HOBJ2D*ddib)
{
    if(openaslink)
    {
        char base[256];
        char relative[256];
        if(GetTexturePath2d(hsp,base))
        {
            if(GetRelativePath(base,s,relative))
            {
                return CreateRDib2d(hsp,relative);
            }
        }
        else
        {
            MSGBOX(GetSpaceWindow2d(hsp),MAKEINTRESOURCE(IDS_str223),_MSG_INFO,MB_ICONINFORMATION|MB_OK);
        }
    }
    else
    {
        BOOL dbl=0;
        HOBJ2D  hdib = ImportRaster2d(hsp, s, type, &dbl);
        if(dbl && ddib)
            *ddib=hdib;

        return hdib;
    }
    return 0;
}

HOBJ2D ImportDBM(HSP2D hsp,char *s,int type)
{
    //if(!type) type=GetFileType(s,0);
    //if(type>=raster_double_start && type<=raster_END)
    {
        if(openaslink)
        {
            char base[256];
            char relative[256];
            if(GetTexturePath2d(hsp,base))
            {
                if(GetRelativePath(base,s,relative))
                    return CreateRDoubleDib2d(hsp,relative);
            }
            else
            {
                MSGBOX(GetSpaceWindow2d(hsp),MAKEINTRESOURCE(IDS_str223),_MSG_INFO,MB_ICONINFORMATION|MB_OK);
            }
        }
        else
        {
            BOOL dbl=0;
            HOBJ2D  hdib = ImportRaster2d(hsp,s,type,&dbl);
            if(!dbl)
            {
                DeleteTool2d(hsp,DIB2D,hdib);
                hdib=0;
            }
            return hdib;
        }
    }
    return 0;
}

extern BYTE palette[];


HGLOBAL _CreateDIB2d(int sizex,int sizey,int bitCount,COLORREF bkcolor)
{
    void * bits;
    //проверяем корректное ли количество бит в пикселе передано
    switch(bitCount)
    {
        case 1: case 4:	case 8:	case 24:break;
        default: return 0;
    };

    //проверяем корректный ли размер области передан
    if(sizex<1)return 0;
    if(sizey<1)return 0;

    long sizeimage=ScanBytes(sizex,bitCount)*sizey;
    long _size=sizeimage+(sizeof(BITMAPINFO)+sizeof(COLORREF)*NColors((INT16)bitCount));

    BYTE * colortable;
    HGLOBAL block=GlobalAlloc(GMEM_MOVEABLE,_size);

    LPBITMAPINFOHEADER info=(LPBITMAPINFOHEADER) GlobalLock(block);
    info->biSize=sizeof(BITMAPINFOHEADER);
    info->biWidth=sizex;
    info->biHeight=sizey;
    info->biPlanes=1;
    info->biBitCount=(INT16)bitCount;
    info->biCompression=BI_RGB;
    info->biSizeImage=_size;
    info->biXPelsPerMeter=300;
    info->biYPelsPerMeter=300;
    info->biClrUsed=NColors((INT16)bitCount);
    info->biClrImportant=NColors((INT16)bitCount);
    colortable=((BYTE*)info)+sizeof(BITMAPINFOHEADER);
    switch(bitCount)
    {
        case 1:
        {
            COLORREF cr[2]={RGB(255,255,255),RGB(0,0,0)};
            memcpy(colortable,cr,8);
            bits=colortable+8;
        }break;
        case 4:
        {
            memcpy(colortable,palette,16*4);
            bits=colortable+16*4;
        }break;
        case 8:
        {
            COLORREF *p=new COLORREF[256];
            memset(p,0xff,256*4);
            memcpy(p,palette,16*4);
            int f=0;
            for(int r=0;r<6;r++)
                for(int g=0;g<6;g++)
                    for(int b=0;b<6;b++)
                    {
                        COLORREF c=RGB(255*r/5,255*g/5,255*b/5);
                        BOOL was=FALSE;
                        for(int i=0;i<16;i++)
                        {
                            if (p[i]==c)
                            {
                                was=TRUE;
                                break;
                            }
                        }
                        if(!was)
                        {
                            p[f]=c;
                            f++;
                        }
                    }
            for(int i=1;i<26;i++)
            {
                COLORREF c=RGB(255*i/26,255*i/26,255*i/26);
                p[f]=c;f++;
            }
            memcpy(colortable,p,256*4);
            bits=colortable+256*4;
            delete p;
        } break;
        case 24:
            bits=colortable;
            break;
    }
    BYTE fon=GetRValue(bkcolor);
    memset(bits,fon,(int)sizeimage);
    GlobalUnlock(block);
    return block;
};

typedef struct
{
    DWORD   key;
    UINT16  hmf;
    //RECT    bbox;
    INT16    l,t,r,b;
    WORD    inch;
    DWORD   reserved;
    WORD    checksum;
} PLACABLEMETAFILEHEADER;

HOBJ2D ReadRTF(HSP2D hsp,TStream*st){
    TFCollection*f=dllmanager->LoadPlugin("rtfdll32.dll",0);
    if(f){
        HOBJ2D( SDKPROC *ImportRTF)(HSP2D hsp,TStream*st);
        (FARPROC)ImportRTF=GetProcAddress(f->hdll,"ReadRTF");
        if(ImportRTF)return ImportRTF(hsp,st);
    }
    return 0;
};

BOOL WriteRTF(HSP2D hsp,HOBJ2D ht,TStream*st)
{
    TFCollection*f=dllmanager->LoadPlugin("rtfdll32.dll",0);
    if(f)
    {
        BOOL ( SDKPROC *_ExportRTF)(HSP2D hsp,HOBJ2D htm,TStream*st);
        (FARPROC)_ExportRTF=GetProcAddress(f->hdll,"WriteRTF");
        if(_ExportRTF)
            return _ExportRTF(hsp,ht,st);
    }
    return 0;
};

BOOL  SpaceExchange(HSP2D &hsp,HOBJ2D &hobject,GETFILETYPE&ft)
{
    BOOL rez=FALSE;
    TFCollection*f=dllmanager->LoadPlugin(ft.filter,1);
    if(f){
        if(f->_proc){
            TSTRATUMFILTER flt;
            memset(&flt,0,sizeof(flt));
            flt.size=sizeof(flt);
            flt.hsp2d=hsp;
            flt.hobject=hobject;
            lstrcpy(flt.type,ft.type);
            flt.filename=ft.file;
            flt.enabledialog=ft.flags&1;
            UINT mode=SC_IMPORTFILE;
            if(ft.Export)mode=SC_EXPORTFILE;
            rez=f->Procedure(mode,(UINT32)&flt,0);
            hsp=flt.hsp2d;
            if(!f->Procedure(SC_RESIDENT,0,0))dllmanager->FreeDLL(1);
        }else {dllmanager->FreeDLL(1);
            _Error(EM_BADPLUGIN,0,ft.filter);
        }
    }
    return rez;
};

BOOL  SimplyExchange(HSP2D&hsp,char *file,char *ext,BOOL export)
{
    GETFILETYPE ft;
    memset(&ft,0,sizeof(ft));
    ft.file=file;
    lstrcpy(ft.type,ext);
    wsprintf(ft.filter,
         #ifdef WIN32
             "($stratum)%sdll32.dll"
         #else
             "($stratum)%sdll16.dll"
         #endif
             ,ext);
    ft.Export=export;
    return SpaceExchange(hsp,0,ft);
};

/*
BOOL  SaveAsWMF(HSP2D hsp,char *file){
RECT2D r;
{
 PushCrdSystem2d(hsp);
 CRDSYSTEM2D c;
 c.type=MMTEXT_COORDINATE2D;
 SetCrdSystem2d(hsp,&c);
 }
 if(GetSpaceExtent2d(hsp,r))
  {
char szBuf[256];
  GetTempFileName((int)0, "sc3",(int) 0, szBuf);


   POINT2D _m,_d,_o;
   POINT2D _sm,_sd,_so;_sm.y=_sm.x=1.0;_sd.y=_sd.x=1.0;
   RECT _r;
   _so.y=r.left;_so.x=r.top;
   HDC hDc=CreateMetaFile(szBuf);

   _r.left=r.left-1;_r.top=r.top-1;
   _r.right=r.right+1;_r.bottom=r.bottom+1;

   GetScaleSpace2d(hsp,&_m,&_d);
   GetOrgSpace2d(hsp,&_o);

   SetScaleSpace2d(hsp,&_sm,&_sd);
//   SetOrgSpace2d(hsp,&_so);

   PaintSpace2d(hsp,hDc,&_r,PF_DIRECT|PF_NOCACHEDIB);
   SetScaleSpace2d(hsp,&_m,&_d);
//   SetOrgSpace2d(hsp,&_o);
   HMETAFILE hmf=CloseMetaFile(hDc);
   DeleteMetaFile(hmf);

   PLACABLEMETAFILEHEADER h;
   memset(&h,0,sizeof(h));
   h.key = 0x9AC6CDD7L;
    h.hmf=(UINT16)hmf;
   h.inch=96;

   h.l=r.left;
   h.t=r.top;
   h.r=r.right ;
   h.b=r.bottom;

   //
   WORD * w=(WORD*)&h;
   for(int i=0;i<10;i++)h.checksum^=(w[i]);
   {
   TDOSStream src(szBuf,TDOSStream::stOpenRead);
   TDOSStream st(file,TDOSStream::stCreate);
   st.Write(&h,sizeof(h));
   src.CopyTo(&st,0,src.GetSize());
   }
   ScDeleteFile(szBuf);
  }
 PopCrdSystem2d(hsp);
};
*/

//создаем структуру LPBITMAPINFO с изображением заданной области окна
LPBITMAPINFOHEADER CreateScreenDIB(HWND hw, double x,double y,double w,double h, HSP2D hsp, int attr,char **bits1)
{
    if(attr==0) attr = 24;
    HGLOBAL mem=_CreateDIB2d(w,h,attr,0);
    if(!mem) return NULL;
    //Формируем заголовок битовой карты
    LPBITMAPINFOHEADER info=(LPBITMAPINFOHEADER) GlobalLock(mem);
    char *bits=(char*)info;
    char *colortable=((char*)info)+sizeof(BITMAPINFOHEADER);
    switch(attr)
    {
        case 1:bits=colortable+8; break;
        case 4:bits=colortable+16*4; break;
        case 8:bits=colortable+256*4; break;
        case 24:bits=colortable; break;
    };

    HDC hdc = GetDC(hw); //контекст устройства экрана - содержит изображение экрана
    HDC dc=CreateCompatibleDC(hdc); //контекст устройства в памяти - содержит битовую карту
    HBITMAP hbm=CreateCompatibleBitmap(hdc,w,h);//битовая карта, в которую сохраним снимок экрана
    SelectObject(dc,hbm);//выбрием битовую карту в контексте устройсва в памяти

    if(hsp)
    {
        RECT r;
        r.left=0; r.top=0; r.right=w; r.bottom=h;
        //перемещаем графическое пространство в заданную точку
        POINT2D p, org;
        GetOrgSpace2d(hsp,&org);
        p.x=x; p.y=y;
        SetOrgSpace2d(hsp,&p);
        //отрисовываем область окна в контексте устройства
        PaintSpace2d(hsp,dc,&r,0);
        SetOrgSpace2d(hsp,&org);
    }
    else
        BitBlt(dc,0,0,w,h,hdc,x,y,SRCCOPY); //копируем изображение экрана в битовую карту

    GetDIBits(dc,hbm,0,h,bits,(LPBITMAPINFO)info,DIB_RGB_COLORS); //получаем информацию о сделаном снимке
    if(bits1) *bits1=bits;
    DeleteDC(dc);
    ReleaseDC(0,hdc);
    DeleteObject(hbm);
    return info;
}

BOOL SaveSpaceImage2d(HSP2D hsp,char *file,int attr,double x,double y,double nWidth,double nHeight)
{
    char* bits;
    LPBITMAPINFOHEADER info=CreateScreenDIB(0,x,y,nWidth,nHeight,hsp,attr,&bits);
    if(info)
    {
        //формируем заголовок файла BMP
        long sizeimage=ScanBytes(nWidth,attr)*nHeight;
        long _size=sizeimage+(sizeof(BITMAPINFO)+sizeof(COLORREF)*NColors((INT16)attr));

        TDOSStream st(file,TDOSStream::stCreate);
        BITMAPFILEHEADER fh;

        memset(&fh,0,sizeof(fh));
        fh.bfType = 'BM';
        fh.bfSize = sizeof(fh)+_size;
        fh.bfOffBits=(bits-(char*)info)+sizeof(fh);

        //сохраняем заголовок и карту в файл
        st.Write(&fh,sizeof(fh));
#ifdef WIN32
        st.Write(info,_size);
#else
        st.Write((void __huge *)info,_size);
#endif
        GlobalUnlock(info);
        GlobalFree(info);
    }
    return 0;
};

#ifdef  __NO_DEF
INT16     poSize;       // - размер этой структуры в байтах
POINT3D   poOrg;        // - координаты наблюдателя в мировой системе координат
POINT3D   poDirection;  // - направление(или целевая точка) взгляда в мировой системе координат
POINT3D   poUp;         // - вектор определяющий верх наблдателя
gr_float  poFocus;      // - фокусное расстояние (если <=0 то автоматически)
gr_float  poFar_Clip;   // - ближния отсекающая плоскость
gr_float  poNear_Clip;  // - дяльняя отсекающая плоскость
POINT3D   poExtent;     // - масштабы по всем осям
POINT2D   poOffset;     // - смещение центра (1 весь диапазон / 0.5 середина).
gr_float  poHazeScale;
INT32     poHazeLevels;
COLOR3D   poHazeColor;
COLORREF  poBackground; // - фоновый цвет
char      poRenderName[RENDER_NAME_SIZE];  // имя отрисовщика
INT16     poRenderType; // - тип отрисовщика
UINT32    poFlags;      // флаги
INT16     poPerspective;// тип перспективных преобразований
#endif

BOOL CameraProc3d(PObject po,HSP3D hsp3d,HOBJ2D obj,BOOL set){
    if(!po || (hsp3d==0) || (obj==0))return FALSE;
    CAMERA3D camera3d;
    if(GetCamera3d(hsp3d,obj,&camera3d)){
        CAMERA3D old=camera3d;
        if(set){
            camera3d.poOrg.x      =po->GetVarF("poOrgX",camera3d.poOrg.x);
            camera3d.poOrg.y      =po->GetVarF("poOrgY",camera3d.poOrg.y);
            camera3d.poOrg.z      =po->GetVarF("poOrgZ",camera3d.poOrg.z);
            camera3d.poDirection.x=po->GetVarF("poDirectionX",camera3d.poDirection.x);
            camera3d.poDirection.y=po->GetVarF("poDirectionY",camera3d.poDirection.y);
            camera3d.poDirection.z=po->GetVarF("poDirectionZ",camera3d.poDirection.z);
            camera3d.poUp.x       =po->GetVarF("poUpX",camera3d.poUp.x);
            camera3d.poUp.y       =po->GetVarF("poUpY",camera3d.poUp.y);
            camera3d.poUp.z       =po->GetVarF("poUpZ",camera3d.poUp.z);
            camera3d.poFocus      =po->GetVarF("poFocus",camera3d.poFocus);
            camera3d.poFar_Clip   =po->GetVarF("poFar_Clip",camera3d.poFar_Clip);
            camera3d.poNear_Clip  =po->GetVarF("poNear_Clip",camera3d.poNear_Clip);

            camera3d.poExtent.x   =po->GetVarF("poExtentX",camera3d.poExtent.x);
            camera3d.poExtent.y   =po->GetVarF("poExtentY",camera3d.poExtent.y);
            camera3d.poExtent.z   =po->GetVarF("poExtentZ",camera3d.poExtent.z);

            camera3d.poOffset.x   =po->GetVarF("poOffsetX",camera3d.poOffset.x);
            camera3d.poOffset.y   =po->GetVarF("poOffsetY",camera3d.poOffset.y);

            camera3d.poHazeColor.rgb =po->GetVarH("poHazeColor",camera3d.poHazeColor.rgb);
            camera3d.poBackground =po->GetVarH("poBackground",camera3d.poBackground);

            camera3d.poRenderType =po->GetVarF("poRenderType",camera3d.poRenderType);
            lstrcpy(camera3d.poRenderName,po->GetVarS("poRenderName",camera3d.poRenderName));

            camera3d.poFlags      =po->GetVarF("poFlags",camera3d.poFlags);
            camera3d.poPerspective=po->GetVarF("poPerspective",camera3d.poPerspective);

            if(memcmp(&old,&camera3d,sizeof(camera3d))){  return SetCamera3d(hsp3d,obj,&camera3d); }
        }else{

            po->SetVarF("poOrgX",camera3d.poOrg.x);
            po->SetVarF("poOrgY",camera3d.poOrg.y);
            po->SetVarF("poOrgZ",camera3d.poOrg.z);
            po->SetVarF("poDirectionX",camera3d.poDirection.x);
            po->SetVarF("poDirectionY",camera3d.poDirection.y);
            po->SetVarF("poDirectionZ",camera3d.poDirection.z);
            po->SetVarF("poUpX",camera3d.poUp.x);
            po->SetVarF("poUpY",camera3d.poUp.y);
            po->SetVarF("poUpZ",camera3d.poUp.z);
            po->SetVarF("poFocus",camera3d.poFocus);
            po->SetVarF("poFar_Clip",camera3d.poFar_Clip);
            po->SetVarF("poNear_Clip",camera3d.poNear_Clip);
            po->SetVarF("poExtentX",camera3d.poExtent.x);
            po->SetVarF("poExtentY",camera3d.poExtent.y);
            po->SetVarF("poExtentZ",camera3d.poExtent.z);
            po->SetVarF("poOffsetX",camera3d.poOffset.x);
            po->SetVarF("poOffsetY",camera3d.poOffset.y);
            po->SetVarF("poRenderType",camera3d.poRenderType);
            po->SetVarS("poRenderName",camera3d.poRenderName);
            po->SetVarF("poFlags",camera3d.poFlags);
            po->SetVarF("poPerspective",camera3d.poPerspective);

            po->SetVarH("poHazeColor",camera3d.poHazeColor.rgb);
            po->SetVarH("poBackground",camera3d.poBackground);

        }
        return TRUE;
    }
    return FALSE;
};

void SetActiveControl(HSP2D hsp,HOBJ2D hObject,BOOL grp=TRUE){
    HWND hw=GetControl2d(hsp,hObject);
    char s[64];
    GetClassName(hw,s,sizeof(s));

    HOBJ2D hgroup=GetObjectParent2d(hsp,hObject);

    if(!lstrcmpi(s,"BUTTON")){
        LONG style=GetWindowLong(hw,GWL_STYLE);
        int _style=(int)(style&0xf);
        if((_style==BS_AUTORADIOBUTTON)||(_style==BS_RADIOBUTTON)){
            if(hgroup){
                INT16 c=GetGroupItemsNum2d(hsp,hgroup);
                if(grp){
                    HWND __hw=0;
                    for(INT16 i=0;i<c;i++){
                        HOBJ2D ho=GetGroupItem2d(hsp,hgroup,i);
                        HWND _hw=GetControl2d(hsp,ho);
                        if(!i)__hw=_hw;
                        if(SendMessage(_hw,BM_GETCHECK,0,0)){
                            SetFocus(_hw);
                            return;
                        }}
                    SetFocus(__hw);
                    SendMessage(__hw,BM_SETCHECK,1,0);
                }else{
                    for(INT16 i=0;i<c;i++){
                        HOBJ2D ho=GetGroupItem2d(hsp,hgroup,i);
                        HWND _hw=GetControl2d(hsp,ho);
                        SendMessage(_hw,BM_SETCHECK,ho==hObject,0);
                    }
                }
            }else	SendMessage(hw,BM_SETCHECK,1,0);
        }
    }


    SetFocus(hw);
};

typedef struct tagCHILDKEY{
    HWND   hwnd;
    HOBJ2D hObject;
    int    nVirtKey;
    long   lKeyData;
    BOOL   calldef;
}CHILDKEY;

BOOL DefSpaceWindowProc(HSP2D hsp, HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_KEYDOWN:
            if(wParam==VK_TAB)
            {
                BOOL shift=(GetAsyncKeyState(VK_SHIFT)<0);
                HOBJ2D next=(HOBJ2D)1;
                if(shift)next=GetTopObject2d(hsp);else
                    next=GetBottomObject2d(hsp);
                while(next){
                    if(shift)next=GetLowerObject2d(hsp,next);else
                        next=GetUpperObject2d(hsp,next);

                    if(next && GetObjectType2d(hsp,next)==otCONTROL2D){
                        SetActiveControl(hsp,next);
                        return FALSE;
                    };
                }
            }break;
        case WM_PARENTNOTIFY:
            if(wParam==WM_KEYDOWN)
            {
                CHILDKEY &c1=*(CHILDKEY*)lParam;
                switch(c1.nVirtKey){
                    case VK_UP:
                    case VK_DOWN:{
                        int dlg=SendMessage(c1.hwnd,WM_GETDLGCODE,0,0);
                        if(!(dlg&DLGC_WANTARROWS)){
                            HOBJ2D hgroup=GetObjectParent2d(hsp,c1.hObject);
                            if(hgroup){
                                INT16 i=IsGroupContainObject2d(hsp,hgroup,c1.hObject);
                                if(i){
                                    INT16 c=GetGroupItemsNum2d(hsp,hgroup);
                                    i--;
                                    if(c1.nVirtKey==VK_DOWN){
                                        i++;
                                        if(i>=c)i=0;
                                    }else{
                                        if(i<=0)i=c-1;else i--;
                                    }
                                    HOBJ2D ho=GetGroupItem2d(hsp,hgroup,i);
                                    SetActiveControl(hsp,ho,FALSE);
                                }
                            }
                        }
                    }return FALSE;
                    case VK_TAB:{
                        BOOL shift=(GetAsyncKeyState(VK_SHIFT)<0);
                        int dlg=SendMessage(c1.hwnd,WM_GETDLGCODE,0,0);
                        if(!(dlg&DLGC_WANTTAB)){
                            c1.calldef=0;
                            HOBJ2D hgroup=GetObjectParent2d(hsp,c1.hObject);
                            if(hgroup){
m1:
                                HOBJ2D _hgroup;
                                _hgroup=GetObjectParent2d(hsp,hgroup);
                                if(_hgroup){
                                    hgroup=_hgroup;
                                    goto m1;
                                }
                            }


                            HOBJ2D next=c1.hObject;
m2:
                            if(shift)next=GetLowerObject2d(hsp,next);else
                                next=GetUpperObject2d(hsp,next);
                            if(!next){
                                if(shift)next=GetTopObject2d(hsp);else
                                    next=GetBottomObject2d(hsp);
                            }
                            if(next==c1.hObject)return FALSE;

                            if(GetObjectType2d(hsp,next)==otCONTROL2D){
                                if(!IsGroupContainObject2d(hsp,hgroup,next)){
                                    SetActiveControl(hsp,next);
                                    return FALSE;
                                }}
                            goto m2;
                        }
                    };
                }

            }break;

        case WM_PAINT:{
            HDC hdc;
            PAINTSTRUCT ps;
            hdc=BeginPaint(hwnd,&ps);
            PaintSpace2d(hsp,hdc,&ps.rcPaint,0);
            EndPaint(hwnd,&ps);
        }return FALSE;

        case WM_COMMAND:{
            EV_COMMAND
                    HSP2D _hsp;
            HOBJ2D id;

            if(hwndCtl && (wNotifyCode==BN_CLICKED) && GetControlId2d(hwndCtl,&_hsp,&id) && _hsp==hsp){
                CONTROLS type=_GetControlType(hwndCtl);
                if(type==CNT_BUTTON){
                    LONG style=GetWindowLong(hwndCtl,GWL_STYLE)&0x0f;
                    if((style == BS_RADIOBUTTON) ||(style == BS_AUTORADIOBUTTON)){
                        //      BOOL c=SendMessage(hwndCtl,BM_GETCHECK,0,0);
                        //      HOBJ2D group=GetObjectParent2d(hsp,id);
                        SendMessage(hwndCtl,BM_SETCHECK,1,0);
                        SetActiveControl(hsp,id,FALSE);
                    }}
            }
        }break;

    }
    return TRUE;
}

HOBJ2D ImportObject3d(HSP3D hsp3d,char * file,int type)
{
    if (!type)type=GetFileType(file,0);

    switch(type)
    {
        case file3d_3DS:
        {
            if (InitTools3d())
            {
                return Import3DS(hsp3d,file);
            }
        }break;

        case file_SPACE3D:
        {
            TDOSStream st(file, TDOSStream::stOpenRead);
            if(!st.status){
                long size=st.GetSize();
                HGLOBAL block=GlobalAlloc(GMEM_MOVEABLE,size);
#ifdef WIN32
                void * adr=GlobalLock(block);
#else
                void huge* adr=(void huge*)GlobalLock(block);
#endif
                st.Read(adr,size);
                GlobalUnlock(block);
                HOBJ2D object3d=PasteFromMemory3d(hsp3d,block,NULL);
                GlobalFree(block);
                return object3d;
            }}break;
    }
    return 0;
};


