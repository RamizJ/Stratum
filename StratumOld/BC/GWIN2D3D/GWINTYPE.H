/*
Copyright (c) 1996 AAS
graphic library for windows

Module Name:
			gwintype.h

 header file for gwin2d3d.dll

 structures and same definitions

*/


#ifndef __GRAPH_DLL_TYPES
#define __GRAPH_DLL_TYPES
#ifndef _INC_WINDOWS
#include <windows.h>
#endif
#ifndef _NO_OBJECT_TYPES2_3d
#include <gwin2d3d\gwobjt.h>
#endif

#ifndef __BORLANDC__
#ifdef _MSC_VER
#ifdef  _WIN32
#define __FLAT__
#endif
#endif
#endif

#pragma pack( push,1)

// Clipboard format`s

#define CLIP_2D "ALSHL 2D Graphics objects"
#define CLIP_3D "ALSHL 3D Graphics objects"

//-- flags for PasteFromClipboard

#define PFC_MOVEOBJECT 0x8000
#define PFC_SETCURRENT 0x4000
#define PFC_SETFRAME   0x2000
#define PFC_PENS    	  1
#define PFC_BRUHS      2
#define PFC_DIBS       4
#define PFC_DDIBS      8
#define PFC_STRINGS    16
#define PFC_FONTS      32
#define PFC_TEXTS      64
#define PFC_3D         128
#define PFC_ALL        255

// Frame2d flags
#define FRAME_HASPOINTS 1     //  Рисуются точки
#define FRAME_HASLINES  2     //  Рисуется обрамляющая линия
#define FRAME_DASHED    4     //  Обрамляющая линия рисуется точками
#define FRAME_MASK      8     //  Рисуется прямоугольной маской
#define FRAME_NOOBJECT  128   //  Возможно создание без активного объекта


// render types

#define RT_NORENDER      1
#define RT_DEFAULT       2
#define RT_MONOLINES     3
#define RT_PAINTER       4
#define RT_SIMPLEPAINTER 5
#define RT_SCANINTERVAL  6
#define RT_SCANZBUFFER   7
#define RT_MAX           7


// 3d render flags

#define RF_WAITCURSOR    1
#define RF_NOPERSPECTIVE 2

#define SP_SETLAYERS     1
#define SP_GETLAYERS     2
#define SP_SETDEFFLAGS   3
#define SP_GETDEFFLAGS   4
#define SP_GETPALETTE    5
#define SP_SETPALETTE    6
#define SP_SETSPACEFLAG  7
#define SP_GETSPACEFLAG  8
#define SP_SETREADONLY   9
#define SP_GETREADONLY   10

// Space Flags
#define SF_HOOKMOUSE     1
#define SF_GRID          2
#define SF_NOTEQUALTOOLS 4
#define SF_DONTSELECT    8 // Не выбирать

#define PF_DIRECT     1
#define PF_NOCACHEDIB 2
#define PF_DIRECTTEXT 4
#define PF_NOFRAMES   8
#define PF_TOPRINTER  16
#define PF_PRINT  PF_NOCACHEDIB|PF_DIRECT|PF_DIRECTTEXT|PF_NOFRAMES|PF_TOPRINTER

// Сообщения для пользовательских объектов.

#define OM_PAINT      1
#define OM_MOVE       2
#define OM_SIZE       3
#define OM_ROTATE     4
#define OM_LOAD       5
#define OM_SAVE       6
#define OM_DESTROY    7
#define OM_AWAKEN     8
#define OM_POSTSAVE   16
#define OM_POSTLOAD   17
#define OM_SPACEVALID 18
#define OM_ACTUALSIZE 19
#define OM_GETBITMAP  20 // для текстур
#define OM_DESTROYTEXTURE 21

//     - первый параметр имя для текстуры
//     - второй указатель на внутренние данные
//     - второй на LPBITMAPINFO

#define UF_MMTEXT         256
#define UF_MMANIZATROPIC  512



