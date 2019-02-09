/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
      dynodata.cpp
Author:
      Alexander Shelemekhov

Description:
    Работа с динамическими переменными
*/

#include "dynodata.h"
#include <mem.h>
#include <stdio.h>
#include "registry.h"
#include "dllman.h"
#if !defined(WIN32)
#include <dos.h>
#endif

#include "../BC/GWIN2D3D/GWINTYPE.H"


// TDynVar     S T A R T

TDynVar::TDynVar(): TCollection(10, 10){ }

TDynVar::TDynVar(PStream ps): TCollection(10, 10)
{
    C_TYPE count_ = ps->ReadWord();
    for (C_TYPE i = 0; i < count_; i++)
    {
        char s[MAX_STRING];
        ps->ReadStr(s, sizeof(s));

        PClass _class = GetClassByName(s);
        TDynItem * item = CreateItem(_class);
        if(item)
        {
            Insert(item);
        }
        else
            return;

        int _count=ps->ReadWord();
        if(_count)
        {
            int pos = 0;
            for (int _id=0; _id<_count; _id++)
            {
                ps->ReadStr(s, sizeof(s));
                INT16 id = _class->GetVarIdByName(s, FALSE);
                ps->ReadStr(s, sizeof(s));
                if(id>-1)
                {
                    if(!lstrcmpi(_class->vars[id].classType->GetClassName(), s))
                    {
                        int __size=0;
                        __size = _class->vars[id].classType->GetClass()->varsize;
                        if(_class->vars[id].classType->isSTRING())
                            ps->ReadStr((char *)&(item->_data[pos]), __size);
                        else
                            ps->Read(&(item->_data[pos]), __size);
                        pos+=__size;
                    }
                }
                else
                    return;
            }
        }
        else
            ps->Read((void *)item->_data, (int)item->_class->varsize);
    }
} ;

TDynVar::~TDynVar()
{
    FreeAll();
    DeleteAll();
} ;

void TDynVar::Store(TStream*ps)
{
    ps->WriteWord(count);
    for (C_TYPE i = 0; i < count; i++)
    {
        TDynItem * item = (TDynItem *)At(i);

        ps->WriteStr(item->_class->name);
        ps->WriteWord(item->_class->varcount);

        int pos = 0;
        if(item->_class->varcount)
        {
            for (int i = 0; i < item->_class->varcount; i++)
            {
                ps->WriteStr(item->_class->vars[i].name);
                ps->WriteStr(item->_class->vars[i].classType->GetClassName());

                int _size = item->_class->vars[i].classType->GetClass()->varsize;
                if(item->_class->vars[i].classType->isSTRING())
                    ps->WriteStr((char *)&(item->_data[pos]));
                else
                    ps->Write(&(item->_data[pos]), _size);
                pos += _size;
            }
        }
        else
            ps->Write((void *)item->_data, (int)item->_class->varsize);
    }
} ;

void TDynVar::FreeItem(pointer p){delete (TDynItem *)p;};

TDynItem* TDynVar::CreateItem(PClass _class)
{
    if(_class)
    {
        if(_class->varsize)
        {
            int size = sizeof(TDynItem) - 1 + _class->varsize;
            TDynItem* item = (TDynItem *)new char[size];

            memset(item, 0, size);
            item->_class = _class;
            return item;
        }
    }
    return NULL;
}

// TDynVar    E N D

//Создание массива
dyno_t TMemVarManager::NewVar()
{
    TDynVar * v= new TDynVar();
    v->project = GetCurrentProject();

    dyno_t h = InsertVar(v);
    if(!h)
    {
        delete v;
        Error(DE_UNABLECREATE);
    }
    return h;
} ;

//Удаление массива
BOOL TMemVarManager::DeleteVar(dyno_t h)
{
    C_TYPE i=index_by_H(h);
    if(i>-1)
    {
        pointer p=At(i);
        if(p)
        {
            FreeItem(p);
            AtPut(i, NULL);
            return TRUE;
        }
    }
    if(h)
        Error(DE_INVALIDHANDLE);
    return FALSE;
} ;

