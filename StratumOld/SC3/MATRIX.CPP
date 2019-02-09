/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
            matrix.cpp
Author:
            Alexander Shelemekhov
*/
#include <dos.h>
#include <stdlib.h>
#define FILE_ITERATOR
#include "matrix.h"
#include "matrix_h.h"
#include <math.h>
#include <mem.h>
#include "class.h"
#include <gwin2d3d\gwinfunc.h>
#include "merrors.rh"
#include "project.h"
#include "graphic\3dtools.h"

const NOT_SQUARE=1;
const OUT_OF_BOUNDS=2;
const BAD_SIZE=3;
const INVALID_SIZE=4;
const MUL_SIZE_INVALID=5;
const TO_MANYMATRIXES=6;
const TO_INVALIDMATRIXNUM=7;
const TO_INVALIDMATRIXNUM1=8;

BOOL TMatManager::Error(int c,int Q,long y,long x){
    char add[256];
    char *_add=add;
    switch(c){
        case M_OUT_OF_BOUNDS:{wsprintf(add,"( Q:%d y:%ld x:%ld )",Q,y,x);}break;
        default:_add=NULL;
    }
    return  GroupFuncError(c,M_SPRINTF,_add);
};

TMatrix::TMatrix(long miny,long maxy,long minx,long maxx,INT16 atype)
{
    dimX = (maxx - minx) + 1;
    dimY = (maxy - miny) + 1;

    minX = minx;
    minY = miny;
    type = atype;

    Create();
    Clear(0);
    project = GetCurrentProject();
};

TMatrix::TMatrix(TStream&st){

    dimY=st.ReadLong();
    dimX=st.ReadLong();
    minY=st.ReadLong();
    minX=st.ReadLong();

    type = st.ReadWord();
    st.ReadWord();  //read 0
    Create();

    project = GetCurrentProject();
    st.Read(data, dimX * dimY * GetCellSize());
    changed=0;
};

TMatrix::~TMatrix(){
    Done();
};

void TMatrix::Store(TStream&st)
{
    st.WriteLong(dimY);
    st.WriteLong(dimX);
    st.WriteLong(minY);
    st.WriteLong(minX);

    st.WriteWord(type);
    st.WriteWord(0);
    st.Write(data, dimX * dimY * GetCellSize());
};

double TMatrix::Get(long y, long x)
{
    y -= minY;
    x -= minX;

    if (x < dimX && y < dimY && x >= 0 && y >= 0)
        return GET(y, x);

    if(matrixs)
        matrixs->Error(M_OUT_OF_BOUNDS, handle, y,x);

    return 0;
};
void TMatrix::Put(long y,long x,double value){
    y-=minY;
    x-=minX;

    if (x < dimX && y < dimY && x >= 0 && y >= 0)
        PUT(y, x, value);
    else
    {
        if(matrixs)
            matrixs->Error(M_OUT_OF_BOUNDS, handle, y,x);
    }
}

void TMatrix::Resize(long/* y*/,long /*x*/){
};

void TMatrix::Clear(double value)
{
    long count = dimX * dimY;
    for(long i = 0; i < count; i++)
        ddata[i] = value;

    changed = TRUE;
};

void TMatrix::Done()
{
    if(block)
    {
        GlobalUnlock(block);
        GlobalFree(block);
    }
    else
    {
        delete data;
    }
    block=0;
    data=NULL;
};

void TMatrix::Create()
{
    block=0;
    data=NULL;

    long size = dimX * dimY * GetCellSize();
    if (size < 16384)
        data = (double*)new char[(int)size];
    else
    {
        block=GlobalAlloc(GMEM_MOVEABLE,size);
        data=GlobalLock(block);
    }
};

int TMatrix::GetCellSize()
{
    return sizeof(double);
};
/****************************************************************************
                         M A T R I X  M A N A G E R
****************************************************************************/
TMatrix* TMatManager::GetMatrix(int m,BOOL error)
{
    TCollection *items = NULL;
    if (m>0) items=plusItems;
    if (m<0) items=minusItems;
    if (m==0)return NULL;

    m = (abs(m)) - 1;
    if (m<items->count)
    {
        TMatrix* matrix=(TMatrix*)items->At(m);
        if(matrix)
        {
            return matrix;
        }
    }
    if(error)Error(M_TO_INVALIDMATRIXNUM1);
    return NULL;
};

TMatrix* TMatManager::CreateMatrix(int m,long y1, long y2, long x1, long x2, int type)
{
    long dx, dy;
    dy = (y2-y1)+1;
    dx = (x2-x1) + 1;

    if ((dy > 0) && (dx > 0) && (dx * dy < 400000L))
    {
        TMatrix* mat = new TMatrix(y1, y2, x1, x2, (INT16)type);
        mat->handle = m;
        return mat;
    }
    Error(M_INVALID_SIZE);
    return NULL;
};

