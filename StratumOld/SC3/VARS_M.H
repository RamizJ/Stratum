/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
            vars_m.h
Author:
            Alexander Shelemekhov
Description:
         Sc var menager
*/

#if !defined(SC_VARS_M_H_)
#define SC_VARS_M_H_

#include "../BC/bc_include_stratum/COLLECT.H"

class TScVarManager:public TSortedCollection
{
    typedef struct tagSCM
    {
        BYTE type;
        BYTE size;
        char name[1];
    }SCM;

public:
    TScVarManager():TSortedCollection(10,10){}
    ~TScVarManager();
    void 	FreeItem(pointer p){delete p;}
    virtual int    Compare(pointer key1,pointer key2);
    //****************************************************
    BOOL SetVarS(char*,char*);
    BOOL SetVarI(char*,INT32);
    BOOL SetVarD(char*,double);
    BOOL GetVarS(char*,char*);
    BOOL GetVarI(char*,INT32&);
    BOOL GetVarD(char*,double&);
    int  GetVarDef(char*,int def);

    BOOL DeleteVar(char*);

    SCM* __CreateVar(char* name, BYTE type, int size);
    SCM * GetVar(char*name, C_TYPE& index);

    void Store(TStream&st);
    BOOL Load(TStream&st);

    void ToText(TStream&st,UINT16 flags); // 1 - title | 2 - types
};

extern TScVarManager* sc_vars;
#endif