#ifndef INT8
#  define INT8    char
#endif // INT8

#ifndef UINT8
#  define UINT8   unsigned char
#endif // UINT8

#ifndef INT16
#if defined(__FLAT__)
#  define INT16   short
#else
#  define INT16   int
#endif // defined(__FLAT__)
#endif // INT16

#ifndef UINT16
#if defined(__FLAT__)
#  define UINT16  unsigned short
#else
#  define UINT16  unsigned int
#endif // defined(__FLAT__)
#endif // UINT16

#ifndef INT32
#  define INT32   long
#endif // INT32
#ifndef UINT32
#  define UINT32  unsigned long
#endif // UINT32

#ifdef STRICT
#define DECLARE_HANDLE_GWIN(name) struct name##__ { INT16 unused; }; \
		  typedef const struct name##__ NEAR* name
#else
 #define DECLARE_HANDLE_GWIN(name)    typedef INT16 name
#endif

/*
#ifdef STRICT
#define DECLARE_HANDLE_GWIN(name) struct name##__ { INT16 unused; }; \
		  typedef const struct name##__ NEAR* name
#else
 #define DECLARE_HANDLE_GWIN(name)    typedef INT16 name
#endif
*/

/*
#ifdef  __GRAPH_DLL_INTERNAL
typedef INT16 HOBJ2D;
#else
DECLARE_HANDLE_GWIN(HOBJ2D);
#endif
*/
typedef INT16 HOBJ2D; //Пока

DECLARE_HANDLE_GWIN(HSPACE);
DECLARE_HANDLE_GWIN(HSP2D);
DECLARE_HANDLE_GWIN(HSP3D);

#undef DECLARE_HANDLE_GWIN

#define OID_NONE     ((HOBJ2D)0xffffu)
#define OID_RESERVED ((HOBJ2D)32000)
#define OID_RCENTER  ((HOBJ2D)32001)
#define OID_FRAME2d1 ((HOBJ2D)32002)
#define OID_FRAME2d2 ((HOBJ2D)32003)
#define OID_FRAME2d3 ((HOBJ2D)32004)
#define OID_FRAME2d4 ((HOBJ2D)32005)
#define OID_FRAME2d5 ((HOBJ2D)32006)
#define OID_FRAME2d6 ((HOBJ2D)32007)
#define OID_FRAME2d7 ((HOBJ2D)32008)
#define OID_FRAME2d8 ((HOBJ2D)32009)
#define OID_FRAME2d  ((HOBJ2D)32010)
//------------- 3d -----------------------
#define OID_AXIS3d   ((HOBJ2D)32100)
#define OID_FRAME3d1 ((HOBJ2D)32102)
#define OID_FRAME3d2 ((HOBJ2D)32103)
#define OID_FRAME3d3 ((HOBJ2D)32104)
#define OID_FRAME3d4 ((HOBJ2D)32105)
#define OID_FRAME3d5 ((HOBJ2D)32106)
#define OID_FRAME3d6 ((HOBJ2D)32107)
#define OID_FRAME3d7 ((HOBJ2D)32108)
#define OID_FRAME3d8 ((HOBJ2D)32109)
#define OID_FRAME3d  ((HOBJ2D)32110)


#define CAMERA_FRONT       0x1
#define CAMERA_AKSONOM     0x15x

// состояния объектов
#define STATE_HIDDEN         1 // спрятан
#define STATE_EDITING        2 // редактируется
#define STATE_HARDDONTSELECT 4 // абсолютно невыбираем
#define STATE_DONTSELECT     8 // невыбираем
#define STATE_POLYEDGE       16 // дополнительно рисуется окаймление линии
#define STATE_WINDING      128 // полигон заполняется в режиме WINDING иначе ALTERNATE
#define STATE_LAYERS        0x1f00u // маска для слоев

#define OF_HIDDEN         1
#define OF_EDITING        2
#define OF_HARDDONTSELECT 4
#define OF_DONTSELECT     8


