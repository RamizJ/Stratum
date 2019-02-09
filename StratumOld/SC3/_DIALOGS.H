#include <windows.h>
#include <commdlg.h>
#include <mem.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "graphic\_g_def.h"
#include "class.h"
#include "dialogs.h"
#include "dialogs.rh"
#include "systab.h"
#include "messages.h"
#include "records.h"
#include "stream.h"
#include "vardata.h"
#include "compiler.h"
#define  _NOCURSOR
#include "sc3.rh"
#include "browser.h"
#include "matrix.h"
#include "grtools.h"
#include "hyper.h"
#include "treeview.h"
#include "video\scvideo.h"
#include "msgcrk.h"
#include "tree_itm.h"
#include "varinfo.h"
#include "\sc3_add\syntax\syntax.h"
#include "syncolor.h"
#include "print.h"
#include "project.h"
// for printer
#include "strings.rh"
#include <stratum\stratum.h>
#include <stratum\sc_types.h>
#define OT_EDIT      1
#define OT_COMBOBOX  2
#define OT_BUTTON    3
#define OT_LISTBOX   4

#if !defined(WIN32)
#define IDHELP 998
#include <dos.h>

#endif
#include "idehelp.h"
#include <stratum\sccall.h>
void SetDlgItemDouble(HWND h,int id,double d);

extern HINSTANCE hInstance;

BOOL IsIntValid(HWND hwnd,int id,INT16&,int _min,int _max,BOOL _msg=TRUE);
BOOL IsDoubleValid(HWND hwnd,int id,double&,double _min,double _max,BOOL _msg=TRUE);
void AddROP2(HWND hwnd,int id,int cur);

TPropertyButton*GetDlgPropertyButton(HWND hwnd);
int CreatePropertySheet(HWND hwnd,const char*title,TPropertyButton*b,int active=-1);
int CreatePropertySheetEx(HWND hwnd,const char*title,TPropertyButton*b,int active,LPCSTR _template);
BOOL IsAllPagesValid(HWND); // самый главный диалог

BOOL ObjHyperProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
//BOOL scCALLBACK ObjBrushProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
void ChkStyle(HWND hwnd,int id,DWORD&style,DWORD bit);

#define INTtoCHAR(i)   ((char*)((WORD)(i)))
#define _STDHELP(ctx) if(StdHelpProc(msg,wParam,lParam,ctx))return TRUE;
BOOL StdHelpProc(UINT msg,WPARAM wParam,LPARAM lParam,int ctx);

BOOL EditSubClass(HWND,int,double _min,double _max,double _step,int flags);
void SetDlgOrgSize2d(HWND,HSP2D,HOBJ2D);

class TBrushTProperty:public TPropertyButton{
public:
HOBJ2D hdib;
TBrushTProperty(int,int  _helpctx=0);
};

#define UDF_DOUBLE   1
#define UDF_MIN      2
#define UDF_MAX      4
#define UDF_MINMAX 2|4




