
#include <gwin2d3d\gwinfunc.h>
//#include <mmsystem.h>
#include <mem.h>                 
//#include <vfw\avifile.h>
//#include <vfw\drawdib.h>
//#include <vfw\compddk.h>
#include "video\scvideo.rh"

#include "video\vengine.h"
#include "video\vid_type.h"
#include "graphic\_g_def.h"
#include "msgcrk.h"
#include <stratum\stratum.h>
#include <stratum\sc_msg.h>

#ifndef WIN32
#include <string.h>
#endif

#define camera_header "$CAMERA_SOURCE:"

long ScanBytes(int pixWidth, int bitsPixel);
VIDEO_MODE GetVideoType(char * file){
if( !strnicmp(file,camera_header,lstrlen(camera_header)) ){
  return VIDEOCAMERA;
 }else return VIDEOAVI;
};
BYTE extern capconnectfiled;

TVideo* CreateVideo(char * file,UINT16 flags,HV2D *index)
{
#ifdef LOGON
 	LogMessage("CreateVideo called");
#endif
	TVideo * video=NULL;
 	VIDEO_MODE t=GetVideoType(file);
 	if(index)*index=0;
 	switch(t){
  case VIDEOCAMERA:
#ifdef LOGON
 	LogMessage("  Type: VIDEOCAMERA");
#endif
  	if(!capconnectfiled){video=new TCamera(file);}break;
  case VIDEOAVI:
#ifdef LOGON
 	LogMessage("  Type: VIDEOAVI");
#endif
  	video=new TAviVideo(file,flags);
 	};

#ifdef LOGON
  if(video)
	 	LogMessage("  Video created");
  else
	 	LogMessage("  Video NOT created");
#endif

 	if(video)
  {
 		if (video->Ok())
    {
#ifdef LOGON
	 	LogMessage("  Video Ok");
#endif
  		HV2D hv;
  		InserVideo(video,hv);
  		if(index)*index=hv;
 		}else{
#ifdef LOGON
	 	LogMessage("  Video NOT Ok");
#endif
  		delete video;
  		video=0;
 		}
  }
 	return video;
};

extern "C" HV2D _EXPORT OpenVideo(char far*file,UINT16 flags)
{
 HV2D hv=0;
 TVideo * video=CreateVideo(file,flags,&hv);
 return hv;
};

extern "C" BOOL _EXPORT CloseVideo(HV2D hv){
TVideo *v = GetVideo(hv,0);
 if(v->IsUsed())return FALSE;
 if (v){
  videos->AtPut(videos->IndexOf(v),NULL);
  delete v;
  return TRUE;
 }
 return FALSE;
};

extern "C" _EXPORT HV2D BeginWriteVideo2d(HSP2D _hsp,char*file,int x,int y,int cx,int cy,char*f){
  TWrite * w=new TWrite(_hsp,file,x,y,cx,cy,f);
  if(w->Ok()){
	HV2D hv=0;
   InserVideo((TVideo*)w,hv);
   return hv;
  }else delete w;
 return 0;
};

extern "C" _EXPORT HV2D  WriteVideoFrame2d(HV2D hv){
TVideo *v = GetVideo(hv,0);
if(v && (v->mode==VIDEOWRITE)){
 TWrite *w=(TWrite *)v;
  return w->Write();
 }
 return 0;
};

extern "C" BOOL _EXPORT InitVideo2d(VIDEOINIT*vi){
  notifywnd=vi->hwnd;
  playing=vi->playing;
  return 1;
};

extern "C" void _EXPORT DoneVideo2d(){
	DoneVideo();
	doned=1;
};


extern "C" HOBJ2D  _EXPORT CreateVideoFrame2d(HSP2D hsp,HV2D hv,gr_float x,gr_float y,gr_float sizex,gr_float sizey,RECT * src,DWORD flags){
 TVideo *v = GetVideo(hv,0);
 if (v && ((v->mode==VIDEOAVI)||(v->mode==VIDEOCAMERA))){
	TFrame*f=new TFrame(hsp,v,x,y,sizex,sizey,src,flags);
   v->Frames->Insert(f);
   if(f->handle){
	return f->handle;
   }else{
	if (f->video)f->video->Frames->Delete(f);
	delete f;
	return 0;
   }
 }
 return 0;
};


