/*
Copyright (c) 1995,1996  Virtual World
Module Name:
     tools2d.cpp
Author:
     Alexander Shelemekhov
*/
#include "tools2d.h"
#include "mem.h"
#include "../../BC/bc_include_gwin2d3d/GWINTYPE.H"
#define  CVT_CORE_TO_INFO


typedef struct tagOldLOGFONT {     /* lf */
    INT16 lfHeight;
    INT16 lfWidth;
    INT16 lfEscapement;
    INT16 lfOrientation;
    INT16 lfWeight;
    BYTE  lfItalic;
    BYTE  lfUnderline;
    BYTE  lfStrikeOut;
    BYTE  lfCharSet;
    BYTE  lfOutPrecision;
    BYTE  lfClipPrecision;
    BYTE  lfQuality;
    BYTE  lfPitchAndFamily;
    BYTE  lfFaceName[32];
} OldLOGFONT;

#define CONVERT_FONTS(lf1,lf2)\
    lf1.lfHeight          =    lf2.lfHeight;\
    lf1.lfWidth           =    lf2.lfWidth;\
    lf1.lfEscapement      =    lf2.lfEscapement;\
    lf1.lfOrientation     =    lf2.lfOrientation;\
    lf1.lfWeight          =    lf2.lfWeight;\
    lf1.lfItalic          =    lf2.lfItalic;\
    lf1.lfUnderline       =    lf2.lfUnderline;\
    lf1.lfStrikeOut       =    lf2.lfStrikeOut;\
    lf1.lfCharSet         =    lf2.lfCharSet;\
    lf1.lfOutPrecision    =    lf2.lfOutPrecision;\
    lf1.lfClipPrecision   =    lf2.lfClipPrecision;\
    lf1.lfQuality         =    lf2.lfQuality;\
    lf1.lfPitchAndFamily  =    lf2.lfPitchAndFamily;\
    lstrcpy(lf1.lfFaceName,lf2.lfFaceName);

BYTE extern done_mode;

BOOL IsPensEqual(PPen2d p1, PPen2d p2)
{
    return ((p1->color==p2->color)&&(p1->style==p2->style)&&
         (p1->width==p2->width)&&(p1->rop2==p2->rop2));
}

BOOL IsBrushsEqual(PBrush2d p1,PBrush2d p2)
{return ((p1->color==p2->color)&&(p1->style==p2->style)&&
         (p1->hatch==p2->hatch)&&(p1->dib==p2->dib)&&
         (p1->rop2==p2->rop2));
};

BOOL __IsDibsEqual(PDib2d p1,PDib2d p2){
    if ((p1->W!=p2->W)||(p1->H!=p2->H)||
        (p1->NumClrs!=p2->NumClrs))return 0;
    if ((p1->Info->bmiHeader.biBitCount!=p2->Info->bmiHeader.biBitCount)||
        (p1->Info->bmiHeader.biSize!=p2->Info->bmiHeader.biSize)||
        (p1->Info->bmiHeader.biSizeImage!=p2->Info->bmiHeader.biSizeImage))return 0;
    if (p1->Info->bmiHeader.biBitCount<=256){
        int n=p1->Info->bmiHeader.biClrUsed;
        for(int i=0;i<n;i++){
            if ((p1->Info->bmiColors[i].rgbBlue!=p2->Info->bmiColors[i].rgbBlue)||
                (p1->Info->bmiColors[i].rgbGreen!=p2->Info->bmiColors[i].rgbGreen)||
                (p1->Info->bmiColors[i].rgbRed!=p2->Info->bmiColors[i].rgbRed))return 0;
        } }
#ifdef WIN32
    int __size=p1->Info->bmiHeader.biSizeImage/4;
    if (p1->Info->bmiHeader.biSizeImage%4)__size;
    DWORD * _b1=(DWORD*)p1->Bits;
    DWORD * _b2=(DWORD*)p2->Bits;
    for(int i=0;i<__size;i++){if(_b1[i]!=_b2[i])return 0; }
#else
    if (p1->Info->bmiHeader.biSizeImage<65530u){
        BYTE * _b1=(BYTE*)(p1->Bits);
        BYTE * _b2=(BYTE*)(p2->Bits);
        WORD i,n=(WORD)p1->Info->bmiHeader.biSizeImage;
        for(i=0;i<n;i++)if (*(_b1++)!=(*(_b2++)))return 0;
    }else{
        BYTE _huge * _b1=(BYTE _huge *)p1->Bits;
        BYTE _huge * _b2=(BYTE _huge*)p2->Bits;
        long i,n=p1->Info->bmiHeader.biSizeImage;
        for(i=0;i<n;i++)if (*(_b1++)!=(*(_b2++)))return 0;
    }
#endif
    return 1;
};

