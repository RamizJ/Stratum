/*
Copyright (c) 1996  TST
Project: Stratum 3.0
Module :  Equation
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "stream.h"
#include "collect.h"
#include "equ.h"
#include "mem.h"
#include "vmcodes.h"
#include "class.h"
#include "vmachine.h"
#include "treeview.h"
#include "tree_itm.h"
#include "project.h"
#include "matrix.h"
#include "dialogs.h"
#include <stratum/stratum.h>
#include "strings.rh"
//#define PUSH_VAR _PUSH_FLOAT

#define Matrix(x,y) kmatrix[(x)*(Size+1)+(y)]
#define PMatrix(x,y) pmatrix[(x)*(Size+1)+(y)]

// equ
TEqu::TEqu()
{
    memset(this,0,sizeof(TEqu));
};

TEqu::TEqu(TEqu* ptr,int first)
{
    memset(this,0,sizeof(TEqu));

    switch(ptr->code)
    {
        case EC_VAR :
        {
            code = EC_VAR;
            index = ptr->index;
            adress = ptr->adress;
            eqflag = ptr->eqflag;
            if (ptr->next && !first) next = new TEqu(ptr->next,0);
            return;
        }
        case EC_CONST :
        {
            code = EC_CONST;
            value = ptr->value;
            if (ptr->next && !first) next = new TEqu(ptr->next,0);
            return;
        }
        default :
        {
            code = ptr->code;
            if (ptr->items) items = new TEqu(ptr->items,0);
            if (ptr->next && !first) next = new TEqu(ptr->next,0);
        }
    }

};

TEqu::~TEqu()
{
    if (code!=EC_VAR && code!=EC_CONST)
    {
        if (items) delete items;
    }
    if (next) delete next;
};

#if !defined DEMO
void TEqu::Store(TStream&st){
    st.WriteWord(code);
    BYTE b=0;
    if (code==EC_VAR) { st.WriteWord(index); st.WriteWord(eqflag); }
    if (code==EC_CONST)st.Write(&value,8);
    if (code && code!=EC_VAR){
        if (items){
            b=1;st.Write(&b,1);
            items->Store(st);
        } }
    if (next){
        b=2;st.Write(&b,1);
        next->Store(st);
    }
    b=0;st.Write(&b,1);
};
#endif

TEqu::TEqu(TStream&st){
    memset(this,0,sizeof(TEqu));
    code=st.ReadWord();
    if (code==EC_VAR)
    {
        index=st.ReadWord();
        eqflag = st.ReadWord();
    }
    if (code==EC_CONST)
        st.Read(&value,8);

    BYTE b=0;
    st.Read(&b,1);
    while (b)
    {
        switch(b)
        {
            case 1:
                items = new TEqu(st);
                break;

            case 2:
                next = new TEqu(st);
                break;
        }
        st.Read(&b,1);
    }
};

void TEqu::AddElement(TEqu* dt)
{
    if (!items)
    {
        items = dt;
        return;
    }

    TEqu* ptr = items;
    while (ptr->next)
        ptr = ptr->next;
    ptr->next = dt;
}

void TEqu::InsertElement(TEqu* dt)
{
    TEqu* ptr = this;
    while (ptr->next) ptr = ptr->next;
    ptr->next = dt;
}

void TEqu::MakeCode(TStream* ecode,double* first)
{
    switch(code)
    {
        case EC_VAR :
        {
            ecode->WriteWord(PUSH_FLOAT_PTR);
            //   if (eqflag == 2) ecode->WriteLong((long)adress+1); // новое состояние
            ecode->WriteLong((long)adress);
            goto cnt;
        }
        case EC_CONST :
        {
            ecode->WriteWord(PUSH_FLOAT_const);
            ecode->Write(&value,8);
            goto cnt;
        }
    }

    if (items->next) items->next->MakeCode(ecode,0);
    if (items) items->MakeCode(ecode,0);

    ecode->WriteWord(code);
cnt:
    if (first)
    {
        ecode->WriteWord(POP_FLOAT_PTR);
        ecode->WriteLong((long)first);
    }
}

int TEqu::TestVar()
{
    int count = 0;
    switch (code)
    {
        case EC_CONST : return 0;
        case EC_VAR : if (eqflag) return 1; else return 0;
    }
    // if (!ptr->items) return ;
    if (items->next) count += items->next->TestVar();
    if (items) count += items->TestVar();
    return count;
}

void TEqu::Open()
{
    int op_code = code;  // save code
    int op_code2 = items->next->code;  // save code2
    TEqu* x = items->next->items->next; //save x
    TEqu* op2 = items; // save op2
    TEqu* op1 = items->next->items; // save op1
    TEqu* plus = items->next; // save plus

    TEqu* _mul = new TEqu; // create *2
    _mul->code = op_code;
    _mul->items = items;   // set op2
    _mul->items->next = x; // set X

    code = op_code2;
    items = plus; // set plus
    items->next = _mul;  // modify
    items->code = op_code;
    items->items = new TEqu(op2); // set copy of op2
    items->items->next = op1;     // set op1
    items->items->next->next = 0;
}

void TEqu::Minus()
{
    TEqu* modin = new TEqu; // create -1
    modin->code = EC_CONST;
    modin->value = -1;

    TEqu* node = new TEqu;  // create *
    node->code = EC_MUL;
    node->items = items; // set op2
    node->next = items->next; // set op1
    node->items->next = modin; // set -1

    items = node;
    code = EC_PLUS;
}
/*
void TEqu::Minus(int first)
{
 if (items->code == EC_PLUS || items->code == EC_MINUS)
 {
  items->Minus(0);
  code = EC_PLUS;
  return;
 }
 if (first)  { BMinus(); return; }
 TEqu* modin = new TEqu; // create -1
 modin->code = EC_CONST;
 modin->value = -1;

 TEqu* node = new TEqu;  // create *
 node->code = EC_MUL;
 node->items = items->next; // set op1
 node->items->next = modin; // set -1

 items->next = node;
}
*/
void TEqu::DivVar()
{
    TEqu* odin = new TEqu; // create 1
    odin->code = EC_CONST;
    odin->value = 1;

    TEqu* _div = new TEqu; // create DIV
    _div->code = EC_DIV;

    _div->next = items->next; // set x
    _div->items = items;      // set op1
    _div->items->next = odin; // set 1

    code = EC_MUL;
    items = _div;
}

void TEqu::Div()
{
    code = EC_MUL;

    TEqu* tmp = items->next; // save op1

    items->next = items->items; // set x
    items->next->next = 0;

    items->items = items->items->next; // set op2
    items->items->next = tmp;   // set op1
    items->items->next->next = 0;
}

void TEqu::Mul()
{
    TEqu* tmp = items->next; // save op1

    items->next = items->items->next; // set x
    items->next->next = 0;

    items->items->next = tmp;   // set op1
    items->items->next->next = 0;
}

void TEqu::Rotate()
{
    TEqu* tmp = items; // save op1
    items = items->next;
    items->next = tmp;
    items->next->next = 0;
}

void TEqu::UnaryMinus()
{
    if (items->code == EC_CONST)
    {
        value = -value;
        return;
    }

    TEqu* odin = new TEqu; // create -1
    odin->code = EC_CONST;
    odin->value = -1;

    odin->next = items;
    // items = odin;

    code = EC_MUL;
    items = odin;
}

#define node2 ptr->items
#define node1 ptr->items->next

//-------------------------------------------------------------   TMethod

TMethod::TMethod(TCollection* vars,TCollection* objs,PObject po,TCollection* trees)
{
    Vars = vars->count;
    Eqs = trees->count;
    EQVars = vars;
    EQObjs = objs;
    Size = Vars < Eqs ? Eqs:Vars;
    n = (Size+1)*(Size+1);
    kmatrix = new double[n];
    counter = 0;
    pobj = po;
};

TMethod::~TMethod()
{
    if (kmatrix) delete kmatrix;
    if (EQVars) DeleteCollection(EQVars);
    if (EQObjs) DeleteCollection(EQObjs);
};

EQGetVarName(PObject _po,double* ptr,char* buf)
{
    for(int i=0;i<_po->_class->varcount;i++)
    {
        if (_po->vars[i]==ptr)
        {
            lstrcat(buf,_po->_class->vars[i].name);
            lstrcat(buf,",");
        }
    }

    PObject po=_po->childObjects;

    while(po)
    {
        EQGetVarName(po,ptr,buf);
        po=po->next;
    }
    return TRUE;
}

