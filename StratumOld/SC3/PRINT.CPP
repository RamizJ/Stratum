#include <windows.h>
#include <commdlg.h>
#include <gwin2d3d\gwinfunc.h>
#include "collect.h"
#include "print.h"
#include "string.h"
#include "grtools.h"
#include <stdio.h>
#include "records.h"
#define PPR_SIGNATURE "SCPRINTPAGE"
#include "strings.rh"
#include <math.h>

#ifdef PROTECT
#include "password.h"
#endif

extern HINSTANCE HResource;
int StrToDouble(char*text,double&value);

 BOOL TPrintSpace::Store(TStream&st){
  st.WriteStr(PPR_SIGNATURE);

  st.WriteWord(PPR_SCALEMODE);
  st.WriteWord(scalemode);
  if(scalemode==3){
   st.WriteWord(PPR_SCALECUSTOM);
   st.Write(&scale,sizeof(scale));
  }
  if(addborders){
   st.WriteWord(PPR_USEADDF);
  };
  if(!allextent){
   st.WriteWord(PPR_USESRC);
  }
  if(copies>1){
   st.WriteWord(PPR_PAGECOUNT);
   st.WriteWord(copies);
  }

  if((fileds.left!=0)||(fileds.right!=0)||(fileds.top!=0)||(fileds.bottom!=0)){
   st.WriteWord(PPR_FIELDS);
   st.Write(&fileds,sizeof(fileds));
  };
  if(addf.x!=0 || addf.y!=0){
   st.WriteWord(PPR_ADDFIELDS);
   st.Write(&addf,sizeof(addf));
  };

   st.WriteWord(0);//end of love
 };
 BOOL TPrintSpace::Restore(TStream&st){
   char s[64];
   st.ReadStr(s,sizeof(s));
   if(lstrcmp(s,PPR_SIGNATURE))return FALSE;

   fileds.left=fileds.right=fileds.top=fileds.bottom=0;
   scale=100;
   addf.x=addf.y=0;
   allextent=1;
   copies=1;
   addborders=0;
   scalemode=0;

   INT16 code;
   while(code=st.ReadWord()){
	switch(code){
	 case PPR_SCALEMODE  :scalemode=st.ReadWord();break;
	 case PPR_SCALECUSTOM:st.Read(&scale,sizeof(scale));break;
	 case PPR_USEADDF:addborders=1;break;
	 case PPR_USESRC:allextent=0;break;
	 case PPR_PAGECOUNT:copies=st.ReadWord();break;
	 case PPR_FIELDS:st.Read(&fileds,sizeof(fileds));break;
	 case PPR_ADDFIELDS:st.Read(&addf,sizeof(addf));break;
	}
   }
  UpdateAllDialogs();
  return TRUE;
 };
 void TPrintSpace::UpdatePrinter(char*p){
  if(p==NULL){
   GetProfileString("windows", "device", ",,,",szDevString, 120);
  }else{
   GetProfileString("devices",p, ",,,",szDevString, 120);
  }
  if(!lstrcmp(szDevString,",,,")){ errorcode=1; return; }
  if (p){
   char s[256];
   wsprintf(s,"%s, %s",p,szDevString);
   lstrcpy(szDevString,s);
  }

	if ((szPrinter = strtok(szDevString,
			   (const char *) ","))
			&& (szDriver = strtok ((char *) NULL,
			   (const char *) ", "))
			&& (szPort = strtok ((char *) NULL,
			   (const char *) ", ")))
      if (hDC) DeleteDC(hDC);
		hDC = CreateDC(szDriver, szPrinter,
			szPort, NULL);
      Init();

 };
 TPrintSpace::TPrintSpace(HWND _hwnd,HSP2D _hsp){
  memset(this,0,sizeof(TPrintSpace));
  xitems=NULL;
  yitems=NULL;
  mpagemode=0;
  copies=1;
  hwnd=_hwnd;
  hsp=_hsp;
  scale=1.0;
  allextent=1;
  UpdatePrinter(NULL);
   if(!GetSpaceExtent2d(hsp,rect2d)){errorcode=3;return; }
   else{
   rect2dcustom=rect2d;
	Init();
   if(width==0 || _HORZRES==0 ||
	  height==0 || _VERTRES==0){
	   errorcode=2;
	  }
   }
 };

 TPrintSpace::~TPrintSpace(){
  if(hDC) DeleteDC(hDC);
  DoneItems();
 };
