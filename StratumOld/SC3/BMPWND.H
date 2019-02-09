
#ifndef _SC_BMPWND_H_
#define _SC_BMPWND_H_

#ifndef _TWINDOW_SC_H_
#include "twindow.h"
#endif

#define TBMP_GETSIZE 0x800
#define TBMP_SETDATA 0x801
#define TBMP_GETDATA 0x802
class TSchemeWindow;
class TBMPChild:public TMDIChildWindow{
public:
 TSchemeWindow* scheme;
 HOBJ2D object2d;
 HSP2D hsp;
 TBMPChild(LPCREATESTRUCT,HWND hwnd);
 virtual int  IsCommandDisable(int command);
 BOOL OpenBMP(HGLOBAL,HGLOBAL);
 virtual void EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode);
 LRESULT WndProc(UINT message, WPARAM wParam,LPARAM lParam);

 virtual BOOL CanClose();
 };
#endif //_SC_BMPWND_H_
