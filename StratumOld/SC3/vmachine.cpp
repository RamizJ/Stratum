#define _WIN32_WINNT 0x0501

#ifdef WIN32
#undef _WINREG_
#endif

#include "scbuild.h"
#include <stdlib.h>
#include "_vmach.h"
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <mem.h>
#include <dos.h>
#include <shellapi.h>
#include "../BC/bc_include_stratum/STRATUM.H"
#include <float.h>
#include "errors.h"
#include "winman.h"
#include "matrix.h"
#include "media.h"
#include "stream_m.h"
#include "grtools.h"
#include "autosntz.h"
#include "dialogs.h"
#include "_class.h"
#include "encryption.h"
#include "sc3_graphics/_SYSTEM.H"

#ifdef USEAUDIERE
#include "audio\scaudio.h"
#endif

#include "TextAnalyser\TextAnalyser.h"
#include "NUI.h"
#include "sc3.rh"

#ifdef USE_OGRE
#include "OgreWrapperCommands.h"
#endif

#ifdef DBE
#include "dbengine.h"
#endif

#include "equ.h"
#include "dllman.h"
#include "strings.rh"

#include "userkey.h"
#include "dynodata.h"

//#include <time.h>

//#define FAST_EXECUTE без проверок в виртуальной машине

HWND GetControl(CONTROLS *type);

int MatEditor(int);

LPSTR Lstrcpyn(LPSTR s1, LPCSTR s2, int n)
{
    if(s1)
    {
        if(s2&&n)
        {
            if(n>STRING_SIZE)
                n=STRING_SIZE;
            return lstrcpyn(s1, s2, n);
        }
        s1=0;
    }
    return 0;
}

double *spf; // Указатель плавающего стека

static union
{
    void** variables;
    double** variablesf;
    long** variablesl;
    char** variabless;
};

BYTE invfunction=0;
VMPROC *commands;
PObject pobject=0; // текущий объект
INT16 string_buffer_index;
INT16 string_buffer_count;
char *tmp_string_buffer;
double *stuck_f; //стек для плавающих
WORD maxsttuck;
BYTE VM_ERROR=0;

WORD *v_code=0;
WORD *codepoint=0;
WORD vm_nullcode = 0; // как заглушка
INT16 operation;    // Текущая операция

void inline UPDATE_VARS()
{
    if(pobject)
    {
        ((void *)variables)=pobject->vars;
    }
}

///////////////////////////////////////////////////////////////////
////////***********************************************////////////
///////////////////////////////////////////////////////////////////

double inline EXP(double x)
{
    if(x>60)
        return MAX_DOUBLE;
    else
        return exp(x);
} ;

double inline LOG(double x)
{
    if(x>0)
        return log(x);
    else
        return MIN_DOUBLE;
}

bool infinityStringBuffer=false;

char* GetTmpString()
{
    if(string_buffer_index>=string_buffer_count)
    {
        string_buffer_index=0;
        if(infinityStringBuffer)
        {
            string_buffer_count+=string_buffer_count/2;
            int arraySize = STRING_SIZE*string_buffer_count;
            if(arraySize<0)//на случай если STRING_SIZE*string_buffer_count больше максимального для int
            {
                string_buffer_count=32;
                arraySize = STRING_SIZE*string_buffer_count;
            }
            char* temp = new char[arraySize];
            tmp_string_buffer=temp;
        }
    }
    char* address = tmp_string_buffer + string_buffer_index * STRING_SIZE;
    string_buffer_index++;
    return address;
};

void vm_setStringBufferMode()
{
    infinityStringBuffer=(POP_DOUBLE)>0;
}

/*
char * _EXPORT VM_GetTmpString(){
 return GetTmpString();
};*/
//--------------------------------------------- SPECIAL

void pop_float_ptr()
{
    double *v=(double *)(*((void ** )codepoint));
    double x=POP_DOUBLE;
    *v = x;
    codepoint+=2;
}

void push_float_ptr()
{
    double *v=(double *)(*((void ** )codepoint));
    double x=*v;
    PUSH_DOUBLE(x);
    codepoint+=2;
}

void push_ptr()
{
    char *pstr = variabless[*codepoint];
    PUSH_PTR(pstr);
    codepoint += 1;
}

void push_ptr_new()
{
    INT16 i = *codepoint;
    char *pstr = variabless[i];
    pstr += pobject->_class->vars[i].classType->_class->varsize;

    PUSH_PTR(pstr);
    codepoint+=1;
}

void push_string()
{
    char* pstr = variabless[*codepoint];
    PUSH_PTR(pstr);
    codepoint+=1;
}

void _push_string()
{
    char *pstr = variabless[*codepoint];
    PUSH_PTR(pstr + STRING_SIZE);
    codepoint += 1;
}

void _pop_string()
{
    char *pstr=variabless[*codepoint];
    Lstrcpyn(pstr+STRING_SIZE, POP_PTR, STRING_SIZE);
    codepoint+=1;
}

void _pop_string_old()
{
    char *pstr=variabless[*codepoint];
    Lstrcpyn(pstr, POP_PTR, STRING_SIZE);
    codepoint+=1;
}

void push_string_const()
{
    PUSH_PTR((char *)(codepoint+1));
    codepoint+=( *codepoint)+1;
}

void float_to_long()
{
    long i=POP_DOUBLE;
    PUSH_LONG(i);
}

void long_to_float()
{
    double i=POP_LONG;
    PUSH_DOUBLE(i);
}

void vm_gettime()
{
    struct time t;
    gettime(&t);

    *((double *)POP_PTR) = t.ti_hund;
    *((double *)POP_PTR) = t.ti_sec;
    *((double *)POP_PTR) = t.ti_min;
    *((double *)POP_PTR) = t.ti_hour;
} ;

void vm_getdate()
{
    struct date d;
    getdate(&d);
    *((double *)POP_PTR)=d.da_day;
    *((double *)POP_PTR)=d.da_mon;
    *((double *)POP_PTR)=d.da_year;
} ;

//--------------------------------------------- OPERATORS

void v_mod()
{
    *(spf+1)=fmod(*(spf+1), (*spf));
    spf++;
} ;

//-----------------------------------------------
void v_jmp()
{
    INT16 ncp= *((INT16 *)codepoint);
    codepoint=v_code+ncp;
}

void v_jz()
{
#ifdef LOGON
    //LogMessage("v_jz 1");
#endif
    double jm=POP_DOUBLE;
#ifdef LOGON
    //LogMessage("v_jz 2");
#endif
    INT16 ncp=*((INT16 *)codepoint);
#ifdef LOGON
    //LogMessage("v_jz 3");
#endif
    if(!jm)
    { codepoint=v_code+ncp;}
    else
        codepoint+=1;
#ifdef LOGON
    //LogMessage("v_jz 4");
#endif
}

void v_jnz()
{
    double jm=POP_DOUBLE;
    INT16 ncp=*((INT16 *)codepoint);
    if(jm)
    { codepoint=v_code+ncp;}
    else
        codepoint+=1;
}

void v_jz_handle()
{
    long jm=POP_LONG;
    INT16 ncp=*((INT16 *)codepoint);
    if(!jm)
    { codepoint=v_code+ncp;}
    else
        codepoint+=1;
}

void v_jnz_handle()
{
    long jm=POP_LONG;
    INT16 ncp=*((INT16 *)codepoint);
    if(jm)
    { codepoint=v_code+ncp;}
    else
        codepoint+=1;
}

// -------------------------------- LOGICAL FLOAT
void v_equal()
{
    *(spf+1)=((*(spf+1))==(*(spf)));
    spf++;
}

void v_notequal()
{
    *(spf+1)=((*(spf+1))!=(*(spf)));
    spf++;
}

void v_more()
{
    *(spf+1)=((*(spf+1))>(*(spf)));
    spf++;
}

void v_moreorequal()
{
    *(spf+1)=((*(spf+1))>=(*(spf)));
    spf++;
}

void v_low()
{
    *(spf+1)=((*(spf+1))<(*(spf)));
    spf++;
} ;

void v_loworequal()
{
    *(spf+1)=((*(spf+1))<=(*(spf)));
    spf++;
} ;

void v_equali()
{
    long p1=POP_LONG;
    long p2=POP_LONG;
    PUSH_DOUBLE(p1==p2);
}

void v_notequali()
{
    long p1=POP_LONG;
    long p2=POP_LONG;
    PUSH_DOUBLE(p1!=p2);
}

// -------------------------------- LOGICAL STRING

void s_equal()
{
    char *s2=POP_PTR;
    char *s1=POP_PTR;
    int i=(lstrcmp(s1, s2)==0);
    PUSH_DOUBLE(i);
}

void s_notequal()
{
    char *s2=POP_PTR;
    char *s1=POP_PTR;
    double i=lstrcmp(s1, s2);
    PUSH_DOUBLE(i!=0);
}

void s_more()
{
    char *s2=POP_PTR;
    char *s1=POP_PTR;
    double i=lstrcmp(s1, s2);
    PUSH_DOUBLE(i>0);
}

void s_moreorequal()
{
    char *s2=POP_PTR;
    char *s1=POP_PTR;
    double i=lstrcmp(s1, s2);
    PUSH_DOUBLE(i>=0);
}

void s_low()
{
    char *s2=POP_PTR;
    char *s1=POP_PTR;
    double i=lstrcmp(s1, s2);
    PUSH_DOUBLE(i<0);
} ;

void s_loworequal()
{
    char *s2=POP_PTR;
    char *s1=POP_PTR;
    double i=lstrcmp(s1, s2);
    PUSH_DOUBLE(i<=0);
} ;

void v_and_binary()
{
    *(spf+1)=((UINT32)(*(spf+1)))&((UINT32)(*(spf)));
    spf++;
}

void v_or_binary()
{
    UINT32 i1=*(spf+1);
    UINT32 i2=*(spf);
    i1|=i2;
    *(spf+1)=i1;
    spf++;
}

void v_shl()
{
    UINT32 p=POP_DOUBLE;
    BYTE b=POP_DOUBLE;
    p=p<<b;
    PUSH_DOUBLE(p);
} ;

void v_shr()
{
    UINT32 p=POP_DOUBLE;
    BYTE b=POP_DOUBLE;
    p=p>>b;
    PUSH_DOUBLE(p);
} ;

//--------------------------------------------- FUNCTION

void gettickcount(){PUSH_DOUBLE((double)GetTickCount());};

void v_sin() {*spf = sin(*spf);}

void v_asin(){*spf=asin(*spf);}

void v_cos(){*spf=cos(*spf);}

void v_acos(){*spf=acos(*spf);}

void v_tan()
{
    *spf = tan(*spf);
}

void v_atan(){*spf=atan(*spf);}

void v_exp(){*spf=EXP(*spf);}

void v_sqrt()
{
    if(*spf>0)
        *spf=sqrt(*spf);
    else
        *spf=0.0;
} ;

void v_sqr(){*spf=( *spf) *(*spf);}

void v_ed(){*spf=*spf>0.0 ? 1.0 : 0;}

void v_edi()
{
    LONG i=POP_LONG;
    PUSH_DOUBLE((i>0L) ? (1.0) : (0.0));
} ;

void v_delta(){*spf=*spf==0.0 ? 1.0 : 0.0;}

void v_ln(){*spf=LOG(*spf);}

void v_lg()
{
    double x=POP_DOUBLE;
    PUSH_DOUBLE(LOG(x)/LOG(10));
}

void v_log()
{
    double a=POP_DOUBLE, n=POP_DOUBLE;
    PUSH_DOUBLE(LOG(a)/LOG(n));
}

void v_stepen()
{
    double y = POP_DOUBLE, x = POP_DOUBLE, r;
    if(x==0)
        r=0;
    else
    {
        r=EXP(y*LOG(fabs(x)));
        //if(x<0&&y<1) r=-r;
        if(x < 0 && ((long)floor(y))&1)
            r = -r;
    }
    PUSH_DOUBLE(r);
}

void v_max()
{
    *(spf+1)=((*spf)>(*(spf+1))) ? (*spf) : (*(spf+1));
    spf++;
}

void v_min()
{
    *(spf+1)=((*spf)<(*(spf+1))) ? (*spf) : (*(spf+1));
    spf++;
}

void v_average()
{
    *(spf+1)=(( *spf)+(*(spf+1)))/2;
    spf++;
}

void v_random()
{
    *spf=( *spf) *((double)rand())/((double)RAND_MAX);
};

void v_randomize()
{
    double a=POP_DOUBLE;
    if(a<0)
        randomize();
    else
        srand(a);
}

void v_round()
{
    double digit=floor(POP_DOUBLE);
    double a=POP_DOUBLE;
    double r;
    if(digit>14)
        r=a;
    else
    {
        int i;
        for (i=0; i<digit; i++)
            a*=10;
        r=floor(a);
        if((a-r)>0.5)
            r+=1.0;
        for (i=0; i<digit; i++)
            r/=10;
    }
    PUSH_DOUBLE(r);
}

void v_roundt()
{
    double digit=floor(POP_DOUBLE);
    double a=POP_DOUBLE;
    double r;
    if(digit>14)
        r=a;
    else
    {
        int i;
        for (i=0; i<digit; i++)
            a*=10;
        r=floor(a);
        if((a-r)>=0.5)
            r+=1.0;
        for (i=0; i<digit; i++)
            r/=10;
    }
    PUSH_DOUBLE(r);
}

void v_trunc()
{
    double r=floor(POP_DOUBLE);
    r=floor(r);
    PUSH_DOUBLE(r);
}

void v_abs()
{
    *(spf)=fabs(*spf);
};

void v_sgn()
{
    double a=*(spf);
    if(a<0)
        a=-1.0;
    else
    {
        if(a>0)
            a=1.0;
        else
            a=0.0;
    }
    *(spf)=a;
}

void v_rad()
{
    *(spf)=M_PI *(*(spf))/180.0;
};

void v_deg()
{
    *(spf)=180.0*(*(spf))/M_PI;
};

void v_and()
{
    *(spf+1)=(*spf)&&(*(spf+1));
    spf++;
}

void v_or()
{
    *(spf+1)=(*spf)||(*(spf+1));
    spf++;
}

void v_notbin(){*(spf+1)=~((UINT32)(*spf));};

void v_xorbin()
{
    *(spf+1)=((UINT32)(*spf))^((UINT32)(*(spf+1)));
    spf++;
}

void v_xor()
{
    (*spf)=(((UINT32)(*spf))!=0)^(((UINT32)(*(spf+1)))!=0);
    spf++;
}

void v_not()
{
    if(*spf==0)
        *spf=1.0;
    else
        *spf=0.0;
}

void v_andi()
{
    *(spl+1)=(*spl)&&(*(spl+1));
    spl++;
}

void v_ori()
{
    *(spl+1)=(*spl)||(*(spl+1));
    spl++;
}

void v_noti()
{
    LONG i=POP_LONG;
    PUSH_DOUBLE(i==0L ? (1.0) : (0.0));
}

void v_stop()
{
    if ((POP_DOUBLE)>0)
        Stop();
}

void v_closeall()
{
    EXEC_FLAGS |= EF_MUSTSTOP;
}

void v_un_minus(){*(spf)=-(*spf);}

//int MCreate(int Q,long miny,long maxy,long minx,long maxx,int type);
void v_mcreate()
{
    double f=POP_DOUBLE;
    double maxx=POP_DOUBLE;
    double minx=POP_DOUBLE;
    double maxy=POP_DOUBLE;
    double miny=POP_DOUBLE;
    double Q=POP_DOUBLE;
    if(f>0)
    { Q=matrixs->MCreate(Q, miny, maxy, minx, maxx, 0);};
    PUSH_DOUBLE(Q);
}

//int MDelete(int Q);
void v_mdelete()
{
    double f=POP_DOUBLE;
    double Q=POP_DOUBLE;
    if(f>0)
    { Q=matrixs->MDelete(Q);}
    else
        Q=0;
    PUSH_DOUBLE(Q);
}

//int MFill(int Q,double value);
void v_mfill()
{
    double f=POP_DOUBLE;
    double value=POP_DOUBLE;
    double Q=POP_DOUBLE;

    if(f>0)
    { Q=matrixs->MFill(Q, value);}
    else
        Q=0;
    PUSH_DOUBLE(Q);
}

//int MDiag(int Q,double value);
void v_mdiag()
{
    double f=POP_DOUBLE;
    double value=POP_DOUBLE;
    double Q=POP_DOUBLE;
    if(f>0)
    { Q=matrixs->MDiag(Q, value);}
    else
        Q=0;
    PUSH_DOUBLE(Q);
}

void v_maddx()
{
    double f=POP_DOUBLE;
    double value=POP_DOUBLE;
    double Q=POP_DOUBLE;
    if(f>0)
    { Q=matrixs->MAddX(Q, value);}
    else
        Q=0;
    PUSH_DOUBLE(Q);
}

void v_msubx()
{
    double f=POP_DOUBLE;
    double value=POP_DOUBLE;
    double Q=POP_DOUBLE;
    if(f>0)
    { Q=matrixs->MSubX(Q, value);}
    else
        Q=0;
    PUSH_DOUBLE(Q);
}

void v_mmulx()
{
    double f=POP_DOUBLE;
    double value=POP_DOUBLE;
    double Q=POP_DOUBLE;
    if(f>0)
    { Q=matrixs->MMulX(Q, value);}
    else
        Q=0;
    PUSH_DOUBLE(Q);
}

void v_mdivx()
{
    double f=POP_DOUBLE;
    double value=POP_DOUBLE;
    double Q=POP_DOUBLE;
    if(f>0)
    { Q=matrixs->MDivX(Q, value);}
    else
        Q=0;
    PUSH_DOUBLE(Q);
}

void v_med()
{
    double f=POP_DOUBLE;
    double Q=POP_DOUBLE;
    if(f>0)
    { Q=matrixs->Med(Q);}
    else
        Q=0;
    PUSH_DOUBLE(Q);
}

void v_mdelta()
{
    double f=POP_DOUBLE;
    double Q=POP_DOUBLE;
    if(f>0)
    { Q=matrixs->MDelta(Q);}
    else
        Q=0;
    PUSH_DOUBLE(Q);
}

void v_mdet()
{
    double f=POP_DOUBLE;
    double Q=POP_DOUBLE;
    if(f>0)
    { Q=matrixs->MDet(Q);}
    else
        Q=0;
    PUSH_DOUBLE(Q);
}

void v_transp()
{
    double f=POP_DOUBLE;
    double Q2=POP_DOUBLE;
    double Q1=POP_DOUBLE;
    if(f>0)
    { Q2=matrixs->MTransp(Q1, Q2);}
    else
        Q2=0;
    PUSH_DOUBLE(Q2);
}

void v_maddc()
{
    double f=POP_DOUBLE;
    double Q3=POP_DOUBLE;
    double Q2=POP_DOUBLE;
    double Q1=POP_DOUBLE;
    if(f>0)
    {
        Q3 = matrixs->MAddC(Q1, Q2, Q3);
    }
    else
        Q3=0;

    PUSH_DOUBLE(Q3);
}

void v_mnot()
{
    double f=POP_DOUBLE;
    int Q1=POP_DOUBLE;
    if(f>0)
    { Q1=matrixs->MNot(Q1);}
    else
        Q1=0;
    PUSH_DOUBLE(Q1);
} ;

void v_msum()
{
    double f=POP_DOUBLE;
    double Q1=POP_DOUBLE;
    if(f>0)
    { Q1=matrixs->MSum((int)Q1);}
    else
        Q1=0;
    PUSH_DOUBLE(Q1);
} ;

void v_msubc()
{
    double f=POP_DOUBLE;
    double Q3=POP_DOUBLE;
    double Q2=POP_DOUBLE;
    double Q1=POP_DOUBLE;
    if(f>0)
    { Q3=matrixs->MSubC(Q1, Q2, Q3);}
    else
        Q3=0;
    PUSH_DOUBLE(Q3);
} ;

void v_mmulc()
{
    double f=POP_DOUBLE;
    double Q3=POP_DOUBLE;
    double Q2=POP_DOUBLE;
    double Q1=POP_DOUBLE;
    if(f>0)
    { Q3=matrixs->MMulC(Q1, Q2, Q3);}
    else
        Q3=0;
    PUSH_DOUBLE(Q3);
} ;

void v_mdivc()
{
    double f=POP_DOUBLE;
    double Q3=POP_DOUBLE;
    double Q2=POP_DOUBLE;
    double Q1=POP_DOUBLE;
    if(f>0)
    { Q3=matrixs->MDivC(Q1, Q2, Q3);}
    else
        Q3=0;
    PUSH_DOUBLE(Q3);
} ;

void v_mmul()
{
    double f=POP_DOUBLE;
    double Q3=POP_DOUBLE;
    double Q2=POP_DOUBLE;
    double Q1=POP_DOUBLE;
    if(f>0)
        Q3=matrixs->MMul(Q1, Q2, Q3);
    else
        Q3=0;
    PUSH_DOUBLE(Q3);
} ;

void v_mglue()
{
    double f=POP_DOUBLE;
    double x=POP_DOUBLE;
    double y=POP_DOUBLE;
    double Q3=POP_DOUBLE;
    double Q2=POP_DOUBLE;
    double Q1=POP_DOUBLE;
    if(f>0)
        Q3=matrixs->MGlue(Q1, Q2, Q3, y, x);
    else
        Q3=0;
    PUSH_DOUBLE(Q3);
} ;

void v_mcut()
{
    double f=POP_DOUBLE;
    double sx=POP_DOUBLE;
    double sy=POP_DOUBLE;
    double x=POP_DOUBLE;
    double y=POP_DOUBLE;
    double Q2=POP_DOUBLE;
    double Q1=POP_DOUBLE;
    if(f>0)
        Q2=matrixs->MCut(Q1, Q2, y, x, sy, sx);
    else
        Q2=0;
    PUSH_DOUBLE(Q2);
} ;

void v_mmove()
{
    double f=POP_DOUBLE;
    double x=POP_DOUBLE;
    double y=POP_DOUBLE;
    double Q1=POP_DOUBLE;
    if(f>0)
        Q1=matrixs->MMove(Q1, y, x);
    else
        Q1=0;
    PUSH_DOUBLE(Q1);
} ;

void v_mobr()
{
    double f=POP_DOUBLE;
    double Q2=POP_DOUBLE;
    double Q1=POP_DOUBLE;
    if(f>0)
        Q2=matrixs->MObr(Q1, Q2);
    else
        Q2=0;
    PUSH_DOUBLE(Q2);
} ;

//int MPut(int Q,long y,long x,double value);
void v_mput()
{
    double f=POP_DOUBLE;
    double value=POP_DOUBLE;
    double x=POP_DOUBLE;
    double y=POP_DOUBLE;
    double Q=POP_DOUBLE;
    if(f>0)
    { Q=matrixs->MPut(Q, y, x, value);}
    else
        Q=0;
    PUSH_DOUBLE(Q);
}

//double MGet(int Q,long y,long x,double value);
void v_mget()
{
    double f=POP_DOUBLE;
    double x=POP_DOUBLE;
    double y=POP_DOUBLE;
    double Q=POP_DOUBLE;
    if(f>0)
    { Q=matrixs->MGet(Q, y, x);}
    else
        Q=0;
    PUSH_DOUBLE(Q);
} ;

void v_meditor()
{
    double f=POP_DOUBLE;
    double Q=POP_DOUBLE;
    if(f)
        MatEditor(Q);
}

void v_msaveas()
{
    double f=POP_DOUBLE;
    char *name = POP_PTR;
    double Q=POP_DOUBLE;

    if(f>0)
    {
        char s[1000];
        GetFullFileNameEx((TProject *)GetCurrentProject(), name, s);
        Q=matrixs->SaveMatrixAs(Q, s);
    }
    else
        Q=0;
    PUSH_DOUBLE(Q);
} ;

void v_mload()
{
    double f=POP_DOUBLE;
    char *name=POP_PTR;
    double Q=POP_DOUBLE;

    if(f>0)
    {
        char s[1000];
        GetFullFileNameEx((TProject *)GetCurrentProject(), name, s);
        Q = matrixs->LoadMatrix(Q, s);
    }
    else
        Q=0;
    PUSH_DOUBLE(Q);
}

void v_msort()
{
    double f=POP_DOUBLE;
    double d=POP_DOUBLE;
    double k=POP_DOUBLE;
    double Q=POP_DOUBLE;
    if(f>0)
    { Q=matrixs->MSort(Q, k, d);}
    else
        Q=0;
    PUSH_DOUBLE(Q);
}

void v_mdim()
{
    double *sx=(double *)POP_PTR;
    double *sy=(double *)POP_PTR;
    double *x=(double *)POP_PTR;
    double *y=(double *)POP_PTR;
    int Q=POP_DOUBLE;
    TMatrix *m=matrixs->GetMatrix(Q);
    if(Q)
    {
        *sx=m->dimX+m->minX-1;
        *sy=m->dimY+m->minY-1;
        *x=m->minX;
        *y=m->minY;
    }
    else
        Q = 0;

    PUSH_DOUBLE(Q);
} ;

void v_loaddialog()
{
    //char * flags  =
    POP_PTR;
    char *filter=POP_PTR;
    char *f=GetTmpString();
    lstrcpy(f, filter);
    char *s=GetTmpString();
    lstrcpy(s, POP_PTR);
    WORD *c=codepoint;
    int op=*codepoint;
    if(!winmanager->mFileDialog(s, f, 0))
        lstrcpy(s, "");
    if(c!=codepoint||(op!=*codepoint)){}
    PUSH_PTR(s);
}

void v_savedialog()
{
    // char * flags  =
    POP_PTR;
    char *filter=POP_PTR;
    char *s=GetTmpString();
    char *f=GetTmpString();
    lstrcpy(f, filter);
    lstrcpy(s, POP_PTR);
    if(!winmanager->mFileDialog(s, f, FD_SAVE))
        lstrcpy(s, "");
    PUSH_PTR(s);
}

void l_to_f()
{
    long i=(*spl);
    spl--;  // достаем из стека
    spf++;
    *spf=i; // засовываем обратно
} ;

void f_to_l()
{
    long i=(*spf);
    spf--;
    spl++;
    *spl=i;
} ;

//............................................. strings

void plus_string()
{
    char *s1=POP_PTR;
    char *s2=POP_PTR;
    char *s=GetTmpString();
    int n=lstrlen(Lstrcpyn(s, s2, STRING_SIZE));
    Lstrcpyn(s+n, s1, STRING_SIZE-n);
    PUSH_PTR(s);
} ;


void left_string()
{
    int n=(int)POP_DOUBLE;
    n++;
    char *s1=POP_PTR;
    char *s=GetTmpString();
    if(n>STRING_SIZE)
        n=STRING_SIZE;
    Lstrcpyn(s, s1, n);
    PUSH_PTR(s);
} ;
//int r_n;
//int r_size;
//char r_str[MAX_STRING];

void right_string()
{
    int n=(int)POP_DOUBLE;
    n++;
    char *s1=POP_PTR;
    char *s=GetTmpString();
    int size=lstrlen(s1);
    if(n>size+1)
        n=size;
    Lstrcpyn(s, s1+size-n+1, n);
    PUSH_PTR(s);
} ;

void substr_string()
{
    int n2=(int)POP_DOUBLE;
    n2++;
    int n1=(int)POP_DOUBLE;

    if(n1 < 0)
        n1 = 0;

    if(n2 < 0)
        n2 = 0;

    char* s1 = POP_PTR;
    char *s = GetTmpString();

    if(n1 > STRING_SIZE)
        n1 = STRING_SIZE;

    if((n1 + n2) > STRING_SIZE)
        n2 = (STRING_SIZE - n1);

    Lstrcpyn(s, s1+n1, n2);
    // s[n1+n2+1] = 0;
    PUSH_PTR(s);
}