void TMethod::GetVar(PObject _po,char* buf, int num)
{
    // itoa(num,buf,10);
    EQGetVarName(_po,(double*)EQVars->At(num),buf);
}

int TMethod::SearchVar(double* pvar)
{
    // if (counter) return 0;
    if (EQVars->IndexOf(pvar) >= 0)
    {
        if (++counter == Size) return 1;
        return 0;
    }

    // {
    //   if (counter++ == (EQVars->count-1)) return 1;
    //   else return 0;
    // }

    // for (int i=0; i < EQVars->count; i++)
    // {
    //  if ((double*)EQVars->At(i) == pvar)
    //  {
    //   if (counter++ == (EQVars->count-1)) { counter = 0;  return 1; }
    //	else return 0;
    //  }
    // }
    return 0;
}

int TMethod::SearchVarInTree(TEqu* ptr)
{
    int v = 0;
    if (ptr->code == EC_VAR)
    {
        if (ptr->eqflag = 1) v++;
        return v;
    }
    if (ptr->code == EC_CONST) return v;
    if (!ptr->items) return v;
    if (ptr->items->next) v += SearchVarInTree(ptr->items->next);
    if (ptr->items) v += SearchVarInTree(ptr->items);
    return v;
}

void TMethod::RemVarInTree(TEqu* ptr,double* var)
{
    if (ptr->code == EC_VAR)
    {
        if (ptr->adress == var) ptr->eqflag = 0;
        return;
    }
    if (ptr->code == EC_CONST) return;
    if (!ptr->items) return;
    if (ptr->items->next) RemVarInTree(ptr->items->next,var);
    if (ptr->items) RemVarInTree(ptr->items,var);
    return;
}
/*
TEqu* TMethod::ExtractVar(TEqu* ptr)
{
 TEqu* tmp = 0;
 if (ptr->code == EC_VAR || ptr->code == EC_CONST) return tmp;

 if (node2->TestVar()) tmp = ExtractVar(node2);
 if (node1) { if (node1->TestVar()) tmp = ExtractVar(node1); }

 switch (ptr->code)
 {


  default
 }

 return tmp;
}
*/
//-------------------------------------------------------------   MLinears
MLinears::MLinears(TCollection* trees,PObject po,TCollection* vars,int t,TCollection* objs) : TMethod(vars,objs,po,trees)
{
    Type = eq_unknown;
    ecode = new TMemoryStream(TRUE);
    TEqu* ptr;
    TEqu* prv;
    int v,i;
    EQNumber = trees->count;
    TEqu* cst = NULL;

    for (i = 0; i < trees->count; i++)   // по деревьям системы
    {
        ptr = (TEqu*)trees->At(i);
        NodeSimplify(ptr);       	// упрощаем дерево
        ptr = NodeCollect(ptr);  	// собираем плюсы в кучу
        ptr = NodeAdsorb(ptr);   	// складываем одинаковые переменные
        trees->AtPut(i,ptr);


        if(SCGetScVar("equ_nl_optimize",0))
        {
            // оптимизация

            // а пошукаем ка на простые уравнения
            if (ptr->next->code != EC_MUL && SearchVarInTree(ptr) == 1)
            {
                double* tvar = ptr->items->next->adress;

                TEqu* min = new TEqu;
                min->code = EC_UNMINUS;
                TEqu* div = new TEqu;
                div->code = EC_DIV;
                TEqu* cst = new TEqu;
                cst->code = EC_CONST;
                cst->value = ptr->items->value;

                min->items = div;
                div->items = cst;
                cst->next = ptr->next;
                ptr->next = min;

                ptr->next->MakeCode(ecode,tvar);
                ptr->next->MakeCode(ecode,tvar+1);
                delete ptr;
                Size--;
                Vars--;
                EQNumber--;
                n = (Size+1)*(Size+1);
                trees->AtFree(i);
                EQVars->AtFree(i);
                for (int j = 0; j < trees->count; j++)
                {
                    RemVarInTree((TEqu*)trees->At(j),tvar);
                }
                i--;
            }
        }
    }
    // инициализируем матрицы
    pmatrix = new double*[n];
    for (i=0; i < n; i++) pmatrix[i] = kmatrix + i;
    for (i=0; i < EQVars->count; i++) pmatrix[i+1] = ((double*)EQVars->At(i))+1; //пишем в новые значения

    // а сейчас мы ее распотрошим
    for (i = 0; i < trees->count; i++)   // по деревьям системы
    {
        ptr = (TEqu*)trees->At(i);
        do  // поехали по константам ***
        {
            if (ptr->code == EC_MUL && ptr->items->next->code == EC_VAR && (v = TestVar(ptr->items->next->adress)) >= 0)
            {
                // обрабатываем переменную !
                ptr->items->MakeCode(ecode,&Matrix(i+1,v+1));
                prv = ptr->next;
                ptr->next = NULL;
                delete ptr;
                ptr = prv;
            } else
            {
                // обрабатываем константу !
                if (cst == NULL)
                {
                    cst = ptr;
                    ptr = ptr->next;
                    cst->next = 0;
                }else
                {
                    prv = ptr;
                    TEqu* plus = new TEqu; // create +
                    plus->code = EC_PLUS;
                    // наполняем его
                    plus->items = cst;       //  ?
                    plus->items->next = ptr; //  ?

                    cst = plus;
                    ptr = prv->next;
                    prv->next=NULL;
                }
            }
        } while (ptr);

        if (cst) // а теперь пощупаем правую часть
        {
            // просто помножим на -1
            if (cst->code == EC_CONST) cst->value *= -1;
            else
            {
                if (cst->code == EC_PLUS && cst->items == EC_CONST && cst->items->next == EC_CONST)
                {
                    if (cst->items == EC_CONST) cst->items->value *= -1;
                    else cst->items->next->value *= -1;
                }
                else
                {
                    TEqu* odin = new TEqu;  // create 1
                    odin->code = EC_CONST;
                    odin->value = -1;

                    TEqu* mul = new TEqu;   // create *
                    mul->code = EC_MUL;
                    // наполняем его
                    mul->items = cst;       //  ?
                    mul->items->next = odin; // ?
                    cst = mul;
                }
            }
            cst->MakeCode(ecode,&Matrix(i+1,0));
            delete cst;
            cst = NULL;
        }
    }
    if (trees) DeleteCollection(trees);
    Type = t;
    ecode->WriteWord(0);

}

MLinears::~MLinears()
{
    if (ecode) delete ecode;
    if (pmatrix) delete pmatrix;
}

TEqu* MLinears::NodeCollect(TEqu* ptr)
{
    TEqu* ptr2;
    if (ptr->code == EC_PLUS)
    {
        ptr2 = NodeCollect(ptr->items->next);
        ptr->items->next = 0;
        ptr2->InsertElement(NodeCollect(ptr->items));
        ptr->Clear(); delete ptr;
        return ptr2;
    }
    return ptr;
}

