/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			menu.h
Author:
			Alexander Shelemekhov
*/
#ifndef _TMENU_SC_H_
#define _TMENU_SC_H_

#include "template.h"
#include "twindow.h"

class TWindow;
class TMenu{
 TWindow*window;
 TTplReader*tpl;
 BOOL  ReadMenu();
 BOOL  ReadItem(HMENU );
 BOOL  ReadPopup(HMENU );
 BOOL ReadKeyWord(int & code);
 public:
 void * project;
 HMENU menu,_menu;
 TMenu();
 TMenu(TStream*);
 ~TMenu();
 BOOL Assign(TWindow*_window);
 BOOL Remove();
 BOOL WndProc(UINT msg,WPARAM,LPARAM);
 BOOL Ok(){return menu!=NULL;};
};
/*
class TMenuWindow:public TWindow{
 TWindow *parent;
 public:
 ~TMenuWindow();
 TMenuWindow(LPCREATESTRUCT,HWND);
 virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
};
*/
#endif //_TMENU_SC_H_