extern "C" BOOL _EXPORT  VideoSetPos(HV2D hv,long pos){
TVideo *v = GetVideo(hv);
if (v){v->SetPos(pos);
  return TRUE;
 }
 return 0;
};
extern "C" BOOL _EXPORT GetVideoProp(HV2D hv,VIDEOPROP *vp){
TVideo *v = GetVideo(hv,0);
memset(vp,0,sizeof(VIDEOPROP));
if (v){
 vp->type=v->mode;
 vp->sizex=(INT16)v->framesize.x;
 vp->sizey=(INT16)v->framesize.y;
 vp->start=v->framestart;
 vp->end=v->frameend;
 vp->paused=v->pause;
 vp->timestart=v->timeStart;
 vp->timeend=v->timeEnd;
 vp->pos=v->pos;
 if(v->mode==VIDEOAVI){
 TAviVideo*_v=(TAviVideo*)v;
if (_v->gpaviAudio){
 vp->audioChannels=_v->audioChannels;
 vp->audioSamplesPerSec=_v->audioSamplesPerSec;
 vp->audioFormatTag=_v->audioFormatTag;
}else {
 vp->audioChannels=0;
 vp->audioSamplesPerSec=0;
 vp->audioFormatTag=0;
 }}
char buffer[260];
char*_ps= v->GetFileName(TRUE,buffer);
  if(_ps){
	lstrcpy(vp->file,_ps);
  }
return TRUE;
}
return FALSE;
};
extern "C" BOOL _EXPORT FrameSetPos2d(HSP2D hsp,HOBJ2D obj,long pos){
 TFrame * f=GetFrame( hsp,obj);
 if (f&& f->video){
  f->SetPos(pos);
  return TRUE;
 }
 return FALSE;
};
extern "C" BOOL _EXPORT GetFrameProp2d(HSP2D hsp,HOBJ2D obj,FRAMEPROP*fp){
 TFrame * f=GetFrame( hsp,obj);
 if (f){
 // && f->video
 if (fp){
  if(!(f->video))f->VideoConnect();
  fp->pos=f->pos;
  fp->video=videos->IndexOf(f->video);
  if (fp->video>-1)fp->video+=(INT16)4096;else fp->video=0;
  fp->src=f->src;
  fp->flags=f->flags;
  fp->color=f->color;
  fp->sense=f->sense;
  if (f->flags&V2_SYNC && f->video){
	 fp->playing=f->video->playing;
	 fp->paused=f->video->pause;
	}else{
	  fp->playing=f->playing;
	  fp->paused=f->paused;
	}
  }
  return TRUE;
 }
 return FALSE;
};
extern "C" BOOL _EXPORT SetFrameProp2d(HSP2D hsp,HOBJ2D obj,FRAMEPROP*fp){
 TFrame * f=GetFrame( hsp,obj);
 if (f){
 BOOL u=FALSE;
 if (f->video != GetVideo(fp->video,0)){
 	   if (f->video)f->video->Frames->Delete(f);
	  	else frames->Delete(f);
		if (f->video && f->video->Frames->count==0 && f->video->flags&V2_AUTODELETE &&
              (f->video->Textures==NULL || (f->video->Textures->count==0))){
               videos->AtPut(videos->IndexOf(f->video),NULL);
             delete f->video;
		}
     u=TRUE;
     f->video=GetVideo(fp->video,0);
     f->video->Frames->Insert(f);
  }

  if (u || f->src.left!=fp->src.left ||
		f->src.right!=fp->src.right ||
		f->src.top!=fp->src.top ||
		f->src.bottom!=fp->src.bottom ||
		((f->flags&V2_TRANSPARENT) != (fp->flags&V2_TRANSPARENT))
		|| (fp->flags&V2_TRANSPARENT && (f->color!=fp->color))
      ||(  fp->sense!=f->sense)

		){
		f->src=fp->src;
		if (f->src.left<0)f->src.left=0;
		if (f->src.top<0)f->src.top=0;

	  if (f->video){
		if (f->src.right>f->video->framesize.x ||
			 f->src.right<0  )f->src.right=f->video->framesize.x;
		if (f->src.bottom>f->video->framesize.y ||
			 f->src.bottom<0	)f->src.bottom=f->video->framesize.y;
	  }
	  UpdateObject2d(f->hsp,f->handle,NULL);

	  }
     SetSpaceModify2d(f->hsp,1);
	 f->color=fp->color;
	 f->flags=fp->flags;
    f->sense=fp->sense;
	 return TRUE;
	}
 return FALSE;
};

extern "C" BOOL _EXPORT  VideoPlay2d(HSP2D hsp,HOBJ2D obj,long start,long end,long rate,UINT32 flags){
 TFrame * f=GetFrame( hsp,obj);
 BOOL rez=FALSE;
 if (f && f->video){
  if (end<0)end=0x7FFFFFFFl;
   f->BeginPlay(start,end,rate,(UINT16)flags);
  rez=TRUE;
 }
UpdatePlaying();
 return rez;
};


