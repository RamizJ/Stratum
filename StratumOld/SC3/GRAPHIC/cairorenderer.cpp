/*
Copyright (c) 1995  Virtual World
Module Name:
	 cairorenderer.cpp
Author:
	 Sergey Marchenko
*/

#include "cairorenderer.h"

template<class T>
void LoadFunctionImpl(HMODULE Module, T& a_Function, const char* a_Name)
{
	a_Function = (T)GetProcAddress(Module, a_Name);
}
#define LoadFunction(module, func, name) LoadFunctionImpl(module, func, name);


bool cairoInited=false;
bool InitCairo()
{
	if(cairoInited) return true;
  HMODULE cairo = LoadLibrary("cairowrapper.dll");
  if(!cairo) MessageBox(0,"error load cairo","error",0);;

  LoadFunction(cairo, cw_create, "cw_create");if(!cw_create) return false;
  LoadFunction(cairo, cw_set_operator, "cw_set_operator");if(!cw_set_operator) return false;
  LoadFunction(cairo, cw_get_operator, "cw_get_operator");if(!cw_get_operator) return false;

  LoadFunction(cairo, cw_win32_surface_create, "cw_win32_surface_create");if(!cw_win32_surface_create) return false;
  LoadFunction(cairo, cw_surface_create_for_rectangle, "cw_surface_create_for_rectangle");if(!cw_surface_create_for_rectangle) return false;

  LoadFunction(cairo, cw_image_surface_create, "cw_image_surface_create");if(!cw_image_surface_create) return false;
  LoadFunction(cairo, cw_image_surface_create_for_data, "cw_image_surface_create_for_data");if(!cw_image_surface_create_for_data) return false;
  LoadFunction(cairo, cw_surface_reference, "cw_surface_reference");if(!cw_surface_reference) return false;
  LoadFunction(cairo, cw_surface_flush, "cw_surface_flush");          if(!cw_surface_flush) return false;

  LoadFunction(cairo, cw_image_surface_create_from_png, "cw_image_surface_create_from_png");if(!cw_image_surface_create_from_png) return false;
  LoadFunction(cairo, cw_image_surface_create_from_png_stream, "cw_image_surface_create_from_png_stream");if(!cw_image_surface_create_from_png_stream) return false;
  LoadFunction(cairo, cw_surface_write_to_png, "cw_surface_write_to_png");if(!cw_surface_write_to_png) return false;
  LoadFunction(cairo, cw_surface_write_to_png_stream, "cw_surface_write_to_png_stream");if(!cw_surface_write_to_png_stream) return false;

  LoadFunction(cairo, cw_mask_surface, "cw_mask_surface");if(!cw_mask_surface) return false;

  LoadFunction(cairo, cw_set_source, "cw_set_source");									if(!cw_set_source) return false;
  LoadFunction(cairo, cw_set_source_surface, "cw_set_source_surface");	if(!cw_set_source_surface) return false;
  LoadFunction(cairo, cw_surface_status, "cw_surface_status");					if(!cw_surface_status) return false;

  LoadFunction(cairo, cw_set_source_rgba, "cw_set_source_rgba");	if(!cw_set_source_rgba) return false;
  LoadFunction(cairo, cw_set_line_width, "cw_set_line_width");		if(!cw_set_line_width) return false;
  LoadFunction(cairo, cw_set_line_join, "cw_set_line_join");			if(!cw_set_line_join) return false;
  LoadFunction(cairo, cw_set_line_cap, "cw_set_line_cap");				if(!cw_set_line_cap) return false;
  LoadFunction(cairo, cw_set_dash, "cw_set_dash");								if(!cw_set_dash) return false;

  LoadFunction(cairo, cw_pattern_create_rgba, "cw_pattern_create_rgba");if(!cw_pattern_create_rgba) return false;
  LoadFunction(cairo, cw_pattern_create_linear, "cw_pattern_create_linear");if(!cw_pattern_create_linear) return false;
  LoadFunction(cairo, cw_pattern_create_radial, "cw_pattern_create_radial");if(!cw_pattern_create_radial) return false;
  LoadFunction(cairo, cw_pattern_add_color_stop_rgba, "cw_pattern_add_color_stop_rgba");if(!cw_pattern_add_color_stop_rgba) return false;

  LoadFunction(cairo, cw_move_to, "cw_move_to");      if(!cw_move_to) return false;
  LoadFunction(cairo, cw_line_to, "cw_line_to");      if(!cw_line_to) return false;
  LoadFunction(cairo, cw_close_path, "cw_close_path");if(!cw_close_path) return false;
  LoadFunction(cairo, cw_arc, "cw_arc");              if(!cw_arc) return false;
  LoadFunction(cairo, cw_curve_to, "cw_curve_to");    if(!cw_curve_to) return false;
  LoadFunction(cairo, cw_rectangle, "cw_rectangle");  if(!cw_rectangle) return false;

  LoadFunction(cairo, cw_image_surface_get_data, "cw_image_surface_get_data");     if(!cw_image_surface_get_data) return false;
  LoadFunction(cairo, cw_image_surface_get_format, "cw_image_surface_get_format"); if(!cw_image_surface_get_format) return false;
  LoadFunction(cairo, cw_image_surface_get_width, "cw_image_surface_get_width");   if(!cw_image_surface_get_width) return false;
  LoadFunction(cairo, cw_image_surface_get_height, "cw_image_surface_get_height"); if(!cw_image_surface_get_height) return false;
  LoadFunction(cairo, cw_image_surface_get_stride, "cw_image_surface_get_stride"); if(!cw_image_surface_get_stride) return false;
  LoadFunction(cairo, cw_format_stride_for_width, "cw_format_stride_for_width");   if(!cw_format_stride_for_width) return false;

  LoadFunction(cairo, cw_translate, "cw_translate");  if(!cw_translate) return false;
  LoadFunction(cairo, cw_scale, "cw_scale");          if(!cw_scale) return false;
  LoadFunction(cairo, cw_rotate, "cw_rotate");        if(!cw_rotate) return false;
  LoadFunction(cairo, cw_get_matrix, "cw_get_matrix");        		if(!cw_get_matrix) return false;
  LoadFunction(cairo, cw_set_matrix, "cw_set_matrix");        		if(!cw_set_matrix) return false;
  LoadFunction(cairo, cw_identity_matrix, "cw_identity_matrix"); 	if(!cw_identity_matrix) return false;

  LoadFunction(cairo, cw_stroke, "cw_stroke");        if(!cw_stroke) return false;
  LoadFunction(cairo, cw_fill, "cw_fill");            if(!cw_fill) return false;
  LoadFunction(cairo, cw_paint, "cw_paint");          if(!cw_paint) return false;

  LoadFunction(cairo, cw_destroy, "cw_destroy");									if(!cw_destroy) return false;
  LoadFunction(cairo, cw_surface_destroy, "cw_surface_destroy");	if(!cw_surface_destroy) return false;
  LoadFunction(cairo, cw_surface_finish, "cw_surface_finish");		if(!cw_surface_finish) return false;

  cairoInited=true;
  return true;
}