void TPrintSpace::DoneItems(){
 if(xitems){
  for(C_TYPE i=0;i<xitems->count;i++){
	delete xitems->At(i);
  }
   delete xitems;xitems=NULL;
  }
 if(yitems){
  for(C_TYPE i=0;i<yitems->count;i++){
	delete yitems->At(i);
  }
   delete yitems;yitems=NULL;
  }
 }
 TCollection *xitems;
 TCollection *yitems;

 TPrintSpace::Init(){
   width=GetDeviceCaps(hDC,HORZSIZE);
   height=GetDeviceCaps(hDC,VERTSIZE);
   _HORZRES   =GetDeviceCaps(hDC,HORZRES);
   _VERTRES   =GetDeviceCaps(hDC,VERTRES);
   _LOGPIXELSX=GetDeviceCaps(hDC,LOGPIXELSX);
   _LOGPIXELSY=GetDeviceCaps(hDC,LOGPIXELSY);

   _scale.x=_LOGPIXELSX/96.0;
   _scale.y=_LOGPIXELSY/96.0;

 };
 double TPrintSpace::ConvertX(double a,CONVERT_MODE mode){
  switch(mode){
   case SPACE_to_PRINTER:return a*_scale.x*scale;
   case PRINTER_to_SPACE:return a/(_scale.x*scale);
   case MM_to_PRINTER   :return a*((double)_HORZRES)/((double)width);
   case PRINTER_to_MM   :return a*((double)width)/((double)_HORZRES);
   case MM_to_SPACE     :return a*((double)_HORZRES)/((double)width)/(_scale.x*scale);
   case SPACE_to_MM     :return a*(_scale.x*scale)*((double)width)/((double)_HORZRES);
  }
  return 0;
 };
 double TPrintSpace::ConvertY(double a,CONVERT_MODE mode){
  switch(mode){
   case SPACE_to_PRINTER:return a*_scale.y*scale;
   case PRINTER_to_SPACE:return a/(_scale.y*scale);
   case MM_to_PRINTER   :return a*((double)_VERTRES)/((double)height);
   case PRINTER_to_MM   :return a*((double)height)/((double)_VERTRES);
   case MM_to_SPACE     :return a*((double)_VERTRES)/((double)height)/(_scale.y*scale);
   case SPACE_to_MM     :return a*(_scale.y*scale)*((double)height)/((double)_VERTRES);
  }
  return 0;
 };
 BOOL TPrintSpace::Setup(HWND _hwnd){
 if(PrinterSetupDialog(_hwnd)){
   Init();
  UpdateAllDialogs();
  return TRUE;
 }
/*
 PRINTDLG pd;
// Set all structure members to zero.

memset(&pd, 0, sizeof(PRINTDLG));

// Initialize the necessary PRINTDLG structure members.

pd.lStructSize = sizeof(PRINTDLG);
pd.hwndOwner = hwnd;
pd.hDC=hDC;
pd.Flags = PD_RETURNDC|PD_NOPAGENUMS |PD_PRINTSETUP	;
if(PrintDlg(&pd) != 0){
  if(hDC) DeleteDC(hDC);
  hDC=pd.hDC;
  Init();
	 if (pd.hDevMode != NULL)
		 GlobalFree(pd.hDevMode);
	 if (pd.hDevNames != NULL)
		 GlobalFree(pd.hDevNames);
UpdateAllDialogs();
return TRUE;
}
*/
return FALSE;
};
#ifdef WIN32
#define WIN32_P
#endif

BOOL TPrintSpace::BeginPrint(){
#ifdef WIN32_P
DOCINFO di;
memset(&di,0,sizeof(di));
	   di.cbSize=sizeof(di);
	   di.lpszDocName="Space2d-VDR(Print)";
	   di.lpszDatatype="Space2d-VDR";
 if((StartDoc( hDC,&di))<0)return FALSE;
#else
 Escape(hDC, STARTDOC, 8, "Space2d-VDR", NULL);
#endif
hCsave=SetCursor(LoadCursor(0,IDC_WAIT));
return TRUE;
};
BOOL TPrintSpace::EndPrint(){
#ifdef WIN32_P
  EndDoc( hDC );
#else
  Escape(hDC, ENDDOC, 0, NULL, NULL);
#endif
SetCursor(hCsave);
return TRUE;
};

