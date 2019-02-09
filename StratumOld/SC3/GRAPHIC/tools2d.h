/*
Copyright (c) 1994  Virtual World
Module Name:
         tools2d.h
Author:
         Alexander Shelemekhov
*/
#ifndef _tools2d_h_
#define _tools2d_h_
#ifndef __WINDOWS_H
#define NOSOUND          // Sound APIs and definitions
#define NOCOMM           // Comm driver APIs and definitions
#define NODRIVERS        // Installable driver APIs and definitions
#define NOMETAFILE       // Metafile support
#define NOSYSMETRICS     // GetSystemMetrics() and related SM_* definitions
#define NOSYSTEMPARAMSINFO //SystemParametersInfo() and SPI_* definitions
#define NOMSG            // APIs and definitions that use MSG structure
#define NOWINSTYLES      // Window style definitions
#define NOWINOFFSETS     // Get/SetWindowWord/Long offset definitions
#define NOSHOWWINDOW     // ShowWindow and related definitions
#define NODEFERWINDOWPOS  // DeferWindowPos and related definitions
#define NOVIRTUALKEYCODES // VK_* virtual key codes
#define NOKEYSTATES       // MK_* message key state flags
#define NOWH              // SetWindowsHook and related WH_* definitions
#define NOMENUS           // Menu APIs
#define NOSCROLL          // Scrolling APIs and scroll bar control
//#define NOCLIPBOARD       // Clipboard APIs and definitions
#define NOICONS           // IDI_* icon IDs
//#define NOMB              // MessageBox and related definitions
#define NOSYSCOMMANDS     // WM_SYSCOMMAND SC_* definitions
#define NOMDI             // MDI support
#define NOCTLMGR          // Control management and controls
#define NOWINMESSAGES     // WM_* window messages
#define NOHELP            // Help support
#include <windows.h>
#endif
#ifndef __GRAPH_DLL_TYPES
#include "../../BC/bc_include_gwin2d3d/gwintype.h"
#endif
#include "object.h"
#include <gwin2d3d\3drender.h>
#define CVT_CORE_TO_INFO

//const rfmustsaved=128u;
HGLOBAL ReadDIB(PStream);
WriteDIB(PStream,HGLOBAL);
//*-----------------------------------------------------*

class TPen2d:public TTools2d
{
public:
    COLORREF color;
    INT16 style,width,rop2;

    TPen2d();
    TPen2d(COLORREF acolor,INT16 aStyle,INT16 aWidth,INT16 Rop2,HOBJ2D handle=0);
    TPen2d(PStream,LONG _pos);
    virtual void Store(PStream);
    virtual WORD WhoIsIt(){return 101;}
};
typedef TPen2d * PPen2d;


//*-----------------------------------------------------*
class TDib2d:public TTools2d
{
    BOOL FreeDib;
protected:
    int Read(PStream ps, long offBits);
    void _TDib2d(PStream);
public:
    // —лужебна¤ информаци¤
    HGLOBAL dib;
    HBITMAP bitmap; // ”же конвертированна¤ в формат устройства
    POINT imagesize;
    void far* Bits;
    INT16 H, W;
    BITMAPINFO far* Info;
    BOOL IsCore;
    WORD Mode;
    DWORD NumClrs;
    long perline;
    char * filename;

    TDib2d():TTools2d(){dib=0;bitmap=0;FreeDib=TRUE;}
    TDib2d(HGLOBAL,HOBJ2D handle=0);
    TDib2d(PStream,LONG _pos);
    TDib2d(int,PStream);
    ~TDib2d();

    void _Store(PStream);
    virtual void Store(PStream);
    virtual WORD WhoIsIt(){return 103;}
    int InfoFromHandle();
    int SetDib(HGLOBAL);
    int Convert();
    void FreeBitmap();
    virtual BOOL SetFreeDib(BOOL l);
    virtual BOOL SetPixel(int x,int y,COLORREF);
    virtual COLORREF GetPixel(int x,int y,BYTE *index);
};
typedef TDib2d * PDib2d;

//*-----------------------------------------------------*
class TDoubleDib2d:public TDib2d
{
public:
    PDib2d andbitmap;

    TDoubleDib2d();
    TDoubleDib2d(HGLOBAL,HGLOBAL,HOBJ2D handle=0);
    TDoubleDib2d(PStream,LONG _pos);
    TDoubleDib2d(int,PStream);
    ~TDoubleDib2d();

    int SetDibs(HGLOBAL,HGLOBAL);
    virtual void Store(PStream);
    virtual WORD WhoIsIt(){return 104;}
    void _Store(PStream);
    virtual BOOL SetFreeDib(BOOL l);
};
typedef TDoubleDib2d * PDoubleDib2d;

//*-----------------------------------------------------*
#define BS_GRADIENT_LINEAR 10
#define BS_GRADIENT_RADIAL 11

class TBrush2d:public TTools2d
{
public:
    COLORREF color;
    COLORREF backcolor;//for hatch style only

    double gradient_x0;//for gradient style only
    double gradient_y0;
    double gradient_x1;
    double gradient_y1;
    COLORREF* gradient_colors;
    int gradient_colors_count;