extern "C" BOOL _EXPORT  VideoPause2d(HSP2D hsp ,HOBJ2D obj){
 TFrame * f=GetFrame( hsp,obj);
 if (f){f->Pause();
 UpdatePlaying();
 return TRUE;
 }return FALSE;
};

extern "C" BOOL _EXPORT  VideoResume2d(HSP2D hsp,HOBJ2D obj){
 TFrame * f=GetFrame( hsp,obj);
 if (f){
     if(f->video && f->video->mode==VIDEOCAMERA){
      f->BeginPlay(0,1,15,0);
     }else{
	  f->Resume();
     }
	  UpdatePlaying();
	  return TRUE;
	 }return FALSE;
};

extern "C" BOOL _EXPORT  VideoStop2d(HSP2D hsp,HOBJ2D obj){
if (!hsp && !obj){
 // Full Stop
 if (audio){ delete audio; audio=NULL; }
}else{
 TFrame * f=GetFrame( hsp,obj);
 if (f)f->StopPlay(FALSE);
 }
UpdatePlaying();
return TRUE;
};
DWORD timedelta=0;
extern "C" BOOL _EXPORT VideoHandle(){
if (paused)return 1;
static long _l=0;
 if (audio){
  audio->BeginPlay();
  TVideo * video=audio->video;
	  long time=audio->aviaudioTime();


	  if (audio->failed){
		TVideo *video=audio->video;
		TFrame* frame=audio->frame;
		delete audio;audio=0;
		if (frame || video){
		 if (video || (frame && frame->flags&V2_SYNC)){
		 //video->rate=video->GetRate(0);
		 playvideos->Insert(video);}else{
		  frame->rate=video->GetRate();
		  playframes->Insert(frame);
			}
		 }
	  }else{

		if ((time >=video->timeEnd && !audio->sfLooping)	|| (time<0) )
		{delete audio;audio=0;
			UpdatePlaying();
		 }else
		 {
//		 long frame=AVIStreamTimeToSample(audio->video->gpaviVideo,time);
      long frame=audio->video->TimeToSample(time);
		if (frame<video->framestart)frame=video->framestart;
		if (frame>video->frameend)frame=video->framestart;
		if (audio->video)audio->video->SetPos(frame);
		if (audio->frame)audio->frame->SetPos(frame);
		 }
	  }
	 }
	DWORD l=timeGetTime();
	l-=timedelta;
	int i=0;
	while (i<playvideos->count){
	 TVideo*v=(TVideo*)playvideos->At(i);


    if(v->playing && !v->pause){
     if(v->mode==VIDEOAVI){
       TAviVideo*_v=(TAviVideo*)v;
		 DWORD sample;
		 if(_v->GetSampleByTime(l,sample,_v->glPlayStartTime,_v->playstart,_v->playend,_v->looping)){i++;if (l!=_v->pos)v->SetPos(sample);	}
		    else {playvideos->Delete(v);v->playing=0;UpdatePlaying();}
      }else
      if(v->mode==VIDEOCAMERA){
        TCamera*_c=(TCamera*)v;
        if(_c->updated)
          {
           _c->UpdateViews(NULL);
          }
         i++;
      }else i++;
	 }else i++;
	}
  i=0;
  while (i<playframes->count){

	TFrame*f=(TFrame*)playframes->At(i);

	if (f->playing && !f->paused){
	DWORD sample;
   if(f->video->mode==VIDEOAVI){
	if(((TAviVideo*)(f->video))->GetSampleByTime(l,sample,f->glPlayStartTime,f->playstart,f->playend,((f->flags&V2_CYCLED)!=0)|f->looping))
		      {
              if (l!=f->pos)f->SetPos(sample);
            }
       else {
              playframes->Delete(f);f->playing=0;
			     UpdatePlaying();
              continue;
			   }
    }
	}
   i++;
  }
 return FALSE;
};
HWND notifywnd=0;
TVideo *videoview=NULL;
BOOL CALLBACK VideoInfoProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM){
switch(msg){
	case WM_INITDIALOG:{
    char buffer[256];
	 SetDlgItemText(hwnd,IDC_FILENAME,videoview->GetFileName(TRUE,buffer));
	 char s[255];
	 wsprintf(s,"%ld",videoview->framestart);
	 SetDlgItemText(hwnd,IDC_FS,s);
	 wsprintf(s,"%ld",videoview->frameend);
	 SetDlgItemText(hwnd,IDC_FE,s);
	 wsprintf(s,"%ld (ms)",videoview->timeStart);
	 SetDlgItemText(hwnd,IDC_TS,s);
	 wsprintf(s,"%ld (ms)",videoview->timeEnd);
	 SetDlgItemText(hwnd,IDC_TE,s);
	 wsprintf(s,"%dx%d ",
			videoview->framesize.x,videoview->framesize.y);
	 SetDlgItemText(hwnd,IDC_VIDEOFORMAT,s);
    lstrcpy(s,"No audio channel");
    if(videoview->mode==VIDEOAVI){
    TAviVideo*video=(TAviVideo*)videoview;
	 if (video->gpaviAudio){
	 wsprintf(s, "%ldx Hz", video->audioSamplesPerSec);
		if (video->audioChannels==1)lstrcat(s," Mono");
		if (video->audioChannels==2)lstrcat(s," Stereo");
		}
    }

	 SetDlgItemText(hwnd,IDC_AUDIOFORMAT,s);

	 return TRUE;
	};
	case WM_COMMAND:{
    EV_COMMANDsimply;
	 switch (command){
		case IDOK:{
		  EndDialog(hwnd,1);
		return TRUE;
	  }
	 case IDCANCEL: EndDialog(hwnd,0);return TRUE;
	}
  }break;
}
return FALSE;
};