C_TYPE TMemVarManager::InsertInVar(dyno_t h, char* name)
{
    TDynVar* v = Get(h);
    if(v)
    {
        PClass _class = GetClassByName(name);
        if(_class)
        {
            TDynItem * item = v->CreateItem(_class);
            if(item)
                v->Insert(item);
            else
            {
                if(!_class->varsize)
                    Error(DE_NULLVARSIZE);
            }
            return v->count;
        }
    }
    return 0;
}

BOOL TMemVarManager::DeleteInVar(dyno_t h, C_TYPE index)
{
    TDynVar * v=Get(h);
    if(v)
    {
        if(index >=0 && v->count > index)
        {
            v->AtFree(index);
            return TRUE;
        }
    }
    return FALSE;
} ;

BOOL TMemVarManager::Store(dyno_t h, TStream*st)
{
    TDynVar * v=Get(h);
    if(v)
    {
        v->Store(st);
        return (st->status==0);
    }
    return FALSE;
} ;

dyno_t TMemVarManager::Load(TStream*st)
{
    TDynVar * v=new TDynVar(st);
    v->project=GetCurrentProject();
    dyno_t h=InsertVar(v);
    if(!h)
    {
        delete v;
        Error(DE_UNABLECREATE);
    }
    return h;
} ;

char* TMemVarManager::GetType(dyno_t h, C_TYPE index)
{
    TDynItem * item=GetItem(h, index);
    if(item)
    { return item->_class->name;}
    return NULL;
}

dyno_t TMemVarManager::DirectoryInfo(char*filter, int attr)
{
    dyno_t mem=0;
#ifdef WIN32
    WIN32_FIND_DATA fdata;
    memset(&fdata, 0, sizeof(fdata));
    BOOL ok;
    //if(attr==0) attr=FILE_ATTRIBUTE_NORMAL;
    //fdata.dwFileAttributes=attr;
    HANDLE fhandle=FindFirstFile(filter, &fdata); // address of returned information
    ok=(fhandle!=INVALID_HANDLE_VALUE);
    if(ok)
    {
        mem=NewVar();
        while(ok)
        {
            if((fdata.dwFileAttributes&attr)==attr || attr==0)
            {
                C_TYPE index=InsertInVar(mem, "FILE_ATTRIBUTE")-1;
                SetVar(mem, index, "Name", 3, fdata.cFileName);
                double size=(((double)fdata.nFileSizeLow)+((double)fdata.nFileSizeHigh)*4294967296.0);
                SetVar(mem, index, "Size", 0, &size);
                size=fdata.dwFileAttributes;
                SetVar(mem, index, "Attr", 0, &size);
                SYSTEMTIME SystemTime;
                if(FileTimeToSystemTime(&(fdata.ftCreationTime), &SystemTime))
                {
                    size=(SystemTime.wYear);
                    SetVar(mem, index, "Year", 0, &size);
                    size=SystemTime.wMonth;
                    SetVar(mem, index, "Month", 0, &size);
                    size=SystemTime.wDayOfWeek;
                    SetVar(mem, index, "DayOfWeek", 0, &size);
                    size=SystemTime.wDay;
                    SetVar(mem, index, "Day", 0, &size);
                    size=SystemTime.wHour;
                    SetVar(mem, index, "Hour", 0, &size);
                    size=SystemTime.wMinute;
                    SetVar(mem, index, "Minute", 0, &size);
                    size=SystemTime.wSecond;
                    SetVar(mem, index, "Second", 0, &size);
                }
            }
            //fdata.dwFileAttributes=attr;
            ok=FindNextFile(fhandle, &fdata);
        }
        FindClose(fhandle);
    }
#else

    find_t ffblk;
    memset(&ffblk, 0, sizeof(ffblk));
    int done;
    done=_dos_findfirst(filter, _A_NORMAL, &ffblk);
    if(!done)
    {
        mem=NewVar();
        while(!done)
        {
            C_TYPE index=InsertInVar(mem, "FILE_ATTRIBUTE")-1;
            SetVar(mem, index, "Name", 3, ffblk.name);
            double size;
            size=ffblk.attrib;
            SetVar(mem, index, "Attr", 0, &size);
            size=ffblk.size;
            SetVar(mem, index, "Size", 0, &size);

            size=1980.0+((ffblk.wr_date&0xfe00u)>>9);
            SetVar(mem, index, "Year", 0, &size);

            size=(ffblk.wr_date&0xe0)>>5;
            SetVar(mem, index, "Month", 0, &size);

            //      SetVar(mem,index, "DayOfWeek",0,&size);
            size=ffblk.wr_date&0x1c;
            SetVar(mem, index, "Day", 0, &size);

            size=(ffblk.wr_date&0x001f);
            SetVar(mem, index, "Second", 0, &size);
            size=2*(ffblk.wr_date&0x07e0)>>5;
            SetVar(mem, index, "Minute", 0, &size);
            size=(ffblk.wr_date&0xf800)>>11;
            SetVar(mem, index, "Hour", 0, &size);

            done=_dos_findnext(&ffblk);
        } ;
    }
#endif

    return mem;
}

