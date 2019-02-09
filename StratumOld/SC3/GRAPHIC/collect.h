/*
Copyright (c) 1994  Virtual World
Module Name:
         collect.h
Author:
         Alexander Shelemekhov
*/
#ifndef _collection_h
#define _collection_h
#include "_object.h"
#define _fast_non_check
#ifdef WIN32
const max_size=31999;
#else
const max_size=65520u /4;
#endif
//*----------------------------------------------------*
typedef INT16 C_TYPE;
class TCollection : public _OBJECT{
public:
    C_TYPE count,delta,limit;
    BOOL static fullstore;
    pointer * items;
    TCollection()
    { items=NULL;count=0;limit=0;delta=0; }
    TCollection(C_TYPE asize,C_TYPE adelta);
    TCollection(PStream);
    ~TCollection();
    pointer At(C_TYPE index);
#ifndef _fast_non_check
#define _At(index) At(index)
#else
    inline pointer _At(register index){return items[index];}
#endif
    virtual void Error(INT16);
    void AtDelete(C_TYPE index);
    void AtFree(C_TYPE index)
    {
        FreeItem(At(index));
    }
    void AtInsert(C_TYPE index,pointer item);
    void AtPut(C_TYPE index,pointer item);
    void Delete(pointer item)
    {
        C_TYPE index=IndexOf(item);
        if (index>-1)AtDelete(index);
    }
    void DeleteAll(){count=0;}
    void FreeAll();
    void Free(pointer item)
    {
        Delete(item);
        FreeItem(item);
    };
    virtual C_TYPE IndexOf(pointer item);
    virtual C_TYPE Insert(pointer item)
    {
        AtInsert(count, item);
        return (C_TYPE)(count-(C_TYPE)1);
    }
    void Pack();
    void SetLimit(C_TYPE);
    void Swap(C_TYPE i,C_TYPE j);
    virtual WORD WhoIsIt(){return 1000;}
    virtual void Store(PStream);
    inline  BOOL IsIndexValid(register Index);
    virtual BOOL IsNeedToSave(pointer);
    C_TYPE GetCountToSave();
    virtual pointer   GetItem(PStream );
    virtual void      PutItem(PStream,pointer);
    virtual void 		FreeItem(pointer );
    void              LoadItems(PStream);
    virtual void      _store(PStream){};
};
//*----------------------------------------------------*
class TSortedCollection : public TCollection
{
public:
    unsigned short Duplicates;
    TSortedCollection(WORD asize,WORD adelta):TCollection(asize,adelta){Duplicates=1;};
    TSortedCollection(PStream);
    virtual C_TYPE Insert(pointer item);
    virtual C_TYPE IndexOf(pointer item);
    virtual BOOL   Search(pointer key,C_TYPE &index);
    virtual int    Compare(pointer key1,pointer key2){return (WORD)_abstract();};
    virtual WORD 	WhoIsIt(){return (WORD)_abstract();}
    virtual void 	_store(PStream);
};

//*----------------------------------------------------*

class THANDLECollection : public TSortedCollection
{
public:
    HOBJ2D current;// Это дескриптор

    THANDLECollection(C_TYPE asize, C_TYPE adelta) : TSortedCollection(asize, adelta)
    {
        current = OID_NONE;
    }
    THANDLECollection(PStream ps):TSortedCollection(ps){current=ps->ReadWord();}

    virtual int Compare(pointer key1,pointer key2);
    virtual WORD WhoIsIt(){return (WORD)_abstract();}
    C_TYPE InsertObject(pointer,C_TYPE start=0); // return new HANDLE
    virtual HOBJ2D GetHandle(pointer);
    pointer GetByHandle(HOBJ2D);
    virtual void SetHandle(pointer,C_TYPE);
    virtual void _store(PStream);
    int CheckForCurrent(C_TYPE);
    // Может быть вызванна для проверки того, что
    // не удалили текущую

    virtual BOOL    _DeleteObject(C_TYPE handle);
    HOBJ2D          DeleteUnused(PStream ps=NULL);
    virtual BOOL    MayDelete(pointer){return 1;};
    /*
                                                          Вызывается для проверки того , что элемент
                                                          может быть удален
                                                         */
    HOBJ2D           GetNextHandle(HOBJ2D);
};

//*----------------------------------------------------*
class TToolsCollection:public THANDLECollection
{
public:
    TToolsCollection(C_TYPE asize,C_TYPE adelta):THANDLECollection(asize,adelta)
    {}

    TToolsCollection(PStream ps):THANDLECollection(ps)
    {}

    virtual HOBJ2D 		GetHandle(pointer);
    virtual void 		SetHandle(pointer,HOBJ2D);
    virtual BOOL       MayDelete(pointer);
    virtual BOOL       IsNeedToSave(pointer);
};
//*----------------------------------------------------*
class T3dCollection:public THANDLECollection{
public:
    T3dCollection(C_TYPE asize,C_TYPE adelta):THANDLECollection(asize,adelta){};
    T3dCollection(PStream ps):THANDLECollection(ps){};
    virtual HOBJ2D  GetHandle(pointer);
    virtual void 	  SetHandle(pointer,HOBJ2D);
    virtual BOOL    MayDelete(pointer);
    virtual WORD 	  WhoIsIt(){return 1012;}
    virtual BOOL    IsNeedToSave(pointer);
};
//*----------------------------------------------------*
class TMaterialls3d:public TToolsCollection{
public:
    TMaterialls3d(C_TYPE asize,C_TYPE adelta):TToolsCollection(asize,adelta){};
    TMaterialls3d(PStream ps):TToolsCollection(ps){};
    virtual WORD 	  WhoIsIt(){return otMATERIALS3D;}
};
//*----------------------------------------------------*
class TPenCollection:public TToolsCollection
{
public:
    TPenCollection(C_TYPE asize,C_TYPE adelta) : TToolsCollection(asize,adelta)
    {};