TEqu* MLinears::NodeAdsorb(TEqu* ptr)
{
    TEqu* ptr2;
    TEqu* tmp;
    TEqu* prv = NULL;
    ptr2 = ptr;
    double d;

    do //нормализация узлов
    {
        if (ptr2->code == EC_VAR)
        {
            TEqu* odin = new TEqu; // create 1
            odin->code = EC_CONST;
            odin->value = 1;

            TEqu* mul = new TEqu; // create *
            mul->code = EC_MUL;

            mul->next = ptr2->next;
            mul->items = odin;      // set 1
            mul->items->next = ptr2; // set var

            ptr2->next = NULL;
            ptr2 = mul;
            if (prv) prv->next = ptr2;
            else ptr = prv = ptr2;
        }
        else
        {
            // если стоят плохо перевернем
            if (ptr2->code == EC_MUL && ptr2->items->code == EC_VAR && (TestVar(ptr2->items->adress)>=0)) ptr2->Rotate();
            // нулевая константа
            if (ptr2->code == EC_CONST && ptr2->value == 0)
            {
                if (prv)
                { prv->next = ptr2->next;
                    ptr2->next = NULL;
                    delete ptr2;
                    ptr2 = prv->next;
                } else
                {
                    prv = ptr2->next;
                    ptr2->next = NULL;
                    delete ptr2;
                    ptr = ptr2 = prv;
                    prv = NULL;
                }
                continue;
            }

            // если константы попробуем их упростить
            if (ptr2->code == EC_MUL && ptr2->items->code == EC_CONST && ptr2->items->next->code == EC_CONST)
            {
                ptr2->code = EC_CONST;
                d = ptr2->items->value * ptr2->items->next->value;
                delete ptr2->items;
                ptr2->value = d;
            }
            if (ptr2->code == EC_PLUS && ptr2->items->code == EC_CONST && ptr2->items->next->code == EC_CONST)
            {
                ptr2->code = EC_CONST;
                d = ptr2->items->value + ptr2->items->next->value;
                delete ptr2->items;
                ptr2->value = d;
            }
        }
        prv = ptr2;
        ptr2 = ptr2->next;
    } while (ptr2);

    ptr2 = ptr;
    do // объединение одинаковых узлов
    {
        prv = ptr2;
        tmp = ptr2->next;
        if (!tmp) break;

        if (ptr2->code == EC_MUL && ptr2->items->next->code == EC_VAR)
        {
            do // ищем похожие узлы
            {
                if (tmp->code == EC_MUL && tmp->items->next->code == EC_VAR && ptr2->items->next->adress == tmp->items->next->adress) // если переменная cовпадает
                {
                    TEqu* plus = new TEqu; // create +
                    plus->code = EC_PLUS;

                    delete tmp->items->next;
                    tmp->items->next = NULL;
                    plus->next = ptr2->items->next;
                    ptr2->items->next = NULL;
                    plus->items = tmp->items;
                    plus->items->next = ptr2->items;
                    ptr2->items = plus;

                    prv->next = tmp->next;
                    tmp->Clear();
                    delete tmp;
                    tmp = prv->next;
                } else
                {
                    prv = tmp;
                    tmp = tmp->next;
                }
            } while (tmp);
        }
        ptr2 = ptr2->next;
    } while (ptr2);

    return ptr;
}

void MLinears::NodeSimplify(TEqu* ptr)
{
    // int k = 0;
    if (ptr->code == EC_VAR || ptr->code == EC_CONST) return;

    if (node2->TestVar()) NodeSimplify(node2);
    if (node1) { if (node1->TestVar()) NodeSimplify(node1); }

    switch (ptr->code)
    {
        case EC_MINUS :
        {
            ptr->Minus();
            NodeSimplify(ptr);
            break;
        }
        case EC_MUL :
        {
            if ( node1->code == EC_CONST && node2->code == EC_CONST)
            {
                ptr->code = EC_CONST;
                double s = node1->value * node2->value;
                delete node2; // and node1;
                ptr->value = s;
                break;
            }
            if ( (node1->code == EC_PLUS || node1->code == EC_MINUS) && node1->TestVar() )
            {
                ptr->Open();
                NodeSimplify(ptr);
                break;
            }
            if ( (node2->code == EC_PLUS || node2->code == EC_MINUS) && node2->TestVar() )
            {
                ptr->Rotate();
                ptr->Open();
                NodeSimplify(ptr);
                break;
            }
            if (node1->code == EC_MUL || node2->code == EC_MUL)
            {
                if (node1->code == EC_MUL) ptr->Rotate();
                if (node2->items->next->eqflag || node2->items->eqflag)
                {
                    if (node2->items->eqflag) node2->Rotate();
                    ptr->Mul();
                    NodeSimplify(ptr);
                    break;
                }
            }
            break;
        }
        case EC_DIV :
        {
            if (node1->code == EC_VAR)
            {
                ptr->DivVar();
                NodeSimplify(ptr); // ???
                break;
            }
            if ( (node1->code == EC_MUL || node1->code == EC_PLUS || node1->code == EC_MINUS) && node1->TestVar() )
            {
                ptr->Open();
                NodeSimplify(ptr);
                break;
            }
            if ( node2->code == EC_DIV )
            {
                ptr->Div();
                NodeSimplify(ptr);
                break;
            }
            break;
        }
        case EC_UNMINUS :
        {
            ptr->UnaryMinus();
            NodeSimplify(ptr);
            break;
        }
    }
}

void MLinears::GetMX(char* buf,int i)
{
    char tmp[255];
    for (int k = 1; k <= Size; k++)
    {
        sprintf(tmp,"% 06.4f",*PMatrix(i+1,k));
        lstrcat(buf,tmp);
        lstrcat(buf," ");
    }
    lstrcat(buf," = ");
    sprintf(tmp,"% 06.4f",*PMatrix(i+1,0));
    lstrcat(buf,tmp);
}

void MLinears::GetEQ(char* buf,int i)
{
    int c = 0;
    INT16 num;

    itoa(i,buf+lstrlen(buf),10);
    lstrcat(buf,". ");
    for (int k = 1; k <= Size; k++) if (*PMatrix(i,k)) c++;

    char s[256];
    LoadString(HResource,IDS_str210,s,sizeof(s));
    wsprintf(buf+lstrlen(buf),s,c);
    // lstrcat(buf,"Коэффициентов- ");
    // itoa(c,buf+lstrlen(buf),10);
    // lstrcat(buf,", объект #");


    PObject lpo = (PObject)EQObjs->At(i-1);
    if(lpo)itoa((int)lpo->GetHandle(&num),buf+lstrlen(buf),10); // это я добавил
}

int MLinears::GetWeightX(int i)
{
    int w = 0;
    for (int k = 1; k <= Size; k++) if (*PMatrix(i,k)) w++;
    return w;
}

int MLinears::GetWeightY(int i)
{
    int w = 0;
    for (int k = 1; k <= Size; k++) if (*PMatrix(k,i)) w++;
    return w;
}

void MLinears::SwapX(int i,int j)
{
    double* tmp;
    for (int k = 0; k <= Size; k++)
    {
        tmp = PMatrix(j,k);
        PMatrix(j,k) = PMatrix(i,k);
        PMatrix(i,k) = tmp;
    }
}

void MLinears::LAdd(int k,int m)
{
    for (int z = 0; z <= Size; z++)
    {
        *PMatrix(m,z) += *PMatrix(k,z)*2;
    }
}

void MLinears::SwapY(int i,int j)
{
    double* tmp;
    // EQVars->Swap(i-1,j-1);
    for (int k = 0; k <= Size; k++)
    {
        tmp = PMatrix(k,j);
        PMatrix(k,j) = PMatrix(k,i);
        PMatrix(k,i) = tmp;
    }
}

int MLinears::Swapper()
{
    int i = 0;
    int x,y,wx,wy,tmp;
    for (int j = 1; j <= Size; j++) if (*PMatrix(j,j)) i++;
    if (i == Size) return 1;
    // if (!i)
    // {
    //  _Message("Обнаружена неразрешимая система линейных уравнений (det = 0) !");
    //  return 0;
    // }

    // search element
    for (i = 1; i <= Size; i++)
    {
        x = -1; wx = Size+1; y = -1; wy = Size+1;
        for (j = 1; j <= Size; j++)
        {
            if (*PMatrix(j,i) && *PMatrix(i,j))
            {
                tmp = GetWeightX(j);
                if (tmp < wx) { wx = tmp; x = j;}
                tmp = GetWeightY(j);
                if (tmp < wy) { wy = tmp; y = j;}
            }
        }
        if (wy < wx)
        { if (y >= 0) { SwapY(i,y); continue; }  }
        else
        { if (x >= 0) { SwapX(i,x); continue; }  }

        // adding search
        for (j = i; j <= Size; j++)
        {
            if (*PMatrix(j,i))
            {
                tmp = GetWeightX(j);
                if (tmp < wx) { wx = tmp; x = j;}
            }
            if (*PMatrix(i,j))
            {
                tmp = GetWeightY(j);
                if (tmp < wy) { wy = tmp; y = j;}
            }
        }
        if (wy < wx)  {  if (y >= 0) { SwapY(i,y); continue; }  }
        else
        { if (x >= 0) { SwapX(i,x); continue; }  }

        // а теперь суммируем строки !
        //  for (j = 1; j <= Size; j++)
        //  {
        //	if (*PMatrix(j,i)) break;
        //  }
        //  LAdd (j,i);
    }

    return 1;
}


