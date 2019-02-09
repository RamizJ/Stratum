// TEXTURE_256   для 8 битного цвета
// TEXTURE_MODE  - c умножением на коэффициент плоскости но без учета цветов точек
// без  TEXTURE_MODE  (выигрыш за счет убирания умножения на каждый пиксел)
// шаблон для PutPixel

#ifdef TEXTURED_POLYGON
// tPutPixelNoShade
{
 long _zp=*zptr;
 if (z>(_zp)){
  long _x1=Q>>16;
  long _y1=S>>16;
  while(_x1>ps.txtWidth_1){
   _x1-=ps.txtWidth;
  }
  while(_y1>ps.txtHeight_1){
   _y1-=ps.txtHeight;
  }
 _y1=(ps.txtHeight_1-_y1);
 *zptr=z;

#ifdef TEXTURE_256
  BYTE  _bmp=*((BYTE _HUGE*)(ps.txtBits+(_y1)*ps.txtperline+_x1));
  LONG * bmp=(LONG *)(((BITMAPINFO*)(ps.txtInfo))->bmiColors+_bmp);
#else
  long _HUGE * bmp=(long _HUGE*)(ps.txtBits+(_y1)*ps.txtperline+(_x1)*3);
#endif

#ifdef TEXTURE_MODE
COLORREF clr=(*bmp);
  *curptr=(GetRValue(clr)*_cc)>>8;curptr++;
  *curptr=(GetGValue(clr)*_cc)>>8;curptr++;
  *curptr=(GetBValue(clr)*_cc)>>8;curptr++;
 }else curptr+=3;
#else
#ifdef SHADED_POLYGON
COLORREF clr=(*bmp);
  *curptr=(GetRValue(clr)*(int)HIWORD(b))>>8;curptr++;
  *curptr=(GetGValue(clr)*(int)HIWORD(g))>>8;curptr++;
  *curptr=(GetBValue(clr)*(int)HIWORD(r))>>8;curptr++;
 }else curptr+=3;
#else
  long _HUGE *_curptr=(long _HUGE *)curptr;
  *_curptr=((*_curptr)&(0xff000000ul))| ((*bmp)&0xfffffful);
  }curptr+=3;
#endif

#endif
}
#else
#ifdef SHADED_POLYGON
// PutPixel
{
 long _zp=*zptr;
 long t=z-_zp;if(t<0)t=-t;
 if (!(t&0xffffff00ul)){
  *curptr=(BYTE)(((int)HIWORD(b)+(int)(*curptr))/2);curptr++;
  *curptr=(BYTE)(((int)HIWORD(g)+(int)(*curptr))/2);curptr++;
  *curptr=(BYTE)(((int)HIWORD(r)+(int)(*curptr))/2);curptr++;
 }else{
 if (z>(_zp)){
  *curptr=_cb.b;curptr++;
  *curptr=_cg.b;curptr++;
  *curptr=_cr.b;curptr++;
  *zptr=z;
  }else{
   curptr+=3;
  };}
};
#else
// PutPixelNoShade
{
 long _zp=*zptr;
 long t=z-_zp;if(t<0)t=-t;
 if (!(t&0xffffff00ul)){
  *curptr=(BYTE)(((int)b+(int)(*curptr))/2);curptr++;
  *curptr=(BYTE)(((int)g+(int)(*curptr))/2);curptr++;
  *curptr=(BYTE)(((int)r+(int)(*curptr))/2);curptr++;
 }else{
 if (z>(_zp)){
  *curptr=b;curptr++;
  *curptr=g;curptr++;
  *curptr=r;curptr++;
  *zptr=z;
  }else{
   curptr+=3;
  };}
};

#endif
#endif