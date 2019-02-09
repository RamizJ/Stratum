/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
                        matrix.h
Author:
                        Alexander Shelemekhov
*/
#ifndef ___STREAM_h
#include "stream.h"
#endif
#ifndef _collection_h
#include "collect.h"
#endif

class TMatrix
{
public:
    HGLOBAL block;
    union
    {
#ifdef WIN32
        void   * data;
        double * ddata;
#else
        void   __huge * data;
        double __huge* ddata;
#endif
    };

    long dimX,dimY; // dimension by X and Y
    long minX, minY;

    INT16 type;
    int  handle;
    BOOL changed;
    void * project;

    TMatrix(long miny, long maxy, long minx, long maxx, INT16 atype);
    TMatrix(TStream&);
    ~TMatrix();

    void Store(TStream&);

    double Get(long y, long x);
    void Put(long y, long x, double);
    long MaxX(){return dimX+minX-1;};
    long MaxY(){return dimY+minY-1;};
    double inline GET(long y,long x){return ddata[y*dimX+x];};
    void inline PUT(long y,long x,double value){ddata[y*dimX+x]=value;changed=TRUE;};

    void Resize(long y,long x);
    void Clear(double);

    void _Save(char*);

    void Done();
    void Create();
    int GetCellSize();

};

class TMatManager{
public:
    TCollection * plusItems;
    TCollection * minusItems;

    TMatrix* GetMatrix(int, BOOL error = TRUE);

    TMatManager();
    ~TMatManager();
    int Delete(int);

    int MSort(int Q,int f1, int d); // new

    int MCreate(int Q,long miny,long maxy,long minx,long maxx,int type);
    int _MCreate(int m, TMatrix* mat);
    TMatrix* CreateMatrix(int m,long y1,long y2,long x1,long x2,int type);
    int PostCreateMatrix(TMatrix*);

    int MDelete(int Q, BOOL fromdisk=TRUE);

    int MFill(int Q,double value);
    int MDiag(int Q,double value);
    int MPut(int Q,long y,long x,double value);
    double MGet(int Q,long y,long x);
    int MAddX(int Q,double value);
    int MSubX(int Q,double value)
    {
        return MAddX(Q,-value);
    }
    int MMulX(int Q,double value);
    int MDivX(int Q,double value)
    {
        if (value!=0.0)return MMulX(Q,1.0/value);
        return 0;
    }
    // Далее поэлементные операции
    int MSome(int Q1,int Q2,int Q3,int mode);
    int MAddC(int Q1,int Q2,int Q3);
    int MSubC(int Q1,int Q2,int Q3);
    int MMulC(int Q1,int Q2,int Q3);
    int MDivC(int Q1,int Q2,int Q3);
    // -----------------------------------
    int MMul(int Q1,int Q2,int Q3);
    int MTransp(int Q1,int Q2);
    double MDet(int Q);
    int MGlue(int Q1,int Q2,int Q3,long y,long x);
    int MCut(int Q1,int Q2,long y,long x,long sy,long sx);
    int MMove(int Q,long y,long x);
    int MObr(int Q1,int Q2);

    int Med(int Q);
    int MDelta(int Q);
    int MNot(int Q);
    double MSum(int Q);

    int SaveMatrixAs(int Q, char*);
    int LoadMatrix(int Q,char*);
    void SaveAllMatrix(char*);

    void    DeleteAllMatrix(void * project);
    void    DeleteMinusMatrix(void * project);

    int     MGetDim3d(int Q,HSP3D hsp3d,HOBJ2D obj);
    int     MGetBase3d(int Q,HSP3D hsp3d,HOBJ2D obj);
    int     MSetMatrix3d(int Q,HSP3D hsp3d,HOBJ2D obj);
    int     MGetMatrix3d(int Q,HSP3D hsp3d,HOBJ2D obj);

    BOOL    MTransform3d(int Q,HSP3D hsp3d,HOBJ2D obj);
    TMatrix* GetMat3d(int Q,long Y,long X,BYTE flags=0);
    // 1 возможно создание матрицы при отсутствии ее
    // 2 возможно возвращение матрицы при несовпадении размеров (объем равный)
    // 4 возвращение матрицы большего размера


    BOOL    MRotate3d(int Q,HSP3D hsp3d,HOBJ2D obj,gr_float angle);
    int     MGetPoints3d(int Q,HSP3D hsp3d,HOBJ2D obj);
    int     MSetPoints3d(int Q,HSP3D hsp3d,HOBJ2D obj);
    HOBJ2D  MMakeFace3d(HSP3D hsp3d,HOBJ2D obj,int Q,int Qc,COLORREF rgb_color,int f=0);
    int     MGetColors3d(int Q,HSP3D hsp3d,HOBJ2D obj,int from=0,int count=-1);
    int     MSetColors3d(int Q,HSP3D hsp3d,HOBJ2D obj,int from=0,int count=-1);


    BOOL    MTransformPoints3d(int Q,HSP3D hsp3d,HOBJ2D obj);
    BOOL    MRotatePoints3d(int Q,HSP3D hsp3d,HOBJ2D obj,gr_float angle);
    HOBJ2D  MSweep3d(HSP3D hsp3d,HOBJ2D old,int Qp,int Qr,double angle,
                     double x,double y,double z,INT16 asteps,COLORREF rgb_color,UINT32 flags);

    BOOL    _SetCrdSystem(HSP2D,int type,int Q);
    BOOL Error(int c,int Q=0,long y=0,long x=0);
private:
    void DeleteList(TCollection*);

};
TMatManager extern * matrixs;

void InitMatrixs();
void DoneMatrixs();


