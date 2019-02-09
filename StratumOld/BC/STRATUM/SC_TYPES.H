/*
Copyright (c) 1996,1997  Laboratory of United Modeling Knowledjes

Project: Stratum for Windows

Module Name:
			sc_types.h

Author:
			Alexander Shelemekhov

Description:
		 types used in stratum.h
*/

#if !defined(SC_TYPES_HH)
#define SC_TYPES_HH

struct TLibrary;
struct TClassListInfo;
struct TClass;
struct TObject;
struct TLinkEntryInfo;
struct TSetVar;
struct TProject;
struct TEqu;
struct TLink;
struct TVarInfo;
struct TLinkInfo;
struct TChildInfo;
struct TCollection;

typedef void*pointer;
typedef TProject*PProject;
typedef TClass*PClass;
typedef TObject*PObject;
typedef TLink*PLink;
typedef TLinkEntryInfo*PLinkEntryInfo;
typedef TClassListInfo*PClassListInfo;
typedef TVarInfo*PVarInfo;
typedef TLinkInfo*PLinkInfo;
typedef TChildInfo*PChildInfo;

typedef UINT32 (CALLBACK* SCDLLPROC)(UINT16 code,UINT32 p1,UINT32 p2);


struct TSTRATUMINFO
{
	UINT16    size;
 	UINT32    version;
 	HINSTANCE hInstance;
 	HINSTANCE hModule;  // called dll
 	HWND      hMainWindow;
 	SCDLLPROC ScProc;
 	char *    path;
 	UINT16    codepage;
};

struct TSTRATUMVERSION
{
 	UINT16    size;
 	UINT16    hVersion;
 	UINT16    lVersion;
 	UINT16    build;
 	UINT16    flags;
 	UINT16    codepage;
 	char      internalname[64];
 	char      date[24];
 	HINSTANCE hInstance;
};
// flags for TSTRATUMVERSION.flags
#define svfDEMO    1
#define svfRUNTIME 2
#define svfPROTECT 4
#define svfBETA    8
#define svfWIN32   16
#define svfPROF    32 // professional

struct TSTRATUMFILTER
{
 	UINT16    size;
 	HSP2D     hsp2d;
 	HSP3D     hsp3d;
 	HOBJ2D    hobject;
 	LPCSTR    filename;
 	char      type[24];
 	BYTE      enabledialog;
 	UINT32    flags;
};

struct TSTRATUMMENU
{
 	HWND     hWindow;
 	HMENU    hMenu;
 	UINT16   hType;
 	HSP2D    hSpace;
 	HOBJ2D   hObject;
 	UINT32   data1;
 	UINT32   data2;
};

// SCGetWindowInfo
struct TSTRATUMWINDOW
{
 	HWND     hWindow;
 	DWORD    flags;
 	INT16    type;
 	HSP2D    hSpace;
};

class TPropertyButton;

struct TSTRATUMTOOLBAR
{
 	TPropertyButton * buttons;
 	char * title;
 	char   prefix[32];
 	UINT32 flags;
 	int    x,y,cx,cy;
 	int    id;
 	HINSTANCE hTemplate;
 	LPCSTR    sTemplate;
};
// | 1  - Visible
// | 2  - Put in View Menu
// | 4  - hResize
// | 8  - vResize
// | 16 - только в меню


// type flags
#define SCWF_SPACE2D  1
#define SCWF_EDITOR   2
#define SCWF_READONLY 4
#define SCWF_SUBMDI   8

// for TSchemeWindow
#define SCWF_CLASS    256
#define SCWF_OBJECT   512
#define SCWF_SCHEME   1024
#define SCWF_IMAGE    2048


#define SCWT_ERROR   0
#define SCWT_OTHERAPP 16384

#define SCWT_MAIN     1
#define SCWT_MDI      2
#define SCWT_MDICHILD 3
#define SCWT_STATUS   4
#define SCWT_TOOLBAR  5
#define SCWT_TXTEDIT  6
#define SCWT_TREE     7
#define SCWT_SPACE    8 // TMSGWindow
#define SCWT_SCHEME   9

#ifndef STRATUM_INTERNAL

struct TVarInfo
{
 	char * name;
 	char * info;
 	char * def;
 	PClassListInfo type;
 	UINT32 flags;
};

struct  TVAR
{
 	char * name;
 	void * data;
 	TClassListInfo*type;
};

typedef  pointer* TVARARRY;

struct TClassListInfo
{
  UINT32 flags;
  TLibrary *parent;
  char * filename; // имя файла
  union
  {
		char * classname; // либо имя,
		TClass * _class; //  либо класс
  };
 	BOOL   GetFileName(char*,BOOL forsave=TRUE);
 	char*  GetClassName();
 	TClass GetClass();
};

struct TLibrary{
 UINT32 flags;
 TLibrary *parent;
 char*path;
 TCollection*items;
 INT32 Ref;
};
struct TChildInfo{
 PClassListInfo type;
 HOBJ2D handle;      // дескриптор на схеме
 char * name;
 gr_float x,y; // координаты на схеме
 BYTE   flags;
};

struct TLinkEntryInfo{
 char * name1;
 char * name2;
};

struct TLinkInfo{
 HOBJ2D obj1Id;
 HOBJ2D obj2Id;
 HOBJ2D handle;       // дескриптор на схеме
 HOBJ2D varhandle;
 DWORD flags;
 INT16 count;
 PLinkEntryInfo items;
};

struct TClass
{
char*     name;       // имя
PClassListInfo listinfo;
DWORD     flags;      // флаги
PObject   first;      // указатель на первый объект этого класса
char*     description;// описание класса в текстовом виде
UINT32    _classid;   // Уникальный номер класса


long      varsize;    // размер всех переменных(1 состояние) в байтах
							 // для float -8, для long 4.
/* Переменные */
INT16     varcount;   // число переменных
PVarInfo  vars;
INT16     disablevar; // индекс переменной
/* Связи  */
INT16     linkcount;  // число линков
PLinkInfo link;
/* Чилды */
INT16     childcount; // число детей
PChildInfo child;
char*     text;       // исходный текст присваиваний и уравнений
/* Код */
long      codesize;   // виртуальный код
WORD *    VM_code;
TEqu *    equ;
BYTE *    moveinfo;   // то-же
};

struct TObject{
 PClass    _class;     // указатель на класс
 PObject   next;       // указатель на следующий в родительской группе
 PObject   items;      // указатель на первый подэлемент
 PObject   class_next; // указатель на такой же следующий элемент
 PObject   parent;     // Указатнель на родительский объект
 DWORD     flags;      // флаги
 TSetVar*  vardata;
// HSP2D     hSpace;

 TVARARRY  vars;    // указатель на переменные
};
#endif
#endif
