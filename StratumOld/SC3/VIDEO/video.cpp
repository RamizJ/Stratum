#undef STRATUM_INTERNAL
#ifndef WIN32
#include <stdlib.h>
#endif


//#include <vfw\avifile.h>
//#include <vfw\drawdib.h>
//#include <vfw\compddk.h>

#include <gwin2d3d\gwinfunc.h>
#include <mem.h>
//#include <windows.h>
//#include <mmsystem.h>
#include <stdio.h>

#include "video\scvideo.rh"
#include "video\vengine.h"
#include "video\scvideo.h"
#include "video\vid_type.h"
#include <stratum\stratum.h>
#include <string.h>


#define MS_FOR_15FPS             66
//#define CAPFRAME
BYTE  capconnectfiled=0;
//http://www.microsoft.com/hwdev/desinit/vfwmap.htm
BOOL StrToDouble(char*text,double&value){
 if(text){
char * endptr=(char *)text;
 value = strtod(text,&endptr);
	if (endptr==NULL || *endptr==0) return TRUE;
}else{
 value=0;
}

return 0;
}

BOOL TVideo::IsEqual(char*f2){
  char file1[260];
  char *f1=GetFileName(TRUE,file1);
  if (!lstrcmpi(f1,f2))return TRUE;
 return FALSE;
};
BOOL TCamera::IsEqual(char*f2){
  if(GetVideoType(f2)==mode){
   char *_ps=strstr(f2,"/driver=");
   if(_ps){
    return FALSE;
   }
   return TRUE;
  }
  return FALSE;
};

TVideo::TVideo(VIDEO_MODE m,int size){
   int sizeofvmt=((BYTE*)&mode)-(BYTE*)(this);
   memset( ((BYTE*)this)+sizeofvmt , 0 , size - sizeofvmt );
   mode=m;
   Frames=new TCollection(10,10);
 };

char * TVideo::GetFileName(BOOL fullname,char*buffer,HSP2D){
   return "";
 };

TVideo::~TVideo(){
 if (Frames){
  for(int i=0;i<Frames->count;i++){TFrame * f=(TFrame *)Frames->At(i);delete f;}
  delete Frames;
 }
 if (Textures){
    for(int i=0;i<Textures->count;i++){
     Texture3d * f=(Texture3d *)Textures->At(i);
     delete f;
    }
  delete Textures;
 }
};

BOOL TVideo::IsUsed(){
 if(Frames && Frames->count)return TRUE;
 if(Textures && Textures->count)return TRUE;
 return FALSE;
};
BOOL TVideo::UpdateViews(LONG*pos){
 for(C_TYPE i=0;i<Frames->count;i++)
  {
    TFrame * f=(TFrame *)Frames->At(i);
      if ((f->flags&V2_SYNC)||(mode==VIDEOCAMERA))
        {
         if ((pos==NULL)||(f->pos!=(*pos)))
          {
           if(pos)f->pos=*pos;
           UpdateObject2d(f->hsp,f->handle,NULL);
          }
        }
  }
 if(Textures){
   for(C_TYPE i=0;i<Textures->count;i++)
   {
    Texture3d * f=(Texture3d *)Textures->At(i);
    UpdateTool2d((HSP2D)f->hsp3d,TEXTURE3D,f->hMaterial,NULL);
   }
 }
};
BOOL TVideo::DrawFrame(HDC hdc, int xDst, int yDst,int dxDst, int dyDst,
     LONG *pos,int xSrc,int ySrc, int dxSrc,   int dySrc  ){
        return FALSE;
      };

LRESULT CALLBACK SC_capVideoStreamCallback( HWND hwnd, LPVIDEOHDR lpVHdr){
TCamera*cam=(TCamera*)capGetUserData(hwnd);
 if(cam){
  return cam->HandleStream(lpVHdr);
  UpdateWindow(scmainhwnd);
 }else{
  capCaptureStop(hwnd);
 }
 return TRUE;
};