typedef double gr_float;

typedef struct tagPOINT2D{
  gr_float x;
  gr_float y;
}POINT2D;

typedef struct tagRECT2D {    /* rc */
  gr_float left;
  gr_float top;
  gr_float right;
  gr_float bottom;
} RECT2D;

typedef POINT2D far *LPPOINT2D;

typedef struct tagLOGTEXT {
	 COLORREF ltFgColor;
	 COLORREF ltBgColor;
	 HOBJ2D ltFontIndex;
	 HOBJ2D ltStringIndex;
} LOGTEXT;

typedef struct tagROTATOR2D
{
	POINT2D center;
	gr_float angle;
} ROTATOR2D;

typedef struct tagARROW2D{
	 gr_float Alength;
	 gr_float Aangle;
	 gr_float Adistance;
	 gr_float Blength;
	 gr_float Bangle;
	 gr_float Bdistance;
	 WORD     flags;
} ARROW2D;

typedef struct tagLOGFONT2D
{
    UINT16  lfSize;
/* Структура аналогичная LOGFONT */
    int     lfHeight;
    int     lfWidth;
    int     lfEscapement;
    int     lfOrientation;
    int     lfWeight;
    BYTE    lfItalic;
    BYTE    lfUnderline;
    BYTE    lfStrikeOut;
    BYTE    lfCharSet;
    BYTE    lfOutPrecision;
    BYTE    lfClipPrecision;
    BYTE    lfQuality;
    BYTE    lfPitchAndFamily;
    char    lfFaceName[LF_FACESIZE];
/* Структура аналогичная LOGFONT */
    LONG    lfFontSize; // Двойной размер шрифта в пунктах
    LONG    lfStyle;    // дополнительные установки
} LOGFONT2D;

// флаги для LOGFONT2D
#define LF2_FSVALID    1 // поле lfFontSize действительно


#define MMTEXT_COORDINATE2D  0
#define XRYU_COORDINATE2D    1
#define XRYD_COORDINATE2D    2
#define XLYU_COORDINATE2D    3
#define XLYD_COORDINATE2D    4

#define MATRIX_COORDINATE2D   16

typedef struct tagCRDSYSTEM2D{
 UINT16   type;
 HOBJ2D   hObject;
 POINT2D  center;
 gr_float matrix[3][3];
}CRDSYSTEM2D;



#if !defined(GRAPH_NO3D)


typedef struct tagPOINT3D {
	 gr_float x;
	 gr_float y;
	 gr_float z;
} POINT3D;

typedef struct tagLP2DLP3D{
 POINT3D  _base;
 gr_float a,b,c,d;
 UINT16   type;
}LP2DLP3D;

typedef struct tag_COLOR3D{
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a;
}_COLOR3D;

typedef union tagCOLOR3D{
 COLORREF rgb;
 _COLOR3D c;
}COLOR3D;

typedef struct tagMATERIAL3D{
	INT16     poSize;
	UINT32    flags;
	HGLOBAL   hdib;
   char   *  name;
	char   *  hfile;
   char      exttype[10];
   gr_float  defXtile;
   gr_float  defYtile;
   union {
    COLOR3D   ambient;     // Ambient color
    COLOR3D    color;
    };
   COLOR3D   diffuse;      // Diffuse color
   COLOR3D   specular;     // Specular color
   COLOR3D   emittance;     // Emitance color
   gr_float  shine;        // Shine factor
   gr_float  transparency; // transparency
}MATERIAL3D;

#define TF_FILE          1 // в том же каталоге что и файл
#define TF_CONSTCOLOR    2 // цвет не меняется от угла
#define TF_EXTERNALDATA  4 // Картинку поставляет приложение