void pos_string()
{
    int n=-1;
    int k=(int)POP_DOUBLE; // n
    char *fr = POP_PTR;      // key
    char *str = POP_PTR;     // string

    int i=0;
    int c=1;
    while(*str)
    {
        if(*str == *fr)
        {
            char *p1 = str;
            char *p2 = fr;
            while(*p1 && *p1 == *p2)
            {
                p1++;
                p2++;
                if(!(*p2))
                {
                    if(c++==k)
                    {
                        n=i;
                        goto ps2;
                    }
                    else
                        goto ps1;
                }
            }
        }
ps1:
        i++;
        str++;
    }
ps2:
    PUSH_DOUBLE(n);
} ;

void replicate_string()
{
    int n=(int)POP_DOUBLE; // n
    char *s1=POP_PTR;      // shablon
    char *s=GetTmpString();
    int size=lstrlen(s1);
    if(size>STRING_SIZE)
        size=STRING_SIZE;
    if(n>0)
        lstrcpy(s, s1);
    int maxsize=size;
    for (int i=1; i<n; i++)
    {
        if((maxsize+size)>=STRING_SIZE)
            break;
        lstrcat(s, s1);
        maxsize+=size;
    }
    PUSH_PTR(s);
} ;

void lower_string()
{
    char *s1=POP_PTR;
    char *s=GetTmpString();
    Lstrcpyn(s, s1, STRING_SIZE);
    AnsiLower(s);
    PUSH_PTR(s);
} ;

void upper_string()
{
    char *s1=POP_PTR;
    char *s=GetTmpString();
    Lstrcpyn(s, s1, STRING_SIZE);
    AnsiUpper(s);
    PUSH_PTR(s);
} ;

void ansi_to_oem()
{
    char *s1=POP_PTR;

    char *s=GetTmpString();
    Lstrcpyn(s, s1, STRING_SIZE);
    AnsiToOem(s1, s);

    PUSH_PTR(s);
} ;

void oem_to_ansi()
{
    char *s1=POP_PTR;
    char *s=GetTmpString();
    Lstrcpyn(s, s1, STRING_SIZE);
    OemToAnsi(s1, s);
    PUSH_PTR(s);
} ;

void compare_string()
{
    double n=lstrcmp(*(spp+1), *spp);
    spp+=2;
    PUSH_DOUBLE(!n);
} ;

void comparei_string()
{
    int n=(int)POP_DOUBLE;

    char *s1=POP_PTR;
    char *s2=POP_PTR;

    char *stmp1=GetTmpString();
    char *stmp2=GetTmpString();

    if(n > STRING_SIZE)
        n = STRING_SIZE;

    Lstrcpyn(stmp1, s1, n);
    Lstrcpyn(stmp2, s2, n);

    double i = lstrcmp(stmp1, stmp2);
    PUSH_DOUBLE(!i);
} ;

void length_string()
{
    double n=lstrlen(*spp);
    if(n>STRING_SIZE)
        n=STRING_SIZE;
    spp++;
    PUSH_DOUBLE(n);
} ;

void ltrim_string()
{
    char *s1=POP_PTR;
    char *s=GetTmpString();
    Lstrcpyn(s, s1, STRING_SIZE);
    int i=0;
    while(s[i++]==' ')
        ;
    i--;
    PUSH_PTR(s+i);
} ;

void rtrim_string()
{
    char *s1=POP_PTR;
    char *s=GetTmpString();
    Lstrcpyn(s, s1, STRING_SIZE);
    int i=lstrlen(s);
    while(s[--i]==' ')
        ;
    i++;
    s[i]=0;
    PUSH_PTR(s);
} ;

void alltrim_string()
{
    char *s1=POP_PTR;
    char *s=GetTmpString();
    Lstrcpyn(s, s1, STRING_SIZE);
    int i=lstrlen(s);
    while(s[--i]==' ')
        ;
    i++;
    s[i]=0;
    i=0;
    while(s[i++]==' ')
        ;
    i--;
    PUSH_PTR(s+i);
} ;

void ascii_string()
{
    char *s=POP_PTR;
    PUSH_DOUBLE(s[0]);
} ;

void chr_string()
{
    char n=(char)POP_DOUBLE;
    char *s=GetTmpString();
    s[0]=n;
    s[1]=0;
    PUSH_PTR(s);
} ;

void float_to_string()
{
    double n=POP_DOUBLE;
    char *s=GetTmpString();
    // string_tmp = ecvt(n,10, &dec, &sign);
    sprintf(s, "%g", n);
    PUSH_PTR(s);
} ;

int SComp(char *str1, char *str2)
{
    while(*str1 && *str2)
    {
        if (*str1++ != *str2++) return 0;
    }
    if(*str1==0 && *str2!=0)return 0;
    return 1;
} ;

void schange()
{
    char *s3=POP_PTR; // на что
    char *s2=POP_PTR; // что
    char *s1=POP_PTR; // где
    char *s=GetTmpString();

    int i=0;
    int j=0;
    int k=lstrlen(s2);
    int l=lstrlen(s3);
    int m=lstrlen(s1);
    while(i<m)
    {
        if(SComp(s1+i, s2))
        {
            if(j+l>=STRING_SIZE)
                break;
            lstrcpy(s+j, s3);
            j+=l;
            i+=k;
        }
        else
        {
            if(j>=STRING_SIZE)
                break;
            s[j]=s1[i];
            i++;
            j++;
        }
    }
    s[j]=0;
    PUSH_PTR(s);
} ;

void vm_getanglebyxy()
{
    double x, y;
    y=POP_DOUBLE;
    x=POP_DOUBLE;
    PUSH_DOUBLE(GetAngleByXY2d(x, y));
} ;
/*
void float_to_integer()
{
 double n = POP_DOUBLE;
 string_buffer =
 PUSH_PTR(string_buffer);
};

void float_to_word()
{
 double n = POP_DOUBLE;
 string_buffer =
 PUSH_PTR(string_buffer);
};
*/
void string_to_float()
{
    char *s=POP_PTR;
    double n=atof(s);
    PUSH_DOUBLE(n);
} ;
/*
void string_to_integer()
{
 char* s = POP_PTR;
 double n = atol(n);
 PUSH_DOUBLE(n);
};

void string_to_word()
{
 char* s = POP_PTR;
 double n = atoi(n);
 PUSH_DOUBLE(n);
};
*/

//............................................. Window

void v_loadspacewindow()
{
    char *style=POP_PTR;
    char *file=POP_PTR;
    char *winname=POP_PTR;
    HSP2D hsp=winmanager->LoadSpaceWindow(winname, file, style);
    PUSH_HSP(hsp);
} ;

void v_openschemewindow()
{
    char *style=POP_PTR;
    char *_classn=POP_PTR;
    char *winname=POP_PTR;
    HSP2D hsp=winmanager->OpenSchemeWindow(winname, _classn, style);
    PUSH_HSP(hsp);
}

//new
void v_createwindowex()
{
    char *style=POP_PTR;
    int cy=POP_DOUBLE;
    int cx=POP_DOUBLE;
    int y=POP_DOUBLE;
    int x=POP_DOUBLE;
    char *data=POP_PTR;
    char *parent=POP_PTR;
    char *wname=POP_PTR;

    HSP2D hsp = winmanager->CreateWindowEx_(wname, parent, data, 0, x, y, cx, cy, style);
    PUSH_HSP(hsp);
}

// name "CreateWindowEx"  arg "STRING","STRING","STRING","FLOAT","FLOAT","FLOAT","FLOAT","STRING" ret "HANDLE" out 421
//new
void v_closewindow()
{
    char *winname=POP_PTR;
    int r=winmanager->CloseWindow(winname);
    PUSH_DOUBLE(r);
} ;

void v_setwindowtitle()
{
    char *title=POP_PTR;
    char *winname=POP_PTR;
    double r = winmanager->SetWindowTitle(winname, title);
    PUSH_DOUBLE(r);
}

void v_getwindowtitle()
{
    char *winname=POP_PTR;
    char *s=GetTmpString();
    //double r=
    winmanager->GetWindowTitle(winname, s);
    PUSH_PTR(s);
} ;

void v_showwindow()
{
    int nCmdShow=POP_DOUBLE;
    char *winname=POP_PTR;
    double r=winmanager->ShowWindow(winname, nCmdShow);
    PUSH_DOUBLE(r);
} ;

void v_setwindowpos()
{
    int sy=POP_DOUBLE;
    int sx=POP_DOUBLE;
    int y=POP_DOUBLE;
    int x=POP_DOUBLE;

    char *winname=POP_PTR;
    double r=winmanager->SetWindowPos(winname, x, y, sx, sy);
    PUSH_DOUBLE(r);
} ;

void v_setwindoworg()
{
    int y=POP_DOUBLE;
    int x=POP_DOUBLE;
    char *winname=POP_PTR;
    double r=winmanager->SetWindowOrg(winname, x, y);
    PUSH_DOUBLE(r);
} ;

void v_setwindowsize()
{
    int y=POP_DOUBLE;
    int x=POP_DOUBLE;
    char *winname=POP_PTR;
    double r=winmanager->SetWindowSize(winname, x, y);
    PUSH_DOUBLE(r);
} ;

void v_setclientsize()
{
    int y=POP_DOUBLE;
    int x=POP_DOUBLE;
    char *winname=POP_PTR;
    double r=winmanager->SetClientSize(winname, x, y);
    PUSH_DOUBLE(r);
} ;

void v_winorgx()
{
    char *winname=POP_PTR;
    RECT r;
    winmanager->WindowRect(winname, r);
    PUSH_DOUBLE(r.left);
} ;

void v_winorgy()
{
    char *winname=POP_PTR;
    RECT r;
    winmanager->WindowRect(winname, r);
    PUSH_DOUBLE(r.top);
} ;

void v_winsizex()
{
    char *winname=POP_PTR;
    RECT r;
    winmanager->WindowRect(winname, r);
    PUSH_DOUBLE(r.right-r.left);
} ;

void v_winsizey()
{
    char *winname=POP_PTR;
    RECT r;
    winmanager->WindowRect(winname, r);
    PUSH_DOUBLE(r.bottom-r.top);
} ;

void v_iswindowvisible()
{
    char *winname=POP_PTR;
    double r=winmanager->IsWindowVisible(winname);
    PUSH_DOUBLE(r);
} ;

void v_isIconic()
{
    char *winname=POP_PTR;
    double r=winmanager->IsIconic(winname);
    PUSH_DOUBLE(r);
} ;

void v_iswindowexist()
{
    char *winname=POP_PTR;
    double r=winmanager->IsWindowExist(winname);
    PUSH_DOUBLE(r);
} ;

void v_bringwindowtotop()
{
    char *winname=POP_PTR;
    double r=winmanager->BringWindowToTop(winname);
    PUSH_DOUBLE(r);
} ;

void v_cascadewindows(){winmanager->CascadeWindows();};

void v_tile(){winmanager->Tile(POP_DOUBLE);};

void v_arrangeicons(){winmanager->ArrangeIcons();};

void v_getnamebyhsp()
{
    char *n=GetTmpString();
    *n=0;
    HSP2D hsp=POP_HSP;
    winmanager->GetNameByHSP(hsp, n);
    PUSH_PTR(n);
} ;

void v_gethspbyname()
{
    char *n=POP_PTR;
    PUSH_HSP(winmanager->GetHANDLE(n));
} ;

void v_getobjectbyname()
{
    char *name=POP_PTR;
    HOBJ2D group=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    HOBJ2D obj = GetObjectByName2d(hsp, group, name);
    PUSH_HOBJ(obj)
} ;

void v_getobjectname2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    char *name=GetTmpString();
    ;
    *name=0;
    GetObjectName2d(hsp, obj, name, 255);
    PUSH_PTR(name)
} ;

void v_setobjectname2d()
{
    char *name=POP_PTR;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;

    int ret=SetObjectName2d(hsp, obj, name);
    PUSH_DOUBLE(ret)
} ;

static HOBJ2D _last_primary=0;

void v_getobjectfrompoint2d()
{
    POINT2D p;
    p.y=POP_DOUBLE;
    p.x=POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    HOBJ2D obj = GetObjectFromPoint2d(hsp, &p, &_last_primary);
    PUSH_HOBJ(obj);
} ;

void v_lock2d()
{
    INT16 lock=POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(LockUpdChange2d(hsp, lock));
}

void v_isintersect2d()
{
    UINT32 f=POP_DOUBLE;
    HOBJ2D hobj1=POP_HOBJ;
    HOBJ2D hobj2=POP_HOBJ;
    HSP2D hsp=POP_HSP;

    f=IsObjectsIntersect2d((HSPACE)hsp, hobj1, hobj2, f);
    PUSH_DOUBLE(f);
}

void v_setspacelayers()
{
    DWORD layers=POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetSpaceParam2d(hsp, SP_SETLAYERS, layers));
} ;

void v_getspacelayers()
{
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetSpaceParam2d(hsp, SP_GETLAYERS, 0));
} ;

void v_getobjectlayer2d()
{
    HOBJ2D hobj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetObjectLayer2d(hsp, hobj));
} ;

void v_setobjectlayer2d()
{
    INT16 l=POP_DOUBLE;
    HOBJ2D hobj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    SetObjectLayer2d(hsp, hobj, l);

    PUSH_DOUBLE(1);
} ;

void v_getobjectfrompoint2dex()
{
    UINT32 layers=POP_DOUBLE;
    POINT2D p;
    p.y=POP_DOUBLE;
    p.x=POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    HOBJ2D obj = GetObjectFromPoint2d(hsp, &p, &_last_primary, layers);
    PUSH_HOBJ(obj);
}

void vm_GetPrimaryFromPoint2d()
{
    PUSH_HOBJ(_last_primary);
}

void v_getobjecthandle(){PUSH_HOBJ(GetObjectHandle(pobject));};

void vm_getnamebyhandle()
{
    HOBJ2D h=POP_HOBJ;
    char *classname=POP_PTR;
    char *name=GetTmpString();
    *name=0;
    PClass _class=GetClassByName(classname);
    if(_class)
    {
        INT16 i=_class->GetChildById(h);
        if(i>-1)
        {
            if(_class->childs[i].name)
                lstrcpy(name,_class->childs[i].name);
        }
    }
    PUSH_PTR(name);
}

void vm_getactualwidth()
{
    HOBJ2D h=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    POINT2D p;
    if(!GetActualSize2d(hsp, h, &p))
        p.x=0.0;
    PUSH_DOUBLE(p.x);
} ;

void vm_getactualheight()
{
    HOBJ2D h=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    POINT2D p;
    if(!GetActualSize2d(hsp, h, &p))
        p.y=0.0;
    PUSH_DOUBLE(p.y);
} ;

void vm_getactualsize()
{
    double *y = (double *)POP_PTR;
    double *x = (double *)POP_PTR;
    HOBJ2D h = POP_HOBJ;
    HSP2D hsp = POP_HSP;
    POINT2D p;
    if(GetActualSize2d(hsp, h, &p))
    {
        *y=p.y;
        *x=p.x;
        PUSH_DOUBLE(1);
    }
    else
    {
        *y=*x=0;
        PUSH_DOUBLE(0);
    }
}

void vm_saverectarea()
{
    gr_float nHeight=POP_DOUBLE;
    gr_float nWidth=POP_DOUBLE;
    gr_float y=POP_DOUBLE;
    gr_float x=POP_DOUBLE;
    int attr=POP_DOUBLE;
    char *file=POP_PTR;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SaveSpaceImage2d(hsp, file, attr, x, y, nWidth, nHeight));
} ;

void vm_createcontrol2d()
{
    INT16 id=POP_DOUBLE;
    gr_float nHeight=POP_DOUBLE;
    gr_float nWidth=POP_DOUBLE;
    gr_float y=POP_DOUBLE;
    gr_float x=POP_DOUBLE;

    DWORD dwStyle=POP_DOUBLE;
    char *text=POP_PTR;
    char *classname=POP_PTR;
    HSP2D hsp=POP_HSP;
    HOBJ2D obj=CreateControlObject2d(hsp, classname, text, dwStyle, x, y, nWidth, nHeight, id, 0);
    PUSH_HOBJ(obj);
}

void vm_getcontroltext()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    HWND hwnd=GetControl2d(hsp, obj);
    char *text=GetTmpString();
    if(hwnd)
    { GetWindowText(hwnd, text, STRING_SIZE);}
    else
        *text=0;
    PUSH_PTR(text);
} ;

void vm_getcontroltextpart()
{
    int len = POP_DOUBLE;
    int begin = POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    HWND hwnd=GetControl2d(hsp, obj);
    char *text=GetTmpString();
    //*text=0;
    if(hwnd)
    {
        int length = GetWindowTextLength(hwnd);
        if(begin<length)
        {
            char* all=new char[length+1];
            GetWindowText(hwnd, all, length+1);
            len=max(0,min(STRING_SIZE,min(len,length-begin)));
            if(len>0)
            {
                strncpy(text,all+begin,len);
            }
            text[len]=0;
            delete all;
        }
    }
    PUSH_PTR(text);
} ;

void vm_getcontroltextlength()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    HWND hwnd=GetControl2d(hsp, obj);
    int length=0;
    if(hwnd) length = GetWindowTextLength(hwnd);
    PUSH_DOUBLE(length)
}

void vm_setcontroltext()
{
    char *text=POP_PTR;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    HWND hwnd=GetControl2d(hsp, obj);
    if(hwnd)
    {
        SetWindowText(hwnd, text);
        PUSH_DOUBLE(1);
    }
    else
    { PUSH_DOUBLE(0);}
} ;

void vm_addcontroltext()
{
    char *text=POP_PTR;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
#ifdef LOGON
    LogMessage("vm_addcontroltext 1");
#endif
    HWND hwnd=GetControl2d(hsp, obj);
    if(hwnd)
    {
#ifdef LOGON
        LogMessage("vm_addcontroltext 2");
#endif
        int length = GetWindowTextLength(hwnd);
#ifdef LOGON
        LogMessage("vm_addcontroltext 3");
#endif
        int len = strlen(text);
        if(length+len>0)
        {
#ifdef LOGON
            LogMessage("vm_addcontroltext 4");
#endif
            char* all=new char[length+len+1];
#ifdef LOGON
            LogMessage("vm_addcontroltext 5");
#endif
            GetWindowText(hwnd, all, length+1);
#ifdef LOGON
            LogMessage("vm_addcontroltext 6");
#endif
            strcat(all,text);
#ifdef LOGON
            LogMessage("vm_addcontroltext 7");
#endif
            SetWindowText(hwnd, all);
#ifdef LOGON
            LogMessage("vm_addcontroltext 8");
#endif
            //delete all;
            PUSH_DOUBLE(1);
            return;
        }
    }
    PUSH_DOUBLE(0);
} ;

void vm_insertcontroltext()
{
    int begin = POP_DOUBLE;
    char *text=POP_PTR;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    HWND hwnd=GetControl2d(hsp, obj);
    if(hwnd)
    {
        int length = GetWindowTextLength(hwnd);
        int len = strlen(text);
        if(length+len>0)
        {
            char* all=new char[length+len];
            GetWindowText(hwnd, all, length+1);
            begin=min(begin,length);
            char* endStr = NULL;
            if(begin<length)
            {
                endStr = new char[length-begin+1];
                strcpy(endStr,all+begin);
            }
            strcpy(all+begin,text);
            if(endStr!=NULL)  strcpy(all+begin+len,endStr);
            SetWindowText(hwnd, all);
            delete all;
            delete endStr;
            PUSH_DOUBLE(1);
        }
    }
    else
    { PUSH_DOUBLE(0);}
} ;

void vm_setcontrolfocus()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    if(obj)
    {
        HWND hwnd=GetControl2d(hsp, obj);
        if(hwnd)SetFocus(hwnd);
    }
    else
    {
        if(winmanager->GetWindow(hsp))
            SetFocus(winmanager->GetWindow(hsp)->HWindow);
    }
} ;

void vm_setcontrolfont()
{
    HOBJ2D hfont=POP_HOBJ;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    HWND hwnd=GetControl2d(hsp, obj);
    LOGFONT2D lf;
    if(hwnd && GetLogFont2d(hsp, hfont, &lf))
    {
        HFONT font = CreateFont(lf.lfFontSize/2, 0, lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
                                lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut,
                                lf.lfCharSet, lf.lfOutPrecision,
                                lf.lfClipPrecision, lf.lfQuality,
                                lf.lfPitchAndFamily, lf.lfFaceName);
        SendMessage(hwnd, WM_SETFONT, (WPARAM)font, true);
        PUSH_DOUBLE(1);
    }
    else
    {PUSH_DOUBLE(0)}
}

void vm_setcontroltextcolor()
{
    COLORREF rgb=POP_LONG;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    HWND hwnd=GetControl2d(hsp, obj);
    if(hwnd)
    {
        //HBRUSH br = (HBRUSH)SendMessage(hwnd, WM_CTLCOLORSTATIC, (WPARAM)GetDC(hwnd), (LPARAM)hwnd);
        //SetBkColor(GetDC(hwnd),TRANSPARENT);
        int rez = -1;
        //rez= br==NULL;
        rez = SetTextColor(GetDC(hwnd),rgb);
        PUSH_DOUBLE(rez==0);
    }
    else
    {PUSH_DOUBLE(0)}
}

void vm_checkdlgbutton()
{
    int mode=POP_DOUBLE;
    CONTROLS type;
    HWND hwnd=GetControl(&type);
    BOOL rez=0;
    if(hwnd)
    {
        if(type==CNT_BUTTON)
        {
            SendMessage(hwnd, BM_SETCHECK, mode, 0);
            rez=1;
        }
    }
    PUSH_DOUBLE(0);
} ;

void vm_isdlgbuttonchecked()
{
    int m=0;
    CONTROLS type;
    HWND hwnd=GetControl(&type);
    if(hwnd)
    {
        if(type==CNT_BUTTON)
        { m=(int)SendMessage(hwnd, BM_GETCHECK, 0, 0);}
    }
    PUSH_DOUBLE(m);
} ;

void setcontrolstyle2d()
{
    UINT32 s=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    HWND hwnd=GetControl2d(hsp, obj);
    UINT32 rez=0;
    if(hwnd)
    { rez=SetControlStyle2d(hsp, obj, s, 1);}
    PUSH_DOUBLE(rez);
} ;

CONTROLS _GetControlType(HWND hwnd)
{
    char lpszClassName[64];
    GetClassName(hwnd, lpszClassName, sizeof(lpszClassName));
    if(!lstrcmpi(lpszClassName, "LISTBOX"))
    { return CNT_LISTBOX;}
    if(!lstrcmpi(lpszClassName, "COMBOBOX"))
    { return CNT_COMBOBOX;}
    if(!lstrcmpi(lpszClassName, "BUTTON"))
    { return CNT_BUTTON;}
    if(!lstrcmpi(lpszClassName, "EDIT"))
    { return CNT_EDIT;}
    return CNT_NONE;
} ;

void getcontrolstyle2d(){ } ;

HWND GetControl(CONTROLS *type)
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    HWND hwnd=GetControl2d(hsp, obj);
    if(hwnd)
    { *type=_GetControlType(hwnd);}
    return hwnd;
}

void vm_enablecontrol()
{
    int e=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    HWND hwnd=GetControl2d(hsp, obj);
    BOOL rez=0;
    if(hwnd)
        rez=EnableWindow(hwnd, e);
    PUSH_DOUBLE(rez);
} ;

void lbaddstring()
{
    CONTROLS type;
    char *str=POP_PTR;
    HWND hwnd=GetControl(&type);
    int rez=0;
    if(hwnd)
    {
        if(type==CNT_LISTBOX)
            rez=(int)SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)str);
        if(type==CNT_COMBOBOX)
            rez=(int)SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)str);
    }
    PUSH_DOUBLE(rez);
} ;

void lbinsertstring()
{
    int index=POP_DOUBLE;
    CONTROLS type;
    char *str=POP_PTR;
    HWND hwnd=GetControl(&type);
    int rez=0;
    if(hwnd)
    {
        if(type==CNT_LISTBOX)
            rez=(int)SendMessage(hwnd, LB_INSERTSTRING, index, (LPARAM)str);
        if(type==CNT_COMBOBOX)
            rez=(int)SendMessage(hwnd, CB_INSERTSTRING, index, (LPARAM)str);
    }
    PUSH_DOUBLE(rez);
}

void lbgetstring()
{
    int index=POP_DOUBLE;
    CONTROLS type;
    char *str=GetTmpString();
    *str=0;
    HWND hwnd=GetControl(&type);
    if(hwnd)
    {
        if(type==CNT_LISTBOX)
            SendMessage(hwnd, LB_GETTEXT, index, (LPARAM)str);
        if(type==CNT_COMBOBOX)
            SendMessage(hwnd, CB_GETLBTEXT, index, (LPARAM)str);
    }
    PUSH_PTR(str);
}

void lbclearlist()
{
    CONTROLS type;
    HWND hwnd=GetControl(&type);
    BOOL rez=0;
    if(hwnd)
    {
        if(type==CNT_LISTBOX)
        {
            SendMessage(hwnd, LB_RESETCONTENT, 0, 0);
            rez=1;
        }
        if(type==CNT_COMBOBOX)
        {
            SendMessage(hwnd, CB_RESETCONTENT, 0, 0);
            rez=1;
        }
    }
    PUSH_DOUBLE(rez);
}

void lbdeletestring()
{
    int index=POP_DOUBLE;
    CONTROLS type;
    HWND hwnd=GetControl(&type);
    int rez=-1;
    if(hwnd)
    {
        if(type==CNT_LISTBOX)
            rez=(int)SendMessage(hwnd, LB_DELETESTRING, index, 0);
        if(type==CNT_COMBOBOX)
            rez=(int)SendMessage(hwnd, CB_DELETESTRING, index, 0);
    }
    PUSH_DOUBLE(rez);
}

void lbgetcount()
{
    CONTROLS type;
    HWND hwnd=GetControl(&type);
    long count=0;
    if(hwnd)
    {
        if(type==CNT_LISTBOX)
            count=SendMessage(hwnd, LB_GETCOUNT, 0, 0);
        if(type==CNT_COMBOBOX)
            count=SendMessage(hwnd, CB_GETCOUNT, 0, 0);
    }
    PUSH_DOUBLE(count);
}

void lbgetselindex()
{
    CONTROLS type;
    HWND hwnd=GetControl(&type);

    long index=-1;
    if(hwnd)
    {
        if(type==CNT_LISTBOX)
            index=SendMessage(hwnd, LB_GETCURSEL, 0, 0);
        if(type==CNT_COMBOBOX)
            index=SendMessage(hwnd, CB_GETCURSEL, 0, 0);
    }
    PUSH_DOUBLE(index);
}

void lbgetselindexs()
{
    CONTROLS type;
    HWND hwnd=GetControl(&type);
    dyno_t h = dyno_mem->NewVar();

    if(hwnd && type==CNT_LISTBOX)
    {
        int count = SendMessage(hwnd, LB_GETSELCOUNT, 0, 0);
        if(count > 0)
        {
            int *buf = new int[count];
            SendMessage(hwnd, LB_GETSELITEMS, (WPARAM)count, (LPARAM)buf);
            if(h)
            {
                for(int i=0;i<count;i++)
                {
                    if(dyno_mem->InsertInVar(h,"FLOAT"))
                    {
                        double t =(double)(buf[i]);
                        dyno_mem->SetVar(h, i, "FLOAT", 0, &t);
                    }
                }
            }
            delete buf;
        }
    }
    PUSH_LONG(h);
}

