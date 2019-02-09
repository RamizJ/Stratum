/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			winman.h
      winman.cpp
Author:
			Alexander Shelemekhov
Description:
			Manager for user defined windows
*/
#ifndef __WIN_MAN__SC3_H_
#define __WIN_MAN__SC3_H_
#ifndef _TWINDOW_SC_H_
#include "twindow.h"
#ifndef _collection_h
#include "collect.h"
#endif
#endif
#define MAX_WIN_NAME 128

#define WF_MODAL   1
#define WF_NOIDLE  2

class TWinItem
{
public:
	union
  {
                TWindow* _window;
                TMSGWindow* window;
	};

	enum WIN_TYPE{WIN_MDI=0,WIN_DIALOG=1,WIN_POPUP=2,WIN_CHILD=3};

 	int browse:1;
	int nochangesize:1;

	TMDIChildWindow * mdichild;

 	char name[128];
 	char * _classname;
 	char * _filename;
 	char * popupparent;
 	void * project;
 	POINT  size; // для неизменяемого окна
 	BOOL   nosize;
 	BOOL   system;
 	WIN_TYPE mode;
 	HSP2D hsp;
 	DWORD flags;
 	HWND  HWindow;
 	TWinItem * Parent;
 	BOOL  autoscroll;

  int TransparentLevel;
  COLORREF TransparentColor;
  bool ChangeTransparentColor;

 	TWinItem(int,char*);
    TWinItem(char*winname,HSP2D _hsp,PClass _class,char*filename,int x,int y,int sizex,int sizey,DWORD _flags, HWND _parent=NULL);
 	TWinItem(char*winname,int table,char *,DWORD _flags); // для BROWSE
 	TWinItem(char*winname,TMDIChildWindow*,HSP2D _hsp); // для MDIChildWindow
	//TWinItem(HWND,char*winname,DWORD _flags); // диалог
 	TWinItem(HWND hwnd,char*winname,char *,DWORD __flags,DWORD win_style,PObject); // диалог
 	TWinItem(TWindow *w,char*winname);

 	~TWinItem();

 	void CreateMDI(int x=CW_USEDEFAULT,int y=CW_USEDEFAULT,int sizex=CW_USEDEFAULT,int sizey=CW_USEDEFAULT);
 	void _CreateSpaceWindow(DWORD,DWORD,HWND);
 	HSP2D Replace(PClass,char*file,HSP2D _hsp=0);
  void Activate();
  void SetClientSize(int x,int y);
  void Destroy();
  void Send();
 	BOOL  SetTable(HOBJ2D hobj,int t,char* c);
 	void  SetSize();

 	//Добавил Марченко С.В.
 	int SetTransparent();
 	int SetTransparent(int);
 	int SetTransparent(COLORREF);
 	int SetWindowRegion(long);
 	// - конец - Добавил Марченко С.В.

private:
 	HSP2D GetClassSpace(PClass);
 	void  HandleMessage(UINT);
 	void  CreateWindows();
};



int SCMessageBox(LPCSTR lpszText,LPCSTR lpszTitle,UINT fuStyle);
class TWinItem;

class TWindowManager : public TSortedCollection
{
public:
	TWinItem * captured;
	PObject    capobj;
	int        capflags;
	TCollection *allmdi;

	TWindowManager();
	~TWindowManager();

	//HSP2D CreateWindow(char*wname,char* style);
	HSP2D LoadSpace(char*wname,char*filename);
	HSP2D LoadSpaceWindow(char*wname,char*filename,char* style);
	HSP2D OpenSchemeWindow(char*wname,char* _classn,char* style);
	int   OpenBrowseWindow(char*wname,int table,char* style);
    HSP2D CreateWindowEx_(char*wname,char *parent,char *data,HSP2D _hsp,int x,int y,int cx,int cy,char *style);
	//HSP2D CreateDlgWindow(HWND parent,char*wname,char *data,int x,int y,int cx,int cy,char *style);
	HSP2D _CreateDialog(HWND parent,char*wname,char * classsname,BOOL part,PObject);
	//HWND  CreateDialog(HWND parent,char*wname,char * classsname);
	BOOL  PostCreateMDIWindow(char*,TMDIChildWindow*,HSP2D hsp);
	HSP2D  Loaded(char*wname,char *data,PClass * _class);

