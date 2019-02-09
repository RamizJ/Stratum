/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			ballon.h
Author:
			Alexander Shelemekhov
*/
#ifndef _SC_BALLON_H_
#define _SC_BALLON_H_
#define MAX_BALLONSTR 80
class TBallonWindow:public TWindow{
 char text[MAX_BALLONSTR];
 BOOL show,wait;
 UINT htimer;
 int x,y;
 int cursory;
 public:
TBallonWindow(LPCREATESTRUCT,HWND hwnd);
~TBallonWindow();
void SetData(int x,int y,char*);
virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
private:
void HideWindow();
void AdjustPos();
};

#endif