typedef struct tagPRIMITIVE3D{
	INT16     count;              // - количество точек
	UINT16    flags;              // - флаги
   COLOR3D   color;              // - цвет примитива, если нет материала
	INT16   * points;             // - указатель на номера точек
	POINT2D * texturePOINTS;      // - точки на текстуре (каждая координата от 0.0 до 1.0)
	HOBJ2D    material;           // - дескриптор материала
}PRIMITIVE3D;   // Структура описывающая 3d примитив

// Флаги для 3d примитива:
#define PRIMITIVE_POLYGON    0x08  // - полигон
#define PRIMITIVE_POLYLINE   0x10  // - полилиния
#define PRIMITIVE_POINTS     0     // - только точки
#define PRIMITIVE_CLOSELINE  0x01  // - линия закрывается
#define PRIMITIVE_BORDER     0x01  // - рисуется бордер
#define PRIMITIVE_TEXTURED   0x100 // - закрашивается с использованием текстуры
#define PRIMITIVE_ALWAYS     0x200 // - всегда видимы обе стороны поверхности
#define PRIMITIVE_NOSHADE    0x400 // - рисуется без сглаживания

#define PRIMITIVE_SMOTHGROUP 0xf800  // маска для сглаживания

#define MAX_IN_PRIMITIVE     32    // - макисмальное количество точек в примитиве

typedef struct tagPOINT3Di{
 INT32 x,y,z;
}POINT3Di;

typedef struct tagROTATOR3D {
	POINT3D VctOrg;
	POINT3D VctSize;
	gr_float   Angle;
} ROTATOR3D;


#define RENDER_NAME_SIZE 32 // максимальный размер имени отрисовщика


typedef struct tagCAMERA3D{
	INT16     poSize;       // - размер этой структуры в байтах
	POINT3D   poOrg;        // - координаты наблюдателя в мировой системе координат
	POINT3D   poDirection;  // - направление(или целевая точка) взгляда в мировой системе координат
	POINT3D   poUp;         // - вектор определяющий верх наблдателя
	gr_float  poFocus;      // - фокусное расстояние (если <=0 то автоматически)

	gr_float  poFar_Clip;   // - ближния отсекающая плоскость
	gr_float  poNear_Clip;  // - дяльняя отсекающая плоскость
		 
	POINT3D   poExtent;     // - масштабы по всем осям
	POINT2D   poOffset;     // - смещение центра (1 весь диапазон / 0.5 середина).

	gr_float  poHazeScale;
	INT32     poHazeLevels;
	COLOR3D   poHazeColor;
	COLORREF  poBackground; // - фоновый цвет

	char      poRenderName[RENDER_NAME_SIZE];  // имя отрисовщика
	INT16     poRenderType; // - тип отрисовщика
	UINT32    poFlags;      // флаги
	INT16    poPerspective;// тип перспективных преобразований
}CAMERA3D;     // Структура описывающая камеру

// Флаги для камеры:

// это для poFlags
#define CAMERA_PERSPECTIVE  2  // - с перспективой
#define CAMERA_TARGET      32  // Direction  это таргет

// это для poRenderType
#define RENDER_DISABLED   0
#define RENDER_BOUNDED    1
#define RENDER_POINTS     2
#define RENDER_WIREFRAME  3
#define RENDER_FACETED    4
#define RENDER_TYPEMASK   0x0f
                                        
#define RENDER_TRANSPARENT 0x010  // - прозрачная
#define RENDER_TEXTURED   0x020   // - c текстурой
#define RENDER_USELIGHT   0x100
#define RENDER_HAZE       0x200
#define RENDER_FOG        0x400
#define RENDER_SHADED     0x800
#define RENDER_ZSORT      0x1000
#define RENDER_ALLFACES   0x2000
#define RENDER_NOBUFFER   0x4000 //Без буффера
#define RENDER_WAITCURSOR 0x8000ul
               
//poPerspective
#define PERSPECTIVE_NO    0
#define PERSPECTIVE_YES   1

