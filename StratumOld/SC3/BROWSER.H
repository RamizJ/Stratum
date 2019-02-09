/*
Copyright (c) 1996  TST

Project:Stratum 3.0

Module Name:
			browser.h
Author:
			Alexander Shelemekhov
*/
#ifndef __SC_BROWSER_H_
#define __SC_BROWSER_H_
#ifndef _collection_h
#include "collect.h"
#endif

#define BF_VLINES   1
#define BF_HLINES   2
#define BF_APPEND   4
#define BF_INSETACTIVE 8  // идет обработка SetActive

#define CELL_H 20

#define CF_USER   0
#define CF_EDIT   1
#define CF_COMBOEDIT 2
#define CF_COMBOLIST 3

#define CF_DRAW     16
#define CF_READONLY 32

#define BS_STORECELL    1
#define BS_READONLY     2

#define BLS_HORIZONTAL  1
#define BLS_USERTEXT    2  // Запрашивать текст
#define BLS_MARKCURRENT 4  // Отмечать текущую (Не работает)
#define BLS_RESIZE      8  // Можно менять размер
#define BLS_MOUSENOTIFY 16 // Извещения о нажатии на панель
struct CELLINFO{
long     y,x;
WORD     flags;
COLORREF color;
RECT     rect;
char     *text;
BOOL     ok;
BOOL     message;
HDC      hDC;
};

struct TLABELINFO{
HWND   hwnd;
long   pos;
char * text;
int    id;
TLABELINFO(HWND _hwnd,char*_text){
   pos=0;hwnd=_hwnd;text=_text;
#ifdef WIN32
 id=GetWindowLong(hwnd,GWL_ID);
#else
 id=GetWindowWord(hwnd,GWW_ID);
#endif
          }
TLABELINFO();
};

class TCollumn{
public:
int _min,_max,current;
long start;
int type;
WORD flags;
TCollumn(int _amin,int _amax,int _type);
TCollumn(TCollumn*);
};

class TBrowser;

class TBrowserLabel:public TWindow{
 public:
 BOOL h; //horosontal bar
 BOOL gettext; // parent must get text
 BOOL markcurrent;
 BYTE flags;
 int  drag;
 int sizex,sizey;
 TBrowser* browser;
 TBrowserLabel(LPCREATESTRUCT l,HWND h);
 virtual void EvSize(WORD sizeType,int sx,int sy);
 virtual LRESULT WndProc(UINT , WPARAM , LPARAM);
 void Paint(HDC,RECT&);
 void PaintPanel(HDC,RECT&);
 void Repaint();
 void DrawSText(HDC hdc,int x,int y,char*,RECT *rect);
 virtual void EvLButtonDown(WORD modKeys,int xPos,int yPos);
 virtual void EvLButtonUp(WORD modKeys,int xPos,int yPos);
 virtual void EvMouseMove(WORD modKeys,int xPos,int yPos);
 int GetFromMouse(int x);
};

class TBrowser:public TWindow{
public:
TCollection * items;
// HBITMAP bitmap; // Bitmap for grid
 HWND edit;      // edit line handle
 HWND combo;     // combobox
 HWND subwindow;

 HFONT font;     // шрифт
 HWND notify;    // окно уведомитель
TBrowserLabel*hlabel;
TBrowserLabel*vlabel;

long top;// в элементах
long left;// в элементах
int delta;
long activeX,activeY;
BYTE activevalid;
long count;
int sizex,sizey;
long wholeXsize; //
WORD flags;

 TBrowser(LPCREATESTRUCT l,HWND h);
~TBrowser();
 void DeleteItems();
 void Paint(HDC,RECT&);
 virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
 virtual void ScrollTo(long*x,long*y);
 void Cm_Down();
 void Cm_Up();
 void Cm_Insert();
 virtual void PaintCell(HDC,long x,long y,int xpos,int ypos);
 void EvLButtonDown(WORD modKeys,int xPos,int yPos);
 void EvRButtonDown(WORD modKeys,int xPos,int yPos);
 virtual void EvSize(WORD sizeType,int sx,int sy);
 virtual COLORREF GetStr(long y,long x,char*);
 void ReCalc();
 void SetRange(long c);
 void SetActive(long y,long x,BOOL notify=TRUE);
 BOOL SetData(BOOL msg=TRUE);
 void SelectEdit();
 void UpdateSize();
 BOOL GetCellByMouse(int x,int y,LONG&,LONG&);
 virtual void EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode);
 virtual void EvLButtonDblClk(WORD modKeys,int x,int y);
};
 struct SEARCH_in_CURSOR;

class TCursorWindow:public TWindow
{
public:
  HWND browser,hl,vl;
  TCollection *items;
  SEARCH_in_CURSOR * searchdata;

  int table;
  int lastfield;
  BOOL inmdi;
  TCursorWindow(LPCREATESTRUCT l,HWND h);
 ~TCursorWindow();
  virtual void EvSize(WORD sizeType,int sx,int sy);
  virtual LRESULT WndProc(UINT , WPARAM , LPARAM);
  void SetTable(int t,char *p);
  void DeleteItems();
  int GetFieldLen(int id);
  virtual int  IsCommandDisable(int command);
  virtual void EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode);
 };
#endif