BOOL IsDibsEqual(PDib2d p1,PDib2d p2)
{WORD t1=p1->WhoIsIt(),t2=p2->WhoIsIt();
    if (t1!=t2)return 0;
    if (t1==110){
        if (p1->GetMainObject()==p2->GetMainObject())return 1;else return 0;
    }
    return __IsDibsEqual(p1,p2);
};

BOOL IsDDibsEqual(PDoubleDib2d p1,PDoubleDib2d p2)
{WORD t1=p1->WhoIsIt(),t2=p2->WhoIsIt();
    if (t1!=t2)return 0;
    if (t1==111){
        if (p1->GetMainObject()==p2->GetMainObject())return 1;else return 0;
    }
    return __IsDibsEqual((PDib2d)p1,(PDib2d)p2)&&
            __IsDibsEqual(p1->andbitmap,p2->andbitmap);
};

BOOL IsStringsEqual(PString2d p1,PString2d p2)
{
    return lstrcmp(p1->string,p2->string)==0;
};

BOOL IsFontsEqual(PFont2d p1,PFont2d p2)
{
    return ((p1->font2d.lfHeight==p2->font2d.lfHeight)&&(p1->font2d.lfWidth==p2->font2d.lfWidth)&&
            (p1->font2d.lfWeight==p2->font2d.lfWeight)&&(p1->font2d.lfItalic==p1->font2d.lfItalic)&&
            (p1->font2d.lfUnderline==p2->font2d.lfUnderline)&&(p1->font2d.lfStrikeOut==p2->font2d.lfStrikeOut)&&
            (p1->font2d.lfCharSet==p2->font2d.lfCharSet)&&
            (p1->font2d.lfPitchAndFamily==p2->font2d.lfPitchAndFamily)&&
            (lstrcmp(p1->font2d.lfFaceName,p2->font2d.lfFaceName)==0)&&
            (p1->font2d.lfFontSize==p2->font2d.lfFontSize)&&
            (p1->font2d.lfStyle==p2->font2d.lfStyle));
};

BOOL IsTextsEqual(PText2d p1,PText2d p2)
{C_TYPE i;
    if (p1->text->count!=p2->text->count)return 0;
    PTextRecord t1,t2;
    for (i=0;i<p1->text->count;i++)
    {t1=(PTextRecord)p1->text->At(i);t2=(PTextRecord)p2->text->At(i);
        if ((t1->ltFgColor!=t2->ltFgColor)||(t1->ltBgColor!=t2->ltBgColor)||
            (t1->string!=t2->string)||(t1->font!=t2->font))return 0;
    }
    return 1;
};

//*-------------- ƒл¤ ReadDib -----------------------
long NColors(WORD bitCount)
{
    if (bitCount == 1 || bitCount == 4 || bitCount == 8)
        return 1 << bitCount;

    else if (bitCount == 24)
        return 0;

    return -1;
}

long ScanBytes(int pixWidth, int bitsPixel)
{
    return (((long)pixWidth*bitsPixel+31) / 32) * 4;
}

//*--------------------------------------------------
TPen2d::TPen2d():TTools2d(){style=0;width=0;rop2=0;};
TPen2d::TPen2d(COLORREF acolor,INT16 aStyle,INT16 aWidth,
               INT16 Rop2,HOBJ2D handle):TTools2d(handle){
    color=acolor;style=aStyle;width=aWidth;
    rop2=Rop2;
};

TPen2d::TPen2d(PStream ps,LONG _pos):TTools2d(ps,_pos)
{
    ps->Read(&color,4);
    style=ps->ReadWord();
    width=ps->ReadWord();
    rop2=ps->ReadWord();
};
void TPen2d::Store(PStream ps)
{TTools2d::Store(ps);
    ps->Write(&color,4);
    ps->WriteWord(style);
    ps->WriteWord(width);
    ps->WriteWord(rop2);
};
//*-------------------------------------------
TFont2d::TFont2d():TTools2d()
{
    font=(LOGFONT*)(&(font2d.lfHeight));
    memset(&font,0,sizeof(font));
};