CairoRenderer::CairoRenderer(PSpace2d ps, HDC hDc, RECT* UpdRect,WORD flags)
{
	if(!InitCairo())return;

  UpdRect->left-=5;//hack для косяка с непрорисовкой линий при скроллировании окна
  UpdRect->top-=5;
  UpdRect->right+=5;
  UpdRect->bottom+=5;

	this->ps=ps;
  this->dc=hDc;
  this->flags=TSpace2d::paint_flags=flags;

  this->wX=UpdRect->left;
	this->wY=UpdRect->top;
  this->wW=UpdRect->right-UpdRect->left;
	this->wH=UpdRect->bottom-UpdRect->top;

  ps->wndupdaterect=(*UpdRect);

  CreateSurface();

  POINT2D delta=ps->org;
  if(isRenderPart)
  {
    INT16 _x=(UpdRect->left+delta.x);
    INT16 _y=(UpdRect->top+delta.y);
    delta.x=-(_x&7);
    delta.y=-(_y&7);
  }
  else
  {
    INT16 _x=(delta.x);
    INT16 _y=(delta.y);
    delta.x=-(_x&7);
    delta.y=-(_y&7);
  }
 	SetBrushOrgEx(memdc,delta.x,delta.y,NULL);
	SetStretchBltMode(memdc,COLORONCOLOR);  //нужно для корректного масштабирования изображений

	RECT r=(*UpdRect);
	ps->_DpToLp((LPPOINT) &r, 2);
  { // Идет проверка на необходимость увеличения области,
		// чтоб не было полос
		BOOL 	b1=ps->scale_mul.x>ps->scale_div.x,
     			b2=ps->scale_mul.y>ps->scale_div.y;
 		if (b1 || b2)
 	  {
 			RECT r1=r;
 			ps->_LpToDp((LPPOINT) &r1, 2);
      if (r1.bottom<UpdRect->bottom)	r.bottom++;
 			if (r1.top>UpdRect->top)				r.top--;
 			if (r1.right<UpdRect->right)		r.right++;
 			if (r1.left>UpdRect->left)			r.left--;
  		if (r.top==r.bottom)						r.top--;
 			if (r.left==r.bottom)						r.left--;
		}
	}
  this->UpdRect=(*UpdRect);
  this->SpaceUpdRect=r;
  /*UpdRect->left		=r.left;
  UpdRect->right	=r.right;
  UpdRect->top		=r.top;
  UpdRect->bottom	=r.bottom;*/
}

void CairoRenderer::CreateSurface()
{
  this->surface = this->wsurface =cw_win32_surface_create(dc);
  this->cr = this->wcr = cw_create(wsurface);
  isRenderPart=!(flags&PF_DIRECT);
  if(isRenderPart)
  {
    hbmp=CreateCompatibleBitmap(dc,wW,wH);
    memdc=CreateCompatibleDC(dc);
    hbmpold=SelectObject(memdc,hbmp);

	  this->surface = cw_win32_surface_create(memdc);
	  this->cr = cw_create(surface);

	  ps->membitmaporg.x=wX;
 		ps->membitmaporg.y=wY;
  }
  else
  {
  	memdc=dc;
	  ps->membitmaporg.x=0;
  	ps->membitmaporg.y=0;
  }
  cw_translate(cr,-wX-ps->org.x,-wY-ps->org.y);
  cw_scale(cr,(double)ps->scale_mul.x/(double)ps->scale_div.x,
    		(double)ps->scale_mul.y/(double)ps->scale_div.y);
	flush
}

bool CairoRenderer::RenderSpace2D()
{
	if(!InitCairo())return false;

  RenderSpaceBackground();//TODO
  RenderSpaceGrid();//TODO
  RenderObjects2D();

  ps->SetTextMode(memdc);
  ps->View(memdc,SpaceUpdRect); //дорисовывает глифы

  FinishRender();

  return true;
}

void CairoRenderer::RenderSpaceBackground()
{
	if (ps->BkBrush)
	{
 		PDib2d dib=ps->BkBrush->dib?(PDib2d)ps->BkBrush->dib->GetMainObject():NULL;
  	if ((dib) &&(dib->H!=8 || dib->W!=8))
	  {
   		if(!(dib->bitmap))
   	  {
  			dib->imagesize.x=dib->W;
  			dib->imagesize.y=dib->H;
	 			ps->DIBtoBITMAP(dib,dc);
   		}
  		if(dib->bitmap)
     	{
	 			int dx,dy,cx,cy;
   			PaintBmpProc(ps->org.x,wX,wX+wW,dib->W,dx,cx);
  			PaintBmpProc(ps->org.y,wY,wY+wH,dib->H,dy,cy);
     	  HDC mdc=CreateCompatibleDC(dc);
				HGDIOBJ hbmpold=SelectObject(mdc,dib->bitmap);

	 			for(int i=0;i<cy;i++)
    		{
   				int y=dib->H*i+dy;
     			for(int j=0;j<cx;j++)
       	  {
	       		int x=dib->W*j+dx;
        		BitBlt(memdc,x,y,dib->W,dib->H,mdc,0,0,SRCCOPY);
	     		}
    		}
  			SelectObject(mdc,hbmpold);
				DeleteDC(mdc);
	 		}
  	}
   	else
	  {
   		HGDIOBJ  b=ps->_CreateBrush(ps->BkBrush);
  		HGDIOBJ  bo=SelectObject(memdc,b);
  		if (hbmp)
	     	PatBlt(memdc,0,0,wW,wH,PATCOPY);
      else
			  PatBlt(dc,wX,wY,wW,wH,PATCOPY);
  		SelectObject(memdc,bo);
	 		DeleteObject(b);
  	}
	}
  else
	{
		if (!(ps->State&SF_GRID))
   	{
			if (isRenderPart)
      	PatBlt(memdc,0,0,wW,wH,WHITENESS);
   	  else
				PatBlt(memdc,wX,wY,wW,wH,WHITENESS);
	  }
  }
}

