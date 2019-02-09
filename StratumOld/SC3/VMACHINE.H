/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
      vmachine.h
Description
      Virtual Machine
Author:
      Alexander Shelemekhov
*/
// FLOAT это доубле
#ifndef _VMACHINE_H_
#define _VMACHINE_H_

// достает из стека целое и помещает плавающее
//#define V_plus_L          20

//int VMTest();

int VM_Init();
int VM_Done();
int VM_Execute(WORD *vcode, void *vars, void *object);

int VM_RESET();
int VM_Error();

BOOL Push_Context();
void Pop_Context();
void Clear_Context();
void Restore_Context();

BOOL CanModifyClass(PClass _class); // возвращает 1 если можно изменить данный класс
void Vm_Halt();                     // тормозит вычисление

/* выполняет виртуальный код, который указывается параметром vcode,
  vars адреса переменных выполняемого класса
*/

/*
Example
 Class
 var
 a,x:float;

 x:=sin(a)*10.0

 code
 int code[]={PUSH_FLOAT,0,V_SIN,PUSH_FLOAT_const,10.0,V_MUL_F,_POP_FLOAT,V_END}
                                  ^
                                  разбить на 4 int
 PObject po=....
 Execute(vcode,&(po->vars[0]));


*/
#endif //_VMACHINE_H_