void lbsetselindex()
{
    int index = POP_DOUBLE;
    CONTROLS type;
    HWND hwnd = GetControl(&type);
    int rez=-1;

    if(hwnd)
    {
        if(type==CNT_LISTBOX)
            rez=(int)SendMessage(hwnd, LB_SETCURSEL, index, 0);
        if(type==CNT_COMBOBOX)
            rez=(int)SendMessage(hwnd, CB_SETCURSEL, index, 0);
    }
    PUSH_DOUBLE(rez);
}

void lbgetcaretindex()
{
    CONTROLS type;
    HWND hwnd=GetControl(&type);

    long index=-1;
    if(hwnd)
    {
        if (type==CNT_LISTBOX)index=SendMessage(hwnd,LB_GETCARETINDEX,0,0);
    }
    PUSH_DOUBLE(index);
}

void lbsetcaretindex()
{
    int index=POP_DOUBLE;
    CONTROLS type;
    HWND hwnd=GetControl(&type);
    int rez=-1;
    if(hwnd)
    {
        if (type==CNT_LISTBOX)rez=(int)SendMessage(hwnd,LB_SETCARETINDEX,index,0);
    }
    PUSH_DOUBLE(rez);
}

void lbfindstring()
{
    int index=POP_DOUBLE;
    CONTROLS type;
    char *str=POP_PTR;
    HWND hwnd=GetControl(&type);
    if(hwnd)
    {
        if(type==CNT_LISTBOX)
            index=(int)SendMessage(hwnd, LB_FINDSTRING, index, (LPARAM)str);
        if(type==CNT_COMBOBOX)
            index=(int)SendMessage(hwnd, CB_FINDSTRING, index, (LPARAM)str);
    }
    PUSH_DOUBLE(index);
}

void lbfindstringexact()
{
    int index=POP_DOUBLE;
    char *str=POP_PTR;
    CONTROLS type;
    HWND hwnd=GetControl(&type);
    if(hwnd)
    {
        if(type==CNT_LISTBOX)
            index=(int)SendMessage(hwnd, LB_FINDSTRINGEXACT, index, (LPARAM)str);
        if(type==CNT_COMBOBOX)
            index=(int)SendMessage(hwnd, CB_FINDSTRINGEXACT, index, (LPARAM)str);
    }
    PUSH_DOUBLE(index);
}

//............................................................ COLOREF

void rgb_color()
{
    double b=POP_DOUBLE;
    double g=POP_DOUBLE;
    double r=POP_DOUBLE;
    COLORREF rgb=RGB(r, g, b);
    PUSH_LONG(rgb)
} ;

void rgb_colorex()
{
    double b=POP_DOUBLE;
    double g=POP_DOUBLE;
    double r=POP_DOUBLE;
    double _a=POP_DOUBLE;

    long a=0;
    a=_a;
    a=a<<24;

    COLORREF rgb=RGB(r, g, b);
    rgb|=a;
    PUSH_LONG(rgb)
} ;

void message_box()
{
    unsigned int style=(unsigned int)POP_DOUBLE;
    char *s2=POP_PTR;
    char *s1=POP_PTR;
    float ret=SCMessageBox(s1, s2, style);
    PUSH_DOUBLE(ret);
} ;

void vm_dialogex(){
    /*
 INT16 size = *((INT16*)codepoint); codepoint+=1;
 char** p = NULL;
 if(size){p=(char**)new UINT32[size];
 INT16 i = (INT16)(size);
  while (i--){p[i] = POP_PTR; }
 };

 char*name = POP_PTR;
 double ret=0;
 if(size){
  EXEC_FLAGS|=EF_ONDIALG;
  ret=ModelDialogBox(name,size,p);
  EXEC_FLAGS&=~EF_ONDIALG;
  delete p;
 }
 PUSH_DOUBLE(ret);
 */
} ;

void vm_dialogex2()
{
    long h=POP_LONG;
    char *name=POP_PTR;
    int size=dyno_mem->GetCount(h);
    double ret=0;
    if(size)
    {
        char ** p=(char ** )new UINT32[size*3];
        char s[MAX_STRING]="";
        for (int i=0; i<size; i++)
        {
            dyno_mem->SetVar(h, i, "template", 0x80|3, s);
            p[i*3+1]=NewStr(s);
            dyno_mem->SetVar(h, i, "title", 0x80|3, s);
            p[i*3]=NewStr(s);
            dyno_mem->SetVar(h, i, "object", 0x80|3, s);
            p[i*3+2]=NewStr(s);
        }
        EXEC_FLAGS|=EF_ONDIALG;
        ret=ModelDialogBox(name, size*3, p);
        EXEC_FLAGS&=~EF_ONDIALG;
        for (i=0; i<size*3; i++)
        {
            if(p[i])delete p[i];
        }
        delete p;
    }
    PUSH_DOUBLE(ret);
}

//............................................................ GRAPHIC
void createobjectfromfile()
{
    INT16 flags=POP_DOUBLE;

    gr_float y=POP_DOUBLE;
    gr_float x=POP_DOUBLE;

    char *file=POP_PTR;

    HSP2D hsp=POP_HSP;

    char dst[MAX_STRING];
    pobject->GetFullFileName(file, dst);

    HOBJ2D h=CreateObjectFromFile(hsp, dst, x, y, flags);

    PUSH_HOBJ(h)
}

void createline2d()
{
    POINT2D p;
    p.y=POP_DOUBLE;
    p.x=POP_DOUBLE;

    HOBJ2D hbrush=POP_HOBJ;
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;

    HOBJ2D obj=(hsp && (hpen||hbrush)) ?
                   CreatePolyline2d(hsp, hpen, hbrush, &p, (INT16)1) :
                   (INT16)0;
    PUSH_HOBJ(obj)
} ;

void vm_setarrow2d()
{
    double aa, al, ba, bl, as, bs;
    bs=POP_DOUBLE;
    bl=POP_DOUBLE;
    ba=POP_DOUBLE;

    as=POP_DOUBLE;
    al=POP_DOUBLE;
    aa=POP_DOUBLE;

    HOBJ2D hobj2d=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    BOOL rez=FALSE;
    ARROW2D a;
    if(!GetVectorArrows2d(hsp, hobj2d, &a))
        memset(&a, 0, sizeof(a));
    a.Alength=al;
    a.Aangle=aa;
    a.Blength=bl;
    a.Bangle=ba;
    //  a.Adistance=as;a.Bdistance=bs;

    if(as)
        a.flags|=1;
    else
    { a.flags&=~1;}
    if(bs)
        a.flags|=2;
    else
    { a.flags&=~2;}
    rez=SetVectorArrows2d(hsp, hobj2d, &a);

    PUSH_DOUBLE(rez);
} ;

void emptyspace2d()
{
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(EmptySpace2d(hsp));
} ;

void CreatePolyLine2D()
{
    INT16 size=*((INT16*)codepoint);
    codepoint += 1;
    POINT2D *p = new POINT2D[size];

    INT16 i=(INT16)(size/2);
    while(i--)
    {
        p[i].y=POP_DOUBLE;
        p[i].x=POP_DOUBLE;
    }

    HOBJ2D hbrush = POP_HOBJ;
    HOBJ2D hpen = POP_HOBJ;
    HSP2D hsp = POP_HSP;

    HOBJ2D obj = CreatePolyline2d(hsp, hpen, hbrush, p, (INT16)(size/2));
    PUSH_HOBJ(obj);
    delete p;
}

void addvectorpoint2d()
{
    POINT2D p;
    p.y=POP_DOUBLE;
    p.x=POP_DOUBLE;
    INT16 point=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    BOOL pn=AddVectorPoint2d(hsp, obj, point, &p);
    PUSH_DOUBLE(pn)
} ;

void delvectorpoint2d()
{
    INT16 point=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    BOOL pn=DelVectorPoint2d(hsp, obj, point);
    PUSH_DOUBLE(pn)
} ;

void setvectorpoints2d()
{
    POINT2D p;
    p.y=POP_DOUBLE;
    p.x=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    BOOL pn=SetVectorPoints2d(hsp, obj, &p, 0);
    PUSH_DOUBLE(pn)
} ;

void setobjectattribute2d()
{
    ATTR_MODE mode=(ATTR_MODE)POP_DOUBLE;
    UINT16 Attr=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetAttribute2d(hsp, obj, Attr, mode));
}

void getobjectattribute2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetAttribute2d(hsp, obj));
}

void getbrushobject2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(GetBrushObject2d(hsp, obj));
} ;

void vm_setspacerenderengine2d()
{
    INT32 id=(INT32)POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetSpaceRenderEngine2d(hsp, id));
} ;

void setbkbrush()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetBkBrush2d(hsp, obj));
} ;

void getbkbrush()
{
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(GetBkBrush2d(hsp));
} ;

void getpenobject2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(GetPenObject2d(hsp, obj));
} ;

void getrgncreatemode()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetRgnCreateMode(hsp, obj));
} ;

void getvectornumpoints2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetVectorNumPoints2d(hsp, obj));
} ;

void getvectorpoint2dx()
{
    INT16 pn=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    POINT2D pp;
    GetVectorPoint2d(hsp, obj, pn, &pp);
    PUSH_DOUBLE(pp.x);
} ;

void getvectorpoint2dy()
{
    INT16 pn=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    POINT2D pp;
    GetVectorPoint2d(hsp, obj, pn, &pp);
    PUSH_DOUBLE(pp.y);
} ;

void setbrushobject2d()
{
    HOBJ2D hbrush=POP_HOBJ;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(SetBrushObject2d(hsp, obj, hbrush));
} ;

void setpenobject2d()
{
    HOBJ2D hpen=POP_HOBJ;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(SetPenObject2d(hsp, obj, hpen));
} ;

void setrgncreatemode()
{
    INT16 mode=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetRgnCreateMode(hsp, obj, mode));
} ;

void setvectorpoint2d()
{
    POINT2D p;
    p.y=POP_DOUBLE;
    p.x=POP_DOUBLE;
    INT16 num=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetVectorPoint2d(hsp, obj, num, &p));
} ;

void createpen2d()
{
    INT16 rop = POP_DOUBLE;
    COLORREF rgb = POP_LONG;
    INT16 width = POP_DOUBLE;
    INT16 style = POP_DOUBLE;
    HSP2D hsp = POP_HSP;

    PUSH_HOBJ(CreatePen2d(hsp, style, width, rgb, rop));
} ;

void getpencolor2d()
{
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_LONG(GetPenColor2d(hsp, hpen));
} ;

void getpenrop2d()
{
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetPenRop2d(hsp, hpen));
} ;

void getpenstyle2d()
{
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetPenStyle2d(hsp, hpen));
} ;

void getpenwidth2d()
{
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetPenWidth2d(hsp, hpen));
} ;

void setpencolor2d()
{
    COLORREF rgb=POP_LONG;
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetPenColor2d(hsp, hpen, rgb));
} ;

void setpenrop2d()
{
    INT16 rop=POP_DOUBLE;
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetPenRop2d(hsp, hpen, rop));
} ;

void setpenstyle2d()
{
    INT16 style=POP_DOUBLE;
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetPenStyle2d(hsp, hpen, style));
} ;

void setpenwidth2d()
{
    INT16 w=POP_DOUBLE;
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetPenWidth2d(hsp, hpen, w));
} ;

void deletetool2d()
{
    HOBJ2D hpen=POP_HOBJ;
    TOOL_TYPE type=(TOOL_TYPE)POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(DeleteTool2d(hsp, type, hpen));
} ;

void gettoolref2d()
{
    HOBJ2D hpen=POP_HOBJ;
    TOOL_TYPE type=(TOOL_TYPE)POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetToolRef2d(hsp, type, hpen));
} ;

//----- brush ----

void getbrushcolor2d()
{
    HOBJ2D hb=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_LONG(GetBrushColor2d(hsp, hb));
}

void getbrushrop2d()
{
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetBrushRop2d(hsp, hpen));
}

void getbrushdib2d()
{
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_LONG(GetBrushDibIndex2d(hsp, hpen));
} ;

void getbrushstyle2d()
{
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetBrushStyle2d(hsp, hpen));
} ;

void getbrushhatch2d()
{
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetBrushHatch2d(hsp, hpen));
} ;

void setbrushcolor2d()
{
    COLORREF rgb=POP_LONG;
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetBrushColor2d(hsp, hpen, rgb));
} ;

void setbrushrop2d()
{
    INT16 rop=POP_DOUBLE;
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetBrushRop2d(hsp, hpen, rop));
} ;

void setbrushdib2d()
{
    HOBJ2D hdib=POP_HOBJ;
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetBrushDibIndex2d(hsp, hpen, hdib));
} ;

void setbrushstyle2d()
{
    INT16 style=POP_DOUBLE;
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetBrushStyle2d(hsp, hpen, style));
} ;

void setbrushhatch2d()
{
    INT16 w=POP_DOUBLE;
    HOBJ2D hpen=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetBrushHatch2d(hsp, hpen, w));
} ;

//--------------

void setdibobject2d()
{
    HOBJ2D hdib=POP_HOBJ;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetDibObject2d(hsp, obj, hdib));
}

void setddibobject2d()
{
    HOBJ2D hdib=POP_HOBJ;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetDoubleDibObject2d(hsp, obj, hdib));
}

void getdibobject2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(GetDibObject2d(hsp, obj));
}

void getddibobject2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(GetDoubleDibObject2d(hsp, obj));
}

void deleteobject2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(DeleteObject2d(hsp, obj));
} ;

void getobjectorg2dx()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    POINT2D p;
    GetObjectOrgSize2d(hsp, obj, &p, NULL);
    PUSH_DOUBLE(p.x);
} ;

void getobjectorg2dy()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    POINT2D p;
    GetObjectOrgSize2d(hsp, obj, &p, NULL);
    PUSH_DOUBLE(p.y);
} ;

void getobjectparent2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(GetObjectParent2d(hsp, obj));
} ;

void getobjectangle2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetObjectAngle2d(hsp, obj));
} ;

void getobjectsize2dx()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    POINT2D p;
    GetObjectOrgSize2d(hsp, obj, NULL, &p);
    PUSH_DOUBLE(p.x);
} ;

void getobjectsize2dy()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    POINT2D p;
    GetObjectOrgSize2d(hsp, obj, NULL, &p);
    PUSH_DOUBLE(p.y);
} ;

void getobjecttype()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetObjectType2d(hsp, obj));
} ;

void setobjectorg2d()
{
    POINT2D p;
    p.y=POP_DOUBLE;
    p.x=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(ISpaceGraphicsItem*(hsp, obj, &p));
} ;

void setobjectsize2d()
{
    POINT2D p;
    p.y=POP_DOUBLE;
    p.x=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetObjectSize2d(hsp, obj, &p));
}

void createbrush2d()
{
    INT16 rop=POP_DOUBLE;
    HOBJ2D dib=POP_HOBJ;
    COLORREF rgb=POP_LONG;
    INT16 hatch=POP_DOUBLE;
    INT16 style=POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(CreateBrush2d(hsp, style, hatch, rgb, dib, rop));
} ;

void createdoubledib2d()
{
    char *name=POP_PTR;
    HSP2D hsp=POP_HSP;
    char s[MAX_STRING];
    pobject->GetFullFileName(name, s);
    PUSH_HOBJ(ImportDBM(hsp, s));
}

void createdib2d()
{
    char *name=POP_PTR;
    HSP2D hsp=POP_HSP;

    char s[MAX_STRING];
    pobject->GetFullFileName(name, s);

    PUSH_HOBJ(ImportBMP(hsp, s));
} ;

void createrdoubledib2d()
{
    char *name=POP_PTR;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(CreateRDoubleDib2d(hsp, name));
}

void createrdib2d()
{
    char *name=POP_PTR;
    HSP2D hsp=POP_HSP;
    char s[MAX_STRING];
    pobject->GetFullFileName(name, s);
    PUSH_HOBJ(CreateRDib2d(hsp, s));
}

void createbitmap2d()
{
    POINT2D so, ss, vs, vo;
    vo.y=POP_DOUBLE;
    vo.x=POP_DOUBLE;
    HOBJ2D hdib=POP_HOBJ;
    HSP2D hsp=POP_HSP;

    so.x=0;  so.y=0;
    ss.x=-1; ss.y=-1;
    vs=ss;
    PUSH_HOBJ(CreateBitmap2d(hsp, hdib, &so, &ss, &vo, &vs));
}

void createdoublebitmap2d()
{
    POINT2D so, ss, vs, vo;
    vo.y=POP_DOUBLE;
    vo.x=POP_DOUBLE;
    HOBJ2D hddib=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    so.x=0;  so.y=0;
    ss.x=-1; ss.y=-1;
    vs=ss;
    PUSH_HOBJ(CreateDoubleBitmap2d(hsp, hddib, &so, &ss, &vo, &vs));
}

void getscalespace2d()
{
    HSP2D hsp=POP_HSP;
    POINT2D _m, _d;
    GetScaleSpace2d(hsp, &_m, &_d);
    double s = _m.x;
    s /= ((double)_d.x);
    PUSH_DOUBLE(s);
}

void setscalespace2d()
{
    double sc=POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    POINT2D m, d;
    d=TPoint(100, 100);
    m=TPoint(sc*100, sc*100);
    if(m.x<0)
    { m.y=m.x=0;}
    PUSH_DOUBLE(SetScaleSpace2d(hsp, &m, &d));
}

void setspaceorg2d()
{
    POINT2D p;
    p.y=POP_DOUBLE;
    p.x=POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(winmanager->SetOrg(hsp, p.x, p.y));
}

void setscrollrange()
{
    double p2=POP_DOUBLE;
    double p1=POP_DOUBLE;
    double h=POP_DOUBLE;
    char *name=POP_PTR;
    PUSH_DOUBLE(winmanager->SetScrollRange(name, h, p1, p2));
}

void getwindowprop()
{
    char *data=GetTmpString();
    *data=0;
    char *type=POP_PTR;
    char *name=POP_PTR;
    winmanager->GetWindowProp(name, type, data);
    PUSH_PTR(data);
} ;

void getspaceorgx()
{
    HSP2D hsp = POP_HSP;

    POINT2D p;
    GetOrgSpace2d(hsp, &p);
    PUSH_DOUBLE(p.x);
}

void getspaceorgy()
{
    HSP2D hsp=POP_HSP;
    POINT2D p;
    GetOrgSpace2d(hsp, &p);
    PUSH_DOUBLE(p.y);
}

void setlogstring2d()
{
    char *s=POP_PTR;
    HOBJ2D hstr=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetLogString2d(hsp, hstr, s));
} ;

void getlogstring2d()
{
    HOBJ2D hstr=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    char *s=GetTmpString();
    GetLogString2d(hsp, hstr, s, 255);
    PUSH_PTR(s);
} ;

void createstring2d()
{
    char *s=POP_PTR;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(CreateString2d(hsp, s));
} ;

void createfont2d()
{
    LOGFONT2D lf;
    memset(&lf, 0, sizeof(lf));
    lf.lfSize = sizeof(lf);

    int fl = POP_DOUBLE;
    lf.lfHeight = POP_DOUBLE;

    if(fl&1)	 lf.lfItalic=1;
    if(fl&2)   lf.lfUnderline=1;
    if(fl&4)   lf.lfStrikeOut=1;
    if(fl&8)   lf.lfWeight=FW_BOLD;

    char *s=POP_PTR;
    lstrcpy(lf.lfFaceName, s);

    //lf.lfFontSize=lf.lfHeight*3/2;
    HSP2D hsp = POP_HSP;
    HOBJ2D hfont = CreateFont2d(hsp, &lf);
    PUSH_HOBJ(hfont);
}

void createfont2dpt()
{
    LOGFONT2D lf;
    memset(&lf, 0, sizeof(lf));
    lf.lfSize=sizeof(lf);

    int fl=POP_DOUBLE;
    int size = POP_DOUBLE;

    if(fl&1)	 lf.lfItalic=1;
    if(fl&2)   lf.lfUnderline=1;
    if(fl&4)   lf.lfStrikeOut=1;
    if(fl&8)   lf.lfWeight=FW_BOLD;

    char *s=POP_PTR;
    lstrcpy(lf.lfFaceName, s);
    lf.lfHeight = -4 * size / 3;
    lf.lfFontSize = size * 2;

    HSP2D hsp=POP_HSP;
    HOBJ2D hfont=CreateFont2d(hsp, &lf);
    PUSH_HOBJ(hfont);
} ;

void createtext2d()
{
    LOGTEXT lt;
    lt.ltBgColor=POP_LONG;
    lt.ltFgColor=POP_LONG;
    lt.ltStringIndex=POP_HOBJ;
    lt.ltFontIndex=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(CreateText2d(hsp, &lt, 1));
} ;

void createrastertext2d()
{
    float angle=POP_DOUBLE;
    POINT2D org;
    org.y=POP_DOUBLE;
    org.x=POP_DOUBLE;
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(CreateTextRaster2d(hsp, htext, &org, &TPoint(-1, -1), &TPoint(0, 0), angle));
} ;

void gettextobject2d()
{
    HOBJ2D hobject=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(GetTextObject2d(hsp, hobject));
} ;

void gettextcount2d()
{
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetTextNumItems2d(hsp,htext));
} ;

void gettextstring2d()
{
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    LOGTEXT lt;
    if(GetLogText2d(hsp, htext, &lt, 1))
    {PUSH_HOBJ(lt.ltStringIndex);}
    else
    {PUSH_HOBJ(0);}
}

void gettextstring2d_i()
{
    int i=(int)POP_DOUBLE;
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    if(i>=0 && i<GetTextNumItems2d(hsp,htext))
    {
        i++;
        LOGTEXT* lt=new LOGTEXT[i];
        if(GetLogText2d(hsp, htext, lt, i))
        {
            PUSH_HOBJ(lt[i-1].ltStringIndex);
            delete[] lt;
            return;
        }
        delete[] lt;
    }
    PUSH_HOBJ(0);
}

void settextstring2d_i()
{
    short string_index=POP_HOBJ;
    int i=(int)POP_DOUBLE;
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    int count=GetTextNumItems2d(hsp,htext);
    if(i>=0 && i<count && count>0)
    {
        LOGTEXT* lt=new LOGTEXT[count];
        if(GetLogText2d(hsp, htext, lt, count))
        {
            lt[i].ltStringIndex=string_index;
            PUSH_DOUBLE(SetLogText2d(hsp, htext, lt, count));
            delete[] lt;
            return;
        }
        delete[] lt;
    }
    PUSH_DOUBLE(0);
}

void gettextfont2d()
{
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    LOGTEXT lt;
    if(GetLogText2d(hsp, htext, &lt, 1))
    { PUSH_HOBJ(lt.ltFontIndex);}
    else
    { PUSH_HOBJ(0);}
}

void gettextfont2d_i()
{
    int i=(int)POP_DOUBLE;
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    if(i>=0 && i<GetTextNumItems2d(hsp,htext))
    {
        i++;
        LOGTEXT* lt=new LOGTEXT[i];
        if(GetLogText2d(hsp, htext, lt, i))
        {
            PUSH_HOBJ(lt[i-1].ltFontIndex);
            delete[] lt;
            return;
        }
        delete[] lt;
    }
    PUSH_HOBJ(0);
}

void settextfont2d_i()
{
    short font_index=POP_HOBJ;
    int i=(int)POP_DOUBLE;
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    int count=GetTextNumItems2d(hsp,htext);
    if(i>=0 && i<count && count>0)
    {
        LOGTEXT* lt=new LOGTEXT[count];
        if(GetLogText2d(hsp, htext, lt, count))
        {
            lt[i].ltFontIndex=font_index;
            PUSH_DOUBLE(SetLogText2d(hsp, htext, lt, count));
            delete[] lt;
            return;
        }
        delete[] lt;
    }
    PUSH_DOUBLE(0);
}

void gettextfg2d()
{
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    LOGTEXT lt;
    if(GetLogText2d(hsp, htext, &lt, 1))
    { PUSH_LONG(lt.ltFgColor);}
    else
    { PUSH_LONG(lt.ltBgColor);}
}

void gettextfg2d_i()
{
    int i=(int)POP_DOUBLE;
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    if(i>=0 && i<GetTextNumItems2d(hsp,htext))
    {
        i++;
        LOGTEXT* lt=new LOGTEXT[i];
        if(GetLogText2d(hsp, htext, lt, i))
        {
            PUSH_LONG(lt[i-1].ltFgColor);
            delete[] lt;
            return;
        }
        delete[] lt;
    }
    PUSH_LONG(0);
}

void settextfg2d_i()
{
    long fg=POP_LONG;
    int i=(int)POP_DOUBLE;
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    int count=GetTextNumItems2d(hsp,htext);
    if(i>=0 && i<count && count>0)
    {
        LOGTEXT* lt=new LOGTEXT[count];
        if(GetLogText2d(hsp, htext, lt, count))
        {
            lt[i].ltFgColor=fg;
            PUSH_DOUBLE(SetLogText2d(hsp, htext, lt, count));
            delete[] lt;
            return;
        }
        delete[] lt;
    }
    PUSH_DOUBLE(0);
}

void gettextbk2d()
{
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    LOGTEXT lt;
    if(GetLogText2d(hsp, htext, &lt, 1))
    { PUSH_LONG(lt.ltBgColor);}
    else
    { PUSH_LONG(lt.ltBgColor);}
}

void gettextbg2d_i()
{
    int i=(int)POP_DOUBLE;
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    if(i>=0 && i<GetTextNumItems2d(hsp,htext))
    {
        i++;
        LOGTEXT* lt=new LOGTEXT[i];
        if(GetLogText2d(hsp, htext, lt, i))
        {
            PUSH_LONG(lt[i-1].ltBgColor);
            delete[] lt;
            return;
        }
        delete[] lt;
    }
    PUSH_LONG(0);
}

void settextbg2d_i()
{
    long bg=POP_LONG;
    int i=(int)POP_DOUBLE;
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    int count=GetTextNumItems2d(hsp,htext);
    if(i>=0 && i<count && count>0)
    {
        LOGTEXT* lt=new LOGTEXT[count];
        if(GetLogText2d(hsp, htext, lt, count))
        {
            lt[i].ltBgColor=bg;
            PUSH_DOUBLE(SetLogText2d(hsp, htext, lt, count));
            delete[] lt;
            return;
        }
        delete[] lt;
    }
    PUSH_DOUBLE(0);
}

void vm_setlogtext2d()
{
    LOGTEXT lt;
    lt.ltBgColor=POP_LONG;
    lt.ltFgColor=POP_LONG;
    lt.ltStringIndex=POP_HOBJ;
    lt.ltFontIndex=POP_HOBJ;
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetLogText2d(hsp, htext, &lt, 1));
};

void settext2d_i()
{
    LOGTEXT lt;
    lt.ltBgColor=POP_LONG;
    lt.ltFgColor=POP_LONG;
    lt.ltStringIndex=POP_HOBJ;
    lt.ltFontIndex=POP_HOBJ;
    int i=(int)POP_DOUBLE;
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    int count=GetTextNumItems2d(hsp,htext);
    if(i>=0 && i<count && count>0)
    {
        LOGTEXT* lta=new LOGTEXT[count];
        if(GetLogText2d(hsp, htext, lta, count))
        {
            lta[i].ltFontIndex=lt.ltFontIndex;
            lta[i].ltStringIndex=lt.ltStringIndex;
            lta[i].ltFgColor=lt.ltFgColor;
            lta[i].ltBgColor=lt.ltBgColor;
            PUSH_DOUBLE(SetLogText2d(hsp, htext, lta, count));
            delete[] lta;
            return;
        }
        delete[] lta;
    }
    PUSH_DOUBLE(0);
}

