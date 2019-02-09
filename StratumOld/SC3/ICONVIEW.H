#ifndef __ICON_VIEW_SC3_H_
#define __ICON_VIEW_SC3_H_

class TIconWindow:public TWindow{
public:
 TIconWindow(LPCREATESTRUCT,HWND);
 PLibrary library;
 int mode;
 int sizex,sizey;
 int active;
 int top;
 int perline;
 int pery;
 void Paint(HDC,RECT &r);
 void SetActive(int h,BOOL s=1);
 void Update(int h);
 POINT GetSize();
 virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
 virtual void EvSize(WORD sizeType,int sx,int sy);
 virtual void EvLButtonDown(WORD modKeys,int xPos,int yPos);
 virtual void EvLButtonDblClk(WORD modKeys,int xPos,int yPos);
 virtual void ScrollTo(long*x,long*y);
};
#endif __ICON_VIEW_SC3_H_