TFont2d::TFont2d(LOGFONT2D * lf,int handle) :
    TTools2d(handle)
{
    font2d = *lf;
    font = (LOGFONT*)(&(font2d.lfHeight));
    tmValid=FALSE;

#ifdef WIN32
    /*
        if(lf->lfCharSet==1251){
        char lfFaceName[256];
        lstrcpy(lfFaceName,font->lfFaceName);
        AnsiUpper(lfFaceName);
        char *cyr=strstr(lfFaceName," CYR");
        if(!cyr){
          font->lfCharSet=0;
          lstrcat(font->lfFaceName," Cyr");
        }
    }; */
#endif
};

void TFont2d::GetTM(HDC dc)
{
    TEXTMETRIC tm;
    tmValid=GetTextMetrics(dc,&tm);
    tmHeight  =tm.tmHeight;
    tmAscent  =tm.tmAscent;
    tmDescent =tm.tmDescent;
}

TFont2d::TFont2d(PStream ps,LONG _pos):TTools2d(ps,_pos)
{
    font = (LOGFONT*)(&(font2d.lfHeight));
    memset(&font2d,0,sizeof(font2d));

    font2d.lfSize = sizeof(font2d);

    OldLOGFONT lf;
    ps->Read(&lf,sizeof(OldLOGFONT));

    CONVERT_FONTS((*font),lf);

    if(ps->fileversion >= 0x0203)
    {
        font2d.lfFontSize=ps->ReadLong();
        font2d.lfStyle= ps->ReadLong();
    }
}

void TFont2d::Store(PStream ps){
    TTools2d::Store(ps);
    OldLOGFONT lf;
    CONVERT_FONTS(lf,(*font));
    ps->Write(&lf,sizeof(OldLOGFONT));
    ps->WriteLong(font2d.lfFontSize);
    ps->WriteLong(font2d.lfStyle);
};
//*-----------------------------------------------------*
TString2d::TString2d():TTools2d()
{
    string=NULL;
};
TString2d::TString2d(char* str,int handle):TTools2d(handle)
{
    if ((str!=NULL)&&(lstrlen(str)!=0)){
        string=NewStr(str);
    }else string=NULL;
    UpdateFlags();
};
TString2d::TString2d(PStream ps,LONG _pos):TTools2d(ps,_pos)
{
    string=ps->ReadStr();
    UpdateFlags();
};

TString2d::~TString2d()
{
    if (string) {delete string;string=NULL;}
};
void TString2d::Store(PStream ps){
    TTools2d::Store(ps);
    ps->WriteStr(string);
};
char * TString2d::GetString()
{
    return string;
};
int TString2d::StrLen()
{
    if (string)return lstrlen(string);
    return 0;
};
void TString2d::UpdateFlags()
{char * p;p=string;
    flags=0;
    if (string)
    {while (*p!=0){if (*p=='\n')flags|=1;p++;}  }
};
//*-----------------------------------------------------*
TText2d::TText2d():TTools2d()
{
    text=new TLOGTextCollection(5,5);
};

TText2d::TText2d(PStream ps,LONG _pos):TTools2d(ps,_pos)
{
    text = (PLOGTextCollection) ps->Get(1011);
};

TText2d::~TText2d()
{
    if (text)
        DeleteCollection(text);
};

void TText2d::Store(PStream ps)
{
    TTools2d::Store(ps);
    ps->Put(text);
};
//*-----------------------------------------------------*
/*class TBrush2d:public TTools2d{
public:
 COLORREF color;
 PDib2d dib;
 int rop2;*/
TBrush2d::TBrush2d():TTools2d()
{
    color=0;dib=0;rop2=0;style=0;hatch=0;
    backcolor=RGB(255,255,255);
    gradient_colors_count=2;
    gradient_colors=new COLORREF[gradient_colors_count];
    gradient_colors[0]=backcolor;
    gradient_colors[1]=color;
    gradient_x0=0;gradient_y0=0;
    gradient_x1=0;gradient_y1=1;
};