void inserttext2d()
{
    LOGTEXT lt;
    lt.ltBgColor=POP_LONG;
    lt.ltFgColor=POP_LONG;
    lt.ltStringIndex=POP_HOBJ;
    lt.ltFontIndex=POP_HOBJ;
    int i=(int)POP_DOUBLE;
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;

    int count=GetTextNumItems2d(hsp,htext);

    if(i>=0 && i<=count)
    {
        LOGTEXT* lta=new LOGTEXT[count+1];
        if(GetLogText2d(hsp, htext, lta, count))
        {
            for(int t=count;t>i;t--)
                lta[t]=lta[t-1];
            lta[i].ltFontIndex=lt.ltFontIndex;
            lta[i].ltStringIndex=lt.ltStringIndex;
            lta[i].ltFgColor=lt.ltFgColor;
            lta[i].ltBgColor=lt.ltBgColor;
            PUSH_DOUBLE(SetLogText2d(hsp, htext, lta, count+1));
            delete[] lta;
            return;
        }
        delete[] lta;
    }
    PUSH_DOUBLE(0);
}

void addtext2d()
{
    LOGTEXT lt;
    lt.ltBgColor=POP_LONG;
    lt.ltFgColor=POP_LONG;
    lt.ltStringIndex=POP_HOBJ;
    lt.ltFontIndex=POP_HOBJ;
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;

    int count=GetTextNumItems2d(hsp,htext);
    LOGTEXT* lta=new LOGTEXT[count+1];
    if(GetLogText2d(hsp, htext, lta, count))
    {
        lta[count].ltFontIndex=lt.ltFontIndex;
        lta[count].ltStringIndex=lt.ltStringIndex;
        lta[count].ltFgColor=lt.ltFgColor;
        lta[count].ltBgColor=lt.ltBgColor;
        PUSH_DOUBLE(SetLogText2d(hsp, htext, lta, count+1));
        delete[] lta;
        return;
    }
    PUSH_DOUBLE(0);
}

void removetext2d()
{
    int i=(int)POP_DOUBLE;
    HOBJ2D htext=POP_HOBJ;
    HSP2D hsp=POP_HSP;

    int count=GetTextNumItems2d(hsp,htext);

    if(i>=0 && i<count)
    {
        LOGTEXT* lta=new LOGTEXT[count];
        if(GetLogText2d(hsp, htext, lta, count))
        {
            for(int t=i;t<count-1;t++)
                lta[t]=lta[t+1];
            PUSH_DOUBLE(SetLogText2d(hsp, htext, lta, count-1));
            delete[] lta;
            return;
        }
        delete[] lta;
    }
    PUSH_DOUBLE(0);
}

void getfontname2d()
{
    HOBJ2D hfont=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    LOGFONT2D lf;
    if(GetLogFont2d(hsp, hfont, &lf))
    {
        char *s=GetTmpString();
        lstrcpy(s, lf.lfFaceName);
        PUSH_PTR(s);
        return;
    }
    PUSH_PTR("");
};

void setfontname2d()
{
    char *s=POP_PTR;
    HOBJ2D hfont=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    LOGFONT2D lf;
    if(GetLogFont2d(hsp, hfont, &lf))
    {
        lstrcpy(lf.lfFaceName, s);
        PUSH_DOUBLE(SetLogFont2d(hsp, hfont, &lf))
    }
    else
    {PUSH_DOUBLE(0);}
};

void getfontsize2d()
{
    HOBJ2D hfont=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    LOGFONT2D lf;
    if(GetLogFont2d(hsp, hfont, &lf))
    {
        PUSH_DOUBLE(lf.lfFontSize/2)
    }
    else
    {
        PUSH_DOUBLE(0)
    }
}

void setfontsize2d()
{
    int size=(int)POP_DOUBLE;
    HOBJ2D hfont=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    LOGFONT2D lf;
    if(GetLogFont2d(hsp, hfont, &lf) && size>0)
    {
        lf.lfFontSize=size*2;
        lf.lfHeight=-(4*size)/3;
        PUSH_DOUBLE(SetLogFont2d(hsp, hfont, &lf))
    }
    else
    {PUSH_DOUBLE(0)}
};

void getfontstyle2d()
{
    HOBJ2D hfont=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    LOGFONT2D lf;
    if(GetLogFont2d(hsp, hfont, &lf))
    {
        int style=0;
        style|=lf.lfItalic;
        style|=lf.lfUnderline*2;
        style|=lf.lfStrikeOut*4;
        style|=(lf.lfWeight==FW_BOLD)*8;
        PUSH_DOUBLE(style)
    }
    else
    {PUSH_DOUBLE(0)}
};

void setfontstyle2d()
{
    int style=(int)POP_DOUBLE;
    HOBJ2D hfont=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    LOGFONT2D lf;
    if(GetLogFont2d(hsp, hfont, &lf))
    {
        if(style&1)	 	lf.lfItalic=1;
        else 				 	lf.lfItalic=0;
        if(style&2)  	lf.lfUnderline=1;
        else					lf.lfUnderline=0;
        if(style&4)  	lf.lfStrikeOut=1;
        else					lf.lfStrikeOut=0;
        if(style&8)  	lf.lfWeight=FW_BOLD;
        else					lf.lfWeight=0;
        PUSH_DOUBLE(SetLogFont2d(hsp, hfont, &lf))
    }
    else
        PUSH_DOUBLE(0)
};

void setshowobject2d()
{
    int isshow = (int)POP_DOUBLE;
    HOBJ2D hobj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    int rez=SetShowObject2d(hsp, hobj, isshow);
    PUSH_DOUBLE(rez)
} ;

void showobject2d()
{
    HOBJ2D hobj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    ShowObject2d(hsp, hobj);
}

void hideobject2d()
{
    HOBJ2D hobj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    HideObject2d(hsp, hobj);
} ;

void vm_stdhyperjump()
{
    INT16 fwKeys=POP_DOUBLE;
    HOBJ2D hobj=POP_HOBJ;

    POINT2D p;
    p.y=POP_DOUBLE;
    p.x=POP_DOUBLE;

    HSP2D hsp=POP_HSP;

    if(Push_Context())
        winmanager->StdHyperJump(hsp, p, hobj, fwKeys);

    Pop_Context();
}

void rotateobject2d()
{
    ROTATOR2D r;
    r.angle = POP_DOUBLE;
    r.center.y = POP_DOUBLE;
    r.center.x = POP_DOUBLE;
    HOBJ2D hobj = POP_HOBJ;
    HSP2D hsp = POP_HSP;
    int rez = RotateObject2d(hsp, hobj, &r);
    PUSH_DOUBLE(rez)
} ;

void v_sndplaysound()
{
    int wFlags=POP_DOUBLE;
    char *file=POP_PTR;
    PUSH_DOUBLE(PlayWAV(file, wFlags));
}

void getclientwidth()
{
    char *name = POP_PTR;
    PUSH_DOUBLE(winmanager->GetClientWidth(name));
}

void getclientheight()
{
    char *name=POP_PTR;
    PUSH_DOUBLE(winmanager->GetClientHeight(name));
} ;

void chosecolordialog()
{
    COLORREF rgb = POP_LONG;
    char *title=POP_PTR;
    PUSH_LONG(winmanager->ColorDialog(title, rgb));
} ;

//-------------------
void getbottomobject2d()
{
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(GetBottomObject2d(hsp));
} ;

void getupperobject2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(GetUpperObject2d(hsp, obj));
} ;

void getobjectfromzorder2d()
{
    INT16 order=POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(GetObjectFromZOrder2d(hsp, order));
} ;

void getlowerobject2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(GetLowerObject2d(hsp, obj));
} ;

void gettopobject2d()
{
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(GetTopObject2d(hsp));
} ;

void getzorder2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetZOrder2d(hsp, obj));
} ;

void objecttobottom2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(ObjectToBottom2d(hsp, obj));
} ;

void objecttotop2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(ObjectToTop2d(hsp, obj));
} ;

void setzorder2d()
{
    INT16 order=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetZOrder2d(hsp, obj, order));
} ;

void swapobject2d()
{
    HOBJ2D obj1=POP_HOBJ;
    HOBJ2D obj2=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SwapObject2d(hsp, obj1, obj2));
} ;

void setbitmapsrcrect()
{
    POINT2D size, org;
    size.y=POP_DOUBLE;
    size.x=POP_DOUBLE;
    org.y=POP_DOUBLE;
    org.x=POP_DOUBLE;
    HOBJ2D Object2d=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    double r=SetBitmapSrcRect(hsp, Object2d, &org, &size);
    PUSH_DOUBLE(r);
} ;

void getbitmapsrcrect()
{
    POINT2D size, org;
    double *p1=(double *)POP_PTR;
    double *p2=(double *)POP_PTR;
    double *p3=(double *)POP_PTR;
    double *p4=(double *)POP_PTR;
    HOBJ2D Object2d=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    double r=GetBitmapSrcRect(hsp, Object2d, &org, &size);
    if(r)
    {
        *p1=size.y;
        *p2=size.x;
        *p3=org.y;
        *p4=org.x;
    }
    PUSH_DOUBLE(r);
} ;

void v_settodef()
{
    char *obj=POP_PTR;
    PObject po=SCGetObject(obj);
    if(po)
    {
        po->ClearAndDef();
        PUSH_DOUBLE(1);
    }
    else
        PUSH_DOUBLE(0);
}

void v_loadobjectstate()
{
    char *file=POP_PTR;
    char *obj=POP_PTR;
    PObject po=SCGetObject(obj);
    if(po)
    {
        DWORD ef=EXEC_FLAGS;
        EXEC_FLAGS&=~EF_STEPACTIVE;
        char dst[MAX_STRING];
        pobject->GetFullFileName(file, dst);
        PUSH_DOUBLE(LoadState(po, dst));
        EXEC_FLAGS=ef;
    }
    else
        PUSH_DOUBLE(0);
}

void v_saveobjectstate()
{
    char *file=POP_PTR;
    char *obj=POP_PTR;
    PObject po=SCGetObject(obj);
    if(po)
    {
        DWORD ef=EXEC_FLAGS;
        EXEC_FLAGS&=~EF_STEPACTIVE;
        char dst[MAX_STRING];
        pobject->GetFullFileName(file, dst);

        PUSH_DOUBLE(SaveState(po, dst));
        EXEC_FLAGS=ef;
    }
    else
        PUSH_DOUBLE(0);
}

void v_setimagename()
{
    char *name=POP_PTR;
    HOBJ2D hobj=POP_HOBJ;
    char *_classname=POP_PTR;
    PClass _class=GetClassByName(_classname);
    if(_class)
    {
        INT16 i=_class->GetChildById(hobj);
        if(i>-1)
        {
            if(_class->childs[i].name)
            {
                if(lstrcmp(_class->childs[i].name, name))
                {
                    delete _class->childs[i].name;
                    _class->childs[i].name=NULL;
                }
                else
                {
                    PUSH_DOUBLE(1);
                    return;
                }
            }
            if(name&&*name)
                _class->childs[i].name=NewStr(name);
            _class->SetModify();
            PUSH_DOUBLE(1);
            return;
        }
    }
    PUSH_DOUBLE(0);
} ;

//---------------------              <-------- STREAMS !!!

void v_createstream()
{
    char *flags=POP_PTR;
    char *name=POP_PTR;
    char *device=POP_PTR;

    int h=streams->CreateStream(device, name, flags);

    PUSH_LONG(h);
} ;

void v_closestream()
{
    int h=POP_LONGi;
    PUSH_DOUBLE(streams->CloseStream(h));
} ;

void v_seek()
{
    long pos=POP_DOUBLE;
    int h=POP_LONGi;
    PUSH_DOUBLE(streams->Seek(h, pos));
} ;

void v_streamstatus()
{
    int h=POP_LONGi;
    PUSH_DOUBLE(streams->Status(h));
} ;

void v_eof()
{
    int h=POP_LONGi;
    PUSH_DOUBLE(streams->Eof(h));
} ;

void v_getpos()
{
    int h=POP_LONGi;
    PUSH_DOUBLE(streams->GetPos(h));
} ;

void v_getsize()
{
    int h=POP_LONGi;
    PUSH_DOUBLE(streams->GetSize(h));
} ;

void v_setwidth()
{
    int w=(int)POP_DOUBLE;
    int h=POP_LONGi;
    PUSH_DOUBLE(streams->SetWidth(h, w));
} ;

void v_fread()
{
    int h=POP_LONGi;
    double d;
    if(!streams->FRead(h, &d))
        d=0;
    PUSH_DOUBLE(d);
}

void v_sread()
{
    int h = POP_LONGi;
    char *stmp = GetTmpString();
    if(!streams->GetLine(h, stmp))
        stmp[0]=0;

    else
    {
        char *ps=stmp;
        while(*ps)
        {
            if(*ps==13||*ps==10)
            {
                *ps=0;
                break;
            }
            ps++;
        }
    }
    PUSH_PTR(stmp);
} ;

void v_fwrite()
{
    double f=POP_DOUBLE;
    int h=POP_LONGi;
    PUSH_DOUBLE(streams->FWrite(h, f));
} ;

void v_copyblock()
{
    double size=POP_DOUBLE;
    double from=POP_DOUBLE;
    int dst=POP_LONGi;
    int src=POP_LONGi;
    TStream *_s=streams->GetStream(src);
    TStream *_d=streams->GetStream(dst);
    long r=0;
    if(_s&&_d)
    {
        if(from<0)
            from=0;
        if(size<0)
            size=_s->GetSize()-from;
        r=_s->CopyTo(_d, from, size);
    }
    PUSH_DOUBLE(r);
}

void v_truncate()
{
    int s=POP_LONGi;
    TStream *_s=streams->GetStream(s);
    long r=0;
    if(_s)
    {
        _s->Truncate();
        r=1;
    }
    PUSH_DOUBLE(r);
}

void v_swrite()
{
    char *str=POP_PTR;
    int h=POP_LONGi;
    PUSH_DOUBLE(streams->SWrite(h, str));
}

void v_getline()
{
    char *dl=POP_PTR;
    int sz=(int)POP_DOUBLE;
    int h=POP_LONGi;

    sz=min(sz, STRING_SIZE);
    char *stmp=GetTmpString();
    if(!streams->GetLine(h, stmp, sz, dl[0]))
        stmp[0]=0;
    PUSH_PTR(stmp);
} ;

//---------------------
void jgetx()
{
    double d=GetJoyX(POP_DOUBLE);
    PUSH_DOUBLE(d);
}

void jgety()
{
    double d=GetJoyY(POP_DOUBLE);
    PUSH_DOUBLE(d);
}

void jgetz()
{
    double d=GetJoyZ(POP_DOUBLE);
    PUSH_DOUBLE(d);
}

void jgetbuttons()
{
    double d=GetJoyBtn(POP_DOUBLE);
    PUSH_DOUBLE(d);
}

BOOL ScGetKeyState(int key);

void getasynckeystate()
{
    int vkey=POP_DOUBLE;
    int p=ScGetKeyState(vkey);
    PUSH_DOUBLE(p);
}

void system()
{
    INT16 count=*((INT16 *)codepoint);
    codepoint += 1;
    double *prm = NULL;
    if(count)
    {
        prm = new double[count];
        for (int i = 0; i < count; i++)
            prm[(count-1)-i] = POP_DOUBLE;
    }
    int code=POP_DOUBLE;

    double rez=0;
    switch(code){
        case 1:
            rez=AboutDialog(mainwindow->HWindow);
            break;
        case 2:
            rez=__SC_BUILD__;
            break;

#ifndef RUNTIME

        case 3:
            SysInfoDialog(mainwindow->HWindow);
            break;
#endif

        case 4:
            rez=calcedsteps;
            break;
        case 6:
            if(count==1)
                mainwindow->ShowControl(prm[0]);
            break;
        case 8:
            if(count==1)
                mainwindow->ShowStatus(prm[0]);
            break;
        case 10:
            if(count==1)
                rez=mainwindow->ShowSysMenu(prm[0]);
            break;
        case 11:
            rez=(GetMenu(mainwindow->HWindow)!=NULL);
            break;
        case 20:
            if(count==1)
            {
                if(prm[0])
                {
                    mainwindow->ShowControl(1);
                    mainwindow->ShowStatus(2);
                    mainwindow->ShowSysMenu(1);
                }
                else
                {
                    mainwindow->ShowControl(0);
                    mainwindow->ShowStatus(0);
                    mainwindow->ShowSysMenu(0);
                }
                rez=1;
            }
            break;
        case 21:
            if(count==2)
            {
                UINT32 u=prm[0];
                rez=Object2dDialog(mainwindow->HWindow, (HSP2D)u, (HOBJ2D)prm[1], 0, 0);
            }
            break;
        case 22:
            if(count==2)
            {
                UINT32 u=prm[0];
                rez=Object3dDialog(mainwindow->HWindow, (HSP3D)u, (HOBJ2D)prm[1]);
            }
            break;
        case 256:
            if(count>0)
            { // информация о базах

#ifdef DBE

                rez=DBSys(prm[0], count>1 ? prm[1] : 0);
#endif

            }
            break;
    } ;
    if(prm)
        delete prm;
    PUSH_DOUBLE(rez);
} ;

void systemstr()
{
    int code = POP_DOUBLE;

    char *s=GetTmpString();
    switch(code)
    {
        default:
            lstrcpy(s, "");
    }
    PUSH_PTR(s);
}

void getclassdir()
{
    char *chr=POP_PTR;
    char *s=GetTmpString();
    PClassListInfo pcli;
    if(lstrlen(chr))
        pcli=GetClassListInfo(chr);
    else
        pcli=pobject->_class->GetClassListInfo();
    if(pcli)
    {
        TLibrary *lib=pcli->library;
        if(pcli->flags&CLF_PACKFILE)
        {
            while(lib&&(lib->flags&CLF_PACKFILE))
                lib=lib->parent;
            lib->GetPath(s);
        }
        lib->GetPath(s);
    }
    else
        lstrcpy(s, "");
    PUSH_PTR(s);
} ;

void getprojectdir()
{
    char *s=GetTmpString();
    GetProjectPath(s);
    PUSH_PTR(s);
} ;

void getwindowsdir()
{
    char *s=GetTmpString();
    GetWindowsDirectory(s, 255);
    PUSH_PTR(s);
} ;

void getsystemdir()
{
    char *s=GetTmpString();
    GetSystemDirectory(s, 255);
    PUSH_PTR(s);
} ;

void getdirfromfile()
{
    char *chr=POP_PTR;
    char *s=GetTmpString();
    GetPath(chr, s);
    PUSH_PTR(s);
} ;

void addslash()
{
    char *chr=POP_PTR;
    char *s=GetTmpString();
    lstrcpy(s, chr);
    AddSlash(s);
    PUSH_PTR(s);
} ;

void getstratumdir(){PUSH_PTR(sc_path);};

void winexecute()
{
    int fuCmdShow=POP_DOUBLE;
    char *s=POP_PTR;
    int rez = WinExec(s, fuCmdShow);
    PUSH_DOUBLE(rez);
} ;

void vm_winshell()
{
    int fuCmdShow=POP_DOUBLE;
    char *dir=POP_PTR;
    char *param=POP_PTR;
    char *cmd = POP_PTR;

    int hi=(int)ShellExecute(mainwindow->HWindow, NULL, // address of string for operation to perform
                             cmd,                       // address of string for filename
                             param,                     // address of string for executable-file parameters
                             dir,                       // address of string for default directory
                             fuCmdShow                  // whether file is shown when opened
                             );
    PUSH_DOUBLE(hi>32);
}


//--------------------------------------------- Message

void message2()
{
    INT16 count=*((INT16 *)codepoint);
    codepoint+=1;

    char ** vars=spp;
    spp+=count;
    char *type=POP_PTR;
    char *target=POP_PTR;

    if(Push_Context())
    {
        char _target[MAX_STRING];
        char filter[MAX_STRING];
        char *_filter=NULL;
        //обработка фильтра

        if(target&&*target)
        {
            int l=lstrlen(target);
            if(l>0)
            {
                l--;
                char *ps=target+l;
                BOOL was=FALSE;
                char *pos=target;
                while(ps&&ps>=target)
                {
                    if(((*ps)=='*')||((*ps)=='?'))
                        was=TRUE;

                    if(*ps=='\\')
                    {
                        pos=ps+1;
                        goto m1;
                    }
                    ps--;
                }
m1:
                if(was)
                {
                    l=(int)(pos-target);
                    if(l>0)
                    {
                        Lstrcpyn(_target, target, l+1);
                        lstrcat(_target, "*");
                        lstrcpy(filter, pos);
                        target=_target;
                        _filter=filter;
                    }
                }
            } // l>1
        }
        pobject->SendMessage2(type, target, vars, count/2, _filter);
    }
    Pop_Context();
    if(VM_ERROR)
        codepoint=&vm_nullcode;
}

void v_quit()
{
    int flags = POP_DOUBLE;
    SystemQuit(flags);
}

void registerobject()
{
    UINT32 flags=POP_DOUBLE;
    INT16 code=POP_DOUBLE;
    
    char* path=POP_PTR;
    HOBJ2D obj=POP_HOBJ;
    
    HSP2D hsp=POP_HSP;

    winmanager->RegisterObject(SCGetObject(path), hsp, obj, code, flags);
}

void unregisterobject()
{
    INT16 code=POP_DOUBLE;
    char *path=POP_PTR;
    HSP2D hsp=POP_HSP;
    winmanager->UnRegisterObject(SCGetObject(path), hsp, code);
}

void registerobjects()
{
    UINT32 flags=POP_DOUBLE;
    INT16 code=POP_DOUBLE;
    char *path=POP_PTR;
    HOBJ2D obj=POP_HOBJ;
    char *hsp=POP_PTR;
    winmanager->RegisterObject(SCGetObject(path), hsp, obj, code, flags);
}

void unregisterobjects()
{
    INT16 code=POP_DOUBLE;
    char *path=POP_PTR;
    char *hsp=POP_PTR;
    winmanager->UnRegisterObject(SCGetObject(path), hsp, code);
}

void setcapture()
{
    int code=POP_DOUBLE;
    char *path=POP_PTR;
    HSP2D hsp=POP_HSP;

    winmanager->setcapture(SCGetObject(path), hsp, code);
}

void releasecapture()
{
    winmanager->releasecapture();
}

void addgroupitem2d()
{
    HOBJ2D Object2d=POP_HOBJ;
    HOBJ2D group2d=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    int r =AddGroupItem2d(hsp, group2d, Object2d);
    PUSH_DOUBLE(r);
}

void creategroup2d()
{
    INT16 size=*((INT16 *)codepoint);
    codepoint+=1;
    HOBJ2D *p=NULL;
    if(size)
    {
        p=new HOBJ2D[size];
        INT16 i=(INT16)(size);
        while(i--)
        {
            p[i]=POP_HOBJ;
        }
    }
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(CreateGroup2d(hsp, p, (INT16)(size)));
    if(p)
        delete p;
}

void vm_setcrdsystem()
{
    int matrix=POP_DOUBLE;
    int type=POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(matrixs->_SetCrdSystem(hsp, type, matrix));
}

void deletegroup2d()
{
    HOBJ2D group2d=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    BOOL r = DeleteGroup2d(hsp, group2d);
    PUSH_DOUBLE(r);
}

void delgroupitem2d()
{
    HOBJ2D Item=POP_HOBJ;
    HOBJ2D group2d=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    BOOL r=DelGroupItem2d(hsp, group2d, Item);
    PUSH_DOUBLE(r);
}

void getgroupitem2d()
{
    INT16 ItemNum=POP_DOUBLE;
    HOBJ2D group2d=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(GetGroupItem2d(hsp, group2d, ItemNum));
}

void getgroupitems2d()
{
    // int GetGroupItems2d( HSP2D hSpace2d, int group2d, int * Object2d,int MaxObjects );
}

void getgroupitemsnum2d()
{
    HOBJ2D group2d=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    INT16 r=GetGroupItemsNum2d(hsp, group2d);
    PUSH_DOUBLE(r);
}

void isgroupcontainobject2d()
{
    HOBJ2D Object2d=POP_HOBJ;
    HOBJ2D group2d=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    INT16 r=IsGroupContainObject2d(hsp, group2d, Object2d);
    PUSH_DOUBLE(r);
}

void setgroupitem2d()
{
    HOBJ2D Object2d=POP_HOBJ;
    INT16 ItemNum=POP_DOUBLE;
    HOBJ2D group2d=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    BOOL r=SetGroupItem2d(hsp, group2d, ItemNum, Object2d);
    PUSH_DOUBLE(r);
}

void copytoclipboard2d()
{
    HOBJ2D Object2d=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    BOOL r=CopyToClipboard2d(hsp, Object2d);
    PUSH_DOUBLE(r);
}

void pastefromclipboard2d()
{
    INT16 flags=POP_DOUBLE;
    POINT2D p;
    p.y=POP_DOUBLE;
    p.x=POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(PasteFromClipboard2d(hsp, &p, flags));
}

void SetGroupItems2d()
{
    INT16 size=*((INT16 *)codepoint);
    codepoint+=1;

    HOBJ2D *p=new HOBJ2D[size];

    INT16 i=(INT16)(size);
    while(i--)
    { p[i]=POP_HOBJ;}

    HOBJ2D hgroup=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetGroupItems2d(hsp, hgroup, p, (INT16)(size)));
    delete p;
}

void setstatustext()
{
    char *str=POP_PTR;
    int id=POP_DOUBLE;
    SCSetStatusText(id, str);
}

void mcisendstring()
{
    char *cmd = POP_PTR;
    char *ret = GetTmpString();
    V_mcisendstring(cmd, ret, 0);
    PUSH_PTR(ret);
}

void mcisendstring_int()
{
    char *cmd=POP_PTR;
    double r=V_mcisendstring(cmd, NULL, 0);
    PUSH_DOUBLE(r);
}

void mcisendstringex()
{
    int num=POP_DOUBLE;
    char *cmd=POP_PTR;
    char *ret=GetTmpString();
    V_mcisendstring(cmd, ret, num);
    PUSH_PTR(ret);
}

void getlastmcierror()
{
    double r=GetLastMCIrez();
    PUSH_DOUBLE(r);
}

void getmcierror()
{
    double r=POP_DOUBLE;
    char *error=GetTmpString();
    _mcigeterrorestring(r, error);
    PUSH_PTR(error);
}

//--------------------------------------------- Auto Sintez
void vm_addchildren()
{
    gr_float y=(int)POP_DOUBLE;
    gr_float x=(int)POP_DOUBLE;
    char *alias=POP_PTR;
    char *name=POP_PTR;
    char *parent=POP_PTR;
    PUSH_HOBJ(Vm_AddChildren(parent, name, alias, x, y));
}

void vm_removechildren()
{
    BOOL noquestion=POP_DOUBLE;
    HOBJ2D handle=POP_HOBJ;
    char *parent=POP_PTR;
    double r=Vm_RemoveChildren(parent, handle, !noquestion, pobject);
    UPDATE_VARS();
    PUSH_DOUBLE(r);
}

void vm_createclass()
{
    int flags=(int)POP_DOUBLE;
    char *_template=POP_PTR;
    char *newclass=POP_PTR;

    PUSH_DOUBLE(Vm_CreateClass(newclass, _template, flags));
}

void getchildcount()
{
    char *name=POP_PTR;
    PClass _class=GetClassByName(name);
    int c=0;
    if(_class)
    { c=_class->childcount;}
    PUSH_DOUBLE(c);
}

