/*
Copyright (c) 1995  Virtual World
Module Name:
            _space.cpp
Author:
            Alexander Shelemekhov
*/
#include "_space.h"
#include <mem.h>

#include "../../BC/GWIN2D3D/GWINTYPE.H"

_SPACE::_SPACE()
{
    dataitems=NULL;
    primary=new TPrimaryCollection(10,10);
    all    = new TObjectCollection(10,10);
    pushpop=NULL;
};

_SPACE::_SPACE(PStream ps)
{
    all=NULL;
    primary=NULL;
    pushpop=NULL;
};

_SPACE::~_SPACE()
{
    if (all){DeleteCollection(all);all=0;}
    if (primary){primary->DeleteAll(); delete primary;primary=0;}
    if (dataitems){delete dataitems;dataitems=NULL;}
    if(pushpop)DeleteCollection(pushpop);
};

PObject _SPACE::GetObjByHANDLE(HOBJ2D h)
{
    //TObject**objs=(TObject**)all->items;
    return  (PObject)all->GetByHandle(h);
};

INT16 _SPACE::GetPrimaryOrder(HOBJ2D h)
{
    pointer p = all->GetByHandle(h);
    if (p)
        return primary->IndexOf(p);
    return -1;
};

HOBJ2D _SPACE::InsertObject(PObject obj,HOBJ2D start)
{
    //if(all->IndexOf(obj)<0){
    primary->Insert(obj);
    return all->InsertObject(obj,start);
    //   }
    //  return 0;
}

BOOL _SPACE::_DeleteObject(HOBJ2D h) // если неуcпешно то 0
{
    PObject obj=GetObjByHANDLE(h);
    if (obj)
    {
        if (obj->owner)
        {
            ((PGroup) obj->owner)->items->Delete(obj);
        }
        // Удалили из группы если есть
        if(obj->IsGroup())
        {
            while(((PGroup) obj)->items->count)
            {
                _DeleteObject(((PObject )((PGroup) obj)->items->At(0) )->_HANDLE );
            }
            all->_DeleteObject(h);
        }
        else
        {
            primary->Delete(obj);
            all->_DeleteObject(h);
        }
        return TRUE;
    }
    return FALSE;
};

HOBJ2D _SPACE::CreateGroup(HOBJ2D * Objects2d, INT16 NumObjects ) // если 0 то пустая
{
    C_TYPE i;
    if (!ChkFutureGroupMember(Objects2d,NumObjects,0))
        return 0;

    PGroup pg=_CreateGroup();
    C_TYPE h=all->InsertObject(pg);
    for(i = 0; i < NumObjects; i++)
        AddToGroup(h, Objects2d[i]);

    return h;
};

BOOL _SPACE::ChkFutureGroupMember(HOBJ2D * Objects2d, INT16 NumObjects, HOBJ2D owner)
{
    C_TYPE i;
    PObject obj;
    PObject po;
    if (owner)
    {
        po = GetObjByHANDLE(owner);
        if (po == NULL)
            return 0;
    }

    for(i = 0; i < NumObjects; i++)
    {
        if(Objects2d[i] < 1 || Objects2d[i] > 31999)
            return FALSE;

        obj = GetObjByHANDLE(Objects2d[i]);
        if (obj == NULL)
            return 0;

        if ((owner==0) && (obj->owner!=NULL))return FALSE;
        if ((obj->owner)&&(obj->owner->_HANDLE!=owner))return FALSE;
    }
    return TRUE;
}

BOOL _SPACE::_Awaken()
{INT16 i;
    for(i=0;i<primary->count;i++)
    {pointer p=all->GetByHandle((HOBJ2D)primary->At(i));
        primary->AtPut(i,p);
    }
    return TRUE;
}