void CairoRenderer::RenderSpaceGrid()
{
	if (!(ps->State&SF_GRID))return;
  HGDIOBJ b=GetCrossBrush();
 	HGDIOBJ  bo=SelectObject(memdc,b);
  DWORD fdwRop;
  COLORREF oldt,oldbk;
 	if (ps->BkBrush)
  {
    fdwRop=PATINVERT;
 	  oldt=SetTextColor(memdc,RGB(51,51,51));
    oldbk=SetBkColor(memdc,RGB(0,0,0));
  }
 	else
  {
   	fdwRop=PATCOPY;
    oldt=SetTextColor(memdc,RGB(204,204,204));
 	  oldbk=SetBkColor(memdc,RGB(255,255,255));
  }
  UnrealizeObject(b);
 	if(hbmp)	PatBlt(memdc,0,0,wW,wH,fdwRop);
  else   	  PatBlt(dc,wX,wY,wW,wH,fdwRop);
  SelectObject(memdc,bo);
  SetTextColor(memdc,oldt);
 	SetBkColor(memdc,oldbk);
}

void CairoRenderer::RenderObjects2D()
{
	C_TYPE i;PObject2d po;
	for (i=0;i<ps->primary->count;i++)
	{
  	po=(PObject2d)ps->primary->At(i);
		if (ps->IsVisible(po)&&(RectIntersect(SpaceUpdRect,po->GetRect(1))))
		{
	    ResetRenderAnisotropic();
    	switch(po->WhoIsIt())
      {
		  	case 20:RenderPolyLine2D((PLine_2d)po); 	break;
		  	case 21:RenderBitmap((PBitmap_2d)po,(PDib2d)(((PBitmap_2d)po)->dib->GetMainObject())); break;
			  case 22:RenderDoubleBitmap((PDoubleBitmap_2d)po); break;
			  case 23:RenderText2D((PText_2d)po); break;
			  case 24:
        	ps->SetTextMode(memdc);
	        ps->_DrawSpace3d(memdc,(PView3d_2d)po,SpaceUpdRect);
					break;
		  	case 25:
        	ps->SetTextMode(memdc);
        	ps->_DrawUser2d(memdc,(TUserObject2d*)po,SpaceUpdRect);
		  }
    }
  }
}

void CairoRenderer::RenderPolyLine2D(PLine_2d pl)
{
	if(pl->brush && (pl->brush->style==BS_HATCHED
  	|| pl->brush->style==BS_PATTERN || pl->brush->dib!=NULL))
  {
  	ps->_DrawPolyLine(memdc,pl);
  	return;
  }

	int r2;
	int oldfill;
  bool havepen=false,
  		 havebrush=false;

  if(pl->pen && pl->pen->style!=PS_NULL)havepen=true;
  if(pl->brush && pl->brush->style!=BS_NULL)havebrush=true;;
	if(!havepen && !havebrush)return;

	//SetRenderAnisotropic();
/*	if ((pl->owner)&&(pl->owner->WhoIsIt()==4))//TODO
	{
  	if(pl->owner->items->At(0)==pl) // Чертится группа регионов
		{
    	HGDIOBJ bo,b;
		 	if(pl->brush)
      {
      	b=_CreateBrush(pl->brush);
				bo=SelectObject(dc,b);
		 		HRGN rgn=((PRGroup2d)pl->owner)->GetRegion();
		 		if (pl->brush->rop2!=R2_COPYPEN)
      		r2=SetROP2(dc,pl->brush->rop2);
      	PaintRgn(dc,rgn);
		 		if (pl->brush->rop2!=R2_COPYPEN)
      		SetROP2(dc,r2);
		 		::DeleteObject(rgn);
		 		SelectObject(dc,bo);
		 		::DeleteObject(b);
			}
		}
  }
  else*/
	{
	  if(havepen)
    {
	 		if(havebrush)
      {
        	r2=SetROP2(dc,pl->brush->rop2);
					//HGDIOBJ hp001;
					//hp001=SelectObject(dc,GetStockObject(NULL_PEN));
					//oldfill=SetPolyFillMode(dc,pl->GetRgnCreateMode());

					SetBrush(pl->brush,pl->GetRect(),pl->GetAlpha());
					RenderPolylineByPoints(pl->_points,pl->count);
          fill

					//SetPolyFillMode(dc,oldfill);
					//SelectObject(dc,hp001);
					SetMixMode(pl->pen->rop2);
					SetPen(pl->pen,pl->GetAlpha());
					RenderPolylineByPoints(pl->_points,pl->count,true);
      		stroke
					SetMixMode(r2);
			}
			else
      {
				if (pl->pen->rop2!=R2_COPYPEN)	r2=SetMixMode(pl->pen->rop2);
        
				SetPen(pl->pen,pl->GetAlpha());
				RenderPolylineByPoints(pl->_points,pl->count);
        stroke
				RenderPolylineArrows(pl);

				if (pl->pen->rop2!=R2_COPYPEN) 	SetMixMode(r2);
			}
		}
    else
		{
    	//HGDIOBJ hp;hp=SelectObject(dc,GetStockObject(NULL_PEN));
			if (pl->brush->rop2!=R2_COPYPEN)
				r2=SetMixMode(pl->brush->rop2);

		 	//oldfill=SetPolyFillMode(dc,pl->GetRgnCreateMode());
			SetBrush(pl->brush,pl->GetRect(),pl->GetAlpha());
			RenderPolylineByPoints(pl->_points,pl->count);
      fill
		 	//SetPolyFillMode(dc,oldfill);

		 	if (pl->brush->rop2!=R2_COPYPEN)
      	SetMixMode(r2);
			//SelectObject(dc,hp);
	 	}
	}
	/*if (pl->options & STATE_POLYEDGE)
  {
	  r2=SetROP2(dc,R2_XORPEN);
	  HPEN pen=::CreatePen(PS_DOT,0,RGB(255,255,255));
	  HGDIOBJ old=SelectObject(dc,pen);
	  Polyline(dc,pl->_points, pl->count );
	  SelectObject(dc,old);
	  DeleteObject(pen);
	  SetROP2(dc,r2);
	}
	SetTextMode(dc);
	if (pl->options & STATE_EDITING)
	{
  	POINT c;int i;
	  SelectObject(dc,GetStockObject(WHITE_PEN));
	  r2=SetROP2(dc,R2_XORPEN);
	  for (i=0;i<pl->count;i++)
		{
    	c=pl->_points[i];
		 	_LpToDp(&c,1);
		 	_MRect(dc,c);
		}
	  SetROP2(dc,r2);
	}      */
}

