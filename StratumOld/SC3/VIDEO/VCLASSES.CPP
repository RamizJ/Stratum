#undef STRATUM_INTERNAL;
#include <gwin2d3d\gwinfunc.h>
#include <mem.h>

#ifndef WIN32
#include <stdlib.h>
#endif

#include "video\vengine.h"
#include "video\scvideo.rh"
#include "video\vid_type.h"
#include <string.h>
#include <stdio.h>
#include <stratum\stratum.h>

#ifdef WIN32
#define SC_huge *
#else
#define SC_huge _huge*

#endif

HRESULT AVIError(HRESULT r)
{
if(r!=AVIERR_OK){
char * _ps="No description";
switch((LONG)r){
 case REGDB_E_CLASSNOTREG:_ps="No handler could be found to open this type of file.";break;
 case AVIERR_UNSUPPORTED      :_ps="Compression is not supported for this type of data. This error might be returned if you try to compress data that is not audio or video.";break;
 case AVIERR_BADFORMAT        :_ps="The file was corrupted or not in the proper format, and could not be read.";break;
 case AVIERR_MEMORY           :_ps="Not enough memory.";break;
 case AVIERR_INTERNAL         :_ps="AVIERR_INTERNAL";break;
 case AVIERR_BADFLAGS         :_ps="AVIERR_BADFLAGS";break;
 case AVIERR_BADPARAM         :_ps="AVIERR_BADPARAM";break;
 case AVIERR_BADSIZE          :_ps="AVIERR_BADSIZE";break;
 case AVIERR_BADHANDLE        :_ps="AVIERR_BADHANDLE";break;
 case AVIERR_FILEREAD         :_ps="case A disk error occurred while reading the file.";break;
 case AVIERR_FILEWRITE        :_ps="AVIERR_FILEWRITE";break;
 case AVIERR_FILEOPEN         :_ps="A disk error occurred while opening the file.";break;
 case AVIERR_COMPRESSOR       :_ps="AVIERR_COMPRESSOR";break;
 case AVIERR_NOCOMPRESSOR     :_ps="A suitable compressor cannot be found.";break;
 case AVIERR_READONLY         :_ps="The file has been opened with write permission";break;
 case AVIERR_NODATA           :_ps="There is no stream in the file corresponding to the values passed in for fccType and lParam.";break;
 case AVIERR_BUFFERTOOSMALL   :_ps="The buffer provided is not large enough to hold the entire format.";break;
 case AVIERR_CANTCOMPRESS     :_ps="AVIERR_CANTCOMPRESS";break;
 case AVIERR_USERABORT        :_ps="The user wishes to abort the save operation";break;
 case AVIERR_ERROR            :_ps="AVIERR_ERROR";break;

}
 char s[256];
 sprintf(s,"Video error (code %ld): %s",r,_ps);
 SCMessage(s);
}

return r;
}

#ifndef WIN32
ULONG AVIError(ULONG r){return (ULONG)AVIError((HRESULT)r);}
#endif



BOOL TAviVideo::DrawFrame(HDC hdc, int xDst, int yDst,int dxDst, int dyDst,
     LONG *pos,int xSrc,int ySrc, int dxSrc,   int dySrc  ){
  return	DrawDibDraw(ghdd[VideoStream],hdc,
		 xDst, yDst,dxDst,dyDst,
		 GetFrame(pos), NULL,
		 xSrc,ySrc, // SRC ORG
		 dxSrc,dySrc, // SRC SIZE
		 DDF_BACKGROUNDPAL
		 );
}
char * TAviVideo::GetFileName(BOOL fullname,char*buffer,HSP2D hsp){
 if((!fullname) && hsp)
   {
    char s[260];
    char _buf[260];
    if(GetTexturePath2d(hsp,s)){
     if(s[lstrlen(s)]!='\\')lstrcat(s,"\\");
     lstrcpy(_buf,file);
     AnsiUpper(s);AnsiUpper(_buf);

     int i = 0;
     char * pf=_buf;
     char * pp=s;
     while (*pf && *pp && ((*pp)==(*pf))){i++;pf++;pp++;}
      if(i && s[i-1]=='\\'){
       lstrcpy(buffer,"?");
        char * pf=file+i;
        char * pp=s+i;
        while(pp && *pp){
         lstrcat(buffer,"..\\");
         char *_pp=strstr(pp,"\\");
         if(_pp)pp=_pp+1;
        };
        lstrcat(buffer,pf);
        return buffer;
      }
    }
   }
 return file;
};
BOOL TWrite::Dialog(HWND _hwnd){
// HWND hwnd = GetSpaceWindow2d(hsp);
 return ICCompressorChoose(_hwnd,ICMF_CHOOSE_KEYFRAME|
                                ICMF_CHOOSE_DATARATE|
                                ICMF_CHOOSE_ALLCOMPRESSORS,
                                ps,NULL,&(cmp),"Video configure");
};
TWrite::TWrite(HSP2D _hsp,char*file,int x,int y,int cx,int cy,char*f):TVideo(VIDEOWRITE,sizeof(TWrite)){
 POINT2D  Org2d;
 cmp.cbSize=sizeof(COMPVARS);
 if(GetOrgSpace2d(_hsp,&Org2d )){ //test for space exist
   hsp=_hsp;
   filename=NewStr(file);
   SetRect(&rect,x,y,cx,cy);
   HRESULT		hr;
  hr = AVIError(AVIFileOpen(&pfile,		// returned file pointer
					 file,		// file name
	   OF_WRITE|OF_CREATE,	    // mode to open file with
				   NULL));		// use handler determined

	if (hr != AVIERR_OK) return;

   hbuffer =_CreateDIB2d(cx,cy,24,RGB(0,0,0));
   image = (LPBITMAPINFOHEADER) GlobalLock(hbuffer);

	 AVISTREAMINFO strhdr;

	memset(&strhdr, 0, sizeof(strhdr));
	strhdr.fccType                = streamtypeVIDEO;// stream type
	strhdr.fccHandler             = 0;
	strhdr.dwScale                = 1;
	strhdr.dwRate                 = 15;		    // 15 fps
	strhdr.dwSuggestedBufferSize  = image->biSizeImage;
	SetRect(&strhdr.rcFrame, 0, 0,		    // rectangle for stream
		(int) image->biWidth,
		(int) image->biHeight);

	 while  (f && *f){
	  if(*f=='/'){
	  char s[256];
	  char *_ps=strstr(f+1,"/");
	  if(_ps){lstrcpyn(s,f+1,(_ps-f)+1);f=_ps;}else {lstrcpy(s,f+1);f=0;}
	  _ps=strstr(s,"=");
	  if(_ps){
		char ss[256];
		lstrcpyn(ss,s,_ps-s+1);
		if(!lstrcmpi(ss,"driver")){
		 memcpy(&(cmp.fccHandler),_ps+1,4);
		 continue;
		}
		if(!lstrcmpi(ss,"fps")){
		 int i;
		 sscanf(_ps+1,"%d",&i);
		 strhdr.dwRate=i;
		 continue;
		}
		if(!lstrcmpi(ss,"Quality")){
		 int i;
		 sscanf(_ps+1,"%d",&i);
		 cmp.lQ=i;
		 continue;
		}
	  }
	 }else f=strstr(f,"/");
	}

	// And create the stream;
	hr = AVIError(AVIFileCreateStream(pfile,		    // file pointer
				   &ps,		    // returned stream pointer
				   &strhdr));	    // stream header

	if (hr != AVIERR_OK) return;

 }
};