    INT16 style;
    INT16 hatch;
    PDib2d dib;
    INT16 rop2;
    TBrush2d();
    TBrush2d(int Style,COLORREF rgb,PDib2d aDib,int ahatch,int Rop2,int handle=0);
    ~TBrush2d();
    TBrush2d(PStream,LONG _pos);
    virtual void Store(PStream);
    virtual WORD WhoIsIt(){return 102;}
    virtual int Awaken(_SPACE*);
};
typedef TBrush2d * PBrush2d;

//*-----------------------------------------------------*
class TFont2d : public TTools2d
{
public:
    LOGFONT2D font2d;
    LOGFONT* font;
    // textmetrick
    LONG tmHeight;
    LONG tmAscent;
    LONG tmDescent;
    BOOL tmValid;
    // textmetrick
    TFont2d();
    TFont2d( LOGFONT2D *,int handle=0);
    TFont2d(PStream,LONG _pos);
    void    GetTM(HDC);
    virtual void Store(PStream);
    virtual WORD WhoIsIt(){return 105;}
};
typedef TFont2d * PFont2d;

//*-----------------------------------------------------*
class TString2d:public TTools2d
{
public:
    int flags;
    char* string;

    TString2d();
    TString2d(char*,int handle=0);
    TString2d(PStream,LONG _pos);
    ~TString2d();

    virtual void Store(PStream);
    virtual WORD WhoIsIt(){return 106;}
    char * GetString();
    int StrLen();
    void UpdateFlags();
};
typedef TString2d * PString2d;

//*-----------------------------------------------------*
class TText2d : public TTools2d
{
public:
    PLOGTextCollection text;

    TText2d();
    TText2d(PStream,LONG _pos);
    ~TText2d();

    virtual void Store(PStream);
    virtual WORD WhoIsIt(){return 107;}
    virtual int Awaken(_SPACE*);
};
typedef TText2d * PText2d;



class TToolRef :public TTools2d
{
public:
    TToolRef();
    TToolRef(PStream ps,LONG _pos);
    ~TToolRef();

    virtual TTools2d * GetMainObject() {return tool ? tool->GetMainObject() : 0;}
    virtual int Awaken(_SPACE*) {return tool ? 1 : 0;}

public:
    TTools2d * tool;
    char * name;
};

class TRefToDib2d : public TToolRef
{
public:
    TRefToDib2d();
    TRefToDib2d(char*,pointer);
    TRefToDib2d(PStream,LONG _pos);
    ~TRefToDib2d();

    virtual void Store(PStream);
    virtual WORD WhoIsIt(){return 110;}
    virtual int Awaken(_SPACE*);
};

class TRefToDoubleDib2d : public TToolRef
{
public:
    TRefToDoubleDib2d();
    TRefToDoubleDib2d(char*,pointer);
    TRefToDoubleDib2d(PStream,LONG _pos);
    ~TRefToDoubleDib2d();
    virtual void Store(PStream);
    virtual WORD WhoIsIt(){return 111;}
    virtual int Awaken(_SPACE*);
};




//*-----------------------------------------------------*
BOOL IsPensEqual(PPen2d,PPen2d);
BOOL IsBrushsEqual(PBrush2d,PBrush2d);
BOOL IsDibsEqual(PDib2d,PDib2d);
BOOL IsDDibsEqual(PDoubleDib2d,PDoubleDib2d);
BOOL IsStringsEqual(PString2d,PString2d);
BOOL IsFontsEqual(PFont2d,PFont2d);
BOOL IsTextsEqual(PText2d,PText2d);
//*-----------------------------------------------------*
struct TextRecord
{
    _OBJECT* text2d;			// сам инструмент
    PFont2d font;       //ссылка на инструмент-шрифт
    PString2d string;   //ссылка на инструмент-строку
    COLORREF ltFgColor; //цвет текста
    COLORREF ltBgColor; //цвет фона текста

    TextRecord();
    TextRecord(LOGTEXT*);
    ~TextRecord();

    void GetLogText(LOGTEXT*);
};

typedef TextRecord * PTextRecord;
typedef TRefToDib2d * PRefToDib2d;
typedef TRefToDoubleDib2d * PRefToDoubleDib2d;
void ChkRDibToUnload();
void ChkRDoubleDibToUnload();

PDib2d GetRDib2d(char *,_OBJECT*);
PDoubleDib2d GetRDoubleDib2d(char *,_OBJECT*);
void MarkToolsToSave(PTools2d,int);
//*-----------------------------------------------------*

class TSpace3d;
class TMaterial3d:public TTools2d{
public:
    TMATERIAL3D _m;
    USERTYPESTRUCT * userdata;
    char   * file;
    char   * exttype;
    TDib2d * dib;

    double  defXtile;
    double  defYtile;
    TSpace3d*space3d;
    _setup();
    TMaterial3d();
    TMaterial3d(MATERIAL3D*,TSpace3d*);
    TMaterial3d(PStream,LONG _pos);
    ~TMaterial3d();
    void  Set(MATERIAL3D*);
    virtual void Store(PStream);
    virtual WORD WhoIsIt(){return 112;}
    BOOL    LoadDib();
};
long NColors(WORD bitCount);
long ScanBytes(int pixWidth, int bitsPixel);
TDib2d * ImportRasterImage(char*filename,UINT filetype,BOOL* dbl);
#endif //_tools2d_h_