int TMatManager::MCreate(int m, long y1,long y2,
                         long x1,long x2,int type)
{
    return PostCreateMatrix(CreateMatrix(m, y1, y2, x1, x2, type));
}

int TMatManager::PostCreateMatrix(TMatrix* mat)
{
    if(mat)
    {
        int Q = mat->handle;
        return _MCreate(Q == 0 ? 0 : Q, mat);  // было _MCreate(Q<0?0:Q,mat)
    }
    return 0;
}

int TMatManager::_MCreate(int m, TMatrix*mat)
{
    if(mat==NULL)
        return 0;

    if (m > 0)
    {
        m--;
        if(m < plusItems->limit)
        {
            TMatrix* _mat = (TMatrix*)plusItems->At((C_TYPE)m);
            if(_mat)
                Delete((C_TYPE)(m+1));

            plusItems->AtPut((C_TYPE)m,mat);
            mat->handle = m+1;
            return (m+1);
        }
        else
        {
            delete mat;
            Error(M_TO_INVALIDMATRIXNUM);
            return 0;
        }
    }
    else
    {
        if (m < 0)
        {
            int j = abs(m);
            while (j > minusItems->count)
                minusItems->Insert(NULL);

            if(minusItems->At((C_TYPE)(j-1)) == NULL)
            {
                minusItems->AtPut((C_TYPE)(j-1),mat);
                mat->handle = m;
            }
            else
            {
                MDelete(m, TRUE);
                minusItems->AtPut((C_TYPE)(j-1),mat);
                mat->handle = m;
            }
            return m;
        }
        for(C_TYPE i = 0; i < minusItems->count; i++)
        {
            if (!minusItems->At(i))
            {
                minusItems->AtPut(i,mat);
                mat->handle=-(i + 1);
                return -(i + 1);
            }
        }
        if(minusItems->count>=minusItems->limit)
        {
            delete mat;
            Error(M_TO_MANYMATRIXES);
            return 0;
        }
        int r = minusItems->Insert(mat);
        mat->handle = -(r+1);
        return -(r + 1);
    }
}

int TMatManager::Delete(int m)
{
    TMatrix* mat = GetMatrix(m);
    int ret = mat != 0;

    if (mat)
    {
        delete mat;
        if (m < 0)
        {
            minusItems->AtPut((C_TYPE)(abs(m)-1),NULL);
        }
        else
        {
            plusItems->AtPut((C_TYPE)(m-1),NULL);
        }
    }
    return ret;
};
TMatManager::TMatManager(){
    // _path=0;
    plusItems=new TCollection(4096,0);
    plusItems->count = 4096;

    minusItems=new TCollection(1000,0);
};

void TMatManager::DeleteList(TCollection*items)
{
    for(int i=0;i<items->count;i++)
    {
        TMatrix* matrix=(TMatrix*)items->At(i);
        if (matrix)delete matrix;
    }
    items->DeleteAll();
    delete items;
}

TMatManager::~TMatManager(){
    DeleteList(plusItems);plusItems=NULL;
    DeleteList(minusItems);minusItems=NULL;
};

int TMatManager::MDelete(int Q,BOOL fromdisk)
{
    TMatrix* mat = GetMatrix(Q);
    if (mat && fromdisk)
    {
        char s[260];
        if(mat->project)
        {
            lstrcpy(s,((TProject*)mat->project)->path);
        };
        AddSlash(s);
        char ss[10];
        wsprintf(ss,"%d_.mat",Q);
        lstrcat(s,ss);
        ScDeleteFile(s);
    }
    return Delete(Q);
}

int TMatManager::MFill(int Q,double value)
{
    TMatrix* m=GetMatrix(Q);
    if (m){m->Clear(value);return TRUE;}return FALSE;
}

int TMatManager::MDiag(int Q,double value)
{
    TMatrix* m = GetMatrix(Q);
    if (m)
    {
        m->Clear(0);
        long _min = min(m->dimX, m->dimY);
        for (long i = 0; i < _min; i++)
            m->PUT(i, i, value);
        return TRUE;
    }
    return FALSE;
};

int TMatManager::MPut(int Q,long y,long x,double value)
{
    TMatrix* m=GetMatrix(Q);
    if (m)
    {
        m->Put(y,x,value);
        return TRUE;
    }return FALSE;
};

double TMatManager::MGet(int Q,long y,long x)
{
    TMatrix* m=GetMatrix(Q);
    if (m)
    {
        return m->Get(y,x);
    }
    return 0;
};

int TMatManager::MAddX(int Q,double value)
{
    TMatrix* m=GetMatrix(Q);
    if (m)
    {
        for(long y=0;y<m->dimY;y++)
            for(long x=0;x<m->dimX;x++)
                m->PUT(y,x,m->GET(y,x)+value);

        return TRUE;
    }
    return 0;
};

