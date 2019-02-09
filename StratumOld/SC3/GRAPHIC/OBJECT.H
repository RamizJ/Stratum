/*
Copyright (c) 1995  Virtual World
Module Name:
         object.h
Author:
         Alexander Shelemekhov
*/

#ifndef _OBJECT_H
#define _OBJECT_H

//#include "referenc.h"
#include "collect.h"
#ifndef __GRAPH_DLL_TYPES
#include <gwin2d3d\gwintype.h>
#endif
#include <stdio.h>

class TGroup;
class _SPACE;

typedef TGroup * PGroup;
class TRotator3d;
class TMatrix2;
class TMatrix;
//*-------------------------------------*
class TObject : public _OBJECT
{
public:
    PGroup owner;
    WORD options;
    DWORD PTR;
    char * name;
    HOBJ2D _HANDLE;
    TDataItems* dataitems;

public:
    TObject();
    ~TObject();
    TObject(PStream,LONG);
    virtual void 	Store(PStream);
    virtual BOOL IsGroup(){return 0;}
    virtual BOOL Awaken(_SPACE*){return 1;}
    virtual void StoreCoordinates(PStream);
    virtual void LoadCoordinates(PStream);
    virtual void MarkToSave(int);
    virtual void SetState(WORD astate,ATTR_MODE op,pointer );
    //*-----------------------------------------*
    //2d
    virtual RECT GetRect(int FromPaint=0);
    virtual POINT2D Move2d(POINT2D);
    virtual void Rotate2d(POINT2D,gr_float);
    virtual POINT2D GetOrigin2d();
    virtual POINT2D GetSize2d();
    virtual POINT2D SetSize2d(POINT2D);
    virtual double GetAngle(){return 0;}
    //3d
    //virtual POINT3D Move3d(POINT3D);
    //virtual void Rotate3d(TRotator3d*);
    virtual BOOL GetOrg_Size3d(POINT3D &,POINT3D &);
    virtual POINT3D SetSize3d(POINT3D);
    virtual void Transform3d(TMatrix&);
    virtual TMatrix2*GetMatrix(TMatrix2*);
    //*------------------------------*
    virtual void UpdateSize(BOOL copy=1,void * space=NULL);
    virtual void    PostStore(PStream); // осуществляет запсь необязательных данных общих для детей
    virtual BOOL    ReadChunk(CHUNK&);// осуществляет чтение необязательных данных общих для детей
};
typedef TObject * PObject;


//*-------------------------------------*
class TGroup :public TObject
{
public:
    PPrimaryCollection items;

    TGroup();
    TGroup(PStream,LONG);
    ~TGroup();

    BOOL    Insert(PObject);
    C_TYPE  IndexOf(PObject);
    BOOL    Delete(C_TYPE); //nomer in collection
    //int PutItem(int,PObject); //nomer in collection
    pointer GetItem(C_TYPE);//nomer in collection
    virtual WORD WhoIsIt();
    virtual void Store(PStream);
    virtual BOOL IsGroup(){return TRUE;}
    virtual BOOL Awaken(_SPACE*);
    virtual void StoreCoordinates(PStream);
    virtual void LoadCoordinates(PStream);
    virtual void MarkToSave(int);
    virtual void UpdateSize(int copy=1,void * space=NULL);
    virtual void SetState(WORD astate,ATTR_MODE op,pointer );
};

//*-------------------------------------*
struct TRef2d{
    int  count;
    WORD type;
    _OBJECT*object;
    TRef2d*next;
    TRef2d(_OBJECT*obj);
};

class TTools2d :public _OBJECT
{
    INT16 _RefCount;
public:
    TRef2d*_RefObject;
    BYTE flag;
    INT16 IncRef(_OBJECT*);
    INT16 DecRef(_OBJECT*);
    //INT16 IncRef(){return ++_RefCount;}
    //INT16 DecRef(){if (_RefCount>0)return --_RefCount;else return (INT16)_Error(EM_referror);};
    inline BOOL IsZeroRef(){return _RefCount==0;}
    inline INT16 GetRefCount(){return _RefCount;}
    HOBJ2D _HANDLE;

    TTools2d(HOBJ2D handle = 0);
    TTools2d(PStream,LONG);
    ~TTools2d();

    virtual void Store(PStream);
    virtual BOOL Awaken(_SPACE*){return 1;};
    virtual TTools2d * GetMainObject(){return this;};
};

struct TData
{
    UINT16  id;
    UINT16  size;
    BYTE  data[1];
};
TData * MakeData(UINT16 id,UINT16 start,UINT16 size,void*far);

typedef TTools2d * PTools2d;
#endif
