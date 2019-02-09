
/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			vengine.h
Description:
			internal header
Author:
			Alexander Shelemekhov
*/

#ifndef _SC3_VENGINE_H_
#define _SC3_VENGINE_H_

#include "video\collect.h"
#include "video\vid_type.h"

//#include <mmsystem.h>
//#include <msvideo.h>

#define MMNODRV
#define NOMCIWND  // no MCI Wnd
#define MMNOJOY   // no Joystick support
#define MMNOMIDI  // MIDI support

#ifdef WIN32
#define __MFC_COMPAT__
#include <vfw.h>
#else
//#include <mem.h>
//#include <mmsystem.h>
//#include <mmreg.h>
//#include <vfw\msvideo.h>
#include <vfw.h>
//#include <mmsystem.h>
#endif

//#include <mmreg.h>
//#include <avicap.h>


//#include <vfw\compman.h>
//#include <vfw\msvideo.h>
//#include <vfw\avicap.h>

typedef struct _RECT16 {    // rc
	INT16 left;
	INT16 top;
	INT16 right;
	INT16 bottom;
} RECT16 ;

typedef struct tagTexture3d{
 HSP3D hsp3d;
 HOBJ2D hMaterial;
}Texture3d;

typedef struct tagFrameSTREAM{
  WORD   version;
  RECT16 src;
  DWORD  flags;
  long   pos;     // позиция
  WORD   length;  // длинна строки
  DWORD  vflags;
  COLORREF color;
  BYTE   sense;
  char   video[1];
}FrameSTREAM;

 enum    VIDEO_MODE {VIDEONONE=0,VIDEOAVI=1,VIDEOWRITE=2,VIDEOCAMERA=3};

class TVideo{
public:

  VIDEO_MODE mode;         // must be first
  POINT    framesize;      // Размер картинки
  DWORD    flags;
  BYTE     pause;

  long     framestart;  // первый фрейм
  long     frameend;    // последний фрейм
  long     framecount;  // число фреймов
  long     playstart;   // начало проигрывания
  long     playend;     // конец проигрывания
  long     pos;         // текущая позиция
  BOOL     playing;
  BOOL     looping;

 long timeStart,timeEnd;
 DWORD glPlayStartTime;
 DWORD currentTime;

  TCollection * Frames;
  TCollection * Textures;
  TVideo(VIDEO_MODE m,int size);
  virtual ~TVideo();

  char virtual* GetFileName(BOOL fullname,char*buffer,HSP2D hsp=NULL);
  BOOL virtual IsUsed();
  long virtual SetPos(long)=0;
  long virtual GetPos(){return pos;};
  long virtual GetRate(){return 0;}; // return current rate
  virtual BOOL Ok()=0;
  LPBITMAPINFOHEADER virtual GetFrame(LONG*){return NULL;};// if NULL then current
  BOOL virtual DrawFrame(HDC hdc, int xDst, int yDst,int dxDst, int dyDst,
     LONG *pos,int xSrc,int ySrc, int dxSrc,   int dySrc  );

  LONG virtual SetFrame(LPBITMAPINFOHEADER){return -1;};//return write count
  BOOL UpdateViews(LONG*pos);
  BOOL virtual CanPlayAudio(){return FALSE;};
  void virtual SetPause(BOOL apause){pause=apause;};
  LONG virtual SampleToTime(LONG ){return 0;};
  LONG virtual TimeToSample(LONG ){return 0;};
  BOOL virtual Dialog(HWND){return FALSE;};
  BOOL virtual IsEqual(char*);
  BOOL GetMarker(VIDEOMARKER*);
};


class TWrite: public TVideo{
public:
  PAVIFILE		      pfile;
  PAVISTREAM	      ps, psCompressed, psText;
  HGLOBAL            hbuffer;
  LPBITMAPINFOHEADER image;
  COMPVARS           cmp;// параметры компрессии
 char * filename;
 HSP2D  hsp;
 RECT   rect;
 int    count;
 TWrite(HSP2D _hsp,char*file,int x,int y,int cx,int cy,char*);
 ~TWrite();
 char   *GetFileName(BOOL fullname,char*,HSP2D hsp=NULL);
 int Write();
 BOOL _Init();
 BOOL virtual Ok(){return (pfile && ps && image);};
 long virtual SetPos(long){return 0;};
 long virtual GetPos(){return 0;};
 BOOL virtual Dialog(HWND);
};