TBrush2d::TBrush2d(int Style,COLORREF rgb,PDib2d aDib,int ahatch,
                   int Rop2,int handle):TTools2d(handle)
{
    backcolor=RGB(255,255,255);
    style=Style;color=rgb;dib=aDib;rop2=Rop2;hatch=ahatch;
    if (dib)dib->IncRef(this);
    gradient_colors_count=2;
    gradient_colors=new COLORREF[gradient_colors_count];
    gradient_colors[0]=backcolor;
    gradient_colors[1]=color;
    gradient_x0=0;gradient_y0=0;
    gradient_x1=0;gradient_y1=1;
};

TBrush2d::TBrush2d(PStream ps,LONG _pos):TTools2d(ps,_pos)
{
    backcolor=RGB(255,255,255);
    ps->Read(&color,4);
    style=ps->ReadWord();
    hatch=ps->ReadWord();
    rop2=ps->ReadWord();
    dib=(PDib2d)(DWORD)(INT16)ps->ReadWord();

    gradient_colors_count=2;
    gradient_colors=new COLORREF[gradient_colors_count];
    gradient_colors[0]=backcolor;
    gradient_colors[1]=color;
    gradient_x0=0;gradient_y0=0;
    gradient_x1=0;gradient_y1=1;
};

void TBrush2d::Store(PStream ps)
{
    TTools2d::Store(ps);
    ps->Write(&color,4);ps->WriteWord(style);ps->WriteWord(hatch);
    ps->WriteWord(rop2);
    if (dib) ps->WriteWord(dib->_HANDLE);
    else ps->WriteWord(0);
};
/*
 HGLOBAL dib;
 HBITMAP bitmap;
 */

TDib2d::TDib2d(HGLOBAL h,HOBJ2D handle):TTools2d(handle)
{
    FreeDib=TRUE;
    W=0;H=0;Bits=0;Info=0;IsCore=0;Mode=0;
    bitmap=0;dib=h;perline=0;
    InfoFromHandle();
    imagesize.x=0;imagesize.y=0;filename=NULL;
};

TDib2d::TDib2d(PStream ps,LONG _pos) : TTools2d(ps,_pos)
{
    FreeDib=TRUE;
    filename=NULL;
    _TDib2d(ps);
};

void TDib2d::_TDib2d(PStream ps)
{
    imagesize.x=0;
    imagesize.y=0;
    dib=0;
    Info=0;
    bitmap=0;
    BITMAPFILEHEADER bmf;
    ps->Read(&bmf, sizeof(bmf));

    if(bmf.bfType == 'BM')
        Read(ps, bmf.bfOffBits ? bmf.bfOffBits - sizeof(BITMAPFILEHEADER) : 0);
}

TDib2d::TDib2d(int ,PStream ps):TTools2d()
{
    FreeDib=TRUE;
    filename=NULL;
    _TDib2d(ps);
}

TDib2d::~TDib2d()
{
    FreeBitmap();
    if(dib && done_mode)
    {
        GlobalUnlock(dib);
        if(FreeDib)
            ::GlobalFree(dib);
        dib=0;
    }
    if (filename)delete filename;
};

BOOL TDib2d::SetFreeDib(BOOL l)
{
    BOOL o=FreeDib; FreeDib=l;return o;
};

int TDib2d::Convert(){ return 0; }