BOOL TWrite::_Init(){
 if(!count){
	HRESULT		hr;
	AVICOMPRESSOPTIONS	opts;
	memset(&opts, 0, sizeof(opts));
	opts.dwFlags = AVICOMPRESSF_VALID;
	opts.fccType = streamtypeVIDEO;
	opts.fccHandler = mmioFOURCC('M', 'S', 'V', 'C');
	opts.dwQuality = 6500;
	opts.dwKeyFrameEvery = 7;
  if(cmp.dwFlags&ICMF_COMPVARS_VALID){
   opts.fccHandler = cmp.fccHandler;
   opts.dwKeyFrameEvery =  cmp.lKey;
   opts.dwBytesPerSecond = cmp.lDataRate;
   opts.dwQuality = cmp.lQ;
  }


	hr = AVIError(AVIMakeCompressedStream(&psCompressed, ps, &opts, NULL));
	if (hr != AVIERR_OK) return FALSE;

	hr = AVIError(AVIStreamSetFormat(psCompressed, 0,
				   image,	    // stream format
				   image->biSize +   // format size
				   image->biClrUsed * sizeof(RGBQUAD)));
	if (hr != AVIERR_OK) return FALSE;
 }
 return TRUE;
}

TWrite::~TWrite(){
if (ps)
	AVIError(AVIStreamClose(ps));

	if (psCompressed)
	AVIError(AVIStreamClose(psCompressed));

	if (psText)
	AVIError(AVIStreamClose(psText));

	if (pfile)
	AVIError(AVIFileClose(pfile));

	if(image)GlobalUnlock(hbuffer);
	if(hbuffer)GlobalFree(hbuffer);
	if(filename)delete filename;
};

long ScanBytes(int pixWidth, int bitsPixel);
long NColors(WORD bitCount);

char *TWrite::GetFileName(BOOL fullname,char*,HSP2D hsp){
 return filename;
};
int TWrite::Write(){
 if(!count){
  if(!_Init())return -1;
 }
   char *bits=(char*)image;
	char * colortable=((char*)image)+sizeof(BITMAPINFOHEADER);
   int attr=24;
   switch(attr){
	 case  1:bits=colortable+8; break;
	 case  4:bits=colortable+16*4; break;
	 case  8:bits=colortable+256*4; break;
	 case 24:bits=colortable; break;
	};

	  POINT2D org;
	  GetOrgSpace2d(hsp,&org);
	  HDC dc;
	  HDC hdc = GetDC(0);
	  dc=CreateCompatibleDC(hdc);
	  HBITMAP hbm=CreateCompatibleBitmap(hdc,image->biWidth,image->biHeight);
	  POINT2D p;p.x=rect.left;p.y=rect.top;
	  SetOrgSpace2d(hsp,&p);
	  SelectObject(dc,hbm);
     RECT r1;
     r1.left=0;r1.top=0;r1.bottom=image->biHeight;r1.right=image->biWidth;
	  PaintSpace2d(hsp,dc,&r1,0);

	  long sizeimage = GetDIBits(dc,hbm,0,image->biHeight,bits,(LPBITMAPINFO)image,DIB_RGB_COLORS);
	  DeleteDC(dc);
	 ReleaseDC(0,hdc);
	  DeleteObject(hbm);
	  SetOrgSpace2d(hsp,&org);
	HRESULT hr = AVIError(AVIStreamWrite(psCompressed,	// stream pointer
				  count ,		    // time of this frame
				  1,		            // number to write
				  (LPBYTE) image +      // pointer to data
				  image->biSize +
				  image->biClrUsed * sizeof(RGBQUAD),
				  image->biSizeImage,// size of this frame
				  AVIIF_KEYFRAME,	    // flags....
				  NULL, NULL));
		if (hr != AVIERR_OK)return -1;
   count++;
return count;
};



TFrame * GetFrame(HSP2D hsp,HOBJ2D obj){
USEROBJSTRUCT * u= GetUserData2d(hsp,obj);
if (u)return (TFrame *)u->ptr;else return NULL;
};

struct VSTREAM{
PAVISTREAM stream;
VSTREAM *next;
VSTREAM(){memset(this,0,sizeof(VSTREAM));}
};