int TMemVarManager::GetVarType(dyno_t h, C_TYPE index, char *name)
{
    TDynItem * item = GetItem(h, index);
    if(item)
    {
        PClass _type = NULL;
        if(name && *name && item->_class && item->_class->varcount)
        {
            for (int i = 0; i < item->_class->varcount; i++)
            {
                if(!(lstrcmpi(name, item->_class->vars[i].name)))
                {
                    _type = item->_class->vars[i].classType->GetClass();
                    break;
                }
            }
        }
        else
            _type=item->_class;
        if(_type)
        {
            PClassListInfo _t=_type->listinfo;
            if(_t->isFLOAT()) return 0;
            if(_t->isHANDLE()) return 1;
            if(_t->isRGB()) return 2;
            if(_t->isSTRING()) return 3;
        }
    }
    return -1;
} ;

BOOL TMemVarManager::SetVar(dyno_t h, C_TYPE index, char* name, BYTE type, void*data)
{
    TDynItem * item=GetItem(h, index);
    if(item)
    {
        PClass _type = NULL;
        void * _data = NULL;
        if(name && *name && item->_class->varcount)
        {
            int delta = 0;
            for (int i=0; i < item->_class->varcount; i++)
            {
                if(!(lstrcmpi(name, item->_class->vars[i].name)))
                {
                    _type = item->_class->vars[i].classType->GetClass();
                    _data = &(item->_data[delta]); // was i
                }
                delta += item->_class->vars[i].classType->GetClass()->varsize;
            }
        }
        else
        {
            _type = item->_class;
            _data = &(item->_data);
        }
        BOOL get = (type&0x80)!=0;
        type = type&0x7f;
        if(get && data)
        {
            void * _d=data;
            data=_data;
            _data=_d;
        }
        if(!_type)  return 0;
        PClassListInfo _t=_type->listinfo;
        if(_type)
        {
            if(data)
                switch(type)
                {
                    case 0:
                        if(_t->isFLOAT())
                        { *((double *)_data)=*((double *)data);     return TRUE;  }
                        break;
                    case 1:
                        if(_t->isHANDLE())
                        { *((long *)_data)=*((long *)data);         return TRUE;  }
                        break;
                    case 2:
                        if(_t->isRGB())
                        { *((COLORREF *)_data)=*((COLORREF *)data);	return TRUE;  }
                        break;
                    case 3:
                        if(_t->isSTRING())
                        { lstrcpyn(((char *)_data), ((char *)data), MAX_STRING);  return TRUE;   }
                        break;
                    default:
                        return FALSE;
                }
            return TRUE;
        }
        Error(DE_TYPEMISMATCH);
    }
    return FALSE;
} ;

//http://us.imdb.com/cache/urls/person+b59146
C_TYPE TMemVarManager::GetCount(dyno_t h)
{
    TDynVar * v=Get(h);
    if(v)
        return v->count;
    return 0;
} ;

