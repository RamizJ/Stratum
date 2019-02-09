/*
Copyright (c) 1995  Virtual World
Module Name:
     object.cpp
Author:
     Alexander Shelemekhov
*/

#include "object.h"
#include <mem.h>

TObject::TObject()
{
    _HANDLE=0;
    options=0;
    owner=NULL;
    PTR=NULL;
    name=NULL;
    dataitems=NULL;
}

TObject::~TObject(){
    if (name)delete name;name=0;
    if (dataitems)delete dataitems;dataitems=NULL;
};
TGroup::TGroup():TObject()
{
    items=new TPrimaryCollection(4,4);
}
TGroup::TGroup(PStream ps,LONG _pos):TObject(ps,_pos)
{
    items = (PPrimaryCollection)ps->Get();
};

TGroup::~TGroup()
{
    if (items){items->DeleteAll();delete items;}
}

BOOL TGroup::Insert(PObject obj)
{
    if (obj->owner==NULL)
    {
        obj->owner=this;
        items->Insert(obj);
        return TRUE;
    }
    return FALSE;
};
void TGroup::StoreCoordinates(PStream ps)
{C_TYPE i;
    for(i=0;i<items->count;i++)
        ((PObject)items->At(i))->StoreCoordinates(ps);
};
void TGroup::LoadCoordinates(PStream ps)
{
    C_TYPE i;
    for(i=0;i<items->count;i++)
        ((PObject)items->At(i))->LoadCoordinates(ps);
};
void TGroup::MarkToSave(int m)
{C_TYPE i;
    TObject::MarkToSave(m);
    for(i=0;i<items->count;i++)
        ((PObject)items->At(i))->MarkToSave(m);
};
void TGroup::UpdateSize(int copy,void * space)
{C_TYPE i;
    for(i=0;i<items->count;i++)
        ((PObject)items->At(i))->UpdateSize(copy,space);
};
C_TYPE TGroup::IndexOf(PObject obj)
{
    return items->IndexOf(obj);
};
BOOL TGroup::Delete(C_TYPE i)
{
    if ((i>=0)&&(i<items->count))
    { ((PObject)items->At(i))->owner=NULL;
        items->AtDelete(i);
        return TRUE;
    }
    return FALSE;
};
//int TGroup::PutItem(int i,PObject obj);
pointer TGroup::GetItem(C_TYPE i)
{
    if ((i>=0)&&(i<items->count))
    {
        ((PObject)items->At(i))->owner=NULL;
        items->AtDelete(i);
    }
    return NULL;
};
WORD TGroup::WhoIsIt(){return 1;}

void TGroup::SetState(WORD astate,ATTR_MODE op,pointer p){
    TObject::SetState(astate,op,p);
    for(C_TYPE i=0;i<items->count;i++){
        ((PObject)items->At(i))->SetState(astate,op,p);
    }
};
void TGroup::Store(PStream ps)
{  TObject::Store(ps);
    ps->Put(items);
};
//*------------------------------------------------*
TRef2d::TRef2d(_OBJECT*obj){
    next=NULL;
    count=1;
    object=obj;
    type=object->WhoIsIt();
};

INT16 TTools2d::IncRef(_OBJECT*obj)
{
    if(obj){
        TRef2d*r=_RefObject;
        while(r){
            if(r->object==obj){ r->count++;goto m1; }
            r=r->next;
        }
        r=new TRef2d(obj);
        r->next=_RefObject;
        _RefObject=r;
    }
m1:
    return ++_RefCount;
}

INT16 TTools2d::DecRef(_OBJECT*obj){
    if(obj){
        TRef2d*_r=NULL;
        TRef2d*r=_RefObject;
        while(r){
            if(r->object==obj){
                r->count--;
                if(r->count<1){
                    if(_r){ _r->next=r->next;}else{ _RefObject=r->next;   }
                }
            }
            _r=r;
            r=r->next;
        }

    }
m1:
    if (_RefCount>0)return --_RefCount;else return (INT16)_Error(EM_referror);
};
//INT16 IncRef(){return ++_RefCount;}
//INT16 DecRef(){if (_RefCount>0)return --_RefCount;else return (INT16)_Error(EM_referror);};


TTools2d::TTools2d(HOBJ2D handle)
{
    _RefCount=0;
    flag=0;
    _RefObject=NULL;
    _HANDLE=handle;
};

TTools2d::TTools2d(PStream ps,LONG _pos)
{
    _RefCount=0;
    _RefObject=NULL;
    flag=0;

    ps->ReadWord(); // Читается _RefCount и выкидывается
    _HANDLE = ps->ReadWord();
}