int TDib2d::Read(PStream ps, long offBits)
{
    DWORD headerSize;
    ps->Read(&headerSize, sizeof(headerSize));
    if (ps->status ||headerSize != sizeof(BITMAPCOREHEADER) && headerSize != sizeof(BITMAPINFOHEADER))
    {
        _Error(EM_notwindowsbitmap);
        //TRACEX(OwlGDI, 0, "Not a Windows 3.x or PM 1.x bitmap file");
        return FALSE;
    }

    // Prepare to accept a header that is either Core(PM) or Info(Win) type
    // Note thet the biSize and the bcSize fields are the same(the only ones).
    //
    union
    {
        BITMAPINFOHEADER infoHeader;
        BITMAPCOREHEADER coreHeader;
    };

#if defined(CVT_CORE_TO_INFO)
    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
#else

    infoHeader.biSize = headerSize;
#endif

    ps->Read(&infoHeader.biWidth, (int)headerSize-sizeof(DWORD));
    if (ps->status)
    {
        //TRACEX(OwlGDI, 0, "Invalid DIB Header");
        return FALSE;
    }

    // Type dependent fields kept independently
    //
    WORD  bitCount;
    int   colorAlloc;
    int   colorRead;
    DWORD bitsAlloc;

    // If this is a PM 1.X Dib, expand header and fill out reamining fields
    // for win3 dib info
    // Calculate size of color table in memory using RGBQUADs if converting.
    // For PM Dibs, this is NOT the size on disk.
    //
    if (headerSize == sizeof(BITMAPCOREHEADER))
    {
        // check number of planes. Windows 3.x supports only 1 plane DIBs

        bitCount = coreHeader.bcBitCount;
        NumClrs = NColors(bitCount);
        colorRead = (int)NumClrs * sizeof(RGBTRIPLE); // Color tables on disk
        W = coreHeader.bcWidth;
        H = coreHeader.bcHeight;
#if defined(CVT_CORE_TO_INFO)
        IsCore = FALSE;
        // Note reverse field order for copying in place
        infoHeader.biBitCount = coreHeader.bcBitCount;
        infoHeader.biPlanes = 1;
        infoHeader.biWidth = W;
        infoHeader.biHeight = H;
        infoHeader.biCompression = BI_RGB;
        infoHeader.biSizeImage = 0; // calculate this below
        infoHeader.biXPelsPerMeter = 0;
        infoHeader.biYPelsPerMeter = 0;
        infoHeader.biClrUsed = NumClrs;
        infoHeader.biClrImportant = 0;
        colorAlloc = (int)(NumClrs * sizeof(RGBQUAD)); // size of color tables in mem
#else
        IsCore = TRUE;
        colorAlloc = colorRead;
#endif
        bitsAlloc = 0;      // Calculate below
    }
    else
    {
        IsCore = FALSE;
        bitCount = infoHeader.biBitCount;

        /// ||||||||||||||

        if (!infoHeader.biClrUsed)
        {
            infoHeader.biClrUsed = NColors(bitCount);
            NumClrs = (int)infoHeader.biClrUsed;
        }
        else
            NumClrs =NColors(bitCount);

        colorRead = ( infoHeader.biClrUsed )* sizeof(RGBQUAD);
        /// ||||||||||||||

        colorAlloc = (int)NumClrs * sizeof(RGBQUAD); // size of color tables
        if(bitCount>8)
            colorAlloc=0;
        //	 colorRead = colorAlloc;

        W = (int)infoHeader.biWidth;
        H = (int)infoHeader.biHeight;
        bitsAlloc = infoHeader.biSizeImage;
    }

    // Some applications do not fill in the SizeImage field in the header.
    // (Actually the truth is more likely that some drivers do not fill the
    // field in and the apps do not compensate for these buggy drivers.)
    // Or it is a PM 1.X Dib.
    // Therefore, if compression was not used, we will(re)compute the size,
    // but if compression is used, we have no choice but to trust the size.
    //

    if (IsCore || infoHeader.biCompression == BI_RGB)
    {
        bitsAlloc = ScanBytes(W, bitCount) * H;
        if (!IsCore)
            infoHeader.biSizeImage = bitsAlloc;
    }

    dib = ::GlobalAlloc(GMEM_MOVEABLE, infoHeader.biSize + colorAlloc + bitsAlloc);
    if (!dib)return FALSE;

    Info = (LPBITMAPINFO)::GlobalLock(dib);
    if (!Info)return FALSE;
    Info->bmiHeader = infoHeader;

    // Read color table.  Expand to RGBQUADs if it is a PM Dib & we are converting
    //
    if (colorAlloc) 
    {
        ps->Read((char far*)Info+(int)infoHeader.biSize, colorRead);
        if (ps->status)	
            return FALSE;

#if defined(CVT_CORE_TO_INFO)
        if (IsCore)
        {
            for (int i = ((int)NumClrs)-1; i >= 0; i--)
            {
                Info->bmiColors[i].rgbRed = ((RGBTRIPLE*)Info->bmiColors)[i].rgbtRed;
                Info->bmiColors[i].rgbGreen = ((RGBTRIPLE*)Info->bmiColors)[i].rgbtGreen;
                Info->bmiColors[i].rgbBlue = ((RGBTRIPLE*)Info->bmiColors)[i].rgbtBlue;
                Info->bmiColors[i].rgbReserved = 0;
            }
        }
#endif
    }
    Mode = DIB_RGB_COLORS;

    // Locate & Read Bits, skipping Pad if any.  Ignore OffBits if it is zero.
    // Ignore OffBits if less than the current position(its probably bad)
    //
    Bits = (char far*)Info+((int)infoHeader.biSize + colorAlloc);
    if (offBits && offBits - (long)(headerSize+colorRead) > 0)
        //	 ps->Seek(offBits- (headerSize+colorRead), TFile::cur);
        ps->Seek(ps->GetPos()+offBits- (headerSize+colorRead));
#ifdef WIN32
    ps->Read(Bits, bitsAlloc);
#else
    ps->Read((void __huge*)Bits,bitsAlloc);
#endif
    if (ps->status)	return FALSE;
    return TRUE;
}