LRESULT CALLBACK Video2dProc(HSP2D hsp,USEROBJSTRUCT*u,UINT command,DWORD lParam){
 if (command==OM_DESTROYTEXTURE){
     if(!lParam)return NULL;
     HSP3D hsp3d=(HSP3D)hsp;
     HOBJ2D ht=(HOBJ2D)u;
	  TVideo*_v=NULL;
//     char * avifile=(char *)lParam;
		for(C_TYPE i=0;i<videos->count;i++){
			 TVideo*v=(TVideo*)videos->At(i);
      if(v && v->Textures)
       {
         for(C_TYPE j=0;j<v->Textures->count;j++){
           Texture3d *t3d=(Texture3d *)v->Textures->At(j);
           if((t3d->hsp3d==hsp3d)&&(t3d->hMaterial==ht)){
            delete t3d;
            v->Textures->AtDelete(j);

			if (v->Frames->count==0 && v->flags&V2_AUTODELETE &&
              (v->Textures->count==0)){
               videos->AtPut(videos->IndexOf(v),NULL);
               delete v;
            }
            break;
           }}}}

   return 0;
 };
 if (command==OM_GETBITMAP){
    if(!lParam)return NULL;
	 char * avifile=
	 (char *)lParam;
	  TVideo*_v=NULL;
		for(C_TYPE i=0;i<videos->count;i++){
			 TVideo*v=(TVideo*)videos->At(i);
           if(v){

           if(v->IsEqual(avifile)){
             _v=v;break;
           }

              }}
			if(!_v){
                 _v=CreateVideo(avifile,V2_AUTODELETE);
                }
        if(!_v)return 0;
        if(!_v->Textures)_v->Textures=new TCollection(10,10);
        if(!_v)return 0;
       LPBITMAPINFOHEADER lpbi=_v->GetFrame(NULL);

        HSP3D hsp3d=(HSP3D)hsp;
        HOBJ2D ht=(HOBJ2D)u;
        BOOL okinsert=TRUE;
         for(C_TYPE j=0;j<_v->Textures->count;j++){
           Texture3d *t3d=(Texture3d *)_v->Textures->At(j);
           if((t3d->hsp3d==hsp3d)&&(t3d->hMaterial==ht)){okinsert=FALSE;break;}
         }
        if(okinsert){
           Texture3d *t3d=new Texture3d;
           t3d->hsp3d=hsp3d;
           t3d->hMaterial=ht;
           _v->Textures->Insert(t3d);
        }
      return (LRESULT)lpbi;
 };

TFrame * f=((TFrame *)u->ptr);

if (command==OM_LOAD){
  USERSTREAM * us=(USERSTREAM *)lParam;
  f=new TFrame(u,us->size,us->data);
  u->ptr=f;
  frames->Insert(f);
  return 1;
 }

if(f){
 switch(command){
  case OM_SPACEVALID:f->hsp=hsp;f->handle=u->handle;break;
  case OM_AWAKEN:f->hsp=hsp;break;
  case OM_DESTROY:{
			TVideo * video=NULL;
			if (f->video){video=f->video;}
			delete f;
			if (video)video->Frames->Delete(f);
				else frames->Delete(f);

			if (video && video->Frames->count==0 && video->flags&V2_AUTODELETE &&
              (video->Textures==NULL || (video->Textures->count==0))){
             videos->AtPut(videos->IndexOf(video),NULL);
             delete video;
			}
			return 0;
	 }
  }
 return f->Proc2d(u,command,lParam);
 }
return 0;
};

/* Init video system */

HBITMAP HGRAYDIB=0;
BOOL soundpresent=0;

int InitVideo(){

// Определяем наличие звуковой поддержки
UINT num=waveOutGetNumDevs();
 if(num){
 WAVEOUTCAPS  caps;
 for(int i=0;i<num;i++){
  if(waveOutGetDevCaps(i,&caps,sizeof(caps))==MMSYSERR_NOERROR){
   soundpresent=TRUE;
  }
 }
 };

 AVIFileInit();
 palette=(HPALETTE)SetSpaceParam2d(0,SP_GETPALETTE,0);
 USERTYPESTRUCT ut;
 ut.name="VIDEO2D";
 ut.objproc=Video2dProc;
 ut.flags=UF_MMTEXT;
 ut.usersize=0;
 int reg=RegisterUserType2d(&ut);
 videos=new TCollection(10,10);
 frames=new TCollection(10,10);
 playvideos=new TCollection(2,2);
 playframes=new TCollection(2,2);
 if (reg){
	HRSRC hrsrc;
//	HGLOBAL HGRAYDIB;
//	hrsrc = FindResource(hInstance,MAKEINTRESOURCE(BM_MASK), RT_BITMAP);
//	HGRAYDIB = LoadResource(hInstance, hrsrc);
//	LockResource(HGRAYDIB);
//	BGrayed = CreateDIBPatternBrush(HGRAYDIB, DIB_RGB_COLORS);
//	UnlockResource(HGRAYDIB);
HGRAYDIB = LoadBitmap(hInstance,MAKEINTRESOURCE(BM_MASK));
 BGrayed = CreatePatternBrush(HGRAYDIB);

	}
 return reg;
};

/*
  Done video system
*/

void DoneVideo(){
if (audio)delete audio;

 for(C_TYPE i=0;i<frames->count;i++){
	TFrame*f=(TFrame*)frames->At(i);
	if (f) delete f;
 } delete frames;

 for(i=0;i<videos->count;i++){
	TVideo*v=(TVideo*)videos->At(i);
	if (v){
    delete v;
	}
 }delete videos;


 delete playvideos;
 delete playframes;
 DeleteObject(BGrayed);
 DeleteObject(HGRAYDIB);
// FreeResource(HGRAYDIB);
 if(hCapDll)FreeLibrary(hCapDll);
 AVIFileExit();
};

TAviVideo::TAviVideo(char *_file,DWORD f):TVideo(VIDEOAVI,sizeof(TAviVideo))
{
#ifdef LOGON
  MessageBox(0,"TAviVideo constructor called","awef",0);
 	//LogMessage("CreateVideo called");
#endif
	flags=f;
  AVIFileInit();

	hr = AVIFileOpen(&pfile, _file, OF_READ | OF_WRITE | OF_SHARE_DENY_NONE, NULL);
  if (hr != 0) {return;	}
#ifdef LOGON
  MessageBox(0,"AVIFileOpen called success","awef",0);
 	//LogMessage("CreateVideo called");
#endif
 	int i=0;
	file=NewStr(_file);
	AnsiUpper(file);
	VSTREAM*next=new VSTREAM();
	VSTREAM*vs=next;
	VSTREAM*prev=NULL;
	while (AVIFileGetStream(pfile,&(vs->stream), 0L, i) == AVIERR_OK)
  {
		vs->next=new VSTREAM;
		prev=vs;
		vs=vs->next;
  	i++;
 	}
 	streamcount=i;
 	gapavi=new PAVISTREAM[streamcount];

 	prev->next=0;
 	streamcount=i;
  i=0;
  while (next)
  {
   	gapavi[i]=next->stream;
   	VSTREAM*vs=next;
   	next=next->next;
   	delete vs;
   	i++;
 	}

  gaAVIOptions=new AVICOMPRESSOPTIONS[streamcount];
  galpAVIOptions=new LPAVICOMPRESSOPTIONS[streamcount];
  gapgf=new PGETFRAME[streamcount];
  ghdd=new HDRAWDIB[streamcount];

  memset(gaAVIOptions,0,sizeof(AVICOMPRESSOPTIONS)*streamcount);
  memset(galpAVIOptions,0,sizeof(LPAVICOMPRESSOPTIONS)*streamcount);
  memset(gapgf,0,sizeof(PGETFRAME)*streamcount);
  memset(ghdd,0,sizeof(HDRAWDIB)*streamcount);
 	InitStreams();
 	Frames=new TCollection(5,1);
};