typedef struct tagLIGHT3D{
	INT16    lSize;            // - размер этой структуры в байтах
	INT16    lType;            // - тип источника света
	COLOR3D  lColor;           // - цвет
	POINT3D  lOrg;             // - положение источника света
	POINT3D  lDir;             // - направление освещения
	gr_float lRange;           // - диапазон
	gr_float lFocus;           // - фокус
	gr_float lMinIntensity;    // - ?
	gr_float lMaxIntensity;    // - ?
}LIGHT3D;   // Структура описывающая источник света

// Флаги для источника света:

#define LIGHT_NORMAL 0
#define LIGHT_SPOT   1

// для трехмерных объектов

#define STATE_EDIT3D    2  // Объект рисуется фреймами  //???
#define STATE_WIREFRAME 16 // Объект рисуется фреймами
#define STATE_BOUNDS    32 // Объект рисуется с bounded rect om
#define STATE_AXIS3D    64 // Объект рисуется с Axisom
#define STATE_NOSHADED  128  // без сглаживания
#define STATE_POINTS    0x4000u
#define STATE_ALLFACES  0x8000u  // Видимы все грани

// Системы координат

#define WORLD_COORDINATE  0 // Мировые координаты
#define LOCAL_COORDINATE  1 // Локальные координаты
#define PARENT_COORDINATE 2 // Родительские (первый чилд)
#define VIEW_COORDINATE   3 // Координаты проекции
#define MATRIX_COORDINATE 4 // Задается матрицей


typedef struct tagCRDSYSTEM{
 UINT16   type;
 HOBJ2D   hObject;
 gr_float matrix[4][4];
}CRDSYSTEM;


#endif //GRAPH_NO3D


typedef struct tagRGNGROUPITEM {
	 INT16 Rop;
	 HOBJ2D Object;
} RGNGROUPITEM;

// meta record header

typedef struct tagMETA_RECORD {
	 INT32 PrevRecord;
	 INT16 RedoCode;
	 INT16 UndoCode;
	 INT32 UndoPos;
	 INT32 NextRecord;
} META_RECORD;

// meta file header

typedef struct tagMETASIGNATURE {
	 INT32 SIGNATURE;  // == META
	 INT32 image;      // - offset to vdr file
	 INT32 dir;
	 INT16  fileversion;
	 INT16  minversion;
}METASIGNATURE;

enum TOOL_TYPE{PEN2D=1,BRUSH2D=2,DIB2D=3,DOUBLEDIB2D=4,FONT2D=5,STRING2D=6,TEXT2D=7,SPACE3D=8,TEXTURE3D=9};
enum ATTR_MODE{ATTRSET=1,ATTRRESET=2,ATTRPUT=3};

typedef struct tagUSEROBJSTRUCT{
 char  * type;
 gr_float x,y,sizex,sizey;
 HOBJ2D   handle;
 void  * far data;
 void  * far ptr;
}USEROBJSTRUCT;

typedef LRESULT (CALLBACK* OBJ2DPROC)(HSP2D,USEROBJSTRUCT*,UINT,DWORD);
typedef struct tagUSERTYPESTRUCT{
 char *    name;
 OBJ2DPROC objproc;
 DWORD     flags;
 UINT      usersize;
}USERTYPESTRUCT;


typedef struct tagUSERPAINT{
 HDC     hDc;
 RECT    paint;
 RECT    paintdc;
 POINT2D _org;
 POINT2D _size;
}USERPAINT;

typedef struct tagUSERSTREAM{
 void * far data;
 INT32   size;
 UINT16  flags;
}USERSTREAM;

typedef struct tagINTERNAL2D{
 HSP2D     hSpace2d;
 HOBJ2D    hobject;
 TOOL_TYPE tooltype; // если равно 0 то объект
 UINT32    what;
 UINT32    data1;
 UINT32    data2;
}INTERNAL2D;

/*
10 - для разделяемых битовых карт в (char*)data1 копируется имя
     реально используемого файла.
     data2 - размер буфера
*/
#pragma pack( pop)

#endif //__GRAPH_DLL_TYPES
