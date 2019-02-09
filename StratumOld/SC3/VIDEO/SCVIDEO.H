/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			scvideo.h
Description:
			interface header
Author:
			Alexander Shelemekhov
*/

#ifndef _SC3_VI_H_
#define _SC3_VI_H_

#define camera_header "$CAMERA_SOURCE:" //not redefine

#define GetVideoMarker2d(vm) aviaudioMessage(NULL,WM_SCV_GETMARKER,(LONG)vm,0)

#ifndef _SC3_VIT_H_
#include "video\vid_type.h"
#endif


BOOL VideoAvailable2d();
void PauseAll();
void ResumeAll();
HV2D GetNextVideo(HV2D);
BOOL VideoInfoDialog(HWND,HV2D);
void CloseAllVideos();
BOOL CanCloseAllVideos();

#ifdef VIDEO_LINK

extern "C" {
BOOL WINAPI InitVideo2d(VIDEOINIT*);
void WINAPI DoneVideo2d();

HV2D WINAPI OpenVideo(char far*,UINT16 flags);
BOOL WINAPI CloseVideo(HV2D);
BOOL WINAPI GetVideoProp(HV2D,VIDEOPROP*);
BOOL WINAPI GetFrameProp2d(HSP2D,HOBJ2D,FRAMEPROP*);
BOOL WINAPI SetFrameProp2d(HSP2D,HOBJ2D,FRAMEPROP*);

HOBJ2D WINAPI CreateVideoFrame2d(HSP2D,HV2D ,gr_float x,gr_float y,gr_float sizex,gr_float sizey,RECT * src,DWORD flags);
BOOL WINAPI VideoSetPos(HV2D,long pos);
BOOL WINAPI FrameSetPos2d(HSP2D,HOBJ2D,long pos);
BOOL WINAPI VideoHandle();

BOOL WINAPI VideoPlay2d(HSP2D,HOBJ2D ,long start,long end,long rate,UINT32 flags);
BOOL WINAPI VideoPause2d(HSP2D,HOBJ2D);
BOOL WINAPI VideoResume2d(HSP2D,HOBJ2D);
BOOL WINAPI VideoStop2d(HSP2D,HOBJ2D );
LRESULT WINAPI aviaudioMessage(HWND hwnd, unsigned msg,LONG wParam, LONG lParam);
};

#else

BOOL InitVideo2d(VIDEOINIT*);
void DoneVideo2d();

extern HV2D (WINAPI * OpenVideo)(char far*,UINT16 flags);
extern BOOL (WINAPI * CloseVideo)(HV2D);
extern BOOL (WINAPI * GetVideoProp)(HV2D,VIDEOPROP*);
extern BOOL (WINAPI * GetFrameProp2d)(HSP2D,HOBJ2D,FRAMEPROP*);
extern BOOL (WINAPI * SetFrameProp2d)(HSP2D,HOBJ2D,FRAMEPROP*);

extern HOBJ2D (WINAPI * CreateVideoFrame2d)(HSP2D,HV2D ,gr_float x,gr_float y,gr_float sizex,gr_float sizey,RECT * src,DWORD flags);
extern BOOL   (WINAPI * VideoSetPos)(HV2D,long pos);
extern BOOL   (WINAPI * FrameSetPos2d)(HSP2D,HOBJ2D,long pos);
extern BOOL   (WINAPI * VideoHandle)();

extern BOOL (WINAPI * VideoPlay2d)(HSP2D,HOBJ2D ,long start,long end,long rate,UINT32 flags);
extern BOOL (WINAPI * VideoPause2d)(HSP2D,HOBJ2D);
extern BOOL (WINAPI * VideoResume2d)(HSP2D,HOBJ2D);
extern BOOL (WINAPI * VideoStop2d)(HSP2D,HOBJ2D);

extern HV2D (WINAPI * BeginWriteVideo2d)(HSP2D _hsp,char*file,int x,int y,int cx,int cy,char*f);
extern BOOL (WINAPI * WriteVideoFrame2d)(HV2D hv);

extern LRESULT (WINAPI* aviaudioMessage)(HWND hwnd, unsigned msg,LONG wParam, LONG lParam);

#endif


#endif
