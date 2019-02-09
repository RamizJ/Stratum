/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			tscheme.h
Author:
			Alexander Shelemekhov
*/
#ifndef RUNTIME
#ifndef SC_SCHEME_EDITOR__3
#define SC_SCHEME_EDITOR__3
enum HARD_MODE{SCHEME_EDITOR,IMAGE_EDITOR,EDITOR};
class TScheme;

#include "TWINDOW.H"
#include "../BC/bc_include_gwin2d3d/GWINTYPE.H"

struct LSTRUCT;

class TSchemeWindow: public TSpaceWindow
{
	BOOL   readonly:1;
public:
 	HARD_MODE hardmode;  // Обязательно первая !!!!!!!!!!!!!!!!!!!    ******
 	HANDLE lastinclipboard;
 	int pathcount;
 	HOBJ2D *_path;
 	TProject *project;

 	TScheme * scheme;
        TGrid* grid;
 	PClass _class;
 	PObject _object;

 	HOBJ2D oldlink;
 	INT16 editpoint;
 	HOBJ2D linkobj;
 	HOBJ2D __frame;
 	HOBJ2D __objects;
 	HOBJ2D __objects2d; // Выделенные двухмерные объекты

 	HOBJ2D linkframe;

 	BOOL   captured:1;
 	BOOL   updateparent:1;
 	BOOL   lastclickvalid:1;
 	TLinkInfo* plink;

        HOBJ2D object, link, object2d, subobject;
 	int state,substate;
 	POINT2D _mousepos,lastpoint,delta,sizer;
 	UINT16   pasteflags;

// 3d --- S T A R T
 	HOBJ2D  object3d,hcamera,hrotate,HFRAME3D,hlocalcs,htmpframe,hmain3d;
 	POINT3D point3d,base,org3d;
 	gr_float size3d;
 	void  * tmpdata;
 	HSP3D   hsp3d;
 	POINT2D view_org,view_size;
 	CAMERA3D camera3d;
 	INT16 submode3d;
 	INT16 mode3d;
 	BYTE  addmode3d; // режим (текстура, edit points i т.д.)
// 3d --- E N D

	TSchemeWindow(LPCREATESTRUCT,HWND);
	~TSchemeWindow();

	void CreateImageFrame();
	void SetClass(PClass,PObject);
	void SetSpace(HSP2D hsp);
	void UpdateObject();
	void ProjectClose(TProject*);

	virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
	virtual void EvLButtonDblClk(WORD modKeys,int xPos,int yPos);
	virtual void EvLButtonDown(WORD modKeys,int xPos,int yPos);
	virtual void EvLButtonUp(WORD modKeys,int xPos,int yPos);
	virtual void EvRButtonDown(WORD modKeys,int xPos,int yPos);
        virtual void EvMouseMove(WORD modKeys,int xPos,int yPos);

	void ObjectMenu(HOBJ2D,POINT);
	virtual BOOL AcceptDrop(DRAGDROPSTRUCT&);
	virtual BOOL ProcessDrop(DRAGDROPSTRUCT&);
	virtual int  IsCommandDisable(int command);

	BOOL RCenter(){return GetObjectType2d(hsp,OID_RCENTER)==otROTATECENTER2D;};
	BOOL Axis3d(){return GetObjectType2d(hsp,OID_AXIS3d)==otAXIS3D;};

	void    DeleteAllEditItems(HSP2D hsp);
	virtual void GetSpaceInfo(char *s);
	void    __ImportObject(char*,int type=0,POINT*point=NULL);
	virtual BOOL  GetBallonStr(BALLONSTRUCT *bs);
	void TrackGroup(POINT2D&);
	void SetObjectPos(POINT2D);
	void SetReadOnly(BOOL);
	void DeleteFrames(HSP2D _hsp=0);
	void Repair(int );

private:
	void    Cm_ImportObject(int type);
	BOOL LocalUndo(BOOL);
	void GetRect2d(RECT& r,HOBJ2D obj);
	void Snap(POINT2D&);
	void DeleteFrame();
	void SetCurrentObject(HOBJ2D o);
	void PrepareNewProjection();
	BOOL InProjection(POINT2D&p);
	HOBJ2D  _GetObjectFromPoint(POINT2D&point);
	HOBJ2D   _GetObject3dFromPoint(POINT2D& point);

	//void _EvLButtonDblClk(WORD modKeys,POINT& point);
	//void _EvRButtonDown(WORD modKeys,POINT& point);
	//void _EvLButtonDown(WORD modKeys,POINT& point);
	//void _EvMouseMove(WORD modKeys, POINT& point);
	//void _EvLButtonUp(WORD modKeys,POINT& point);

	HOBJ2D  GetDefaultPen();
	HOBJ2D  GetDefaultBrush();

	void SetState(int);
	void OpenClassDialog(HOBJ2D obj);
	void OpenLinksDialog(HOBJ2D obj);
	void SelectNext(BOOL);
	INT16  WhatObject(HOBJ2D obj);
	void _DeleteObject2d();
	void _DeleteObject3d();

	POINT2D GetPastePoint();
	void Cm_Paste();
	void Cm_PasteDIB();
	void Cm_PasteTEXT();
	BOOL Cm_PasteRtf();
	BOOL Cm_PasteWMF();
	void Cm_PasteClass();
	void Cm_PasteObj(BOOL _2d=TRUE); // Проверять на наличие 2d дан

	void Cm_Cut();
	void Cm_Copy();
	void Cm_CopyObj();