void CairoRenderer::RenderPolylineByPoints(POINT* points,int count,bool close,bool curve)
{
	if(count<1)return;
  cw_move_to(cr,points[0].x,points[0].y);
  if(curve && count>2)
  {
  	double qx1,qy1,qx2,qy2,x1,y1,x2,y2,x,y;
  	int i=1;
	  for(;i<count-1;i+=2)
    {
/*		  cw_curve_to(cr,
      	points[i].x,points[i].y,
        points[i+1].x,points[i+1].y,
        points[i+2].x,points[i+2].y
      );
      continue;*/
	      x = points[i-1].x;
	      y = points[i-1].y;
	      qx1 = points[i].x;
	      qy1 = points[i].y;
	      qx2 = points[i+1].x;
	      qy2 = points[i+1].y;

	  /* We need to convert the quadratic into a cubic bezier. */
	  //x1 = x + (qx1 - x) * 2.0 / 3.0;
	  //y1 = y + (qy1 - y) * 2.0 / 3.0;

	  //x2 = x1 + (qx2 - x) / 3.0;
	  //y2 = y1 + (qy2 - y) / 3.0;
	  x1 = (x + qx1 * 2.0) / 3.0;
	  y1 = (y + qy1 * 2.0) / 3.0;
	  x2 = (qx1*2.0 + qx2) / 3.0;
	  y2 = (qy1*2.0 + qy2) / 3.0;

	  cw_curve_to (cr, x1, y1, x2, y2, qx2, qy2);
    }

  }
  else
  {
	  for(int i=1; i<count;i++)
		  cw_line_to(cr,points[i].x,points[i].y);
  }
  if(close)cw_close_path(cr);
}

void CairoRenderer::RenderPolylineArrows(PLine_2d pl)
{
	if(pl->arrows && pl->count)
  {
  	if (pl->arrows->Alength!=0)
    {
   		RenderArrow(pl->points[pl->count-2],pl->points[pl->count-1],
	 			pl->arrows->Alength,pl->arrows->Aangle,pl->arrows->flags&1,pl);
  	}
  	if(pl->arrows->Blength!=0)
    {
  		RenderArrow(pl->points[1],pl->points[0],
	 			pl->arrows->Blength,pl->arrows->Bangle,pl->arrows->flags&2,pl);
  	}
 	}
}

void CairoRenderer::RenderArrow(POINT2D&_p2,POINT2D&_p1,double length,double angle,BOOL f,PLine_2d pl)
{
	POINT2D p1,p2;
	double a=__GetAngle(_p2,_p1);
	p1=_p1;
	p1.x+=length;
	RotatePoint(p1,_p1,a);
	p2=p1;
	RotatePoint(p1,_p1,M_PI+angle);
	RotatePoint(p2,_p1,M_PI-angle);
	POINT p[4];
	PntToPnt(p[0],p1);
	PntToPnt(p[1],_p1);
	PntToPnt(p[2],p2);

	if (f && pl->pen)
  {
  	RenderPolylineByPoints(p,3);
    fill
	 	/*HGDIOBJ b=CreateSolidBrush(pl->pen->color);
	 	HGDIOBJ old=SelectObject(dc,b);
	 	Polygon(dc,p,3);
	 	SelectObject(dc,old);
	 	DeleteObject(b);*/
	}
  else
  {
  	RenderPolylineByPoints(p,3);
    stroke
  	//Polyline(dc,p,3);
  }
}


void CairoRenderer::RenderBitmap(PBitmap_2d pb,PDib2d dib,cairo_operator_t op)
{
  if(dib)
  {
  	POINT DIBsize,dstsize,origin;
		PntToPnt(origin,pb->origin);
		PntToPnt(dstsize,pb->size);

		ps->_LpToDpWO(&dstsize,1);
		ps->_LpToDp(&origin,1);

		DIBsize.x=dib->W;DIBsize.y=dib->H;

		/*BOOL cb=(((long)(dib->perline)*(dib->H)<1024*1024)
		  &&((flags&PF_NOCACHEDIB)==0) );*/

		if((//(DIBsize.x!=dib->imagesize.x)||(DIBsize.x!=dib->imagesize.x)||
		  (dib->bitmap==NULL) && dib->WhoIsIt()!=200)/* && cb*/)
	  {
			dib->imagesize=DIBsize;
			ps->DIBtoBITMAP(dib,memdc);
	  }

		if(dib->bitmap || dib->WhoIsIt()==200/* && cb*/)
		{
	  	POINT srcsize; PntToPnt(srcsize,pb->SrcSize);
	  	POINT srcorg; PntToPnt(srcorg,pb->SrcOrigin);

     	if(srcorg.x>dib->W)srcorg.x=0;
      if(srcorg.y>dib->H)srcorg.y=0;
	  	if((srcsize.x+srcorg.x)>dib->W)srcsize.x=dib->W-srcorg.x;
	  	if((srcsize.y+srcorg.y)>dib->H)srcsize.y=dib->H-srcorg.y;

      HDC mdc=0;
      HGDIOBJ hbmpold=0;
      cairo_surface_t	*s=0;

    	if(dib->WhoIsIt()==200){s=((PPNGDib2d)dib)->GetSurface();}
      else
      {
      	mdc = CreateCompatibleDC(memdc);
      	hbmpold=SelectObject(mdc, dib->bitmap);
      	s = cw_win32_surface_create(mdc);
     	}
     	cairo_surface_t *scrop = cw_surface_create_for_rectangle(
      	s,srcorg.x,srcorg.y,srcsize.x,srcsize.y);

      RECT r = pb->GetRect();
      cairo_matrix_t m;
      cw_get_matrix(cr,&m);
      cw_translate(cr,(double)r.left+0.5*(double)(r.right-r.left),(double)r.top+0.5*(double)(r.bottom-r.top));
      cw_rotate(cr,pb->GetAngle());
      cw_scale(cr
      	,(double)dstsize.x/(double)srcsize.x*(double)ps->scale_div.x/(double)ps->scale_mul.x
      	,(double)dstsize.y/(double)srcsize.y*(double)ps->scale_div.y/(double)ps->scale_mul.y);
      cw_translate(cr,-0.5*(double)srcsize.x,-0.5*(double)srcsize.y);

      cairo_operator_t opOld;
      if(op)
      {
        opOld = cw_get_operator(cr);
        cw_set_operator(cr,op);
      }
      cw_set_source_surface(cr,scrop,0.0,0.0);
      paint(pb->GetAlpha());
      cw_set_source_surface(cr,surface,0.0,0.0);
      cw_set_matrix(cr,&m);
      cw_surface_destroy (scrop);
      if(mdc)
      {
    	  cw_surface_destroy (s);
		  	SelectObject(mdc,hbmpold);
		  	DeleteDC(mdc);
      }
      if(op)cw_set_operator(cr,opOld);
		}else{
  	//_DrawBmpStub(memdc,pb);

	  	/*POINT srcsize; PntToPnt(srcsize,pb->SrcSize);
	  	POINT srcorg; PntToPnt(srcorg,pb->SrcOrigin);

	  	if((srcsize.x+srcorg.x)>dib->W)srcsize.x=dib->W-srcorg.x;
	  	if((srcsize.y+srcorg.y)>dib->H)srcsize.y=dib->H-srcorg.y;

	 		if (srcsize.y!=dib->H)	 srcorg.y=((dib->H-srcorg.y)-srcsize.y);

	  	StretchDIBits(memdc,origin.x,origin.y,dstsize.x,dstsize.y,
				  srcorg.x,srcorg.y,srcsize.x,srcsize.y,
				  dib->Bits,dib->Info,DIB_RGB_COLORS,rop);*/
	 	}
	}else{
  	ps->_DrawBmpStub(memdc,pb);
  }
};

