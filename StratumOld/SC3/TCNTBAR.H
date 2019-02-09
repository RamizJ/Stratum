/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			tcntbar.h
Author:
			Alexander Shelemekhov
*/
#if !defined(__CONTROL_BAR__H_)
#define __CONTROL_BAR__H_

#define TBS_STDBUTTON   0
#define TBS_SEPARATOR   2
#define TBS_CHECK       3
#define TBS_CHECKGROUP  4
#define TBS_CONTROL     5

#define TB_PRESSED   1
#define TB_DISABLED  2
#define TB_NODISABLE 32
#define TB_CHANGED   16

#define BUTTON_WIDTH  20
#define BUTTON_HEIGHT 20

class TRange;
class _TCommand;

class TControlBar:public TWindow{
 BOOL popup;
 BOOL alone;
 _TCommand *focus;
 _TCommand *highlight;
 TCollection*sizes;
 UINT    htimer;
public:
 TWindow*parent;
 int state;
 int sizex,sizey;
 int caption;
 int start;
 int _dock,dock;
 BOOL moved;
 RECT trackrect;

 RECT poprect;
 int  lastdock;
 TRange * range;
 POINT trackpoint,delta;
 TControlBar(LPCREATESTRUCT,HWND);
 ~TControlBar();
 void Adjust();
 void         MakeSizeList();
 void         DoneSizeList();
 virtual void EvSize(WORD sizeType,int sx,int sy);
 virtual void EvLButtonDown(WORD modKeys,int xPos,int yPos);
 virtual void EvRButtonDown(WORD modKeys,int xPos,int yPos);
 virtual void EvLButtonUp(WORD modKeys,int xPos,int yPos);
 virtual void EvMouseMove(WORD modKeys,int xPos,int yPos);
 _TCommand* GetButton(int xPos,int yPos);
 int     NcHitTest(int xPos,int yPos);
 virtual void EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode);
 virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
 BOOL   GetBallonStr(BALLONSTRUCT *bs);
 void   IdleAction();
 void   GetRect(int,POINT&);
 int    GetLenght(int dock);
 void   UpdateCombos(int dock_);
 void   RemoveCombos();
 void   HardRemove();
 void   PaintCaption(HDC );
 BOOL   inline IsPopup(){return popup;};
 void _Adjust(int dock_,POINT &_size,BOOL _popup);
private:


 void DrawFocusRect();
 void DrawSizeRect();
 void PaintButtons(HDC,RECT&);
 void PaintButton(HDC,_TCommand*,BOOL);
 void PaintButton(_TCommand*,BOOL pressed);
 BOOL IsButton(_TCommand*);
 void PrepareResize(int );
};

class TControlDock:public TCollection{
public:
 int mode;
 int size;
 int _size;
 TControlDock(int);
 BOOL InsertBar(TControlBar*,int flags=0,C_TYPE line=0);
 BOOL Adjust(TControlBar*);
 void Recalc(); // после изменения off97
 C_TYPE Remove(TControlBar*); // remove line return
 int ChkBar(TControlBar*); // Возвращает позицию первоначальной вставки

 int  GetSize();
 void FreeItem(pointer item){delete (TCollection*)item;};
 private:
 TCollection* InsertNew(TControlBar*);
};
BOOL InitControlBars(TControlDock*[]);
BOOL DoneControlBars();
HMENU CreateControBarMenu();
BOOL SwapControlBar(int);

BOOL PaintCaptions();
BOOL StoreControlBars(TStream&);
BOOL RestoreControlBars(TStream&);
void ControlColorChanged();
BOOL GetCommandInfo(int ,char*);

#endif
