/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
                        twindow.h
Author:
                        Alexander Shelemekhov
*/
#ifndef _TWINDOW_SC_H_
#define _TWINDOW_SC_H_
#include <windows.h>

#include <gwin2d3d\gwinfunc.h>
#include <stratum\sccall.h>

#include "messages.h"
//#include "class.h"
#define MainClassName "STRATUM_FRAME_30"
#define ControlBarName "STRATUM_CntBar_30"
// »дентификаторы окон
#define ID_LIBRARYS      656
#define ID_HIERARCHY     657

#define DATA_CLASS     2

enum MDI_CHILD_MODE{MODE_NOTHING=0,   MODE_EDITOR2D= 1000,
                    MODE_EDITORTEXT=1001,MODE_MESSAGES=1002,
                    MODE_HIERARCHY=1003, MODE_LIBRARY=1004,
                    MODE_MODEL= 1005,    MODE_IMAGE=1006,
                    MODE_WATCH =1007,    MODE_BMPEDIT= 1008};

#define sMDI_CHILD  "SC_CHILD"

// типы данных в дропе

#define RANGE_SCHEME         10
#define RANGE_SCHEMELINK     11
#define RANGE_SCHEMEEDITOR2D 12
#define RANGE_IMAGE          13
#define RANGE_EDITOR2D       14

#define RANGE_TREE           15
#define RANGE_TEXT           16

#define RANGE_HIERARCHY  ID_LIBRARYS
#define RANGE_LIBRARYS   ID_HIERARCHY

#define SS_2DEDITOR      1

#define VB_TRANSPARENT   1 // просмотр диба с прозрачностью
#define VB_EXE           2 // просмотр диба из SC.exe прозрачностью
#define VB_BUTTON        4
#define VB_CHECKED       8
#define VB_DIB           16

#define CWF_NOMENU       1 // просмотр таблицы без меню

// ƒл¤ скроллеров

#define SF_SCHEME  2
#define SF_FIXTOP  4
#define SF_MANUAL  8 // должен пр¤тататьс¤ и показыватьс¤ автоматически

#define STATUS_ERROR         4
#define STATUS_OPERATION     5

// ƒл¤ ColorButton

// 0 - стандартна¤
// 1 - с возможностью выбора стандартного цвета
// 2 - сразу диалог
// 4 - c возможностью задани¤ прозрачного

#define CBS_STD           0
#define CBS_SYSCOLOR      1
#define CBS_STDDLG        2
#define CBS_TRANSPARENT   4


UINT    extern ballon_message;
UINT    extern scwnd_message;
BOOL    extern taskactive;
HBITMAP extern bmp_folder;

class TCollection;

POINT2D inline TPoint(gr_float x,gr_float y){POINT2D p;p.x=x;p.y=y;return p;}
class TLinkInfo;

typedef struct tagBALLONSTRUCT{
    POINT   bsPoint;
    char  * bsText;
    int     bsSize;
}BALLONSTRUCT;

struct UPDATESTRUCT{
    UPDATE_MODE mode;
    UPDATE_WHAT what;
    void *data;
    void *parent;
};
struct TSTRATUMMENU;
struct DRAGDROPSTRUCT{
    DWORD  data;
    INT16    datatype;
    POINT  CursorPos;
};

class TClass;
class TObject;
typedef TClass*PClass;
typedef TObject*PObject;

class TreeItemData;

class TScroller;
#ifndef RUNTIME
class TSchemeWindow;
#endif
int RegisterWindows();
class TMSGItems;
class TMenu;

class TWindow
{
    HINSTANCE CursorModule,DefCursorModule;
    LPCSTR    CursorResId,DefCursorResId;
    HCURSOR   HCursor,HDefCursor;
    BYTE      _scrollFlag;

public:
    TMSGItems * messages;
    TMenu *tmenu;
    BYTE   CanUseDelete;
    POINT static lastmousepos;
    HWND HWindow;
    TScroller * vscroll;
    TScroller * hscroll;

    TWindow(LPCREATESTRUCT,HWND hwnd);
    ~TWindow();