TCamera::TCamera(char *_info):TVideo(VIDEOCAMERA,sizeof(TCamera)){
 hwnd=0;
 if(capconnectfiled)return;
 InitTitle("avicap");
 BOOL _auto=FALSE;
 fps=15;
 if(!hCapDll){
#ifdef WIN32
 WriteTitleStr("Loading avicap.dll");
 hCapDll = LoadLibrary("avicap32.dll");
#else
 WriteTitleStr("Loading avicap.dll");
 hCapDll = LoadLibrary("avicap.dll");
#endif
  };

WriteTitleStr("Create capture window");
HWND (VFWAPI * capCreateWindow)(LPCSTR lpszWindowName,
                                DWORD dwStyle,
                                int x, int y, int nWidth, int nHeight,
                                HWND hwndParent, int nID);

(FARPROC)capCreateWindow=GetProcAddress(hCapDll,
#ifdef WIN32
    "capCreateCaptureWindowA"
#else
    "capCreateCaptureWindow"
#endif
   );
//Ok=FALSE;
info=NewStr(_info);
if(capCreateWindow){
POINT _size={160,120};
char *ps=info+lstrlen(camera_header);
while (*ps){
 if(*ps=='/'){
  ps++;
  if(((*(ps)=='x')||(*(ps)=='X'))&&(*(ps+1)=='=')){sscanf(ps+2,"%d",&(_size.x)); }
  if(((*(ps)=='y')||(*(ps)=='Y'))&&(*(ps+1)=='=')){sscanf(ps+2,"%d",&(_size.y)); }
  if(!strnicmp(ps,"fps=",4)){
                          char *_ps=ps+4;
                          while (*_ps && (((*_ps)>='0' && (*_ps)<'9')||
                                         (*_ps=='.') ||(*_ps=='e')||(*_ps=='E'))
                                )_ps++;
                          char ss[256];
                          lstrcpyn(ss,ps+4,(_ps-ps)-3);
                          double _fps;
                          if(StrToDouble(ss,_fps))fps=_fps;
              }
  if(!strnicmp(ps,"auto",4))_auto=TRUE;
 }
 ps++;
}
if(fps<=0)fps=15;
framesize=_size;
POINT offset={0,0};
#ifdef CAPFRAME
offset.x=(-_size.x)+1;
offset.y=(-_size.y)+1;
#endif
hwnd =  capCreateWindow ( "Stratum video",
	WS_CHILD
#ifdef CAPFRAME
	|WS_VISIBLE
#endif
     ,offset.x,offset.y, _size.x,_size.y, scmainhwnd,0);
WriteTitleStr("Connecting to driver");

if(!hwnd){
DoneTitle();
 MessageBox(scmainhwnd,
 "Cannot create video capture window\nYou should correct errors in the configuration of videocapture card, restart STRATUM 2000 and repeat Your attempt.","Video for stratum",MB_OK|MB_ICONSTOP);
 capconnectfiled=1;
}else{
_ok=_connect=capDriverConnect (hwnd, 0);
if(!_connect){
 DoneTitle();
 MessageBox(scmainhwnd,"Cannot connect to video capture driver\nYou should correct errors in the configuration of videocapture card, restart STRATUM 2000 and repeat Your attempt.","Video for stratum",MB_OK|MB_ICONSTOP);
 capconnectfiled=1;
}
 capOverlay(hwnd,1);
 capPreviewScale(hwnd,1);
 capDriverGetName(hwnd,name,sizeof(name));
 WriteTitleStr(name);
/*
#ifdef WIN32
 char s[256];
 lstrcpy(s,name);
 for(int i=0;i<6;i++){
  Sleep(100);
  lstrcat(s,".");
  WriteTitleStr(s);
 }
 Sleep(250);
#endif
*/
 capDriverGetCaps(hwnd,&capdrivercaps,sizeof(capdrivercaps));
 ghdd = DrawDibOpen();
 capSetUserData(hwnd,this);
}}
 // ok=hwnd!=NULL;
DoneTitle();
if(_auto){
Capture(1);
 UpdatePlaying();
}
};

 TCamera::~TCamera(){
   Capture(FALSE);
   capSetUserData(hwnd,NULL);
   if(info)delete info;
   if(hwnd){
            if(_connect)capDriverDisconnect(hwnd);
            DestroyWindow(hwnd);
           }
   if(formatinfo){delete formatinfo;formatinfo=NULL;}
   if(ghdd)DrawDibClose(ghdd);

 };

char*TCamera::GetFileName(BOOL fullname,char*fn,HSP2D hsp){
   //lstrcpy(fn,info);
   return info;
};

BOOL TCamera::Dialog(HWND _hwnd)
{
  capEditCopy(_hwnd);
  // capGrabFrameNoStop(hwnd);
  return capDlgVideoSource(hwnd);
};