BOOL TAviVideo::Dialog(HWND hwnd){
  return DialogBox(hInstance,MAKEINTRESOURCE(VIDEO_DIALOG),hwnd,VideoInfoProc);
}
void StopAll(){};
extern "C" LRESULT _EXPORT aviaudioMessage(HWND hwnd, unsigned msg, LONG wParam, LONG lParam)
{switch (msg){
 case WM_SCV_GETMARKER:{
   VIDEOMARKER*vp=(VIDEOMARKER*)wParam;
    TVideo *v=NULL;
   if(vp->hv2d){v=GetVideo(vp->hv2d,0);}
   else {
    TFrame * f=GetFrame(vp->hsp,vp->hframe);
    if(f)v=f->video;
   }
   if(v){
   return v->GetMarker(vp);
   }
 }return FALSE;
 case WM_SCV_GETPIXEL:{
   VIDEOPIXEL*vp=(VIDEOPIXEL*)wParam;
   POINT2D org,size;
   if(GetObjectOrgSize2d(vp->hsp,vp->hframe,&org,&size)){
    if ((org.x<=vp->point.x)&&(org.y<=vp->point.y)&&
        (org.x+size.x>=vp->point.x)&&(org.y+size.y>=vp->point.y)){
          TFrame * f=GetFrame(vp->hsp,vp->hframe);
          if(f && f->video){
            LPBITMAPINFOHEADER lpb= f->video->GetFrame(NULL);
            if(lpb){
              int x=(vp->point.x-org.x)-f->src.left;
              int y=(vp->point.y-org.y)-f->src.top;
              int perline=ScanBytes(lpb->biWidth,lpb->biBitCount);
              BYTE*_b=((BYTE*)lpb)+sizeof(BITMAPINFOHEADER)+perline*(lpb->biHeight-y);
              switch(lpb->biBitCount){
               case 24:vp->color=(*((COLORREF*)(_b+3*x)))&0xfffffful;break;
               default:return FALSE;
              }
              return TRUE;
            }}}}
 }return FALSE;
 case WM_SCV_canVIDEOCLOSEALL:{
  for(C_TYPE i=0;i<videos->count;i++){
  TVideo*v=(TVideo*)videos->At(i);
   if (v && (!(v->IsUsed())) ){
	return TRUE;
   }
  }
  return FALSE;
 }
 case WM_SCV_VIDEOCLOSEALL:{
  C_TYPE j=0;
  for(C_TYPE i=0;i<videos->count;i++){
	TVideo*v=(TVideo*)videos->At(i);
    if (v && (!(v->IsUsed())))
     {
	    videos->AtPut(i,NULL);
	    j++;
       delete v;
     }
  }
   return j;
 }break;
 case WM_SCV_VIDEODIALOG:{
	videoview=GetVideo(lParam,0);
	if (videoview){return videoview->Dialog((HWND)wParam);
	}videoview=0;
 }break;
 case WM_SCV_GETNEXTVIDEO:{
 TVideo *v;
  BOOL mr=(wParam==0);
	for(C_TYPE i=0;i<videos->count;i++){
	 v=(TVideo*)videos->At(i);
	 if (v){
		if (mr)return i+4096;
		if (i+4096==wParam)mr=TRUE;
	 }
	}return 0;
  }

 case WM_SCV_GETNEXTFRAME:break;
 case WM_SCV_STOPALL:StopAll();break;
 case WM_SCV_PAUSEALL:PauseAll(wParam);break;
 case MM_WOM_DONE:if (audio){
	if ((((WPARAM)audio->shWaveOut)==wParam) && audio->playwasstarted){
	audio->swBuffersOut--;
	audio->aviaudioiFillBuffers();
	}
  }break;
 }
 return 0;
};

