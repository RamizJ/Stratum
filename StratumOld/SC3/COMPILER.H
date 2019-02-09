/*
Copyright (c) 1996  TST
Project: Stratum 3.0
Module :
          Fast Compiler
Author:
            Vladimir Noskov
*/

#include "vardata.h"
#include "class.h"

//#include "vmcodes.h"
//#include <iostream.h>

#define MAX_STRING 256
#define MAX_STACK 128
#define MAX_FNAME 64
class TMacro;
class TMacroCollection;
class TOperator;
class TOperatorCollection;
class ID;

class TIndexStack
{
    C_TYPE buf[MAX_STACK];
    C_TYPE ix;
public:
    TIndexStack() {ix = 0; };
    C_TYPE Push(C_TYPE k)
    {
        if (ix != MAX_STACK)
        {
            buf[ix++] = k;
            return 1;
        }
        else
            return 0;
    };
    C_TYPE Pop()
    {
        if (ix)
            return buf[--ix];
        else
            return 0;
    };
    C_TYPE GetTop(){return buf[ix-1];};
    C_TYPE GetSize(){ return ix; };
    void Clear() {ix = 0; };
    C_TYPE NoEmpty() {return ix > 0; };
    C_TYPE Empty() {return ix <= 0; };
    void IncreaseTop() { buf[ix-1]++;};
};

class TVarStack
{
    PClassListInfo buf[MAX_STACK];
    INT16 code[MAX_STACK];
    C_TYPE ix; //количество элементов в стеке

public:
    TVarStack() {ix = 0; }

    C_TYPE Push(PClassListInfo k,INT16 = -1);
    PClassListInfo Pop(int&);
    PClassListInfo GetTop() { return buf[ix-1]; }
    C_TYPE SetPtr(TStream* s =0);
    C_TYPE GetSize() { return ix; }
    void Clear() {ix = 0; }
    C_TYPE NoEmpty() {return ix > 0; }
    C_TYPE Empty() {return ix <= 0; }
};

class TStringStack
{
    char buf[MAX_STACK][MAX_FNAME];
    C_TYPE ix;

public:
    TStringStack() { ix = 0; }

    C_TYPE Push(char* str)
    {
        if (ix != MAX_STACK)
        {
            lstrcpy(&buf[ix][0],str);
            ix+=(C_TYPE)1;
            return 1;
        } else return 0;
    };
    char* Pop() { if (ix) return &buf[--ix][0]; else return buf[0]; }
    char* GetTop() { return &buf[ix-1][0]; }
    C_TYPE GetSize() { return ix; }
    void Clear() {ix = 0; }
    C_TYPE NoEmpty() {return ix > 0; }
    C_TYPE Empty() {return ix <= 0; }
};

class TEQStack
{
    TEqu* buf[MAX_STACK];
    C_TYPE ix;

public:
    TEQStack() {ix = 0; }
    C_TYPE Push(TEqu* k)
    {
        if (ix != MAX_STACK)
        {
            buf[ix++] = k;
            return 1;
        } else return 0;
    }
    TEqu* Pop() { if (ix) return buf[--ix]; else return buf[0]; }
    TEqu* GetTop() { return buf[ix-1]; }
    C_TYPE GetSize() { return ix; }
    void Clear() {ix = 0; }
    C_TYPE NoEmpty() {return ix > 0; }
    C_TYPE Empty() {return ix <= 0; }
};

class MCompiler
{
    TOperatorCollection* Operators;
    TOperatorCollection* Functions;
    TMacroCollection* Constants;

    TOperator* Epilog;

    TIndexStack Stack;
    TVarStack VarStack;
    TIndexStack FunStack;
    TStringStack FunNStack;
    TEQStack EQStack;
    TStringStack SwitchVar;

    TVarData* pvardata;
    TEqu* root;
    TStream* outcode;
    TClass* pclass;

    C_TYPE Failed;
    char FirstString[MAX_STRING];
    C_TYPE FunFlag_ix,UNARY_ix;//,UNARYN_ix;
    C_TYPE Err_x1,Err_x2;
    C_TYPE VFunction_ix,DLLFunction_ix;
    C_TYPE GetElement_ix,SetElement_ix;
    char ErrText[255];
    // char* path;

    C_TYPE New_modificator;
    C_TYPE OnePhase_mode;
    C_TYPE EQ_mode;
    C_TYPE FN_mode;
    ID* id;
    // C_TYPE Array_Element;
    C_TYPE IsDiffer;

public:
    MCompiler(char* = 0);
    ~MCompiler();

    C_TYPE Compile(TClass*,TVarData*,char*);
    // C_TYPE AnalizeOPTR(C_TYPE,C_TYPE = -1);

    C_TYPE AnalizeOPTR(C_TYPE,char* = 0);
    C_TYPE EQAnalyze();

    C_TYPE CheckStack(C_TYPE = 0);
    C_TYPE CheckVarStack();
    C_TYPE CheckEQStack();

    char* GetLastError();
    void GetLastErrorPos(C_TYPE* px,C_TYPE* py);

    C_TYPE OperatorOut(TOperator*,C_TYPE = 0);
    C_TYPE FunctionOut();        // обычная
    C_TYPE FunctionOut2(C_TYPE,PClassListInfo*, C_TYPE); // переменное число аргументов
    C_TYPE FunctionOut3(char*,PClassListInfo*, C_TYPE);  // функция имидж
    C_TYPE FunctionOut4(char*,PClassListInfo*, C_TYPE);  // функция DLL
    C_TYPE VarOut(char*);
    C_TYPE ConstantOut(char*);

    C_TYPE ReadDefVars(char*);
    C_TYPE ReadMString(C_TYPE = 0);
    C_TYPE ReadString(C_TYPE,C_TYPE = 0,TIndexStack* = 0);

    void SetError (C_TYPE,char* = 0,C_TYPE = 0);
    C_TYPE SetLabel(C_TYPE = -1);
    C_TYPE LinkLabel(C_TYPE);

    TVarInfo* InsertVar(C_TYPE&,char*,char* = 0);
    C_TYPE ReadMessageDef();
    C_TYPE SetVars(C_TYPE);

    void FillList(HWND);
    int  GetColor(char*,C_TYPE size);
    BOOL GetConstant(char*,double&);
    void FunctionHelp(HWND,C_TYPE);
    BOOL GetFunctionName(INT16,char*);

    // C_TYPE LinearTest(TEqu*,C_TYPE&);
};

MCompiler extern * compiler;