void getchildidbynum()
{
    int num=POP_DOUBLE;
    char *name=POP_PTR;
    PClass _class=GetClassByName(name);
    HOBJ2D h=0;
    if(_class&&num>=0&&num<_class->childcount)
        h=_class->childs[num].handle;
    PUSH_HOBJ(h)
}

void getchildclass()
{
    HOBJ2D h=POP_HOBJ;
    char *name=POP_PTR;
    PClass _class=GetClassByName(name);
    char *str=GetTmpString();
    char *_s="";
    *str=0;
    if(_class)
    {
        int num=_class->GetChildById(h);
        if(num>-1)
            _s=_class->childs[num].type->GetClassName();
    }
    lstrcpy(str, _s);
    PUSH_PTR(str);
}

void vm_deleteclass()
{
    char *classname=POP_PTR;
    Vm_DeleteClass(classname);
}

void vm_openclassscheme()
{
    int flags=(int)POP_DOUBLE;
    char *classname=POP_PTR;
    PUSH_HSP(Vm_OpenClassScheme(classname, flags));
}

void vm_closeclassscheme()
{
    char *classname=POP_PTR;
    PUSH_DOUBLE(Vm_CloseClassScheme(classname));
}

void vm_createlink()
{
    HOBJ2D id2=POP_HOBJ;
    HOBJ2D id1=POP_HOBJ;
    char *parent=POP_PTR;
    PUSH_HOBJ(Vm_CreateLink(parent, id1, id2));
}

void vm_getlink()
{
    HOBJ2D id2=POP_HOBJ;
    HOBJ2D id1=POP_HOBJ;
    char *classname=POP_PTR;
    HOBJ2D hl=0;
    PClass _class=GetClassByName(classname);
    if(_class)
    {
        PLinkInfo pl=_class->GetLinkByHH(id1, id2);
        if(pl)
            hl=pl->handle;
    }
    PUSH_HOBJ(hl);
}

void vm_createlink2()
{
    char *id2=POP_PTR;
    char *id1=POP_PTR;
    char *parent=POP_PTR;
    PObject h1=SCGetObject(id1);
    PObject h2=SCGetObject(id2);
    if(h1&&h2)
    {
        HOBJ2D hh1=GetObjectHandle(h1);
        HOBJ2D hh2=GetObjectHandle(h2);
        if(hh1*hh2)
        {
            HOBJ2D ret=Vm_CreateLink(parent, hh1, hh2);
            PUSH_HOBJ(ret);
            return;
        }
    }
    PUSH_HOBJ(0);
}

int setModelText(int h, char* className, bool showErrorDialog)
{
    PClass _class=GetClassByName(className);
    if(!_class)
        return 0;

    BOOL k=0;
    char *txt=new char[MAX_CLASSTEXTSIZE];
    if(! streams->GetAllText(h, txt))
    {
        goto m1;
    }
    if(CanModifyClass(_class))
    {
        k = SetClassText(_class, txt, showErrorDialog);
        UPDATE_VARS();
    }
    else
    {
        char s[MAX_STRING]="Limitation ";
        AddObjectName(s);
        _Message(s);
    }
m1:
    delete txt;
    return k;
}

void vm_setmodeltext()
{
    int h = POP_LONGi;
    char* className = POP_PTR;
    PUSH_DOUBLE(setModelText(h, className, true));
}

void vm_SetModelTextWithoutError()
{
    bool showErrorDialog = (POP_DOUBLE)>0;
    int h = POP_LONGi;
    char *className = POP_PTR;
    PUSH_DOUBLE(setModelText(h, className, showErrorDialog));
}


void vm_getmodeltext()
{
    int h=POP_LONGi;
    char *ClassName=POP_PTR;

    PClass _class=GetClassByName(ClassName);
    if(!_class)
    {
        PUSH_DOUBLE(0);
        return;
    }
    char *text=GetClassText(_class);
    int l;
    if(text)
    {
        l=lstrlen(text);
        streams->Write(h, text, l);
        delete text;
    }
    PUSH_DOUBLE(l);
}

void vm_setlinkvars()
{
    char *text=POP_PTR;
    HOBJ2D id=POP_HOBJ;
    char *parent=POP_PTR;
    double r=Vm_SetLinkVars(parent, id, text);
    UPDATE_VARS();
    PUSH_DOUBLE(r);
}

void vm_removelink()
{
    HOBJ2D id1=POP_HOBJ;
    char *parent=POP_PTR;
    double r=Vm_RemoveLink(parent, id1);
    UPDATE_VARS();
    PUSH_DOUBLE(r);
}

void vm_getuniqueclassname()
{
    char *n2=GetTmpString();
    char *n1=POP_PTR;
    GetUniqueClassName(n1, n2);
    PUSH_PTR(n2);
}

void vm_inputbox()
{
    char *n3=POP_PTR;
    char *n2=POP_PTR;
    char *n1=POP_PTR;
    char *re=GetTmpString();
    lstrcpy(re, n3);
    winmanager->InputBox(n1, n2, re);
    PUSH_PTR(re);
}

void vm_getschemeobject()
{
    char *path=POP_PTR;
    PObject po=SCGetObject(path);
    HOBJ2D h=0;
    if(po)
    { h=GetObjectHandle(po);}
    PUSH_HOBJ(h);
}

void v_getclass()
{
    char * path = POP_PTR;
#ifdef LOGON
    LogMessage("v_getclass 1");
#endif
    PObject po=SCGetObject(path);
#ifdef LOGON
    LogMessage("v_getclass 2");
#endif
    char * name = GetTmpString();
#ifdef LOGON
    LogMessage("v_getclass 3");
#endif
    if(po)
    { lstrcpy(name, po->_class->name);}
    else
    { lstrcpy(name, "");}
#ifdef LOGON
    LogMessage("v_getclass 4");
#endif
    PUSH_PTR(name);
}

void v_getvarf()
{
    char *VarName=POP_PTR;
    char *ObjName=POP_PTR;
    double d=0;
    PObject po=SCGetObject(ObjName);
    if(po)
    { d=po->GetVarF(VarName, 0);}
    PUSH_DOUBLE(d);
}

void v_getvars()
{
    char *VarName=POP_PTR;
    char *ObjName=POP_PTR;
    PObject po=SCGetObject(ObjName);
    char *s="";
    if(po)
    { s=po->GetVarS(VarName, "");}
    PUSH_PTR(s);
}

void v_getvarh()
{
    char *VarName=POP_PTR;
    char *ObjName=POP_PTR;
    long h=0;
    PObject po=SCGetObject(ObjName);
    if(po)
    { h=po->GetVarH(VarName, 0);}
    PUSH_LONG(h);
}

void v_setvarf()
{
    double d=POP_DOUBLE;
    char *VarName=POP_PTR;
    char *ObjName=POP_PTR;
    PObject po=SCGetObject(ObjName);
    if(po)
    { po->SetVarF(VarName, d);}
}

void v_setvarh()
{
    long h=POP_LONG;
    char *VarName=POP_PTR;
    char *ObjName=POP_PTR;
    PObject po=SCGetObject(ObjName);
    if(po)
    { po->SetVarH(VarName, h);}
}

void v_setvars()
{
    char *s=POP_PTR;
    char *VarName=POP_PTR;
    char *ObjName=POP_PTR;
    PObject po=SCGetObject(ObjName);
    if(po)
    { po->SetVarS(VarName, s);}
}

//#include "dialogs.h"

void vm_scmessage()
{
    //  SysInfoDialog(mainwindow->HWindow);
    _Message(POP_PTR);
}

//int DBGetFId(int table,char*field);
//int DBGetFiled(int table,int id,char*field);

void vm_setcurrentobj2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetCurrentObject2d(hsp, obj));
} ;

void vm_getcurrentobj2d()
{
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(GetCurrentObject2d(hsp));
} ;

#ifdef DBE

void db_setwintable()
{
    char *s=POP_PTR;
    int table=POP_LONGi;
    char *w=POP_PTR;
    int r=winmanager->SetTable(w, table, s);
    PUSH_DOUBLE(r);
} ;

void db_setcontroltable()
{
    char *s=POP_PTR;
    int table=POP_LONGi;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp2d=POP_HSP;
    int r=winmanager->SetTable(hsp2d, obj, table, s);
    PUSH_DOUBLE(r);
} ;

void db_openbase()
{
    char *flags=POP_PTR;
    char *password=POP_PTR;
    char *type=POP_PTR;
    char *name=POP_PTR;
    int r=DBOpenDataBase(name, type, password, flags);
    PUSH_LONG(r);
} ;

void db_packtable(){PUSH_DOUBLE(DbPackTable(POP_LONGi));};

void db_sorttable()
{
    INT16 size=*((INT16 *)codepoint);
    codepoint+=1;
    char ** p=NULL;
    if(size)
    {
        p=(char ** )new UINT32[size];
        INT16 i=(INT16)(size);
        while(i--)
        { p[i]=POP_PTR;}
    }
    UINT16 flags=POP_DOUBLE;
    int base=POP_LONGi;
    PUSH_DOUBLE(DBSort(base, flags, size, p));
    delete p;
} ;

void db_opentable()
{
    char *flags=POP_PTR;
    UINT indexId=POP_DOUBLE;
    char *index=POP_PTR;
    char *indextag=POP_PTR;
    char *type=POP_PTR;
    char *name=POP_PTR;
    int base=POP_LONGi;
    int r=DBOpenTable(base, name, type, index, indextag, indexId, flags);
    PUSH_LONG(r);
} ;

void db_closebase()
{
    int base=POP_LONGi;
    int r=DBCloseBase(base);
    PUSH_DOUBLE(r);
} ;

void db_recno()
{
    int table=POP_LONGi;
    int r=DBRecno(table);
    PUSH_DOUBLE(r);
} ;

void db_closetable()
{
    int table=POP_LONGi;
    int r=DBCloseTable(table);
    PUSH_DOUBLE(r);
} ;

void db_geterror()
{
    int r=DBGetLastError();
    PUSH_DOUBLE(r);
} ;

void db_geterrorstr()
{
    int code=POP_DOUBLE;
    char *s=GetTmpString();
    DBGetTextError(code, s);
    PUSH_PTR(s);
} ;

void db_setdir()
{
    char *path=POP_PTR;
    int base=POP_LONGi;
    int r=DBSetDir(base, path);
    PUSH_DOUBLE(r);
} ;

void db_gotop()
{
    int table=POP_LONGi;
    int r=DBGoTop(table);
    PUSH_DOUBLE(r);
} ;

void db_gobottom()
{
    int table=POP_LONGi;
    int r=DBGoBottom(table);
    PUSH_DOUBLE(r);
} ;

void db_skip()
{
    long delta=POP_DOUBLE;
    int table=POP_LONGi;
    int r=DBSkip(table, delta);
    PUSH_DOUBLE(r);
} ;

void db_getFid()
{
    char *f=POP_PTR;
    int table=POP_LONGi;
    int r=DBGetFId(table, f);
    PUSH_DOUBLE(r);
} ;

void db_select()
{
    int type=POP_DOUBLE;
    char *s=POP_PTR;
    int base=POP_LONGi;
    int table=DBSQL(base, s, type);
    PUSH_LONG(table);
}

void db_select_h()
{
    int type=POP_DOUBLE;
    int stream=POP_LONGi;
    int base=POP_LONGi;
    int table=0;
    if(streams->GetStream(stream))
    {
        char* s = new char[MAX_CLASSTEXTSIZE];
        if(streams->GetAllText(stream,s))
        {
            table = DBSQL(base, s, type);
        }
        delete s;
    }
    PUSH_LONG(table);
}

void db_getcount()
{
    int table=POP_LONGi;
    double r=DBGetCount(table);
    PUSH_DOUBLE(r);
}

void db_browse()
{
    char *flags=POP_PTR;
    int table=POP_LONGi;
    char *win=POP_PTR;
    double r=winmanager->OpenBrowseWindow(win, table, flags);
    PUSH_DOUBLE(r);
} ;

void db_writeblob()
{
    int stream=POP_LONGi;
    INT16 ID=POP_DOUBLE;
    int table=POP_LONGi;
    PUSH_DOUBLE(DBWriteBlob(table, ID, stream));
} ;

void db_readblob()
{
    int stream=POP_LONGi;
    INT16 ID=POP_DOUBLE;
    int table=POP_LONGi;
    PUSH_DOUBLE(DBReadBlob(table, ID, stream));
} ;

void db_freeblob()
{
    INT16 id=POP_DOUBLE;
    int table=POP_LONGi;
    PUSH_DOUBLE(DBFreeBlob(table, id));
} ;

void db_getdouble()
{
    INT16 ID=POP_DOUBLE;
    int table=POP_LONGi;
    double r;
    DBGetField(table, ID, &r);
    PUSH_DOUBLE(r);
} ;

void db_getfstr()
{
    INT16 ID=POP_DOUBLE;
    int table=POP_LONGi;
    char *s=GetTmpString();
    DBGetField(table, ID, s);
    PUSH_PTR(s);
} ;

void db_Sgetdouble()
{
    char *Fn=POP_PTR;
    int table=POP_LONGi;
    double r;
    DBGetField(table, Fn, &r);
    PUSH_DOUBLE(r);
} ;

void db_Sgetfstr()
{
    char *Fn=POP_PTR;
    int table=POP_LONGi;
    char *s=GetTmpString();
    DBGetField(table, Fn, s);
    PUSH_PTR(s);
} ;

void db_setfield()
{
    char *s=POP_PTR;
    INT16 ID=POP_DOUBLE;
    int table=POP_LONGi;
    double r=DBSetField(table, ID, s);
    PUSH_DOUBLE(r);
} ;

void db_setfieldnum()
{
    double r=POP_DOUBLE;
    INT16 ID=POP_DOUBLE;
    int table=POP_LONGi;
    r=DBSetField(table, ID, r);
    PUSH_DOUBLE(r);
} ;

void db_Ssetfield()
{
    char *s=POP_PTR;
    char *Fn=POP_PTR;
    int table=POP_LONGi;
    double r=DBSetField(table, Fn, s);
    PUSH_DOUBLE(r);
} ;

void db_Ssetfieldnum()
{
    double r=POP_DOUBLE;
    char *Fn=POP_PTR;
    int table=POP_LONGi;
    r=DBSetField(table, Fn, r);
    PUSH_DOUBLE(r);
} ;

void db_getfname()
{
    INT16 ID=POP_DOUBLE;
    int table=POP_LONGi;
    char *s=GetTmpString();
    DBFieldName(table, ID, s);
    PUSH_PTR(s);
}

void db_getftype()
{
    INT16 ID=POP_DOUBLE;
    int table=POP_LONGi;
    double r=DBFieldType(table, ID);
    PUSH_DOUBLE(r);
}

void db_getfcount()
{
    int table=POP_LONGi;
    double r=DBFieldCount(table);
    PUSH_DOUBLE(r);
} ;

void db_insertrecord()
{
    int table=POP_LONGi;
    double r=DBInsertRecord(table);
    PUSH_DOUBLE(r);
} ;

void db_appendrecord()
{
    int table=POP_LONGi;
    double r=DBAppendRecord(table);
    PUSH_DOUBLE(r);
} ;

void db_deleterecord()
{
    int table=POP_LONGi;
    double r=DBDeleteRecord(table);
    PUSH_DOUBLE(r);
} ;

void db_createtable()
{
    int Overwrite=POP_DOUBLE;
    char *desc=POP_PTR;
    char *driver=POP_PTR;
    char *name=POP_PTR;
    int base=POP_LONGi;
    int h=DBCreateTable(base, name, driver, desc, Overwrite);
    PUSH_DOUBLE(h);
} ;

void db_zap()
{
    double r=DBZap(POP_LONGi);
    PUSH_DOUBLE(r);
} ;

void db_closeall(){DBCloseAll(GetCurrentProject());}

// ----------------Created by AK{
void db_addindex()
{
    char ** Fields=NULL;
    INT16 size=*((INT16 *)codepoint);
    codepoint+=1;
    if(size)
    {
        Fields=(char ** )new UINT32[size];
        INT16 i=(INT16)(size);
        while(i--)
        { Fields[i]=POP_PTR;}
    }
    char *keycond=POP_PTR;
    char *keyexpr=POP_PTR;
    UINT16 keytype=POP_DOUBLE;
    UINT16 Props=POP_DOUBLE;
    char *tag=POP_PTR;
    UINT16 IndexId=POP_DOUBLE;
    char *name=POP_PTR;
    int table=POP_LONGi;
    double r=DBAddIndex(table, name, IndexId, tag, Props, Fields, size, keytype, keyexpr, keycond);
    if(Fields)
        delete Fields;
    PUSH_DOUBLE(r);
} ;

void db_deleteindex()
{
    char *tag=POP_PTR;
    UINT16 IndexId=POP_DOUBLE;
    char *name=POP_PTR;
    int table=POP_LONGi;
    double r=DBDeleteIndex(table, name, IndexId, tag);
    PUSH_DOUBLE(r);
}

void db_openindex()
{
    UINT16 IndexId=POP_DOUBLE;
    char *name=POP_PTR;
    int table=POP_LONGi;
    double r=DBOpenIndex(table, name, IndexId);
    PUSH_DOUBLE(r);
} ;

void db_switchtoindex()
{
    BOOL bCurrRec=POP_DOUBLE;
    char *tag=POP_PTR;
    UINT16 IndexId=POP_DOUBLE;
    char *name=POP_PTR;
    int table=POP_LONGi;
    double r=DBSwitchToIndex(table, name, IndexId, tag, bCurrRec);
    PUSH_DOUBLE(r);
} ;

void db_closeindex()
{
    char *name=POP_PTR;
    int table=POP_LONGi;
    double r=DBCloseIndex(table, name);
    PUSH_DOUBLE(r);
} ;

void db_regenindex()
{
    char *tag=POP_PTR;
    UINT16 IndexId=POP_DOUBLE;
    char *name=POP_PTR;
    int table=POP_LONGi;
    double r=DBRegenIndex(table, name, tag, IndexId);
    PUSH_DOUBLE(r);
} ;

void db_getdelmode()
{
    int table=POP_LONGi;
    PUSH_DOUBLE(DBGetDeleteMode(table));
} ;

void db_settokey()
{
    char *key=POP_PTR;
    int table=POP_LONGi;
    PUSH_DOUBLE(DBSetToKey(table, key));
} ;

void db_setdelmode()
{
    BOOL mode=POP_DOUBLE;
    int table=POP_LONGi;
    PUSH_DOUBLE(DBSetDeleteMode(table, mode));
} ;

void db_setcodepage()
{
    UINT16 code=POP_DOUBLE;
    int table=POP_LONGi;
    code=DBSetCodePage(table, code);
    PUSH_DOUBLE(code);
} ;

void db_getcodepage()
{
    int table=POP_LONGi;
    UINT16 code=DBGetCodePage(table);
    PUSH_DOUBLE(code);
} ;

void db_lock()
{
    BYTE m=POP_DOUBLE;
    int table=POP_LONGi;
    PUSH_DOUBLE(DBLock(table, m));
} ;

void db_unlock()
{
    BYTE m=POP_DOUBLE;
    int table=POP_LONGi;
    PUSH_DOUBLE(DBUnLock(table, m));
} ;

void db_undelete()
{
    int table=POP_LONGi;
    PUSH_DOUBLE(DBUndeleteRecord(table));
} ;

void db_copyto()
{
    char *type=POP_PTR;
    char *dst=POP_PTR;
    int table=POP_LONGi;
    PUSH_DOUBLE(DBCopyTo(table, dst, type));
}

void vm_bmdestub()
{
    BOOL
            static mayload=1;
    if(mayload&&DBInitSQL())
    {
        commands[DB_OPENBASE]=db_openbase;
        commands[DB_OPENTABLE]=db_opentable;
        commands[DB_CLOSEBASE]=db_closebase;
        commands[DB_CLOSETABLE]=db_closetable;
        commands[DB_GETERROR]=db_geterror;
        commands[DB_GETERRORSTR]=db_geterrorstr;
        commands[DB_SETDIR]=db_setdir;
        commands[DB_CLOSEALLBASES]=db_closeall;
        commands[DB_GOTOP]=db_gotop;
        commands[DB_GOBOTTOM]=db_gobottom;
        commands[DB_SKIP]=db_skip;
        commands[DB_GETFID]=db_getFid;
        commands[DB_GETFSTR]=db_getfstr;
        commands[DB_SELECT]=db_select;
        commands[VM_DBSQLH]=db_select_h;
        commands[DB_GETCOUNT]=db_getcount;
        commands[DB_BROWSE]=db_browse;

        commands[DB_GETDOUBLE]=db_getdouble;
        commands[DB_GETFNAME]=db_getfname;
        commands[DB_GETFTYPE]=db_getftype;
        commands[DB_GETFCOUNT]=db_getfcount;

        commands[DB_INSERTRECORD]=db_insertrecord;
        commands[DB_APPENDRECORD]=db_appendrecord;
        commands[DB_DELETERECORD]=db_deleterecord;

        commands[DB_ADDINDEX]=db_addindex;           // by AK
        commands[DB_DELETEINDEX]=db_deleteindex;     // by AK
        commands[DB_OPENINDEX]=db_openindex;         // by AK
        commands[DB_SWITCHTOINDEX]=db_switchtoindex; // by AK
        commands[DB_CLOSEINDEX]=db_closeindex;       // by AK
        commands[DB_REGENINDEX]=db_regenindex;       // by AK

        commands[DB_SSETFIELD]=db_Ssetfield;
        commands[DB_SSETFIELDNUM]=db_Ssetfieldnum;
        commands[DB_SGETDOUBLE]=db_Sgetdouble;
        commands[DB_SGETFSTR]=db_Sgetfstr;

        commands[DB_CREATETABLE]=db_createtable;
        commands[DB_ZAP]=db_zap;

        commands[DB_SETWINTABLE]=db_setwintable;
        commands[DB_SETCONTROLTABLE]=db_setcontroltable;

        commands[DB_READBLOB]=db_readblob;
        commands[DB_WRITEBLOB]=db_writeblob;
        commands[DB_FREEBLOB]=db_freeblob;
        commands[DB_PACKTABLE]=db_packtable;
        commands[DB_SORTTABLE]=db_sorttable;
        commands[DB_GETDELMODE]=db_getdelmode;
        commands[DB_SETDELMODE]=db_setdelmode;
        commands[DB_SETCODEPAGE]=db_setcodepage;
        commands[DB_GETCODEPAGE]=db_getcodepage;
        commands[DB_LOCK]=db_lock;
        commands[DB_UNLOCK]=db_unlock;
        commands[DB_UNDELETE]=db_undelete;
        commands[DB_COPYTO]=db_copyto;
        commands[DB_SETTOKEY]=db_settokey;
        commands[DB_RECNO]=db_recno;
    }
    else
    {
        mayload=0;
        codepoint=&vm_nullcode;
        return;
    }
    codepoint--;
} ;
#else

void vm_bmdestub()
{
    _Error(IDS_str226);
    codepoint=&vm_nullcode;
    return;
}
#endif

//-------------------------------------

// Создание массива
void dm_new()
{
    dyno_t h = dyno_mem->NewVar();
    PUSH_LONG(h);
}

// Удаление массива
void dm_delete(){dyno_mem->DeleteVar(POP_LONG);}

//Удаление элементов массива
void dm_clearall()
{
    dyno_mem->Done(GetCurrentProject());
}

//Вставка элемента
void dm_vinsert()
{
    char *type = POP_PTR;
    C_TYPE i = dyno_mem->InsertInVar(POP_LONG, type);
    PUSH_DOUBLE(i);
}

//Удаление элемента
void dm_vdelete()
{
    INT16 k=POP_DOUBLE;
    BOOL i=dyno_mem->DeleteInVar(POP_LONG, k);
    PUSH_DOUBLE(i);
}

//Получение кол-ва элементов массива
void dm_vgetcount()
{
    long h=POP_LONG;
    PUSH_DOUBLE(dyno_mem->GetCount(h));
}

//получение типа элемента
void dm_gettype()
{
    char *s=GetTmpString();
    C_TYPE i=POP_DOUBLE;
    char *t=dyno_mem->GetType(POP_LONG, i);
    if(t)
    { lstrcpy(s, t);}
    else
        lstrcpy(s, 0);
    PUSH_PTR(s);
}

//установка числа
void dm_vsetf()
{
    double data = POP_DOUBLE;
    char* name = POP_PTR;
    C_TYPE i = POP_DOUBLE;
    dyno_mem->SetVar((dyno_t)POP_LONG, i, name, 0, &data);
}

//установка хэндла
void dm_vseth()
{
    long data=POP_LONG;
    char *name=POP_PTR;
    C_TYPE i=POP_DOUBLE;
    dyno_mem->SetVar((dyno_t)POP_LONG, i, name, 1, &data);
}

//установка строки
void dm_vsets()
{
    char *data=POP_PTR;
    char *name=POP_PTR;
    C_TYPE i=POP_DOUBLE;
    dyno_mem->SetVar((dyno_t)POP_LONG, i, name, 3, data);
}

//Получение числа
void dm_vgetf()
{
    double data;
    char *name = POP_PTR;
    C_TYPE i = POP_DOUBLE;
    dyno_mem->SetVar((dyno_t)POP_LONG, i, name, 0x80|0, &data);
    PUSH_DOUBLE(data);
}

//Получение хэндла
void dm_vgeth()
{
    long h;
    char *name=POP_PTR;
    C_TYPE i=POP_DOUBLE;
    dyno_mem->SetVar((dyno_t)POP_LONG, i, name, 0x80|1, &h);
    PUSH_LONG(h);
}

//Получение строки
void dm_vgets()
{
    char *s=GetTmpString();
    char *name=POP_PTR;
    C_TYPE i=POP_DOUBLE;
    dyno_mem->SetVar((dyno_t)POP_LONG, i, name, 0x80|3, s);
    PUSH_PTR(s);
}

//Сохранение
void dm_save()
{
    int src=POP_LONGi;
    dyno_t m = (dyno_t)POP_LONG;

    TStream *_s = streams->GetStream(src);

    BOOL rez = dyno_mem->Store(m, _s);

    PUSH_DOUBLE(rez);
}

//Загрузка
void dm_load()
{
    int src=POP_LONGi;
    TStream *_s=streams->GetStream(src);
    dyno_t h=dyno_mem->Load(_s);
    PUSH_LONG(h);
}
// ------------------ X-Files !!!!!!!!!

void vm_createdir()
{
    char *path=POP_PTR;
    PUSH_DOUBLE(MakeDirectory(path));
}

void vm_deletedir()
{
    char *path=POP_PTR;
    PUSH_DOUBLE(DeleteDirectory(path));
}

void vm_filerename()
{
    char *patch2=POP_PTR;
    char *patch1=POP_PTR;
    PUSH_DOUBLE(ScRenameFile(patch1, patch2));
}

void vm_filecopy()
{
    char *file2 = POP_PTR;
    char *file1 = POP_PTR;
    BOOL rez = ScCopyFile(file1, file2);
    PUSH_DOUBLE(rez);
}

void vm_fileexist()
{
    char *path = POP_PTR;
    char s[999];
    GetFullFileNameEx((TProject*)GetCurrentProject(), path, s);
    PUSH_DOUBLE(ScFileExist(s));
}

void vm_filedelete()
{
    char *path=POP_PTR;
    char s[MAX_STRING];
    GetFullFileNameEx((TProject *)GetCurrentProject(), path, s);
    PUSH_DOUBLE(ScDeleteFile(s));
}

void vm_getfilelist()
{
    int attr=POP_DOUBLE;
    char *filter=POP_PTR;
    PUSH_LONG(dyno_mem->DirectoryInfo(filter, attr));
} ;