void CairoRenderer::RenderDoubleBitmap(PDoubleBitmap_2d pb)
{
	PDoubleDib2d pd=(PDoubleDib2d)pb->dib->GetMainObject();
  if(pd)
  {
	 	pd->andbitmap->filename=pd->filename;
	 	RenderBitmap((PBitmap_2d)pb,pd->andbitmap,CAIRO_OPERATOR_DARKEN);
	 	RenderBitmap((PBitmap_2d)pb,pd,CAIRO_OPERATOR_LIGHTEN);
	 	pd->andbitmap->filename=NULL;
  }else{
		ps->_DrawBmpStub(memdc,pb);
  }
}


void CairoRenderer::RenderText2D(PText_2d pt)
{
	ps->SetTextMode(memdc);
	if(false || pt->GetAlpha()<1.0)
  {
    RECT r = pt->GetRect();
		HBITMAP tbm=CreateCompatibleBitmap(memdc,r.right-r.left,r.bottom-r.top);
		HDC tdc=CreateCompatibleDC(memdc);
		HGDIOBJ oldobj=SelectObject(tdc,tbm);
    cairo_surface_t* ts = cw_win32_surface_create(tdc);
    RECT rect;
    rect.left=-r.left-SpaceUpdRect.left;
    rect.right=rect.left+r.right-r.left;
    rect.top=-SpaceUpdRect.top;
    rect.bottom=rect.top+r.bottom-r.top;

 	  POINT2D org=ps->org,morg=ps->membitmaporg;
		ps->org.x=r.left-SpaceUpdRect.left;
		ps->org.y=r.top-SpaceUpdRect.top;
    //ps->membitmaporg.x=0;
    //ps->membitmaporg.y=0;

		ps->SetAnisotropic(tdc);
    ps->_DrawText(tdc,pt,SpaceUpdRect);

 	  ps->org=org;
    ps->membitmaporg=morg;

    cairo_matrix_t m;
    cw_get_matrix(cr,&m);

    cw_translate(cr,r.left,r.top);
    //cw_rotate(cr,pt->GetAngle());
    /*cw_scale(cr
	   	,(double)dstsize.x/(double)srcsize.x*(double)ps->scale_div.x/(double)ps->scale_mul.x
     	,(double)dstsize.y/(double)srcsize.y*(double)ps->scale_div.y/(double)ps->scale_mul.y);*/
    //cw_translate(cr,-0.5*(r.right-r.left),-0.5*(r.bottom-r.top));

    cairo_operator_t opOld = cw_get_operator(cr);
    //cw_set_operator(cr,CAIRO_OPERATOR_DARKEN);

    cw_set_source_surface(cr,ts,0,0);
    paint(pt->GetAlpha());
    cw_set_source_surface(cr,surface,0,0);

    cw_set_operator(cr,opOld);
    cw_set_matrix(cr,&m);
    cw_surface_destroy(ts);

		SelectObject(tdc,oldobj);	DeleteDC(tdc); ::DeleteObject(tbm);
  }
  else
  {
		ps->_DrawText(memdc,pt,SpaceUpdRect);
  }
  return;
/*
 	BOOL _print=(flags&PF_TOPRINTER);
 	BOOL makergn=TRUE;
 	if(_print)
  {
    POINT2D _s=ps->CalcTextSize(pt);
    if(_s.x<=pt->size.x+4 && _s.y<=pt->size.y+4)makergn=FALSE;
 	}
 	POINT2D _size;_size.x=0;_size.y=0;
 	if (pt->text->text->count)
  {
 		HRGN rgn=NULL;
 		HDC cdc=dc,mdc=0;
 		HBITMAP cbm,mbm,oldcolor,oldmono;
 		RECT r;
 		int needdraw=1,_H,_W,notrotate;

 		if (pt->angle!=0)
 		{
 			double f,f1;
 			f1=modf(pt->GetAngle()/(M_PI_2),&f);
 			f1=fabs(f1);
 			if (f1>0.5)f1=1-f1;
 			notrotate=f1<((1.0/900.0)*M_PI_2);
 		}
    else
    	notrotate=1;
 		if (notrotate)
  	{
    	SetRenderAnisotropic();
   		if(makergn)
      {
				r.left=pt->origin.x;
				r.top=pt->origin.y;
				r.right=r.left+pt->size.x;
				r.bottom=r.top+pt->size.y;
				ps->_LpToDp((LPPOINT) &r,2);
				RotateRect(r,pt->GetAngle());
				rgn=CreateRectRgn(r.left,r.top,r.right,r.bottom);
				OffsetRgn(rgn,-ps->membitmaporg.x,-ps->membitmaporg.y);
				SelectClipRgn(memdc,rgn);
   		}
  	}
    else
  	{
   		if(_print)	SetRenderAnisotropic();
  		if (!(pt->options&0x80) && !_print)
      {
				r.left=pt->origin.x;
				r.top=pt->origin.y;
				r.right=r.left+pt->size.x;
				r.bottom=r.top+pt->size.y;
				POINT tops [4];
				GetRotateRectPoints(r,pt->GetAngle(),tops);
				RotateRect(r,pt->GetAngle());
				r.left=max(r.left,SpaceUpdRect.left);
				r.right=min(r.right,SpaceUpdRect.right)+1;
				r.top=max(r.top,SpaceUpdRect.top);
				r.bottom=min(r.bottom,SpaceUpdRect.bottom)+1;
				rgn=CreatePolygonRgn(tops,4,ALTERNATE);
				needdraw=RectInRegion(rgn,&r);
				if (needdraw)
	 			{
					HRGN rgnDest,rgn2;
					rgnDest = CreateRectRgn(0, 0, 0, 0);
          rgn2 = CreateRectRgn(r.left,r.top,r.right,r.bottom);
					CombineRgn(rgnDest,rgn2,rgn, RGN_XOR);
					DeleteObject(rgn);DeleteObject(rgn2);
					rgn=rgnDest; // Это регион
					ps->_LpToDp((LPPOINT)&r,2);
					_H=r.bottom-r.top;
					_W=r.right-r.left;

					cbm=CreateCompatibleBitmap(memdc,_W,_H);
					mbm=CreateBitmap(_W,_H,1,1,NULL);

					cdc=CreateCompatibleDC(memdc);
					mdc=CreateCompatibleDC(memdc);

					oldcolor=(HBITMAP)SelectObject(cdc,cbm);
          oldmono=(HBITMAP)SelectObject(mdc,mbm);

					PatBlt(cdc,0,0,_W,_H,BLACKNESS);
          PatBlt(mdc,0,0,_W,_H,WHITENESS);
					SetTextColor(mdc,RGB(0,0,0));
					SetBkColor(mdc,RGB(0,0,0));
					POINT2D oldorg=ps->membitmaporg;
					ps->membitmaporg.x=r.left;ps->membitmaporg.y=r.top;
					SetRenderAnisotropic(cdc);
					SetRenderAnisotropic(mdc);
					ps->membitmaporg=oldorg;
	 			}
  		}
    }
		if (needdraw)
    {
			INT16 deltay=0,i;
			POINT2D origin=pt->origin;
			origin.x-=pt->Delta.x;origin.y-=pt->Delta.y;

			textItems=new TCollection(5,5);
			PLOGTextCollection text=pt->text->text;
			for (i=0;i<text->count;i++)
			{
      	PTextRecord ptr=(PTextRecord)text->At(i);
 				if (ptr->string->flags&1)
  			{
        	char * p=ptr->string->string;
					if (p)
					{
          	char * sbegin=p;
						{
	            while (*p!=0)
  	          {
    	        	if (*p=='\n')
								{
        	      	AddTextItem(memdc,ptr,sbegin,(p-sbegin));
                  sbegin=p;sbegin++;
          	      INT16 __dy=RenderTextItems(cdc,mdc,pt,origin,deltay,_size);
		 							deltay=__dy;
								}
	              p++;
		  				}
	  					AddTextItem(cdc,ptr,sbegin);
	  				}
					}
  			}
    	  else
	      {
  				AddTextItem(memdc,ptr,ptr->string->string);
				}
			}
			RenderTextItems(cdc,mdc,pt,origin,deltay,_size);
			delete textItems;
	 		if (notrotate||_print)
  	  {
 				if(makergn)SelectClipRgn(dc,0);
	 			ps->SetTextMode(memdc);
 				SetBkColor(dc,RGB(255,255,255));
 				SetTextColor(dc,0);
			}
  	  else
			{
				if(_print)ps->SetTextMode(memdc);
				if (!(pt->options&0x80) && !_print)
      	{
//*************************
	 				HGDIOBJ cbr=SelectObject(cdc,GetStockObject(BLACK_BRUSH));
 					HGDIOBJ mbr=SelectObject(mdc,GetStockObject(WHITE_BRUSH));
 					PaintRgn(cdc,rgn);
      	  PaintRgn(mdc,rgn);
 					SelectObject(cdc,cbr);SelectObject(mdc,mbr);
//*************************
					//POINT2D origin=pt->origin;_LpToDp(&origin,1);
 					SetMapMode(cdc,MM_TEXT);SetMapMode(mdc,MM_TEXT);

 					SetViewportOrgEx(cdc,0,0,NULL);
        	SetViewportOrgEx(mdc,0,0,NULL);
	 				SetWindowOrgEx(cdc,0,0,NULL);
  	      SetWindowOrgEx(mdc,0,0,NULL);
 					BitBlt(memdc,r.left,r.top,_W,_H,mdc,0,0,SRCAND);
 					BitBlt(memdc,r.left,r.top,_W,_H,cdc,0,0,SRCINVERT);
 					SelectObject(cdc,oldcolor);SelectObject(mdc,oldmono);
	 				DeleteDC(cdc); DeleteDC(mdc);
 					::DeleteObject(cbm);::DeleteObject(mbm);
 				}
			}
		}//needdraw
		if(rgn)	DeleteObject(rgn);
	}
	return true;//_size;*/
}