TTools2d::~TTools2d()
{
    if (!IsZeroRef())
        _Error(EM_referror);
}

void TTools2d::Store(PStream ps)
{
    _OBJECT::Store(ps);
    ps->WriteWord(GetRefCount()); // Записывается для сведения
    ps->WriteWord(_HANDLE);
}
void TObject::Store(PStream ps)
{
    ps->WriteWord(_HANDLE);
    ps->WriteWord(options);
    //	 ps->WriteStr(name);
}

TObject::TObject(PStream ps,LONG _pos)
{
    name=NULL;
    dataitems=NULL;

    _HANDLE=ps->ReadWord();
    options=ps->ReadWord();
    if ((ps->fileversion >= 0x0102) && (ps->fileversion < 0x0300))
    {
        name=ps->ReadStr();
    }
    owner=NULL;
    PTR=NULL;
};

void TObject::MarkToSave(int m)
{
    if (m)
        options |= 0x8000;
    else
        options &= 0x7fff;
}

void TObject::SetState(WORD astate,ATTR_MODE op, pointer )
{
    switch(op & 15)
    {
        case ATTRPUT:  { options=astate;break;}
        case ATTRSET:  { options|=astate;break;}
        case ATTRRESET:{ options&=(WORD)(~astate);break;}
    }
}

void  TObject::PostStore(PStream ps)
{
    if(name)
    {
        CHUNK chunk;chunk.WriteStr(ps, stOBJNAME, name);
    }
    if(dataitems)ps->Put(dataitems);
};

BOOL TObject::ReadChunk(CHUNK&chunk)
{
    switch(chunk.code)
    {
        case otDATAITEMS:
            return ((dataitems=(TDataItems*)chunk.st->AllocateObject(chunk))!=NULL);

        case stOBJNAME:
            name=chunk.ReadStr();
            break;
    }
};

HOBJ2D TObjectCollection::GetHandle(pointer obj)
{
    return ((PObject)obj)->_HANDLE;
}

void TObjectCollection::SetHandle(pointer obj,HOBJ2D h)
{
    ((PObject)obj)->_HANDLE=h;
}

int   TObjectCollection::IsNeedToSave(pointer p)
{
    return ( (PObject) p)->options & 0x8000;
}

pointer TPrimaryCollection::GetItem(PStream ps)
{
    return (void*)MAKELONG(ps->ReadWord(),0);
}

void TPrimaryCollection::PutItem(PStream ps,pointer p)
{
    ps->WriteWord(( (PObject) p)->_HANDLE);
}

BOOL  TPrimaryCollection::IsNeedToSave(pointer p)
{
    return ( (PObject) p)->options & 0x8000;
}
//*-------------------------------------------*

void TObject::StoreCoordinates(PStream){_abstract();};
void TObject::LoadCoordinates(PStream){_abstract();};

POINT2D TObject::Move2d(POINT2D p){_abstract();return p;};
void TObject::Rotate2d(POINT2D,gr_float){_abstract();};
void   TObject::Transform3d(TMatrix&){};
POINT2D TObject::GetOrigin2d(){_abstract();POINT2D p;p.x=0;return p;};
POINT2D TObject::GetSize2d(){_abstract();POINT2D p;p.x=0;return p;};
POINT2D TObject::SetSize2d(POINT2D p){_abstract();return p;};
RECT TObject::GetRect(int){_abstract();RECT r;r.left=0;return r;};
void TObject::UpdateSize(int ,void *){_abstract();};
//RECT TObject::UpdateSize(int,void *=NULL){};
TData * MakeData(UINT16 id,UINT16 start,UINT16 size,void*far __d){
    if (size<1)return 0;
    UINT16 _size=start+size;
    TData *data=(TData*)new BYTE[4+_size];
    data->id=id;
    data->size=_size;
    memset(data->data,0,_size);
    if (__d) memcpy(data->data+start,__d,size);
    return data;
};
pointer TDataItems::GetItem(PStream ps)
{
    WORD id=ps->ReadWord();
    WORD size=ps->ReadWord();
    TData * data=(TData *)new BYTE[ 2*2+size];
    data->id=id;
    data->size=size;
    ps->Read(data->data,size);
    return data;
};
void  TDataItems::PutItem(PStream ps,pointer p){
    TData * data=(TData *)p;
    ps->WriteWord(data->id);
    ps->WriteWord(data->size);
    ps->Write(data->data,data->size);
};