// ------------------ Advanced functions !!!!!!!!!

extern TClassListInfo*_cliFLOAT;

extern TClassListInfo*_cliINTEGER;

extern TClassListInfo*_cliSTRING;

extern TClassListInfo*_cliWORD;

extern TClassListInfo*_cliBYTE;

extern TClassListInfo*_cliHANDLE;

extern TClassListInfo*_cliRGB;

//#define VF_RETURN 1

PClassListInfo __GetArgument()
{
    INT16 code=(INT16) *codepoint;
    if((code)<0)
    {
        codepoint++;
        switch(code){
            case -1:
                return _cliFLOAT;
            case -2:
                return _cliSTRING;
            case -3:
                return _cliHANDLE;
            case -4:
                return _cliRGB;
            default:
                return NULL;
        }
    }
    else
    {
        if(code)
        {
            char *pstr=((char *)(codepoint+1));
            codepoint+=( *codepoint)+2;
            return GetClassListInfo(pstr);
        }
        else
        {
            codepoint++;
            return NULL;
        }
    }
}

void VFunction()
{
    char *pstr=((char *)(codepoint+1));
    codepoint += (*codepoint)+1;

    PClassListInfo pcli = GetClassListInfo(pstr);
    if(pcli)
    {
        PClass _class = pcli->GetClass();

        PClassListInfo ret;
        INT16 count = *codepoint;
        codepoint++;

        if(count <= _class->varcount)
        {
            BYTE* buf;
            TVARARRY vars;
            if(_class->varcount)
            {
                buf = new BYTE[(int)_class->varsize];
                memset(buf, 0, _class->varsize);
                vars = new pointer[_class->varcount];
            }
            else
            {
                buf = NULL;
                vars = NULL;
            }
            BYTE* _buf = buf;
            INT16 retnum = -1;
            for (INT16 i = 0; i < _class->varcount; i++)
            {
                vars[i] = _buf;
                _buf += _class->vars[i].classType->GetClass()->varsize;
                if(_class->vars[i].flags & VF_RETURN)
                    retnum = i;
            }

            for (i = count-1; i>=0; i--)
            {
                INT16 code = *codepoint;
                PClassListInfo var=__GetArgument();
                if(var&&(_class->vars[i].classType!=var))
                {
                    Vm_Halt();
                    goto m1;
                }
                switch(code)
                {
                    case -1:
                        *((double *)(vars[i])) = POP_DOUBLE;
                        break;

                    case -2:
                        lstrcpy((char *)(vars[i]), POP_PTR);
                        break;

                    case -4:
                    case -3:
                        *((UINT32 *)(vars[i])) = POP_LONG;
                        break;

                    default:
                        memcpy(vars[i], spb, (int)_class->vars[i].classType->GetClass()->varsize);
                        spb+=_class->vars[i].classType->GetClass()->varsize;
                }
            }
            ret = __GetArgument();
            //    codepoint--;
            if(Push_Context())
            {
                invfunction=1;
                VM_Execute(_class->VM_code, vars, pobject);
            }
            Pop_Context();

            if(ret)
            {
                if(ret==_cliSTRING)
                {
                    char *_ret=GetTmpString();
                    lstrcpyn(_ret, (char *)(vars[retnum]), STRING_SIZE);
                    PUSH_PTR(_ret);
                }
                else
                {
                    int size=_class->vars[retnum].classType->GetClass()->varsize;
                    spb-=size;
                    memcpy(spb, vars[retnum], size);
                }
            }
m1:
            if(buf)
            {
                delete buf;
                delete vars;
            }
        }
    }
    else
    {
        char s[STRING_SIZE]="";
        wsprintf(s, "Class function %s not found (system stopped)", pstr);
        _Message(s);
        Vm_Halt();
    }
}

void DLLFunction()
{
    char *pstr=((char *)(codepoint+1));
    codepoint+=( *codepoint)+1;

    TDLLFunction* func = dllmanager->SearchFunction(pstr);
    if(func)
    {
        if(func->Call(&spb))
            return;
    }
    char s[STRING_SIZE];
    wsprintf(s, "Function %s not found (system stopped)", pstr);
    _Message(s);
    Vm_Halt();
}

void vm_loadproject()
{
    char* path = POP_PTR;
    TProject *_prj = project;
    project = NULL;
    if(LoadProject(path, FALSE))
    {
        PUSH_DOUBLE(1);}

    else
    {
        PUSH_DOUBLE(0);
    }
    project=_prj;
}

void vm_unloadproject()
{
    char *project=POP_PTR;
    PUSH_DOUBLE(CloseProject(GetProject(project)));
}

void vm_setactiveproject()
{
    char *project=POP_PTR;
    BOOL b=SetActiveProject(GetProject(project));
    PUSH_DOUBLE(b);
}

void vm_setprojectprop()
{
    double v=POP_DOUBLE;
    char *prop=POP_PTR;
    char *project=POP_PTR;
    TProject *p=GetProject(project);
    double b=p ? p->SetProjectProp(prop, v) : 0;
    PUSH_DOUBLE(b);
}

void vm_setprojectprops()
{
    char *v=POP_PTR;
    char *prop=POP_PTR;
    char *project=POP_PTR;
    TProject *p=GetProject(project);
    double b=p ? p->SetProjectProp(prop, v) : 0;
    PUSH_DOUBLE(b);
}

void vm_getprojectprop()
{
    char *prop = POP_PTR;
    char *project = POP_PTR;
    TProject *p = GetProject(project);
    double b=p ? p->GetProjectProp(prop) : 0;
    PUSH_DOUBLE(b);
}

void vm_isprojectexist()
{
    char *project=POP_PTR;
    PUSH_DOUBLE(GetProject(project)!=0);
}

void GetElement(){ }

void SetElement(){ }

// ------------------
// may be fast

void v_mul_f()
{
    *(spf+1)=(*(spf+1))*(*spf);
    spf++;
}

void v_div_f()
{
    if(*spf==0)
    {
        _SelfMathCatcher(FPE_ZERODIVIDE);
        *(spf+1)=0.0;
    }
    else
    {
        *(spf+1) = (*(spf+1))/(*spf);
    }
    spf++;
}

void v_add_f()
{
    *(spf+1)=(*(spf+1))+(*spf);
    spf++;
}

void v_sub_f()
{
    *(spf+1)=(*(spf+1))-(*spf);
    spf++;
}

void push_float_const()
{
    PUSH_DOUBLE(*((double *)(codepoint)));
    codepoint += 4;
}

void push_long_const()
{
    PUSH_LONG(*((UINT32 *)(codepoint)));
    codepoint+=2;
}

void push_long()
{
    PUSH_LONG(*(variablesl[*codepoint++]));
}

void _push_long()
{
    // long* l = (long*)variabless[*codepoint];
    // PUSH_LONG(l[1])
    // codepoint+=1;
    PUSH_LONG(*(variablesl[*codepoint++]+1));
}

void _pop_long_old()
{
    long *l=(long *)variablesf[*codepoint];
    *l=POP_LONG;
    codepoint+=1;
}

void _pop_long()
{
    long *l=(long *)variablesf[*codepoint];
    l[1] = POP_LONG;
    codepoint += 1;
}

void push_float()
{
    //double *_v=variablesf[*codepoint];
    //PUSH_DOUBLE(*_v)
    //codepoint+=1;
    PUSH_DOUBLE(*(variablesf[*codepoint++]));
}

void _push_float()
{
    // double *_v=variablesf[*codepoint];
    // PUSH_DOUBLE(_v[1])
    // codepoint+=1;
    PUSH_DOUBLE(*(variablesf[*codepoint++] + 1))
}

void _pop_float()
{
    // double *_v=variablesf[*codepoint];
    // _v[1]=POP_DOUBLE;
    // codepoint+=1;
    *(variablesf[*codepoint++]+1) = POP_DOUBLE;
}

void _pop_float_old()
{
    double* _v = variablesf[*codepoint];
    _v[0] = POP_DOUBLE;
    codepoint+=1;
}

void vm_diff1()
{
    double y = POP_DOUBLE;  // правая часть
    double dt = POP_DOUBLE; // шаг
    double x = POP_DOUBLE;  // старое значение

    PUSH_DOUBLE(x + y*dt)
}

void vm_diff2()
{
    double y = POP_DOUBLE;        // правая часть
    double* y4 =(double*)POP_PTR; // старое значение
    double* y3 =(double*)POP_PTR; // старое значение
    double* y2 =(double*)POP_PTR; // старое значение
    double* t = (double*)POP_PTR; // время
    double dt = POP_DOUBLE;       // шаг
    double x = POP_DOUBLE;        // старое значение
    double xx = 0;

    // шагаем по времени
    t[1] = t[0] + dt;
    // if (!y2[0]) y2[0] = y;
    // if (!y3[0]) y3[0] = y;
    // if (!y4[0]) y4[0] = y;

    xx = x + dt*(y + 2*y2[0] + 2*y3[0] + y4[0])/6;
    y2[1] = y;
    y3[2] = y2[1];
    y4[1] = y3[2];

    PUSH_DOUBLE(xx)
}

void vm_equation()
{
    double * pvar = variablesf[*codepoint++];
    TProject* prj = (TProject*)GetCurrentProject();
    Push_Context();
    prj->ChkEqu();
    if (prj->equations) prj->equations->SolveSystem(pvar);
    Pop_Context();
}

void vm_dequation()
{
    double * pvar = variablesf[*codepoint++];
    TProject* prj = (TProject*)GetCurrentProject();
    Push_Context();
    prj->ChkEqu();
    if (prj->equations) prj->equations->SolveDSystem(pvar);
    Pop_Context();
}


//Добавил Марченко С.В.

void vm_increment_after_f()
{
    *(variablesf[*codepoint++]+1) = (*spf)+1;
    (*spf)++;
    //double* val=(double*)POP_PTR;
    //(*val)++;
}

void plus_string_float()
{
    double n=POP_DOUBLE;
    char *s1=POP_PTR;
    char *s=GetTmpString();
    sprintf(s,"%s%g",s1,n);
    PUSH_PTR(s);
}

void plus_float_string()
{
    char *s1=POP_PTR;
    double n=POP_DOUBLE;

    char *s=GetTmpString();
    sprintf(s,"%g%s",n,s1);
    PUSH_PTR(s);
}

void vm_getscreenwidth()
{
    PUSH_DOUBLE(GetSystemMetrics(SM_CXSCREEN));
}

void vm_getscreenheight()
{
    PUSH_DOUBLE(GetSystemMetrics(SM_CYSCREEN));
}

void vm_getworkareax()
{
    RECT r;
    SystemParametersInfo(SPI_GETWORKAREA,0,&r,0);
    PUSH_DOUBLE(r.left);
}

void vm_getworkareay()
{
    RECT r;
    SystemParametersInfo(SPI_GETWORKAREA,0,&r,0);
    PUSH_DOUBLE(r.top);
}

void vm_getworkareawidth()
{
    RECT r;
    SystemParametersInfo(SPI_GETWORKAREA,0,&r,0);
    PUSH_DOUBLE(r.right);
}

void vm_getworkareaheight()
{
    RECT r;
    SystemParametersInfo(SPI_GETWORKAREA,0,&r,0);
    PUSH_DOUBLE(r.bottom);
}

void vm_getkeyboardlayout()
{
    PUSH_DOUBLE(LOWORD(GetKeyboardLayout(0)));
}

void vm_gettitleheight()
{
    PUSH_DOUBLE(GetSystemMetrics(SM_CYCAPTION));
}

void vm_getsmalltitleheight()
{
    PUSH_DOUBLE(GetSystemMetrics(SM_CYSMCAPTION));
}

void vm_getfixedframewidth()
{
    PUSH_DOUBLE(GetSystemMetrics(SM_CXFIXEDFRAME));
}

void vm_getfixedframeheight()
{
    PUSH_DOUBLE(GetSystemMetrics(SM_CYFIXEDFRAME));
}

void vm_getsizeframewidth()
{
    PUSH_DOUBLE(GetSystemMetrics(SM_CXSIZEFRAME));
}

void vm_getsizeframeheight()
{
    PUSH_DOUBLE(GetSystemMetrics(SM_CYSIZEFRAME));
}

void vm_substr1_string()
{
    int n1=(int)POP_DOUBLE;
    if(n1<0) n1=0;
    char *s1=POP_PTR;
    char *s=GetTmpString();
    if(n1>STRING_SIZE)
        n1=STRING_SIZE;
    Lstrcpyn(s, s1+n1, strlen(s1)-n1+1);
    // s[n1+n2+1] = 0;
    PUSH_PTR(s);
}

void vm_setbrushobject2d_handle()
{
    HOBJ2D hbrush=POP_HOBJ;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(SetBrushObject2d(hsp, obj, hbrush));
}

void vm_setpenobject2d_handle()
{
    HOBJ2D hpen=POP_HOBJ;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_HOBJ(SetPenObject2d(hsp, obj, hpen));
} ;


void vm_setwindowtransparent()
{
    int TransparentLevel=(int)POP_DOUBLE;
    char *winname=POP_PTR;
    if(winmanager->GetWindow(winname))
    {PUSH_DOUBLE(winmanager->GetWindow(winname)->SetTransparent(TransparentLevel));}
    else
    { PUSH_DOUBLE(0);}
}

void vm_setwindowtransparent_h()
{
    int TransparentLevel=(int)POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    if(winmanager->GetWindow(hsp))
    {	PUSH_DOUBLE(winmanager->GetWindow(hsp)->SetTransparent(TransparentLevel));}
    else
    { PUSH_DOUBLE(0);}
}

void vm_setwindowtransparentcolor()
{
     { PUSH_DOUBLE(0);}
}

void vm_setwindowtransparentcolor_h()
{
    COLORREF rgb=POP_LONG;
    HSP2D hsp = POP_HSP;
    if(winmanager->GetWindow(hsp))
    {
        PUSH_DOUBLE(winmanager->GetWindow(hsp)->SetTransparent(rgb));
    }
    else
    {
        PUSH_DOUBLE(0);
    }
}

void vm_setwindowregion()
{
    long Region = POP_LONG;
    char *winname=POP_PTR;
    if(winmanager->GetWindow(winname))
    {	PUSH_DOUBLE(winmanager->GetWindow(winname)->SetWindowRegion(Region));}
    else
    { PUSH_DOUBLE(0);}
}

void vm_setwindowregion_h()
{
    long Region = POP_LONG;
    HSP2D hsp = POP_HSP;
    if(winmanager->GetWindow(hsp))
    {	PUSH_DOUBLE(winmanager->GetWindow(hsp)->SetWindowRegion(Region));}
    else
    { PUSH_DOUBLE(0);}
}

void vm_setwindowowner()
{
    HSP2D hspParent = POP_HSP;
    HSP2D hsp = POP_HSP;

    TWinItem* w = winmanager->GetWindow(hsp);
    if(w)
    {
        TWinItem* wP = winmanager->GetWindow(hspParent);
        //int style = 0; GetWindowLong(w->HWindow, GWL_STYLE) | GetWindowLong(w->HWindow, GWL_EXSTYLE);
        if(wP)
            SetWindowLong(w->HWindow, GWL_HWNDPARENT, (LONG)wP->HWindow);
        else
            SetWindowLong(w->HWindow, GWL_HWNDPARENT, (LONG)mainwindow->HWindow);
        PUSH_DOUBLE(1);
    }
    else
    {PUSH_DOUBLE(0);}
}

void vm_setwindowparent()
{
    HSP2D hspParent = POP_HSP;
    HSP2D hsp = POP_HSP;

    TWinItem* w = winmanager->GetWindow(hsp);
    if(w)
    {
        TWinItem* wP = winmanager->GetWindow(hspParent);
        if(wP)
        {
            int style = GetWindowLong(w->HWindow, GWL_EXSTYLE);
            SetParent(w->HWindow,wP->HWindow);
            SetWindowLong(w->HWindow, GWL_EXSTYLE, style | WS_CHILD);
        }
        else
            SetParent(w->HWindow,mainwindow->HWindow);
        PUSH_DOUBLE(1);
    }
    else
    {PUSH_DOUBLE(0);}
}

void vm_windowintaskbar_h()
{
    int IsShow = (int)POP_DOUBLE;
    HSP2D hsp=POP_HSP;
    TWinItem*w = winmanager->GetWindow(hsp);
    if(w)
    {
        HWND hwnd = w->HWindow;
        if(hwnd)
        {
            int rez=0;
            if(IsShow)
            {
                rez|=SetWindowLong(hwnd,GWL_EXSTYLE,GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_APPWINDOW);
                //rez|=SetWindowLong(w->HWindow, GWL_HWNDPARENT, (LONG)GetDesktopWindow());
                PUSH_DOUBLE(rez);
                return;
            }
            else
            {
                rez|=SetWindowLong(hwnd,GWL_EXSTYLE,GetWindowLong(hwnd, GWL_EXSTYLE) ^WS_EX_APPWINDOW);
                //rez|=SetWindowLong(w->HWindow, GWL_HWNDPARENT, (LONG)mainwindow->HWindow);
                PUSH_DOUBLE(rez);
                return;
            }
        }
    }
    PUSH_DOUBLE(0);
}

void vm_showcursor()
{
    double isShow = POP_DOUBLE;
    if(isShow>0)
        while(ShowCursor(1)<0);
    else
        while(ShowCursor(0)>=0);
}

LPCSTR ConvertCursorID(int Cursor)
{
    LPCSTR cur=NULL;
    switch(Cursor)
    {
        case 0:cur=IDC_ARROW;break; 		//- Стандартная стрелка
        case 1:cur=IDC_CROSS;break; 		//- Перекрестие
        case 2:cur=IDC_IBEAM;break; 		//- I-прямая вертикальная линия для текста
        case 3:cur=IDC_NO;break; 				//- Перечеркнутый круг
        case 4:cur=IDC_SIZEALL;break; 	//- Четырех направленная стрелка
        case 5:cur=IDC_SIZENESW;break; 	//- Двунаправленная стрелка, указывающая северо-восток и юго-запад
        case 6:cur=IDC_SIZENS;break; 		//- Двунаправленная стрелка, указывающая север и юг
        case 7:cur=IDC_SIZENWSE;break; 	//- Двунаправленная стрелка, указывающая северо-запад и юго-восток
        case 8:cur=IDC_SIZEWE;break; 		//- Двунаправленная стрелка, указывающая запад и восток
        case 9:cur=IDC_UPARROW;break; 	//- Вертикальная стрелка
        case 10:cur=IDC_WAIT;break; 		//- Песочные часы
        case 11:cur=MAKEINTRESOURCE(32649);break; 		//- Выбор ссылки
        case 12:cur=IDC_HELP;break; 		//- Получение справки
        default:return NULL;
    }
    return cur;
}

void vm_setstandartcursor()
{
    int Cursor = (int)POP_DOUBLE;
    HSP2D hsp = POP_HSP;
    if(winmanager->GetWindow(hsp))
    {
        winmanager->GetWindow(hsp)->_window->SetDefCursor(0,ConvertCursorID(Cursor));
    }
}

void vm_setstandartcursor_s()
{
    int Cursor = (int)POP_DOUBLE;
    char *winname=POP_PTR;
    if(winmanager->GetWindow(winname))
    {
        winmanager->GetWindow(winname)->_window->SetDefCursor(0,ConvertCursorID(Cursor));
    }
}

void vm_loadcursor()
{
    char *filename=POP_PTR;
    HSP2D hsp = POP_HSP;

    if(winmanager->GetWindow(hsp))
    {
        winmanager->GetWindow(hsp)->_window->LoadDefCursor(filename);
    }
}

void vm_loadcursor_s()
{
    char *filename=POP_PTR;
    char *winname=POP_PTR;
    if(winmanager->GetWindow(winname))
    {
        winmanager->GetWindow(winname)->_window->LoadDefCursor(filename);
    }
}

//делаем скриншот рабочего стола
void vm_screenshot_desktop_full()
{
    HSP2D hsp_target = POP_HSP;

    if(winmanager->GetWindow(hsp_target))
    {
        HWND hw=GetDesktopWindow();//хотим сделать снимок рабочего стола
        if(hw)
        {
            double x,y,w,h;
            RECT rect;
            if(GetWindowRect(hw, &rect))
            {
                x=rect.left;   y=rect.top;
                w=rect.right-rect.left;
                h=rect.bottom-rect.top;
                HOBJ2D hdib = CreateDib2d(hsp_target,CreateScreenDIB(hw,x,y,w,h));
                PUSH_HOBJ(hdib);
                return;
            }
        }
    }
    PUSH_HOBJ(0);
}

//делаем скриншот всего окна
void vm_screenshot_full()
{
    HSP2D hsp_target = POP_HSP;
    HSP2D hsp = POP_HSP;

    if(winmanager->GetWindow(hsp_target) && winmanager->GetWindow(hsp))
    {
        HWND hw=winmanager->GetWindow(hsp)->HWindow;
        if(hw)
        {
            double x,y,w,h;
            RECT rect;
            if(GetWindowRect(hw, &rect))
            {
                x=rect.left;   y=rect.top;
                w=rect.right-rect.left;
                h=rect.bottom-rect.top;

                GetClientRect(hw, &rect);
                x=-(w-rect.right)/2;
                y=-(h-rect.bottom+x);

                HOBJ2D hdib = CreateDib2d(hsp_target,CreateScreenDIB(hw,x,y,w,h));
                PUSH_HOBJ(hdib);
                return;
            }
        }
    }
    PUSH_HOBJ(0);
}

//делаем скриншот части рабочего стола
void vm_screenshot_desktop()
{
    double h = POP_DOUBLE;
    double w = POP_DOUBLE;
    double y = POP_DOUBLE;
    double x = POP_DOUBLE;
    HSP2D hsp_target = POP_HSP;

    if(winmanager->GetWindow(hsp_target))
    {
        HWND hw=GetDesktopWindow();
        if(hw)
        {
            HOBJ2D hdib = CreateDib2d(hsp_target,CreateScreenDIB(hw,x,y,w,h));
            PUSH_HOBJ(hdib);
            return;
        }
    }
    PUSH_HOBJ(0);
}

//делаем скриншот части окна
void vm_screenshot()
{
    double h = POP_DOUBLE;
    double w = POP_DOUBLE;
    double y = POP_DOUBLE;
    double x = POP_DOUBLE;
    HSP2D hsp_target = POP_HSP;
    HSP2D hsp = POP_HSP;

    if(winmanager->GetWindow(hsp_target) && winmanager->GetWindow(hsp))
    {
        HWND hw=winmanager->GetWindow(hsp)->HWindow;
        if(hw)
        {
            HOBJ2D hdib = CreateDib2d(hsp_target, CreateScreenDIB(hw, x,y, w,h, hsp));
            PUSH_HOBJ(hdib);
            return;
        }
    }
    PUSH_HOBJ(0);
}

void vm_lockobject2d()
{
    int flag=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;

    ATTR_MODE mode=1;
    UINT16 Attr=0;
    if(flag==0)
    { mode=ATTRRESET; Attr=12;}
    if(flag==1)
    {
        mode=ATTRRESET; Attr=4;
        SetAttribute2d(hsp, obj, Attr, mode);
        mode=ATTRSET; Attr=8;
    }
    if(flag==2)
    {
        mode=ATTRRESET; Attr=8;
        SetAttribute2d(hsp, obj, Attr, mode);
        mode=ATTRSET; Attr=4;
    }

    if(Attr==0){PUSH_DOUBLE(0);return;}
    PUSH_DOUBLE(SetAttribute2d(hsp, obj, Attr, mode));
}

void vm_getvarinfofull()
{
    double* flag=(double*)POP_PTR;
    char * info=POP_PTR;
    char * def=POP_PTR;
    char * type=POP_PTR;
    char * name=POP_PTR;
    int  i=POP_DOUBLE;
    char *_classname=POP_PTR;

    PClass _class=GetClassByName(_classname);
    if(_class && i>=0 && i<_class->varcount)
    {
        PVarInfo  pv=_class->vars+i;
        lstrcpy(info,pv->info);
        lstrcpy(def,pv->def);
        lstrcpy(type,pv->classType->GetClassName());
        lstrcpy(name,pv->name);
        *flag=pv->flags;
        PUSH_DOUBLE(1);
        return;
    }
    PUSH_DOUBLE(0)
}

unsigned long ShellProccess(char* file, char* params, char* verb, bool wait=false)
{
    unsigned long result;
    if(wait)
    {
        SHELLEXECUTEINFO sei;
        memset(&sei, 0, sizeof(sei));
        sei.cbSize = sizeof(SHELLEXECUTEINFO);
        sei.hwnd = mainwindow->HWindow;
        sei.fMask = SEE_MASK_NOCLOSEPROCESS;
        sei.nShow = SW_HIDE;

        sei.lpFile = file;
        sei.lpVerb = verb;
        sei.lpParameters = params;

        if(!ShellExecuteEx(&sei)) return 1;
        if(sei.hProcess == 0) return 1;
        WaitForSingleObjectEx(sei.hProcess, INFINITE, false);
        GetExitCodeProcess(sei.hProcess, &result);
        CloseHandle(sei.hProcess);
        //char s[10];
        //wsprintf(s,"%d", result);
        //MessageBox(0,s,"awef",0);
    }
    else
    {
        ShellExecute(mainwindow->HWindow,verb,file,params,NULL,SW_HIDE);
    }
    return result;
}

//TODO: для этой и след.функции нужно сделать перегрузку
//с параметром - ждать окончания вызова или нет

void vm_sendsms()
{
    char *text=POP_PTR;
    char *numbers=POP_PTR;

    char path[MAX_STRING]="";
    char params[MAX_STRING]="";
    char exefile[MAX_STRING]="";
    char logfile[MAX_STRING]="";
    char cmdfile[MAX_STRING]="";

    wsprintf(path,"%s\\plugins\\sms\\",sc_path);
    wsprintf(cmdfile,"\"%sSendSMS.cmd\"",path);
    wsprintf(exefile,"\"%snhsmscl.exe\"",path);
    wsprintf(logfile,"\"%sSendSMS.log\"",path);
    wsprintf(params,"%s %s -phone:%s  \"%s\"",logfile,exefile,numbers,text);

    //MessageBox(0,cmdfile,"",0);
    //MessageBox(0,params,"",0);
    ShellProccess(cmdfile,params,"open");
}

void vm_sendsmswait()
{
    bool wait = (POP_DOUBLE)>0;
    char *text=POP_PTR;
    char *numbers=POP_PTR;

    char path[MAX_STRING]="";
    char params[MAX_STRING]="";
    char exefile[MAX_STRING]="";
    char logfile[MAX_STRING]="";
    char cmdfile[MAX_STRING]="";

    wsprintf(path,"%s\\plugins\\sms\\",sc_path);
    wsprintf(cmdfile,"\"%sSendSMS.cmd\"",path);
    wsprintf(exefile,"\"%snhsmscl.exe\"",path);
    wsprintf(logfile,"\"%sSendSMS.log\"",path);
    wsprintf(params,"%s %s -phone:%s  \"%s\"",logfile,exefile,numbers,text);

    //MessageBox(0,cmdfile,"",0);
    //MessageBox(0,params,"",0);
    unsigned long result = ShellProccess(cmdfile,params,"open",wait);
    PUSH_DOUBLE(result);
}

void vm_sendmailwait()
{
    bool wait = (POP_DOUBLE)>0;
    char file[1000];
    GetFullFileNameEx((TProject *)GetCurrentProject(), POP_PTR, file);

    char exefile[MAX_STRING]="";
    char params[MAX_STRING]="";
    wsprintf(exefile,"\"%s\\plugins\\sendmail\\sendmail.exe\"",sc_path);
    wsprintf(params,"\"%s\"",file);

    //MessageBox(0,exefile,"",0);
    //MessageBox(0,params,"",0);

    unsigned long result = ShellProccess(exefile,params,"open",wait);
    PUSH_DOUBLE(result);
}