int TMatManager::MMulX(int Q,double value)
{
    TMatrix* m=GetMatrix(Q);
    if (m)
    {
        for(long y = 0; y < m->dimY; y++)
            for(long x = 0; x < m->dimX; x++)
                m->PUT(y, x, m->GET(y,x) * value);
        return TRUE;
    }
    return 0;
};

int TMatManager::MSome(int Q1,int Q2,int Q3,int mode)
{
    TMatrix* m1=GetMatrix(Q1);
    TMatrix* m2=GetMatrix(Q2);
    //if (m1->X==m2->Y && m1->Y==m1->X)
    if(m1 && m2)
    {
        long Ymax, Ymin, Xmax, Xmin;

        Ymin = max(m1->minY, m2->minY);
        Xmin = max(m1->minX, m2->minX);

        Ymax=min(m1->MaxY(),m2->MaxY());
        Xmax=min(m1->MaxX(),m2->MaxX());

        if(Ymax >= Ymin && Xmax >= Xmin)
        {
            //	TMatrix* m3=GetMatrix(MCreate(Q3,Ymin,Ymax,Xmin,Xmax,0));
            TMatrix *m3 = CreateMatrix(Q3, Ymin, Ymax, Xmin, Xmax, 0);
            if(!m3)
                return 0;

            for(long y = Ymin; y <= Ymax; y++)
                for(long x = Xmin; x <= Xmax; x++)
                {
                    double a = m1->Get(y, x);
                    double b = m2->Get(y, x);
                    double t = 2;

                    switch(mode)
                    {
                        case 1:
                            t = a + b;
                            break;
                        case 2:
                            t = a - b;
                            break;
                        case 3:
                            t = a * b;
                            break;
                        case 4:
                            if (b != 0)
                                t = a / b;
                            else
                                t = 0;
                            break;
                    }
                    m3->Put(y, x, t);
                }
            return PostCreateMatrix(m3);
        }
    }
    return 0;
}

int TMatManager::MAddC(int Q1,int Q2,int Q3)
{
    return MSome(Q1,Q2,Q3,1);
}

int TMatManager::MSubC(int Q1,int Q2,int Q3)
{
    return MSome(Q1,Q2,Q3,2);
}

int TMatManager::MMulC(int Q1,int Q2,int Q3)
{
    return MSome(Q1,Q2,Q3,3);
}

int TMatManager::MDivC(int Q1,int Q2,int Q3)
{
    return MSome(Q1,Q2,Q3,4);
}

int TMatManager::MMul(int Q1,int Q2,int Q3)
{
    TMatrix* m1=GetMatrix(Q1);
    TMatrix* m2=GetMatrix(Q2);
    if(m1 && m2)
    {
        if (m1->dimX==m2->dimY)
        {
            long sizex = m2->dimX;
            long sizey = m1->dimY;
            // TMatrix* m3=GetMatrix(MCreate(Q3,0,sizey-1,0,sizex-1,0));
            TMatrix *m3 = CreateMatrix(Q3, 0, sizey-1, 0, sizex-1, 0);
            if(!m3)
                return 0;

            for(long y = 0; y < sizey; y++)
                for(long x = 0; x < sizex; x++)
                {
                    double t=0;
                    for(long i = 0; i < m1->dimX; i++)
                        t = t + m1->GET(y, i) * m2->GET(i, x);

                    m3->PUT(y, x, t);
                }
            m3->minX = m1->minX;
            m3->minY = m1->minY;
            return PostCreateMatrix(m3);;
        }
        else
        {
            Error(M_MUL_SIZE_INVALID);
        }
    }
    return 0;
}

int TMatManager::MTransp(int Q1,int Q2)
{
    TMatrix* m = GetMatrix(Q1);
    if (m)
    {
        if(Q1==Q2)
        {
            for(long y=0;y<m->dimY;y++)
                for(long x = y+1; x < m->dimX; x++)
                {
                    double d=m->GET(y,x);
                    m->PUT(y,x,m->GET(x,y));
                    m->PUT(x,y,d);
                }
            return Q1;
        }
        else
        {
            TMatrix* mt = GetMatrix(MCreate(Q2, m->minX, m->dimX, m->minY, m->dimY,0));
            mt->minX = m->minY;
            mt->minY = m->minX;

            for(long y=0;y<m->dimY;y++)
                for(long x=0;x<m->dimX;x++)
                    mt->PUT(x,y,m->GET(y,x));

            return mt->handle;
        }
    }
    return 0;
}