#if defined(WIN32)
//#pragma argsused
BOOL WINAPI DllEntryPoint( HINSTANCE hinstDll,
									DWORD fdwRreason,
									LPVOID plvReserved)
#else /* not flat model  */
int FAR _pascal LibMain( HINSTANCE hInstance,
								WORD ,
								WORD wHeapSize,
								LPSTR  )
#endif /* __FLAT */
{
#ifdef  WIN32

switch(fdwRreason){
 case DLL_PROCESS_ATTACH:{
	 ::hInstance=hinstDll;
    return InitVideo();// Indicate that the DLL was initialized successfully.
 }
 case DLL_PROCESS_DETACH:{
   exiting=1;
   if (!doned)DoneVideo();
	return TRUE;
 }
}
 return FALSE;
#else
 if ( wHeapSize != 0 ) UnlockData( 0 );
	 ::hInstance=hInstance;

 return InitVideo();// Indicate that the DLL was initialized successfully.
#endif

}
#if !defined(WIN32)
int CALLBACK WEP(int nExitType){
//if (nExitType==WEP_FREE_DLL)
 exiting=1;
 if (!doned)DoneVideo();
 return 1;
}
#endif

HWND      scmainhwnd=0;
HINSTANCE hInstance=0;
HINSTANCE hCapDll=0;
HBRUSH BGrayed;
BOOL exiting=0;
BOOL doned=0;
BOOL waspaused=0;

long ScanBytes(int pixWidth, int bitsPixel) {
  return (((long)pixWidth*bitsPixel+31) / 32) * 4;
}
long NColors(WORD bitCount)
{
  if (bitCount == 1 || bitCount == 4 || bitCount == 8)
	 return 1 << bitCount;
  else if (bitCount == 24)
	 return 0;
  return -1;
}


HGLOBAL _CreateDIB2d(int sizex,int sizey,int bitCount,COLORREF bkcolor){
void * bits;

   switch(bitCount){
	 case 1:
	case 4:
	case 8:
	case 24:break;

	default: return 0;
  };

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
	 switch(bitCount){
	 case 1:{COLORREF cr[2]={RGB(255,255,255),RGB(0,0,0)};
			  memcpy(colortable,cr,8);
			  bits=colortable+8;
			  }break;
	 case 4:memcpy(colortable,palette,16*4);
			  bits=colortable+16*4;
			  break;
	 case 8:{COLORREF *p=new COLORREF[256];
			  memset(p,0xff,256*4);
			  memcpy(p,palette,16*4);
			  int f=0;
			  for(int r=0;r<6;r++)
				for(int g=0;g<6;g++)
				 for(int b=0;b<6;b++){
				  COLORREF c=RGB(255*r/5,255*g/5,255*b/5);
				  BOOL was=FALSE;
				  for(int i=0;i<16;i++){
					if (p[i]==c){was=TRUE;break;}
				  }
				  if(!was){p[f]=c;f++;}
				 }
				for(int i=1;i<26;i++){
				 COLORREF c=RGB(255*i/26,255*i/26,255*i/26);
             p[f]=c;f++;
				}
			  memcpy(colortable,p,256*4);
			  bits=colortable+256*4;
			  delete p;
			  }
			  break;
	case 24:bits=colortable;
			  break;
	}
  BYTE fon=GetRValue(bkcolor);
  memset(bits,fon,(int)sizeimage);
	GlobalUnlock(block);
  return block;
};

extern "C" UINT32 _SDKPROC ScDllProc(UINT16 code,UINT32 p1,UINT32 p2){
 switch(code){
  case SC_DONELIBRARY:{
      DoneVideo2d();
  }return TRUE;
  case SC_INITLIBRARY:{
			TSTRATUMINFO*sci=(TSTRATUMINFO*)p1;
	  		scmainhwnd=sci->hMainWindow;
					  }return TRUE;
 }
return 0;
};
