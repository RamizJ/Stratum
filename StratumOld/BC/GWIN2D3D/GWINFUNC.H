/*
Copyright (c) 1996 TST
graphic library for windows

Module Name:
			graphic.h

 header file for graphic.dll

 functions and same definitions

*/

#ifndef __GRAPHIC_DLL_2D_3D_
#define __GRAPHIC_DLL_2D_3D_
#ifndef __WINDOWS_H
#include <windows.h>
#endif
#ifndef __GRAPH_DLL_TYPES
#include <GWIN2D3D\gwintype.h>
#endif

#define IsReserved(obj)  (((INT16)(obj))>=(INT16)OID_RESERVED)
#define NotReserved(obj)  (((INT16)(obj))<(INT16)OID_RESERVED)
// SetAttribute flags

#ifdef __cplusplus
#ifndef _GRAPHIC_CPP_NAME
extern "C" {            /* Assume C declarations for C++ */
#endif
#endif  /* __cplusplus */

//__declspec(dllimport)
#ifdef _MSC_VER
#define gWINAPI __declspec(dllimport)
#else
#ifdef WIN32
#define gWINAPI __stdcall
#else
#define gWINAPI WINAPI
#endif
#endif
gr_float gWINAPI GetAngleByXY2d(gr_float x,gr_float y);
BOOL   gWINAPI GetInternal2d(INTERNAL2D*);

void   gWINAPI DoneGraphics2d();
HSP2D  gWINAPI CreateSpace2d(HWND win,LPSTR path);
HSP2D  gWINAPI LoadSpace2d( HWND win,LPSTR filename,LPSTR path,LPSTR localpth);
BOOL   gWINAPI State2dSaveAs( HSP2D hSpace2d,LPSTR filename);
BOOL   gWINAPI State2dSave( HSP2D hSpace2d);
BOOL   gWINAPI DeleteSpace2d( HSP2D hSpace2d);
BOOL   gWINAPI EmptySpace2d(HSP2D hSpace2d);
HSP2D  gWINAPI GetNextSpace2d(HSP2D hspPrev);
HSP2D  gWINAPI GetSpace2dBy3d(HSP3D);

HOBJ2D gWINAPI GetNextObject2d( HSP2D hSpace2d,HOBJ2D Object2d);
BOOL   gWINAPI SetTexturePath2d(HSP2D hSpace2d,char*);
BOOL   gWINAPI GetTexturePath2d(HSP2D hSpace2d,char*);


HSP3D  gWINAPI GetNextSpace3d( HSP2D hSpace2d,HSP3D hSpace3d);
LPSTR  gWINAPI GetSpaceFile2d( HSP2D hSpace2d);
BOOL   gWINAPI SetSpaceFile2d( HSP2D hSpace2d,LPSTR);
HWND   gWINAPI AssignWindowToSpace2d(HSP2D hSpace2d,HWND hwnd);
HWND   gWINAPI GetSpaceWindow2d(HSP2D hSpace2d);
BOOL   gWINAPI SetOrgSpace2d( HSP2D hSpace2d, POINT2D* Org2d );
BOOL   gWINAPI IsSpaceChanged2d(HSP2D hSpace2d);
BOOL   gWINAPI IsSpaceRectChanged2d(HSP2D hSpace2d);
BOOL   gWINAPI SetSpaceModify2d(HSP2D hSpace2d,BOOL modify);
BOOL   gWINAPI GetOrgSpace2d( HSP2D hSpace2d ,POINT2D*);
BOOL 	 gWINAPI SetSpaceRenderEngine2d(HSP2D hSpace2d,INT32 id);
BOOL   gWINAPI SetBkBrush2d( HSP2D hSpace2d,HOBJ2D hbrush);
HOBJ2D gWINAPI GetBkBrush2d( HSP2D hSpace2d);
HOBJ2D gWINAPI GetFrameCursor2d(HSP2D hSpace2d,POINT2D *p);
BOOL   gWINAPI UpdateFromFrame2d(HSP2D hSpace2d);
BOOL   gWINAPI SetScaleSpace2d( HSP2D hSpace2d, POINT2D *_MUL,POINT2D *_DIV );
BOOL   gWINAPI GetScaleSpace2d( HSP2D hSpace2d, POINT2D * _MUL,POINT2D *_DIV );
BOOL   gWINAPI GetUpdRect2d( HSP2D hSpace2d, RECT FAR * UpdRect );
BOOL   gWINAPI GetUpdRgn2d( HSP2D hSpace2d, HRGN FAR * hUpdRgn );
BOOL   gWINAPI PaintSpace2d( HSP2D hSpace2d, HDC hDC,RECT FAR * UpdRect,WORD flags);
BOOL   gWINAPI PaintSpace3d( HSP3D hSpace3d ,HDC dc,RECT FAR * r,CAMERA3D FAR *pr);
HPALETTE gWINAPI GetColorPalette2d( HSP2D hSpace2d );
HPALETTE gWINAPI SetColorPalette2d( HSP2D hSpace2d ,HPALETTE);
BOOL    gWINAPI DPtoLP2d( HSP2D hSpace2d ,POINT2D FAR* lppt,INT16 cPoints);
BOOL    gWINAPI LPtoDP2d( HSP2D hSpace2d ,POINT2D FAR* lppt,INT16 cPoints);
BOOL    gWINAPI GetPOINT2D(HSP2D hSpace2d ,POINT2D FAR* point,LPARAM);
BOOL    gWINAPI GetSpaceExtent2d(HSP2D hsp,RECT2D &rect,BOOL usecrd=FALSE);
BOOL    gWINAPI LockUpdChange2d(HSP2D hSpace2d,INT16 lock); // 1 Lock , 0 - UnLock

BOOL    gWINAPI CopyToClipboard2d(HSP2D hSpace2d,HOBJ2D Object2d);
HOBJ2D  gWINAPI PasteFromClipboard2d(HSP2D  hSpace2d,POINT2D *org ,WORD flags);
BOOL    gWINAPI CopyToClipboard3d(HSP3D hSpace3d,HOBJ2D Object3d);
HOBJ2D  gWINAPI PasteFromClipboard3d(HSP3D  hSpace3d,POINT3D FAR* org);
HGLOBAL gWINAPI SaveToMemory2d( HSP2D hSpace2d,INT32 FAR* size);
HSP2D   gWINAPI LoadFromMemory2d( HWND hwnd,HGLOBAL block,LPSTR path,LPSTR localpth);
HOBJ2D  gWINAPI PasteFromMemory3d(HSP3D  hSpace3d,HGLOBAL hg,POINT3D * org);
HGLOBAL gWINAPI CopyToMemory3d(HSP3D hSpace3d,HOBJ2D Object3d,INT32 FAR* size);
HGLOBAL gWINAPI CopyToMemory2d(HSP2D hSpace2d,HOBJ2D Object2d,INT32 FAR* size);
HOBJ2D  gWINAPI PasteFromMemory2d(HSP2D hSpace2d,HGLOBAL hg,POINT2D* org ,WORD flags);


/* Meta File support Functions */ //While not supported
HSP2D  gWINAPI CreateFromMetaFile2d(HWND hwnd,LPSTR FileName,LPSTR);
HSP2D  gWINAPI CreateMetaFile2d(HSP2D hSpace2d,LPSTR FileName);
BOOL   gWINAPI CloseMetaFile2d(HSP2D hSpace2d);
HSP2D  gWINAPI GetMetaFile2d(HSP2D hSpace2d);
INT32  gWINAPI PlayMetaRecord(HSP2D ,INT32 direction);
BOOL   gWINAPI SaveMetaFile2d(HSP2D ,LPSTR FileName);
BOOL   gWINAPI BeginDragDrop2d(HSP2D hSpace2d,HOBJ2D obj2d);
BOOL   gWINAPI EndDragDrop2d(HSP2D hSpace2d,HOBJ2D obj2d);
BOOL   gWINAPI NewMetaGroup2d(HSP2D);
INT32  gWINAPI PlayGroupMetaRecord2d(HSP2D ,INT32 direction);
UINT16 gWINAPI GetNextPrevMetaRecord2d(HSP2D ,INT16 direction);
/* Meta File support Functions */


HOBJ2D gWINAPI GetCurrentTool2d( HSP2D hSpace2d,TOOL_TYPE what );
HOBJ2D gWINAPI SetCurrentTool2d( HSP2D hSpace2d,TOOL_TYPE what,HOBJ2D);

BOOL 	 gWINAPI DeleteTool2d( HSP2D hSpace2d,TOOL_TYPE what,HOBJ2D index);

INT16  gWINAPI DeleteUnusedTools2d( HSP2D hSpace2d,TOOL_TYPE what);
HOBJ2D gWINAPI GetNextTool2d( HSP2D hSpace2d,TOOL_TYPE what,HOBJ2D htool);
UINT16 gWINAPI GetToolType2d( HSP2D hSpace2d,TOOL_TYPE what,HOBJ2D htool );
HOBJ2D gWINAPI GetToolHANDLEbyNum(HSP2D hSpace2d,TOOL_TYPE what,INT16  num);
INT16  gWINAPI GetToolNumByHANDLE(HSP2D hSpace2d,TOOL_TYPE what,HOBJ2D num);
BOOL   gWINAPI UpdateTool2d(HSP2D hSpace2d,TOOL_TYPE what,HOBJ2D,void*);

INT16  gWINAPI GetToolRef2d(HSP2D hSpace2d,TOOL_TYPE what,HOBJ2D htool);
INT16	 gWINAPI GetToolCount2d( HSP2D hSpace2d,TOOL_TYPE what );

//  Type Classification 2D Objects
UINT16 gWINAPI GetObjectType2d3d(HSPACE hSpace,HOBJ2D Object );
BOOL gWINAPI PushCrdSystem(HSPACE hSpace);
BOOL gWINAPI PopCrdSystem(HSPACE hSpace);

#define PushCrdSystem2d(h) PushCrdSystem((HSPACE)h)
#define PopCrdSystem2d(h)  PopCrdSystem((HSPACE)h)

BOOL gWINAPI SetCrdSystem2d(HSP2D hSpace2d,CRDSYSTEM2D*crd);
BOOL gWINAPI GetCrdSystem2d(HSP2D hSpace2d,CRDSYSTEM2D*crd);

#ifdef __cplusplus
inline UINT16 gWINAPI GetObjectType2d(HSP2D hSpace,HOBJ2D Object){return GetObjectType2d3d((HSPACE)hSpace,Object );}
inline UINT16 gWINAPI GetObjectType3d(HSP3D hSpace,HOBJ2D Object){return GetObjectType2d3d((HSPACE)hSpace,Object );}
#endif
// Pen
HOBJ2D gWINAPI CreatePen2d( HSP2D hSpace2d,INT16 Style,INT16 Width,
						 COLORREF rgb, INT16 Rop2=R2_COPYPEN);

BOOL 	gWINAPI SetPenWidth2d( HSP2D hSpace2d,HOBJ2D index,INT16 Width );
BOOL 	gWINAPI SetPenColor2d( HSP2D hSpace2d,HOBJ2D index, COLORREF rgb );
BOOL 	gWINAPI SetPenRop2d( HSP2D hSpace2d, HOBJ2D index,INT16 Rop2 );
BOOL 	gWINAPI SetPenStyle2d( HSP2D hSpace2d,HOBJ2D index,INT16 Style );
INT16 gWINAPI GetPenStyle2d( HSP2D hSpace2d,HOBJ2D index );
INT16	gWINAPI GetPenWidth2d( HSP2D hSpace2d,HOBJ2D index );
INT16 gWINAPI GetPenRop2d( HSP2D hSpace2d,HOBJ2D index );
COLORREF gWINAPI GetPenColor2d( HSP2D hSpace2d,HOBJ2D index );
HPEN	gWINAPI CreateGDIPen2d(HSP2D hSpace2d,HOBJ2D hpen);

// Brush

HOBJ2D gWINAPI CreateBrush2d( HSP2D hSpace2d,INT16 Style,
						 INT16 Hatch,COLORREF rgb,HOBJ2D DibIndex,INT16 Rop2 );
BOOL   gWINAPI SetBrushStyle2d( HSP2D hSpace2d,HOBJ2D hbrush, INT16 Style );
BOOL   gWINAPI SetBrushHatch2d( HSP2D hSpace2d,HOBJ2D hbrush, INT16 Hatch );
BOOL   gWINAPI SetBrushDibIndex2d( HSP2D hSpace2d,HOBJ2D hbrush,HOBJ2D DibIndex );
BOOL   gWINAPI SetBrushColor2d( HSP2D hSpace2d,HOBJ2D hbrush, COLORREF rgb );
BOOL   gWINAPI SetBrushRop2d( HSP2D hSpace2d,HOBJ2D hbrush,INT16 Rop2 );
BOOL   gWINAPI SetBrushPoints2d( HSP2D hSpace2d,HOBJ2D hbrush,double x0,double y0,double x1,double y1);
BOOL   gWINAPI SetBrushColors2d( HSP2D hSpace2d,HOBJ2D hbrush,COLORREF* colors,int colors_count);

INT16  gWINAPI GetBrushStyle2d( HSP2D hSpace2d,HOBJ2D hbrush );
INT16  gWINAPI GetBrushHatch2d( HSP2D hSpace2d,HOBJ2D hbrush );
HOBJ2D gWINAPI GetBrushDibIndex2d( HSP2D hSpace2d,HOBJ2D hbrush );
COLORREF gWINAPI GetBrushColor2d( HSP2D hSpace2d,HOBJ2D hbrush );
INT16  gWINAPI GetBrushRop2d( HSP2D hSpace2d,HOBJ2D hbrush );
HBRUSH gWINAPI CreateGDIBrush2d(HSP2D hSpace2d,HOBJ2D hbrush);

// Dib
HOBJ2D  gWINAPI CreateDib2d( HSP2D hSpace2d, HGLOBAL Dib );
HOBJ2D  gWINAPI CreateDib2dFromResource( HSP2D hSpace2d,HINSTANCE hInstance,LPCSTR lpszBitmap);
HOBJ2D  gWINAPI FCreateDib2d(HSP2D hSpace2d,LPSTR FileName);
HOBJ2D  gWINAPI CreateRDib2d( HSP2D hSpace2d,LPSTR FileName);
HGLOBAL gWINAPI GetDib2d( HSP2D hSpace2d,HOBJ2D DibIndex );
COLORREF gWINAPI DibGetPixel2d( HSP2D hSpace2d,HOBJ2D hdib,int x,int y);
COLORREF gWINAPI DibSetPixel2d( HSP2D hSpace2d,HOBJ2D hdib,int x,int y,COLORREF rgb);
//R
BOOL   gWINAPI SetDib2d( HSP2D hSpace2d, HOBJ2D DibIndex, HGLOBAL Dib );
BOOL   gWINAPI SetRDib2d( HSP2D hSpace2d,HOBJ2D dib,LPSTR FileName);
INT16  gWINAPI GetRDib2d( HSP2D hSpace2d,HOBJ2D dib,LPSTR ,INT16 bufsize);
//DoubleDibs
HOBJ2D gWINAPI CreateDoubleDib2d( HSP2D hSpace2d, HGLOBAL XorDib, HGLOBAL AndDib );
HOBJ2D gWINAPI FCreateDoubleDib2d(HSP2D hSpace2d,LPSTR FileName);

HOBJ2D gWINAPI CreateRDoubleDib2d( HSP2D hSpace2d,LPSTR FileName);
BOOL   gWINAPI SetRDoubleDib2d( HSP2D hSpace2d,HOBJ2D dib,LPSTR FileName);
BOOL   gWINAPI SetDoubleDib2d( HSP2D hSpace2d,HOBJ2D DoubleDibIndex,
										  HGLOBAL XorDib, HGLOBAL AndDib );
HGLOBAL gWINAPI GetXorDib2d( HSP2D hSpace2d, HOBJ2D DoubleDibIndex );
HGLOBAL gWINAPI GetAndDib2d( HSP2D hSpace2d, HOBJ2D DoubleDibIndex );
INT16  gWINAPI GetRDoubleDib2d( HSP2D hSpace2d,HOBJ2D dib,LPSTR,INT16 bufsize);

BOOL gWINAPI Dib2dSaveAs(HSP2D hSpace2d,HOBJ2D DibIndex,char *FileName);
BOOL gWINAPI DoubleDib2dSaveAs(HSP2D hSpace2d,HOBJ2D DibIndex,char *FileName);
// String

HOBJ2D gWINAPI CreateString2d( HSP2D hSpace2d, LPSTR String );
BOOL   gWINAPI SetLogString2d( HSP2D hSpace2d,HOBJ2D index, LPSTR String );
INT16  gWINAPI GetLogString2d( HSP2D hSpace2d,HOBJ2D index, LPSTR String,INT16 MaxLen );

//font

HOBJ2D gWINAPI CreateFont2d( HSP2D hSpace2d, LOGFONT2D FAR* LogFont );
BOOL 	 gWINAPI SetLogFont2d( HSP2D hSpace2d,HOBJ2D index, LOGFONT2D FAR* Logfont );
BOOL   gWINAPI GetLogFont2d( HSP2D hSpace2d,HOBJ2D index, LOGFONT2D FAR* Logfont );

//text

HOBJ2D gWINAPI CreateText2d( HSP2D hSpace2d, LOGTEXT FAR* Text,INT16 NumItems );
BOOL   gWINAPI SetLogText2d( HSP2D hSpace2d,HOBJ2D index, LOGTEXT FAR* Text,INT16 NumItems );
INT16	 gWINAPI GetTextNumItems2d( HSP2D hSpace2d,HOBJ2D index );

INT16  gWINAPI GetLogText2d( HSP2D hSpace2d,HOBJ2D index, LOGTEXT FAR* Text,INT16 MaxItems );
BOOL   gWINAPI PaintText2d(HSP2D hSpace2d,HDC hdc,RECT upd,HOBJ2D htext,INT16 angle);
BOOL   gWINAPI GetTextSize2d(HSP2D hSpace2d,HOBJ2D htext,POINT2D *);

//  Primary 2d objects

BOOL 	 gWINAPI GetActualSize2d(HSP2D hSpace2d,HOBJ2D hobject2d,POINT2D FAR * size);
HOBJ2D gWINAPI CreatePolyline2d( HSP2D hSpace2d,HOBJ2D Pen,HOBJ2D Brush,POINT2D FAR* points,
						 INT16 NumPoints );

HOBJ2D gWINAPI CreateBitmap2d( HSP2D hSpace2d,
						 HOBJ2D DibIndex, POINT2D* SrcOrg, POINT2D FAR* SrcSize,
						 POINT2D* DstOrg, POINT2D* DstSize,INT16 DstAngle=0 );
HOBJ2D gWINAPI CreateDoubleBitmap2d( HSP2D hSpace2d,HOBJ2D DoubleDibIndex,
						 POINT2D* SrcOrg, POINT2D* SrcSize,
						 POINT2D* DstOrg, POINT2D* DstSize,INT16 DstAngle=0  );
HOBJ2D gWINAPI CreateTextRaster2d( HSP2D hSpace2d,HOBJ2D TextIndex,
						 POINT2D* Org, POINT2D* Size, POINT2D  FAR* Delta,INT16 DstAngle=0  );
BOOL   gWINAPI IsObjectVisible(HSPACE,HOBJ2D); // 2d || 3d objects

// Work with PolyLine
BOOL 	gWINAPI AddVectorPoint2d( HSP2D hSpace2d,HOBJ2D VectorObj2d,
						 INT16 pointNum,POINT2D* point );
BOOL  gWINAPI DelVectorPoint2d( HSP2D hSpace2d,HOBJ2D VectorObj2d,
						 INT16 pointNum);
BOOL  gWINAPI SetVectorPoints2d( HSP2D hSpace2d,HOBJ2D VectorObj2d,
						 POINT2D FAR* points, INT16 NumPoints );
BOOL  gWINAPI SetVectorPoint2d( HSP2D hSpace2d,HOBJ2D VectorObj2d,
						 INT16 pointNum,POINT2D* point );

BOOL	gWINAPI GetVectorPoints2d( HSP2D hSpace2d,
						 HOBJ2D VectorObj2d, POINT2D FAR* points,INT16 MaxPoints );
INT16	gWINAPI GetVectorNumPoints2d( HSP2D hSpace2d,HOBJ2D VectorObj2d );
BOOL  gWINAPI GetVectorPoint2d( HSP2D hSpace2d,
						 HOBJ2D VectorObj2d,INT16 pointNum ,POINT2D FAR* point);

HOBJ2D gWINAPI GetPenObject2d( HSP2D hSpace, HOBJ2D Object2d );
BOOL	 gWINAPI SetPenObject2d( HSP2D hSpace, HOBJ2D Object2d, HOBJ2D PenIndex );
HOBJ2D gWINAPI GetBrushObject2d( HSP2D hSpace,HOBJ2D Object2d );
BOOL   gWINAPI SetBrushObject2d( HSP2D hSpace,HOBJ2D Object2d, HOBJ2D BrushIndex );
INT16  gWINAPI GetRgnCreateMode( HSP2D hSpace2d,HOBJ2D Rgn2d);
BOOL   gWINAPI SetRgnCreateMode( HSP2D hSpace2d,HOBJ2D Rgn2d,INT16 mode );

BOOL   gWINAPI SetVectorArrows2d(HSP2D hSpace,HOBJ2D Object2d,ARROW2D*);
BOOL   gWINAPI GetVectorArrows2d(HSP2D hSpace,HOBJ2D Object2d,ARROW2D*);

// Bitmap
// all
BOOL 	gWINAPI SetBitmapSrcRect(HSP2D hSpace2d,HOBJ2D Object2d ,
						 POINT2D FAR* org,POINT2D FAR* size);
BOOL 	gWINAPI GetBitmapSrcRect(HSP2D hSpace2d,HOBJ2D Object2d,
						 POINT2D FAR* org,POINT2D FAR* size);
// one
HOBJ2D gWINAPI GetDibObject2d( HSP2D hSpace,HOBJ2D Object2d );
BOOL 	 gWINAPI SetDibObject2d( HSP2D hSpace,HOBJ2D Object2d,HOBJ2D DibIndex );

//double
HOBJ2D gWINAPI GetDoubleDibObject2d( HSP2D hSpace2d,HOBJ2D Object2d );
BOOL	 gWINAPI SetDoubleDibObject2d( HSP2D hSpace2d,HOBJ2D Object2d,HOBJ2D DoubleDibIndex );

// Text
BOOL	 gWINAPI SetTextObject2d( HSP2D hSpace2d,HOBJ2D Object2d,HOBJ2D TextIndex );
HOBJ2D gWINAPI GetTextObject2d( HSP2D hSpace2d,HOBJ2D Object2d );
BOOL	 gWINAPI SetTextDelta2d( HSP2D hSpace2d,HOBJ2D Object2d ,POINT2D*);
BOOL   gWINAPI GetTextDelta2d( HSP2D hSpace2d,HOBJ2D Object2d,POINT2D*);

//3d Projection
HSP3D gWINAPI Get3dSpace2d( HSP2D hSpace2d,HOBJ2D Object2d );
BOOL  gWINAPI Set3dSpace2d( HSP2D hSpace2d,HOBJ2D Object2d, HSP3D hSpace3d );
// new
BOOL    gWINAPI Set3dViewCamera3d(HSP2D hSpace2d,HOBJ2D Pr3dObject2d,HOBJ2D);
HOBJ2D  gWINAPI Get3dViewCamera3d(HSP2D hSpace2d,HOBJ2D Pr3dObject2d);

// all secondary

BOOL	gWINAPI DelGroupItem2d( HSP2D hSpace2d,HOBJ2D group2d,HOBJ2D Item );
INT16	gWINAPI GetGroupItemsNum2d( HSP2D hSpace2d,HOBJ2D group2d );
BOOL 	gWINAPI DeleteGroup2d( HSP2D hSpace,HOBJ2D Group2d );
INT16   gWINAPI IsGroupContainObject2d(HSP2D hSpace2d,HOBJ2D g,HOBJ2D o);

// -- Group 2d
HOBJ2D  gWINAPI CreateGroup2d(   HSP2D hSpace2d, HOBJ2D FAR* Objects2d,INT16 NumObjects );
BOOL	gWINAPI SetGroupItems2d( HSP2D hSpace2d, HOBJ2D group2d, HOBJ2D FAR* Object2d,INT16 NumObject );
BOOL 	gWINAPI SetGroupItem2d(  HSP2D hSpace2d, HOBJ2D group2d, INT16 ItemNum,HOBJ2D Object2d );
BOOL	gWINAPI AddGroupItem2d(  HSP2D hSpace2d, HOBJ2D group2d, HOBJ2D Object2d );
HOBJ2D  gWINAPI GetGroupItem2d(  HSP2D hSpace2d, HOBJ2D group2d, INT16 ItemNum );
BOOL	gWINAPI GetGroupItems2d( HSP2D hSpace2d, HOBJ2D group2d, HOBJ2D FAR* Object2d,
						 INT16 MaxObjects );


// RGN Group
HOBJ2D	gWINAPI CreateRgnGroup2d( HSP2D hSpace2d, RGNGROUPITEM * items,INT16 NumItems );
BOOL gWINAPI SetRgnGroupItems2d( HSP2D hSpace2d, HOBJ2D group2d,
						 RGNGROUPITEM FAR* Object2d, HOBJ2D NumObject );
BOOL gWINAPI SetRgnGroupItem2d( HSP2D hSpace2d, HOBJ2D group2d,INT16 ItemNum,
						 RGNGROUPITEM FAR* Object2d );
BOOL gWINAPI AddRgnGroupItem2d( HSP2D hSpace2d,HOBJ2D group2d, INT16 ItemNum,
						 RGNGROUPITEM FAR* Object2d );
BOOL gWINAPI GetRgnGroupItems2d( HSP2D hSpace2d,HOBJ2D group2d,
						 RGNGROUPITEM FAR* Object2d,INT16 MaxObjects );
BOOL gWINAPI GetRgnGroupItem2d( HSP2D hSpace2d,HOBJ2D group2d,INT16 ItemNum,
						 RGNGROUPITEM FAR* Object2d );

BOOL gWINAPI DeleteObject2d( HSP2D hSpace,HOBJ2D Object2d );

HOBJ2D gWINAPI GetObjectFromPoint2d( HSP2D hSpace2d, POINT2D *Point ,HOBJ2D FAR* primary=NULL,UINT32 layer=0ul);
HOBJ2D gWINAPI GetSubObjectFromPoint2d( HSP2D hSpace2d, HOBJ2D ParentObject,
						 POINT2D *Point,HOBJ2D FAR* primary=NULL ,UINT32 layer=0ul);
INT16  gWINAPI GetLinePointFromPoint2d( HSP2D hSpace2d,HOBJ2D Vect, POINT2D* Point );
HOBJ2D gWINAPI GetObject3dFromPoint2d( HSP2D hSpace2d,HOBJ2D Pr3dObject2d,
						 POINT2D* pt,HOBJ2D FAR* primary=NULL ,INT16*item=NULL);
HOBJ2D gWINAPI GetSubObject3dFromPoint2d( HSP2D hSpace2d,HOBJ2D Pr3dObject2d,
						 HOBJ2D Parent3d,POINT2D* pt  ,HOBJ2D FAR* primary=NULL);

HOBJ2D	gWINAPI GetObjectItem3dbyPoint2d( HSP2D hSpace2d,HOBJ2D Pr3dObject2d,
						 HOBJ2D object3d,POINT2D*pt ,
						 INT16 *primitive);

int 	gWINAPI GetNearestEdge( HSP2D hSpace2d, int Pr3dObject2d,int object3d,
						 POINT2D point,POINT3D FAR*p);

INT16 	gWINAPI GetPoint3dFromPoint2d( HSP2D hSpace2d,HOBJ2D Pr3dObject2d,
						 HOBJ2D PrObject3d, POINT2D* pt ,BYTE d=2);

BOOL  gWINAPI GetObjectOrgSize2d( HSP2D hSpace2d,HOBJ2D Object2d,POINT2D*o,POINT2D*s );
BOOL  gWINAPI GetObjectRect2d(HSP2D hSpace2d,HOBJ2D Object2d,RECT2D*); // view

BOOL  gWINAPI SetObjectOrg2d( HSP2D hSpace2d, HOBJ2D Object2d, POINT2D *NewOrigin );
BOOL  gWINAPI SetObjectSize2d( HSP2D hSpace2d,HOBJ2D Object2d, POINT2D* NewSize );
BOOL  gWINAPI RotateObject2d( HSP2D hSpace2d,HOBJ2D Object2d, ROTATOR2D FAR* rotator );
gr_float gWINAPI GetObjectAngle2d( HSP2D hSpace2d,HOBJ2D Object2d);
BOOL gWINAPI SetObjectAlpha2d( HSP2D hSpace2d,HOBJ2D Object2d,gr_float alpha);
gr_float gWINAPI GetObjectAlpha2d( HSP2D hSpace2d,HOBJ2D Object2d);

// Z order

BOOL 	gWINAPI ObjectToTop2d( HSP2D hSpace2d,HOBJ2D Object2d );
BOOL  gWINAPI ObjectToBottom2d( HSP2D hSpace2d,HOBJ2D Object2d );
BOOL	gWINAPI SwapObject2d( HSP2D hSpace2d,HOBJ2D First2d,HOBJ2D Second2d );
BOOL 	gWINAPI SetZOrder2d( HSP2D hSpace2d,HOBJ2D Object2d,INT16 ZOrder );
INT16	 gWINAPI GetZOrder2d( HSP2D hSpace2d,HOBJ2D Object2d );
HOBJ2D gWINAPI GetObjectFromZOrder2d( HSP2D hSpace2d,INT16 ZOrder );
HOBJ2D gWINAPI GetTopObject2d( HSP2D hSpace2d );
HOBJ2D gWINAPI GetBottomObject2d( HSP2D hSpace2d );
HOBJ2D gWINAPI GetUpperObject2d( HSP2D hSpace2d,HOBJ2D Object2d );
HOBJ2D gWINAPI GetLowerObject2d( HSP2D hSpace2d,HOBJ2D Object2d );

HOBJ2D gWINAPI GetCurrentObject2d( HSP2D hSpace2d );
HOBJ2D gWINAPI SetCurrentObject2d( HSP2D hSpace2d,HOBJ2D Object2d );
#define SetCurrentObject3d(hSpace3d,Object2d ) SetCurrentObject2d((HSP2D)hSpace3d,Object2d )
#define GetCurrentObject3d(hSpace3d ) GetCurrentObject2d((HSP2D)hSpace3d )

// 2d edit
HOBJ2D gWINAPI CreateEditFrame2d( HSP2D hSpace2d,POINT2D* SizerSize,
						 BYTE f=FRAME_HASPOINTS| FRAME_HASLINES );
HOBJ2D gWINAPI CreateRotateCenter2d( HSP2D hSpace2d, POINT2D* center );
// 3d edit
HOBJ2D gWINAPI CreateEditFrame3d(HSP2D hSpace2d,POINT2D* pSize);
HOBJ2D gWINAPI UpdateFromFrame3d(HSP2D hSpace2d);
HOBJ2D gWINAPI CreateRotateAxis3d( HSP2D hSpace2d,POINT3D FAR*center, int );

//----------------
BOOL   gWINAPI SetAttribute2d( HSP2D hSpace2d, HOBJ2D Object2d,UINT16 Attr ,ATTR_MODE mode);
UINT16 gWINAPI GetAttribute2d( HSP2D hSpace2d, HOBJ2D Object2d );
HOBJ2D gWINAPI GetObjectParent2d(HSP2D hSpace2d,HOBJ2D Object2d);

#define GetShowObject2d(hSpace2d, Object2d )\
 (!(GetAttribute2d(hSpace2d,Object2d) & 1))
#define  SetShowObject2d(hSpace2d,Object2d, enable )\
 SetAttribute2d(hSpace2d,Object2d,1 ,enable ? ATTRRESET : ATTRSET)

// Objects PTR
BOOL 	  gWINAPI PointNearLine2d(POINT2D* p1,POINT2D* p2,POINT2D* p);

BOOL    gWINAPI SetObjectName2d(HSP2D hSpace2d,HOBJ2D Object2d,char*name);
INT16   gWINAPI GetObjectName2d(HSP2D hSpace2d,HOBJ2D Object2d,char*name,INT16 size);
HOBJ2D  gWINAPI GetObjectByName2d(HSP2D hSpace2d,HOBJ2D Object2d,char*name);

DWORD   gWINAPI SetSpaceParam2d( HSP2D hSpace2d,UINT16 command,DWORD data);

// Control
HOBJ2D gWINAPI CreateControlObject2d(HSP2D hsp,
				  LPCSTR lpszClassName,	/* address of registered class name	*/
				  LPCSTR lpszWindowName,	/* address of window text	*/
				  DWORD dwStyle,	/* window style	*/
				  int x,	/* horizontal position of window	*/
				  int y,	/* vertical position of window	*/
				  int nWidth,	/* window width	*/
				  int nHeight,	/* window height	*/
				  INT16 id,
				  DWORD ExdwStyle);
HWND gWINAPI GetControl2d(HSP2D hSpace2d,HOBJ2D Object2d);
DWORD gWINAPI SetControlStyle2d(HSP2D hsp,HOBJ2D Object2d,DWORD style,WORD flags);
// flags |1 установить стиль
// возвращает предыдущий
BOOL gWINAPI GetControlId2d(HWND control,HSP2D * hsp,HOBJ2D *id);

// USER OBJECTS START

BOOL   gWINAPI RegisterUserType2d(USERTYPESTRUCT *_t);
HOBJ2D gWINAPI CreateUserObject2d(HSP2D hsp,USEROBJSTRUCT*us);
BOOL   gWINAPI UpdateObject2d(HSP2D hsp,HOBJ2D Obj,RECT2D far*r);
USEROBJSTRUCT * gWINAPI GetUserData2d(HSP2D hSpace2d,HOBJ2D Object2d);

BOOL gWINAPI SetObjectData2d(HSP2D hSpace2d,HOBJ2D Object2d,UINT16 id,void * far data,UINT16 start,UINT16 size);
UINT16 gWINAPI GetObjectData2d(HSP2D hSpace2d,HOBJ2D Object2d,UINT16 id,void * far data,UINT16 start,UINT16 size);

// ??????????????

BOOL   gWINAPI SetObjectPTR2d(HSP2D hSpace2d,HOBJ2D Object2d,DWORD ptr);
DWORD  gWINAPI GetObjectPTR2d(HSP2D hSpace2d,HOBJ2D Object2d);

INT32  gWINAPI IsObjectsIntersect2d(HSPACE hsp,HOBJ2D obj1,HOBJ2D obj2,UINT32 flags); // 3d/2d

#if !defined(GRAPH_NO3D)
HSP3D  gWINAPI CreateSpace3d( HSP2D hSpace2d);
HOBJ2D gWINAPI Create3dProjection2d(HSP3D hSpace3d,HOBJ2D hCamera,
						 POINT2D* DstOrg, POINT2D* DstSize );
INT16  gWINAPI GetUsageSpace3d( HSP3D hSpace3d );
BOOL   gWINAPI DeleteSpace3d( HSP3D hSpace3d );


// Работа с точками на экране и в миру

BOOL   gWINAPI LP2dtoLP3d(HSP2D hSpace2d,HOBJ2D Obj2dIndex ,LP2DLP3D*lp,POINT2D*,POINT3D*,INT16 count);
BOOL   gWINAPI LP3dtoLP2d( HSP2D hSpace2d,HOBJ2D Obj2dIndex ,POINT2D FAR* lp2d,POINT3D FAR* lp3d,INT16 cPoints,BYTE flags=0);
// flags &1 not use space 2d scale
// flags &2 not use view3d2d offset

BOOL   gWINAPI LP3dtoLP3d( HSP3D hSpace3d,POINT3D FAR* src,POINT3D FAR* dst,INT16 cPoints,BYTE srcf,BYTE dstf,HOBJ2D hview=0);

#define LP3D_WORLD   0
#define LP3D_LOCAL   1
#define LP3D_CURRENT 2
#define LP3D_VIEW    3 // hview must specified
#define LP3D_SIZE    0x80u


// Из координат пространства в координпты проекции
BOOL gWINAPI  LP2dtoView(HSP2D hSpace2d,HOBJ2D Obj2dIndex,POINT2D*point2din,POINT2D*point2dout,INT16 count);

// Переводит из одной системы координат в другую
BOOL  gWINAPI TransformPoint3d( HSP2D hSpace2d,HOBJ2D Obj2dIndex,POINT3D FAR* p,INT16 cPoints,BYTE f);

#define TRANSFORM_WORLD_to_VIEW    1
#define TRANSFORM_VIEW_to_WORLD    0

BOOL	gWINAPI RotatePoint3d(ROTATOR3D FAR* r,POINT3D FAR*p,INT16 c);


//BOOL	gWINAPI TransformCamera3d(CAMERA3D FAR* pr,gr_float f,INT16 c);
BOOL	gWINAPI MakeDefaultCamera(CAMERA3D FAR* p,int view);
BOOL    gWINAPI TrCameraToFeet(HSP2D ,HOBJ2D hView3d,HOBJ2D Hobj3d,gr_float );

//  OBJECTS 3d
HOBJ2D 	gWINAPI CreateObject3d( HSP3D hSpace3d);
INT16	gWINAPI DeleteEmptyObjects3d( HSP3D hSpace3d );
BOOL 	gWINAPI SetObjectOrg3d( HSP3D hSpace3d,HOBJ2D Object3d, POINT3D FAR* Origin );
BOOL	gWINAPI SetObjectSize3d( HSP3D hSpace3d,HOBJ2D Object3d, POINT3D FAR* Size );
BOOL 	gWINAPI RotateObject3d( HSP3D hSpace3d,HOBJ2D Object3d, ROTATOR3D FAR* rotator );
BOOL 	gWINAPI TransformObject3d( HSP3D hSpace3d,HOBJ2D Object3d, gr_float*);

BOOL 	gWINAPI SetObjectColor3d( HSP3D hSpace3d,HOBJ2D Object3d,COLOR3D*color);
INT16   gWINAPI GetObjectColor3d( HSP3D hSpace3d,HOBJ2D Object3d,COLOR3D*color);

HOBJ2D  gWINAPI CreateBar3d(HSP3D hSpace3d,POINT3D FAR*org,
						 POINT3D FAR*size,COLORREF rgb);


BOOL 	gWINAPI GetObjectOrgSize3d( HSP3D hSpace3d, HOBJ2D Object3d, POINT3D * Origin ,POINT3D * Size);


// Camera Functions
HOBJ2D  gWINAPI CreateCamera3d(HSP3D hSpace3d,CAMERA3D*);
BOOL    gWINAPI GetCamera3d(HSP3D hSpace3d,HOBJ2D,CAMERA3D*);
BOOL    gWINAPI SetCamera3d(HSP3D hSpace3d,HOBJ2D,CAMERA3D*);

//  Light Functions
HOBJ2D  gWINAPI CreateLight3d(HSP3D hSpace3d,LIGHT3D*);
BOOL    gWINAPI GetLight3d(HSP3D hSpace3d,HOBJ2D,LIGHT3D*);
BOOL    gWINAPI SetLight3d(HSP3D hSpace3d,HOBJ2D,LIGHT3D*);

// TextureFunction
HOBJ2D  gWINAPI CreateMaterial3d(HSP3D hSpace3d,MATERIAL3D*);
HOBJ2D  gWINAPI SetMaterial3d(HSP3D hSpace3d,HOBJ2D,MATERIAL3D*);
HOBJ2D  gWINAPI GetMaterial3d(HSP3D hSpace3d,HOBJ2D,MATERIAL3D*);
//HOBJ2D  gWINAPI DeleteMaterial3d(HSP3D hSpace3d,HOBJ2D);
HOBJ2D  gWINAPI GetMaterialByName3d(HSP3D hSpace3d,char*s);

//******************************************************//
// Для работы с POINT3D                                 //
//******************************************************//

BOOL 	gWINAPI SetPoints3di( HSP3D hSpace3d,HOBJ2D Object3d,
						 POINT3D FAR* pointa,INT16 * nums,INT16 count);
BOOL	gWINAPI SetPoints3d( HSP3D hSpace3d,HOBJ2D Object3d, POINT3D FAR* points,
						 INT16 start,INT16 count );

INT16 	gWINAPI AddPoints3d( HSP3D hSpace3d,HOBJ2D Object3d, POINT3D FAR* point,INT16 count=1);// count
BOOL 	gWINAPI DelPoints3di( HSP3D hSpace3d,HOBJ2D Object3d,INT16* PointsNum ,INT16 count);
BOOL    gWINAPI DelPoints3d( HSP3D hSpace3d, HOBJ2D Object3d,INT16 from,INT16 count);
INT16	gWINAPI DelUnusedPoints3d( HSP3D hSpace3d,HOBJ2D Object3d );// count
INT32 	gWINAPI GetNumPoints3d( HSP3D hSpace3d,HOBJ2D Object3d );

BOOL	gWINAPI GetPoints3di( HSP3D hSpace3d,HOBJ2D Object3d,POINT3D FAR* points,
				  INT16* numPoints,INT16 MaxPoint);
BOOL 	gWINAPI GetPoints3d( HSP3D hSpace3d,HOBJ2D Object3d,POINT3D FAR* point,INT16 from,INT16 count);

BOOL 	gWINAPI TransformObjectPoints3d( HSP3D hSpace3d,HOBJ2D Object3d, gr_float*);
BOOL 	gWINAPI RotateObjectPoints3d( HSP3D hSpace3d,HOBJ2D Object3d,ROTATOR3D*);
BOOL    gWINAPI RotateObjectPoints3d( HSP3D hSpace3d,HOBJ2D Object3d,ROTATOR3D * rotator);

// Для работы с примитивами
INT16   gWINAPI CreatePrimitives3d(HSP3D hSpace3d,HOBJ2D Object3d,PRIMITIVE3D*,INT16 num);
INT16   gWINAPI CreatePrimitives3di(HSP3D hSpace3d,HOBJ2D Object3d,PRIMITIVE3D*mask,
				 INT16 *points,INT16 numpoints);
BOOL    gWINAPI GetPrimitives3d(HSP3D hSpace3d,HOBJ2D Object3d,PRIMITIVE3D*p,
				INT16 from,INT16 count,
				INT16*pnums,INT16 pnumsize,
				POINT2D*txtPoints,INT16 p2size);
BOOL    gWINAPI SetPrimitives3di(HSP3D hSpace3d,HOBJ2D Object3d,PRIMITIVE3D*mask,
				INT16* items,INT16 count);
BOOL    gWINAPI SetPrimitives3d(HSP3D hSpace3d,HOBJ2D Object3d,PRIMITIVE3D*mask,
				INT16 from,INT16 count);
BOOL 	gWINAPI DelPrimitives3d( HSP3D hSpace3d,HOBJ2D Object3d,INT16* PrimNum,INT16 count);


INT32   gWINAPI GetNumPrimitives3d(HSP3D hSpace3d,HOBJ2D);

// Objects PTR

INT16 gWINAPI GetNextRenderDriver3d(INT16 prev,LPSTR descr,int m);
INT32 gWINAPI GetNumSpacePoints3d( HSP3D hSpace3d );
INT32 gWINAPI GetNumSpacePrimitives3d( HSP3D hSpace3d );
INT16 gWINAPI GetNumPrimary3d( HSP3D hSpace3d );
INT16 gWINAPI GetNumSecondary3d( HSP3D hSpace3d );

BOOL  gWINAPI SetCrdSystem3d(HSP3D hSpace3d,CRDSYSTEM*crd);
BOOL  gWINAPI GetCrdSystem3d(HSP3D hSpace3d,CRDSYSTEM*crd);

// !!!


BOOL gWINAPI GetObjectBase3d( HSP3D hSpace3d,HOBJ2D Object3d, POINT3D * Base);
BOOL gWINAPI SetObjectBase3d( HSP3D hSpace3d,HOBJ2D Object3d, POINT3D * Base);

BOOL gWINAPI SetMatrix3d( HSP3D hSpace3d,HOBJ2D Object3d,void*buf);
BOOL gWINAPI GetMatrix3d( HSP3D hSpace3d,HOBJ2D Object3d,void*buf);


void gWINAPI __MUL_MAT4(void*src1,void*src2,void*dst);
void gWINAPI __INV_MAT4(void*src,void*dst);

BOOL gWINAPI Driver3dDialog(HWND hwnd,char*r3d);

#endif //GRAPH_NO3D

HINSTANCE gWINAPI LoadLibrary2d(char*);
BOOL      gWINAPI FreeLibrary2d(HINSTANCE);

UINT16  gWINAPI GetLastGraphicError();
UINT    gWINAPI GetFileType2d(char*,UINT16*); // точность
HOBJ2D  gWINAPI ImportRaster2d(HSP2D HSpace2d,char*filename,UINT filetype,BOOL* dbl);

#define PushCrdSystem3d(h) PushCrdSystem((HSPACE)h)
#define PopCrdSystem3d(h)  PopCrdSystem((HSPACE)h)


#define  GetNextObject3d(hSpace3d,Object3d) GetNextObject2d( (HSP2D) hSpace3d,Object3d)
#define  GetObjectParent3d(hSpace3d,Object3d) GetObjectParent2d((HSP2D) hSpace3d,Object3d)

#define  SetObjectName3d(hSpace3d,Object3d,name) SetObjectName2d((HSP2D)hSpace3d,Object3d,name)
#define  GetObjectName3d(hSpace3d,Object3d,name,size) GetObjectName2d((HSP2D)hSpace3d,Object3d,name,size)
#define  GetObjectByName3d(hSpace3d,Object3d,name) GetObjectByName2d((HSP2D)hSpace3d,Object3d,name)

#define SetAttribute3d(hSpace3d,Object3d,Attr,mode) SetAttribute2d((HSP2D) hSpace3d,Object3d,Attr,mode)
#define GetAttribute3d(hSpace3d,Object3d) GetAttribute2d((HSP2D) hSpace3d,Object3d)

#define GetShowObject3d(hSpace3d, Object3d )\
 (!(GetAttribute3d(hSpace3d,Object3d) & 1))

#define  SetShowObject3d(hSpace3d,Object3d, enable )\
 SetAttribute3d(hSpace3d,Object3d,1 ,enable ? ATTRRESET : ATTRSET)

#define HideObject3d( hSpace3d, index) SetAttribute3d(hSpace3d, index,1,ATTRSET)
#define ShowObject3d( hSpace3d, index) SetAttribute3d(hSpace3d, index,1,ATTRRESET)


#undef gWINAPI
// USER OBJECTS END

// inline Function
#define PntToPnt(p1,p2) {p1.x=p2.x;p1.y=p2.y;}

#define SetObjectLayer2d(hSpace2d, index,layer)\
 SetAttribute2d(hSpace2d, index,0x1f00u,ATTRRESET);\
 SetAttribute2d(hSpace2d, index,((layer&0x1fu)<<8),ATTRSET)

#define GetObjectLayer2d(hSpace2d, index)\
 ((GetAttribute2d(hSpace2d,index)&0x1f00)>>8)

#define DeleteEditFrame2d(hSpace2d) DeleteObject2d(hSpace2d,OID_FRAME2d)
#define DeleteRotateCenter2d(hSpace2d) DeleteObject2d(hSpace2d,OID_RCENTER)
#define DeleteEditFrame3d( hSpace2d) DeleteObject2d(hSpace2d,OID_FRAME3d)
#define DeleteRotateAxis3d(hSpace2d ) DeleteObject2d(hSpace2d,OID_AXIS3d)
#define Lock2d(hSpace2d) LockUpdChange2d(hSpace2d,1)
#define UnLock2d(hSpace2d) LockUpdChange2d(hSpace2d,0)

	// Set/Get Current tool 2d

#define GetCurrentPen2d(hSpace2d ) GetCurrentTool2d(hSpace2d,PEN2D)
#define GetCurrentBrush2d(hSpace2d ) GetCurrentTool2d(hSpace2d,BRUSH2D)
#define GetCurrentDib2d(hSpace2d ) GetCurrentTool2d(hSpace2d,DIB2D)
#define GetCurrentDoubleDib2d( hSpace2d ) GetCurrentTool2d(hSpace2d,DOUBLEDIB2D)
#define GetCurrentFont2d(hSpace2d ) GetCurrentTool2d(hSpace2d,FONT2D)
#define GetCurrentString2d(hSpace2d ) GetCurrentTool2d(hSpace2d,STRING2D)
#define GetCurrentText2d(hSpace2d) GetCurrentTool2d(hSpace2d,TEXT2D)
#define GetCurrentSpace3d( hSpace2d) GetCurrentTool2d(hSpace2d,SPACE3D)

#define SetCurrentPen2d(hSpace2d, index ) SetCurrentTool2d(hSpace2d,PEN2D, index)
#define SetCurrentBrush2d(hSpace2d, index ) SetCurrentTool2d(hSpace2d,BRUSH2D, index)
#define SetCurrentDib2d(hSpace2d, index ) SetCurrentTool2d(hSpace2d,DIB2D, index)
#define SetCurrentDoubleDib2d(  hSpace2d , index) SetCurrentTool2d(hSpace2d,DOUBLEDIB2D, index)
#define SetCurrentFont2d(  hSpace2d, index ) SetCurrentTool2d(hSpace2d,FONT2D, index)
#define SetCurrentString2d(  hSpace2d, index ) SetCurrentTool2d(hSpace2d,STRING2D, index)
#define SetCurrentText2d( hSpace2d, index) SetCurrentTool2d(hSpace2d,TEXT2D, index)
#define SetCurrentSpace3d( hSpace2d, index) SetCurrentTool2d(hSpace2d,SPACE3D, index)

#define DeletePen2d( hSpace2d, index) DeleteTool2d(hSpace2d,PEN2D,index)
#define DeleteBrush2d( hSpace2d, index) DeleteTool2d(hSpace2d,BRUSH2D,index)
#define DeleteDib2d( hSpace2d, index) DeleteTool2d(hSpace2d,DIB2D,index)
#define DeleteDoubleDib2d( hSpace2d, index) DeleteTool2d(hSpace2d,DOUBLEDIB2D,index)
#define DeleteFont2d( hSpace2d, index) DeleteTool2d(hSpace2d,FONT2D,index)
#define DeleteString2d( hSpace2d, index) DeleteTool2d(hSpace2d,STRING2D,index)
#define DeleteText2d( hSpace2d, index) DeleteTool2d(hSpace2d,TEXT2D,index)



#define HideObject2d( hSpace2d, index) SetAttribute2d(hSpace2d, index,1,ATTRSET)
#define ShowObject2d( hSpace2d, index) SetAttribute2d(hSpace2d, index,1,ATTRRESET)


#ifdef  __cplusplus
#ifndef _GRAPHIC_CPP_NAME
}
#endif
#endif

#endif //__GRAPHIC_DLL_2D_3D_