    void AddCopyCutMenu(HMENU m);
    virtual void EvLButtonDblClk(WORD modKeys,int x,int y){}
    virtual void EvLButtonDown(WORD modKeys,int x,int y){}
    virtual void EvLButtonUp(WORD modKeys,int x,int y){}
    virtual void EvRButtonDblClk(WORD modKeys,int x,int y){}
    virtual void EvRButtonDown(WORD modKeys,int x,int y){}
    virtual void EvRButtonUp(WORD modKeys,int x,int y){}
    virtual void EvMouseMove(WORD modKeys,int x,int y){}
    virtual void EvSize(WORD sizeType,int sx,int sy);

    virtual void EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode);

    virtual void ScrollTo(long*x,long*y){};
    virtual void PostScrollTo(){};
    virtual int  IsCommandDisable(int command);
    virtual BOOL GetBallonStr(BALLONSTRUCT*);
    virtual BOOL CanClose();
    BOOL EvSetCursor(HWND hWndCursor,UINT hitTest,UINT mouseMsg);
    BOOL SetCursor(HINSTANCE,LPCSTR);
    BOOL SetDefCursor(HINSTANCE=0,LPCSTR=0);
    inline  BOOL SetCursor(HINSTANCE module,int id)
    {
        return SetCursor(module,MAKEINTRESOURCE(id));
    };
    inline  BOOL SetDefCursor(HINSTANCE module,int id)
    {
        return SetDefCursor(module,MAKEINTRESOURCE(id));
    };
    BOOL LoadDefCursor(char* filename);
    virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
    LRESULT _WndProc(UINT, WPARAM,LPARAM);
    virtual LRESULT DefWindowProc(UINT, WPARAM,LPARAM);
    virtual BOOL AcceptDrop(DRAGDROPSTRUCT&);
    virtual BOOL ProcessDrop(DRAGDROPSTRUCT&);
    void  TrackPopupMenuR(POINT point,int r,TSTRATUMMENU*,BOOL chkDisable=FALSE);
    void   TrackPopupMenu(POINT point,TSTRATUMMENU*,BOOL chkDisable=FALSE);
    void    GetWinSizeByClient(int &_x,int &_y);
    void    CloseCombo();
    BOOL    SetScrollRange(RECT&sr);
};

class TScroller{
    TWindow*win;
    long lo,hi,_lo,hiscroll,pos;
    long step;
    long page; // –азмер рабочей области
#ifdef WIN32
    long Spage;
#endif
    INT16  mode;
    void _SetPos(long _pos);
public:
    long  singlestep; // размер шага при нажатии на стрелки
    BYTE  flags;
    int   autohide;   // автоматическое пр¤танье
    BOOL  show;       //
    TScroller(TWindow*,long amin,long amax,long _step,int mode);
    int inline GetPage(){return (int)page;};
    long GetPos();
    void SetPos(long,BOOL scroll=TRUE);
    void SetRange(long amin,long amax);
    LRESULT Process(WPARAM,LPARAM);
    void ChangeSize(long);
    long ValidValue(long);
    void Show(BOOL show);
    void SetParam(long amin,long amax,long apos,BOOL show,long apage);
#ifdef WIN32
    void SetPageSize();
#endif
};

#define WF_PROJECT 1
#define WF_PROGRAM 2
#define WF_HSP     4

class TProject;
class TWaitCursor{
    HCURSOR old;
public:
    TWaitCursor(HWND hwnd=0);
    virtual ~TWaitCursor();
};
//class TMenuWindow;
class TMDIChildWindow:public TWindow{
public:
    TProject*prj;
    long     flags;
    TWindow* client;
    int helpctx;
    // TMenuWindow *menuwnd;
    HWND     _client;
    MDI_CHILD_MODE MODE;
    TWindow * msgserver;
    TMDIChildWindow * mdiparent;
    TMDIChildWindow(LPCREATESTRUCT,HWND);
    ~TMDIChildWindow();
    virtual LRESULT DefWindowProc(UINT, WPARAM,LPARAM);
    virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
    virtual void EvSize(WORD sizeType,int sx,int sy);
    virtual void EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode);
    virtual BOOL CanClose();
};

class TSpaceWindow : public TWindow
{
public:
    HSP2D hsp;
    BOOL  scaling; // asking For scalebox