int MLinears::Rotate() // метод вращения
{
    int n = Size;
    int i,j,k;
    double a,b,r,l,m;
    for (i = 1; i <= n-1; i++)
    {
        for (k = i+1; k <= n; k++)
        {
            a = *PMatrix(i,i); b = *PMatrix(k,i);
            if (a != 0 || b != 0)
            {
                m = sqrt(a*a + b*b);
                l = -b / m;	m = a / m;
            }
            else
            {
                m = 1; l = 0;
            }
            for (j=1; j<=n; j++)
            {
                r = m * *PMatrix(i,j) - l * *PMatrix(k,j);
                *PMatrix(k,j) = l * *PMatrix(i,j) + m * *PMatrix(k,j);
                *PMatrix(i,j) = r;

            }
            r = m * *PMatrix(i,0) - l * *PMatrix(k,0);
            *PMatrix(k,0) = l * *PMatrix(i,0) + m * *PMatrix(k,0);
            *PMatrix(i,0) = r;
        }
    }

    /*
  int mm = matrixs->MCreate(0,1,n,0,n,0);
  for(int z=0; z <= n ; z++)
  {
   for(int y=0; y <= n; y++)
   {
     matrixs->MPut(mm,z,y,*PMatrix(z,y));
   }
  }
  MatrixDialog(mainwindow->HWindow,mm);
  matrixs->MDelete(mm);
*/
    /*
  // проверим диагональ
  for (i = 1; i <= Size; i++)
  {
   if (*PMatrix(i,i)) continue;
   // а теперь суммируем строки !
   for (int j = 1; j <= Size; j++)
   {
    if (*PMatrix(j,i)) break;
   }
   LAdd (j,i);
  }
*/

    for (i = n; i >= 1; i--)
    {
        m = 0;
        for (k = 0; k <= n-i-1; k++)
        {
            m += *PMatrix(0,n-k) * *PMatrix(i,n-k);
        }
        if (*PMatrix(i,i) == 0)
            return 0;
        *PMatrix(0,i) = (*PMatrix(i,0) - m) / *PMatrix(i,i);
    }


    return 1;
}
/*
int MLinears::SolveP() // система с переопределенной матрицей
{
 int n = Size;
 for j = 1 n
 {
  for k = j n
  {
   s = 0
   for i=1 m
   {
    s= s + a(i,j)*a(i,k)
   }
   c(k)=s
  }
  c = 0
  for i = 0
 }
}
*/
void MLinears::SetVars()
{
    for (int i=0; i < EQVars->count; i++)  // set solving values
    {
        *(((double*)EQVars->At(i))+1) = *pmatrix[i+1]; // пишем в новые значения
        //  *(((double*)EQVars->At(i))) = kmatrix[i+1]; // пишем в старые значения
    }
}

void MLinears::Regeneration()
{
    for (int x = Size; x < n; x++) kmatrix[x] = 0;
    if (!VM_Execute((WORD*)ecode->base,0,0)) {
        //_Message("Ошибка в ВМ при выполнении вспомогательного кода линейного уравнения !");
        _Message(IDS_str172);
        return; }
}

void MLinears::Calculate()
{
    if (Type == eq_linear1)
    {
        for (int x = Size; x < n; x++) kmatrix[x] = 0;
        if (!VM_Execute((WORD*)ecode->base,0,0)) {
            //_Message("Ошибка в ВМ при выполнении вспомогательного кода линейного уравнения !");
            _Message(IDS_str172);
            return; }
        if (!Size) return;
        if (!Swapper()) return;
        if (!Rotate()) {// _Message("Неразрешимая система линейных уравнений (линейная зависимость)!");
            _Message(IDS_str171);
            return;}
    }
    /*
 {
  for (int x = Size; x < n; x++) kmatrix[x] = 0;
  if (!VM_Execute((WORD*)ecode->base,0,0)) { _Message("Ошибка в ВМ при выполнении вспомогательного кода линейного уравнения !"); return; }
  if (!Rotate())
  {
    for (int x = Size; x < n; x++) kmatrix[x] = 0;
    if (!VM_Execute((WORD*)ecode->base,0,0)) { _Message("Ошибка в ВМ при выполнении вспомогательного кода линейного уравнения !"); return; }
    if (!Swapper()) return;
    if (!Rotate()) { _Message("Неразрешимая система линейных уравнений (линейная зависимость)!"); return;}
  }
 }
*/
}

void MLinears::LZTest(char* buf)
{
    int i,j,k,l;
    Regeneration();
    for (i = 1; i <= Size; i++) // едем по строкам
    {
        for (j = i+1; j <= Size; j++) // едем по оставшимся
        {
            l = 1;
            for (k = 1; k <= Size; k++) // едем по уравнению
            {
                if (*PMatrix(i,k) != *PMatrix(j,k))  { l = 0; break;}
            }
            if (l)
            {
                lstrcat(buf," (Warning ");
                itoa(i,buf+lstrlen(buf),10);
                lstrcat(buf," = ");
                itoa(j,buf+lstrlen(buf),10);
                lstrcat(buf," !)");
                return;
            }
        }
    }

}

void MLinears::GetType(char* buf)
{
    switch (Type)
    {
        case eq_linear1 :
        {
            //   lstrcpy(buf,"Система линейных уравнений");
            LoadString(HResource,IDS_str173,buf,80);
            LZTest(buf);
            break;
        }
        case eq_linear2 :  {
            //lstrcpy(buf,"Переопределенная система линейных уравнений (Не решаема !)");
            LoadString(HResource,IDS_str175,buf,80);
            break; }
        case eq_linear3 :  {
            // lstrcpy(buf,"Недоопределенная система линейных уравнений (Не решаема !)");
            LoadString(HResource,IDS_str174,buf,80);

            break; }
    }
    lstrcat(buf," (");
    itoa(EQNumber,buf+lstrlen(buf),10);
    lstrcat(buf,"x");
    itoa(Size,buf+lstrlen(buf),10);
    lstrcat(buf,")");
};

//-------------------------------------------------------------   MNoLinears

MNoLinears::MNoLinears(TCollection* trees,PObject po,TCollection* vars,TCollection* objs) : TMethod(vars,objs,po,trees)
{
    Type = eq_unknown;
    int t;
    ecode = new TMemoryStream(TRUE);
    // creating collections of elements equations
    B = new double[Size];
    X = new double[Size]; // буфер для неизвестных
    F = new double[Size]; // результат решения уравнения

    TEqu* ptr;


    for (int i = 0; i < trees->count; i++)
    {
        ptr = (TEqu*)trees->At(i);

        if(SCGetScVar("equ_nl_optimize",0))
        {
            // оптимизация

            if (ptr->items->code==EC_CONST && ptr->items->next->code==EC_MUL && ptr->items->next->items->code==EC_VAR && ptr->items->next->items->eqflag==1 && SearchVarInTree(ptr) == 1)
            {
                double* tvar = ptr->items->next->items->adress;
                TEqu* tmp;

                ptr->items->next->items->code = EC_CONST;
                ptr->items->next->items->value = ptr->items->value;
                ptr->items->next->code = EC_DIV;
                tmp = ptr->items->next;
                ptr->items->next = 0;
                delete ptr;

                tmp->Rotate();
                tmp->MakeCode(ecode,tvar);
                tmp->MakeCode(ecode,tvar+1);
                delete tmp;
                Size--;
                Vars--;
                n = (Size+1)*(Size+1);
                trees->AtFree(i);
                EQVars->AtFree(i);
                for (int j = 0; j < trees->count; j++) RemVarInTree((TEqu*)trees->At(j),tvar);
                i--;
                continue;
            } else
            {
                if (ptr->items->code==EC_CONST && ptr->items->next->code==EC_VAR && ptr->items->next->eqflag==1 && SearchVarInTree(ptr) == 1)
                {
                    double* tvar = ptr->items->next->adress;
                    ptr->code = EC_DIV;
                    ptr->items->next->code = EC_CONST;
                    ptr->items->next->value = 1;
                    ptr->Rotate();
                    ptr->MakeCode(ecode,tvar);
                    ptr->MakeCode(ecode,tvar+1);
                    delete ptr;
                    Size--;
                    Vars--;
                    n = (Size+1)*(Size+1);
                    trees->AtFree(i);
                    EQVars->AtFree(i);
                    for (int j = 0; j < trees->count; j++) RemVarInTree((TEqu*)trees->At(j),tvar);
                    i--;
                    continue;
                }
            }
        }

        ptr->MakeCode(ecode,&F[i]);
        delete ptr;
    }

    ecode->WriteWord(0);
    if (trees) DeleteCollection(trees);

    MaxIter = SCGetScVar("equ_nl_maxstep",30);
    t = SCGetScVarD("equ_nl_sense",Sense);
    if (!t || !Sense) Sense = 0.1;
    t = SCGetScVarD("equ_nl_error",Mist);
    if (!t || !Mist) Mist = 0.001;
}