class TCamera: public TVideo{
public:
  LPBITMAPINFOHEADER image;
  LPBITMAPINFOHEADER formatinfo;
  DWORD              formatsize;
  DWORD              allocbytes;
  HDRAWDIB           ghdd;
  HGLOBAL            block;
  BOOL               updated;
  char * info;
  char name[256];
  double fps;
  HWND hwnd;
//  POINT size;
  unsigned int _ok:1;
  unsigned int _connect:1;
  unsigned int _videohandled:1;
  int rate;
  LONG   inprocess;
  CAPDRIVERCAPS capdrivercaps;
 TCamera(char *info);
 ~TCamera();
  char   *GetFileName(BOOL fullname,char*,HSP2D hsp=NULL);
  BOOL virtual Ok(){return ((hwnd!=NULL) && (_ok) && (_connect));};
  long virtual SetPos(long){return 0;};
  long virtual GetPos(){return 0;};
  BOOL virtual Dialog(HWND);
  BOOL         Capture(BOOL _play); // <-= S_T_A_R_T  o r S_T_O_P capture =->

  LPBITMAPINFOHEADER virtual GetFrame(LONG*);// if NULL then current
  BOOL virtual DrawFrame(HDC hdc, int xDst, int yDst,int dxDst, int dyDst,
     LONG *pos,int xSrc,int ySrc, int dxSrc,   int dySrc  );
  BOOL  HandleStream(LPVIDEOHDR);
  BOOL virtual IsEqual(char*);
};

class TAviVideo:public TVideo{
public:
 char * file;
PAVISTREAM           * gapavi;	// the current streams
AVICOMPRESSOPTIONS   * gaAVIOptions;// compression options
LPAVICOMPRESSOPTIONS * galpAVIOptions;
PGETFRAME	         * gapgf;	// data for decompressing video
HDRAWDIB	            * ghdd;	// drawdib handles

PAVISTREAM          gpaviAudio;                 // 1st audio stream found
PAVISTREAM          gpaviVideo;                 // 1st video stream found
int                 VideoStream;                // 1st video stream number found

 BYTE   audioChannels;
 DWORD  audioSamplesPerSec;
 WORD   audioFormatTag;

 long     rate;
// int      handle;
 PAVIFILE	pfile;
 HRESULT	hr;
 int streamcount;
// TVideo(HSP2D,char *file);
 TAviVideo(char *file,DWORD flags);
 ~TAviVideo();
 TCollection * Frames;
 TCollection * Textures;
 BOOL GetSampleByTime(DWORD t,DWORD& s,DWORD &_glPlayStartTime,DWORD _start,DWORD _end,BOOL looping);
 BOOL GetTimeBySample(DWORD t,DWORD& s);
 long SetPos(long _pos);
 long  virtual GetRate();
 LPBITMAPINFOHEADER virtual GetFrame(LONG*);// if NULL then current
 BOOL virtual Ok(){return (gpaviVideo && file);};
 BOOL virtual DrawFrame(HDC hdc, int xDst, int yDst,int dxDst, int dyDst,
     LONG *pos,int xSrc,int ySrc, int dxSrc,   int dySrc  );
 BOOL virtual CanPlayAudio(){return ((gpaviAudio!=NULL)&&((flags&V2_NOAUDIO)==0));};

 void virtual SetPause(BOOL apause);
 LONG virtual SampleToTime(LONG );
 LONG virtual TimeToSample(LONG );
 char virtual* GetFileName(BOOL fullname,char*buffer,HSP2D hsp=NULL);
 BOOL virtual Dialog(HWND);
 protected:
 void InitStreams();

};