void vm_sendmail()
{
    char file[1000];
    GetFullFileNameEx((TProject *)GetCurrentProject(), POP_PTR, file);

    char exefile[MAX_STRING]="";
    char params[MAX_STRING]="";
    wsprintf(exefile,"\"%s\\plugins\\sendmail\\sendmail.exe\"",sc_path);
    wsprintf(params,"\"%s\"",file);

    //MessageBox(0,exefile,"",0);
    //MessageBox(0,params,"",0);

    ShellProccess(exefile,params,"open");
}

void inc()
{
    double* value=(double*)POP_PTR;
    (*value)++;
}

void inc_step()
{
    double step=POP_DOUBLE;
    double* value=(double*)POP_PTR;
    (*value)+=step;
}

void dec()
{
    double* value = (double*)POP_PTR;
    (*value)--;
}

void dec_step()
{
    double step=POP_DOUBLE;
    double* value=(double*)POP_PTR;
    (*value)-=step;
}

void limit()
{
    double val_max=POP_DOUBLE;
    double val_min=POP_DOUBLE;
    double value=POP_DOUBLE;
    value=max(min(value,val_max),val_min);
    PUSH_DOUBLE(value);
}

int scCALLBACK VM_EnumFontFam(const ENUMLOGFONT FAR*lpnlf,
                              const NEWTEXTMETRIC FAR*tm,int FontType, LPARAM  lParam)
{
    if(tm->tmPitchAndFamily&TMPF_TRUETYPE)
    {
        dyno_t h=(dyno_t)lParam;
        if(dyno_mem->InsertInVar(h,"STRING"))
        {
            int i=max(dyno_mem->GetCount(h)-1,0);
            dyno_mem->SetVar(h, i, "STRING", 3, (void*)lpnlf->elfLogFont.lfFaceName);
        }
    }
    return TRUE;
};

void getfontlist()
{
    HWND hw=mainwindow->HWindow;
    if(hw)
    {
        HDC hdc=GetDC(hw);
        if(hdc)
        {
            dyno_t h=dyno_mem->NewVar();
            if(h)
            {
                EnumFontFamilies(hdc, NULL, (FONTENUMPROC)VM_EnumFontFam, (LPARAM)h);
                ReleaseDC(hw,hdc);
                PUSH_LONG(h);
                return;
            }
        }
    }
    PUSH_LONG(0);
}

void vm_vsort()
{
    INT16 count = *((INT16 *)codepoint);
    codepoint += 1;
    char** vars = spp;
    spp += count;
    double desc = POP_DOUBLE;
    dyno_t h = POP_LONG;

    bool *asc = new bool[max(count,1)];
    for(int i=0;i<max(count,1);i++) asc[i]=desc<=0;
    PUSH_DOUBLE(dyno_mem->Sort(h,vars,count,asc,-1));
    delete asc;
}

void vm_vsort_asc()
{
    INT16 count=*((INT16 *)codepoint);
    codepoint+=1;
    char ** vars=spp;
    spp+=count;
    dyno_t h=POP_LONG;
    PUSH_DOUBLE(dyno_mem->Sort(h,vars,count,NULL,-1));
}

void vm_vsort_different()
{
    INT16 count=*((INT16 *)codepoint);
    codepoint+=1;
    char **vars=spp;
    spp+=count+count/2;
    dyno_t h=POP_LONG;
    struct fieldstruct
    {
        double desc;
        char *name;
    };
    fieldstruct* fields=(fieldstruct *)vars;
    count/=2;
    bool *asc=new bool[count];
    char **Fnames=new char*[count];
    for(int i=0;i<count;i++)
    {
        Fnames[i]=new char[strlen(fields[i].name)];
        strcpy(Fnames[i],fields[i].name);
        asc[i]=(fields[i].desc)<=0;
    }
    PUSH_DOUBLE(dyno_mem->Sort(h,Fnames,count,asc,-1));
    for(i=0;i<count;i++)
        delete Fnames[i];
    delete[] Fnames;
    delete asc;
}

void vm_readuserkey()
{
    char* fileName=POP_PTR;
    char s[999];
    GetFullFileNameEx((TProject *)GetCurrentProject(), fileName, s);
    UserKey* uk=new UserKey(s);
    //MessageBox(0,s,"awef",0);
    PUSH_LONG((long)uk);
}

void vm_readprojectkey()
{
    char* fileName=POP_PTR;
    UserKey* uk=(UserKey*)POP_LONG;
    char s[999];
    GetFullFileNameEx((TProject *)GetCurrentProject(), fileName, s);
    uk->ReadProjectKey(s);
    PUSH_DOUBLE(uk->ReadProjectKey(s));
}


void vm_getuserkeyvalue()
{
    char* name=POP_PTR;
    if(userkey && userkey->tags)
    {
        PUSH_PTR(userkey->tags->GetTagValue(name));
        return;
    }
    PUSH_PTR("");
}

void vm_getuserkeyvalue_uk()
{
    char* name=POP_PTR;
    UserKey* uk=(UserKey*)POP_LONG;
    if(uk && uk->tags)
    {
        PUSH_PTR(uk->tags->GetTagValue(name));
        return;
    }
    PUSH_PTR("");
}

void vm_getuserkeyfullvalue()
{
    char* name=POP_PTR;
    if(userkey && userkey->tags)
    {
        PUSH_PTR(userkey->tags->GetTagFullValue(name));
        return;
    }
    PUSH_PTR("");
}

void vm_getuserkeyfullvalue_uk()
{
    char* name=POP_PTR;
    UserKey* uk=(UserKey*)POP_LONG;
    if(uk && uk->tags)
    {
        PUSH_PTR(uk->tags->GetTagFullValue(name));
        return;
    }
    PUSH_PTR("");
}


void vm_senduserresult()
{
    int h=POP_LONGi;
    if(userkey)
    {
        PUSH_DOUBLE(userkey->SendResult(h));
        return;
    }
    PUSH_DOUBLE(0);
}

void vm_senduserresult_uk()
{
    int h=POP_LONGi;
    UserKey* uk=(UserKey*)POP_LONG;
    if(uk)
    {
        PUSH_DOUBLE(uk->SendResult(h));
        return;
    }
    PUSH_DOUBLE(0);
}


void vm_copyuserresult()
{
    if(userkey)
    {
        PUSH_DOUBLE(userkey->CopyResult());
        return;
    }
    PUSH_DOUBLE(0);
}

void vm_copyuserresult_uk()
{
    UserKey* uk=(UserKey*)POP_LONG;
    if(uk)
    {
        PUSH_DOUBLE(uk->CopyResult());
        return;
    }
    PUSH_DOUBLE(0);
}

void vm_userkeyisautorized()
{
    if(userkey)
    {
        PUSH_DOUBLE(userkey->IsAutorized);
        return;
    }
    PUSH_DOUBLE(0);
}

void vm_userkeyisautorized_uk()
{
    UserKey* uk=(UserKey*)POP_LONG;
    if(uk)
    {
        PUSH_DOUBLE(uk->IsAutorized);
        return;
    }
    PUSH_DOUBLE(0);
}


void vm_gettempdirectory()
{
    char*s=GetTmpString();
    GetTempPath(200,s);
    PUSH_PTR(s);
}

void vm_getromdrivenames()
{
    char* result = GetTmpString();
    result[0]=0;
    char buf[100];
    GetLogicalDriveStringsA(sizeof(buf)/sizeof(char), buf);
    LPCSTR sDrivePath;
    for (char* s = buf; *s; s+=strlen(s)+1)
    {
        sDrivePath = (LPCSTR)s;
        if(GetDriveTypeA(sDrivePath) == DRIVE_CDROM)
        {
            if(strlen(result)>0) strcat(result,",");
            strncat(result,sDrivePath,1);
        }
    }
    PUSH_PTR(result);
}

void vm_shellwait()
{
    int fuCmdShow=POP_DOUBLE;
    char *dir=POP_PTR;
    char *param=POP_PTR;
    char *cmd=POP_PTR;

    SHELLEXECUTEINFO sei;
    memset(&sei, 0, sizeof(sei));
    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.hwnd = mainwindow->HWindow;

    sei.lpFile = cmd;
    sei.lpParameters = param;
    sei.lpDirectory = dir;
    sei.nShow = fuCmdShow;

    if(!ShellExecuteEx(&sei))
    {
        PUSH_DOUBLE(0);
        return;
    }
    if(sei.hProcess == 0)
    {
        PUSH_DOUBLE(0);
        return;
    }
    WaitForSingleObjectEx(sei.hProcess, INFINITE, false);
    CloseHandle(sei.hProcess);

    PUSH_DOUBLE(1);
}

void vm_getprojectclasses()
{
    int includeAddLibs=POP_DOUBLE;
    dyno_t h=dyno_mem->NewVar();
    if(h)
    {
        PProject project = (PProject)GetCurrentProject();
        for(C_TYPE i=0;i<classes->count;i++)
        {
            PClassListInfo pcli=(PClassListInfo)classes->At(i);
            bool contains=false;
            for(C_TYPE iLib=0;(includeAddLibs && iLib<project->libs->count) || iLib<1;iLib++)
            {
                if(((TLibrary*)project->libs->At(iLib))->IsContain(pcli))
                { contains=true;break;}
            }
            if(!contains)continue;
            if(dyno_mem->InsertInVar(h,"STRING"))
            {
                int i=max(dyno_mem->GetCount(h)-1,0);
                dyno_mem->SetVar(h, i, "STRING", 3, (void*)pcli->GetClassName());
            }
        }
    }
    PUSH_LONG(h);
}

void vm_getclassfile()
{
    char *classname=POP_PTR;
    PClass cls=GetClassByName(classname);
    char* filename=new char[1];
    filename[0]=0;
    ///strcpy(filename,"");
    if(cls)
    {
        //filename=new char[999];
        //cls->listinfo->GetFileName(filename,true);//это определяет полный путь к файлу класса
        filename = cls->listinfo->filename;//это только имя самого файла класса без разрешения и пути
    }
    PUSH_PTR(filename);
}

void vm_setobjectalpha2d()
{
    double a=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetObjectAlpha2d(hsp, obj, a));
}

void vm_getobjectalpha2d()
{
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(GetObjectAlpha2d(hsp, obj));
}

void vm_setbrushpoints2d()
{
    double y1=POP_DOUBLE;
    double x1=POP_DOUBLE;
    double y0=POP_DOUBLE;
    double x0=POP_DOUBLE;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;
    PUSH_DOUBLE(SetBrushPoints2d(hsp,obj,x0,y0,x1,y1));
}

void vm_setbrushcolors2d()
{
    dyno_t h=(dyno_t)POP_LONG;
    HOBJ2D obj=POP_HOBJ;
    HSP2D hsp=POP_HSP;

    int count = dyno_mem->GetCount(h);
    if(count<2){PUSH_DOUBLE(-1);return;}

    COLORREF *colors=new COLORREF[count];
    for(int i=0;i<count;i++)
    {
        double color;
        dyno_mem->SetVar(h, i, "FLOAT", 0x80|0, &color);
        colors[i]=color;
    }

    PUSH_DOUBLE(SetBrushColors2d(hsp,obj,colors,count));
}

void vm_encryptstream()
{
    int resultH=POP_LONGi;
    int origH=POP_LONGi;

    char* s=new char[MAX_CLASSTEXTSIZE];
    if(streams->GetAllText(origH,s))
    {
        char* encText = Encryption::Encrypt(s);
        delete[] s;
        if(streams->SWrite(resultH,encText))
        {
            PUSH_DOUBLE(1);
            return;
        }
    }
    PUSH_DOUBLE(0);
}

void vm_decryptstream()
{
    int resultH=POP_LONGi;
    int origH=POP_LONGi;

    char* s=new char[MAX_CLASSTEXTSIZE];
    if(streams->GetAllText(origH,s))
    {
        char* encText = Encryption::Decrypt(s);
        delete[] s;
        if(streams->SWrite(resultH,encText))
        {
            PUSH_DOUBLE(1);
            return;
        }
    }
    PUSH_DOUBLE(0);
}

void vm_senddata()
{
    bool wait = (POP_DOUBLE)>0;
    char file[1000];
    GetFullFileNameEx((TProject *)GetCurrentProject(), POP_PTR, file);

    char exefile[MAX_STRING]="";
    char params[MAX_STRING]="";
    wsprintf(exefile,"\"%s\\plugins\\senddata\\senddata.exe\"",sc_path);
    wsprintf(params,"\"%s\"",file);

    unsigned long result = ShellProccess(exefile,params,"open",wait);
    PUSH_DOUBLE(result);
}


//char s[20];
//wsprintf(s,"%d",*value);
//MessageBox(0,s,"awef",0);

// - конец - Добавил Марченко С.В.

#ifdef FAST_EXECUTE
int VM_Execute(WORD*vcode,void *vars,void* _po)
{
    if (VM_ERROR)return 0;
    INT16 op;
    pobject=(PObject)_po;
    v_code=vcode;
    (void*)variables=vars;
    codepoint=vcode;
    while((operation=op=*codepoint++)!=0)
        commands[op]();
    pobject=NULL;
    return TRUE;
};
#else

int VM_Execute(WORD* vcode, void* vars, void* _po)
{
    if (VM_ERROR)
        return 0;

    int ret = TRUE;
    void* oldStack = spl;

    pobject = (PObject)_po;
    v_code = vcode;

    (void*) variables = vars;
    codepoint = vcode;

    INT16 op;
    VMPROC _oper;
    while((operation = op = *codepoint) != 0)
    {
        codepoint++;
        if (op > 0 && op <= VM_MAXIMUM_code)
        {
            _oper = commands[op];
            if (_oper)
            {
#ifdef LOGON
                /*  char s[100];
  wsprintf(s,"VM_Execute command=%d", op);
    LogMessage(s);*/
#endif
                _oper();
            }
            else
                goto M_vm_error;
        }
        else
        {
M_vm_error:
            _Error(EM_VMUNCNOWOP,op);
            VM_ERROR = 1;
            ret = 0;
            goto m2;
        }

        static int chkСount = 16384;
        chkСount--;
        if (!chkСount)
        {
            chkСount  = 16384;

            if ((GetAsyncKeyState(VK_CONTROL) < 0) && (GetAsyncKeyState(VK_MENU) < 0)
                &&(GetAsyncKeyState('Z') < 0))
            {
                char s[MAX_STRING];
                char ss[MAX_STRING];
                LoadString(HResource, IDS_MSG32, ss, sizeof(ss));

                wsprintf(s, ss, pobject ? pobject->_class->name : "Unknown Type");
                if( MessageBox(mainwindow->HWindow,s,"Stratum Computer Virtual Machine",
                               MB_TASKMODAL|MB_YESNO|MB_ICONSTOP|MB_DEFBUTTON1)==IDYES)
                {
                    VM_ERROR=2;
                    ret=0;
                    spl=(long*)oldStack;
                    goto m2;
                }
            }
        }
    }

    if (oldStack != spl && (!VM_ERROR))
    {
        if(pobject)
            _Error(EM_VMINTERNAL,0,pobject->_class->name);
        else
            _Error(EM_VMINTERNAL,0,"in function ");
    }
m2:
    pobject = NULL;
    return ret;
}
#endif

//--------------------------------------------- Table

int VM_RESET()
{
    spf = &(stuck_f[maxsttuck - 1]); // Установили плавающего стека
    Clear_Context();

    VM_ERROR = 0;
    return 1;
}

int VM_Done()
{
    delete stuck_f;
    delete commands;
    delete tmp_string_buffer;
    Clear_Context();
    return 0;
}

void Vm_Halt()
{
    VM_ERROR = 1;
    codepoint = &vm_nullcode;
}

int VM_Error()
{
    return VM_ERROR;
};

INT16 GetRunningObject(PClass*_class,PObject* po)
{
    if (pobject)
    {
        if (_class)*_class=pobject->_class;
        if (po)*po=pobject;
        return operation;
    }
    return 0;
};

// SDK functions
extern "C" char* SCVm_InsertString(char*s,int size)
{
    char *_s=GetTmpString();
    if(s){lstrcpyn(_s,s,max(size+1,STRING_SIZE));}
    return _s;
}

extern "C" PObject SCGetObject(char * path)
{
    if (path)
    {
        PObject _po = pobject;
        if(!_po)
        {
            GetRootObject(NULL,&_po);
            if(!_po)
                return NULL;
        }
        return _po->GetReceiver(path);
    }
    return pobject;
};

struct VMCONTEXT
{
    BYTE   _invfunction;
    PObject _pobject;
    WORD *_codepoint;
    WORD *_v_code;
    void ** _variables;
    VMCONTEXT * prev;
};

VMCONTEXT * vmcontext = NULL;

BOOL CanModifyClass(PClass _class)
{
    PObject po = pobject;
    while(po)
    {
        if(po->_class == _class)
            return FALSE;

        VMCONTEXT* vm = vmcontext;
        while(vm)
        {
            if(vm->_pobject && vm->_pobject->_class==_class)return FALSE;
            vm = vm->prev;
        }
        po = po->parent;
    }
    return TRUE;
};

static INT16 context_count=0;

BOOL Push_Context()
{
    VMCONTEXT *vm = new VMCONTEXT;
    vm->prev = vmcontext;
    vmcontext = vm;

    vmcontext->_invfunction = invfunction;
    vmcontext->_pobject = pobject;
    vmcontext->_codepoint = codepoint;
    vmcontext->_v_code = v_code;
    vmcontext->_variables = variables;
    context_count++;

    return (context_count < 60);
}

void Pop_Context()
{
    if (vmcontext)
    {
        pobject=vmcontext->_pobject;
        codepoint=vmcontext->_codepoint;
        v_code=vmcontext->_v_code;
        invfunction=vmcontext->_invfunction;

        variables = vmcontext->_variables;
        if(!invfunction)
            UPDATE_VARS();

        VMCONTEXT* vm=vmcontext;
        vmcontext=vm->prev;
        delete vm;
        context_count--;
    }
    else
    {
        _Error(EM_VMINTERNAL);
    }
}

void Clear_Context()
{
    while (vmcontext)
    {
        VMCONTEXT* vm = vmcontext;
        vmcontext = vm->prev;
        delete vm;
    }
}

// ***************
//char * temp_pchar;
//char temp_char;

void Commands3d();