dyno_t TMemVarManager::InsertVar(TDynVar* v)
{
    for (C_TYPE i = 0; i < count; i++)
    {
        TDynVar * _v=(TDynVar *)_At(i);
        if(!_v)
        {
            AtPut(i, v);
            return H_by_index(i);
        }
    }
    C_TYPE index=Insert(v);
    if(index>-1)
        return H_by_index(index);
    return NULL;
} ;

TDynVar *TMemVarManager::Get(dyno_t h)
{
    C_TYPE i=index_by_H(h);
    if(i>-1)
    {
        TDynVar * d=(TDynVar *)At(i);
        if(d)
            return d;
    }
    if(h)
        Error(DE_INVALIDHANDLE);
    return NULL;
} ;

TDynItem *TMemVarManager::GetItem(dyno_t h, C_TYPE index)
{
    TDynVar * v=Get(h);
    if(v)
    {
        if((index>=0)&&(index<v->count))
        { return (TDynItem *)v->At(index);}
        else
            Error(DE_INVALIDINDEX);
    }
    return 0;
} ;

void TMemVarManager::FreeItem(pointer p){delete (TDynVar *)p;};

void TMemVarManager::Error(int code){GroupFuncError(code, D_SPRINTF);};

TMemVarManager::TMemVarManager(): TCollection(10, 50){ } ;

TMemVarManager::~TMemVarManager(){Done(NULL);};

void TMemVarManager::Done(void* project)
{
    C_TYPE i=0;
    while(i<count)
    {
        TDynVar * v=(TDynVar *)At(i);
        if(v && ((v->project==project)||(project==NULL)))
        {
            FreeItem(v);
            AtPut(i, NULL);
        } ;
        i++;
    }
    //  FreeAll();
    //  DeleteAll();
} ;
TMemVarManager * dyno_mem=NULL;

//сортируем массив h методом Хоара (быстрая сортировка)
void TMemVarManager::QuickSort(dyno_t h,char** Fields, INT16 &Fcount, int first, int last, bool *asc, INT16 FieldsDirection)
{
    if(first>=last || first<0 || last<0)return;
    //если список переменных пуст, но массив имеет простой тип
    if(Fcount==0 && GetVarType(h, first, "")>=0)
    {Fields=new char*[1];Fields[0]=new char[5];strcpy(Fields[0],"");Fcount++;}
    if(Fcount==0)return;

    int pivotpoint=first,i=0,t=0,dyntype1=-1,dyntype2=-1,wasswap=0;
    if(asc==NULL)
    {
        asc=new bool[Fcount];
        for(i=0;i<Fcount;i++) asc[i]=true;
    }
    //определяем начало и конец для перебора списка переменных, по которым будем сортировать массив
    int begin=FieldsDirection>=0?0:Fcount-1,
            end=FieldsDirection>=0?Fcount:-1;
    bool need_swap=false;
    double varf1,varf2; long varh1,varh2; char vars1[MAX_STRING], vars2[MAX_STRING];
    TDynVar* dv=Get(h);

    //char s[200];
    //sprintf(s,"f=%d, l=%d , b=%d, e=%d",first,last,begin,end);
    //MessageBox(0,s,"awef",0);

    //перебираем массив для определения точки разделения
    for(i=first+1;i<=last;i++)
    {
        //сравниваем i и pivotpoint по всем заданным полям
        need_swap=false;
        for(t=begin;t!=end;t+=FieldsDirection)
        {
            //определяем тип переменной с именем Fields[t] у элементов i и first
            dyntype1=GetVarType(h, first, Fields[t]);
            dyntype2=GetVarType(h, i, Fields[t]);
            //если переменные найдены и их тип совпадает
            if(dyntype1>=0 && dyntype2>=0 && dyntype1==dyntype2)
            {
                //выполняем сравнение значений у элементов i и first
                if(dyntype1==0)
                {
                    SetVar(h, first, Fields[t],0x80|dyntype1,&varf1);
                    SetVar(h, i, Fields[t],0x80|dyntype1,&varf2);
                    //sprintf(s,"%e, %e",varf1,varf2);
                    if(asc[t]){ if(varf1>varf2)need_swap=true;if(varf1<varf2)break;}
                    else	 { if(varf1<varf2)need_swap=true;if(varf1>varf2)break;}
                    if(varf1==varf2) {wasswap++;continue;}
                }
                if(dyntype1==1)
                {
                    SetVar(h, first, Fields[t],0x80|dyntype1,&varh1);
                    SetVar(h, i, Fields[t],0x80|dyntype1,&varh2);
                    if(asc[t]){ if(varh1>varh2)need_swap=true;if(varh1<varh2)break;}
                    else	 { if(varh1<varh2)need_swap=true;if(varh1>varh2)break;}
                    if(varh1==varh2) {wasswap++;continue;}
                }
                if(dyntype1==3)
                {
                    SetVar(h, first, Fields[t],0x80|dyntype1,&vars1);
                    SetVar(h, i, Fields[t],0x80|dyntype1,&vars2);
                    if(asc[t]){ if(lstrcmp(vars1,vars2)>0)need_swap=true;if(lstrcmp(vars1,vars2)<0)break;}
                    else	 { if(lstrcmp(vars1,vars2)<0)need_swap=true;if(lstrcmp(vars1,vars2)>0)break;}
                    if(lstrcmp(vars1,vars2)==0) {wasswap++;continue;}
                }
                if(need_swap)	break;
            }
        }
        //если нужно поменять i и pivotpoint местами
        if(need_swap)
        {
            pivotpoint++;
            if(pivotpoint!=i)
            {
                dv->Swap(pivotpoint,i);
                wasswap++;
            }
        }
    }
    dv->Swap(pivotpoint,first);
    //if(wasswap==0){return;}
    QuickSort(h,Fields,Fcount,first,pivotpoint-1,asc,FieldsDirection);
    QuickSort(h,Fields,Fcount,pivotpoint+1,last,asc,FieldsDirection);
}