BOOL _SPACE::SetGroup(HOBJ2D group, HOBJ2D * Objects2d, INT16 NumObjects )//если 0 то обнуляется
{
    C_TYPE i;
    if(ChkFutureGroupMember(Objects2d, NumObjects, group))
    {
        PObject po;
        PGroup pg = (PGroup)GetObjByHANDLE(group);
        for(i = 0; i < pg->items->count; i++)
        {
            po=(PObject)pg->items->At(i);
            po->owner = NULL;
        }
        pg->items->DeleteAll();

        for(i = 0; i < NumObjects; i++)
            pg->Insert(GetObjByHANDLE(Objects2d[i]));

        return TRUE;
    }
    return 0;
}

HOBJ2D _SPACE::GetGroupItem(HOBJ2D group,INT16 Num)
{
    PGroup obj = (PGroup)GetObjByHANDLE(group);
    if((obj)&&(obj->IsGroup())&&(Num>-1)&&(Num<obj->items->count))
    {
        return ((PObject)(obj->items->At(Num)))->_HANDLE;
    }
    return 0;
};

BOOL _SPACE::SetGroupItem(HOBJ2D group,INT16 Num,HOBJ2D Obj)
{
    PGroup obj=(PGroup)GetObjByHANDLE(group);
    PObject po=(PObject)GetObjByHANDLE(Obj);

    if (obj && obj->IsGroup() && Num > -1 && Num < obj->items->count &&
        po && po->owner == NULL && po != obj)
    {
        ((PObject)(obj->items->At(Num)))->owner = NULL;
        obj->items->AtPut(Num,po);
        po->owner = NULL;
        return TRUE;
    }
    return FALSE;
}

INT16 _SPACE::GetGroupCount(HOBJ2D group)
{ PGroup obj=(PGroup)GetObjByHANDLE(group);
    if ((obj)&&(obj->IsGroup()))
    {
        return (obj->items->count);
    }
    return 0;
};
int _SPACE::GetGroup(HOBJ2D group,HOBJ2D * Objects2d,HOBJ2D MaxObj)
{PGroup obj=(PGroup)GetObjByHANDLE(group);
    if ((obj)&&(obj->IsGroup()))
    {C_TYPE i,j;j=min(MaxObj,obj->items->count);
        for(i=0;i<j;i++)
        {Objects2d[i]=((PObject)obj->items->At(i))->_HANDLE; }
        return obj->_HANDLE;
    }
    return 0;
}

BOOL _SPACE::SetCurrent(HOBJ2D h)
{
    if ((h==OID_NONE)||(all->GetByHandle(h)))
    {
        all->current=h;
        return 1;
    }
    return 0;
}

BOOL _SPACE::AddToGroup(HOBJ2D group,HOBJ2D object)
{
    PGroup grp=(PGroup)GetObjByHANDLE(group);
    PObject obj=(PObject)GetObjByHANDLE(object);

    if (grp && grp->IsGroup() && obj && obj->owner==NULL && grp != obj)
    {
        grp->Insert(obj);
        return 1;
    }
    return 0;
}

C_TYPE _SPACE::DeleteFromGroup(HOBJ2D group,HOBJ2D object)
{
    PGroup grp = (PGroup)GetObjByHANDLE(group);
    PObject obj = (PGroup)GetObjByHANDLE(object);

    if (grp && grp->IsGroup() && obj)
    {
        C_TYPE k = grp->IndexOf(obj);
        if (k > -1)
        {
            grp->items->Delete(obj);
            obj->owner=NULL;
            return k+1;
        }
    }
    return 0;
}

INT16 _SPACE::IsGroupContainObject2d(HOBJ2D g,HOBJ2D o)
{PGroup pg=(PGroup)all->GetByHandle(g);
    PObject po=(PObject)all->GetByHandle(o);
    if (pg==po)return 0;
    if (pg && po && pg->IsGroup()){
        for (C_TYPE i=0;i<pg->items->count;i++)
        {PObject p=(PObject)pg->items->At(i);
            if (p==po)return i+1;
            if(p->IsGroup()&&IsGroupContainObject2d(p->_HANDLE,o)) return i+1;
        }
    }
    return 0;
}
int _SPACE::IsObjectInObject(PObject po,PObject pg)
{if (po==pg)return 1;
    if (po==NULL ||pg==NULL)return 0;
    PGroup pgo=(PGroup)pg;
    C_TYPE i;
    if (pg->IsGroup())
        for (i=0;i<pgo->items->count;i++)
        {if(IsObjectInObject(po,(PObject)pgo->items->At(i)))
                return 1;
        }pgo=(PGroup)po;
    if (pgo->IsGroup())
        for (i=0;i<pgo->items->count;i++)
        {if(IsObjectInObject(pg,(PObject)pgo->items->At(i)))
                return 1;
        }
    return 0;
}