TAviVideo::~TAviVideo(){
if (file){
if (streamcount){
//AVISaveOptionsFree(streamcount, galpAVIOptions);
 for(int i=0;i<streamcount;i++){
	if (gapgf[i]) {
		 AVIError(AVIStreamGetFrameClose(gapgf[i]));
		 gapgf[i] = NULL;
	}
	if (ghdd[i]) {
		 DrawDibClose(ghdd[i]);
		 ghdd[i] = 0;
	}
	if (galpAVIOptions[i] && galpAVIOptions[i]->lpFormat)
	  GlobalFreePtr(galpAVIOptions[i]->lpFormat);
	 AVIStreamRelease(gapavi[i]);
 }

delete  gapavi;
delete  gaAVIOptions;
delete  galpAVIOptions;
delete  gapgf;
delete  ghdd;
}

 delete file;
 AVIFileRelease(pfile);
 }
};
void TAviVideo::InitStreams()
{
#ifdef LOGON
  MessageBox(0,"InitStreams called","awef",0);
 	//LogMessage("CreateVideo called");
#endif
	LONG	lTemp;
 	int		i;
// WORD	w;
 	AVISTREAMINFO		  avis;
  //
  // Start with bogus times
  //
	timeStart = 0x7FFFFFFFl;
	timeEnd   = 0;
  //
 	// Walk through and init all streams loaded
 	//

	for (i = 0; i < streamcount; i++)
  {
  	AVIStreamInfo(gapavi[i], &avis, sizeof(avis));
	//
	// Save and SaveOptions code takes a pointer to our compression opts
	//
		galpAVIOptions[i] = &gaAVIOptions[i];
	//
	// clear options structure to zeroes
	//
		_fmemset(galpAVIOptions[i], 0, sizeof(AVICOMPRESSOPTIONS));
	//
	// Initialize the compression options to some default stuff
	// !!! Pick something better
	//
		galpAVIOptions[i]->fccType = avis.fccType;
//	char * type=(char *)(&avis.fccType);
		switch(avis.fccType)
    {
    case streamtypeVIDEO:
			galpAVIOptions[i]->dwFlags = AVICOMPRESSF_VALID |	AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_DATARATE;
			galpAVIOptions[i]->fccHandler = NULL;
			galpAVIOptions[i]->dwQuality = (DWORD)ICQUALITY_DEFAULT;
			galpAVIOptions[i]->dwKeyFrameEvery = 7;	// !!! ask compressor?
			galpAVIOptions[i]->dwBytesPerSecond = 60000u;
			framestart=AVIStreamStart(gapavi[i]);
			frameend=AVIStreamEnd(gapavi[i]);
			{
				BYTE	  abFormat[1024];
				LPBITMAPINFOHEADER lpbi;
	 			LONG l=sizeof(abFormat);
	 			AVIError(AVIStreamReadFormat(gapavi[i], framestart, &abFormat, &l));
	 			lpbi = (LPBITMAPINFOHEADER)abFormat;
	  		framesize.x=(int)lpbi->biWidth;
	  		framesize.y=(int)lpbi->biHeight;
			}
			break;
    case streamtypeAUDIO:
    {
			galpAVIOptions[i]->dwFlags |= AVICOMPRESSF_VALID;
			galpAVIOptions[i]->dwInterleaveEvery = 5;
			AVIError(AVIStreamReadFormat(gapavi[i], AVIStreamStart(gapavi[i]), NULL, &lTemp));
			galpAVIOptions[i]->cbFormat = lTemp;
			if (lTemp)
				galpAVIOptions[i]->lpFormat = GlobalAllocPtr(GHND, lTemp);
		// Use current format as default format
			if (galpAVIOptions[i]->lpFormat)
				AVIError(AVIStreamReadFormat(gapavi[i],
					AVIStreamStart(gapavi[i]),
					galpAVIOptions[i]->lpFormat,
					&lTemp));
		 		LPWAVEFORMAT lpa=(LPWAVEFORMAT)(galpAVIOptions[i]->lpFormat);
		 		audioChannels=lpa->nChannels;
		 		audioSamplesPerSec=lpa->nSamplesPerSec;
		 		audioFormatTag=lpa->wFormatTag;
			}
			break;
    default:
			break;
		}
	//
	// We're finding the earliest and latest start and end points for
	// our scrollbar.
	//
		timeStart = min(timeStart, AVIStreamStartTime(gapavi[i]));
		timeEnd   = max(timeEnd, AVIStreamEndTime(gapavi[i]));
#ifdef LOGON
char s[200];
wsprintf(s,"VideoFrame Info:\nStart: %d, End: %d\nWidth: %d, Height: %d\nTimeStart: %d, TimeEnd: %d",framestart,frameend,framesize.x,framesize.y,timeStart,timeEnd);
MessageBox(0,s,"awef",0);
#endif
	//
	// Initialize video streams for getting decompressed frames to display
	//
		if (avis.fccType == streamtypeVIDEO)
    {
#ifdef LOGON
  MessageBox(0,"Initialize streamtypeVIDEO","awef",0);
#endif
    	gapgf[i] = AVIStreamGetFrameOpen(gapavi[i], NULL);
#ifdef LOGON
  if(gapgf[i])
  	MessageBox(0,"AVIStreamGetFrameOpen success","awef",0);
  else
  	MessageBox(0,"AVIStreamGetFrameOpen failed","awef",0);
#endif
      if (gapgf[i] == NULL) continue;
      ghdd[i] = DrawDibOpen();
		 	// !!! DrawDibBegin?
      if (gpaviVideo == NULL)
      {
    //
		// Remember the first video stream --- treat it specially
		//
				gpaviVideo = gapavi[i];
				VideoStream=i;
				DrawDibSetPalette(ghdd[VideoStream],palette);
		// Move one frame on the top video screen for each HSCROLL
	 			//timehscroll = muldiv32(1000, avis.dwScale, avis.dwRate);
				//AVIStreamSampleToTime(PAVISTREAM pavi, LONG lSample)
			 	framecount=AVIStreamLength(gpaviVideo);
			 	frameend=framestart+(framecount-1);
#ifdef LOGON
  if(gpaviVideo)
	  MessageBox(0,"Initialize success","awef",0);
  else
	  MessageBox(0,"Initialize failed","awef",0);
 	//LogMessage("CreateVideo called");
#endif
		 	}
    }
    else if (avis.fccType == streamtypeAUDIO)
    {
     //
		 // Remember the first audio stream --- treat it specially
		 //
		 	if (gpaviAudio == NULL)
      {
#ifdef LOGON
  MessageBox(0,"Initialize streamtypeAUDIO","awef",0);
 	//LogMessage("CreateVideo called");
#endif
				gpaviAudio = gapavi[i];

#ifdef LOGON
  if(gpaviAudio)
	  MessageBox(0,"Initialize success","awef",0);
  else
	  MessageBox(0,"Initialize failed","awef",0);
#endif
      }
    }
	}
};
BOOL TAviVideo::GetTimeBySample(DWORD s,DWORD& t){
	if (s>=framestart && s<=frameend){
		DWORD d=timeEnd-timeStart;
		t=(d*(s-framestart)/(frameend-framestart))+framestart;
		return TRUE;
	}
  return FALSE;
};