//выполняем сортировку массива
BOOL TMemVarManager::Sort(dyno_t h, char** Fields, INT16 Fcount, bool *asc, INT16 FieldsDirection)
{
    if(index_by_H(h)>-1 && GetCount(h))
    {
        QuickSort(h,Fields,Fcount,0,GetCount(h)-1,asc,FieldsDirection);
        return TRUE;
    }
    return FALSE;
}

// Sc Vars

TScVarManager::~TScVarManager()
{
    FreeAll();
    DeleteAll();
} ;

int TScVarManager::Compare(pointer key1, pointer key2)
{
    SCM * s1=(SCM *)key1;
    SCM * s2=(SCM *)key2;
    return lstrcmp(s1->name, s2->name);
} ;

BOOL TScVarManager::SetVarS(char*name, char*string)
{
    C_TYPE index;
    SCM * s=GetVar(name, index);
    int len=lstrlen(string);
    if(len)
    {
        SCM * s1 = __CreateVar(name, 2, len+1);
        lstrcpy(((char *)s1)+s1->size + 2, string);
        if(s)
        {
            delete s;
            AtPut(index, s1);
        }
        else
            AtInsert(index, s1);
    }
    else
    {
        if(s)
        {
            AtDelete(index);
            delete s;
        }
    }
    return TRUE;
} ;

BOOL TScVarManager::SetVarI(char*name, INT32 d)
{
    C_TYPE index;
    SCM * s=GetVar(name, index);
    int len=4;
    if(len)
    {
        if(!s||s->type!=0)
        {
            if(s)
                AtFree(index);
            s = __CreateVar(name, 0, len);
            AtInsert(index, s);
        }
        *((INT32 *)(((char *)s)+s->size+2))=d;
    }
    return TRUE;
} ;

BOOL TScVarManager::SetVarD(char*name, double d)
{
    C_TYPE index;
    SCM * s=GetVar(name, index);
    int len=8;
    if(len)
    {
        if(!s||s->type!=1)
        {
            if(s)
                AtFree(index);
            s=__CreateVar(name, 1, len);
            AtInsert(index, s);
        }
        *((double *)(((char *)s)+s->size+2))=d;
    }
    return TRUE;
} ;