TPRNTEXT* CairoRenderer::AddTextItem(HDC dc,PTextRecord ptr,char * str,int len)
{
	TPRNTEXT* item=new TPRNTEXT();
  item->string=str;
  item->len=(len==-1)?lstrlen(str):len;
  if (item->len>0 && item->string[item->len-1]=='\r')item->len--;

  item->pt=ptr;
  HFONT _font=CreateFontIndirect(ptr->font->font);
  HGDIOBJ old=SelectObject(dc,_font);

  SIZE p;
  if (item->len)
  {
  	GetTextExtentPoint32(dc,item->string,item->len,&p);
  	item->size.x=p.cx;
  }else{
		GetTextExtentPoint32(dc,"A",1,&p);
		item->size.x=0;
  }
  item->size.y=p.cy;

  if(!ptr->font->tmValid)ptr->font->GetTM(dc);
  item->ascent=ptr->font->tmAscent;
  SelectObject(dc,old);
  ::DeleteObject(_font);

  textItems->Insert(item);
  return item;
}

INT16 CairoRenderer::RenderTextItems(HDC dc,HDC mdc,PText_2d pt,POINT2D origin,INT16 deltay,POINT2D & _s)
{
	if(textItems->count)
  {
  	INT16 i,maxy=0,sizex=0;
  	TPRNTEXT * item;
  	int ascent=0;
  	for(i=0;i<textItems->count;i++)
		{
    	item=(TPRNTEXT *)textItems->At(i);
	 		maxy=max(maxy,item->size.y);
      sizex+=item->size.x;
    	if(ascent<item->ascent)ascent=item->ascent;
	 	}
	 	deltay=deltay+maxy;
		origin.y+=deltay;
	 	_s.x=max(_s.x,sizex);
	 	_s.y=origin.y-pt->origin.y;
    origin.y+=ascent-maxy;
    RotatePoint(origin,pt->origin,pt->GetAngle());
#ifdef WIN32
	 	MoveToEx(dc,origin.x,origin.y,NULL);
#else
	 	MoveTo(dc,origin.x,origin.y);
#endif
	 	SetTextAlign(dc,TA_LEFT|TA_BASELINE|TA_UPDATECP);
		if (mdc){
#ifdef WIN32
	 		MoveToEx(mdc,origin.x,origin.y,NULL);
#else
	 		MoveTo(mdc,origin.x,origin.y);
#endif
	 		SetTextAlign(mdc,TA_LEFT|TA_BASELINE|TA_UPDATECP);
		}
  	for(i=0;i<textItems->count;i++)
    {
	 		item =(TPRNTEXT *) textItems->At(i);
	 		if ((item->len)&&
// Проверка по выходу за границу по X.
		  (origin.x+item->size.x > pt->origin.x)&&
		  (origin.x < pt->origin.x+pt->size.x)
		  ){
	 			HGDIOBJ _font=0;//(HGDIOBJ)ps->_CreateFont(item->pt->font,pt->angle);
	 			HGDIOBJ old=SelectObject(dc,_font);
	 			HGDIOBJ oldmfont;
	 			int bkmode=((item->pt->ltBgColor & 0x01000000l)!=0l)?0:OPAQUE;
	 			int oldbk=0;
	 			if(bkmode)
        {
        	SetBkColor(dc,ps->GetGDIColor(item->pt->ltBgColor));
     			oldbk=SetBkMode(dc,OPAQUE);
	  			if(mdc)SetBkMode(mdc,OPAQUE);
	 			}else{
        	oldbk=SetBkMode(dc,TRANSPARENT);
	  			if(mdc)SetBkMode(mdc,TRANSPARENT);
	  		}

	 			SetTextColor(dc,ps->GetGDIColor(item->pt->ltFgColor));
	 			TextOut(dc,0,0,item->string,item->len);
	 			if(mdc)
        {
	 				oldmfont=SelectObject(mdc,_font);
	 				TextOut(mdc,0,0,item->string,item->len);
	 				SelectObject(mdc,oldmfont);
	 			}
	 			origin.x+=item->size.x;
	 			SelectObject(dc,old);
//	 DeleteObject(_font);
	 			SetBkMode(dc,oldbk);
	 		}
	 		delete item;
	 	}
		textItems->DeleteAll();
  }
	return deltay;
 };