void TDib2d::_Store(PStream ps)
{
    {
        //	  int numclrs=Info->bmiHeader.biClrUsed;
        int bitCount = Info->bmiHeader.biBitCount;
        Info->bmiHeader.biClrUsed=NColors(bitCount);
    }
    long bitsAlloc = ScanBytes(W, Info->bmiHeader.biBitCount) * H;

    long size =Info->bmiHeader.biSize+bitsAlloc
               +Info->bmiHeader.biClrUsed*4;

    //Info->bmiHeader.biSizeImage
    //long size =::GlobalSize(dib); раньше было это
    // write file header

    BITMAPFILEHEADER bmf;
    bmf.bfType = 'BM';
    bmf.bfSize = sizeof(bmf) + size;
    bmf.bfReserved1 = 0;
    bmf.bfReserved2 = 0;
    bmf.bfOffBits = sizeof(bmf) +(char far*)Bits - (char far*)Info;
    ps->Write(&bmf, sizeof(bmf));
#ifdef WIN32
    ps->Write(Info,size);
#else
    ps->Write((void __huge*)Info, (long)size);
#endif

    //	ps->Write((void __huge*)Info, sizeof);
    //  	ps->Write((void __huge*)Info, (long)(Info->bmiHeader.biSizeImage));

};

void TDib2d::Store(PStream ps)
{
    TTools2d::Store(ps);
    _Store(ps);
}

int TDib2d::InfoFromHandle()
{
    if (!Info)
        Info = (LPBITMAPINFO)::GlobalLock(dib);
    if (Info==NULL){dib=0;return 0;}
    if (Info->bmiHeader.biSize == sizeof(BITMAPCOREHEADER))
        IsCore = TRUE;
    else
        if (Info->bmiHeader.biSize == sizeof(BITMAPINFOHEADER))
            IsCore = FALSE;
        else
        {
            ::GlobalUnlock(dib);
            dib=NULL;
            return 0;
        }
    int colorAlloc;
    if (IsCore)
    {
        NumClrs = NColors(((LPBITMAPCOREINFO)Info)->bmciHeader.bcBitCount);
        colorAlloc = (int)NumClrs * sizeof(RGBTRIPLE);
        W = ((LPBITMAPCOREINFO)Info)->bmciHeader.bcWidth;
        H = ((LPBITMAPCOREINFO)Info)->bmciHeader.bcHeight;
        perline=ScanBytes(W,((LPBITMAPCOREINFO)Info)->bmciHeader.bcBitCount);
    }
    else
    {
        NumClrs = NColors(Info->bmiHeader.biBitCount);
        colorAlloc = (int)NumClrs * sizeof(RGBQUAD);
        W = (int)Info->bmiHeader.biWidth;
        H = (int)Info->bmiHeader.biHeight;
        perline=ScanBytes(W,Info->bmiHeader.biBitCount);
    }
    //if (Info.biClrUsed==0){ Info.biClrUsed=NumClrs; } // added in 12.07.96
    Bits = (char far*)Info + ((int)Info->bmiHeader.biSize + colorAlloc);
    Mode = DIB_RGB_COLORS;
    return 1;
}