BOOL TScVarManager::GetVarS(char*name, char*string)
{
    C_TYPE index;
    SCM * s=GetVar(name, index);
    if(s&&s->type==2)
    {
        lstrcpy(string, ((char *)s)+s->size+2);
        return TRUE;
    }
    else
        * string=0;
    return FALSE;
} ;

BOOL TScVarManager::GetVarI(char*name, INT32&d)
{
    C_TYPE index;
    SCM * s=GetVar(name, index);
    if(s&&s->type==0)
    {
        d=*((INT32 *)(((char *)s)+s->size+2));
        return TRUE;
    }
    else
        d=0;
    return FALSE;
} ;

BOOL TScVarManager::GetVarD(char*name, double&d)
{
    C_TYPE index;
    SCM * s=GetVar(name, index);
    if(s&&s->type==1)
    {
        d=*((double *)(((char *)s)+s->size+2));
        return TRUE;
    }
    else
        d=0.0;
    return FALSE;
} ;

int TScVarManager::GetVarDef(char*name, int def)
{
    INT32 d;
    if(!GetVarI(name, d))
        return def;
    return d;
} ;

BOOL TScVarManager::DeleteVar(char*name)
{
    C_TYPE index;
    SCM * s=GetVar(name, index);
    if(s)
    {
        AtFree(index);
        return TRUE;
    }
    return FALSE;
} ;

SCM *TScVarManager::__CreateVar(char* name, BYTE type, int size)
{
    char * _ps=name;
    while(_ps&&(*_ps))
    {
        if((((*_ps)>='A')&&((*_ps)<='Z'))||(((*_ps)>='a')&&((*_ps)<='z'))||((*_ps)=='_')
                ||((_ps>name)&&((*_ps)>='0')&&((*_ps)<='9')))
            _ps++;
        else
        {
            MessageBox(0, "Dyno memory 'name' error", "DYNO DYNO DYNO memory", MB_OK);
            _ps=NULL;
            break;
        }
    }

    int len = lstrlen(name);
    len++;

    int s = 2 + len + size;
    BYTE * b=new BYTE[s];

    lstrcpy(b + 2, name);
    b[0]=type;
    b[1]=len;
    return (SCM *)b;
}

SCM *TScVarManager::GetVar(char*name, C_TYPE&index)
{
    if(Search(name-2, index))
    { return (SCM *)At(index);}
    else{
        //TMemVarManager::Error(DE_NONAME);
    }
    return NULL;
}

void TScVarManager::Store(TStream&st)
{
    st.WriteWord(count);
    for (C_TYPE i=0; i<count; i++)
    {
        SCM* scm = (SCM*)At(i);
        int size = 2 + scm->size;

        switch(scm->type)
        {
            case 0:
                size += 4;
                break;

            case 1:
                size += 8;
                break;

            case 2:
                size += (lstrlen(((char *)scm)+size)+1);
                break;
        }

        st.WriteWord(size);
        st.Write(scm, size);
    }
}

void TScVarManager::ToText(TStream&st, UINT16 f)
{
    if(f&1)
    {
        st.WriteDOSSTR("########################################");
        st.WriteDOSSTR("          Variables:");
        st.WriteDOSSTR("########################################");
    }
    for (C_TYPE i=0; i<count; i++)
    {
        SCM * s=(SCM *)At(i);
        char txt2[MAX_STRING];
        char txt[MAX_STRING];
        switch(s->type){
            case 0:
            {
                UINT32 d=*((INT32 *)(((char *)s)+s->size+2));
                sprintf(txt, "%ld", d);
            }            break;
            case 1:
            {
                double d=*((double *)(((char *)s)+s->size+2));
                sprintf(txt, "%g", d);
            }
                break;
            case 2:
            {
                lstrcpy(txt, ((char *)s)+s->size+2);
                break;
            }
        }
        sprintf(txt2, "%s :%s", s->name, txt);
        st.WriteDOSSTR(txt2);
    }
} ;