bool CairoRenderer::SetRenderAnisotropic(HDC hdc)
{
/*	HDC h=hdc?hdc:memdc;
  SetMapMode(h,MM_ANISOTROPIC);

	SetWindowExtEx(h,ps->scale_div.x,ps->scale_div.x,NULL);
  SetViewportExtEx(h,ps->scale_mul.x,ps->scale_mul.y,NULL);
  SetViewportOrgEx(h,-(ps->membitmaporg.x+ps->org.x),-(ps->membitmaporg.y+ps->org.y),NULL);
  SetWindowOrgEx(h,0,0,NULL);*/
}

bool CairoRenderer::ResetRenderAnisotropic(HDC hdc)
{
	HDC h=hdc?hdc:memdc;
	SetWindowExtEx(h,1,1,NULL);
  SetViewportExtEx(h,1,1,NULL);
  SetViewportOrgEx(h,0,0,NULL);
  SetWindowOrgEx(h,0,0,NULL);
}


void CairoRenderer::SetPen(PPen2d pp,double alpha)
{
	if(pp->rop2==R2_MASKPEN)alpha*=0.5;

	SetColor(pp->color,alpha);
	cw_set_line_width (cr, max(pp->width,1.0));
  cw_set_line_join(cr,CAIRO_LINE_JOIN_ROUND);
  cw_set_line_cap(cr,CAIRO_LINE_CAP_ROUND);
  double* dashes;
  int num_dashes=0;
  switch(pp->style)
  {
  	case PS_SOLID:
    case PS_INSIDEFRAME:
      num_dashes=2;
    	dashes=new double[num_dashes];
      dashes[0]=10;
      dashes[1]=0;
    break;
  	case PS_DASH:
      num_dashes=2;
    	dashes=new double[num_dashes];
      dashes[0]=20;
      dashes[1]=5;
    break;
  	case PS_DOT:
      num_dashes=2;
    	dashes=new double[num_dashes];
      dashes[0]=5;
      dashes[1]=5;
    break;
  	case PS_DASHDOT:
      num_dashes=4;
    	dashes=new double[num_dashes];
      dashes[0]=20;
      dashes[1]=5;
      dashes[2]=5;
      dashes[3]=5;
    break;
  	case PS_DASHDOTDOT:
      num_dashes=6;
    	dashes=new double[num_dashes];
      dashes[0]=20;
      dashes[1]=5;
      dashes[2]=5;
      dashes[3]=5;
      dashes[4]=5;
      dashes[5]=5;
    break;
  }
  cw_set_dash(cr,dashes,num_dashes,0);
}

void CairoRenderer::SetBrush(PBrush2d pb,RECT rect,double alpha)
{
	if(pb->rop2==R2_MASKPEN)alpha*=0.5;

	cw_set_source_surface(cr, surface,0,0);

	if(pb->style==BS_NULL || pb->color&0x01000000L)
	{	SetColor(pb->color,0); return;}

  if(pb->style==BS_SOLID)
	{	SetColor(pb->color,alpha); return;}

  if(pb->style==BS_HATCHED)//TODO
  {
    /*int hatch_size = 7;
    int line_width = 1;
    cairo_surface_t *hatch = cw_surface_create_similar(
    	surface,CAIRO_CONTENT_COLOR_ALPHA, hatch_size, hatch_size);

    cw_surface_set_repeat(hatch, 1);

		cw_save(cr);
		cw_set_target_surface (cr, hatch);

		// draw background
		SetColor(pb->backcolor,alpha);
		cairo_rectangle (cr, 0, 0, hatch_size, hatch_size);
		fill;

		// draw vertical line in the foreground
		SetColor(pb->color,alpha);
		cw_set_line_width (cr, line_width);
		cw_move_to(cr, hatch_size / 2.0, 0);
		cw_line_to(cr, hatch_size / 2.0, hatch_size);
		stroke

		cairo_restore (cr);*/

   	switch(pb->hatch)
    {}
    SetColor(pb->color,alpha*0.5);
  	return;
  }

  if(pb->style==BS_GRADIENT_LINEAR)
  {
  	double x=rect.left;
    double y=rect.top;
  	double w=rect.right-x;
    double h=rect.bottom-y;

		cairo_pattern_t *pat = cw_pattern_create_linear(
    	x+w*pb->gradient_x0,y+h*pb->gradient_y0,
      x+w*pb->gradient_x1,y+h*pb->gradient_y1);
  	double offset=0;
    for(int i=0;i<pb->gradient_colors_count;i++)
    {
			COLORREF c = ps->GetGDIColor(pb->gradient_colors[i]);
			cw_pattern_add_color_stop_rgba(pat,(double)i/(double)(pb->gradient_colors_count-1),
      	GetRValue(c)/255.0, GetGValue(c)/255.0, GetBValue(c)/255.0,alpha);
    }
		//cw_pattern_add_color_stop_rgba(pat,0, 0, 1, 0,alpha);
		//cw_rectangle (cr, 0, 0, 256, 256);
		cw_set_source (cr, pat);
		//cw_fill (cr);
    return;
  }

  if(pb->style==BS_PATTERN && pb->dib) //TODO
  {
    SetColor(pb->color,alpha*0.5);
  	return;
  }
  SetColor(pb->color,alpha);

/*  cairo_pattern_t* pattern = cw_pattern_create_rgba(
  	GetRValue(pb->color)/255.0, GetGValue(pb->color)/255.0, GetBValue(pb->color)/255.0,1.0);*/
/*
	if (pb->dib)
	 {PDib2d dib=(PDib2d)(pb->dib->GetMainObject());
     if(dib){
	   HBRUSH h=CreateDIBPatternBrush(dib->dib,DIB_RGB_COLORS);
      if(h)UnrealizeObject(h);
      return h;
     }
	 }
   LOGBRUSH lb;

   if(pb->color&0x01000000L){
    lb.lbHatch = 0;
  	 lb.lbStyle = BS_NULL;
    lb.lbColor = 0;
   }else{
	if ((pb->style==BS_SOLID)&&(palette))
	 {
	  COLORREF cl=GetGDIColor(pb->color);
	  int nColor = GetNearestPaletteIndex(palette,cl);
	  return  CreateSolidBrush(PALETTEINDEX(nColor));
	 }
   lb.lbHatch = pb->hatch;
	lb.lbStyle = pb->style;
	lb.lbColor = GetGDIColor(pb->color);
   }

	return CreateBrushIndirect(&lb);*/
}

