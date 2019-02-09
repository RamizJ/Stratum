/*
Copyright (c) 1995  Virtual World
Module Name:
     space2d.h
Author:
     Alexander Shelemekhov
*/
#ifndef _SPACE2D__H_
#define _SPACE2D__H_
#include "object2d.h"
#include "_space.h"
#include "view3d.h"


typedef struct tagPRNTEXT
{char * string;
    PTextRecord pt;
    int len;
    POINT2D size;
    int    ascent;
}TPRNTEXT;

class TFrame3d;
class TAxis3d;
class TSpace3d;
/**************************************************************************
*  SPACE2D SPACE2D SPACE2D SPACE2D SPACE2D SPACE2D SPACE2D SPACE2D SPACE2D
***************************************************************************/
class TSpace2d :public SPACE
{
public:
    INT32 RenderEngine;//0-old, 1-cairo

    static BYTE _NeedAwaken;
    static PCollection _dibs;
    static PCollection _ddibs;
    static char * currentpath;
    static char * texturepath;
    static WORD paint_flags;
    WORD State;
    POINT2D org;
    POINT2D scale_mul;
    POINT2D scale_div;
    char * name;
    char * author;
    char * description;
    char * filename;
    char * path;
    char * txtPath;
    UINT16 defaultflags;
    DWORD layers;
    PRotateCenter RCenter;
    PEditFrame Frame;
    TFrame3d * Frame3d;
    TAxis3d * Axis3d;
    // not saved in file
    HWND hwnd;
    HSP2D _HANDLE;
    RECT updaterect; // Накапливаемая область обновления
    HPALETTE palette;
    long maxcolor;
    BYTE ischanged;  /* Объекты были изменеы со времени последнего Paint`a */
    RECT wndupdaterect;/* реальная часть экрана которая обновляется в pixela'h */
    POINT2D membitmaporg;
    PCollection tmpCollection;
    PFont2d activefont;
    HFONT   _activefont;
    INT16    activefontangle;
    //*------------------------------*
    //PMetafile meta;
    BYTE record; // 1 идет запись в метафайл;
    BYTE record_flag;
    BYTE lowscale; // необходимо корректировать область обновления
    BYTE rectchange; // изменился обрамляющий прямоугольник пространства
    PStream meta;
    META_RECORD mrecord;
    HOBJ2D _dragobj;
    POINT2D _dragobjorg;
    BOOL modifyed;//В пространство было внесено изменение
    //*------------------------------*
    CRDSYSTEM2D   crd_system;
    double        mobr[3][3];
    BOOL          crd_used;
    //*------------------------------*
    BYTE  readonly;
    INT16 lock; // число блокировок разблокировок
    // tools

    PPenCollection pens;
    PBrushCollection bruhs;
    PDibCollection dibs;
    PDoubleDibCollection ddibs;
    PFontCollection fonts;
    PStringCollection strings;
    PTextCollection texts;
    P3dCollection spaces3d;
    PBrush2d BkBrush;


    TSpace2d(HWND ahwnd,LPSTR apath);
    TSpace2d(PStream);
    ~TSpace2d();

    virtual void _SetModify(){modifyed=TRUE;}

    HWND  SetHWND(HWND hwnd);
    virtual WORD WhoIsIt(){return 17458;}/* 2d */
    virtual void Store(PStream);
    virtual PObject GetObjByHANDLE(HOBJ2D);
    void SetHANDLE(HSP2D);
    int Lock();
    int UnLock(); // если lock 0 то перерисовка
    void Invalidate();
    void SendPaint();
    void View(HDC dc,RECT UpdRect);
    void SetAnisotropic(HDC);
    void SetTextMode(HDC);
    void UpdateLowScale();
    void _LpToDp(POINT2D FAR* lppt,int cPoints);
    void _DpToLp(POINT2D FAR* lppt,int cPoints);
    void _DpToLpWO(POINT2D FAR* lppt,int cPoints);
    void _LpToDpWO(POINT2D FAR* lppt,int cPoints);

    void _LpToDp(POINT FAR* lppt,int cPoints);
    void _DpToLp(POINT FAR* lppt,int cPoints);
    void _DpToLpWO(POINT FAR* lppt,int cPoints);
    void _LpToDpWO(POINT FAR* lppt,int cPoints);

    void CRDtoLP(POINT2D FAR* lppt,int cPoints=1);
    void LPtoCRD(POINT2D FAR* lppt,int cPoints=1);