int _SPACE::DeleteGroup(HOBJ2D h) // Удаляет группу
{
    PGroup obj=(PGroup)GetObjByHANDLE(h);

    if (obj && obj->IsGroup())
    {
        C_TYPE i;
        for(i = 0; i < obj->items->count; i++)
            ((PObject)obj->items->At(i))->owner = NULL;

        obj->items->DeleteAll();

        // Занулил оунера
        if (obj->owner)
            obj->owner->items->Delete(obj);

        all->_DeleteObject(h);
        return h;
    }
    return 0;
}

BOOL TGroup::Awaken(_SPACE* ps)
{C_TYPE i;
    for(i=0;i<items->count;i++)
    {pointer p=ps->all->GetByHandle((HOBJ2D)items->At(i));
        if(p) {items->AtPut(i,p);  ((PObject)p)->owner=this;}
        else return FALSE;
    }
    return TRUE;
};
void _SPACE::_wrobj__(PStream ps,PObject po)
{      ps->Put(po);
       if (po->IsGroup())
       {C_TYPE i;
           PGroup pg=(PGroup)po;
           for (i=0;i<pg->items->count;i++)
           {
               _wrobj__(ps,(PObject)pg->items->At(i));
           }
       }
};

PObject _SPACE::__reobj(PStream ps)
{
    PObject po=(PObject)ps->Get();
    all->Insert(po);
    if (po->IsGroup())
    {
        PGroup pg=(PGroup)po;
        int i;
        for (i=0;i<pg->items->count;i++)
        {__reobj(ps);}
    }
    if (!po->Awaken(this)) _Error(EM_awakenerror);
    return po;
};

void _SPACE::__WriteObject(PStream ps,PObject po)
{      if (po->owner){ps->WriteWord(po->owner->_HANDLE);
        ps->WriteWord(po->owner->items->IndexOf(po));
    }
    else ps->WriteWord(0);
       _wrobj__(ps,po);
          if (po->IsGroup())ps->Put(primary);
          else ps->WriteWord(primary->IndexOf(po));
};
PObject _SPACE::__ReadObject(PStream ps)
{HOBJ2D owner=ps->ReadWord();
    C_TYPE index=-1;
    if (owner)index=ps->ReadWord();
    PObject po=__reobj(ps);
    if (owner)
    {PGroup pg=(PGroup)GetObjByHANDLE(owner);
        pg->items->AtInsert(index,po);
    }
    if(po->IsGroup())
    {PPrimaryCollection pc=(PPrimaryCollection)ps->Get(1021);
        C_TYPE i;
        for (i=0;i<pc->count;i++)
        {pc->AtPut(i,GetObjByHANDLE((HOBJ2D)pc->At(i)));
            if (pc->items[i]==NULL)return 0l;
        }
        primary->DeleteAll();
        DeleteCollection(primary);
        primary=pc;
    }else
    {C_TYPE Z=ps->ReadWord();
        primary->AtInsert(Z,po);
    }
    return po;
}

HOBJ2D _SPACE::GetGrandOwner(PObject po)
{
    while (po->owner)
        po = po->owner;

    return po->_HANDLE;
}

