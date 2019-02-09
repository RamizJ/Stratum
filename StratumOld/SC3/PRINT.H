#ifndef __PRINT_h_
#ifndef ___STREAM_h
#include "stream.h"
#endif
#define UD_PRINTPAGE  13

struct PrintItem{
 double spacex;    // начало в пространстве
 double spacecx;   // размер в координатах пространства
 double spacecxR;  // размер Space без добавочных полей;
 double pstart;    // в пикселах страницы
 double plength;   // длина в пикселах страницы
};

class TPrintSpace{
HWND hwnd;
HCURSOR hCsave;
public:
 BOOL errorcode;
 TCollection *xitems;
 TCollection *yitems;

 enum CONVERT_MODE {SPACE_to_PRINTER,PRINTER_to_SPACE,MM_to_SPACE,MM_to_PRINTER,PRINTER_to_MM,SPACE_to_MM};
 int  width;  // in mm
 int  height; // in mm
 int _HORZRES; //width in pixels
 int _VERTRES; //height in pixels
 int _LOGPIXELSX;//Number of pixels per logical inch along the display width.
 int _LOGPIXELSY;//	Number of pixels per logical inch along the display height.

 HDC hDC;
 RECT2D rect2d;
 RECT2D fileds;
 POINT2D addf;
 HSP2D hsp;
 int pagecount;
 int mpagemode;       // режим печати при многокопийности
 int xpage,ypage;
 int state;
 int maxpage;
 POINT2D _scale;       // Масштаб по x и y(Всегда верен)
// параметры
 int scalemode;
 int copies;
 double scale;

 BOOL  allpages;     // Печать всех страниц
 BOOL  allextent;    // Печатается все
 BOOL  addborders;   // добавляются дополнительные поля
 BOOL  onlyexternal; // Поля только для внешних листов

 RECT2D rect2dcustom;  // Печатаемый прямоугольник (всегда соответствует исходнику!)

// Для диалога

 HWND Rscale1,Rscale2,Rscale3,Rscale4,Tscale;
 HWND Rpage1,Rpage2,Tpages;
 HWND Tcopies;
 HWND Cpagesenum;

 HWND Tsleft,Tstop,Tsright,Tsbottom;
 HWND Tfleft,Tftop,Tfright,Tfbottom;
 HWND Taright,Tabottom,Cause;
 HWND Cextent,Conly;
 unsigned int feet; // Поместить все


char szDevString[120];      /* array for WIN.INI data   */
char *szPrinter, *szDriver; /* printer and driver names */
char *szPort;               /* port name                */
 TPrintSpace(HWND,HSP2D ); // from default printer
 ~TPrintSpace();
 BOOL Setup(HWND _hwnd);
 BOOL BeginPrint();
 BOOL EndPrint();
 BOOL Print(int);
 void UpdatePrinter(char*p);
 void UpdatePage(HWND _page,int mode);
 void UpdatePrinter();
 void SetScaleMode(int );
 void _SetScale(double);
 void SetExtent(BOOL);
 void UpdateAllDialogs(UINT32 =0x00ffu);
 double GetWorkWidth();
 double GetWorkHeight();
 double ConvertX(double,CONVERT_MODE);
 double ConvertY(double,CONVERT_MODE);
 BOOL   Store(TStream&);
 BOOL   Restore(TStream&);
 BOOL   PrinterSetupDialog(HWND hwnd);
private:
 Init();
 void SetDouble(HWND ,double);
 void DoneItems();
};

#endif