BOOL TAviVideo::GetSampleByTime(DWORD t,DWORD& s,DWORD &_glPlayStartTime,DWORD _start,DWORD _end,BOOL looping){
 DWORD time=t-_glPlayStartTime;
 BOOL rez=TRUE;

 s=(((double)(time-timeStart)/((double)(timeEnd-timeStart)))*(frameend-framestart))+framestart+_start;
 if (s<_start)return FALSE;
 if (looping){
  if (s>_end){
	DWORD d=timeEnd-timeStart;
	DWORD t1=(d*(_start-framestart)/(frameend-framestart))+framestart;
	DWORD t2=(d*(_end-framestart)/(frameend-framestart))+framestart;
			d=t2-t1;
    s=( (s-(+framestart+_start)) % (_end-_start) )+(framestart+_start);
	if (d){ while (time>t2)time-=d;
			  _glPlayStartTime=t-time;
	 }
  }
 }else{
  // No Looping
	if (s>_end){s=_end;rez=FALSE;}
 }
 return rez;
};

LPBITMAPINFOHEADER TAviVideo::GetFrame(LONG*_pos){
 return (LPBITMAPINFOHEADER)AVIStreamGetFrame(gapgf[VideoStream],_pos?(*_pos):(pos));
};

long TAviVideo::GetRate(){return 100;};

char * NewStr(char * astr)
	  { int i=lstrlen(astr);
			if (i)
			{char * s=new char[i+1];
					 lstrcpy(s,astr);
			 return s;
			 }
			 return NULL;
	  };

TFrame::TFrame(USEROBJSTRUCT*u,long size,void * d){
 memset(this,0,sizeof(TFrame));
/*
 if (sizeof(FrameSTREAM)>size){
		_Error("Записанный размер о Video Frame меньше требуемого!");
	 return;}
*/
 FrameSTREAM *fs = (FrameSTREAM *)d;
 handle= u->handle;

 src.left   = fs->src.left;
 src.right  = fs->src.right;
 src.top    = fs->src.top;
 src.bottom  = fs->src.bottom;
 flags = fs->flags;
 vflags= fs->flags;
 pos   = fs->pos;
 sense=fs->sense;
 if(fs->version>1){
  color=fs->color;
 };
 char * _name=fs->video;
 if(fs->version==1){
  _name=(char*)&(fs->color);
 }
 avifile=NewStr(_name);

 ;
};

TFrame::TFrame(HSP2D _hsp,TVideo *v,gr_float x,gr_float y,gr_float sizex,gr_float sizey,RECT *_src,DWORD _flags){
memset(this,0,sizeof(TFrame));
video=v;
hsp=_hsp;
pos=video->GetPos();
flags=_flags;
//video=v;
USEROBJSTRUCT uo;
 uo.type="VIDEO2D";
 uo.x=x;
 uo.y=y;
 uo.handle=0;
 uo.data=0;
 uo.ptr=this;
 if (_src){src=*_src;
  if (src.left<0)src.left=0;
  if (src.top<0)src.top=0;
  if (src.right>v->framesize.x || src.right<0)src.right=v->framesize.x;
  if (src.bottom>v->framesize.y || src.bottom<0)src.bottom=v->framesize.y;
 }else {src.left=0;src.top=0;src.right=v->framesize.x;src.bottom=v->framesize.y;}

 uo.sizex=(sizex<0)?(src.right-src.left):sizex;
 uo.sizey=(sizey<0)?(src.bottom-src.top):sizey;

 handle=CreateUserObject2d(hsp,&uo);
};

TFrame::~TFrame(){
 if(video && video->mode==VIDEOCAMERA){
  if(video->Frames->count>1)goto m2;
 }

 StopPlay();
m2:
 if(avifile)delete avifile;
};

void TFrame::_Awaken(TVideo *v){
  video=v;
  video->Frames->Insert(this);
  frames->Delete(this);
  delete avifile;avifile=NULL;
};

BITMAPINFOHEADER _info_mono={sizeof(_info_mono),
			1024/*dx*/,1,1,1,0,1024*3/*dx*3*/,300,300,0,0};
static RGBQUAD _info_mono_rgb[2]={{0,0,0},{255,255,255}};
//_info_mono_rgb должно идти за _info_mono

 BOOL TFrame::VideoConnect(){
  if (!video){ // Загрузка
			 char _buf[260]="";
		   if(*avifile=='?'){
			 if(GetTexturePath2d(hsp,_buf)&& lstrlen(_buf)>0)
			  {
			   if(_buf[lstrlen(_buf)]!='\\')lstrcat(_buf,"\\");
			  }else return FALSE;
         char *_a=avifile+1;
         while (*_a=='.' && _a[1]=='.'&& _a[2]=='\\'){
          _a+=3;
          char *ps=(_buf+lstrlen(_buf))-2;
          while ((ps-_buf)>0 && *ps!='\\')ps--;
          if(*ps=='\\')ps[1]=0;
         }

		   lstrcat(_buf,_a);
		   }else lstrcpy(_buf,avifile);

				 TVideo*_v=NULL;
			 char buffer[256];
				  for(C_TYPE i=0;i<videos->count;i++){
					 TVideo*v=(TVideo*)videos->At(i);
				if(v){
           /*
				char * v_file=v->GetFileName(TRUE,buffer);
					 if (!lstrcmpi(v_file,_buf)){
						if (_v){
									if (v->flags==vflags)_v=v;
				 Il m'y a pas d'aillemes				 } else _v=v;
					  }
           */
           if(v->IsEqual(_buf)){
             _v=v;break;
           }


                 }}

				  if (_v){_Awaken(_v);return 1;}
              TVideo*v=CreateVideo(_buf,V2_AUTODELETE);
              if(v)_Awaken(v);
              /*
				  TVideo*v= new TAviVideo(_buf,vflags);
				  if (v->Ok()){
			      HV2D hv=0;
				  InserVideo(v,hv);
					_Awaken(v);
				  }else {delete v;}
              */
  }
  return TRUE;
 };
