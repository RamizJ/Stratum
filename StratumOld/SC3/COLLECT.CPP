/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
            collect.cpp
Author:
            Alexander Shelemekhov
*/

#include "collect.h"

//#include <windows.h>
//#define MK_FP( seg,ofs )( (void _seg * )( seg ) +( void near * )( ofs ))
//#define FP_SEG( fp )( (unsigned )( void _seg * )( void far * )( fp ))
//#define FP_OFF( fp )( (unsigned )( fp ))
TCollection::TCollection()
{
    items=NULL;
    count=0;
    limit=0;
    delta=0;
};

void 	TCollection::AtFree(C_TYPE index)
{
    FreeItem(At(index));
    AtDelete(index);
};

void 	TCollection::Delete(pointer item)
{
    AtDelete(IndexOf(item));
};

void 	TCollection::Free(pointer item)
{
    Delete(item);
    FreeItem(item);
};

//вставляем элемент void* в конец коллекции и возвращаем число элементов в ней
C_TYPE TCollection::Insert(pointer item)
{
    AtInsert(count,item);
    return (INT16)(count -1);
};

TCollection::TCollection(C_TYPE asize,C_TYPE adelta)
{
    if (asize)
    {
        items = new pointer [asize];
        C_TYPE i;
        for(i=0;i<asize;i++)
            items[i]=NULL;
    }
    else
        items=NULL;
    delta=adelta;
    limit=asize;
    count=0;
};

TCollection::~TCollection(){/*FreeAll();*/delete []items ;};

pointer TCollection::At(register index)
{
    if (IsIndexValid(index))
        return items[index];
    else
    {_Error(EM_coindexerror,index);return NULL;}
};

void TCollection::AtDelete(C_TYPE index)
{
    if (IsIndexValid(index))
    {
        for (index=index;index+1<count;index++)items[index]=items[index+1];
        count--;
    }else _Error(EM_coindexerror,index);
};

//вставляем void* в позицию index коллекции
void TCollection::AtInsert(C_TYPE index,pointer item)
{
    if ((index>=0)&&(index<=count))
    {
        if (count==limit)
        {
            //определяем новый размер коллекции
            C_TYPE l=limit;
            limit+=delta;
            if (limit>max_size)limit=max_size;
            if (l==limit)_Error(EM_cooverflow);
            //создаем увеличенную коллекцию
            pointer * _items=new pointer[limit];
            C_TYPE i;
            //копируем элементы из старой коллекции в новую
            for(i=0;i<count;i++)_items[i]=items[i];
            //удаляем старую коллекцию
            delete [] items;
            items=_items;
        }
        C_TYPE i;
        //переносим элементы после index на одну позицию вперед
        for(i=count;i>index;i--)items[i]=items[i-1];
        //вставляем новый элементв в позицию index
        items[index]=item;
        count++;
    }
    else
        _Error(EM_coindexerror,index);
};
void TCollection::AtPut(C_TYPE index,pointer item){
    if (IsIndexValid(index)) items[index]=item;
    else _Error(EM_coindexerror,index);
};
void TCollection::FreeAll(){C_TYPE i;
                            for (i=0;i<count;i++)FreeItem(items[i]);
                                                    count=0;
                           };

C_TYPE TCollection::IndexOf(pointer item)
{
    C_TYPE i;
    for(i=0;(i<count)&&(items[i]!=item);i++);
    if (i<count) return i;
    else return -1;
};

void TCollection::Swap(C_TYPE i,C_TYPE j)
{
    if(i==j || i<0 || j<0 || i>=count || j>=count) return;
    pointer p1=At(i),p2=At(j);
    items[i]=p2;items[j]=p1;
};

void 		TCollection::FreeItem(pointer item) { };

BOOL  TCollection::IsIndexValid(register Index)
{
    return ((Index>-1)&&(Index<count));
};

C_TYPE TSortedCollection::Insert(pointer item)
{
    C_TYPE I;
    if ((!Search(item,I))||(Duplicates))
    {
        AtInsert(I, item);
        return I;
    }
    return -1;
};

TSortedCollection::TSortedCollection(C_TYPE asize,C_TYPE adelta):TCollection(asize,adelta)
{
    Duplicates=0;
};

int TSortedCollection::Compare(pointer key1,pointer key2)
{return _abstract();};

C_TYPE  TSortedCollection::IndexOf(pointer item)
{
    C_TYPE I;
    if (Search(item, I))return I;
    else return (C_TYPE)-1;
};

