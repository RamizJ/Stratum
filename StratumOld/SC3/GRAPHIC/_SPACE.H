/*
Copyright (c) 1995  Virtual World
Module Name:
     _space.h
Author:
     Alexander Shelemekhov
*/

#ifndef _SUB_SPACE_H_
#define _SUB_SPACE_H_
//#include "tools2d.h"
#include "object.h"

class SPACE : public _OBJECT
{
public:
    PPrimaryCollection	primary; //Объекты по z-порядку (возможно только видимые)
    PObjectCollection  all;  //
    TDataItems* dataitems;
    TItems* pushpop;

    SPACE();
    SPACE(PStream ps);
    ~SPACE();

    //virtual void 		Store(PStream);
    virtual WORD WhoIsIt(){return (WORD)_abstract();}
    virtual PObject GetObjByHANDLE(HOBJ2D);

    INT16   GetPrimaryOrder(HOBJ2D);
    HOBJ2D InsertObject(PObject,HOBJ2D start=0);
    BOOL _DeleteObject(HOBJ2D); // если неупешно то 0
    INT16 IsGroupContainObject2d(HOBJ2D g,HOBJ2D o);
    HOBJ2D CreateGroup(HOBJ2D * Objects2d,INT16 NumObjects ); // если NumObjects = 0 то пуста¤
    BOOL SetGroup(HOBJ2D group,HOBJ2D * Objects2d,INT16 NumObjects );//если NumObjects = 0 то обнул¤етс¤
    INT16 GetGroupCount(HOBJ2D group);
    int GetGroup(HOBJ2D group,HOBJ2D * Objects2d,INT16 MaxObj);
    int SetCurrent(HOBJ2D);
    int AddToGroup(HOBJ2D group,HOBJ2D object);
    virtual C_TYPE DeleteFromGroup(HOBJ2D group,HOBJ2D object);
    virtual void _SetModify(){};
    virtual BOOL DeleteGroup(HOBJ2D);
    HOBJ2D GetGroupItem(HOBJ2D group,INT16 Num);
    BOOL SetGroupItem(HOBJ2D group,INT16 Num,HOBJ2D Obj);
    BOOL ChkFutureGroupMember(HOBJ2D * Objects2d,INT16 NumObjects ,HOBJ2D owner);
    virtual PGroup _CreateGroup(){_abstract();return NULL;};
    int _Awaken();
    int IsObjectInObject(PObject,PObject);
    HOBJ2D GetGrandOwner(PObject);

    //*-----------------------------------------------------------*
    void __WriteObject(PStream,PObject);
    PObject __ReadObject(PStream);
    int __WriteGroupItem(PStream,HOBJ2D g,INT16 i,INT16 z);
    int __ReadGroupItem(PStream);
    BOOL  Overflow();
    UINT16 SetData(HOBJ2D Object2d,UINT16 id,UINT16 start,UINT16 size,void * data);
    UINT16 GetData(HOBJ2D Object2d,UINT16 id,void * far data,UINT16 start,UINT16 size);
    virtual INT32  IsIntersect(HOBJ2D ,HOBJ2D ,UINT32)=0;
    virtual  BOOL PushCrd()=0;
    virtual  BOOL PopCrd()=0;
    virtual  BOOL UpdateTool(int,HOBJ2D,pointer)=0;
    BOOL     SetCrd(CRDSYSTEM2D*);
private:

    void _wrobj__(PStream,PObject);
    PObject __reobj(PStream);


};
#endif //_SUB_SPACE_H_