BOOL TDib2d::SetPixel(int x,int y,COLORREF rgb)
{
    if(x>=0 && x<W && y>=0 && y<H)
    {
        int _perline=ScanBytes(W,Info->bmiHeader.biBitCount);
#ifdef WIN32
        BYTE * curptr=((BYTE*)Bits)+((long)((H-1)-y))*((long)_perline);
#else
        BYTE _huge * curptr=ps.Buffer+((long)((H-1)-y))*((long)_perline);
#endif
        switch(Info->bmiHeader.biBitCount)
        {
            case 24:
                curptr+=((long)(x)*3L);
                curptr[2]=GetRValue(rgb);
                curptr[1]=GetGValue(rgb);
                curptr[0]=GetBValue(rgb);
                if(this->bitmap)::DeleteObject(this->bitmap);
                this->bitmap=NULL;
                return true;
            case 8:
            {
                curptr+=((long)(x));
                *((COLORREF*)(((char*)Info)+sizeof(BITMAPINFOHEADER))+*curptr)=rgb;
                if(this->bitmap)::DeleteObject(this->bitmap);
                this->bitmap=NULL;
                return true;
            }
        }
    }
    return false;
};

COLORREF TDib2d::GetPixel(int x,int y,BYTE *index)
{
    if(x>=0 && x<W && y>=0 && y<H)
    {
        int _perline=ScanBytes(W,Info->bmiHeader.biBitCount);
#ifdef WIN32
        BYTE * curptr=((BYTE*)Bits)+((long)((H-1)-y))*((long)_perline);
#else
        BYTE _huge * curptr=ps.Buffer+((long)((H-1)-y))*((long)_perline);
#endif
        switch(Info->bmiHeader.biBitCount)
        {
            case 24:curptr+=((long)(x)*3L);
                return  RGB(curptr[2],curptr[1],curptr[0]);
            case 8:
            {
                curptr+=((long)(x));
                if(index)*index=*curptr;
                return *((COLORREF*)(((char*)Info)+sizeof(BITMAPINFOHEADER))+*curptr);
            }
        }
    }
    return 0;
};

void TDib2d::FreeBitmap()
{
    if (bitmap)
    {
        ::DeleteObject(bitmap);
        bitmap=0;
    }
};

int TDib2d::SetDib(HGLOBAL hdib)
{
    if (hdib==dib){FreeBitmap();return 1;}
    HGLOBAL oldDib=dib;
    GlobalUnlock(oldDib);
    FreeBitmap();
    W=0;H=0;Bits=0;Info=0;IsCore=0;Mode=0;bitmap=0;dib=hdib;
    int rz=InfoFromHandle();
    if (rz)
    {
        if (oldDib)
        {::GlobalFree(oldDib);}
    }
    else
    {
        W=0;H=0;Bits=0;Info=0;IsCore=0;Mode=0;bitmap=0;dib=oldDib;
        InfoFromHandle();
    }
    //UpdateBitmap();
    return rz;
};

TDoubleDib2d::TDoubleDib2d(HGLOBAL XORm,HGLOBAL ANDm,HOBJ2D h):TDib2d(XORm,h)
{
    andbitmap=NULL;
    if(dib)
    {andbitmap=new TDib2d(ANDm);
        andbitmap->_HANDLE=0;
        if (andbitmap->dib)
        {if ((andbitmap->H!=H)||(andbitmap->W!=W))
            {delete andbitmap;andbitmap=NULL; }
        }else
        {delete andbitmap;andbitmap=NULL;
        }
    }
}

TDoubleDib2d::TDoubleDib2d(PStream ps,LONG _pos) :
    TDib2d(ps,_pos)
{
    andbitmap = new TDib2d(1,ps);
}

TDoubleDib2d::TDoubleDib2d(int a,PStream ps):TDib2d(a,ps)
{
    andbitmap=new TDib2d((int)1,ps);
}

TDoubleDib2d::~TDoubleDib2d()
{
    if  (andbitmap)
        delete andbitmap;
    andbitmap=NULL;
};

void TDoubleDib2d::_Store(PStream ps)
{TDib2d::_Store(ps);andbitmap->_Store(ps);
}
BOOL TDoubleDib2d::SetFreeDib(BOOL l)
{if (andbitmap)andbitmap->SetFreeDib(l);
    BOOL o=TDib2d::SetFreeDib(l);
    return o;
};