    HOBJ2D GetFrameCursor(POINT2D&);
    HOBJ2D GetFrame3dCursor(POINT2D&);
    HOBJ2D CreateEditFrame2d(POINT2D SizerSize ,int f);
    HOBJ2D CreateUserObject2d(USEROBJSTRUCT *us,USERTYPESTRUCT*);
    HOBJ2D CreateControl2d(char const * lpszClassName,
                           char const * lpszWindowName,
                           DWORD dwStyle,
                           int x,  int y,  int nWidth,  int nHeight,
                           INT16 id,  DWORD ExdwStyle);
    BOOL DeleteEditFrame2d();
    HOBJ2D CreateRotateCenter2d(POINT2D& center );
    BOOL DeleteRotateCenter2d();
    HOBJ2D CreateEditFrame3d(POINT2D SizerSize );
    BOOL DeleteEditFrame3d();
    HOBJ2D CreateAxis3d(POINT3D,int);
    BOOL DeleteAxis3d();

    POINT2D GetFramePoint(int);
    BOOL GetFullPath(char* dst,char* fn);

    // Pen
    HOBJ2D CreatePen(INT16 Style,INT16 Width,COLORREF rgb,INT16 Rop2 );
    DWORD SetPenStyle(HOBJ2D index,INT16 Style );
    DWORD SetPenWidth(HOBJ2D index,INT16 Width );
    COLORREF  SetPenColor(HOBJ2D index, COLORREF rgb );
    DWORD SetPenRop2(HOBJ2D index,INT16 Rop2 );
    COLORREF GetGDIColor(COLORREF _color);
    void ChkPen(PPen2d);

    // Brush
    HOBJ2D CreateBrush(INT16 Style,INT16 Hatch,
                       COLORREF rgb,HOBJ2D DibIndex,INT16 Rop2 );
    DWORD SetBrushStyle(HOBJ2D index,INT16 Style );
    DWORD SetBrushHatch(HOBJ2D index,INT16 Width );
    DWORD SetBrushDibIndex(HOBJ2D index,HOBJ2D DibIndex );
    COLORREF  SetBrushColor(HOBJ2D index, COLORREF rgb );
    DWORD SetBrushRop2(HOBJ2D index,INT16 Rop2 );
    DWORD SetBrushPoints(HOBJ2D index,double x0,double y0,double x1,double y1);
    DWORD SetBrushColors(HOBJ2D index,COLORREF* colors,int colors_count);
    void ChkBrush(PBrush2d);

    // Dib2d
    HOBJ2D CreateDib(HGLOBAL Dib );
    HOBJ2D CreateRDib2d(LPSTR);
    BOOL SetDib(HOBJ2D DibIndex, HGLOBAL Dib );
    BOOL SetRDib2d(HOBJ2D,LPSTR,LPSTR );
    void ChkDib(PDib2d pp);
    PDib2d GetDib2d(HOBJ2D h,BOOL ld=FALSE);
    PDoubleDib2d GetDoubleDib2d(HOBJ2D h,BOOL ld=FALSE);
    // DoubleDib2d
    HOBJ2D CreateDoubleDib(HGLOBAL XorDib, HGLOBAL AndDib );
    BOOL   SetDoubleDib(HOBJ2D DoubleDibIndex,HGLOBAL XorDib, HGLOBAL AndDib );
    HOBJ2D CreateRDoubleDib2d(LPSTR);
    BOOL   SetRDoubleDib2d(HOBJ2D,LPSTR,LPSTR);

    void ChkDoubleDib(PDoubleDib2d pp);
    // String2d
    HOBJ2D CreateString2d(LPSTR String );
    BOOL SetLogString2d(HOBJ2D index, LPSTR String );
    void ChkString(PString2d ps);
    //LogFont2d
    HOBJ2D CreateFont2d(LOGFONT2D * LogFont );
    BOOL SetLogFont2d(HOBJ2D index, LOGFONT2D * Logfont );
    void ChkFont(PFont2d pf);
    // Text2d
    HOBJ2D CreateText(LOGTEXT * Text, INT16 NumItems );
    BOOL CheckNewText(LOGTEXT * Text,INT16 NumItems );
    BOOL SetLogText2d(HOBJ2D index, LOGTEXT * Text, INT16 NumItems,INT16 _m=0);
    BOOL ChkLOGTEXT(LOGTEXT * Text);
    void ChkText(PText2d );
    //--------------
    INT16 DeleteUnusedTool(TOOL_TYPE what,PStream ps=NULL);
    PToolsCollection GetToolsById(TOOL_TYPE t);
    int DeleteTool2d(TOOL_TYPE what,HOBJ2D index);
    //---------------
    BOOL SetObjectOrg2d(HOBJ2D Object2d, POINT2D &Origin );
    BOOL SetObjectSize2d(HOBJ2D Object2d, POINT2D &Size );
    BOOL RotateObject2d(HOBJ2D Object2d, ROTATOR2D * rotator );
    BOOL SetObjectAlpha2d(HOBJ2D Object2d, gr_float alpha /*0.0 .. 1.0*/);
    //--------------------
    long SetBkBrush(HOBJ2D);