LRESULT TFrame::Proc2d(USEROBJSTRUCT*,UINT command,DWORD lParam){

static FrameSTREAM *fs=NULL;

switch(command){
case OM_ACTUALSIZE:{
	POINT2D * p=(POINT2D *)lParam;
	 p->x=src.right-src.left;
	 p->y=src.bottom-src.top;
	}	return 1;
case OM_AWAKEN:return 1;
case OM_SPACEVALID:VideoConnect();return 1;
case OM_POSTSAVE:delete fs;fs=0;break;
case OM_SAVE:{
 USERSTREAM * us=(USERSTREAM *)lParam;
              char buffer[260];
              char * _name=avifile;
              if(video){
                  _name=video->GetFileName(FALSE,buffer,hsp);
              }
				  int l=lstrlen(_name);
				  int size=sizeof(FrameSTREAM)+l;

				  fs=(FrameSTREAM *)new char[size];
				  us->size=size;
				  us->data=fs;
				  us->flags|=1;
				  us->flags=(INT16)(video?video->flags:vflags);
				  fs->version=2; // 1 Без цвета
              fs->src.left  = src.left;
              fs->src.right = src.right;
              fs->src.top   = src.top;
              fs->src.bottom= src.bottom ;
              fs->color=color;
				  fs->flags=flags;
				  fs->pos=pos;
				  fs->length=(INT16)l;
              fs->sense=sense;
				  lstrcpy(fs->video,_name);
				 }return 1;
case OM_PAINT:{
  if(!video)VideoConnect();
  USERPAINT * up=(USERPAINT*)lParam;
  RECT r;
  if (flags&V2_DISABLE || !video || !video->Ok()){
badpaint:
    {
     int _x=up->_org.x,_y=up->_org.y;

	  HGDIOBJ hbrOld = SelectObject(up->hDc,BGrayed);
	  PatBlt(up->hDc,_x,_y,up->_size.x, up->_size.y,PATCOPY);
	  SelectObject(up->hDc,hbrOld);
	  return 0;
    }
  }else{

	  HDC hDc;
	  HBITMAP hbmp;
	  int _x=up->_org.x,_y=up->_org.y;
  if (flags&V2_TRANSPARENT){
   _x=_y=0;
	hbmp=CreateCompatibleBitmap(up->hDc,up->_size.x,up->_size.y);
	hDc=CreateCompatibleDC(up->hDc);
	SelectObject(hDc,hbmp);
  }else hDc= up->hDc;
	if(!(video->DrawFrame(hDc, _x,_y,
		 up->_size.x, up->_size.y,&pos,
		 src.left,src.top, // SRC ORG
		 src.right-src.left,src.bottom-src.top)))goto badpaint;

	if (flags&V2_TRANSPARENT){
	 int _sizex=src.right-src.left,_sizey=src.bottom-src.top;
    LPBITMAPINFOHEADER lpbi=video->GetFrame(&pos);
	 int perline=ScanBytes(_sizex,lpbi->biBitCount);
	 int perline_mono=ScanBytes(_sizex,1);

	 HGLOBAL mono=GlobalAlloc(GMEM_FIXED,perline_mono*_sizey);
	 BYTE * _bytes=(BYTE SC_huge)GlobalLock(mono);
	 switch(lpbi->biBitCount){
	 case 24:{
	 COLORREF SC_huge _colors;
      COLORREF Color=RGB(GetBValue(color),GetGValue(color),GetRValue(color));
	  for(int y=0;y<_sizey;y++){

		_colors=(COLORREF SC_huge)(((BYTE*)(lpbi))+sizeof(BITMAPINFOHEADER)+perline*((_sizey-1)-y));
		BYTE * bytes=_bytes+perline_mono*((_sizey-1)-y);
		BYTE b=0;
		int j=0;
if (sense){
// trsense
  BYTE __b=(BYTE)GetBValue(Color);
  BYTE __g=(BYTE)GetGValue(Color);
  BYTE __r=(BYTE)GetRValue(Color);

		for(int x=0;x<_sizex;x++){
		 b<<=1;
       COLORREF _clr=((*_colors)&0xfffffful);
       int _b=(GetRValue(_clr)-__b);
       int _g=(GetGValue(_clr)-__g);
       int _r=(GetBValue(_clr)-__r);
		 if ((abs(_r)<=sense)&&(abs(_g)<=sense)&&(abs(_b)<=sense))b|=0x01;
		 j++;
		 if (j==8){
			*bytes=b;
			bytes++;
			b=0;
			j=0;
		 }
	 ((BYTE SC_huge)_colors)+=3;
		}
}else{
		for(int x=0;x<_sizex;x++){
		 b<<=1;
		 if (((*_colors)&0xffffff)==Color)b|=0x01;
		 j++;
		 if (j==8){
			*bytes=b;
			bytes++;
			b=0;
			j=0;
		 }
	 ((BYTE SC_huge)_colors)+=3;
		}
      } }
	 }break;
    case 8:{
     COLORREF*palette=(COLORREF SC_huge)(((BYTE*)(lpbi))+sizeof(BITMAPINFOHEADER));
     int _id=-1;
     for(int i=0;i<256;i++){if(palette[i]==color){_id=i;break;}}
     if(_id==-1){memset(_bytes,0x0,perline_mono*_sizey);}else
     {
     BYTE id=_id;
	  BYTE SC_huge _colors;
     _id=256;
     if(lpbi->biClrUsed)_id=lpbi->biClrUsed;
     BYTE SC_huge _bits=(((BYTE*)(lpbi))+sizeof(BITMAPINFOHEADER))+sizeof(COLORREF)*_id;

	  for(int y=0;y<_sizey;y++){
		_colors=_bits+perline*((_sizey-1)-y);
		BYTE * bytes=_bytes+perline_mono*((_sizey-1)-y);
		BYTE b=0;
		int j=0;
         for(int x=0;x<_sizex;x++){
          b<<=1;
          if ((*_colors)==id)b|=0x01;
          j++;
          if (j==8){
            *bytes=b;
            bytes++;
            b=0;
            j=0;
          }
          _colors++;
         }
     }}
    }break;
    case 4:{
     COLORREF*palette=(COLORREF SC_huge)(((BYTE*)(lpbi))+sizeof(BITMAPINFOHEADER));
     BYTE SC_huge _bits=(((BYTE*)(lpbi))+sizeof(BITMAPINFOHEADER))+sizeof(COLORREF)*16;
     int id=-1;
     for(int i=0;i<16;i++){if(palette[i]==color){id=i;break;}}
	  BYTE SC_huge _colors;
     int __sizex=_sizex/2;
     if(_sizex&1)__sizex++;

	  for(int y=0;y<_sizey;y++){
		_colors = _bits+perline*((_sizey-1)-y);
		BYTE * bytes=_bytes+perline_mono*((_sizey-1)-y);
		BYTE b=0;
		int j=0;

         for(int x=0;x<__sizex;x++){

          b<<=1;
          if (((*_colors)&0x0f)==id)b|=0x01;
          b<<=1;
          if (((*_colors)&0xf0)==id)b|=0x01;
          j+=2;
          if (j==8){
            *bytes=b;
            bytes++;
            b=0;
            j=0;
          }
          _colors++;
         }

     }
    }break;

		default:memset(_bytes,0x0,perline_mono*_sizey);
	 }

	 _info_mono.biWidth=_sizex;
	 _info_mono.biHeight=_sizey;
	 _info_mono.biSizeImage=perline_mono*_sizey;

	 StretchDIBits(up->hDc,up->_org.x,up->_org.y,up->_size.x, up->_size.y,0,0,_sizex,_sizey,
		_bytes,(BITMAPINFO*)&_info_mono,DIB_RGB_COLORS,SRCAND);
    COLORREF* _c=(COLORREF*)_info_mono_rgb;
    _c[1]=0;
    _c[0]=RGB(255,255,255);

	 StretchDIBits(hDc,0,0,up->_size.x, up->_size.y,0,0,_sizex,_sizey,
		_bytes,(BITMAPINFO*)&_info_mono,DIB_RGB_COLORS,SRCAND);

    _c[0]=0;
    _c[1]=RGB(255,255,255);

	 BitBlt(up->hDc,up->_org.x,up->_org.y,up->_size.x, up->_size.y,hDc,0,0,SRCINVERT);

	 GlobalUnlock(mono);
	 GlobalFree(mono);
	 DeleteDC(hDc);
	 DeleteObject(hbmp);
	}
	 }
  }break;
 }
 return 0;
};