BOOL TCamera::Capture(BOOL _play){

 if(playing)
 {
#ifdef CAPFRAME
 capPreview(hwnd,0);
#else
//capCaptureStop(hwnd);
 capCaptureAbort(hwnd);
#endif
 while(inprocess){
#ifdef WIN32
 Sleep(50);
#else
  Yield();
#endif
 }
   playvideos->Delete(this);
   playing=FALSE;
   _videohandled=FALSE;
   capSetCallbackOnVideoStream(hwnd,NULL);
   capSetCallbackOnFrame(hwnd,NULL);
   if(block){GlobalUnlock(block);block=NULL;}
   if(formatinfo){delete formatinfo;formatinfo=NULL;}
   pause=TRUE;
 }

 if(formatinfo){delete formatinfo;formatinfo=NULL;}

 if(_play)
 {

   formatsize=capGetVideoFormatSize(hwnd);
   if(formatsize){
   formatinfo = (LPBITMAPINFOHEADER)new BYTE[formatsize];
   capGetVideoFormat(hwnd,formatinfo,formatsize);
   if((formatinfo->biWidth != framesize.x) || (formatinfo->biHeight != framesize.y) ||
     ((formatinfo->biBitCount!=8)&&(formatinfo->biBitCount!=24)))
    {
      formatinfo->biWidth  = framesize.x;
      formatinfo->biHeight = framesize.y;
      if((formatinfo->biBitCount!=8)||(formatinfo->biBitCount!=24)){
      formatinfo->biBitCount=24;
      }
      if(!capSetVideoFormat(hwnd,formatinfo,formatsize)){
       capGetVideoFormat(hwnd,formatinfo,formatsize);
       char s[256];
       wsprintf(s,"Video error: capSetVideoFormat failed (%d bit per pixel not supported in textures). Use manual settings.",formatinfo->biBitCount);
       SCMessage(s);
      }
    }
   playvideos->Insert(this);

#ifdef CAPFRAME
  _videohandled= capSetCallbackOnFrame(hwnd,SC_capVideoStreamCallback  );
   DWORD _fps=1000.0*(1.0/fps);
   capPreviewRate(hwnd,_fps);
   capPreview(hwnd,1 );
#else
_videohandled=capSetCallbackOnVideoStream(hwnd,SC_capVideoStreamCallback);
   CAPTUREPARMS c;
   if(capCaptureGetSetup(hwnd,&c,sizeof(c)))
	{
     c.dwRequestMicroSecPerFrame=1e6*(1.0/fps);
	  c.fYield=TRUE;
	  c.fAbortLeftMouse=FALSE;
	  c.fAbortRightMouse=FALSE;
	  c.fCaptureAudio=FALSE;
	  capCaptureSetSetup(hwnd,&c,sizeof(c));
	}
if(!capCaptureSequenceNoFile(hwnd)){
  MessageBox(scmainhwnd,"Video capture driver cann't get frame sequence!","Video for stratum",MB_OK|MB_ICONSTOP);
}

#endif
   playing=TRUE;
   pause=FALSE;
   } // size == 0
 }

};

LPBITMAPINFOHEADER TCamera::GetFrame(LONG*)
   {
	  return image;
   }

BOOL TCamera::DrawFrame(HDC hdc, int xDst, int yDst,int dxDst, int dyDst,
     LONG *pos,int xSrc,int ySrc, int dxSrc,   int dySrc  )
   {
     if(image){
  return	DrawDibDraw(ghdd,hdc,
		 xDst, yDst,dxDst,dyDst,
		 GetFrame(pos), NULL,
		 xSrc,ySrc, // SRC ORG
		 dxSrc,dySrc, // SRC SIZE
		 DDF_BACKGROUNDPAL
		 );
     }else return FALSE;

   };
   
long ScanBytes(int pixWidth, int bitsPixel);
long NColors(WORD bitCount);

BOOL TCamera::HandleStream(LPVIDEOHDR vh){
if(inprocess)return FALSE;
  inprocess=TRUE;
  if((!block)&&(formatinfo)){
  	int perline=ScanBytes(formatinfo->biWidth,formatinfo->biBitCount);
   allocbytes=perline*formatinfo->biHeight;
   DWORD _size=formatsize+allocbytes;
   block=GlobalAlloc(GMEM_MOVEABLE,_size);
   image=(LPBITMAPINFOHEADER)GlobalLock(block);
   memcpy(image,formatinfo,formatsize);
  }

  DWORD tocopy=allocbytes;
  if(vh->dwBufferLength<tocopy)tocopy=vh->dwBufferLength;

#ifdef WIN32
	 memcpy(
#else
	 hmemcpy(
#endif
  ((BYTE*)image)+formatsize,vh->lpData,tocopy);

  updated=TRUE;
  inprocess=FALSE;

};
HWND title=0;
char * _tn="avicap";
HDC tdc=0;
INT16 bmpw,bmph;
HGDIOBJ tbrush=0;

void WriteTitleStr(char *s){
 PatBlt(tdc,bmpw/2,bmph-15,bmpw/2-2,14,PATCOPY);
 TextOut(tdc,bmpw/4+bmpw/2,bmph-15,s,lstrlen(s));
};

HWND InitTitle(char* _tn){

HBITMAP _title=LoadBitmap(hInstance,_tn);
BITMAP bm;

if(GetObject(_title,sizeof(bm),&bm)){
  bmpw=bm.bmWidth; bmph=bm.bmHeight;
  int wh=bmph;
  RECT r;
  GetClientRect(GetDesktopWindow(),&r);
  title=CreateWindowEx(0,
  "STATIC","Loader",WS_POPUP,
	  0,0,bm.bmWidth,wh,scmainhwnd,NULL,(HINSTANCE)hInstance,NULL);

  SetWindowPos(title,HWND_TOP	,(r.right-bm.bmWidth)/2,(r.bottom-wh)/2,bm.bmWidth,wh,
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
//  COLORREF rgb=GetPixel(tdc,bmpw+bmpw/2,bmph-15);
  SetTextColor(tdc,RGB(0,0,0));
  SetBkColor(tdc,RGB(255,255,255));
  tbrush=CreateSolidBrush(RGB(255,255,255));
  SelectObject(tdc,tbrush);
//  WriteTitleStr("Loading core...");
  return title;
}
return 0;
}
void DoneTitle(){
 if(title){
  ReleaseDC(title,tdc);
  DestroyWindow(title);
  DeleteObject(tbrush);
  title=0;
 }
}