    BOOL SetScale2d(POINT2D,POINT2D);
    C_TYPE ObjectToEnd2d(HOBJ2D Object2d,C_TYPE top );
    C_TYPE SetZOrder(HOBJ2D Object2d, C_TYPE ZOrder );
    // Возвращают бывший Z ордер.
    BOOL   SwapObject(HOBJ2D First2d, HOBJ2D Second2d ); // 1 успех
    C_TYPE GetZOrder(HOBJ2D Object2d );
    // Create Objects
    HOBJ2D CreatePolyline2d(HOBJ2D Pen,HOBJ2D Brush,POINT2D * points,INT16 NumPoints );
    HOBJ2D CreateBitmap2d(HOBJ2D DibIndex, POINT2D& SrcOrg, POINT2D& SrcSize,
                          POINT2D& DstOrg, POINT2D& DstSize,INT16 angle=0);
    HOBJ2D CreateDoubleBitmap2d(HOBJ2D DibIndex, POINT2D& SrcOrg, POINT2D& SrcSize,
                                POINT2D& DstOrg, POINT2D& DstSize,INT16 angle=0);
    HOBJ2D CreateTextRaster2d(HOBJ2D TextIndex,POINT2D& Org, POINT2D& Size,
                              POINT2D& Delta,INT16 DstAngle  );
    HOBJ2D Create3dProjection2d(HSP3D hSpace3d,POINT2D& DstOrg,POINT2D& DstSize
                                ,HOBJ2D);
    BOOL DeleteObject2d(HOBJ2D h);
    // ----------------------------------------------------------
    BOOL SetVectorPoint2d(HOBJ2D VectorObj2d,INT16 pointNum,POINT2D & point );
    BOOL SetVectorPoints2d(HOBJ2D VectorObj2d,POINT2D * points,INT16 NumPoints );
    INT16 AddVectorPoint2d(HOBJ2D VectorObj2d,INT16 pointNum,POINT2D& point );
    BOOL SetVectorArrows2d(HOBJ2D Object2d,ARROW2D*);
    BOOL DelVectorPoint2d(HOBJ2D VectorObj2d,INT16 pointNum,POINT2D & point );
    //^ return new  count of the points
    INT16  SetRgnCreateMode(HOBJ2D rgn,INT16 mode);
    DWORD SetVectorPen2d(HOBJ2D VectorObj2d,HOBJ2D apen);
    DWORD SetVectorBrush2d(HOBJ2D VectorObj2d,HOBJ2D abrush);
    //-------------------------------------------------------------
    BOOL SetBitmapSrcRect(HOBJ2D Object2d ,POINT2D &,POINT2D &);
    HOBJ2D SetDibObject2d(HOBJ2D Object2d,HOBJ2D DibIndex );
    HOBJ2D SetDoubleDibObject2d(HOBJ2D Object2d,HOBJ2D DoubleDibIndex );
    HOBJ2D SetTextObject2d(HOBJ2D Object2d,HOBJ2D TextIndex );
    BOOL SetTextDelta2d(HOBJ2D Object2d,POINT2D &p);
    //-------------------------------------------------------------
    POINT2D CalcTextSize(PText_2d);
    HSP3D Set3dSpace2d(HOBJ2D Object2d, HSP3D hSpace3d );
    BOOL  Set3dViewCamera(HOBJ2D Object2d,HOBJ2D,BOOL _m);
    //-------------------------------------------------------------
    HOBJ2D CreateRgnGroup(RGNGROUPITEM* items,INT16 NumItems );
    BOOL ChkFutureRGroupMember(RGNGROUPITEM * Objects2d,INT16 NumObjects,HOBJ2D owner);
    BOOL SetRgnGroupItem(HOBJ2D group2d,INT16 ItemNum,RGNGROUPITEM * Object2d );
    BOOL SetRgnGroupItems(HOBJ2D group,RGNGROUPITEM * Objects2d,HOBJ2D NumObjects );
    BOOL AddRgnGroupItem(HOBJ2D group2d,INT16 ItemNum,RGNGROUPITEM * Object2d );
    BOOL GetRgnGroupItems(HOBJ2D group2d,RGNGROUPITEM * Objects2d, INT16 MaxObjects );
    BOOL GetRgnGroupItem(HOBJ2D group2d,HOBJ2D ItemNum,RGNGROUPITEM * Object2d );
    //-------------------------------------------------------------
    TSpace3d* CreateSpace3d();
    //-------------------------------------------------------------
    virtual C_TYPE DeleteFromGroup(HOBJ2D group,HOBJ2D object);
    virtual BOOL DeleteGroup(HOBJ2D);
    virtual PGroup _CreateGroup();
    TSpace3d * Get3dSpace(HOBJ2D);