double TMatManager::MDet(int Q)
{
    TMatrix* m=GetMatrix(Q);
    if (m)
    {
        if(m->dimX != m->dimY)
        {
            Error(M_NOT_SQUARE);
            return 0;
        }

        HGLOBAL block = 0;
        void * data;//=NULL;
        long size = m->dimX * m->dimY * m->GetCellSize();
        if (size < 16384)
            data=(double*)new char[(int)size];
        else
        {
            block = GlobalAlloc(GMEM_MOVEABLE,size);
            data = GlobalLock(block);
        }
#ifdef WIN32
        memcpy(data,m->data,size);
#else
        hmemcpy(data,m->data,size);
#endif
        long n = m->dimY, i,j,k;
        double t, d = 1, max;

        for(k = 0; k < n; k++)
        {
            max=0;
            for(i = k; i < n; i++)
            {
                t = m->GET(i, k);
                if (fabs(t) > fabs(max))
                {
                    max = t;
                    j = i;
                }
            }
            if (max == 0)
            {
                d = 0;
                goto fin;
            }
            if (j != k)
            {
                d = -d;
                for(i = k; i < n; i++)
                {
                    t = m->GET(j, i);
                    m->PUT(j, i, m->GET(k, i));
                    m->PUT(k, i, t);
                }
            }
            for(i=k+1;i<n;i++)
            {
                t = m->GET(i, k) / max;
                for(j = k + 1; j < n; j++)
                {
                    m->PUT(i, j, m->GET(i, j) - t * m->GET(k, j));
                }
            }//end i
            d = d * m->GET(k, k);
        }//end k
#ifdef WIN32
        memcpy(m->data, data, size);
#else
        hmemcpy(m->data,data,size);
#endif
        if(block)
        {
            GlobalUnlock(block);
            GlobalFree(block);
        }
        else
        {
            delete data;
        }
        // block=0;data=NULL;
fin:
        return d;
    }
    return 0;
}

int TMatManager::MGlue(int Q1,int Q2,int Q3,long y,long x)
{
    TMatrix* m1=GetMatrix(Q1);
    TMatrix* m2=GetMatrix(Q2);

    if (m1 && m2)
    {
        long minx=m2->minX+x;
        long maxx=m2->MaxX()+x;
        long miny=m2->minY+y;
        long maxy=m2->MaxY()+y;

        long Ymax, Ymin, Xmax, Xmin;
        Xmin=min(m1->minX,minx);
        Xmax=max(m1->MaxX(),maxx);
        Ymin=min(m1->minY,miny);
        Ymax=max(m1->MaxY(),maxy);

        //TMatrix* m3=GetMatrix(MCreate(Q3,Ymin,Ymax,Xmin,Xmax,0));
        TMatrix *m3=CreateMatrix(Q3,Ymin,Ymax,Xmin,Xmax,0);
        if (m3)
        {
            {
                for(long i=0;i<m2->dimY;i++)
                    for(long j=0;j<m2->dimX;j++)
                        m3->Put(i+miny,j+minx,m2->GET(i,j));
            }
            {
                for(long i=0;i<m1->dimY;i++)
                    for(long j=0;j<m1->dimX;j++)
                        m3->Put(i+m1->minY,j+m1->minX,m1->GET(i,j));
            }
            return PostCreateMatrix(m3);
        }
    }
    return 0;
};

TMatrix* TMatManager::GetMat3d(int Q,long Y,long X,BYTE flags){
    TMatrix* m=GetMatrix(Q);
    if (flags&1){
        if (!(m && m->dimX==X && m->dimY==Y)){
            m=GetMatrix(MCreate(Q,0,Y-1,0,X-1,0));
        }}else{
        if(m){
            if (m->dimX!=X && m->dimY!=Y){
                if(flags&2){ if(X*Y==(m->dimX*m->dimY))return m; }
                if(flags&4){ if(X*Y<(m->dimX*m->dimY))return m; }
                Error(M_BAD_SIZE);
                return NULL;
            }
        }}
    return m;
};
BOOL TMatManager::_SetCrdSystem(HSP2D hsp,int type,int Q){
    CRDSYSTEM2D c;
    c.hObject=0;
    c.type=type;
    TMatrix* m=GetMat3d(Q,3,3,0);
    if(m){
        memcpy(&c.matrix,m->data,sizeof(c.matrix));
        c.type=MATRIX_COORDINATE2D;
    };
    return SetCrdSystem2d(hsp,&c);
};
int TMatManager::MGetDim3d(int Q,HSP3D hsp3d,HOBJ2D obj){
    POINT3D dim[2];
    if(GetObjectOrgSize3d(hsp3d,obj,dim,dim+1)){
        TMatrix* m=GetMat3d(Q,2,3,1);
        if (m){
            memcpy(m->data,&dim,sizeof(dim));
            return m->handle;
        }}return 0;
};
int TMatManager::MGetMatrix3d(int Q,HSP3D hsp3d,HOBJ2D obj){
    double mat[16];
    if(GetMatrix3d(hsp3d,obj,mat)){
        TMatrix* m = GetMat3d(Q,4,4,1);
        if (m){
            memcpy(m->data,mat,sizeof(mat));
            return m->handle;
        }}
    return 0;
};

