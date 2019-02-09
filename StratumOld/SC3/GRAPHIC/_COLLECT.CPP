/*
Copyright (c) 1995  Virtual World
Module Name:
     collect.cpp
Author:
     Alexander Shelemekhov
*/

#include "collect.h"

//#define MK_FP( seg,ofs )( (void _seg * )( seg ) +( void near * )( ofs ))
//#define FP_SEG( fp )( (unsigned )( void _seg * )( void far * )( fp ))
//#define FP_OFF( fp )( (unsigned )( fp ))

TCollection::TCollection(C_TYPE asize,C_TYPE adelta)
{
    if (asize)
    {
        items = new pointer [asize];
        C_TYPE i;
        for(i = 0; i < asize; i++)
            items[i]=NULL;
    }
    else
        items = NULL;
    delta = adelta;
    limit = asize;
    count = 0;
}

TCollection::~TCollection()
{
    /*FreeAll();*/
    delete []items;
}

pointer TCollection::At(INT16 index)
{
    if (IsIndexValid(index))
        return items[index];
    else
    {
        Error(EM_coindexerror);
        return NULL;
    }
}

void TCollection::AtDelete(C_TYPE index)
{
    if (IsIndexValid(index))
    {
        for (index = index; index + 1 < count; index++)
            items[index] = items[index + 1];

        count--;
    }
    else
        Error(EM_coindexerror);
};

void TCollection::AtInsert(C_TYPE index,pointer item)
{
    if ( (index >= 0) && (index <= count))
    {
        if (count == limit)
        {
            C_TYPE l = limit;
            limit += delta;

            if (limit > max_size)
                limit = max_size;

            if (l == limit)
                Error(EM_cooverflow);

            pointer* _items = new pointer[limit];
            C_TYPE i;
            for(i = 0; i < count; i++)
                _items[i] = items[i];

            delete[] items;
            items = _items;
        }
        C_TYPE i;
        for(i = count; i > index; i--)
            items[i] = items[i-1];
        //       _fmemmove (void far *dest, const void far *src, size_t n)
        /*
 int mc=(count-index)*sizeof(pointer);
if(mc)
memmove (MK_FP(FP_SEG(items),FP_OFF(items)+index*4+4),
MK_FP(FP_SEG(items),FP_OFF(items)+index*4),mc);
*/
        items[index] = item;
        count++;
    }
    else
        Error(EM_coindexerror);
};

void TCollection::Error(INT16 code)
{
    code = _Error(code);
}

void TCollection::AtPut(C_TYPE index,pointer item)
{
    if (IsIndexValid(index))
        items[index] = item;
    else
        Error(EM_coindexerror);
}

void TCollection::FreeAll()
{
    C_TYPE i;
    for (i = 0; i < count; i++)
        FreeItem(items[i]);
    count = 0;
}

C_TYPE TCollection::IndexOf(pointer item)
{
    C_TYPE i;
    for(i = 0; (i < count) && (items[i] != item); i++);

    if (i < count)
        return i;
    else
        return -1;
}

void TCollection::Swap(C_TYPE i,C_TYPE j)
{
    pointer p1, p2; p1 = At(i);
    p2 = At(j);
    items[i] = p2; items[j] = p1;
}

TCollection::TCollection(PStream ps)
{
    count=ps->ReadWord();
    limit=ps->ReadWord();
    delta=ps->ReadWord();

    items = new pointer [limit];
    C_TYPE i;

    if (limit==0)
        limit=count;

    for(i=0;i<limit;i++)
        items[i]=NULL;
}

void TCollection::LoadItems(PStream ps)
{
    C_TYPE i;
    for (i=0;i<count;i++)
        items[i]=GetItem(ps);
}

void TCollection::Store(PStream ps)
{
    ps->WriteWord(GetCountToSave());
    ps->WriteWord(limit);
    ps->WriteWord(delta);
    _store(ps);
    C_TYPE i;
    for (i = 0; i < count; i++)
    {
        pointer item = At(i);
        if ((fullstore) || (IsNeedToSave(item)))
            PutItem(ps, item);
    }
}

int TCollection::IsNeedToSave(pointer){ return 1; }

pointer  TCollection::GetItem(PStream ps)
{
    return ps->Get();
}

C_TYPE TCollection::GetCountToSave()
{
    if (fullstore)
        return count;

    C_TYPE c = 0,i;
    for(i = 0; i < count; i++)
        if (IsNeedToSave(At(i)))
            c++;
    return c;
}

void     TCollection::PutItem(PStream ps,pointer item)
{
    ps->Put(item);
}

void TCollection::FreeItem(pointer item)
{
    delete (_OBJECT*)item;
}

BOOL  TCollection::IsIndexValid(register Index)
{
    return ((Index > -1) && (Index < count));
}

//*---------------------------------------------------*
//TSorted
TSortedCollection::TSortedCollection(PStream ps):
    TCollection(ps)
{
    ps->Read(&Duplicates, 1);
}