void CairoRenderer::SetColor(COLORREF c,double alpha)
{
	c = ps->GetGDIColor(c);//на случай, если установлен системный цвет
	cw_set_source_rgba (cr, GetRValue(c)/255.0, GetGValue(c)/255.0, GetBValue(c)/255.0,alpha);
}

int CairoRenderer::SetMixMode(int mode)
{
	//TODO: http://cairographics.org/manual/cairo-cairo-t.html#cairo-operator-t
	//http://msdn.microsoft.com/en-us/library/dd145088(v=vs.85).aspx

/*switch(mode){
 case R2_BLACK       :lstrcat(s,"BLACK ");break;
case R2_NOTMERGEPEN :lstrcat(s,"NOTMERGEPEN ");break;
case R2_MASKNOTPEN  :lstrcat(s,"MASKNOTPEN ");break;
case R2_NOTCOPYPEN  :lstrcat(s,"NOTCOPYPEN ");break;
case R2_MASKPENNOT  :lstrcat(s,"MASKPENNOT ");break;
case R2_NOT         :lstrcat(s,"NOT ");break;
case R2_XORPEN      :lstrcat(s,"XORPEN ");break;
case R2_NOTMASKPEN  :lstrcat(s,"NOTMASKPEN ");break;
case R2_MASKPEN     :lstrcat(s,"MASKPEN ");break;
case R2_NOTXORPEN   :lstrcat(s,"NOTXORPEN ");break;
case R2_NOP         :lstrcat(s,"NOP ");break;
case R2_MERGENOTPEN :lstrcat(s,"MERGENOTPEN ");break;
case R2_MERGEPENNOT :lstrcat(s,"MERGEPENNOT ");break;
case R2_MERGEPEN    :lstrcat(s,"MERGEPEN ");break;
case R2_WHITE       :lstrcat(s,"WHITE ");break;*/
	return SetROP2(memdc,mode);
}

void CairoRenderer::FinishRender()
{
 	if (isRenderPart)
  {
  	flush
  	cw_surface_destroy(surface);
  	cw_destroy(cr);
		SetViewportOrgEx(memdc,0,0,NULL);
		BitBlt(dc,wX,wY,wW,wH,memdc,0,0,SRCCOPY);

 		SelectObject(memdc,hbmpold);
	  DeleteDC(memdc);
  	DeleteObject(hbmp);
	}
	cw_surface_destroy(wsurface);
	cw_destroy(wcr);
}


//*-----------------------------------------------------*

TPNGDib2d::TPNGDib2d():TDib2d()
{
};

TPNGDib2d::TPNGDib2d(PStream st,LONG pos):TDib2d(st,pos)
{
};

TPNGDib2d::TPNGDib2d(char* filename):TDib2d()
{
	if(!InitCairo())return;

	png = cw_image_surface_create_from_png(filename);
  if(cw_surface_status(png)!=CAIRO_STATUS_SUCCESS){png==0;return;}

  imagesize.x=W=cw_image_surface_get_width(png);
  imagesize.y=H=cw_image_surface_get_height(png);
  //Bits=(void*)cw_image_surface_get_data(png);
	SetFreeDib(TRUE);
 	Bits=0;Info=0;IsCore=0;Mode=0;bitmap=0;perline=0;
  this->filename=NewStr(filename);
  dib=this;

/*  unsigned char * d =cw_image_surface_get_data(png);;
  int stride = cw_image_surface_get_stride(png);
  cairo_format_t format = cw_image_surface_get_format(png);
  cw_surface_destroy(png);
  png=cw_image_surface_create_for_data(d,format,W,H,stride); //не пашет  */
};

TPNGDib2d::~TPNGDib2d()
{
 	Bits=0;Info=0;IsCore=0;Mode=0;bitmap=0;perline=0;
	cw_surface_destroy(png);
  png=0;dib=0;
	if(filename) delete filename;
};

typedef struct
{
	unsigned char* pos;
	unsigned char* end;
} png_closure;

static cairo_status_t WritePNG(char *closure, unsigned char *data,unsigned int length)
{
  //MessageBox(0,"WritePNG","",0);
	MessageBox(0,closure,"",0);
  return CAIRO_STATUS_SUCCESS;

	/*png_closure *cl = new png_closure();
  cl->pos=new char[100];
  cl->end=cl->pos+100;
  strcpy(cl->pos,"awefwaef");
  closure=cl;
  MessageBox(0,"WritePNGq1111","",0);
  return CAIRO_STATUS_SUCCESS;
  png_closure *cl= (png_closure*)(closure);
  //char s[100];wsprintf(s,"%d",closure); MessageBox(0,s,"",0);
  cl->pos=new char[length];
  cl->end=cl->pos+length;
  memcpy(cl->pos,data,length);
  MessageBox(0,data,"",0) ;
  //MessageBox(0,(char*)closure,"",0);
  if(sizeof(closure)==sizeof(png_closure))MessageBox(0,"write","",0);
  else MessageBox(0,"false","",0);
	return CAIRO_STATUS_SUCCESS;*/
}

static cairo_status_t ReadPNG(void *closure,unsigned char *data,unsigned int length)
{
	return CAIRO_STATUS_SUCCESS;
}

void TPNGDib2d::Store(PStream st)//TODO: сохранение в файл класса
{
	return;
	if(!InitCairo())return;

	//char* s=new char[100]; strcpy(s,"awfwaef");
  char* s="fweffewfwefwe";
	png_closure cl;
  cl.pos=new char[100];
  cl.end=cl.pos+100;
  strcpy(cl.pos,"awefwaef");
  MessageBox(0,"before","",0);
  cairo_status_t status = cw_surface_write_to_png_stream(png,(cairo_write_func_t)WritePNG,s);
  MessageBox(0,"after","",0);
  //MessageBox(0,cl->pos,"",0);
}