int TMatManager::MSetMatrix3d(int Q,HSP3D hsp3d,HOBJ2D obj){
    TMatrix* m = GetMat3d(Q,4,4,0);
    if(m){
        return SetMatrix3d(hsp3d,obj,m->data);
    }
    return 0;
};

int TMatManager::MGetBase3d(int Q,HSP3D hsp3d,HOBJ2D obj){
    POINT3D base;
    if(GetObjectBase3d(hsp3d,obj,&base)){
        TMatrix* m=GetMat3d(Q,1,3,1);
        if (m){
            memcpy(m->data,&base,sizeof(base));
            return m->handle;
        }}
    return 0;
};



int TMatManager::MGetPoints3d(int Q,HSP3D hsp3d,HOBJ2D obj){
    INT16 count=GetNumPoints3d(hsp3d,obj);
    if(count>0){
        TMatrix* m=GetMat3d(Q,count,3,1);
        if (m){
            GetPoints3d(hsp3d,obj,(POINT3D*)m->data,0,count);
            return m->handle;
        }
    }return 0;
};
int  TMatManager::MGetColors3d(int Q,HSP3D hsp3d,HOBJ2D obj,int from,int count){
    INT16 _count=GetNumPrimitives3d(hsp3d,obj);
    count=(count>0)?min((_count-from),count):_count;
    if(count>0 && from>-1){
        TMatrix* m=GetMat3d(Q,count,1,1);
        if (m){
            PRIMITIVE3D * p=new PRIMITIVE3D[count];
            GetPrimitives3d(hsp3d,obj,p,from,count,NULL,0,NULL,0);
            for(int i=0;i<count;i++)m->ddata[i]=p[i].color.rgb;
            delete p;
            return m->handle;
        }
    }return 0;
};
int  TMatManager::MSetColors3d(int Q,HSP3D hsp3d,HOBJ2D obj,int from,int count){
    INT16 _count=GetNumPrimitives3d(hsp3d,obj);
    count=(count>0)?min((_count-from),count):_count;
    if(count>0 && from>-1){
        TMatrix* m=GetMat3d(Q,count,1,6);
        if (m){
            PRIMITIVE3D * p=new PRIMITIVE3D[count];
            GetPrimitives3d(hsp3d,obj,p,from,count,NULL,0,NULL,0);
            for(int i=0;i<count;i++)p[i].color.rgb=m->ddata[i];
            SetPrimitives3d(hsp3d,obj,p,from,count);
            delete p;
            return 1;
        }
    }return 0;
};