int VM_Init()
{
    maxsttuck = 1000;
    stuck_f = new double [maxsttuck];
    VM_RESET();

    //
    InitInput();

    //
//    InitMedia();

    string_buffer_index = 0;
    string_buffer_count = 32;
    tmp_string_buffer = new char[STRING_SIZE * string_buffer_count];

    // ***************
    // temp_pchar=tmp_string_buffer+(STRING_SIZE*STRING_BUFFER_SIZE);
    // temp_char=*temp_pchar;

    commands = new VMPROC[VM_MAXIMUM_code + 1];
    memset(commands, 0, 4*(VM_MAXIMUM_code + 1));

    {
    commands[VM_LOADPROJECT] = vm_loadproject;
    commands[VM_GETPROJECTPROP  ] = vm_getprojectprop;
    commands[VM_SETPROJECTPROP  ] = vm_setprojectprop;
    commands[VM_SETACTIVEPROJECT] = vm_setactiveproject;
    commands[VM_UNLOADPROJECT   ] = vm_unloadproject;
    commands[VM_GETPROJECTPROPS ] = vm_setprojectprops;

    commands[PUSH_FLOAT] = push_float;
    commands[_PUSH_FLOAT] = _push_float;
    commands[PUSH_FLOAT_const] = push_float_const;
    commands[PUSH_FLOAT_PTR] = push_float_ptr;

    commands[_POP_FLOAT]= _pop_float;
    commands[_POP_FLOAT_OLD] = _pop_float_old;
    commands[POP_FLOAT_PTR] = pop_float_ptr;

    commands[vmPUSH_LONG] = push_long;
    commands[vmPUSH_LONG_const] = push_long_const;
    commands[vm_PUSH_LONG]=_push_long;
    commands[vm_POP_LONG]=_pop_long;
    commands[vm_POP_LONG_OLD]=_pop_long_old;

    commands[FLOAT_TO_LONG] = float_to_long;
    commands[LONG_TO_FLOAT] = long_to_float;

    commands[PUSH_STRING] = push_string;
    commands[_PUSH_STRING] = _push_string;
    commands[PUSH_STRING_CONST] = push_string_const;
    commands[_POP_STRING] = _pop_string;
    commands[_POP_STRING_OLD] = _pop_string_old;
    commands[GETTICKCOUNT]=gettickcount;
    commands[V_SIN]=v_sin;
    commands[V_COS]=v_cos;
    commands[V_ASIN]=v_asin;
    commands[V_ACOS]=v_acos;
    commands[V_ATAN]=v_atan;
    commands[V_TAN]=v_tan;
    commands[V_EXP]=v_exp;
    commands[V_SQRT]=v_sqrt;
    commands[V_SQR]=v_sqr;
    commands[V_ED]=v_ed;
    commands[V_DELTA]=v_delta;
    commands[V_MUL_F]=v_mul_f;
    commands[V_DIV_F]=v_div_f;
    commands[V_ADD_F]=v_add_f;
    commands[V_SUB_F]=v_sub_f;
    //	commands[V_BIG_SUB]=v_sub_f;
    commands[V_MOD]=v_mod;
    commands[V_LN]=v_ln;
    commands[V_LG]=v_lg;
    commands[V_LOG]=v_log;
    commands[V_STEPEN]=v_stepen;
    commands[V_MAX]=v_max;
    commands[V_MIN]=v_min;
    commands[V_AVERAGE]=v_average;
    commands[V_ROUND]=v_round;
    commands[V_TRUNC]=v_trunc;
    commands[V_RANDOM]=v_random;
    commands[V_ABS]=v_abs;
    commands[V_SGN]=v_sgn;
    commands[V_RAD]=v_rad;
    commands[V_DEG]=v_deg;

    commands[V_AND]=v_and;
    commands[V_OR]=v_or;
    commands[V_NOT]=v_not;
    commands[V_JMP]=v_jmp;
    commands[V_JNZ]=v_jnz;
    commands[V_JZ]=v_jz;
    commands[V_JNZ_HANDLE]=v_jnz_handle;
    commands[V_JZ_HANDLE]=v_jz_handle;
    commands[V_STOP]=v_stop;
    commands[V_UN_MINUS]=v_un_minus;
    commands[V_AND_BINARY]=v_and_binary;
    commands[V_OR_BINARY]=v_or_binary;
    commands[V_SHR]=v_shr;
    commands[V_SHL]=v_shl;
    commands[V_EQUAL]=v_equal;
    commands[V_NOTEQUAL]=v_notequal;
    commands[V_MORE]=v_more;
    commands[V_MOREorEQUAL]=v_moreorequal;
    commands[V_LOW]=v_low;
    commands[V_LOWorEQUAL]=v_loworequal;
    commands[S_EQUAL]=s_equal;
    commands[S_NOTEQUAL]=s_notequal;
    commands[S_MORE]=s_more;
    commands[S_MOREorEQUAL]=s_moreorequal;
    commands[S_LOW]=s_low;
    commands[S_LOWorEQUAL]=s_loworequal;

    commands[V_EQUALI]=v_equali;
    commands[V_NOTEQUALI]=v_notequali;

    commands[V_EDI ]=v_edi;
    commands[V_ANDI]=v_andi;
    commands[V_ORI ]=v_ori;
    commands[V_NOTI]=v_noti;
    commands[V_NOTbin]=v_notbin;

    commands[V_MCREATE]=v_mcreate;
    commands[V_MDELETE]=v_mdelete;
    commands[V_MFILL]=v_mfill;
    commands[V_MGET]=v_mget;
    commands[V_MPUT]=v_mput;
    commands[V_MEDITOR]=v_meditor;
    commands[V_MDIAG]=v_mdiag;
    commands[V_MADDX]=v_maddx;
    commands[V_MSUBX]=v_msubx;
    commands[V_MDET]=v_mdet;
    commands[V_MDELTA]=v_mdelta;
    commands[V_MED]=v_med;
    commands[V_MDIVX]=v_mdivx;
    commands[V_MMULX]=v_mmulx;
    commands[V_TRANSP]=v_transp;
    commands[V_MADDC]=v_maddc;
    commands[V_MNOT]=v_mnot;
    commands[V_MSUM]=v_msum;

    commands[V_MSUBC]=v_msubc;
    commands[V_MMULC]=v_mmulc;

    commands[V_MDIVC]=v_mdivc;
    commands[V_MMUL]=v_mmul;
    commands[V_MGLUE]=v_mglue;
    commands[V_MCUT]=v_mcut;
    commands[V_MMOVE]=v_mmove;
    commands[V_MOBR]=v_mobr;

    commands[V_MLOAD]=v_mload;
    commands[V_MSAVEAS]=v_msaveas;
    commands[V_MDIM]=v_mdim;

    commands[V_SAVEDIALOG]=v_savedialog;
    commands[V_LOADDIALOG]=v_loaddialog;

    commands[V_CLOSEALL]=v_closeall;
    commands[VM_GETNAMEBYHANDLE]=vm_getnamebyhandle;

    commands[SCHANGE] = schange;
    commands[PLUS_STRING] = plus_string;
    commands[LEFT_STRING]=left_string;
    commands[RIGHT_STRING]=right_string;
    commands[SUBSTR_STRING]=substr_string;
    commands[POS_STRING]=pos_string;
    commands[REPLICATE_STRING]=replicate_string;
    commands[LOWER_STRING]=lower_string;
    commands[UPPER_STRING]=upper_string;
    commands[ANSI_TO_OEM_STRING]=ansi_to_oem;
    commands[OEM_TO_ANSI_STRING]=oem_to_ansi;
    commands[COMPARE_STRING]=compare_string;
    commands[COMPAREI_STRING]=comparei_string;
    commands[LENGTH_STRING]=length_string;
    commands[LTRIM_STRING]=ltrim_string;
    commands[RTRIM_STRING]=rtrim_string;
    commands[ALLTRIM_STRING]=alltrim_string;
    commands[ASCII_STRING]=ascii_string;
    commands[CHR_STRING]=chr_string;
    commands[FLOAT_TO_STRING]=float_to_string;
    commands[STRING_TO_FLOAT]=string_to_float;

    commands[V_QUITSC] = v_quit;
    commands[LOADSPACEWINDOW]=v_loadspacewindow;
    commands[OPENSCHEMEWINDOW]=v_openschemewindow;
    commands[CLOSEWINDOW]=v_closewindow;
    commands[SETWINDOWTITLE]=v_setwindowtitle;
    commands[GETWINDOWTITLE]=v_getwindowtitle;
    commands[SHOWWINDOW]=v_showwindow;
    commands[SETWINDOWPOS]=v_setwindowpos;
    commands[SETWINDOWORG]=v_setwindoworg;
    commands[SETWINDOWSIZE]=v_setwindowsize;
    commands[ISWINDOWVISIBLE]=v_iswindowvisible;
    commands[ISICONIC]=v_isIconic;
    commands[ISWINDOWEXIST]=v_iswindowexist;
    commands[BRINGWINDOWTOTOP]=v_bringwindowtotop;
    commands[CASCADEWINDOWS]=v_cascadewindows;
    commands[TILE]=v_tile;
    commands[ARRANGEICONS]=v_arrangeicons;
    commands[VM_GETANGLEBYXY]=vm_getanglebyxy;
    commands[WIN_ORGX]=v_winorgx;
    commands[WIN_ORGY]=v_winorgy;
    commands[WIN_SIZEX]=v_winsizex;
    commands[WIN_SIZEY]=v_winsizey;
    commands[GETOBJECTHANDLE]=v_getobjecthandle;
    commands[GETOBJECTBYNAME]=v_getobjectbyname;

    commands[GETCLIENTWIDTH]=getclientwidth;
    commands[GETCLIENTHEIGHT]=getclientheight;
    commands[CHOSECOLORDIALOG]=chosecolordialog;
    commands[JGETX]=jgetx;
    commands[JGETY]=jgety;
    commands[JGETZ]=jgetz;
    commands[JGETBTN]=jgetbuttons;
    commands[GETAKEYSTATE]=getasynckeystate;

    commands[ GETCLASSDIR]=getclassdir;
    commands[ GETPROJECTDIR]=getprojectdir;
    commands[ GETWINDOWSDIR]=getwindowsdir;
    commands[ GETSYSTEMDIR]=getsystemdir;
    commands[ GETDIRFROMFILE]=getdirfromfile;
    commands[ ADDSLASH]=addslash;
    commands[ GETSTRATUMDIR]=getstratumdir;
    commands[ WINEXECUTE]=winexecute;
    commands[ WINSHELL ]=vm_winshell;

    commands[ MCISENDSTRING   ]=   mcisendstring;
    commands[ MCISENDSTRING_INT]=   mcisendstring_int;
    commands[ MCISENDSTRINGEX ]=   mcisendstringex;
    commands[ GETLASTMCIERROR ]=   getlastmcierror;
    commands[ GETMCIERROR     ]=   getmcierror;

    commands[RGB_COLOR] = rgb_color;
    commands[RGB_COLORex] = rgb_colorex;

    commands[vm_INPUT_BOX] = vm_inputbox;
    commands[MESSAGE_BOX] = message_box;

    commands[V_SYSTEM]=system;
    commands[V_SYSTEMSTR]=systemstr;
    commands[V_REGISTEROBJECT]=registerobject;
    commands[V_UNREGISTEROBJECT]=unregisterobject;
    commands[V_SETCAPTURE]=setcapture;
    commands[V_RELEASECAPTURE]=releasecapture;

    commands[V_CREATESTREAM]=v_createstream;
    commands[V_CLOSESTREAM]=v_closestream;
    commands[V_SEEK]=v_seek;
    commands[V_STREAMSTATUS]=v_streamstatus;
    commands[V_EOF]=v_eof;
    commands[V_GETPOS]=v_getpos;
    commands[V_GETSIZE]=v_getsize;
    commands[V_SETWIDTH]=v_setwidth;
    commands[V_FREAD]=v_fread;
    commands[V_SREAD]=v_sread;
    commands[V_FWRITE]=v_fwrite;
    commands[V_SWRITE]=v_swrite;
    commands[V_GETLINE]=v_getline;

    commands[SETARROW2D]=vm_setarrow2d;
    commands[SETOBJECTATTRIBUTE2D]=setobjectattribute2d;
    commands[GETOBJECTATTRIBUTE2D]=getobjectattribute2d;

    commands[V_LOCK2D]=v_lock2d;
    commands[V_CREATEOBJECTFROMFILE]=createobjectfromfile;
    commands[CREATELINE2D]        = createline2d;
    commands[ADDPOINT2D]          = addvectorpoint2d;
    commands[DELPOINT2D]          = delvectorpoint2d;
    commands[GETBRUSHOBJECT2D]    = getbrushobject2d;
    commands[GETPENOBJECT2D]      = getpenobject2d;
    commands[GETRGNCREATEMODE]    = getrgncreatemode;
    commands[GETVECTORNUMPOINTS2D]= getvectornumpoints2d;
    commands[GETVECTORPOINT2DX]   = getvectorpoint2dx;
    commands[GETVECTORPOINT2DY]   = getvectorpoint2dy;
    commands[SETBRUSHOBJECT2D]    = setbrushobject2d;
    commands[SETPENOBJECT2D]      = setpenobject2d;
    commands[SETRGNCREATEMODE]    = setrgncreatemode;
    commands[SETVECTORPOINT2D]    = setvectorpoint2d;
    commands[CREATEPEN2D]         = createpen2d;
    commands[GETPENCOLOR2D]       = getpencolor2d;
    commands[GETPENROP2D]         = getpenrop2d;
    commands[GETPENSTYLE2D]       = getpenstyle2d;
    commands[GETPENWIDTH2D]       = getpenwidth2d;
    commands[SETPENCOLOR2D]       = setpencolor2d;
    commands[SETPENROP2D]         = setpenrop2d;
    commands[SETPENSTYLE2D]       = setpenstyle2d;
    commands[SETPENWIDTH2D]       = setpenwidth2d;
    commands[DELETETOOL2D]         = deletetool2d;
    commands[VM_GETTOOLREF2D]     = gettoolref2d;
    commands[DELETEOBJECT2D]      = deleteobject2d;
    commands[GETOBJECTORG2DX]     = getobjectorg2dx;
    commands[GETOBJECTORG2DY]     = getobjectorg2dy;
    commands[GETOBJECTPARENT2D]   = getobjectparent2d;
    commands[GETOBJECTANGLE2D]    = getobjectangle2d;
    commands[GETOBJECTSIZE2DX]   = getobjectsize2dx;
    commands[GETOBJECTSIZE2DY]    = getobjectsize2dy;
    commands[GETOBJECTTYPE]     = getobjecttype;
    commands[SETOBJECTORG2D]   = setobjectorg2d;
    commands[SETOBJECTSIZE2D]    = setobjectsize2d;
    commands[CREATEBRUSH2D]     = createbrush2d;
    commands[SETPOINTS2D]     = setvectorpoints2d;

    commands[CREATEDID2D]        =createdib2d;
    commands[CREATEDOUBLEDID2D]  =createdoubledib2d;
    commands[CREATERDID2D]       =createrdib2d;
    commands[CREATERDOUBLEDID2D] =createrdoubledib2d;
    commands[CREATEBITMAP2D]     =createbitmap2d;
    commands[CREATEDOUBLEBITMAP2D]=createdoublebitmap2d;
    commands[VM_SETCRDSYSTEM]=vm_setcrdsystem;
    commands[GETSPACEORGY]=getspaceorgy;
    commands[GETSPACEORGX]=getspaceorgx;
    commands[SETSPACEORG2D]=setspaceorg2d;
    commands[SETSCALESPACE2D]= setscalespace2d;
    commands[GETSCALESPACE2D]=getscalespace2d;
    commands[SNDPLAYSOUND]=v_sndplaysound;

    commands[GETBOTTOMOBJECT2D]=getbottomobject2d;
    commands[GETUPPEROBJECT2D]=getupperobject2d;
    commands[GETOBJECTFROMZORDER2D]=getobjectfromzorder2d;
    commands[GETLOWEROBJECT2D]=getlowerobject2d;
    commands[GETTOPOBJECT2D]=gettopobject2d;
    commands[GETZORDER2D]=getzorder2d;
    commands[OBJECTTOBOTTOM2D]=objecttobottom2d;
    commands[OBJECTTOTOP2D]=objecttotop2d;
    commands[SETZORDER2D]=setzorder2d;
    commands[SWAPOBJECT2D]=swapobject2d;
    commands[SETBITMAPSRCRECT]=setbitmapsrcrect;
    commands[GETBITMAPSRCRECT]=getbitmapsrcrect;

    commands[CREATEFONT2D] = createfont2d;
    commands[CREATESTRING2D]=createstring2d;
    commands[CREATETEXT2D]=createtext2d;
    commands[CREATERASTERTEXT2D]=createrastertext2d;
    commands[SETSHOWOBJECT2D]=setshowobject2d;
    commands[SETLOGSTRING2D]=setlogstring2d;
    commands[GETLOGSTRING2D]=getlogstring2d;

    commands[VM_SETBKBRUSH]= setbkbrush;
    commands[VM_GETBKBRUSH]= getbkbrush;

    commands[ADDGROUPITEM2D]=addgroupitem2d;
    commands[CREATEGROUP2D]=creategroup2d;
    commands[DELETEGROUP2D]=deletegroup2d;
    commands[DELGROUPITEM2D]=delgroupitem2d;
    commands[GETGROUPITEM2D]=getgroupitem2d;
    commands[GETGROUPITEMS2D]=getgroupitems2d;
    commands[GETGROUPITEMSNUM2D]=getgroupitemsnum2d;
    commands[ISGROUPCONTAINOBJECT2D]=isgroupcontainobject2d;
    commands[SETGROUPITEM2D]=setgroupitem2d;
    commands[SETGROUPITEMS2D]=SetGroupItems2d;
    commands[COPYTOCLIPBOARD2D]=copytoclipboard2d;
    commands[PASTEFROMCLIPBOARD2D]=pastefromclipboard2d;
    commands[SETSTATUSTEXT]=setstatustext;
    commands[GETOBJECTNAME2D]=v_getobjectname2d;
    commands[SETOBJECTNAME2D]=v_setobjectname2d;
    commands[GETOBJECTFROMPOINT2D]=v_getobjectfrompoint2d;
    commands[GETOBJECTFROMPOINT2DEX]=v_getobjectfrompoint2dex;

    commands[GETLASTPRIMARY]=vm_GetPrimaryFromPoint2d;
    commands[ROTATEOBJECT2D]=rotateobject2d;
    commands[SHOWOBJECT2D]=showobject2d;
    commands[HIDEOBJECT2D]=hideobject2d;
    commands[STDHYPERJUMP]=vm_stdhyperjump;
    commands[GETCHILDCOUNT   ]= getchildcount;
    commands[GETCHILDIDBYNUM ]= getchildidbynum;
    commands[GETCHILDCLASS   ]= getchildclass;
    commands[VM_CREATECONTROL2D ]=  vm_createcontrol2d;
    commands[VM_GETCONTROLTEXT  ]=  vm_getcontroltext;
    commands[VM_SETCONTROLTEXT  ]=  vm_setcontroltext;
    commands[VM_CHECKDLGBUTTON  ]=  vm_checkdlgbutton;
    commands[VM_ISDLGBUTTONCHECKED]=vm_isdlgbuttonchecked;
    commands[VM_ENABLECONTROL   ]=  vm_enablecontrol;
    commands[SETSCROLLRANGE]=setscrollrange;


    commands[GETBRUSHCOLOR2D  ]=getbrushcolor2d;
    commands[GETBRUSHROP2D    ]=getbrushrop2d;
    commands[GETBRUSHDIB2D    ]=getbrushdib2d;
    commands[GETBRUSHSTYLE2D  ]=getbrushstyle2d;
    commands[GETBRUSHHATCH2D  ]=getbrushhatch2d;
    commands[SETBRUSHCOLOR2D  ]=setbrushcolor2d;
    commands[SETBRUSHROP2D    ]=setbrushrop2d;
    commands[SETBRUSHDIB2D    ]=setbrushdib2d;
    commands[SETBRUSHSTYLE2D  ]=setbrushstyle2d;
    commands[SETBRUSHHATCH2D  ]=setbrushhatch2d;
    commands[V_GETCLASS]=v_getclass;

    commands[VM_ADDCHILDREN]= vm_addchildren;
    commands[VM_REMOVECHILDREN]= vm_removechildren;
    commands[VM_CREATECLASS]= vm_createclass;
    commands[VM_DELETECLASS]= vm_deleteclass;
    commands[VM_OPENCLASSSCHEME]= vm_openclassscheme;
    commands[VM_CLOSECLASSSCHEME]= vm_closeclassscheme;
    commands[VM_CREATELINK]= vm_createlink;
    commands[VM_SETLINKVARS]= vm_setlinkvars;
    commands[VM_REMOVELINK]= vm_removelink;
    commands[VM_GETUNIQUCLASSNAME]= vm_getuniqueclassname;
    commands[V_GETNAMEBYHSP]=v_getnamebyhsp;
    commands[V_GETHSPBYNAME]=v_gethspbyname;
    commands[V_SETCLIENTSIZE]=v_setclientsize;
    commands[VM_CREATELINK2]= vm_createlink2;
    commands[VM_GETLINK]=vm_getlink;
    commands[VM_SETMODELTEXT] = vm_setmodeltext;
    commands[VM_GETMODELTEXT] = vm_getmodeltext;

    commands[V_GETSCHEMEOBJECT]=vm_getschemeobject;
    commands[VM_SETVARTODEF]=v_settodef;
    commands[VM_LOADOBJECTSTATE ]=v_loadobjectstate;
    commands[VM_SAVEOBJECTSTATE ]=v_saveobjectstate;
    commands[VM_GETWINDOWPROP]=getwindowprop;

    commands[VM_SETCURRENTOBJ2D]=vm_setcurrentobj2d;
    commands[VM_GETCURRENTOBJ2D]=vm_getcurrentobj2d;

    commands[GETTEXTOBJECT2D ]=gettextobject2d;
    commands[GETTEXTSTRING2D ]=gettextstring2d;
    commands[GETTEXTFONT2D   ]=gettextfont2d;

    commands[V_GETVARF]=v_getvarf;
    commands[V_GETVARS]=v_getvars;
    commands[V_GETVARH]=v_getvarh;

    commands[V_SETVARF]=v_setvarf;
    commands[V_SETVARS]=v_setvars;
    commands[V_SETVARH]=v_setvarh;

    commands[SETCONTROLSTYLE2D ]= setcontrolstyle2d;
    commands[GETCONTROLSTYLE2D ]= getcontrolstyle2d;
    commands[LBADDSTRING       ]= lbaddstring;
    commands[LBINSERTSTRING    ]= lbinsertstring;
    commands[LBGETSTRING       ]= lbgetstring;
    commands[LBCLEARLIST       ]= lbclearlist;
    commands[LBDELETESTRING    ]= lbdeletestring;
    commands[LBGETCOUNT        ]= lbgetcount;
    commands[LBGETSELINDEX     ]= lbgetselindex;
    commands[LBSETSELINDEX     ]= lbsetselindex;
    commands[LBGETCARETINDEX   ]= lbgetcaretindex;
    commands[LBSETCARETINDEX   ]= lbsetcaretindex;
    commands[LBFINDSTRING      ]= lbfindstring;
    commands[LBFINDSTRINGEXACT ]= lbfindstringexact;

    commands[VFUNCTION   ]= VFunction;             // <-- new !
    commands[DLLFUNCTION ]= DLLFunction;           // <-- new !
    commands[GETELEMENT  ]= GetElement;            // <-- new !
    commands[SETELEMENT  ]= SetElement;            // <-- new !
    commands[CREATEPOLYLINE2D ]= CreatePolyLine2D; // <-- new !
    commands[EMPTYSPACE2D]=emptyspace2d;
    commands[V_SETSPACELAYERS   ]= v_setspacelayers;
    commands[V_GETSPACELAYERS   ]= v_getspacelayers;
    commands[V_GETOBJECTLAYER2D ]= v_getobjectlayer2d;
    commands[V_SETOBJECTLAYER2D ]= v_setobjectlayer2d;
    commands[VM_SCMESSAGE]=vm_scmessage;

    commands[V_ISINTERSECT2D]=v_isintersect2d;
    commands[VM_GETACTUALWIDTH ]=vm_getactualwidth;
    commands[VM_GETACTUALHEIGHT]=vm_getactualheight;
    commands[VM_GETACTUALSIZE  ]=vm_getactualsize;
    commands[VM_SAVEIMAGE      ]=vm_saverectarea;

    commands[VM_SETLOGTEXT2D]=vm_setlogtext2d;
    commands[GETTEXTFG2D]=gettextfg2d;
    commands[GETTEXTBK2D]=gettextbk2d;
    commands[PUSHPTR]=push_ptr;
    commands[PUSHPTRNEW]=push_ptr_new;

    commands[VM_GETTIME ]=vm_gettime;
    commands[VM_GETDATE]=vm_getdate;
    commands[VM_DIALOGEX]=vm_dialogex;
    commands[VM_DIALOGEX2]=vm_dialogex2;

    commands[VM_DIFF1]=vm_diff1;
    commands[VM_EQUATION]=vm_equation;
    commands[VM_DIFF2]=vm_diff2;
    commands[VM_DEQUATION]=vm_dequation;

#ifdef DBE
    for(int i=VM_BDEMIN;i<=VM_BDEMAX;i++)
        commands[i]=vm_bmdestub;
#endif

    commands[VM_SENDMESSAGE]=message2;

    commands[V_CREATEWINDOWEX]=v_createwindowex;

    commands[VM_new]=dm_new;
    commands[VM_delete]=dm_delete;
    commands[VM_VINSERT]=dm_vinsert;
    commands[VM_VDELETE]=dm_vdelete;
    commands[VM_VGETCOUNT]=dm_vgetcount;
    commands[VM_VGETTYPE]=dm_gettype;
    commands[VM_VCLEARALL]=dm_clearall;
    commands[VM_VSETf]=dm_vsetf;
    commands[VM_VSETs]=dm_vsets;
    commands[VM_VSETh]=dm_vseth;
    commands[VM_VGETf]=dm_vgetf;
    commands[VM_VGETh]=dm_vgeth;
    commands[VM_VGETs]=dm_vgets;

    commands[V_COPYBLOCK]=v_copyblock;
    commands[V_TRUNCATE]=v_truncate;

    commands[VM_SETIMAGENAME]=v_setimagename;

    commands[VM_CREATEDIR]=vm_createdir;
    commands[VM_DELETEDIR]=vm_deletedir;
    commands[VM_FILERENAME]=vm_filerename;
    commands[VM_FILECOPY]=vm_filecopy;
    commands[VM_FILEEXIST]=vm_fileexist;
    commands[VM_FILEDELETE]=vm_filedelete;
    commands[VM_GETFILELIST]=vm_getfilelist;
    commands[VM_GETDIBOBJECT2D  ]= getdibobject2d;
    commands[VM_SETDIBOBJECT2D  ]= setdibobject2d;
    commands[VM_GETDDIBOBJECT2D ]= getddibobject2d;
    commands[VM_SETDDIBOBJECT2D ]= setddibobject2d;
    commands[VM_VSTORE]=dm_save;
    commands[VM_VLOAD ]=dm_load;
    commands[REGISTEROBJECTS]=registerobjects;
    commands[UNREGISTEROBJECTS]=unregisterobjects;

    commands[V_XOR   ]= v_xor;
    commands[V_XORBIN]= v_xorbin;
    commands[V_MSORT]= v_msort; // new
    commands[V_RANDOMIZE]=v_randomize;

    Commands3d();

    //Добавил Марченко С.В.

#ifdef USEAUDIERE
    InitAudioCommands();
#endif

#ifdef USE_OGRE
    initOgreCommands();
#endif

    InitTextAnalyserCommands();
    InitNUICommands();

    commands[PLUS_STRING_FLOAT]=plus_string_float;
    commands[PLUS_FLOAT_STRING]=plus_float_string;
    commands[VM_INCREMENT_AFTER] = vm_increment_after_f;
    commands[VM_GETSCREENWIDTH]=vm_getscreenwidth;
    commands[VM_GETSCREENHEIGHT]=vm_getscreenheight;

    commands[VM_GETWORKAREAX]=vm_getworkareax;
    commands[VM_GETWORKAREAY]=vm_getworkareay;
    commands[VM_GETWORKAREAWIDTH]=vm_getworkareawidth;
    commands[VM_GETWORKAREAHEIGHT]=vm_getworkareaheight;

    commands[VM_GETKEYBOARDLAYOUT]=vm_getkeyboardlayout;
    commands[VM_SUBSTR1_STRING]=vm_substr1_string;

    commands[VM_SETWINDOWTRANSPARENT]=vm_setwindowtransparent;
    commands[VM_SETWINDOWTRANSPARENT_H]=vm_setwindowtransparent_h;
    commands[VM_SETWINDOWTRANSPARENTCOLOR]=vm_setwindowtransparentcolor;
    commands[VM_SETWINDOWTRANSPARENTCOLOR_H]=vm_setwindowtransparentcolor_h;

    commands[VM_SETWINDOWREGION]=vm_setwindowregion;
    commands[VM_SETWINDOWREGION_H]=vm_setwindowregion_h;

    commands[VM_GETTITLEHEIGHT]=vm_gettitleheight;
    commands[VM_GETSMALLTITLEHEIGHT]=vm_getsmalltitleheight;

    commands[VM_GETFIXEDFRAMEHEIGHT]=vm_getfixedframeheight;
    commands[VM_GETFIXEDFRAMEWIDTH]=vm_getfixedframewidth;
    commands[VM_GETSIZEFRAMEHEIGHT]=vm_getsizeframeheight;
    commands[VM_GETSIZEFRAMEWIDTH]=vm_getsizeframewidth;

    commands[VM_WINDOWINTASKBAR_H]=vm_windowintaskbar_h;

    commands[VM_SHOWCURSOR]=vm_showcursor;

    commands[VM_SCREENSHOT]=vm_screenshot;
    commands[VM_SCREENSHOT_FULL]=vm_screenshot_full;
    commands[VM_SCREENSHOT_DESKTOP]=vm_screenshot_desktop;
    commands[VM_SCREENSHOT_DESKTOP_FULL]=vm_screenshot_desktop_full;

    commands[VM_LOCKOBJECT2D]=vm_lockobject2d;

    commands[VM_GETVARINFOFULL]=vm_getvarinfofull;

    commands[VM_SENDSMS]=vm_sendsms;
    commands[VM_SENDMAIL]=vm_sendmail;

    commands[VM_GETFONTNAME2D]=getfontname2d;
    commands[VM_GETFONTSIZE2D]=getfontsize2d;
    commands[VM_SETFONTSIZE2D]=setfontsize2d;
    commands[VM_GETTEXTCOUNT2D]=gettextcount2d;
    commands[VM_GETFONTSTYLE2D]=getfontstyle2d;
    commands[VM_SETFONTSTYLE2D]=setfontstyle2d;
    commands[VM_SETFONTNAME2D]=setfontname2d;

    commands[VM_GETTEXTSTRING2D_I]=gettextstring2d_i;
    commands[VM_GETTEXTFONT2D_I]=gettextfont2d_i;
    commands[VM_GETTEXTFG2D_I]=gettextfg2d_i;
    commands[VM_GETTEXTBG2D_I]=gettextbg2d_i;

    commands[VM_SETTEXTSTRING2D_I]=settextstring2d_i;
    commands[VM_SETTEXTFONT2D_I]=settextfont2d_i;
    commands[VM_SETTEXTFG2D_I]=settextfg2d_i;
    commands[VM_SETTEXTBG2D_I]=settextbg2d_i;
    commands[VM_SETTEXT2D_I]=settext2d_i;

    commands[VM_CREATEFONT2DPT] = createfont2dpt;

    commands[VM_SETSTANDARTCURSOR]=vm_setstandartcursor;
    commands[VM_SETSTANDARTCURSOR_S]=vm_setstandartcursor_s;
    commands[VM_LOADCURSOR]=vm_loadcursor;
    commands[VM_LOADCURSOR_S]=vm_loadcursor_s;

    commands[VM_INC]=inc;
    commands[VM_INC_STEP]=inc_step;
    commands[VM_DEC]=dec;
    commands[VM_DEC_STEP]=dec_step;
    commands[VM_LIMIT]=limit;

    commands[VM_GETFONTLIST]=getfontlist;

    commands[VM_VSORT]=vm_vsort;
    commands[VM_VSORT_ASC]=vm_vsort_asc;
    commands[VM_VSORT_DIFFERENT]=vm_vsort_different;

    commands[VM_GETUSERKEYVALUE]=vm_getuserkeyvalue;
    commands[VM_GETUSERKEYFULLVALUE]=vm_getuserkeyfullvalue;
    commands[VM_SENDUSERRESULT]=vm_senduserresult;
    commands[VM_COPYUSERRESULT]=vm_copyuserresult;
    commands[VM_USERKEYISAUTORIZED]=vm_userkeyisautorized;
    commands[VM_USERKEYISAUTORIZED_UK]=vm_userkeyisautorized_uk;

    commands[VM_READUSERKEY]=vm_readuserkey;
    commands[VM_READPROJECTKEY]=vm_readprojectkey;
    commands[VM_GETUSERKEYVALUE_UK]=vm_getuserkeyvalue_uk;
    commands[VM_GETUSERKEYFULLVALUE_UK]=vm_getuserkeyfullvalue_uk;
    commands[VM_SENDUSERRESULT_UK]=vm_senduserresult_uk;
    commands[VM_COPYUSERRESULT_UK]=vm_copyuserresult_uk;

    commands[VM_GETTEMPDIRECTORY]=vm_gettempdirectory;

    commands[VM_GETROMDRIVENAMES]=vm_getromdrivenames;

    commands[VM_SHELLWAIT]=vm_shellwait;

    commands[VM_SETCONTROLFONT]=vm_setcontrolfont;
    commands[VM_SETCONTROLTEXTCOLOR]=vm_setcontroltextcolor;
    commands[VM_GETCONTROLTEXTLENGTH]=vm_getcontroltextlength;
    commands[VM_GETCONTROLTEXTPART]=vm_getcontroltextpart;
    commands[VM_ADDCONTROLTEXT]=vm_addcontroltext;
    commands[VM_INSERTCONTROLTEXT]=vm_insertcontroltext;

    commands[VM_SETWINDOWOWNER]=vm_setwindowowner;
    commands[VM_SETWINDOWPARENT]=vm_setwindowparent;

    commands[VM_GETPROJECTCLASSES]=vm_getprojectclasses;
    commands[VM_GETCLASSFILE]=vm_getclassfile;

    commands[VM_INSERTTEXT2D]=inserttext2d;
    commands[VM_REMOVETEXT2D]=removetext2d;
    commands[VM_ADDTEXT2D]=addtext2d;

    commands[VM_LBGETSELINDEXS]=lbgetselindexs;

    commands[VM_SETSTRINGBUFFERMODE]=vm_setStringBufferMode;

    commands[VM_SETMODELTEXTWITHOUTERROR]=vm_SetModelTextWithoutError;

    commands[VM_SENDSMSWAIT]=vm_sendsmswait;
    commands[VM_SENDMAILWAIT]=vm_sendmailwait;

    commands[VM_SETCONTROLFOCUS]=vm_setcontrolfocus;

    commands[VM_SETOBJECTALPHA2D]=vm_setobjectalpha2d;
    commands[VM_GETOBJECTALPHA2D]=vm_getobjectalpha2d;

    commands[VM_SETBRUSHPOINTS2D]=vm_setbrushpoints2d;
    commands[VM_SETBRUSHCOLORS2D]=vm_setbrushcolors2d;

    commands[VM_ENCRYPTSTREAM]=vm_encryptstream;
    commands[VM_SENDDATA]=vm_senddata;
    commands[VM_DECRYPTSTREAM]=vm_decryptstream;

    commands[VM_ROUNDT]=v_roundt;

    commands[VM_SETSPACERENDERERENGINE2D]=vm_setspacerenderengine2d;

    }

    //commands[VM_SETBRUSHOBJECT2D_HANDLE]=vm_setbrushobject2d_handle;
    //commands[VM_SETPENOBJECT2D_HANDLE]=vm_setpenobject2d_handle;

    // - конец - Добавил Марченко С.В.

    //проверяем свободные номера команд
    if(0)
    {
        FILE *in;
        if((in = fopen("c:\\commands.txt", "w+"))!= NULL)
            for(int i = 0; i < VM_MAXIMUM_code; i++)
                if(commands[i] == NULL)
                    fprintf(in, "%d\n", i);
        fclose(in);
    }
    return 1;
}
//===========================