	BOOL  SetTable(char *,int,char*);
	BOOL  SetTable(HSP2D,HOBJ2D,int,char*);

	BOOL  CloseWindow(char*wname);
	BOOL  SaveSpace(char*wname,char*filename);
	BOOL  SetWindowTitle(char*wname,char*);
	BOOL  GetWindowTitle(char*wname,char*);

	BOOL  ShowWindow(char*wname,int nCmdShow);
	BOOL  SetWindowPos(char*wname,int x,int y,int sx,int sy);
	BOOL  SetWindowOrg(char*wname,int ,int);
	BOOL  SetScrollRange(char*wname,BOOL horisontal,int ,int);
	BOOL  SetWindowSize(char*wname,int ,int);
	BOOL  GetWindowProp(char*wname,char*info,char *data);

	BOOL  IsWindowVisible(char*wname);
	BOOL  IsIconic(char*wname);
	BOOL  IsWindowExist(char*wname);
	BOOL  BringWindowToTop(char*wname);

	void  CascadeWindows();
	void  Tile(BOOL vertical);
	void  ArrangeIcons();
	int   GetClientWidth(char*);
	int   GetClientHeight(char*);
	BOOL  SetClientSize(char*,int x, int y);
	BOOL  GetNameByHSP(HSP2D hsp,char *name);
	HSP2D GetHANDLE(char *name);
	COLORREF  ColorDialog(char*,COLORREF);
	void  CloseAll(void *project,BOOL system);
	void  CreateDialogWindow(char *name,int x,int y,char * type);
	TMDIChildWindow * GetChild(int id);

 	BOOL  WindowRect(char*wname,RECT &r);
	BOOL PostDelete(HWND);

 	virtual int   Compare(pointer key1,pointer key2);
 	virtual void 	FreeItem(pointer);

 	BOOL LoadMenu(char * s,char *,UINT32 f);
 	BOOL DeleteMenu(char * s);

 	BOOL CheckMenuItem(char * s,int id,UINT32 f);
 	BOOL EnableMenuItem(char * s,int id,UINT32 f);

 	BOOL mFileDialog(char * s,char * szFilter,BOOL save);
 	BOOL InputBox(char *title,char*msg, char*s);

 	BOOL RegisterObject(PObject,HSP2D,HOBJ2D obj,INT16 code,UINT32 flags);
 	BOOL UnRegisterObject(PObject,HSP2D,INT16 code);
 	BOOL RegisterObject(PObject,char*,HOBJ2D obj,INT16 code,UINT32 flags);
 	BOOL UnRegisterObject(PObject,char*,INT16 code);

 	BOOL UpdateMSG();
 	BOOL setcapture(PObject,HSP2D,UINT32);
 	BOOL releasecapture();
 	BOOL SetOrg(HSP2D hsp,gr_float,gr_float);
 	void StdHyperJump(HSP2D hsp,POINT2D&,HOBJ2D obj,UINT16 fwKeys);
 	BOOL CanRedo();
 	BOOL CanUndo();
 	void DoRedo();
 	void DoUndo();
 	// fwKeys

 	#define HJ_NORECORD  1024  // без записи в файл протокола
	#define HJ_LOADHYPER 2048 // загрузить THyperKey
 	void   InsertItem();
	C_TYPE InsertWin(TWinItem*);
	HSP2D  GetSpace(C_TYPE);
	TWinItem* GetWindow(char*);
	TWinItem* GetWindow(HSP2D hsp);
	HSP2D  _PostCreateWindow(C_TYPE);
	DWORD  ProcessFlags(char *style,RECT *r=NULL);
	BOOL   PostStep();
	void   HideProjectWindows();
	void   ShowProjectWindows(void*);
};

void SystemQuit(int f);
TWindowManager extern * winmanager;
#endif  // __WIN_MAN__SC3_H_
