/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
			equ.h
Author:
			Alexander Shelemekhov
*/
#ifndef __EQU__SC3_H_
#define __EQU__SC3_H_

#define EC_CONST 0
#define EC_EQSTRING  16383
#define EC_VAR    16384
#define EC_EQDIF  16385
#define EC_UNMINUS V_UN_MINUS
#define EC_PLUS V_ADD_F
#define EC_MINUS V_SUB_F
#define EC_MUL V_MUL_F
#define EC_DIV V_DIV_F
#define EC_STEPEN V_STEPEN

#define Matrix(x,y) kmatrix[(x)*(Size+1)+(y)]
#define PMatrix(x,y) pmatrix[(x)*(Size+1)+(y)]
	

class TObject;
typedef TObject*PObject;

enum {eq_unknown,eq_linear1,eq_linear2,eq_linear3,eq_standart,eq_euler,eq_euler2,eq_euler3,eq_RK,eq_adams}; // type

class TEqu{
public:
//*********** members
int code;
union
{
 double value; // const
 int    index; // varindex
 TEqu* items;   // childrens
};

 TEqu* next;    // next in parent
 double* adress;
 int eqflag;

//*********** members

 TEqu();
 TEqu(TStream&);
 TEqu(TEqu*,int = 1);
 ~TEqu();
#if !defined DEMO
 void Store(TStream&);
#endif
 void AddElement(TEqu*);
 void InsertElement(TEqu*);
 void Clear() { if (code!=EC_VAR && code!=EC_CONST) items = 0; next=0; };
 void MakeCode(TStream*,double*);
 int TestVar();
// void BMinus();
 void Minus();
 void Open();
 void DivVar();
 void Div();
 void Mul();
 void Rotate();
 void UnaryMinus();
};

class TMethod
{
 public:
 TCollection* EQVars; // переменные системы
 TCollection* EQObjs; // объекты системы
 double* kmatrix; // матрица коэффициентов
 int Size; // размерность матрицы
 int n;    // количество элементов
 int Vars; // число переменных
 int Eqs;  // число уравнений
 int counter; // подсчет числа обращений для вычисления системы
 PObject pobj;

 TMethod(TCollection*,TCollection*,PObject,TCollection*);
 virtual ~TMethod();

 virtual void Calculate()=0;
 virtual void Regeneration()=0;
 virtual void GetType(char*)=0;
 void GetVar(PObject,char*,int);
 virtual void GetMX(char*,int)=0;
 virtual void GetEQ(char*,int)=0;
 int GetSize() { return Size; };
 int SearchVar(double*);
 int TestVar(double* pvar) {return EQVars->IndexOf(pvar); };
 virtual double GetElement(int i, int j ) { return Matrix(i,j); };

 void RemVarInTree(TEqu* ptr,double* var);
 int SearchVarInTree(TEqu*);
// TEqu* ExtractVar(TEqu* ptr);
};

class MLinears : public TMethod // (Методы решения линейных систем )
{
 public:
// TTEQCollection* EQElements;
 TMemoryStream* ecode;
 int Type; // тип системы
 MLinears(TCollection*,PObject,TCollection*,int,TCollection*);
 ~MLinears();
 int EQNumber;
 double** pmatrix; // матрица указателей

 TEqu* NodeCollect(TEqu* ptr);
 TEqu* NodeAdsorb(TEqu* ptr);
 void NodeSimplify(TEqu* ptr);

 int  GetWeightX(int);
 int  GetWeightY(int);
 void SwapX(int,int);
 void SwapY(int,int);
 void LAdd(int,int);
 int Swapper();

 void Regeneration();
 void Calculate();
 int Rotate(); // метод вращения (1:1)
 void SetVars();

 void GetType(char*);
 void GetMX(char*,int);
 void GetEQ(char*,int);
 void LZTest(char*);
 double GetElement(int i, int j ) { return *PMatrix(i,j); };

};

class MNoLinears : public TMethod  // (Методы решения нелинейных систем )
{
 public:
 double *B,*X,*F;
 int Type; // тип системы
 TMemoryStream* ecode;

 int MaxIter;
 double Sense;
 double Mist;

 MNoLinears(TCollection*,PObject,TCollection*,TCollection*);
 ~MNoLinears();

 void Calculate();

 int  GetWeightX(int);
 int  GetWeightY(int);
 void SwapX(int,int);
 void SwapY(int,int);
 int Swapper();

 void Regeneration();
 void ExecuteCode();
 void Raphson(); // метод Ньютона - Рафсона
 void SIteration(); // метод Простых итераций
 void SetVars();

 void GetType(char*);
 void GetMX(char*,int);
// void GetEQ(char* buf,int) { lstrcpy(buf,"Не реализовано"); }
 void GetEQ(char* buf,int);
};

class MDiffer : public TMethod  // (Методы решения систем дифференциальных уравнений)
{
 public:
 double *F,*Z,*A,*B,*C;
 double **X;
 double G;
 int Type; // тип системы
// double Step;
 TMemoryStream* ecode;
// TCollection* ecodes; // коды уравнений
// TCollection* EQVars2;// те которые снизу
 double *Step,*Time;
 double tmpTime;
 MDiffer(TCollection*,PObject,TCollection*,double*,double*,TCollection*);
 ~MDiffer();

 void Calculate();

 void Regeneration() { };
 void ExecuteCode();
 void RungeKutt();
 void Eiler();
 void Eiler2();
 void Eiler3();
 void Adams();

 void SetVars();

 void GetType(char*);
 void GetMX(char*,int);
// void GetEQ(char* buf,int) { lstrcpy(buf,"Не реализовано"); }
 void GetEQ(char* buf,int);
};

class Equation
{
 public:
 TCollection* vars;
 TEqu* tree;
 PObject po;

 Equation(TCollection*,TEqu*,PObject);
 ~Equation();
};

class TEQsCollection : public TCollection
{
 public:
 TEQsCollection(WORD asize = 64,WORD adelta = 32) : TCollection(asize,adelta){};
 virtual void FreeItem(pointer );
};

class TMethods : public TCollection
{
 public:
 TMethods(WORD asize = 64,WORD adelta = 32) : TCollection(asize,adelta){};
 virtual void FreeItem(pointer );
};

class Equations
{
 public:
 TMemoryStream* ecode;
// TEQsCollection* EQSystems;
 TCollection* EQAllVars;
 TCollection* eqs;
 TCollection* deqs;
 TMethods* mtds;
 TMethods* dmtds;
 PObject po;
 void VarsScan(PObject);
 Equations(PObject);
 ~Equations();

 void EQVarsScan(PObject,TEqu*,TCollection*);
 void EQAdd(PObject);

 void IncludeVars(TCollection*,TCollection*);
 int VarCompare(TCollection*,TCollection*);
// int OneVarCompare(TCollection*,double*);
 int FindVarInTree(TEqu*,double*);                          // new
 int TestDSystem(TCollection*,TCollection*, TEqu*); // new
// int ExtendedTest(TEqu*,int&);
 int LinearTest(TEqu*,int&);
 PObject FindMaxObject(PObject,PObject);

// void AddEqInObject(PObject);
 void Initialize();
 void Calculate();
 void GetInfo();
 void SolveSystem(double*);
 void SolveDSystem(double*);
// void Regeneration();
 int GetEQTreeInfo(HWND,DWORD,int);
};

#endif  //__EQU__SC3_H_