MNoLinears::~MNoLinears()
{
    if (B) delete B;
    if (X) delete X;
    if (F) delete F;
    if (ecode) delete ecode;
}

void MNoLinears::GetType(char* buf)
{
    // lstrcpy(buf,"Система нелинейных уравнений");
    LoadString(HResource,IDS_str176,buf,80);
    lstrcat(buf," (");
    itoa(Size,buf+lstrlen(buf),10);
    lstrcat(buf,")");
}


void MNoLinears::GetMX(char* buf,int i)
{
    char tmp[255];
    sprintf(tmp,"% 06.4f",X[i]);
    lstrcat(buf,tmp);
    lstrcat(buf," ");
}

void MNoLinears::GetEQ(char* buf,int i)
{
    INT16 num;
    //lstrcat(buf,"Объект #");
    LoadString(HResource,IDS_str211,buf+lstrlen(buf),20);
    PObject lpo = (PObject)EQObjs->At(i-1);
    itoa((int)lpo->GetHandle(&num),buf+lstrlen(buf),10);
}

int MNoLinears::GetWeightX(int i)
{
    int w = 0;
    for (int k = 0; k < Size; k++) if (Matrix(i,k)) w++;
    return w;
}

int MNoLinears::GetWeightY(int i)
{
    int w = 0;
    for (int k = 0; k < Size; k++) if (Matrix(k,i)) w++;
    return w;
}

void MNoLinears::SwapX(int i,int j)
{
    double tmp;
    // EQElements->Swap(i-1,j-1);
    for (int k = 0; k < Size; k++)
    {
        tmp = Matrix(j,k);
        Matrix(j,k) = Matrix(i,k);
        Matrix(i,k) = tmp;
    }
}

void MNoLinears::SwapY(int i,int j)
{
    double tmp;
    EQVars->Swap(i,j);
    tmp = B[i]; B[i] = B[j]; B[i] = tmp;
    tmp = X[i]; X[i] = X[j]; X[i] = tmp;
    tmp = F[i]; F[i] = F[j]; F[i] = tmp;

    for (int k = 0; k < Size; k++)
    {
        tmp = Matrix(k,j);
        Matrix(k,j) = Matrix(k,i);
        Matrix(k,i) = tmp;
    }
}

int MNoLinears::Swapper()
{
    int i = 0;
    int x,y,wx,wy,tmp;
    for (int j = 0; j < Size; j++) if (Matrix(j,j)) i++;
    if (i == Size) return 1;
    // if (!i)
    // {
    //  Type = eq_unknown;
    //  _Message("Обнаружена неразрешимая система нелинейных уравнений !");
    //  return 0;
    // }
    // search element
    for (i = 0; i < Size; i++)
    {
        //  if (Matrix(i,i)) continue;
        // search kandidat for swapping
        // horizontal
        x = -1; wx = Size + 1; y = -1; wy = Size + 1;
        for (j = 0; j < Size; j++)
        {
            if (Matrix(j,i) && Matrix(i,j))
            {
                tmp = GetWeightX(j);
                if (tmp < wx) { wx = tmp; x = j;}
                tmp = GetWeightY(j);
                if (tmp < wy) { wy = tmp; y = j;}
            }
        }
        if (y >= 0) { SwapY(i,y); continue; }
        if (x >= 0) { SwapX(i,x); continue; }

        // adding search
        for (j = i; j < Size; j++)
        {
            if (Matrix(j,i))
            {
                tmp = GetWeightX(j);
                if (tmp < wx) { wx = tmp; x = j;}
            }
            if (Matrix(i,j))
            {
                tmp = GetWeightY(j);
                if (tmp < wy) { wy = tmp; y = j;}
            }
        }
        if (y >= 0) { SwapY(i,y); continue; }
        if (x >= 0) { SwapX(i,x); continue; }
    }

    return 1;
}

void MNoLinears::Regeneration()
{
    for (int i = 0; i < Size; i++)
    {
        X[i] = *((double*)EQVars->At(i));
    }
}

void MNoLinears::ExecuteCode()
{
    for (int i = 0; i < Size; i++)
    {
        *((double*)EQVars->At(i)) = X[i];
    }
    if (!VM_Execute((WORD*)ecode->base,0,0))
    {
        //_Message("Ошибка в ВМ при выполнении вспомогательного кода нелинейного уравнения !");
        _Message(IDS_str178);
        return; }
}

void MNoLinears::Raphson()
{
    float rr,Step;
    int Iter = 0;
    int i,j,l,over;

    // инициализация массива X
    for (j=0; j < Size; j++)  X[j] = *((double*)EQVars->At(j));

    do
    {
        ExecuteCode();
        //  over = 0;

        for (j = 0; j < Size; j++)
        {
            B[j] = -F[j];
            //   over = 0;
        }

        for (j = 0; j < Size; j++)
        {
            rr = X[j];
            Step = Mist*fabs(rr) + Mist;
            X[j] = rr + Step;
            ExecuteCode();
            for (i = 0; i < Size; i++)
            {
                //	over = over || (F[i] + B[i] > Mist);
                Matrix(i,j) = (F[i] + B[i])/Step;
            }
            X[j] = rr;
            if (!X[j]) X[j] = Mist; // !!
        }

        // сортировка матрицы
        if (!Swapper()) return;

        for (j = 0; j < Size; j++) // по уравнениям
        {
            for (i = 0; i < j; i++) // по обработанным уравнениям
            {
                rr = Matrix(j,i);
                for (l = 0; l < Size; l++) // по всем переменным
                    Matrix(j,l) = Matrix(j,l) - Matrix(i,l)*rr; // вычтем из текущего уравнения }
                B[j] = B[j] - B[i]*rr;       // предыдущие умноженные на rr
            }
            rr = Matrix(j,j);
            if (rr != 0)
            {
                //	if (rr != 1)
                //	{
                for (i = j; i < Size; i++) Matrix(j,i) = Matrix(j,i)/rr;
                B[j] = B[j]/rr;
                //	}
            } else
            {
                _Message(IDS_str177);
                //	_Message("Обнаружена неразрешимая система нелинейных уравнений !");
                return;
            }
        }

        for (j = Size-1; j >= 0; j--)
        {
            rr = 0;
            for (i = j + 1; i < Size; i++) rr = rr - F[i]*Matrix(j,i);
            F[j] = rr + B[j];
        }

        over = 0;
        for (i = 0; i < Size; i++)
        {
            X[i] = X[i] + F[i]*Sense;
            if (fabs(X[i]) > 1e+10)
            {
                _Message(IDS_str179);
                //	_Message("Обнаружено переполнение в системе нелинейных уравнений (переполнение)!");
                return;
            }
            over = over || (fabs(F[i]) > Mist);
        }
    } while(over && (Iter++ < MaxIter));
}

void MNoLinears::SIteration() // метод Простых итераций
{
    double E = 0.0001;
    int m = 30; // максимальное число циклов
    int s = 0;
    do
    {
        ExecuteCode();
        for (int i = 0; i < Size; i++)
        {
            if (fabs(F[i]-X[i]) > E) s = 1;
            X[i] = F[i];
        }
    } while (s);
}

void MNoLinears::SetVars()
{
    for (int i = 0; i < Size; i++)
    {
        *(((double*)EQVars->At(i))+1) = X[i]; // пишем в новые значения
    }
}

void MNoLinears::Calculate()
{
    // if (!SearchVar(pvar)) return 0;
    if (!Size) return;
    Regeneration();
    Raphson();
    // SIteration();
    SetVars();
}

//-------------------------------------------------------------   MDiffer
MDiffer::MDiffer(TCollection* trees,PObject po,TCollection* vars,double* st,double* tm,TCollection* objs) : TMethod(vars,objs,po,trees)
{
    Type = eq_unknown;
    TEqu* tree;
    Size = vars->count;
    // creating collections of elements equations
    Step = st;

    if (tm) Time = tm;
    else
    {
        //   _Message("Необходимо перекомпилировать дифференциальные уравнения !");
        _Message(IDS_str209);
        //   "Необходимо перекомпилировать дифференциальные уравнения !"
        Time = &tmpTime;
    }
    ecode = new TMemoryStream(TRUE);

    F = new double[Size];  // результат вычисления ф-ии
    Z = new double[Size];  // старое значение
    A = new double[Size];
    B = new double[Size];
    C = new double[Size];
    X = (double**) new UINT32[Size]; // переменные

    for (int i = 0; i < trees->count; i++)
    {
        tree = (TEqu*)trees->At(i);
        tree->items->next->MakeCode(ecode,&F[i]);
        delete tree;
    }
    Type = eq_standart;

    char buf[255];
    SCGetScVarS("equ_diff",buf);
    if (!lstrcmp(buf,"Euler's")) Type = eq_euler;
    if (!lstrcmp(buf,"Euler's 2")) Type = eq_euler2;
    if (!lstrcmp(buf,"Euler's 3")) Type = eq_euler3;
    if (!lstrcmp(buf,"Runge-Kutt")) Type = eq_RK;
    if (!lstrcmp(buf,"Adams")) Type = eq_adams;
    // старые значения
    for (i = 0; i < Size; i++) { B[i] = C[i] = 0; A[i] = Z[i] = *(((double*)EQVars->At(i))+1); }
    for (i = 0; i < Size; i++) { X[i] = (double*)EQVars->At(i); }
    ecode->WriteWord(0);
    if (trees) DeleteCollection(trees);
}

MDiffer::~MDiffer()
{
    if (F) delete F;
    if (Z) delete Z;
    if (A) delete A;
    if (B) delete B;
    if (C) delete C;
    if (X) delete X;
    // if (EQVars2) DeleteCollection(EQVars2);
    // for (int i = 0; i < Size; i++)  delete ((TMemoryStream*)ecodes->At(i));
    if (ecode) delete ecode;
    // if (ecodes) DeleteCollection(ecodes);
}

void MDiffer::GetType(char* buf)
{
    LoadString(HResource,IDS_str180,buf,80);
    // lstrcpy(buf,"Система дифференциальных уравнений");
    lstrcat(buf," (");
    itoa(Size,buf+lstrlen(buf),10);
    lstrcat(buf,")");
}


void MDiffer::GetMX(char* buf,int i)
{
    char tmp[255];
    sprintf(tmp,"% 06.4f",F[i]);
    lstrcat(buf,tmp);
    lstrcat(buf," ");
}

void MDiffer::GetEQ(char* buf,int i)
{
    INT16 num;
    lstrcpy(buf,"-");
    // PObject lpo = (PObject)EQObjs->At(i-1);
    // itoa((int)lpo->GetHandle(&num),buf+lstrlen(buf),10);
    // itoa((int)pobj->GetHandle(&num),buf+lstrlen(buf),10);
}

void MDiffer::ExecuteCode()
{
    // if (!VM_Execute((WORD*)((TMemoryStream*)ecodes->At(n))->base,0,0))  { _Message("Ошибка в ВМ при выполнении вспомогательного кода дифуравнения !"); return; }
    if (!VM_Execute((WORD*)ecode->base,0,0))
    { //_Message("Ошибка в ВМ при выполнении вспомогательного кода дифференциального уравнения !");
        _Message(IDS_str181);
        return; }
}

void MDiffer::Eiler()
{
    Time[1] += *Step;
    ExecuteCode();
    for (int i = 0; i < Size; i++ )
    {
        *(((double*)EQVars->At(i))+1) += F[i] * *Step;
    }
}

void MDiffer::Eiler2()
{
    ExecuteCode();  // предсказывающая формула
    for (int i = 0; i < Size; i++ )
    {
        A[i] = X[i][1];
        C[i] = F[i];
        X[i][1] = A[i] + F[i] * *Step;
    }

    Time[1] += *Step;
    ExecuteCode();
    for (i = 0; i < Size; i++ )
    {
        X[i][1] = A[i] + *Step * (C[i] + F[i])/2;
    }

}

void MDiffer::Eiler3() // c управляемой точностью доделать !
{
    double E = 0.0000001;
    int sw = 1;
    int n = 40; // максимальное число циклов

    ExecuteCode();  // предсказывающая формула
    for (int i = 0; i < Size; i++ )
    {
        A[i] = X[i][1];
        C[i] = F[i];
        B[i] = X[i][1] = A[i] + F[i] * *Step;
    }

    Time[1] += *Step;
    do
    {
        ExecuteCode();
        for (i = 0; i < Size; i++)
        {
            X[i][1] = A[i] + *Step * (C[i] + F[i])/2;
            sw = sw * (fabs(B[i] - X[i][1]) >= E);
            G = fabs(B[i] - X[i][1]);
            B[i] = X[i][1];
        }
        if (! n--) break;
    } while (sw);

}

void MDiffer::RungeKutt()
{
    double h = *Step;
    // Time[1] += *Step/4;

    ExecuteCode();
    for (int i = 0; i < Size; i++)
    {
        Z[i] = X[i][1];
        A[i] = F[i]* h;
        X[i][1] = Z[i] + A[i];
    }
    Time[1] += *Step/2;

    ExecuteCode();
    for (i = 0; i < Size; i++)
    {
        B[i] = F[i]* h;
        X[i][1] = Z[i] + B[i]/2;
    }
    // Time[1] += *Step/2;

    ExecuteCode();
    for (i = 0; i < Size; i++)
    {
        C[i] = F[i]* h;
        X[i][1] = Z[i] + C[i]/2;
    }
    Time[1] -= *Step/2;

    ExecuteCode();
    for (i = 0; i < Size; i++)
    {
        X[i][1] = Z[i] + (A[i] + 2*(B[i] + C[i]) + F[i]*h)/6;
    }

    Time[1] += *Step;

}

void MDiffer::Adams()
{
    Time[1] += *Step;
    ExecuteCode();
    for (int i = 0; i < Size; i++)
    {
        F[i] = A[i] + F[i]* *Step;
        if (B[i] == 0 && C[i] == 0)
        {
            X[i][1] = F[i]*3/2 - A[i]/2;
            C[i] = B[i];
            B[i] = A[i];
            A[i] = F[i];
            continue;
        }
        if (C[i] == 0)
        {
            X[i][1] = F[i]*23/12 -  A[i]*4/3 + B[i]*5/12;
            C[i] = B[i];
            B[i] = A[i];
            A[i] = F[i];
            continue;
        }

        X[i][1] = F[i]*55/24 - A[i]*59/24 + B[i]*37/24 - C[i]*3/8;
        C[i] = B[i];
        B[i] = A[i];
        A[i] = F[i];
    }
}


void MDiffer::SetVars() {}
void MDiffer::Calculate()
{
    // if (!SearchVar(pvar)) return 0;

    // RungeKutt();

    switch (Type)
    {
        case eq_euler :
            Eiler(); break;
        case eq_euler2 :
            Eiler2(); break;
        case eq_euler3 :
            Eiler3(); break;
        case eq_adams :
            Adams(); break;
        case eq_RK :
            RungeKutt(); break;

    }
    // Type = eq_RK;
}

//------------------------------------------------------------- EQCollections
void TEQsCollection::FreeItem(pointer p)
{
    delete (Equation*)p;
};

void TMethods::FreeItem(pointer p)
{
    delete (TMethod*)p;
};

//------------------------------------------------------------- Equation

Equation::Equation(TCollection* v,TEqu* e,PObject p)
{
    vars = v;
    tree = e;
    po = p;
};

Equation::~Equation()
{
    if (vars) DeleteCollection(vars);
};

//------------------------------------------------------------- Equations

Equations::Equations(PObject root)
{
    eqs = new TEQsCollection();
    deqs = new TCollection(16,16);
    ecode = new TMemoryStream(TRUE);
    EQAllVars = new TCollection(64,64);
    mtds = new TMethods(32,16);
    dmtds = new TMethods(32,16);
    VarsScan(root);
    po = root;
}

Equations::~Equations()
{
    if (EQAllVars) DeleteCollection(EQAllVars);
    if (eqs) DeleteCollection(eqs);
    if (deqs) DeleteCollection(deqs);
    if (mtds) DeleteCollection(mtds);
    if (dmtds) DeleteCollection(dmtds);
    delete ecode;
};