    TPenCollection(PStream ps) : TToolsCollection(ps)
    {};

    virtual WORD WhoIsIt(){return 1004;}
};
//*----------------------------------------------------*
class TBrushCollection:public TToolsCollection{
public:
    TBrushCollection(WORD asize,WORD adelta):TToolsCollection(asize,adelta){};
    TBrushCollection(PStream ps):TToolsCollection(ps){};
    virtual WORD 	  WhoIsIt(){return 1005;}
};

//*----------------------------------------------------*
class TDibCollection:public TToolsCollection
{
public:
    TDibCollection(C_TYPE asize,C_TYPE adelta) :
        TToolsCollection(asize,adelta)
    {}

    TDibCollection(PStream ps) : TToolsCollection(ps)
    {}
    virtual WORD 		WhoIsIt(){return 1006;}
};

//*----------------------------------------------------*
class TDoubleDibCollection:public TToolsCollection{
public:
    TDoubleDibCollection(C_TYPE asize,C_TYPE adelta):TToolsCollection(asize,adelta){};
    TDoubleDibCollection(PStream ps):TToolsCollection(ps){};
    virtual WORD 	  WhoIsIt(){return 1007;}
};
//*----------------------------------------------------*
class TFontCollection:public TToolsCollection{
public:
    TFontCollection(WORD asize,WORD adelta):TToolsCollection(asize,adelta){};
    TFontCollection(PStream ps):TToolsCollection(ps){};
    virtual WORD 		WhoIsIt(){return 1008;}
};
//*----------------------------------------------------*
class TStringCollection:public TToolsCollection{
public:
    TStringCollection(WORD asize,WORD adelta):TToolsCollection(asize,adelta){};
    TStringCollection(PStream ps):TToolsCollection(ps){};
    virtual WORD 	  WhoIsIt(){return 1009;}
};
//*----------------------------------------------------*
class TTextCollection:public TToolsCollection{
public:
    TTextCollection(WORD asize,WORD adelta):TToolsCollection(asize,adelta)
    {}

    TTextCollection(PStream ps):TToolsCollection(ps)
    {}

    virtual WORD 	  WhoIsIt(){return 1010;}
};

class TLOGTextCollection:public TCollection
{
public:
    TLOGTextCollection(WORD asize,WORD adelta):TCollection(asize,adelta)
    {}

    TLOGTextCollection(PStream ps):TCollection(ps)
    {}

    virtual WORD 	  WhoIsIt(){return 1011;}
    virtual void      FreeItem(pointer);
    virtual pointer   GetItem(PStream );
    virtual void      PutItem(PStream,pointer);
};

//*----------------------------------------------------*
class TObjectCollection : public THANDLECollection
{
public:
    TObjectCollection(WORD asize,WORD adelta) : THANDLECollection(asize,adelta)
    {}
    TObjectCollection(PStream ps) : THANDLECollection(ps)
    {}

    virtual WORD  WhoIsIt(){return 1020;}
    virtual HOBJ2D GetHandle(pointer);
    virtual void SetHandle(pointer,HOBJ2D);
    virtual BOOL    IsNeedToSave(pointer);
};

class TPrimaryCollection:public TCollection
{
public:
    TPrimaryCollection(WORD asize,WORD adelta) : TCollection(asize,adelta)
    {}

    TPrimaryCollection(PStream ps) : TCollection(ps)
    {}

    virtual pointer   GetItem(PStream );
    virtual void      PutItem(PStream, pointer);
    virtual WORD 	  WhoIsIt(){return 1021;}
    virtual BOOL      IsNeedToSave(pointer);
};

class TDataItems : public TSortedCollection
{
public:
    TDataItems():TSortedCollection(5,2){}
    TDataItems(PStream ps):TSortedCollection(ps){}
    ~TDataItems(){FreeAll();}

    virtual int Compare(pointer key1,pointer key2);
    virtual pointer   GetItem(PStream );
    virtual void      PutItem(PStream,pointer);
    virtual void      FreeItem(pointer);
    virtual WORD 	    WhoIsIt(){return 1022;}
};
class TItems:public TCollection{
public:
    TItems(C_TYPE c1,C_TYPE c2):TCollection(c1,c2){};
    virtual void      FreeItem(pointer p){delete p;};
    virtual WORD 	    WhoIsIt(){return 1023;}
};
class T3dObjects:public TCollection{
public:
    T3dObjects(C_TYPE c1,C_TYPE c2):TCollection(c1,c2){};
    virtual void      FreeItem(pointer p);
    virtual WORD 	   WhoIsIt(){return 1025;}
};

typedef TCollection * PCollection;
typedef TObjectCollection * PObjectCollection;
typedef TTextCollection * PTextCollection;
typedef TLOGTextCollection * PLOGTextCollection;
typedef TStringCollection * PStringCollection;
typedef TFontCollection * PFontCollection;
typedef TDoubleDibCollection * PDoubleDibCollection;
typedef TDibCollection * PDibCollection;
typedef TBrushCollection * PBrushCollection;
typedef TPenCollection * PPenCollection;
typedef TToolsCollection * PToolsCollection;
typedef TSortedCollection * PSortedCollection;
typedef THANDLECollection * PHANDLECollection;
typedef TPrimaryCollection * PPrimaryCollection;
typedef T3dCollection * P3dCollection;
void DeleteCollection(PCollection );

void _MkTTosave(PToolsCollection pt);
#endif // _collection_h