    TSpaceWindow(LPCREATESTRUCT, HWND);
    ~TSpaceWindow();

    virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
    virtual void ScrollTo(long*x, long*y);
    void         UpdateScrollers(BOOL delta=1);
    virtual void GetSpaceInfo(char *);
    virtual void EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode);
    void    UpdateScaleBox();
    void    Zoom(int xPos,int yPos,BOOL in,POINT2D * newmul=NULL,POINT2D * newdiv=NULL);
    virtual int  IsCommandDisable(int command);
    BOOL    Cm_SaveAs(BOOL settitle=FALSE);
    BOOL    Cm_Save(BOOL settitle=FALSE);
    void    SetCoordinates(POINT2D&p);

private:
    void EvPaletteChanged(HWND hWndPalChg);
    BOOL EvQueryNewPalette();
};

class TCollection;

#ifdef CAN_EDIT_BITMAP
class TBMPChild;
#endif

class TControlDock;

class TMainWindow:public TWindow
{
    BOOL recently_modifyed;
    HBITMAP menu_logo;
    int menu_logoCX;
    int menu_logoCY;
    int controlHeight;
    int statusHeight;
    int sizex,sizey;
public:
    HINSTANCE hinet;
    BOOL fullscreen;
    HBITMAP  bm_home;
    TCollection* recently;
    RECT mdirect;
    TControlDock*docks[4];
    HMENU menu;
    HMENU menu_recently;
    HWND  controlbar,statusbar,mdi;
    DWORD lasttreeitem;
    TMDIChildWindow *lastactivemdi;
    unsigned viewcontrol:1;

    HWND inline GetMDI(){return mdi;};
    TMainWindow(LPCREATESTRUCT,HWND);
    ~TMainWindow();
    virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
    virtual void EvSize(WORD sizeType,int sx,int sy);
    virtual LRESULT DefWindowProc(UINT, WPARAM,LPARAM);
#ifndef RUNTIME
    TSchemeWindow*CreateSchemeWindow(char*,PObject);
    TSchemeWindow*CreateImageWindow(char*);
    int   CreateTextWindow(PClass);
    int   CreateTextWindow(char *file,int mode);
#endif
    virtual void EvRButtonDown(WORD modKeys,int x,int y);
#ifdef CAN_EDIT_BITMAP
    TBMPChild* CreateBmpWindow(char *,HGLOBAL,HGLOBAL);
#endif

    virtual int  IsCommandDisable(int command);
    TMDIChildWindow * CreateMDIChild(char*name,TWindow*,
                                     HWND hwnd,RECT *r,MDI_CHILD_MODE);
    // r - не Rect в чистом виде а r.bottom и r.right - размеры по Y и X
    void AddRecentlyProject(char*);
    void RebuildRecentlyMenu();
    void RemoveRecently(char*);
    void RemoveRecentlyMenu();
    void Cm_Cascade();
    void Cm_TileH();
    void Cm_TileV();
    void Cm_Arrange();
    void Cm_ClearVar();
    void Cm_ClearStream();
    void Cm_ClearWindows();
    BOOL Cm_CloseChildren(BOOL ask);
    void Cm_ClearAll();
    void Cm_VarToDef(BOOL setset=FALSE);
    BOOL CanCloseProject();
    TSpaceWindow * GetWindowWithSpace(HSP2D hsp);
    virtual BOOL CanClose();
    BOOL Cm_Load(char *name=NULL,BOOL alternate=FALSE);

    // Control bar Functions
    int   IsControlChild(void*);
    BOOL  InsertControl(void*);  // ¬ставл¤ет бар
    BOOL  AdjustControl(void*);  // ѕосле перемещени¤
    void  UpdateControlSize();  // “олько вычисл¤ет
    void  SetChildSize(); // ј эта выставл¤ет
    BOOL  RemoveControl(void*); // ”дал¤ет бар
    void* InsertControl(char*,int dock);
    BOOL  ShowSysMenu(int );

    void  LoadDesktop();
    void  StoreDesktop();

    virtual void EvCommand(UINT command, HWND hwndCtl, WPARAM wNotifyCode);

