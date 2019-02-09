/*
Динамические переменные

dynodata.cpp
*/

#include "class.h"
#include "stream.h"
#include "derrors.h"
#include "vars_m.h"
#include "equ.h"
#include "project.h"

#include <SC3/GRAPHIC/collect.h>

typedef long dyno_t;

class TDynItem
{
public:
    PClass _class;
    char _data[1];
};

class TDynVar: public TCollection
{
public:
    void *project;

    TDynVar();
    TDynVar(PStream);
    ~TDynVar();

    void Store(PStream);
    void FreeItem(pointer p);

    TDynItem* CreateItem(PClass _class);
};

class TMemVarManager: public TCollection
{
public:
    TMemVarManager();
    ~TMemVarManager();

    void Done(void* project);
    static void Error(int code);

    dyno_t NewVar();
    BOOL DeleteVar(dyno_t);

    C_TYPE InsertInVar(dyno_t, char *);
    BOOL DeleteInVar(dyno_t, C_TYPE index);

    C_TYPE GetCount(dyno_t);

    char* GetType(dyno_t, C_TYPE index);
    int GetVarType(dyno_t, C_TYPE index, char *name);

    BOOL SetVar(dyno_t, C_TYPE index, char *name, BYTE type, void* data);

    dyno_t DirectoryInfo(char*, int attr);
    BOOL Store(dyno_t, TStream *);
    dyno_t Load(TStream *);
    BOOL Sort(dyno_t h,char** Fields, INT16 Fcount, bool *asc=NULL, INT16 FieldsDirection=1);

    TDynVar* Get(dyno_t);
    TDynItem* GetItem(dyno_t, C_TYPE index);

    C_TYPE index_by_H(dyno_t h)
    {
        C_TYPE i = h-16384L;
        if(i >= count)
            i= -1 ;
        return i;
    }

    dyno_t H_by_index(C_TYPE i)
    {
        return (dyno_t)i + 16384;
    }

private:
    void QuickSort(dyno_t h,char** Fields, INT16 &Fcount, int first, int last, bool *asc=NULL, INT16 FieldsDirection=1);
    void FreeItem(pointer p);
    dyno_t InsertVar(TDynVar *);
};

extern TMemVarManager* dyno_mem;