BOOL TSortedCollection::Search(pointer Key,C_TYPE &Index)
{
    C_TYPE  L, H, I;
    int C;
    BOOL ret=FALSE;
    L = 0;
    H = (INT16)(count - 1);
    while (L <= H)
    {
        I = (INT16)((L + H) >> 1);
        C = Compare(items[I], Key);
        if (C < 0)
        {
            L = (INT16)(I + 1);
        }
        else
        {
            H =(INT16)( I - 1);
            if (C == 0)
            {
                ret = TRUE;
                if (!Duplicates) 	L = I;
            }
        }
    };
    Index = L;
    return ret;
};

//*--------------------------------------------------------*

int TStringCollection::Compare(pointer key1,pointer key2){
    return lstrcmp((LPSTR)key1,(LPSTR)key2);
};
TStringCollection::TStringCollection(WORD asize,WORD adelta):TSortedCollection(asize,adelta){
};

void _SDKPROC DeleteCollection(TCollection*p)
{
    if (!p) return;
    p->FreeAll();
    p->DeleteAll();
    delete p;
};


//---------------------------------------------------------
CTag::CTag(char* _fullvalue)
{
    //формат _fullvalue - <tagname>value</tagname>
    name=0;
    fullvalue=0;
    value=0;

    if(_fullvalue==0) return;
    if(strlen(_fullvalue)<7) return;
    if(_fullvalue[0]!='<'	||
            _fullvalue[strlen(_fullvalue)-1]!='>')return;

    //сохраняем исходную строку
    fullvalue=new char[strlen(_fullvalue)+1];
    strcpy(fullvalue,_fullvalue);

    //определяем имя тега
    int namelen=1;
    while(_fullvalue[namelen]!='>')  namelen++;
    if(namelen>=strlen(_fullvalue) || namelen<=1) return;
    namelen--;
    name=new char[namelen+1];
    strncpy(name,_fullvalue+1,namelen);
    name[namelen]='\0';

    //определяем значение тега
    int valuelen=namelen+2;
    while(_fullvalue[valuelen]!='<') valuelen++;
    if(valuelen>=strlen(_fullvalue)){ name=0; return;}
    valuelen-=namelen+2;
    value=new char[valuelen+1];
    strncpy(value,_fullvalue+namelen+2,valuelen);
    value[valuelen]='\0';

    //проверяем закрывающий тег
    int closetagi=namelen+2+valuelen;
    if(_fullvalue[closetagi+1]!='/') {name=0; return;}
    int closetaglen=strlen(_fullvalue)-closetagi-3;
    char* closetagname=new char[closetaglen+1];
    strncpy(closetagname,_fullvalue+closetagi+2,closetaglen);
    closetagname[closetaglen]='\0';
    int r=lstrcmpi(name,closetagname);
    if(r<0||r>0){name=0; return;}
};

bool CTag::IsValid()
{
    if(name)
        if(strlen(name)>0)
            return true;
    return false;
};

void CTag::Free()
{
    if(name) delete[] name;
    if(value) delete[] value;
    if(fullvalue) delete[] fullvalue;
};


CTagsCollection::CTagsCollection(char* tags,C_TYPE asize,C_TYPE adelta):TCollection(asize,adelta)
{
    Add(tags);
};

void CTagsCollection::Add(char* tags)
{
    CTag* tag;
    char temp[99999];
    //count=0;
    for(int i=0,beg=0;i<strlen(tags);i++)
    {
        if(tags[i]=='\n')
        {
            strncpy(temp,tags+beg,i-beg);
            temp[i-beg]='\0';
            tag=new CTag(temp);
            if(tag->IsValid())Insert(tag);
            //MessageBox(0,tag->value,tag->name,0);
            beg=i+1;
        }
    }
    if(beg<i)
    {
        strncpy(temp,tags+beg,i-beg);
        temp[i-beg]='\0';
        tag=new CTag(temp);
        if(tag->IsValid())Insert(tag);
    }
};

PTag CTagsCollection::SearchTag(char* tagname)
{
    PTag tag=0;
    for (C_TYPE i=0;i<count;i++)
    {
        tag=(PTag)items[i];
        if (lstrcmpi(tagname,tag->name)==0)
            return tag;
    }
    return 0;
};

char* CTagsCollection::GetTagValue(char* tagname)
{
    PTag tag=SearchTag(tagname);
    if(tag)
        return tag->value;
    return "";
};

char* CTagsCollection::GetTagFullValue(char* tagname)
{
    PTag tag=SearchTag(tagname);
    if(tag)
        return tag->fullvalue;
    return "";
};

void CTagsCollection::FreeItem(pointer item)
{
    ((PTag)item)->Free();
    delete item;
};