class TFrame{
public:
 long pos;
 DWORD flags;
 HSP2D hsp;
 HOBJ2D handle;
 RECT src;
 TVideo * video;
 char   * avifile;  // Имя видео на случай отсутствия при создания
 DWORD vflags;
 long  playstart;
 long  playend;
 BOOL  playing;
 BOOL  paused;
 BOOL  looping;
 long  rate;
 DWORD glPlayStartTime;
 COLORREF color;
 BYTE sense;
 TFrame(HSP2D,TVideo*,gr_float x,gr_float y,gr_float sizex,gr_float sizey,RECT *src,DWORD flags);
 TFrame(USEROBJSTRUCT*u,long size,void *);
 void  _Awaken(TVideo *v);
 void  BeginPlay(long start,long end,int rate,int flags);
 void  BeginAudio(long start,long end,DWORD flags);
 void  Pause();
 void  Resume();
 void  StopPlay(BOOL videostop=FALSE);
 long  SetPos(long _pos);
 ~TFrame();
 LRESULT Proc2d(USEROBJSTRUCT*u,UINT command,DWORD lParam);
 BOOL VideoConnect();
};

#define MAX_AUDIO_BUFFERS	16
#define MIN_AUDIO_BUFFERS	2
#define AUDIO_BUFFER_SIZE	16384

class TAudio{
public:
HWAVEOUT	shWaveOut;	/* Current MCI device ID */
LONG		slBegin;
LONG     slBegin1;
LONG		slCurrent;
LONG		slEnd;
LONG		slEnd1;
BOOL		sfLooping;
BOOL		sfPlaying;

WORD		swBuffers;	    // total # buffers
WORD		swBuffersOut;	    // buffers device has
WORD		swLoopOut;	    // buffers device has
WORD		swNextBuffer;	    // next buffer to fill
WORD     swLoopBuffer;
LPWAVEHDR	salpAudioBuf[MAX_AUDIO_BUFFERS];

PAVISTREAM	spavi;		    // stream we're playing
LONG		slSampleSize;	    // size of an audio sample

LONG		sdwBytesPerSec;
LONG		sdwSamplesPerSec;

BOOL     paused;
BOOL     wasloop;
BOOL     failed;
DWORD    glPlayStartTime;
BOOL     playwasstarted;
TVideo * video;
TFrame * frame;
TAudio(HWND hwnd, PAVISTREAM pavi, LONG lStart, LONG lEnd, BOOL fWait);
~TAudio();
BOOL aviaudioPlay(HWND hwnd, PAVISTREAM pavi, LONG lStart, LONG lEnd, BOOL fWait);
BOOL aviaudioiFillBuffers();
LONG aviaudioTime();
BOOL aviaudioOpenDevice(HWND hwnd, PAVISTREAM pavi);
void aviaudioCloseDevice();
void aviaudioStop();
void Pause();
void Resume();
void BeginPlay();
};

char * NewStr(char * astr);
TFrame * GetFrame(HSP2D hsp,HOBJ2D obj);
BOOL InserVideo(TVideo *v,HV2D&);
TVideo * GetVideo(int hv,VIDEO_MODE vm=VIDEOAVI);
void UpdatePlaying();
BOOL  InitVideo();
void DoneVideo();
int  _Error(char *);
void PauseAll(BOOL);

TCollection extern * videos;
TCollection extern * frames;
TCollection extern * playvideos;
TCollection extern * playframes;
BOOL extern paused;
BOOL extern * playing;
BOOL extern exiting;
BOOL extern doned;
BOOL extern waspaused;
DWORD extern timedelta;
DWORD extern pausestarttime;
HWND extern notifywnd;
HWND extern scmainhwnd;
HWND extern capwnd;
HBRUSH extern BGrayed;
HPALETTE  extern palette;
TAudio  extern *audio;
HINSTANCE extern hInstance;
HINSTANCE extern hCapDll;

BOOL extern soundpresent;

HGLOBAL _CreateDIB2d(int sizex,int sizey,int bitCount,COLORREF bkcolor);
HRESULT AVIError(HRESULT r);

void * GlobalAllocPtr(long ,long lTemp);
void GlobalFreePtr(void * p);

VIDEO_MODE GetVideoType(char * file);
TVideo*    CreateVideo(char * file,UINT16 flags,HV2D *index=NULL);

void DoneTitle();
HWND InitTitle(char *_tn);
void WriteTitleStr(char *s);

#define MAXNUMSTREAMS	25
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif
