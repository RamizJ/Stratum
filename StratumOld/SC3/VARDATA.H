/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
                        class.h
Author:
                        Alexander Shelemekhov
*/
#ifndef __VARDATA_H_
#define __VARDATA_H_
#ifndef __CLASS__SC3_H_
#include "class.h"
#endif

#define VDF_MODIFY 1
#define VDF_SUBMODIFY 2

class TVarData
{
public:
    PClass _class;   // класс

    DWORD flags;

    TCollection *items;
    TCollection *titles;

    TVarData(PClass _class);
    TVarData(TVarData&);
    ~TVarData();

    char *GetTitle(int);
    INT16 Insert(char*name, char*type, BOOL fix=0);
    PVarInfo Search(char*,INT16*index);
    BOOL Delete(INT16);

    PVarInfo CreateInfo(INT16* count);

    void UpdateVarInfo();

    BOOL Undo();// Удаляет незафиксированные переменные
    BOOL Fix(); // Фиксирует

    BOOL DeleteCompilerVars();
    BOOL Modifyed(){return (flags&VDF_MODIFY)!=0;}

    C_TYPE GetRealIndex(C_TYPE);// Если локальные
    C_TYPE GetNonLocalCount();

    BOOL SubModifyed()
    {
        BOOL r=(flags&VDF_SUBMODIFY)!=0;
        flags&=(~VDF_SUBMODIFY);
        return r;
    }

private:
    void DeleteEntry(INT16 i);
};

#endif  // __VARDATA_H_