BOOL TPrintSpace::Print(int page){
 POINT2D _MUL,_DIV,_ORG;
 POINT2D _mul,_div;
 POINT2D orgspace;
#ifdef WIN32_P
 StartPage(hDC);
#endif
 GetScaleSpace2d(hsp,&_MUL,&_DIV );
 GetOrgSpace2d(hsp,&_ORG);
 _mul.x=100*_scale.x*scale;_mul.y=100*_scale.x*scale;
 _div.x=_div.y=100.0;
 SetScaleSpace2d(hsp,&_mul,&_div);
 RECT r;

  if(pagecount<2){

	RECT2D r2=rect2dcustom;
	r2.left*=scale*_scale.x;
	r2.top*=scale*_scale.y;
	r2.right*=scale*_scale.x;
	r2.bottom*=scale*_scale.y;
	orgspace.x=r2.left - ConvertX(fileds.left,MM_to_PRINTER);
	orgspace.y=r2.top  - ConvertY(fileds.top,MM_to_PRINTER);
	SetOrgSpace2d(hsp,&orgspace);

	r.left=-32000; r.top    = -32000;
	r.right=32000; r.bottom = 32000;
  PaintSpace2d(hsp,hDC,&r,PF_DIRECT|PF_NOCACHEDIB|PF_TOPRINTER);
  }else{
   int y=page/xitems->count;
   int x=page-y*xitems->count;
   PrintItem*_y=(PrintItem*)yitems->At(y);
   PrintItem*_x=(PrintItem*)xitems->At(x);

   r.left=0;
   r.top=0;
   r.right=ConvertX(_x->spacecx,SPACE_to_PRINTER);
   r.bottom=ConvertY(_y->spacecx,SPACE_to_PRINTER);
 //  RECT2D r2=r;

   orgspace.x=ConvertX(_x->spacex,SPACE_to_PRINTER)- ConvertX(fileds.left,MM_to_PRINTER);
   orgspace.y=ConvertY(_y->spacex,SPACE_to_PRINTER)- ConvertY(fileds.top,MM_to_PRINTER);

   SetOrgSpace2d(hsp,&orgspace);
   SaveDC(hDC);
   PaintSpace2d(hsp,hDC,&r,PF_DIRECT|PF_NOCACHEDIB);
   RestoreDC(hDC,-1);
   if(1){
   POINT p[2];
   HPEN pen=CreatePen(PS_DOT,0,0);
   HGDIOBJ old=SelectObject(hDC,pen);
   if(x<xitems->count-1){
	 p[0].y=0;
	 p[1].y=_VERTRES;
	 p[1].x=p[0].x=ConvertX(_x->spacecxR,SPACE_to_PRINTER);
	 Polyline(hDC,p,2);
	}
   if(y<yitems->count-1){

	 p[0].x=0;
	 p[1].x=_HORZRES;
	 p[1].y=p[0].y=ConvertY(_x->spacecxR,SPACE_to_PRINTER);
	 Polyline(hDC,p,2);
	}
	 SelectObject(hDC,old);
	 DeleteObject(old);
   }
  }

  SetScaleSpace2d(hsp,&_MUL,&_DIV );
  SetOrgSpace2d(hsp,&_ORG);

#ifdef DEMO
  SelectObject(hDC,GetStockObject(ANSI_VAR_FONT));
  char * __ps="DEMO version of Stratum" ;TextOut(hDC,0,0,__ps,lstrlen(__ps));
#endif
#ifdef PROTECT
if(!IsRegistered5()){
char str[]={88,90,89,73,93,95,88,44,63,62,63,64,49,109,51,105,131,136,124,127,130,141,143,129,143,131,131,64,151,135,149,151,142,149,149,72,134};
  //char *__ps="STRATUM 2000 [ Unregistered version ]";
  SelectObject(hDC,GetStockObject(ANSI_VAR_FONT));
  SetBkMode(hDC,TRANSPARENT);
  char s[65];
  for(int i=0;i<sizeof(str);i++)s[i]=str[i]-(i+5);
  s[sizeof(str)]=0;
  char *__ps=s;
  BYTE l=(BYTE)lstrlen(__ps);
  UINT prev=SetTextAlign(hDC,TA_TOP |TA_RIGHT);
  TextOut(hDC,_HORZRES-_scale.x*1.0,1*_scale.y,__ps,l);
  SetTextAlign(hDC,TA_BOTTOM |TA_RIGHT);
  TextOut(hDC,_HORZRES-_scale.x*1.0,_VERTRES-1*_scale.y,__ps,l);
  SetTextAlign(hDC,prev);
  if(l==0){__ps=0;*__ps=0;}
 }
#endif

#ifdef WIN32_P
  EndPage(hDC);
#else
  Escape(hDC, NEWFRAME, 0, NULL, NULL);
#endif
};