	void Cm_DuplicateObj(POINT2D * org=NULL);
	void Cm_EditObject();
	void Cm_ActualSize();
	void Cm_ObjVarSave();
	void Cm_ObjVarLoad();
	void Cm_Zoom();
	void Cm_Avi(int command);
	void Cm_NewObject(WORD);

	void SetControlButton();
	void _ObjectMenu(POINT point,HOBJ2D obj,int mouse);
	HMENU CreateView3dPopup();
	void Cm_Poly(WPARAM p);
	void GetObjectName(HOBJ2D obj,char * s);
	void SetText(POINT2D* tp,int);

// объект
// 1 имидж
// 2 контактная площадка
// 3 линк

	PObject GetActiveObject();
	LRESULT KeyDown(WPARAM wVkey,LPARAM dwKeyData);
	LRESULT KeyUp(WPARAM wVkey,LPARAM dwKeyData);
	BOOL  PressButton0(POINT2D& point,UINT16 flags,UINT modKeys);
	BOOL  PressButton3d(POINT2D& point,UINT16 flags,UINT modKeys);
	void  Object3dMenu(POINT);
	void Cm_Names();
	void Cm_CalcOrder();
	void Un_CalcOrder();
	void UpdateCalcOrder();
	void SetImageText(char *txt);
	void UpdateMousePos();
	HOBJ2D GetGRPPen();

	void CreateGRPFrame(HOBJ2D h,HOBJ2D dib,POINT2D&p);
	void CreateLinkFrame();
	void DeleteLinkFrame(HSP2D _hsp=0);

	HGLOBAL _Copy();
	void    _Paste(HGLOBAL,POINT2D*,BOOL allwaysgrp=FALSE);
	virtual void EvCommand(UINT command,HWND hwndCtl,WPARAM wNotifyCode);
	void    SwithcToBaseMode();
	void    AdjustZOrder(HOBJ2D obj);
	void    PostCreateObject2d(HOBJ2D obj);
	void    UpdateLayers();
	void    CreateControl2d(POINT2D& org,POINT2D& size,int _state);

	void    Cm_NewBitmap();
	void    Cm_NewDoubleBitmap();
	virtual BOOL CanClose();
	void Cm_HideObject();
	HMENU CreateObjectMenu(PClass _class,PObject object);
	int   Create3dViewMenu(HMENU*);
	HOBJ2D CreateRotateObj3d();
	void   DeleteRotateObj3d();
	void   GetTargetPoint3d(POINT3D&); // Дает точку, в которую смотрим
	int    GetRotate3dCursor(POINT2D&);
	void   CreateEditFrame3d();
	void   _DeleteEditFrame3d();
	void   CreateEditFrame2d();
	// virtual LRESULT WndProc(UINT, WPARAM,LPARAM);
	int    GetAxis3d();
	int    GetCrdType();
	void   LowQCamera(CAMERA3D&);
	void   SetDragCursor(BOOL ctrl);
	void   DoneLinkPaste();
	void   SetCapture();
	void   ReleaseCapture();
	HOBJ2D ConvertToOne(char*);
	BOOL  SelfPaste(HGLOBAL hglobal,POINT2D* point,PClass _Class,PObject _Object,HSP2D hSpace,LSTRUCT**_items,INT16 &count);
	BOOL   DoFind(BOOL start);

	BOOL   CreateTextureFrame(int );
	HOBJ2D GetObject3d(int mode=0); // 0 - move ,1 - always this
	BOOL   ApplyTexture();
	BOOL   RemoveTexture();
	BOOL   SetAxis3d(int b);
	void   DoneAddMode(BOOL newmode=FALSE); // new add mode will be
	void   CurrChanged();
	BOOL   CreateToolPopup(int id);
	void   DestroyToolPopup(int id);
	BOOL   NewMovePoint3d(POINT2D&_pp,POINT3D&_newpoint,BOOL mvobj=TRUE);
	void   NewLinkCheck(PLinkInfo,HOBJ2D h1=NULL,HOBJ2D h2=NULL);
	HOBJ2D GetMainLinkGroup(HOBJ2D ho); // получает самую большую группу содержащую связь
	HOBJ2D MergeObject2d(char*,char*,POINT2D);
	void   CopyDib(HGLOBAL,BOOL empty=TRUE);
	void   SaveDib(HGLOBAL);
	void   _CreateText2d(LOGTEXT&);
	void  AddGotoMenu(HMENU m,HOBJ2D obj);
	HOBJ2D CreateRectGroup2d(POINT2D &org,POINT2D &size,BOOL rgn=FALSE,BOOL if1=FALSE);
// if1  - Создаст группу даже с одним элементом
	POINT2D GetRectOrg2d();
	void   SetRectOrg2d(POINT2D&);
	void   CreateGrpFrame2d();
	void   SelectRectProc();
	void   UnTrackLine(POINT2D&point2d,BOOL fromup); // Из LButtonUp или CAPTURECHANGED
	void   UnDrag(POINT2D&point2d);
	BOOL   GrpPressProc(HOBJ2D obj,HOBJ2D);
	void   CreateGroupBy1Element(HOBJ2D); // создает группу с одним элементом
	void   RenderFormat(UINT );
	BOOL   ProcessClipboardFormat(UINT);
	void   AfterMove();
	void   GetGroupLinkItems(HOBJ2D h,TCollection **p);
	void   CreateGroupFromSelection();
	void   GetNewImagePos();
};

UINT GetFileType(char*s,int);
BOOL ClassCompare(PClass _child,PObject po,TChildInfo*ci,char *s);
#endif
#endif //RUNTIME
