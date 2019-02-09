#ifndef __cairorenderer_h__
#define __cairorenderer_h__

#include "_system.h"
#include "cairo-win32.h"

//#pragma comment(lib, "cairo.lib")

#define DLL_CALLCONV _cdecl

//Функции библиотеки Cairo
cairo_t* DLL_CALLCONV (*cw_create) (cairo_surface_t* s);
void DLL_CALLCONV (*cw_set_operator)(cairo_t *cr,cairo_operator_t op);
cairo_operator_t DLL_CALLCONV (*cw_get_operator)(cairo_t *cr);

cairo_surface_t* DLL_CALLCONV (*cw_win32_surface_create) 					(HDC hDc);
cairo_surface_t* DLL_CALLCONV (*cw_surface_create_for_rectangle)	(cairo_surface_t* target,double x,double y,double width,double height);

cairo_surface_t* DLL_CALLCONV (*cw_image_surface_create) 					(cairo_format_t format,int width,int height);
cairo_surface_t* DLL_CALLCONV (*cw_image_surface_create_for_data) (unsigned char *data,cairo_format_t format,int width,int height,int stride);
void DLL_CALLCONV (*cw_mask_surface)(cairo_t *cr,cairo_surface_t *surface,double x,double y);
cairo_surface_t* DLL_CALLCONV (*cw_surface_reference)(cairo_surface_t* surface);
void DLL_CALLCONV (*cw_surface_flush)(cairo_surface_t* surface);

cairo_surface_t* DLL_CALLCONV (*cw_image_surface_create_from_png)(char *filename);
cairo_surface_t* DLL_CALLCONV (*cw_image_surface_create_from_png_stream)(cairo_read_func_t read_func,void *closure);
cairo_status_t DLL_CALLCONV (*cw_surface_write_to_png)(cairo_surface_t *surface, char *filename);
cairo_status_t DLL_CALLCONV (*cw_surface_write_to_png_stream)(cairo_surface_t *surface,cairo_write_func_t write_func,void *closure);

void DLL_CALLCONV (*cw_set_source)(cairo_t *cr,cairo_pattern_t *source);
void DLL_CALLCONV (*cw_set_source_surface)(cairo_t *cr,cairo_surface_t *surface,double x,double y);
cairo_status_t DLL_CALLCONV (*cw_surface_status)(cairo_surface_t *surface);

void DLL_CALLCONV (*cw_set_source_rgba)	(cairo_t *cr,double red, double green, double blue,double alpha);
void DLL_CALLCONV (*cw_set_line_width)	(cairo_t* cr,double width);
void DLL_CALLCONV (*cw_set_line_join)	 	(cairo_t *cr,cairo_line_join_t line_join);
void DLL_CALLCONV (*cw_set_line_cap)		(cairo_t *cr,cairo_line_cap_t line_cap);
void DLL_CALLCONV (*cw_set_dash)				(cairo_t *cr,double *dashes,int count,double offset);

cairo_pattern_t* DLL_CALLCONV (*cw_pattern_create_rgba)(double red, double green, double blue,double alpha);
cairo_pattern_t* DLL_CALLCONV (*cw_pattern_create_linear)(double x0,double y0,double x1,double y1);
cairo_pattern_t* DLL_CALLCONV (*cw_pattern_create_radial)(double x0,double y0, double radius0,double x1,double y1,double radius1);
void DLL_CALLCONV (*cw_pattern_add_color_stop_rgba)(cairo_pattern_t *pattern,double offset,double red,double green,double blue,double alpha);

void DLL_CALLCONV (*cw_move_to)		(cairo_t* cr,double x, 	double y);
void DLL_CALLCONV (*cw_line_to)		(cairo_t* cr,double x, 	double y);
void DLL_CALLCONV (*cw_close_path)(cairo_t* cr);
void DLL_CALLCONV (*cw_curve_to)	(cairo_t* cr,double x1,	double y1, double x2, double y2, double x3, double y3);
void DLL_CALLCONV (*cw_arc)				(cairo_t* cr,double xc,	double yc, double radius, double angle1, double angle2);
void DLL_CALLCONV (*cw_rectangle)	(cairo_t *cr,double x,	double y,double width,double height);

unsigned char * DLL_CALLCONV (*cw_image_surface_get_data)   (cairo_surface_t *surface);
cairo_format_t DLL_CALLCONV  (*cw_image_surface_get_format) (cairo_surface_t *surface);
int DLL_CALLCONV             (*cw_image_surface_get_width)  (cairo_surface_t *surface);
int DLL_CALLCONV             (*cw_image_surface_get_height) (cairo_surface_t *surface);
int DLL_CALLCONV             (*cw_image_surface_get_stride) (cairo_surface_t *surface);
int DLL_CALLCONV             (*cw_format_stride_for_width)  (cairo_format_t format,int width);


