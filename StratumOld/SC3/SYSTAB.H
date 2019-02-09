
//*************************************************
//             TPropertySheet
//*************************************************
#ifndef _SC_systab_H_
#define _SC_systab_H_
#define PSP_DATAVALID  1
#define PSP_INITIALIZE 2

#include "twindow.h"

// стили окна закладок
#define PSS_NORMAL    0
#define PSS_MASTER    1
#define PSS_COMBOLIST 2
#define PSS_DESTROYPAGES 8 // удаляет страницы
#define IDC_COMBOPAGES    512

struct doubleint
{
INT16 d1,d2;
};

class TPropertyButton
{
public:
	HWND    hwnd;
	BYTE    flags;
	int     helpctx;       // help context
	HBITMAP hbitmap;    // картинка
	char *  title;
	char *  templ;       // template name
	HINSTANCE hInstance;// template instance
	DLGPROC   Procedure; // template procedure
	union
  {
		void * user;  // user defined data
		DWORD dwuser;
		HSP2D hsp;
	};
	union
  {
		void * user1; // user defined data
		DWORD  dwuser1;
		HOBJ2D iuser1;
	};
	union
  {
		void * pdata;
		DWORD data;   // user defined data
		doubleint di;
	};
	// служебные
	RECT rect;
	TPropertyButton*next;
	TPropertyButton*prev;
	HWND activehwnd;
	INT16 result;
	int line;
	int sizex;
	TPropertyButton(char*_title,HINSTANCE h,LPCSTR _templ,int  _helpctx=0);
	TPropertyButton(int,HINSTANCE h,LPCSTR _templ,int  _helpctx=0);
	~TPropertyButton();
	void Insert(TPropertyButton*_next);
	inline void Invalid(){flags&=~(PSP_DATAVALID);}
	inline void Valid(){flags|=PSP_DATAVALID;}
};

class TPropertySheet:public TWindow
{
 	TPropertyButton*first;
 	TPropertyButton*active;
 	TCollection*items;
 	BYTE style;
 	BYTE autodelay;
public:
 	int sizex,sizey;
 	int height;
 	int focused;
 	int lines;
 	TPropertySheet(LPCREATESTRUCT,HWND);
 	~TPropertySheet();
 	virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
 	void Repaint();
 	void Paint(HDC,RECT&);
 	void PaintButtons(HDC hdc,RECT*r);
 	void SetButtons(TPropertyButton*,int _sizex);
 	virtual void EvLButtonDown(WORD modKeys,int xPos,int yPos);
 	void SetActive(TPropertyButton*);
};

BOOL CALLBACK DefPropertyItemProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

#endif// _SC_systab_H_