HOBJ2D TMatManager::MMakeFace3d(HSP3D hsp3d,HOBJ2D obj,int Q,int Qc,COLORREF rgb_color,int flags){
    BOOL was=obj!=0;
    TMatrix* points=GetMatrix(Q);
    TMatrix* colors=GetMatrix(Qc);
    long _cx,_cy;
    if (points){
        if (colors){
            if ((points->dimY!=colors->dimY)||((points->dimX/3)!=colors->dimX))return 0;
            _cx=colors->minX;
            _cy=colors->minY;
        }
        int y=points->dimY;
        int x=points->dimX/3;
        int count=(x-1)*(y-1);

        if (x>1 && y>1 && count<16000){
            PRIMITIVE3D * p=new PRIMITIVE3D[count];
            memset(p,0,sizeof(PRIMITIVE3D)*count);
            INT16 *n=new INT16[count*4];

            PRIMITIVE3D* prim=p;
            INT16 *nums=n;
            if(!was || flags&2)
                for(int i=0;i<y-1;i++)
                {
                    for(int j=0;j<x-1;j++){
                        prim->count=4;
                        prim->flags|=PRIMITIVE_POLYGON|PRIMITIVE_ALWAYS;
                        prim->points=nums;
                        nums[0]=j*y+i;
                        nums[1]=(j+1)*y+i;
                        nums[2]=(j+1)*y+i+1;
                        nums[3]=(j)*y+i+1;
                        if (colors){

                            COLORREF c1=colors->Get(i+_cy,j+_cx);
                            COLORREF c2=colors->Get(i+1+_cy,j+_cx);
                            COLORREF c3=colors->Get(i+1+_cy,j+1+_cx);
                            COLORREF c4=colors->Get(i+_cy,j+1+_cx);

                            BYTE cr=((double)(GetRValue(c1)+GetRValue(c2)+GetRValue(c3)+GetRValue(c4)))/4;
                            BYTE cg=((double)(GetGValue(c1)+GetGValue(c2)+GetGValue(c3)+GetGValue(c4)))/4;
                            BYTE cb=((double)(GetBValue(c1)+GetBValue(c2)+GetBValue(c3)+GetBValue(c4)))/4;
                            COLORREF rgb=RGB(cr,cg,cb);
                            prim->color.rgb=rgb;
                        }else prim->color.rgb=rgb_color;
                        prim++;
                        nums+=4;

                    }}
            if(!obj)obj=CreateObject3d(hsp3d);
            POINT3D *_points=(POINT3D*)points->data;

            if(was){
                if(flags&1)SetPoints3d(hsp3d,obj,_points,0,x*y);
            }else AddPoints3d(hsp3d,obj,_points,x*y);
            if(was){
                if(flags&2)SetPrimitives3d(hsp3d,obj,p,0,count);
            }else CreatePrimitives3d(hsp3d,obj,p,count);

            delete n;
            delete p;
            return obj;
        }
    }return 0;
};
HOBJ2D TMatManager::MSweep3d(HSP3D hsp3d,HOBJ2D old,int Qp,int Qr,double angle,
                             double x,double y,double z,INT16 asteps,COLORREF rgb_color,UINT32 flags){
    TMatrix* points=GetMatrix(Qp);
    TMatrix* _r=GetMat3d(Qr,2,3,0);
    if(points && points->dimX==3){
        ROTATOR3D r;
        ROTATOR3D* r_=NULL;
        if(_r){
            memcpy(&r,_r->data,sizeof(double)*2*3);
            r.Angle=angle;
            r_=&r;
        }
        POINT3D delta;
        POINT3D* _delta=NULL;
        if(x!=0.0 || y!=0 || z!=0){
            delta.x=x;
            delta.y=y;
            delta.z=z;
            _delta=&delta;
        };
        COLOR3D color;
        color.rgb=rgb_color;
        if (InitTools3d()){
            return MakeFace3d(hsp3d,old,(POINT3D*)points->data,points->dimY,r_,
                              _delta,asteps,color,NULL,
                              NULL,flags);
        }
    }
    return 0;
}
int TMatManager::MSetPoints3d(int Q,HSP3D hsp3d,HOBJ2D obj){
    INT16 count=GetNumPoints3d(hsp3d,obj);
    if(count>0){
        TMatrix* m=GetMatrix(Q);
        if (m){
            INT32 _c=m->dimX*m->dimY/3;
            if (_c<=count){
                return(SetPoints3d(hsp3d,obj,(POINT3D*)m->data,0,_c));
            }
        }
    }return 0;
};

BOOL TMatManager::MTransform3d(int Q,HSP3D hsp3d,HOBJ2D obj){
    TMatrix* m=GetMat3d(Q,4,4,0);
    if (m){
        return TransformObject3d(hsp3d,obj,m->ddata);
    }return 0;
};

BOOL TMatManager::MTransformPoints3d(int Q,HSP3D hsp3d,HOBJ2D obj){
    TMatrix* m=GetMat3d(Q,4,4,0);
    if (m){
        return TransformObjectPoints3d(hsp3d,obj,m->ddata);
    }return 0;

};
BOOL TMatManager::MRotatePoints3d(int Q,HSP3D hsp3d,HOBJ2D obj,gr_float angle){
    TMatrix* m=GetMat3d(Q,2,3,0);
    if (m){
        ROTATOR3D r;
        memcpy(&r,m->data,sizeof(double)*2*3);
        r.Angle=angle;
        return RotateObjectPoints3d(hsp3d,obj,&r);
    }return 0;
};


BOOL TMatManager::MRotate3d(int Q,HSP3D hsp3d,HOBJ2D obj,gr_float angle){
    TMatrix* m=GetMat3d(Q,2,3,0);
    if (m){
        ROTATOR3D r;
        memcpy(&r,m->data,sizeof(double)*2*3);
        r.Angle=angle;
        return RotateObject3d(hsp3d,obj,&r);
    }return 0;
};

int TMatManager::MCut(int Q1,int Q2,long y,long x,long sy,long sx)
{
    TMatrix* m1=GetMatrix(Q1);
    if (m1)
    {
        long dx = x - m1->minX;
        long dy = y - m1->minY;

        sy = min(sy, m1->dimY - dy);
        sx = min(sx, m1->dimX - dx);

        //	TMatrix* m2=GetMatrix(MCreate(Q2,y,y+sy-1,x,sx+x-1,0));
        TMatrix *m2=CreateMatrix(Q2, y, y + sy - 1, x, sx + x - 1, 0);
        if (m2)
        {
            for(long i = 0; i < sy; i++)
                for(long j = 0; j < sx; j++)
                {
                    m2->PUT(i, j, m1->GET(i + dy, j + dx));
                }
        }
        //	return m2->handle;
        return PostCreateMatrix(m2);
    }
    return 0;
}

int TMatManager::MMove(int Q,long y,long x)
{
    TMatrix* m1=GetMatrix(Q);
    if (m1)
    {
        m1->minX = x;
        m1->minY = y;
        return 1;
    }
    return 0;
}