void TPrintSpace::SetScaleMode(int s){
 scalemode=s;
 UpdateAllDialogs(1);
};
void TPrintSpace::_SetScale(double d){
 scale=d/100.0;
 UpdateAllDialogs(0x100|1);
};
void TPrintSpace::SetExtent(BOOL b){
 allextent=b;
 UpdateAllDialogs();
}
double TPrintSpace::GetWorkWidth(){
return _HORZRES-ConvertX((fileds.right+fileds.left),MM_to_PRINTER);
};
double TPrintSpace::GetWorkHeight(){
return _VERTRES-ConvertY((fileds.bottom+fileds.top),MM_to_PRINTER);
};
double MyRound(double a,int digit){
  double r;
  if (digit>14)r=a;else{
  int i;
  for(i=0;i<digit;i++) a*=10;
  r=floor(a);
//  if ((a-r)>0.5)r+=1.0;
  for(i=0;i<digit;i++) r/=10;
  }
  return r;
 }
void TPrintSpace::UpdateAllDialogs(UINT32 flags){
if(flags&1){
 SendMessage(Rscale1,BM_SETCHECK,(scalemode==0),0);
 SendMessage(Rscale2,BM_SETCHECK,(scalemode==1),0);
 SendMessage(Rscale3,BM_SETCHECK,(scalemode==2),0);
 SendMessage(Rscale4,BM_SETCHECK,(scalemode==3),0);

// EnableWindow(Tscale,scalemode==3);
 BOOL di=FALSE;
 if((scalemode==1)||(scalemode==2))di=TRUE;
 switch(scalemode){
  case 0:scale=1;break;
  case 1:{scale=1;
		 double sx=rect2dcustom.right-rect2dcustom.left;
		 double sy=rect2dcustom.bottom-rect2dcustom.top;
		 double _sx=sx*_scale.x;
		 double _sy=sy*_scale.y;
		   if ((GetWorkWidth()>_sx)&&(GetWorkHeight() >_sy)){
			break;
		  }}
  case 2:{
		 double sx=rect2dcustom.right-rect2dcustom.left;
		 double sy=rect2dcustom.bottom-rect2dcustom.top;
		 double _sx=sx*_scale.x;
		 double _sy=sy*_scale.y;
		 double ww=GetWorkWidth();
		 double wh=GetWorkHeight();
BOOL     p1=FALSE;
m1:
		  double _x= ww  / _sx;
		  double _y= wh / _sy;
		  double _scale=0;
		  if(p1){
			if(_x<_y)_scale=_x;else _scale=_y;
		  }else{
			if(_x<_y)scale=_x;else scale=_y;
		  }
	char text[20];
	sprintf(text,"%g",p1?_scale:scale);
	double _d;
	sscanf(text,"%g",&_d);
	if(StrToDouble(text,_d)){

	  if(_d>scale){
		 ww-=1.0;wh-=1.0;
		 p1=TRUE;
		 goto m1;
	   }}
	  if(p1)scale=_scale;
	  }break;
 }

 if(!(flags&0x100)){
//  char s[64];
//  sprintf(s,"%g",scale*100);
//  SetWindowText(Tscale,s);
	SetDouble(Tscale,scale*100);
 }
 DoneItems();

 if(di){
  pagecount=1;
  xpage = 1;
  ypage = 1;

 }else
 {

  xpage = 1;
  ypage = 1;
// Y
{
  double hh  = GetWorkHeight();
  double wws = ConvertY(hh,PRINTER_to_SPACE);
  double src = (rect2dcustom.bottom-rect2dcustom.top);
  double src1 = src;
  yitems=new TCollection(4,4);

  while(src>0){
   PrintItem *pi=new PrintItem;
	yitems->Insert(pi);
	double _height1   // ширина с полями
		  ,_height2;  // ширина без полей

	pi->spacex=rect2dcustom.top+(src1-src);

	if(yitems->count && onlyexternal){
	  pi->pstart=0;
	 _height1=_VERTRES-ConvertX(fileds.bottom,MM_to_PRINTER);
	 _height2=_VERTRES;
	}else{
	 pi->pstart=fileds.top;
	 _height1=hh;
	 _height2=_VERTRES-ConvertY(fileds.top,MM_to_PRINTER);
	}

   if(src<=ConvertY(_height1,PRINTER_to_SPACE)){
	pi->spacecxR=pi->spacecx=src;
	pi->plength=ConvertY(src,SPACE_to_PRINTER);
	src=0;
   }else{
	 double h=ConvertY(_height2,PRINTER_to_SPACE);
	 pi->plength=_height2;
	 pi->spacecx=h;
	 if(addborders){
	  pi->spacecxR=pi->spacecx-ConvertY(addf.y,MM_to_SPACE);
	 }else pi->spacecxR=pi->spacecx;
	 src-=pi->spacecxR;
   }

  }
}

// X
  double ww  = GetWorkWidth();
  double wws = ConvertX(ww,PRINTER_to_SPACE);
  double src = (rect2dcustom.right-rect2dcustom.left);
  double src1 = src;
  xitems=new TCollection(4,4);

  while(src>0){
   PrintItem *pi=new PrintItem;
	xitems->Insert(pi);
	double _width1   // ширина с полями
		  ,_width2;  // ширина без полей

	pi->spacex=rect2dcustom.left+(src1-src);

	if(xitems->count && onlyexternal){
	 pi->pstart=0;
	 _width1=_HORZRES-ConvertX(fileds.right,MM_to_PRINTER);
	 _width2=_HORZRES;
	}else{
	 pi->pstart=fileds.left;
	 _width1=ww;
	 _width2=_HORZRES-ConvertX(fileds.left,MM_to_PRINTER);
	}

   if(src<=ConvertX(_width1,PRINTER_to_SPACE)){
	pi->spacecxR=pi->spacecx=src;
	pi->plength=ConvertX(src,SPACE_to_PRINTER);
	src=0;
   }else{
	 double w=ConvertX(_width2,PRINTER_to_SPACE);
	 pi->plength=_width2;
	 pi->spacecx=w;
	 if(addborders){
	  pi->spacecxR=pi->spacecx-ConvertX(addf.x,MM_to_SPACE);
	 }else pi->spacecxR=pi->spacecx;
	 src-=pi->spacecxR;
   }

  }
  xpage=xitems->count;
  ypage=yitems->count;
  pagecount=xpage*ypage;
  }

 EnableWindow(Rpage1,!di);
 EnableWindow(Rpage2,!di);

 SetDouble(Tcopies ,copies);

 if(pagecount<2){
  char _tmp[128];
  LoadString(HResource,IDS_MSG19,_tmp,sizeof(_tmp));
  SetWindowText(Cpagesenum,"Все размещается на 1 странице");
  allpages=1;
   EnableWindow(Tpages,0);
 }else{
   EnableWindow(Tpages,1);
  char s[256];
  wsprintf(s,"Всего страниц : %d (%d x %d)",pagecount,xpage,ypage);
   SetWindowText(Cpagesenum,s);
   }
 if(allpages){
   SendMessage(Rpage1,BM_SETCHECK,1,0);
   SendMessage(Rpage2,BM_SETCHECK,0,0);
 }else{
   SendMessage(Rpage1,BM_SETCHECK,0,0);
   SendMessage(Rpage2,BM_SETCHECK,1,0);
  }
 }
 if(flags&2){
 SendMessage(Cextent,BM_SETCHECK,allextent,0);
 SetDouble(Tsleft  ,rect2dcustom.left);
 SetDouble(Tstop   ,  rect2dcustom.top);
 SetDouble(Tsright ,rect2dcustom.right);
 SetDouble(Tsbottom,rect2dcustom.bottom);
 EnableWindow(Tsleft  ,!allextent);
 EnableWindow(Tstop   ,!allextent);
 EnableWindow(Tsright ,!allextent);
 EnableWindow(Tsbottom,!allextent);
 }
 if(flags%4){

 SetDouble(Tfleft  ,fileds.left);
 SetDouble(Tftop   ,fileds.top);
 SetDouble(Tfright ,fileds.right);
 SetDouble(Tfbottom,fileds.bottom);

 SetDouble(Taright ,addf.x);
 SetDouble(Tabottom,addf.y);
 SendMessage(Cause,BM_SETCHECK,addborders,0);
 EnableWindow(Taright ,addborders);
 EnableWindow(Tabottom,addborders);

 }
};
 void TPrintSpace::SetDouble(HWND h,double d){
	char text[20];
	sprintf(text,"%g",d);
	SetWindowText(h,text);
 };