void Equations::VarsScan(PObject _po)
{
    for(int i=0;i<_po->_class->varcount;i++)
    {
        if (_po->_class->vars[i].GetFlags()&VF_EQVAR)
        {
            if (EQAllVars->IndexOf(_po->vars[i]) < 0)
            {
                EQAllVars->Insert(_po->GetVar(i));
            }
        }
    };

    PObject po = _po->childObjects;
    while (po) { VarsScan(po); po=po->next; }
}

void Equations::EQVarsScan(PObject po,TEqu* ptr,TCollection* vars)
{
    if (ptr->eqflag==1)
    {
        ptr->adress = (double*)po->vars[ptr->index];
        ptr->eqflag = 1;
        return;
    }
    if (ptr->code == EC_VAR)
    {
        ptr->adress = (double*)po->vars[ptr->index];
        if (ptr->eqflag==2)
        { ptr->adress++; }
        if (EQAllVars->IndexOf(po->vars[ptr->index]) >= 0)
        {
            vars->Insert(po->vars[ptr->index]);
            ptr->eqflag = 1;
        }
        else ptr->eqflag = 0;
        return;
    }
    if (ptr->code == EC_CONST) return;
    if (!ptr->items) return;
    if (ptr->items->next) EQVarsScan(po,ptr->items->next,vars);
    if (ptr->items) EQVarsScan(po,ptr->items,vars);
}

void Equations::EQAdd(PObject po) // добавить новое уравнение
{
    TEqu* ptr = po->_class->equ; // root of tree
    TEqu* tmp;
    TCollection* vars;
    char cname[156];

    while (ptr)
    {
        vars = new TCollection(16,16);
        tmp = new TEqu(ptr);

        EQVarsScan(po,tmp,vars);

        if (tmp->code == EC_EQSTRING)
        {
            tmp->items->MakeCode(ecode,(tmp->items->next->adress)+1);
            delete tmp;
            ptr=ptr->next;
            if (vars) DeleteCollection(vars);
            continue;
        }

        if (tmp->code == EC_EQDIF)
        {
            tmp->adress = (double*)po;
            deqs->Insert(tmp);
            ptr=ptr->next;
            if (vars) DeleteCollection(vars);
            continue;
        }

        if (!vars->count)
        {char buffer[256];
            //   lstrcpy(cname,"Обнаружено уравнение (");
            LoadString(HResource,IDS_str182,cname,sizeof(cname));
            LoadString(HResource,IDS_str183,buffer,sizeof(buffer));
            lstrcat(cname,po->_class->name);
            lstrcat(cname,buffer);
            _Message(cname);
            ptr=ptr->next;
            if (vars) DeleteCollection(vars);
            delete tmp;
            continue;
        }

        eqs->Insert(new Equation(vars,tmp,po));
        ptr=ptr->next;
    }
}

int Equations::LinearTest(TEqu* ptr,int& var)
{
    int r; int r1=0; int r2=0; int two = 0;

    if (ptr->code == EC_VAR)
    {
        if (ptr->eqflag) var = 1;
        return 1;
    }
    if (ptr->code == EC_CONST) return 1;

    if (ptr->items)
    {
        if (ptr->items->next) { r = LinearTest(ptr->items->next,r1); two = 1; }
        if (!r) return 0;
        if (ptr->items) r = LinearTest(ptr->items,r2);
        if (!r) return 0;
    }

    switch(ptr->code)
    {
        case EC_PLUS :
        {
            var = r1 | r2; return 1;
        }
            //  case EC_BMINUS :
            //  {
            //   var = r1 | r2; return 1;
            //  }
        case EC_MINUS :
        {
            var = r1 | r2; return 1;
        }
        case EC_UNMINUS :
        {
            var = r1 | r2; return 1;
        }
        case EC_MUL :
        {
            if (r1 && r2) return 0; else { var = r1 || r2; return 1; }
        }
        case EC_STEPEN :
        {
            if (r1 || r2) return 0; else { var = r1 || r2; return 1; }
        }
        case EC_DIV :
        {
            if (r2) return 0; else { var = r1; return 1; }
        }
        default :
        {
            if (!two && r2) return 0;
            if (two && r1 && r2) return 0;
            return 1;
        }
    }
}

void Equations::IncludeVars(TCollection* dst,TCollection* src)
{
    int notfind;
    double* lv;
    for (int i=0; i < src->count; i++)
    {
        lv = (double*)src->At(i);
        //   if (!lv) continue;
        notfind = 1;
        for (int j=0; j < dst->count; j++)
        {
            if ( ((double*)(dst->At(j))) == lv) { notfind = 0; break; }
        }
        if (notfind) dst->Insert(lv);
    }
}

int Equations::VarCompare(TCollection* key,TCollection* src)
{
    double* lv;
    int find = 0;
    for (int i=0; i < key->count; i++)
    {
        lv = (double*)key->At(i);
        if (!lv) continue;
        for (int j=0; j < src->count; j++)
        {
            if ( ((double*)src->At(j)) == lv) { find = 1; break; }
        }
        if (find) break;
    }
    return find;
};

/*
int Equations::OneVarCompare(TCollection* src,double* key)
{
 for (int j=0; j < src->count; j++)
 {
  if ( ((double*)src->At(j)) == key)  return 1;
 }
 return 0;
};
*/

int Equations::FindVarInTree(TEqu* ptr,double* var)
{
    int find = 0;
    if (ptr->code == EC_VAR)
    {
        if (ptr->adress == var) return 1;
        else return 0;
    }
    if (ptr->code == EC_CONST) return 0;
    if (!ptr->items) return 0;
    if (ptr->items->next) find += FindVarInTree(ptr->items->next,var);
    if (ptr->items) find += FindVarInTree(ptr->items,var);
    return find;
}

int Equations::TestDSystem(TCollection* strees,TCollection* svars, TEqu* tree)
{
    // по новому уравнению
    for (int i=0; i < svars->count; i++)
    {
        if (FindVarInTree(tree,((double*)svars->At(i))+1)) return 1;
    }
    // по текущим уравнениям системы
    for (i=0; i < strees->count; i++)
    {
        if (FindVarInTree((TEqu*)strees->At(i),(tree->items->next->items->next->adress)+1)) return 1;
    }
    return 0;
}

PObject Equations::FindMaxObject(PObject po1, PObject po2)
{
    if (!po1 && !po2) return 0;
    if (!po1) return po2;
    if (!po2) return po1;
    if (po1 == po2) return po1;
    return (project->object)->GetMainObject(po1,po2);
}