BOOL InserVideo(TVideo *v,HV2D &index)
{
 	for(C_TYPE i=0;i<videos->count;i++)
  {
	  if (!videos->At(i))
    {
     	videos->items[i]=v;
      index=(i+4096);
      return TRUE;
    }
 	}
  index=((videos->Insert(v))+4096);
  return TRUE;
};

TVideo * GetVideo(int hv,VIDEO_MODE vm){
  if (hv>=4096 && hv<videos->count+4096){
   TVideo* v=(TVideo *)videos->At(hv-4096);
	if(v && vm){
          if(vm!=v->mode)v=NULL;
	      }
         return v;
   }
 return 0;
};

long TAviVideo::SetPos(long _pos){
 _pos=max(framestart,_pos);
 _pos=min(frameend,_pos);
 pos=_pos;
 UpdateViews(&pos);
return pos;
};

void TAviVideo::SetPause(BOOL apause){
  TVideo::SetPause(apause);


};

void  TFrame::BeginPlay(long start,long end,int rate,int _flags){
 if(!soundpresent)_flags|=PF_NOAUDIO;

 if (video){
  if (start<video->framestart)start=video->framestart;
  if (end<video->framestart)end=video->framestart;

  if (start>video->frameend)start=video->frameend;
  if (end>video->frameend)end=video->frameend;
 }
 looping=(_flags&PF_CYCLED)!=0;
 if(video->mode==VIDEOCAMERA){
   paused=FALSE;
  ((TCamera*)video)->Capture(TRUE);
//  playvideos->Insert(video);
  return;
 }

 if (playing)StopPlay(TRUE);
 if (flags&V2_SYNC){
 video->looping=looping;
  if (video->CanPlayAudio() && ((_flags&PF_NOAUDIO)==0)){
	  BeginAudio(start,end,video->flags|_flags);
	}else{
//	 video->rate=video->GetRate(0);
	 playvideos->Insert(video);
	}
	video->playstart=start;
	video->playend=end;
	video->playing=TRUE;
	if (::paused){
	 video->glPlayStartTime=pausestarttime-timedelta;
  }else{
	 video->glPlayStartTime=(timeGetTime()-timedelta);
	}
  }else{
 if (video->CanPlayAudio() && ((flags&V2_NOAUDIO)==0) && ((_flags&PF_NOAUDIO)==0)){
	  BeginAudio(start,end,flags|_flags);
	  if (audio)audio->frame=this;
	}else{
//	 rate=video->GetRate(rate);
	 playframes->Insert(this);
	}
	glPlayStartTime=(timeGetTime()-timedelta);
	playstart=start;
	playend=end;
	if (::paused){
	 glPlayStartTime=pausestarttime-timedelta;
  }else{
	 glPlayStartTime=(timeGetTime()-timedelta);
	}
  }
  playing=TRUE;
};

LONG TAviVideo::SampleToTime(LONG s){
  return AVIStreamSampleToTime(gpaviVideo,s);
};

LONG TAviVideo::TimeToSample(LONG t){
  return AVIStreamTimeToSample(gpaviVideo,t);
};

void TFrame::BeginAudio(long Lstart,long Lend,DWORD _flags){
 if(video->mode==VIDEOAVI){
  TAviVideo*video=(TAviVideo*)(TFrame::video);
 if (audio){
	TVideo *_video=audio->video;
	TFrame* _frame=audio->frame;
	 delete audio;audio=0;
	  if (_frame || _video && (_frame!=this)){
		if (_video){
		playvideos->Insert(_video);}else{
		_frame->rate=video->GetRate();
		playframes->Insert(_frame);
	  }
	}
 }

 long _start=AVIStreamSampleToTime(video->gpaviVideo,Lstart);
 long _end=AVIStreamSampleToTime(video->gpaviVideo,Lend);
 long astart=AVIStreamTimeToSample(video->gpaviAudio,_start);
 long aend=AVIStreamTimeToSample(video->gpaviAudio,_end);
 BOOL loop=(_flags&PF_CYCLED)!=0;
 audio=new TAudio(notifywnd,video->gpaviAudio,astart,aend,FALSE);

// audio=new TAudio(notifywnd,video->gpaviAudio,_start,_end,FALSE);

 audio->slEnd1=audio->sdwSamplesPerSec*_end;
 audio->slBegin1=audio->sdwSamplesPerSec*_start;
 audio->sfLooping=loop;
 audio->video=video;
 }
};