void TDoubleDib2d::Store(PStream ps)
{ TDib2d::Store(ps);
    andbitmap->_Store(ps);
};

int TDoubleDib2d::SetDibs(HGLOBAL xorm,HGLOBAL andm)
{
    return SetDib(xorm)&&andbitmap->SetDib(andm);
};

//*-----------------------------------------------------*
HOBJ2D  TToolsCollection::GetHandle(pointer p)
{
    return ((PTools2d) p)->_HANDLE;
};
void TToolsCollection::SetHandle(pointer p,HOBJ2D h)
{
    ((PTools2d) p)->_HANDLE=h;
};
BOOL  TToolsCollection::MayDelete(pointer p){
    return ((PTools2d) p)->IsZeroRef();
};
BOOL  TToolsCollection::IsNeedToSave(pointer p)
{ return ((PTools2d)p)->flag & rfmustsaved;
};
void TLOGTextCollection::FreeItem(pointer _p){
    delete (PTextRecord)_p;
    ;};

pointer TLOGTextCollection::GetItem(PStream ps)
{
    LOGTEXT lt;
    ps->Read(&lt, sizeof(LOGTEXT));
    return new TextRecord(&lt);
};

void    TLOGTextCollection::PutItem(PStream ps,pointer _p){
    LOGTEXT lt;
    ((PTextRecord)_p)->GetLogText(&lt);
    ps->Write(&lt,sizeof(LOGTEXT));
};

TextRecord::TextRecord()
{text2d=0;
    string=NULL;font=NULL;
    ltFgColor=RGB(255,255,255);ltBgColor=0;
};
TextRecord::~TextRecord()
{ if(HIWORD(font))		font->DecRef(text2d);
    if(HIWORD(string))	string->DecRef(text2d);
};

TextRecord::TextRecord(LOGTEXT* lt)
{
    text2d=0;
    ltFgColor=lt->ltFgColor;
    ltBgColor=lt->ltBgColor;
    font=(PFont2d)(DWORD)(INT16)(lt->ltFontIndex);
    string=(PString2d)(DWORD)(INT16)(lt->ltStringIndex);
};

void TextRecord::GetLogText(LOGTEXT* lt)
{
    lt->ltFgColor=ltFgColor;
    lt->ltBgColor=ltBgColor;
    lt->ltFontIndex=font->_HANDLE;
    lt->ltStringIndex=string->_HANDLE;
};

//*---------------------------TToolRef-------------------------------------*

TToolRef::TToolRef() : TTools2d()
{
    tool=NULL;
    name=NULL;
}

TToolRef::TToolRef(PStream ps,LONG _pos) : TTools2d(ps,_pos)
{
    tool=NULL;
    name=NULL;
}

TToolRef::~TToolRef()
{
    if(name)
    {
        delete name;
        name=0;
    }
}

TRefToDib2d::TRefToDib2d():
    TToolRef()
{}

TRefToDib2d::TRefToDib2d(char* s,pointer ps):TToolRef()
{
    name=NewStr(s);
    Awaken((_SPACE*)ps);
};

TRefToDib2d::~TRefToDib2d()
{
    if (HIWORD(tool))
    {
        if(tool)
            tool->DecRef(this);
        ChkRDibToUnload();
    }
};

TRefToDib2d::TRefToDib2d(PStream ps,LONG _pos):TToolRef(ps,_pos)
{
    name=ps->ReadStr();
}

void TRefToDib2d::Store(PStream ps)
{
    TTools2d::Store(ps);
    ps->WriteStr(name);
}


TRefToDoubleDib2d::TRefToDoubleDib2d(){}

TRefToDoubleDib2d::TRefToDoubleDib2d(char* s,pointer ps)
{
    name=NewStr(s);
    Awaken((_SPACE*)ps);
}

TRefToDoubleDib2d::TRefToDoubleDib2d(PStream ps,LONG _pos) :
    TToolRef(ps,_pos)
{
    name=ps->ReadStr();
}

TRefToDoubleDib2d::~TRefToDoubleDib2d()
{
    if (HIWORD(tool))
    {
        if(tool)
            tool->DecRef(this);
        ChkRDoubleDibToUnload();
    }
}

void TRefToDoubleDib2d::Store(PStream ps)
{TTools2d::Store(ps);
    ps->WriteStr(name);
};