void Equations::Initialize()
{
    PObject eq_object;
    TCollection* svars;
    TCollection* strees;
    TCollection* objs;
    int l;
    Equation* le;
    char buffer[255];
    int Type;
    ecode->WriteWord(0);

    while (eqs->count)  // по простым уравнениям
    {
        // create new system
        svars = new TCollection(16,16);
        strees = new TCollection(16,16);
        objs = new TCollection(16,16);

        le = (Equation*)eqs->At(0);
        IncludeVars(svars,le->vars);
        strees->Insert(le->tree);
        objs->Insert(le->po);
        eqs->AtDelete(0);
        eq_object = le->po; // инициализация указателя
        delete le;
        // scan equations
        do
        {
            l = 0;
            for (int i = 0; i < eqs->count; i++)
            {
                le = (Equation*)eqs->At(i);
                if (VarCompare(svars,le->vars))
                {
                    IncludeVars(svars,le->vars);
                    strees->Insert(le->tree);
                    objs->Insert(le->po);
                    eq_object = FindMaxObject(le->po,eq_object);
                    eqs->AtDelete(i);
                    delete le;
                    i--; l = 1;
                }
            }
        } while (l);

        // linear test
        for (int i = 0; i < strees->count; i++)
        {
            l = LinearTest((TEqu*)strees->At(i),0);
            if (!l) break;
        }

        if (l) // линейные методы
        {
            Type = eq_linear1;
            if (svars->count < strees->count) Type = eq_linear2;
            if (svars->count > strees->count) Type = eq_linear3;
            mtds->Insert(new MLinears(strees,eq_object,svars,Type,objs));
        } else
        {
            //  // нелинейные методы
            if (svars->count != strees->count)
            {
                //	lstrcpy(buffer,"Не совпадает число уравнений и переменных (");

                LoadString(HResource,IDS_str184,buffer,sizeof(buffer));
                itoa(strees->count,buffer+lstrlen(buffer),10);
                lstrcat(buffer,"x");
                itoa(svars->count,buffer+lstrlen(buffer),10);
                lstrcat(buffer,")");
                _Message(buffer);
                if (svars) DeleteCollection(svars);
                for (int i = 0; i < strees->count; i++) delete (TEqu*)strees->At(i);
                if (strees) DeleteCollection(strees);
                continue;
            }
            mtds->Insert(new MNoLinears(strees,eq_object,svars,objs));
        }
    }

    double* step; // а тут собираем системы дифуравнений
    double* time;
    TEqu* tree;

    while (deqs->count)
    {
        // create new system
        svars = new TCollection(16,16);
        strees = new TCollection(16,16);
        //  objs = new TCollection(16,16);

        // разделываем и вычленяем запчасти
        tree = (TEqu*)deqs->At(0);
        eq_object = (PObject)tree->adress; // инициализация указателя
        tree->Rotate();
        if (tree->items->code != EC_PLUS)
        {  // по старому
            svars->Insert(tree->items->items->next->adress);
            step = tree->items->items->adress;
            time = 0;
        } else
        { // по новому
            svars->Insert(tree->items->items->next->items->next->adress); // переменная
            step = tree->items->items->next->items->adress; // шаг
            time = tree->items->items->adress; // время
        }

        strees->Insert(tree);
        //  objs->Insert(le->po);
        deqs->AtDelete(0);

        // scan equations
        do
        {
            l = 0;
            for (int i = 0; i < deqs->count; i++)
            {
                tree = (TEqu*)deqs->At(i);
                if (TestDSystem(strees,svars,tree))
                {
                    // разделываем и вычленяем запчасти
                    tree->Rotate();

                    if (tree->items->code != EC_PLUS)
                    {  // по старому
                        svars->Insert(tree->items->items->next->adress);
                    } else
                    {  // по новому
                        svars->Insert(tree->items->items->next->items->next->adress);
                    }

                    //	 time = 0; // !!!!!!!!!!!!!!!!! <-----------
                    strees->Insert(tree);
                    //	 objs->Insert(le->po);
                    deqs->AtDelete(i);
                    i--; l = 1;
                }
            }
        } while (l);

        // create method
        dmtds->Insert(new MDiffer(strees,eq_object,svars,step,time,0));
    }
}

void Equations::SolveSystem(double* pvar)
{
    for (int i=0; i < mtds->count; i++)
    {
        if (((TMethod*)(mtds->At(i)))->SearchVar(pvar))
        {
            ((TMethod*)(mtds->At(i)))->Calculate();
            return;
        }
    }

    for (i=0; i < dmtds->count; i++)
    {
        if (((TMethod*)(dmtds->At(i)))->SearchVar(pvar))
        {
            ((TMethod*)(dmtds->At(i)))->Calculate();
            return;
        }
    }
}

void Equations::SolveDSystem(double* pvar)
{
    for (int i=0; i < dmtds->count; i++)
    {
        if (((TMethod*)(dmtds->At(i)))->SearchVar(pvar))
        {
            ((TMethod*)(dmtds->At(i)))->Calculate();
            return;
        }
    }
}

void Equations::Calculate()
{
    TMethod* pmtd;
    for (int i=0; i < mtds->count; i++)
    {
        pmtd = (TMethod*)(mtds->At(i));
        if (pmtd->counter) { pmtd->counter = 0; continue; } // если уже решено
        if ((pmtd->pobj)->IsDisable()) { continue; } // если объект деактивирован
        pmtd->Calculate();
    }
    for (i=0; i < dmtds->count; i++)
    {
        pmtd = (TMethod*)(dmtds->At(i));
        if ((pmtd->pobj)->IsDisable()) { continue; } // если объект деактивирован
        if (pmtd->counter) { pmtd->counter = 0; continue; }
        pmtd->Calculate();
    }

    // execute ecode
    if (ecode->GetSize() > 2)
    {
        if (!VM_Execute((WORD*)ecode->base,0,0))
        { //_Message("Ошибка в ВМ при выполнении кода дополнительных присвоений !");
            _Message(IDS_str187);
            return; }
    }
}

/*
void Equations::Regeneration()
{
 for (int i=0; i < mtds->count; i++)
 {
  ((TMethod*)(mtds->At(i)))->counter = 0;
 }
 for (i=0; i < dmtds->count; i++)
 {
  ((TMethod*)(dmtds->At(i)))->counter = 0;
 }
}
*/
int Equations::GetEQTreeInfo(HWND tree,DWORD data,int DataType)
{
    TreeItemData td;
    TMethod* mtd;
    char buffer[16384];

    td.data=data;
    if(!::SendMessage(tree,TVN_GETITEMINFO,0,(long)&td))return 0;
    td.parent = data;

    switch(td.datatype)
    {
        case TREE_EQS :
        {
            td.IsLeaf = 0;
            td.datatype = TREE_EQSinfo;

            for (int i=0; i < mtds->count; i++) // типы систем уравнений
            {
                mtd = (TMethod*)(mtds->At(i));
                td.data = (DWORD)mtd;
                mtd->GetType(buffer);
                td.text = buffer;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            }

            for (i=0; i < dmtds->count; i++)
            {
                mtd = (TMethod*)(dmtds->At(i));
                td.data = (DWORD)mtd;
                mtd->GetType(buffer);
                td.text = buffer;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            }

            break;
        }
        case TREE_EQSinfo:
        {
            td.IsLeaf = 0;
            buffer[0] = 0;
            char s[256];
            td.text=s;
            td.datatype = TREE_EQSvar;
            td.data = data + 1;
            //	td.text = "Переменные системы";
            LoadString(HResource,IDS_str188,s,sizeof(s));
            ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);

            td.datatype = TREE_EQSeq;
            td.data = data + 2;
            //	td.text = "Уравнения системы";
            LoadString(HResource,IDS_str189,s,sizeof(s));
            ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);

            td.datatype = TREE_EQSmx;
            td.data = data + 3;
            //	td.text = "Матрица коэффициентов";
            LoadString(HResource,IDS_str190,s,sizeof(s));
            ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            break;
        }
        case TREE_EQSvar : // показываем переменные
        {
            td.IsLeaf = 1;
            td.datatype = 0;
            mtd = (TMethod*)(data-1);// - TREE_EQSvar;

            for (int i=0; i < mtd->GetSize(); i++)
            {
                buffer[0] = 0;
                td.data = (WORD)mtd + i + 1000; // случайное число
                mtd->GetVar(po,buffer,i);
                td.text = buffer;
                buffer[lstrlen(buffer)-1] = 0;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            }
            break;
        }
        case TREE_EQSeq : // показываем уравнения
        {
            td.IsLeaf = 1;
            td.datatype = 0;
            mtd = (TMethod*)(data-2);

            for (int i=1; i <= mtd->Eqs; i++)
            {
                buffer[0] = 0;
                td.data = (WORD)mtd+i+100; // случайное число
                mtd->GetEQ(buffer,i);
                td.text = buffer;
                buffer[lstrlen(buffer)] = 0;
                ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
            }

            break;
        }
        case TREE_EQSmx : // показываем матрицу
        {
            td.IsLeaf = 1;
            td.datatype = 0;
            mtd = (TMethod*)(data-3);
            mtd->Regeneration();
            if (mtd->GetSize() < 10)
            {
                for (int i=0; i < mtd->Eqs; i++)
                {
                    buffer[0] = 0;
                    td.data = (WORD)mtd+i + 3000; // случайное число
                    mtd->GetMX(buffer,i);
                    td.text = buffer;
                    buffer[lstrlen(buffer)-1] = 0;
                    ::SendMessage(tree,TVN_INSERTITEM,0,(long)&td);
                }
            } else // создаем матрицу
            {
                int m = matrixs->MCreate(0,1,mtd->Eqs,0,mtd->Vars,0);
                for(int i=1; i <= mtd->Vars ; i++)
                {
                    for(int j=0; j <= mtd->Eqs; j++)
                    {
                        matrixs->MPut(m,i,j,mtd->GetElement(i,j));
                    }
                }
                MatrixDialog(tree,m);
                matrixs->MDelete(m);
            }
            break;
        }
    }
    return 1;
}