void TSortedCollection::_store(PStream ps)
{
    ps->Write(&Duplicates, 1);
}
C_TYPE TSortedCollection::Insert(pointer item)
{
    C_TYPE I;
    if ((! Search(item, I)) ||(Duplicates))
    {
        AtInsert(I, item);
        return I;
    }
    return -1;
}

C_TYPE TSortedCollection::IndexOf(pointer item)
{
    C_TYPE I;
    if (Search(item, I))
        return I;
    else
        return -1;
}

BOOL TSortedCollection::Search(pointer Key,C_TYPE &Index)
{
    C_TYPE  L, H, I, C, ret=0;
    L = 0;
    H = count - (C_TYPE)1;
    while (L <= H)
    {
        I = (L + H) >> 1;
        C = Compare(items[I], Key);
        if (C < 0)
            L = I + 1;
        else
        {
            H = I - 1;
            if (C == 0)
            {
                ret = TRUE;
                if (!Duplicates)
                    L = I;
            }
        }
    };
    Index = L;
    return ret;
};

//*--------------------------------------------------------*
int THANDLECollection::CheckForCurrent(C_TYPE i)
{
    if (current==i){current=OID_NONE;return 1;}
    return 0;
}

BOOL THANDLECollection::_DeleteObject(HOBJ2D handle)
{
    pointer p=GetByHandle(handle);
    if (p)
    {
        if (MayDelete(p))
        {
            CheckForCurrent(handle);
            Free(p);
            return TRUE;
        }
    }
    return 0;
}

HOBJ2D THANDLECollection::InsertObject(pointer obj,HOBJ2D start)
{
    INT16 i,j,k,h;
    h = 0; j = start;
    if (count && (GetHandle(At(count-1))==count ) )
    {
        h = count + 1;
        i = count;
    }
    else
    {
        for(i=0;(i<count)&&(h==0);i++)
        {
            k=GetHandle(At(i));
            j++;
            if (j!=k)
            {h=j;	break;}
        }
        if(h==0)	h=count+1;
    }
    SetHandle(obj,h);
    AtInsert(i,obj);
    return h;
};

C_TYPE THANDLECollection::DeleteUnused(PStream ps)
{
    HOBJ2D i,j;
    i = j = 0;
    BYTE b = 'T';
    pointer p;
    while(i<count)
    {
        p = At(i);
        if (MayDelete(p))
        {
            if (ps)
            {
                ps->Write(&b,1);
                PutItem(ps,p);
            }
            FreeItem(p);
            AtDelete(i);
            j++;
        }
        else
            i++;
    }
    if (ps)
    {
        b=0;
        ps->Write(&b, 1);
    }
    if (current > 0 && GetByHandle(current) == NULL)
        current=-1;
    return j;
}

HOBJ2D THANDLECollection::GetNextHandle(HOBJ2D handle)
{
    HOBJ2D h=0;
    if (handle)
    {
        h = IndexOf((void*)MAKELONG(handle,0));
        if (h < 0)
            return 0;
        h++;
    }
    if (IsIndexValid(h))
        return GetHandle(At(h));

    return 0;
}

int THANDLECollection::Compare(pointer key1,pointer key2)
{
    HOBJ2D h1, h2;
    if( (unsigned )HIWORD((DWORD)( key1 )))
        h1=GetHandle(key1);
    else
        h1=( (HOBJ2D)( key1 ));

    if( (HOBJ2D)HIWORD((DWORD)( key2 )))
        h2=GetHandle(key2);
    else
        h2=( (HOBJ2D)( key2 ));

    if (h1<h2)
        return -1;
    if (h1>h2)
        return 1;

    return 0;
}

void THANDLECollection::_store(PStream ps)
{
    TSortedCollection::_store(ps);
    //Tlection:Store(ps);
    ps->WriteWord(current);
}

HOBJ2D THANDLECollection::GetHandle(pointer)
{
    return (HOBJ2D)_abstract();
}

void THANDLECollection::SetHandle(pointer,HOBJ2D)
{
    _abstract();
}

pointer  THANDLECollection::GetByHandle(HOBJ2D h)
{
    //  C_TYPE i=IndexOf((void*)(DWORD)h);
    C_TYPE i = IndexOf((void*)MAKELONG(h,0));
    if (i>-1)
        return At(i);
    else
        return NULL;
}

void DeleteCollection(PCollection p)
{
    if(!p)
        return;
    p->FreeAll();
    p->DeleteAll();
    delete p;
}

int TCollection::fullstore=1;

int   TDataItems::Compare(pointer key1,pointer key2)
{
    if (*((WORD*)key1)<*((WORD*)key2))
        return -1;

    if (*((WORD*)key1)>*((WORD*)key2))
        return  1;

    return 0;
}

void  TDataItems::FreeItem(pointer p)
{
    delete p;
}

void TCollection::Pack()
{
    C_TYPE i=0;
    while(i<count)
    {
        if (At(i))
            i++;
        else
            AtDelete(i);
    }
}
