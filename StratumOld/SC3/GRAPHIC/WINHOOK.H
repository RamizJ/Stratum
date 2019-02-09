/*
Copyright (c) 1996  Virtual World
Module Name:
	 winhook.h
Author:
	 Alexander Shelemekhov
*/
#ifndef __WIN_HOOK__H_
#define __WIN_HOOK__H_

typedef void * far t_pointer;
typedef LRESULT (CALLBACK* WNDHOOKPROC)(t_pointer data,HWND hwnd, UINT &msg, WPARAM &wParam, LPARAM &lParam,BOOL & calldefault);
#ifndef ____WIN_HOOK__H_EXT_

#ifdef  _WIN_HOOK_DYNAMIC

extern WNDPROC( WINAPI *  CreateWindowHook)(HWND hwnd,WNDHOOKPROC,t_pointer data);
extern int    ( WINAPI *   ReleaseWindowHook)(HWND);
extern t_pointer ( WINAPI * GetWindowData)(HWND hwnd);
#else
extern "C"{
WNDPROC WINAPI CreateWindowHook(HWND hwnd,WNDHOOKPROC,t_pointer data);
int     WINAPI  ReleaseWindowHook(HWND);
t_pointer WINAPI GetWindowData(HWND hwnd);
}
#endif
#endif
#endif //__WIN_HOOK__H_
