
#ifdef WIN32
#define _HUGE
#else
#define _HUGE _huge
#endif

typedef struct tagBREZENHEM{
 long denom,t,e;
}BREZENHEM;

typedef struct tagPOLYGONstruct{
 BYTE _HUGE* Buffer;
 long _HUGE* Zbuffer;
 int         Zperline;
 int         Width;
 int         Height;
 int         perline;
 HDC         hdc;
 int         minx;
 int         miny;
 int         maxx;
 int         maxy;
 INT16       count;
 INT16*      nums;
 LONG        znearclip;
 BYTE        checknearclip;
 _TPOINT3D **points;

 LPBITMAPINFOHEADER txtInfo;
 BYTE _HUGE *   txtBits;
 int           txtperline;
 POINT2D   *   txtPoints;
 int           txtHeight_1;
 int           txtWidth_1;
 int           txtHeight;
 int           txtWidth;

}POLYGONstruct;

// В эту структуру записываются параметры о:
// - области рисования и области отсечения
// - параметрах битовой карты(DIB)
// - параметры полигона

extern POLYGONstruct ps;
BOOL DrawShadedPolygon(COLOR3D color);
BOOL DrawSolidPolygon(COLOR3D);
BOOL tDrawSolidPolygon(_TPRIMITIVE3D*_primitive,PRIMITIVE3D*p,BYTE flags);

// Черчение линий
BOOL DrawSolidLine(_TPOINT3D *,_TPOINT3D *,COLORREF,BREZENHEM* b=NULL);
BOOL DrawShadedLine(_TPOINT3D *,_TPOINT3D *);
BOOL _DrawSolidLine(_TPOINT3D *p1,_TPOINT3D *p2,COLORREF color,BREZENHEM *brezenhem);
// Две верхние не проверяют выход за границы так что... !!!
// Если нужна проверка то вызывай нижние

BOOL DrawPolyline(COLORREF,BYTE flags);
// flags
//  - 1 close line (polygon)
//  - 2 clip

void ZbPutPixel(_TPOINT3D *);

#ifdef WIN32
#define gWINAPI __stdcall
#else
#define gWINAPI WINAPI
#endif


typedef TMATERIAL3D* PMATERIAL3D;

extern PMATERIAL3D (gWINAPI *r3dGetMaterialTexture)(PRIMITIVE3D*);