    void  ShowControl(BOOL );
    void  ShowStatus(INT16);
    void  SetFullScreen(BOOL );
#ifndef RUNTIME
    void  Cm_Watch();
    void  Cm_Libs();
    void  Cm_Hier();
#endif
    void  Cm_Message();
    void  UpdateMainWindow();

protected:
    int __TreeItemMenu(HWND tree,DWORD data);
    void Cm_Exit();
#ifndef RUNTIME
    void Cm_NewProject();
    void  Cm_NewSimplyProject();
    TMDIChildWindow* CreateSpaceWindow(char*,HSP2D);
#endif
    void Cm_VarSave();
    void Cm_VarLoad();
    BOOL SaveAllEx();
    BOOL OpenURL();

};


class TWinItem;

class TMSGWindow:public TSpaceWindow
{
public:
    int       state;
    BOOL      popup,modal,nonmdi,closeOnLostFocus,shadow,inpsp_page;
    TWinItem* item;
    RECT2D    lastrect;
    TStream * hyperstream;

    TMSGWindow(LPCREATESTRUCT,HWND);
    ~TMSGWindow();

    virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
    void SetState(int _state);
    virtual void EvLButtonDown(WORD modKeys,int xPos,int yPos);
    virtual void EvLButtonUp(WORD modKeys,int xPos,int yPos);
    virtual void EvMouseMove(WORD modKeys,int xPos,int yPos);
    virtual void EvRButtonDown(WORD modKeys,int xPos,int yPos);

    virtual int  IsCommandDisable(int command);
    void EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode);
    LRESULT KeyDown(WPARAM wVkey,LPARAM dwKeyData);
    void StdHyperJump(POINT2D& ,HOBJ2D obj,UINT16 fwKeys);
    void Replace(HSP2D _hsp,char * effect=NULL);
    virtual void GetSpaceInfo(char *s);
    void AutoScroll(BOOL all=FALSE);
    void UpdateAScrollers();
    virtual void PostScrollTo();
    void  ProcessNewSpace();
};

class TViewDIB:public TSpaceWindow{
    HOBJ2D select;
    HOBJ2D mask;
    HOBJ2D win;
    HOBJ2D bwin;
    HOBJ2D object;
    int    state;
    POINT2D org;
    RECT  original;
    RECT  viewrect;
    RECT  maxrect;
    HOBJ2D lines[4];
public:
    TViewDIB(LPCREATESTRUCT,HWND);
    void NewDib(HSP2D h,HOBJ2D object,int type,HOBJ2D tool);
    void SetRect(RECT);
    RECT GetRect(){return viewrect;};
    virtual void EvLButtonDown(WORD modKeys,int xPos,int yPos);
    virtual void EvLButtonUp(WORD modKeys,int xPos,int yPos);
    virtual void EvMouseMove(WORD modKeys,int xPos,int yPos);
    virtual void GetSpaceInfo(char *s);
    void    SetText();
};

class TColorButton:public TWindow{
    int state;
    int sizex,sizey;
    BYTE style;
    COLORREF rgb;
public:
    BOOL modify;
    TColorButton(LPCREATESTRUCT,HWND);
    ~TColorButton();
    void   Paint(HDC);
    void   Repaint();
    virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
    virtual void EvSize(WORD sizeType,int sx,int sy);
    void SetColor(BOOL mode=TRUE);
    virtual void EvLButtonDown(WORD modKeys,int xPos,int yPos);
    virtual void EvLButtonUp(WORD modKeys,int xPos,int yPos);
    virtual void EvMouseMove(WORD modKeys,int xPos,int yPos);
};

class TMBPView:public TWindow{
    BOOL InWindow(int xPos,int yPos);
    BOOL Repaint(BOOL p);
public:
    int sizex,sizey;
    UINT16 flags;
    //0x001 - frame
    //0x100 - captured
    //0x200 - pressed
    //0x400 = checked
    HBITMAP hb;
    HGLOBAL hglobal;
    TMBPView(LPCREATESTRUCT,HWND);
    virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
    virtual void EvLButtonDown(WORD modKeys,int xPos,int yPos);
    virtual void EvLButtonUp(WORD modKeys,int xPos,int yPos);
    virtual void EvMouseMove(WORD modKeys,int xPos,int yPos);
    ~TMBPView();
};