void DLL_CALLCONV (*cw_translate)	(cairo_t* cr, double x, double y);
void DLL_CALLCONV (*cw_scale)			(cairo_t* cr, double x, double y);
void DLL_CALLCONV (*cw_rotate)		(cairo_t* cr, double a);
void DLL_CALLCONV (*cw_get_matrix)(cairo_t *cr,cairo_matrix_t *matrix);
void DLL_CALLCONV (*cw_set_matrix)(cairo_t *cr,cairo_matrix_t *matrix);
void DLL_CALLCONV (*cw_identity_matrix)(cairo_t *cr);

void DLL_CALLCONV (*cw_stroke)	(cairo_t* cr);
void DLL_CALLCONV (*cw_fill)		(cairo_t* cr);
void DLL_CALLCONV (*cw_paint)		(cairo_t* cr,double alpha);

void DLL_CALLCONV (*cw_destroy)					(cairo_t* cr);
void DLL_CALLCONV (*cw_surface_destroy)	(cairo_surface_t* surface);
void DLL_CALLCONV (*cw_surface_finish)	(cairo_surface_t* surface);

#define stroke cw_stroke(cr);
#define fill cw_fill(cr);
#define paint(a) cw_paint(cr,a);
#define flush cw_surface_flush(surface);


class TPNGDib2d:public TDib2d
{
protected:
 	cairo_surface_t* png;
public:

 	TPNGDib2d();
 	TPNGDib2d(PStream st,LONG _pos);//TODO: загрузка из файла класса
 	TPNGDib2d(char* filename);
 	~TPNGDib2d();

  cairo_surface_t* GetSurface(){return png;}
 	virtual void Store(PStream);//TODO: сохранение в файл класса
 	virtual WORD WhoIsIt(){return 200;}
  virtual int Awaken(_SPACE*){return 1;}
  virtual BOOL SetPixel(int x,int y,COLORREF rgb){return false;}
  virtual COLORREF GetPixel(int x,int y,BYTE *index){return 0;}
//	cairo_status_t WritePNG(void *closure,unsigned char *data,unsigned int length);
//	cairo_status_t ReadPNG(void *closure,unsigned char *data,unsigned int length);
};
typedef TPNGDib2d * PPNGDib2d;

class CairoRenderer
{
protected:
	PSpace2d ps;
  HDC dc;
	HDC memdc;
	HGDIOBJ hbmp,hbmpold;
  RECT UpdRect;
  RECT SpaceUpdRect;
  RECT LogUpdRect;
  WORD flags;
	cairo_t* cr;//движок для рисования
	cairo_t* wcr;//движок для окна
  cairo_surface_t* surface;//поверхность для рисования части окна
  cairo_surface_t* wsurface;//поверхность для окна
  bool isRenderPart;
  int wX;
  int wY;
  int wW;
  int wH;

  TCollection* textItems;

public:
	CairoRenderer(PSpace2d ps, HDC hDc, RECT* UpdRect,WORD flags);
	bool RenderSpace2D();
protected:


	void CreateSurface();
	void RenderSpaceBackground();
	void RenderSpaceGrid();

	void RenderObjects2D();

	void RenderPolyLine2D(PLine_2d pl);
	void RenderPolylineByPoints(POINT* points,int count,bool close=false,bool curve=false);
	void RenderPolylineArrows(PLine_2d pl);
	void RenderArrow(POINT2D&_p2,POINT2D&_p1,double length,double angle,BOOL f,PLine_2d pl);

	void RenderBitmap(PBitmap_2d pb,PDib2d dib,cairo_operator_t op=NULL);
	void RenderDoubleBitmap(PDoubleBitmap_2d dib);

	void RenderText2D(PText_2d pt);
  TPRNTEXT* AddTextItem(HDC dc,PTextRecord ptr,char* str,int len=-1);
	INT16 RenderTextItems(HDC dc,HDC mdc,PText_2d pt,POINT2D origin,INT16 deltay,POINT2D & _s);

	bool SetRenderAnisotropic(HDC hdc=NULL);
	bool ResetRenderAnisotropic(HDC hdc=NULL);

  void SetPen(PPen2d pp,double alpha=1.0);
  void SetBrush(PBrush2d pb,RECT rect,double alpha=1.0);
  void SetColor(COLORREF c,double alpha=1.0);

  int SetMixMode(int mode);

  void FinishRender();
};

#endif //__cairorenderer_h__