    //*------------ meta file --------------------*
    void NewMetaRecord(INT16);
    void NewMetaRecord1(INT16,WORD);
    void NewMetaRecord2(INT16,WORD,WORD);
    void NewUndoRecord(INT16);
    void NewUndoRecord1(INT16,WORD);
    void NewUndoRecord2(INT16,WORD,WORD);
    void NewMetaRecord3d(INT16 code,UINT16 hsp);
    void NewUndoRecord3d(INT16 dode,UINT16 hsp);
    void CloseMetaRecord();
    INT16 DoRedo();
    INT16 DoUndo();
    INT16 ProcessMetaRecord(INT16);
    BOOL CloseMetaFile();
    void WriteLastRecord(long);
    void WriteMetaPoints(PStream);
    //-- Clipboard
    int _StoreObject(PStream,HOBJ2D);
    HOBJ2D MergeWithSpace(PStream,POINT2D delta,INT16,int _m=0);
    // если _m!=0 то происходит запись в метафайл
    void AddRect(RECT); /* добавляет прямоугольник к области обновления
 если lock ноль, то посылается сообщение о перерисовке */
    void AddObjectRect(PObject);

    void UpdateSpecObject(PObject2d);

    HBRUSH  _CreateBrush(PBrush2d);
    HPEN  _CreatePen(PPen2d);

    POINT2D _DrawText(HDC dc,PText_2d pt,RECT UpdRect);//return actual size of Text_2d
    void    _DrawBmpStub(HDC dc,PObject2d pb);
    void    _DrawUser2d(HDC dc,TUserObject2d *po,RECT UpdRect);

    DWORD SetSpaceParam(UINT,DWORD);

    BOOL inline IsVisible(PObject po)
    {
        if (((po->options & 1)==0) &&
            ((((DWORD) (1l << ((HIBYTE(po->options)&0x1f)))) & layers) ==0l)
            )return 1;
        return 0;
    };

    virtual INT32 IsIntersect(HOBJ2D ,HOBJ2D ,UINT32);
    virtual  BOOL PushCrd();
    virtual  BOOL PopCrd();
    BOOL     SetCrd(CRDSYSTEM2D*);
    void DIBtoBITMAP(PDib2d pb,HDC);

    inline void _DrawSpace3d(HDC dc,PView3d_2d pv,RECT& UpdRect);
    inline void _DrawPolyLine(HDC dc,PLine_2d pl);

protected:

    void AddCollection();
    void SetDefault();

    HFONT  _CreateFont(PFont2d pf,INT16 angle);
    void _DrawBitmap(HDC dc,PBitmap_2d pb,PDib2d,DWORD);
    inline void _DrawDoubleBitmap(HDC dc,PDoubleBitmap_2d _pb);
    void _DrawFrame(HDC dc,RECT UpdRect);
    void _DrawFrame3d(HDC dc,RECT UpdRect);
    void _DrawAxis3d(HDC dc,RECT UpdRect);
    void _DrawRCenter(HDC dc);
    void _DrawArrows(HDC,PLine_2d);


    void __CrTXD_7gS(PText2d pt,LOGTEXT * Text,INT16 NumItems );
    void UpdateFrame(PObject2d);//Вызывать если меняется объект
    void UpdateSize();
    inline BOOL  Awaken();
    //-- Для печати текста
    INT16 PrintPRNString(HDC xor,HDC and,PText_2d pt,POINT2D origin,INT16 deltay,POINT2D &);// В Origin без ссылок(&);
    TPRNTEXT * InsertPRNitem(HDC dc,PTextRecord ptr,char * str,int len=-1);
    //*-------------------------------------------*
    BOOL ChkObjects3dtoFit(PAINTSTRUCT3D&ps,RECT&upd,BOOL _min);
    BOOL virtual UpdateTool(int,HOBJ2D,pointer){return 0;};
};

typedef TSpace2d * PSpace2d;
int PointNearLine(POINT2D& l1,POINT2D& l2,POINT2D& p,double width=3);
void RotatePoint(POINT2D & p,POINT2D& base,double f);
void MovePoint(POINT2D & p,POINT2D& delta);
int RectIntersect(RECT r,RECT r1);

void AddPointToRect(RECT & r,POINT2D p);
void AddPointToRect(RECT2D & r,POINT2D p);

int isRectEmpty(RECT r);
void Rect_or_Rect(RECT & r,RECT r1);
void RotateRect(RECT& r,double f);
void RotateRect(RECT2D& r,double f);
HRGN GetRotateRectRGN(RECT r,double f);
long Round(double f);
void GetRotateRectPoints(RECT r,double f,POINT2D*);
void GetRotateRectPoints(RECT2D r,double f,POINT2D*);
void GetRotateRectPoints(RECT r,double f,POINT*);

USERTYPESTRUCT * GetUserType(char * type);

double __GetAngle(POINT2D& delta,POINT2D& Origin);
#endif //_SPACE2D__H_
