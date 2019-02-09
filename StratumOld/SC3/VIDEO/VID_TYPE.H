/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			vid_type.h
Author:
			Alexander Shelemekhov
*/

#ifndef _SC3_VIT_H_
#define _SC3_VIT_H_

typedef INT16 HV2D;

typedef struct tagVIDEOPROP{
 INT16 sizex;
 INT16 sizey;
 BOOL  paused;

 WORD  type;

 DWORD start;
 DWORD end;
 DWORD timestart;
 DWORD timeend;
 DWORD pos;

 BYTE   audioChannels;
 DWORD  audioSamplesPerSec;
 WORD   audioFormatTag;
 char   file[255];
}VIDEOPROP;

typedef struct tagFRAMEPROP{
  RECT  src;
  HV2D  video;
  long  pos;
  DWORD flags;
  BYTE  sense; // чуствительность к цвету
  BOOL  playing;
  BOOL  paused;
  COLORREF color;
}FRAMEPROP;

typedef struct tagVIDEOINIT{
  HWND hwnd;
  BOOL far* playing;
}VIDEOINIT;

typedef struct tagVIDEOPIXEL{
  HSP2D    hsp;
  HOBJ2D   hframe;
  POINT2D  point;
  COLORREF color;
}VIDEOPIXEL;

typedef struct tagVIDEOMARKER{
  HSP2D    hsp;
  HOBJ2D   hframe;
  HV2D     hv2d;
  POINT2D  point;
  COLORREF color;      // Наиболее предпочтительный цвет
  COLORREF color_from;
  COLORREF color_to;
}VIDEOMARKER;


#define V2_SYNC        1
#define V2_DISABLE     2
#define V2_LOCALBUFER  4
#define V2_CYCLED      8
#define V2_LOOPING     V2_CYCLED
#define V2_NOAUDIO     256
#define V2_AUTODELETE  16
#define V2_TRANSPARENT 32
#define V2_AUTOPLAY    64

#define PF_NOAUDIO   256
#define PF_CYCLED    512

#define WM_SCV_STOPALL  0x4000
#define WM_SCV_PAUSEALL 0x4001
#define WM_SCV_GETNEXTVIDEO 0x4002
#define WM_SCV_GETNEXTFRAME 0x4003
#define WM_SCV_VIDEODIALOG  0x4004
#define WM_SCV_VIDEOCLOSEALL 0x4005
#define WM_SCV_canVIDEOCLOSEALL 0x4006
#define WM_SCV_GETPIXEL         0x4007
#define WM_SCV_GETMARKER        0x4008
#endif