int TMatManager::MObr(int Q1,int Q2)
{
    TMatrix* m1=GetMatrix(Q1);
    int rez=0;
    if (m1)
    {
        long n, s, i, j, k, m;
        double t;
        n=m1->dimY;
        m=2*n;s=0;
        TMatrix* mt=new TMatrix(0, n-1, 0, m-1, 0);

        // наращивание матрицы еденичной матрицей

        for(i=0;i<n;i++)
            for(j=0;j<m;j++)
                mt->PUT(i,j,(j<n)?(m1->GET(i,j)):((j==n+i)?(1.0):(0.0)));

        // начало обращения
        for(i=0;i<n;i++)
        {
            k=i;
test0:
            if (mt->GET(k,i)==0)
            {s=1;
                if (k<n-1) k=k+1;else goto fin;
                goto test0;
            }

            if (s==1)
                for(j=0;j<m;j++){
                    t=mt->GET(k,j);mt->PUT(k,j,mt->GET(i,j));mt->PUT(i,j,t);
                }

            for(j=m-1;j>=i;j--)mt->PUT(i,j,mt->GET(i,j)/mt->GET(i,i));

            for(k=0;k<n;k++)
                if (k!=i)
                    for(j=m-1;j>=i;j--) mt->PUT(k,j,mt->GET(k,j)-mt->GET(i,j)*mt->GET(k,i));

        }//end i
        {
            TMatrix* m2=GetMatrix(MCreate(Q2,m1->minY,m1->minY+m1->dimY-1,m1->minX,m1->minX+m1->dimX-1,0));
            if(m2){
                for(i=0;i<n;i++)
                    for(j=0;j<n;j++)
                        m2->PUT(i,j,mt->GET(i,j+n));
                rez=m2->handle;
            }
        }
fin:
        delete mt;
    }
    return rez;
};

int TMatManager::Med(int Q)
{
    TMatrix* m=GetMatrix(Q);
    if (m){
        for(long y=0;y<m->dimY;y++)
            for(long x=0;x<m->dimX;x++){
                m->PUT(y,x,m->GET(y,x)<=0?0:1);
            }
        return TRUE;
    }
    return 0;
}

int TMatManager::SaveMatrixAs(int Q,char*name)
{
    TMatrix* m=GetMatrix(Q);
    if (m)
    {
        TDOSStream st(name,TDOSStream::stCreate);
        if (st.status)
            return 0;
        st.WriteStr("MATRIX FILE.");
        m->Store(st);
        if (st.status)
            return 0;
    }
    return 1;
};

int TMatManager::LoadMatrix(int Q,char*name)
{
    TMatrix* m = GetMatrix(Q, FALSE);
    if(m)
        Delete(Q);

    TDOSStream st(name,TDOSStream::stOpenRead);

    char s[80];
    st.ReadStr(s,20);
    if (!lstrcmp(s,"MATRIX FILE."))
    {
        TMatrix *mat=new TMatrix(st);
        return _MCreate(Q,mat);
    }
    else
        return 0;
};

int TMatManager::MDelta(int Q)
{
    TMatrix* m=GetMatrix(Q);
    if (m){
        for(long y=0;y<m->dimY;y++)
            for(long x=0;x<m->dimX;x++){
                m->PUT(y,x,m->GET(y,x)==0?1.0:0.0);
            }
        return TRUE;
    }
    return 0;
};
int TMatManager::MNot(int Q)
{
    TMatrix* m=GetMatrix(Q);
    if (m){
        for(long y=0;y<m->dimY;y++)
            for(long x=0;x<m->dimX;x++){
                double t=m->GET(y,x);
                if (t>0)t=0;else t=1;
                m->PUT(y,x,t);
            }
        return TRUE;
    }
    return 0;
}

double TMatManager::MSum(int Q)
{
    TMatrix* m=GetMatrix(Q);
    if (m)
    {
        double t=0;
        for(long y=0;y<m->dimY;y++)
            for(long x=0;x<m->dimX;x++)
            {
                t+=m->GET(y,x);
            }
        return t;
    }
    return 0;
}

void TMatrix::_Save(char*path)
{
    char s[256];
    char ss[10];
    wsprintf(ss,"%d_.mat",handle);
    lstrcpy(s,path);
    lstrcat(s,ss);

    TDOSStream st(s,TDOSStream::stCreate);
    if (st.status)
        return;

    st.WriteStr("MATRIX FILE.");
    Store(st);
    if (!st.status)
        changed=0;
}

void TMatManager::SaveAllMatrix(char* path)
{
    int i;
#if !defined DEMO
    for(i = 0; i < plusItems->count; i++)
    {
        TMatrix* m = (TMatrix*)plusItems->At(i);
        if (m && m->changed)
            m->_Save(path);
    }
#endif
    /*
for(i=0;i<items2->count;i++){
 TMatrix * m=(TMatrix *)items2->At(i);
 if (m && m->changed)m->_Save(path);
} */
    //if(matrixs->_path)delete(matrixs->_path);matrixs->_path=0;
};

