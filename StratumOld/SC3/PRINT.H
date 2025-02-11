#ifndef __PRINT_h_
#ifndef ___STREAM_h
#include "stream.h"
#endif
#define UD_PRINTPAGE  13

struct PrintItem{
 double spacex;    // ������ � ������������
 double spacecx;   // ������ � ����������� ������������
 double spacecxR;  // ������ Space ��� ���������� �����;
 double pstart;    // � �������� ��������
 double plength;   // ����� � �������� ��������
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
 int mpagemode;       // ����� ������ ��� ���������������
 int xpage,ypage;
 int state;
 int maxpage;
 POINT2D _scale;       // ������� �� x � y(������ �����)
// ���������
 int scalemode;
 int copies;
 double scale;

 BOOL  allpages;     // ������ ���� �������
 BOOL  allextent;    // ���������� ���
 BOOL  addborders;   // ����������� �������������� ����
 BOOL  onlyexternal; // ���� ������ ��� ������� ������

 RECT2D rect2dcustom;  // ���������� ������������� (������ ������������� ���������!)

// ��� �������

 HWND Rscale1,Rscale2,Rscale3,Rscale4,Tscale;
 HWND Rpage1,Rpage2,Tpages;
 HWND Tcopies;
 HWND Cpagesenum;

 HWND Tsleft,Tstop,Tsright,Tsbottom;
 HWND Tfleft,Tftop,Tfright,Tfbottom;
 HWND Taright,Tabottom,Cause;
 HWND Cextent,Conly;
 unsigned int feet; // ��������� ���


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