TMainWindow extern * mainwindow;
UINT  extern CFormat_OBJ;
UINT  extern CFormat_CLASS;
void DrawShadowFrame(HDC hdc,int x,int y,int sizex,int sizey,BYTE flags);
// |1 - рисуетс¤ нажатым
// |2 - заполн¤етс¤ пространство
// |4 - в стиле Ms office 97
void DrawShadowLine(HDC hdc,int x,int y,int sizex,int sizey);

void SetSimpleText(char*);
void SetStatusIcon(int,int);
//void SDKPROC SetStatusProgressBar(int,int percent);

void SetDisableText(int);
void SetStatusMode(BOOL normal=TRUE);


void CloseProjectWindows(void * project);
int  FileDialog(HWND hwnd,char * s,int str,int flags);
int _FileDialog(HWND hwnd,char * s,char * szFilter,BYTE flags);
#define FD_SAVE     1
#define FD_READONLY 2 // можно открывать только на чтение
#define FD_ASLINK   4 // вставл¤ть как ссылку на файл
extern BYTE openinreadonly;
extern BYTE openaslink;

void FullDeleteMenu(HMENU m);
void CheckMenuForEnable(TWindow*,HMENU m);

void ShowControlBar(int,BOOL always=TRUE);
void PressControlButton(int command,int down=1);
BOOL IsControlButtonPressed(int command);
void SetScaleBox(double);
double GetScaleBox();
BOOL GetComboStr(int id,char*,int size);
BOOL SetComboStr(int id,char*);

void HELP(int context,int x,int y,HWND hwnd);
void HELP(int context);
void HELP(char *string);
void CreateBrushes();// создает кисти
void DoneBrushes();// ”дал¤ет эти кисти

HBRUSH extern Blight,Bdark,Bface,BGrayed;
HFONT  extern HFont;
int    extern SCcodepage,SCcharset;

struct TMDICreate{
    HWND   hwnd;
    TWindow * window;
    MDI_CHILD_MODE _mode;
};
char extern * sc_path;
BOOL extern paused;
BOOL extern dialog_active;
BYTE extern sc_interface;
HINSTANCE extern HResource;
BOOL inline IsCtl3d(){return (sc_interface&1)!=0;}
BOOL LoadResources();
BOOL FreeResources();
void PaintIcon(PClass _class,HDC dc,int x,int y,int sx,int sy);
HGLOBAL GetClassDIB();
LRESULT SendCommand(HWND to,HWND from,UINT codeNotify);
BOOL    _SendCommand(HWND to,UINT command);
void CreateBallonWindow();
void SetBallonData(int x,int y,char*);
void _GetBallonData();
void ControlIdle();
void SetMainTitle(char*);
void WriteTitleStr(char *s);

void _TrackPopupMenuR(HWND HWindow,POINT point,int r,TSTRATUMMENU*,BOOL chkDisable);
void _TrackPopupMenu(HWND HWindow,POINT point,TSTRATUMMENU*,BOOL chkDisable);

BOOL    GetModelTextWindow(PClass _class,HWND*mdi,HWND*edit);
HBITMAP ScLoadBitmap(HINSTANCE,LPSTR,int&x,int&y,BYTE,COLORREF bk);
int     MSGBOX(HWND,LPCSTR,LPCSTR,int);
HMENU   ScLoadPopup(int);
void    ScAppendMenu(HMENU,int flag,int cmd,int strid);

TMDIChildWindow*GetMDIParent(HWND hwnd);
extern char secondcode[13];
BOOL   IsCodeValid();
//void   SendCommand(HWND,UINT);
void   SetDefaultMenuItem(HMENU,UINT cmd);
void   CorrectClassMenu(HMENU  p,PClass _class);
void   GetSchemeWindowName(PClass _class,char*s); // »м¤ дл¤ окна схемы
void   GetImageWindowName(PClass _class,char*s); // »м¤ дл¤ окна изображени¤
void   GetTextWindowName(PClass _class,char*s);
void   AddCopyCutMenu(HMENU);
void   SetStatusBeginText();
void   SetCntBarStyle(int d);
void   LaunchURL(char*url);
//*-------------------------------------------------*/
#endif //_TWINDOW_SC_H_