void TMatManager::DeleteAllMatrix(void * project)
{
    INT16 i;
    for(i=0;i<plusItems->count;i++)
    {
        TMatrix * m=(TMatrix *)plusItems->At(i);
        if (m && ((m->project==project)||(project==NULL )))
        {
            delete m;
            plusItems->AtPut(i,NULL);
        }
    };
    DeleteMinusMatrix(project);

}

void TMatManager::DeleteMinusMatrix(void * project)
{
    INT16 i;
    for(i = 0; i < minusItems->count; i++)
    {
        TMatrix * m = (TMatrix *)minusItems->At(i);
        if(m && ((m->project==project)||(project==NULL )))
        {
            delete m;
            minusItems->AtPut(i,NULL);
        }
    }
    // items2->DeleteAll();
};

TMatManager * matrixs;

void InitMatrixs(){
    matrixs=new TMatManager();
};

void DoneMatrixs(){
    delete matrixs;
};
void SaveMatrix(char* name)
{
    char path_[256];
    lstrcpy(path_,name);
    AddSlash(path_);
    matrixs->SaveAllMatrix(path_);
}

void InitProjectMatrix(char*path)
{
    TFileIterator fcls(path, "*.mat");
    while (fcls.GetNext())
    {
        char * ps=fcls.GetFileName();
        int Q=0;
        char buf[256];
        char *bufp = buf;
        while (*ps)
        {
            (*bufp) = (*ps);
            if (*ps=='_')
            {
                (*bufp)=0; Q=atoi(buf);break;
            }
            ps++;bufp++;
        } //retrive matrix name
        if(Q)
        {
            char s[256];lstrcpy(s,path);
            AddSlash(s);lstrcat(s,fcls.GetFileName());
            matrixs->LoadMatrix(Q,s);
        }
    }
};

int TMatManager::MSort(int Q,int f, int d)  // new
{
    TMatrix* m=GetMatrix(Q);

    long maxi, maxj, i, ii;
    double tmp;
    if (!m)
        return 0;

    maxi=m->dimY;
    maxj=m->dimX;
    long best;

    // f1-=m->oY;
    // f2-=m->oX;

    int y = m->minY, x = m->minX;
    if (d < 3)
        y = f - m->minY;
    else
        x = f - m->minX;

    if (!(x < m->dimX && y < m->dimY && x >= 0 && y >= 0))
    {
        if (matrixs)
            matrixs->Error(M_OUT_OF_BOUNDS, Q, y,x);
        return FALSE;
    }

    if (d == 1)
    {
        f = f - m->minY;
        for(i = 0; i < maxi-1; i++)
        {
            best = 0;
            for(ii = 1; ii < maxi-i; ii++)
                if (m->GET(ii,f) > m->GET(best,f)) best = ii;
            for (ii = 0; ii < maxj; ii++)
            {
                tmp = m->GET(best,ii);
                m->PUT(best,ii,m->GET(maxi-i-1,ii));
                m->PUT(maxi-i-1,ii,tmp);
            }
        }
    }

    if (d == 2)
    {
        f = f-m->minY;
        for(i = 0; i < maxi-1; i++)
        {
            best = 0;
            for(ii = 1; ii < maxi-i; ii++) if (m->GET(ii,f) < m->GET(best,f)) best = ii;
            for (ii = 0; ii < maxj; ii++)
            {
                tmp = m->GET(best,ii);
                m->PUT(best,ii,m->GET(maxi-i-1,ii));
                m->PUT(maxi-i-1,ii,tmp);
            }
        }
    }
    if (d == 3)
    {
        f = f-m->minX;
        for(i = 0; i < maxj-1; i++)
        {
            best = 0;
            for(ii = 1; ii < maxj-i; ii++) if (m->GET(f,ii) > m->GET(f,best)) best = ii;
            for (ii = 0; ii < maxi; ii++)
            {
                tmp = m->GET(ii,best);
                m->PUT(ii,best,m->GET(ii,maxj-i-1));
                m->PUT(ii,maxj-i-1,tmp);
            }
        }
    }
    if (d == 4)
    {
        f = f-m->minX;
        for(i = 0; i < maxj-1; i++)
        {
            best = 0;
            for(ii = 1; ii < maxj-i; ii++) if (m->GET(f,ii) < m->GET(f,best)) best = ii;
            for (ii = 0; ii < maxi; ii++)
            {
                tmp = m->GET(ii,best);
                m->PUT(ii,best,m->GET(ii,maxj-i-1));
                m->PUT(ii,maxj-i-1,tmp);
            }
        }
    }
    return 1;
};