void  TFrame::Pause(){
 if (video){
  if(video->mode==VIDEOCAMERA){ StopPlay(FALSE);return;  }

  if (audio && (audio->video==video || audio->frame==this)){
	 audio->Pause();
	 video->SetPause(TRUE);
  }else{
	if (flags&V2_SYNC && !video->pause){
		 video->SetPause(TRUE);
		 paused=1;
		}else paused=1;
  } }
};

void TFrame::Resume(){
 if (video){
  if (audio && (audio->video==video || audio->frame==this)){
	 audio->Resume();
	 video->SetPause(FALSE);
  }else{
	if (flags&V2_SYNC && video->pause){
// синхронизированн с потоком
      if(video->mode==VIDEOAVI){
         TAviVideo*video=(TAviVideo*)(TFrame::video);
			video->SetPause(0);
			DWORD t,ts,tc;
			video->GetTimeBySample(pos,t);
			video->GetTimeBySample(video->framestart,ts);
			tc=(timeGetTime()-timedelta);
			t-=ts;
			tc-=t;
			video->glPlayStartTime=tc;
          }
			paused=0;
		}else {
// НЕ синхронизированн с потоком
if(paused){
        if(video->mode==VIDEOAVI){
        TAviVideo*video=(TAviVideo*)(TFrame::video);
			DWORD t,ts,tc;
			video->GetTimeBySample(pos,t);
			video->GetTimeBySample(video->framestart,ts);
			tc=(timeGetTime()-timedelta);
			t-=ts;
			tc-=t;
			glPlayStartTime=tc;
       }}
         paused=0;
      }
  }}
};
long TFrame::SetPos(long _pos){
if (video){
if (_pos<video->framestart)_pos=video->framestart;
if (_pos>video->frameend)_pos=video->frameend;

if (flags&V2_SYNC && video)video->SetPos(_pos);else
  {
	 if (_pos!=pos){pos=_pos; UpdateObject2d(hsp,handle,NULL);}
  }
}
 return pos;
};

void  TFrame::StopPlay(BOOL videostop){

 if (video){
   if(video->mode==VIDEOCAMERA){
   ((TCamera*)video)->Capture(FALSE);
   playvideos->Delete(video);
   return;
 }

  if (((flags & V2_SYNC)&&  video->Frames->count<2)|| videostop)
		 {video->playing=0; }
  if (audio && (((audio->video==video && video->Frames->count<2)||
		(audio->frame==this))|| videostop)){
	 delete audio;audio=NULL;
  }else{
	 if (flags&V2_SYNC){ playvideos->Delete(video);	}else{
			playframes->Delete(this);
		 }
	} } playing=0;
 UpdatePlaying();
};

BOOL TVideo::GetMarker(VIDEOMARKER*vm){
    long pos=GetPos();
    int _sizex=framesize.x,_sizey=framesize.y;
    LPBITMAPINFOHEADER lpbi=GetFrame(&pos);
    if(!lpbi)return FALSE;
	 int perline=ScanBytes(_sizex,lpbi->biBitCount);
    long count=0;
    long _x=0,_y=0;
	 switch(lpbi->biBitCount){
	 case 24:{
	   COLORREF SC_huge _colors;

  BYTE _r1=(BYTE)GetRValue(vm->color_from);
  BYTE _g1=(BYTE)GetGValue(vm->color_from);
  BYTE _b1=(BYTE)GetBValue(vm->color_from);

  BYTE _r2=(BYTE)GetRValue(vm->color_to);
  BYTE _g2=(BYTE)GetGValue(vm->color_to);
  BYTE _b2=(BYTE)GetBValue(vm->color_to);

	  for(int y=0;y<_sizey;y++){
		_colors=(COLORREF SC_huge)(((BYTE*)(lpbi))+sizeof(BITMAPINFOHEADER)+perline*((_sizey-1)-y));

		for(int x=0;x<_sizex;x++){
       COLORREF _clr=((*_colors)&0xfffffful);
       int _b=GetRValue(_clr);
       int _g=GetGValue(_clr);
       int _r=GetBValue(_clr);
       if ((_r>=_r1)&&(_r<=_r2)&&
           (_g>=_g1)&&(_g<=_g2)&&
           (_b>=_b1)&&(_b<=_b2)){
            _x+=x;
            _y+=y;
            count++;
           }
	  ((BYTE SC_huge)_colors)+=3;
		}
     };
   } // 24 bit
  }
if(count){
    vm->point.x=_x/count;
    vm->point.y=_y/count;
    return TRUE;
}
return FALSE;
}

int _Error(char*){
return 0;
};

void PauseAll(BOOL pause){
  BOOL static audiopaused;
//  DWORD static time=0;
if (pause){
 if (!waspaused){
 waspaused=1;
 paused=1;
 pausestarttime=timeGetTime();
 if (audio){
 audiopaused=audio->paused;
 if (!audiopaused) audio->Pause();
  } }
 }else{
 if (audio){
	if (!audiopaused)	audio->Resume();
  }
	DWORD t=timeGetTime();
	timedelta+=(t-pausestarttime);
	paused=0;
	waspaused=0;
  }
 UpdatePlaying();
};
#define setplay {*playing=TRUE;return;}

void UpdatePlaying(){
if (!playing)return ;
if (playvideos->count || playframes->count || audio){
 if (audio && !(audio->paused))setplay
 for(C_TYPE i=0;i<playvideos->count;i++){
	TVideo*v=(TVideo*)playvideos->At(i);
	if (v && !v->pause) setplay
 }
 for(i=0;i<playframes->count;i++){
	TFrame*f=(TFrame*)playframes->At(i);
	if (!f->paused) setplay
 }
}
 *playing=FALSE;
};

TCollection  * videos=NULL;
TCollection  * frames=NULL;
TCollection * playvideos=NULL;
TCollection  * playframes=NULL;
DWORD pausestarttime=0;
BOOL paused=FALSE;
BOOL _playing;
BOOL * playing=&_playing;
HPALETTE  palette=0;
TAudio*  audio=NULL;
HWND     capwnd = NULL;
// interface
