//----------------------------------------------------------------------------
// TreeView - (C) Copyright 1996 by Virtual World
//----------------------------------------------------------------------------
#ifndef TREE_VIEW_H_
#define TREE_VIEW_H_
#include "twindow.h"

//#include "treeview.rh"

#define PLUS	  0
#define LINE_LR  1
#define MINUS_L  2
#define LINE_L	  3
#define LINE_R	  4
#define MINUS_LR 5
#define PLUS_L	  6
#define BOOK	  7
#define PLUS_LR  8
#define MINUS	  9
#define BOOK_2	  10
#define CLASS_IMG 11
#define VAR_IMG  12
#define VIDEO_IMG  13
#define TABLE_IMG  14
#define WARNING_IMG 15
#define WINDOWold_IMG 16

#define CAMERA_IMG  VIDEO_IMG
#define LIGHT_IMG   17
#define WINDOW_IMG  18
#define WORLD_IMG   19
#define LAMPON_IMG  20
#define LAMPOFF_IMG 21
#define TEXT_IMG    22
#define BMP_IMG     23
#define MATRIX_IMG  24
#define CROSS_IMG   25
#define CHIP_IMG    26
#define DLL_IMG      27
#define STRATUM_IMG 28
#define POLYLINE_IMG 29
#define USER_IMAGE 16384
#define ITEM_HEIGHT 16
#define ITEM_WIDTH 16

class TreeItemData
{
 public:
 DWORD data;
 INT16   datatype;
 DWORD parent;
 char* text;
 INT16 IsImage;
 BYTE  IsLeaf;
};

struct TREEPAINTSTRUCT{
 int x;
 int y;
 int sizex;
 int sizey;
 HDC hDC;
 DWORD data;
 INT16 datatype;
};

class TreeItem //: public TList
{
public:
 TreeItem* NextLeft;
 TreeItem* NextRight;
 TreeItem* Parent;
 int StateImage;
 int Level;
 int Number;

 BYTE IsActive;

 enum{leaf,close,open};
 char* Text;
 INT16 Image;
 DWORD Data;
 INT16 datatype;
 INT16 State;

 TreeItem(TreeItemData,int = 0);
 ~TreeItem();
 TreeItem* GetNextLeft() { return NextLeft; };
 TreeItem* GetNextRight() { return NextRight; };

 void DestroyChild();
 void ExcludeItem();
 void Update(TreeItemData);
 void AddInLeft(TreeItemData);
 void AddInRightEnd(TreeItemData);
 BOOL IsContain(TreeItem*);
 int Regen(int = 0);
 TreeItem* Search(DWORD);
 TreeItem* SearchEx(DWORD,int&);
 int GetTreeSize();

 TreeItem* Scan(int,int,int,int);
 TreeItem* ScanActive(int,int,int,int);
 void PaintText(int,int,HDC);
 void Paint(int,int,HDC,RECT&);
 void Paint(int,int,HDC,int,int);  // от некого элемента и ниже
 int GetNumber() { return Number; };
 int GetNumberLeft();
 int GetNumberRight();
 TreeItem* GetItem(int);
 inline void ActiveOn() { IsActive = 1; };
 inline void ActiveOff() { IsActive = 0; };
 inline int IsLeaf() { return (State == leaf); };
 inline int IsOpen() { return (State == open); };
 inline int IsClose() { return (State == close); };
 inline void OpenOn() { State = open; };
 inline void CloseOn() { State = close; };
 inline void LeafOn() { State = leaf; };
// int IsItemActive() { return IsActive; };
};

//.............................................................

class VTreeWindow :public TWindow
{
 TreeItem* ElementList;
 int DefaultIcon;
 TreeItem* ActiveItem;
 int _X_ofs;
 long Y_ofs;
 int state;
 int shift;
public:
 HWND NotifyWnd;

 VTreeWindow(LPCREATESTRUCT lp,HWND hw);
 ~VTreeWindow();
 int MinViewNumber();
 int MaxViewNumber();
 BOOL CanDrag;

 void UpdateScrollBar();
 virtual void EvLButtonDown(WORD modKeys,int xPos,int yPos);
 virtual void EvLButtonDblClk(WORD modKeys,int xPos,int yPos);
 virtual void EvLButtonUp(WORD modKeys,int xPos,int yPos);
 virtual void EvMouseMove(WORD modKeys,int xPos,int yPos);
 void VTreeWindow::EvRButtonDown(WORD,int xPos,int yPos);
 LRESULT Expand(WPARAM,LPARAM);
 void    _Expand(int count,TreeItem*);
 virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
 LRESULT InsertItem(WPARAM,LPARAM);
 LRESULT DeleteItem(WPARAM,LPARAM);
 BOOL DeleteItem(TreeItem*);
 LRESULT UpdateItem(WPARAM,LPARAM);
 LRESULT KeyDown(WPARAM,LPARAM);
 LRESULT GetItemInfo(int ,TreeItemData*);
 LRESULT UpdateClassWindows(UPDATESTRUCT*us);
 void DestroyChild(TreeItem*);
 void DestroyItem(TreeItem*);
 void ChangeActiveItem(TreeItem*);
 void ChangeActiveItem(int);
 void InsertChild(TreeItem*,TreeItemData*);
 void vn_Edit();
 void vn_Insert();
 void vn_Delete();
 int  GetX_ofs(){ if(shift)return _X_ofs-ITEM_WIDTH;else return _X_ofs; };
 void GetDATA(TreeItemData*,TreeItem*);
 virtual void ScrollTo(long*x,long*y);
 BOOL SetActive(DWORD);
 void CheckActive(TreeItem*);
};


#endif