int _SPACE::__WriteGroupItem(PStream ps,HOBJ2D g,INT16 i,INT16 z)
{      ps->WriteWord(g);ps->WriteWord(i);
       ps->WriteWord(z);
          return 0;
};
int _SPACE::__ReadGroupItem(PStream ps)
{HOBJ2D g=ps->ReadWord(), i=ps->ReadWord();
    C_TYPE	 z=ps->ReadWord();
    PGroup pg=(PGroup) GetObjByHANDLE(g);
    PObject po=(PGroup) GetObjByHANDLE(i);
    if (pg && po && pg->IsGroup() && po->owner==0)
    {pg->items->AtInsert(z,po);
        po->owner=pg;
        return g;
    }
    return 0;
};
BOOL  _SPACE::Overflow()
{
    return all->count>=max_size;
};

int __GetData(TDataItems *dataitems, UINT16 id, void* far data, UINT16 start, UINT16 size)
{
    if (dataitems)
    {
        TData d;
        d.id = id;

        C_TYPE index;
        if (dataitems->Search(&d,index))
        {
            TData*_d = (TData*)dataitems->At(index);

            if (!data)
                return _d->size;

            if (start>=_d->size)
                return 0;

            if (size > _d->size - start)
                size = _d->size - start;

            memcpy(data, _d->data + start, size);
            return size;
        }
    }
    return 0;
}

void __SetData(TDataItems *&dataitems,UINT16 id,UINT16 start,UINT16 size,void * data)
{
    if(dataitems){
        TData d;
        d.id=id;
        C_TYPE index;
        TData *_d=NULL;
        if (dataitems->Search(&d,index)){
            _d=(TData *)dataitems->At(index);
            if ((data==NULL) || (size==0)){size=0;data=NULL;}

            if (data==NULL){
                C_TYPE index=dataitems->IndexOf(_d);
                dataitems->AtDelete(index);
                delete _d;
                if (!dataitems->count){
                    delete(dataitems);dataitems=NULL;
                }
            }else{
                if (//start>=0 &&
                    (size>0)){
                    if ((start+size)<_d->size){

                        memcpy(_d->data+start,data,size);

                    }else{

                        TData *_d1=MakeData(id,0,start+size,NULL);
                        memcpy(_d1->data,_d->data,_d->size);
                        delete _d;
                        dataitems->AtPut(index,_d1);

                        memcpy(_d1->data+start,data,size);
                    }
                }}
        }else {
            if (size &&data) dataitems->Insert(MakeData(id,start,size,data));
        }
    }else{
        if (data && start>=0 && (size)>0){
            dataitems=new TDataItems();
            dataitems->Insert(MakeData(id,start,size,data));
        } }
};

UINT16 _SPACE::SetData(HOBJ2D Object2d,UINT16 id,UINT16 start,UINT16 size,void * data)
{
    if (Object2d==0){
        __SetData(dataitems,id,start,size,data);
        return 1;
    }else{
        PObject po=(PObject)all->GetByHandle(Object2d);
        if (po)
        {
            /**------------------**/
            __SetData(po->dataitems,id,start,size,data);
            return 1;
            /**------------------**/
        }
    }
    return 0;
}

UINT16 _SPACE::GetData(HOBJ2D Object2d, UINT16 id, void* far data, UINT16 start, UINT16 size)
{
    if (Object2d == 0)
    {
        return __GetData(dataitems, id, data, start, size);
    }
    else
    {
        PObject po = (PObject)all->GetByHandle(Object2d);
        if (po)
            return __GetData(po->dataitems, id, data, start, size);
        return 0;
    }
}

void PntToPnt(POINT2D&po,POINT pi){
    po.x=pi.x;
    po.y=pi.y;
};
void PntToPnt(POINT&po,POINT2D& pi){
    if (pi.x>32000)po.x=32000;else { if(pi.x<-32000)po.x=-32000;  else po.x=pi.x;}
    if (pi.y>32000)po.y=32000;else { if(pi.y<-32000)po.y=-32000;  else po.y=pi.y;}
}

void ChkPnt(POINT2D &p)
{
    if (p.x>32000){p.x=32000;}else{if (p.x<-32000)p.x=-32000;}
    if (p.y>32000){p.y=32000;}else{if (p.y<-32000)p.y=-32000;}
}

double _round(double x){if (x>32000)return 32000;else { if(x<-32000)return -32000;  else return x;}};