BOOL TScVarManager::Load(TStream&st)
{
    C_TYPE _count=st.ReadWord();
    for (C_TYPE i=0; i<_count; i++)
    {
        INT16 size=st.ReadWord();
        BYTE * b=new BYTE[size];
        st.Read(b, size);
        if(Insert(b)<0)
            delete b;
    }
    return st.status==0;
} ;

TScVarManager * sc_vars=NULL;

// type
// 0 - int32
// 1 - double
// 2 - string
TScVarManager *GetManager(char*name)
{
    TProject * p=(TProject *)GetCurrentProject();
    if(p && p->vars)
    {
        C_TYPE index;
        void * s = p->vars->GetVar(name, index);
        if(s)
            return p->vars;
    }
    return sc_vars;
}

BYTE

extern sysflags;

void Decode(char*s);

extern
"C"
BOOL
_SDKPROC
SCSetScVarS(char*name, char*string)
{
    if(sc_vars->SetVarS(name, string))
    {
        if(!lstrcmpi(name, "library_password"))
        {
            char s[MAX_STRING];
            lstrcpy(s, string);
            Decode(s);
            if(!lstrcmp(s, "SC-3.097acess"))
                sysflags|=2;
            else
                sysflags&=~2;
        }
        if(!lstrcmpi(name, "user_name"))
        { WritePrivateProfileString("reg", "name", string, ini_file);}
        if(!lstrcmpi(name, "user_email"))
        { WritePrivateProfileString("reg", "email", string, ini_file);}
        return TRUE;
    }
    return FALSE;
} ;

void SetCntBarStyle(int d);

extern
"C"
BOOL
_SDKPROC
SCSetScVarI(char*name, INT32 d)
{
    if(sc_vars->SetVarI(name, d))
    {
        if((!lstrcmpi(name, "cntbar_grayed"))||(!lstrcmpi(name, "cntbar_style")))
        {
            SetCntBarStyle(d);

            return TRUE;
        }
        return TRUE;
    }
    return FALSE;
} ;

extern
"C"
BOOL
_SDKPROC
SCSetScVarD(char*name, double d){return sc_vars->SetVarD(name, d);};

extern "C" BOOL SCGetScVarS(char*name, char*string)
{
    BOOL rez = GetManager(name)->GetVarS(name, string);
    if(!rez)
    {
        if(!lstrcmpi(name, "user_name"))
        {
            GetValueData("Register", "Name", string, 128);
            return rez;
        }
        if(!lstrcmpi(name, "user_email"))
        {
            GetValueData("Register", "EMail", string, 128);
            return rez;
        }
    }
    return rez;
} ;

extern
"C"
BOOL
_SDKPROC
SCGetScVarI(char*name, INT32&d){return GetManager(name)->GetVarI(name, d);};

extern
"C"
int
_SDKPROC
SCGetScVar(char*name, int def){return GetManager(name)->GetVarDef(name, def);};

extern
"C"
BOOL
_SDKPROC
SCGetScVarD(char*name, double&d){return GetManager(name)->GetVarD(name, d);};

extern
"C"
int
_SDKPROC
SC_GetScVar(char*name, int def){return sc_vars->GetVarDef(name, def);};

extern
"C"
BOOL
_SDKPROC
SC_GetScVarD(char*name, double&d){return sc_vars->GetVarD(name, d);};

extern
"C"
BOOL
_SDKPROC
SC_GetScVarS(char*name, char*string){return sc_vars->GetVarS(name, string);};

extern
"C"
BOOL
_SDKPROC
SC_GetScVarI(char*name, INT32&d){return sc_vars->GetVarI(name, d);};

extern
"C"
void
_SDKPROC
SCDeleteScVar(char*name){sc_vars->DeleteVar(name);}

void StoreScVars(TStream&st){sc_vars->Store(st);};

void LoadScVars(TStream&st){sc_vars->Load(st);};

void InitScVars(){sc_vars=new TScVarManager();};
void DoneScVars()
{
    delete sc_vars;
    sc_vars=NULL;
} ;
