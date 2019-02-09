/*
Copyright (c) 1996  TST
Project: Stratum 3.0
Module :
          Math Compiler
Author:
            Vladimir Noskov
*/

#include <stdlib.h>
//#include <fstream.h>
//#include <strstrea.h>
#include "stream.h"
#include <gwin2d3d\gwintype.h>
#include "compiler.h"
#include <windows.h>
#include "compiler.rh"
#include "equ.h"
#include "dllman.h"
#include "_cmpl.h"
#include "twindow.h"
#define FUN_FLAG "#"
#define FUN_FLAG_CHAR '#'
#define EC_EQSTRING 16383
#define EC_EQDIF 16385

#include "vmcodes.h"
#include "strings.rh"
#include <stratum\stratum.h>
//#define POSITION_TYPE "INTEGER"
//#define default_type "FLOAT"

extern HINSTANCE HResource;

char* path;

char* NewStr(char*);
//C_TYPESComp (char*, char*);

C_TYPE PutWordsSting(WORD* buf,char* str)
{
    C_TYPE i = lstrlen(str);
    lstrcpy((char*)buf,str);
    if (i & 1)
    {
        i >>= 1;
        i++;
        buf[i] = 0;
    }
    return i;
}

C_TYPE DecConstantOut(TStream* os ,char* str)
{
    double i = atof(str);
    os->Write(&i,8);
    return 1;
}

C_TYPE DecConstantOut(TStream* os ,double d)
{
    os->Write(&d,8);
    return 1;
}

C_TYPE StrConstantOut(TStream* os ,char* str,C_TYPE size)
{
    os->WriteWord(size >> 1);
    os->Write(str,size);
    return 1;
}

C_TYPE StrConstantOutEx(TStream* os ,char* str)
{
    INT16 _size;
    INT16 size=_size=lstrlen(str);
    BOOL b=0;
    if (size&1){size++;b=TRUE;}else size+=2;
    os->WriteWord(size >> 1);
    os->Write(str,_size+1);
    if(!b)
    {
        b=0;
        os->Write(&b,1);
    }
    return 1;
}

C_TYPE IConstantOut(TStream* os ,char* str)
{
    unsigned long i = atol(str);
    os->Write((unsigned char*)&i,4);
    return 1;
}

C_TYPE HexConstantOut(TStream* os ,char* str)
{
    unsigned long i = strtoul(str,0,16);
    os->Write((unsigned char*)&i,4);
    return 1;
}

C_TYPE PutWordsDec(WORD* buf,char* str)
{
    C_TYPE i = atoi(str);
    *buf = i;
    return 1;
}

C_TYPE PutWordsHex(WORD* buf,char* str)
{
    unsigned long ii = strtoul(str,0,16);

    if (lstrlen(str) <= 4)
    {
        C_TYPE i = (C_TYPE)ii;
        *buf = i;
        return 1;
    }
    else
    {
        *(DWORD*)buf = ii;
        return 2;
    }
}

//.............................................................

//Группы символов из таблицы кодов ANSII
//0-неизвестный символ
//1-буква
//2-цифра
//3- 21-!,25-%,28-(,29-), 2a-*,2b-+, 2c-,,2d--,...
//4-"
//5-/
//6-.
//7-$
//8-:
//9-возврат каретки LF
//10-&
//11-|
//12-#
//13-'
//15-+
//16--
unsigned char chtable[256] =
{//0 1 2 3  4 5 6  7  8 9 a b  c  d  e f
 0,0,0,0, 0,0,0, 0, 0,0,9,0, 0, 0, 0,0,   	// 0
 0,0,0,0, 0,0,0, 0, 0,0,0,0, 0, 0, 0,0,   	// 1
 0,3,4,12,7,3,10,13,3,3,3,15,3,	16,6,5,  		// 2
 2,2,2,2, 2,2,2, 2, 2,2,8,3, 3, 3, 3,3,   	// 3       3b->14
 3,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// 4
 1,1,1,1, 1,1,1, 1, 1,1,1,3, 3, 3, 3,1,   	// 5
 0,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// 6
 1,1,1,1, 1,1,1, 1, 1,1,1,1, 11,1, 3,0,  		// 7
 1,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// 8
 1,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// 9
 1,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// a
 0,0,0,0, 0,0,0, 0, 0,0,0,0, 0, 0, 0,0,   	// b
 1,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// c
 1,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// d
 1,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1,   	// e
 1,1,1,1, 1,1,1, 1, 1,1,1,1, 1, 1, 1,1    	// f
};

void far* pchtable = chtable;

//возвращает номер группы из таблицы кодов, в которую входит символ ch
C_TYPE DefChar(unsigned char ch)
{
    return chtable[ch];
}

/*
C_TYPE DefChar(unsigned char ch)
{
 C_TYPE x = ch;
 asm {
  mov ax,x
  lds bx,dword ptr [pchtable]
  xlat
 }
};
 */
//.............................................................  ID

ID::ID(TStream& i) : is(i)
{
    size = (C_TYPE)is.GetSize();
    buffer = new char[size];
    is.Read(buffer,size);
    counter = -1;
    str[0] = 0;
    iseof = 0;
    Status = 0;
    SConstLength = 0;
    Line = 0;
    NewLine = 0;
    LastType = 0;
}

ID::~ID()
{
    if (buffer)
        delete buffer;
}

C_TYPE ID::IsNextOptr(unsigned char ch)
{
    unsigned char* ptr = buffer + counter + 1;
    while (*ptr == ' ')
        ptr++;
    return *ptr == ch;
}

C_TYPE ID::Wait(char* key)
{
    while (!IsEOF())
        if (Read() == opnd)
            if(!lstrcmpi(str,key))
                return 1;
    return 0;
}

C_TYPE ID::IsEquation()
{
    C_TYPE IsString = 0;
    unsigned char* ptr = buffer + counter;
    while (*ptr++ != '\n' && *ptr)
    {
        if (*ptr == '"' || *ptr == '\'') IsString ^= 1;
        if (!IsString)
        {
            if (*ptr == ':'  && *(ptr+1) == '=') return 0;
            if (*ptr == '/'  && *(ptr+1) == '/') return 0;
            if (*ptr == '='  && *(ptr-1) != '=' && *(ptr-1) != '!'
                && *(ptr-1) != '>' && *(ptr-1) != '<' && *(ptr+1) != '=') return 1;
        }
    }
    return 0;
}

//поиск символа с текущей позиции и до конца строки или текста
C_TYPE ID::ScanChar(char ch)
{
    C_TYPE pos = 0;
    unsigned char* ptr = buffer + counter;
    while (*ptr++ != '\n' && *ptr)
    {
        pos++;
        if (*ptr == ch)
            return pos;
    }
    return 0;
}
#include <stdio.h>

//определение следующей лексемы в тексте
//возвращает класс определенной лексемы
C_TYPE ID::Read()
{
    char* buf = str;
    C_TYPE count = 0;
    C_TYPE k = 0;
    LastPos = counter;
    LastType = Type;

begin:
    if (++counter >= size) //проверяем дошли ли мы до конца текста
        goto eof;
    switch (DefChar(buffer[counter])){//получаем номер группы символа
        case 0:  // unknown
        { goto begin; }
        case 1:  // simbol
        {
            *buf++ = buffer[counter];
            goto contin;
        }
        case 2:  // cifra
        {
            *buf++ = buffer[counter];
            goto dconstant;
        }
        case 3: // operators
        {
            if (buffer[counter] == '-')
            {
                if (LastType==optr && DefChar(buffer[counter+1]) == 2 ) // cifra
                {
                    if (buffer[counter-1]!=')' && buffer[counter-2]!=')') // если скобка впереди
                    {
                        *buf++ = buffer[counter];
                        goto dconstant;
                    }
                }
                if (buffer[counter+1] == '>') // ->
                {
                    ++counter;
                    *buf++ = buffer[counter];
                    *buf++ = buffer[++counter];
                    *buf = 0;
                    return Type = optr;
                }
            }
            if (buffer[counter] == '!' && buffer[counter+1] != '=')
            {
                *buf++ = buffer[counter];
                *buf = 0;
                return Type = optr;
            }
            if (buffer[counter] != ')' && buffer[counter+1] == '=')
            {
                *buf++ = buffer[counter];
                *buf++ = buffer[++counter];
                *buf = 0;
                return Type = optr;
            }
            *buf++ = buffer[counter];
            *buf = 0;
            return Type = optr;
        }
        case 4:  // '"'
        { goto string; }
        case 5:  // comment '//'
        {
            if (buffer[counter+1] == '/') goto wait;
            if (buffer[counter+1] == '*') goto wait2;
            *buf++ = buffer[counter];
            *buf = 0;
            return Type = optr;
        }
        case 6: // '.'
        {
            *buf++ = buffer[counter];
            *buf = 0;
            return Type = optr;
        }
        case 7:  // $ - hconst
        {
            *buf = buffer[counter];
            goto hconstant;
        }
        case 8:  // ':'
        {
            *buf++ = buffer[counter];
            goto assign;
        }
        case 9:  //'LF'
        {
            Line++;
            if (Status)
                SCSetStatusProgressBar(1,(float)counter/(float)size*1000);
            NewLine = 1;
            goto begin;
        }
        case 10: // '&'
        {
            if (buffer[counter+1] == '&') // &&
            {
                *buf++ = buffer[counter];
                *buf++ = buffer[++counter];
                *buf = 0;
                return Type = optr;
            }
            *buf++ = buffer[counter];
            *buf = 0;
            return Type = optr;
        }
        case 11: // '|'
        {
            if (buffer[counter+1] == '|') // ||
            {
                *buf++ = buffer[counter];
                *buf++ = buffer[++counter];
                *buf = 0;
                return Type = optr;
            }
            *buf++ = buffer[counter];
            *buf = 0;
            return Type = optr;
        }
        case 12:  // # - integer const
        {
            *buf = buffer[counter];
            goto iconstant;
        }
        case 13:  // '''  строковая константа 2
        { goto string2; }
            //  case 14:  // ';' разделитель
            //  {
            //   goto begin;
            //   NewLine = 1;
            //  }
        case 15: // '+'
        {
            if (buffer[counter+1] == '+') // инкремент ++
            {
                *buf++ = buffer[counter];
                *buf++ = buffer[++counter];
                *buf = 0;
                return Type = optr;
            }
            *buf++ = buffer[counter];
            *buf = 0;
            return Type = optr;
        }
        case 16: // '-'
        {
            if (buffer[counter+1] == '-') // декремент --
            {
                *buf++ = buffer[counter];
                *buf++ = buffer[++counter];
                *buf = 0;
                return Type = optr;
            }
            *buf++ = buffer[counter];
            *buf = 0;
            return Type = optr;
        }

        default : goto begin;
    }

    //-----------------------------------------------
assign: //нашли двоеточие
    if (++counter >= size)
    {
        *buf = 0;
        return Type = optr;
    }
    if(buffer[counter] == '=') //нашли оператор присвоения :=
    {
        *buf++ = buffer[counter];
        *buf = 0;
        return Type = optr;
    }
    else
    {
        counter--;
        *buf = 0;
        return Type = optr;
    }
    //-----------------------------------------------

wait:  //комментарий до конца строки //...
    if (++counter >= size) goto eof;
    if (buffer[counter] == 0x0a) //если дошли до конца строки
    {
        Line++;
        NewLine = 1;
        goto begin;
    }
    else
        goto wait;

wait2: // конкретный комментарий  /*...*/
    if (++counter >= size) goto eof;
    if (buffer[counter] == '*' && buffer[counter+1] == '/')
    {
        if (++counter >= size) goto eof;
        goto begin;
    }
    if (buffer[counter] == 0x0a)
    {
        Line++;
        NewLine = 1;
    }
    goto wait2;

    //-----------------------------------------------
contin: //выделяем строковую лексему (последовательность букв и цифр)
    if (++counter >= size)//если дошли до конца текста, то выходим
    {
        *buf = 0;
        return Type = opnd;
    }
    k = DefChar(buffer[counter]);//получаем группу символа
    if (k == 1 || k == 2) //если это буква или цифра, то повторяем проход
    {
        *buf++ = buffer[counter];
        goto contin;
    }
    else//если это не буква и не цифра, то выходим
    {
        counter--;
        *buf = 0;
        return Type = opnd;
    }

    //-----------------------------------------------
dconstant: //выделяем цифровую лексему
    if (++counter >= size)
    {
        *buf = 0;
        return Type = dconst;
    }
    //если среди цифр есть буква e или E, то переходим определению
    //вещественной лексемы с выделеной мантиссой и порядком
    if ( buffer[counter] == 0x45 || buffer[counter] == 0x65 )
    {
        *buf++ = buffer[counter];
        goto fconstant4;
    }
    switch (DefChar(buffer[counter])){
        case 6 : // если среди цифр есть точка, то переходим к определнию лексемы с плавающей точкой
        {
            *buf++ = buffer[counter];
            goto fconstant;
        }
        case 2 : //если символ опять цифра, то повторяем цикл
        {
            *buf++ = buffer[counter];
            goto dconstant;
        }
        default:
        {
            counter--;
            *buf = 0;
            return Type = dconst;
        }
    }

    //-----------------------------------------------
iconstant: //выделяем целочисленную лексему #...
    if (++counter >= size)
    {
        *buf = 0;
        return Type = dconst;
    }
    if ( buffer[counter] == 0x45 || buffer[counter] == 0x65 )   // e E
    {
        *buf++ = buffer[counter];
        goto fconstant2;
    }
    switch (DefChar(buffer[counter]))
    {
        case 6 : // .
        {
            *buf++ = buffer[counter];
            goto fconstant;
        }
        case 2 : // number
        {
            *buf++ = buffer[counter];
            goto iconstant;
        }
        default :
        {
            counter--;
            *buf = 0;
            return Type = iconst;
        }
    }

    //-----------------------------------------------
hconstant: //выделение шестнадцатеричной лексему $...
    if (++counter >= size)
    {
        *buf = 0;
        return Type = hconst;
    }
    if ((buffer[counter]>=0x30 && buffer[counter]<=0x39) ||  //0..9
        (buffer[counter]>=0x41 && buffer[counter]<=0x46) ||  //A..F
        (buffer[counter]>=0x61 && buffer[counter]<=0x66))    //a..f
    {
        *buf++ = buffer[counter];
    }
    else
    {
        counter--;
        *++buf = 0;
        return Type = optr;
    }

hconstant2:
    if (++counter >= size)
    {
        *buf = 0;
        return Type = hconst;
    }
    if ((buffer[counter]>=0x30 && buffer[counter]<=0x39) ||
        (buffer[counter]>=0x41 && buffer[counter]<=0x46) ||
        (buffer[counter]>=0x61 && buffer[counter]<=0x66))
    {
        *buf++ = buffer[counter];
        goto hconstant2;
    }
    else
    {
        counter--;
        *buf = 0;
        return Type = hconst;
    }

    //-----------------------------------------------
fconstant: //выделяем вещественную лексему
    if (++counter >= size)
    {
        *buf = 0;
        return Type = fconst;
    }
    if (buffer[counter]>=0x30 && buffer[counter]<=0x39) // number
    {
        *buf++ = buffer[counter];
        goto fconstant2;
    }
    else
    {
        counter -= 2;
        *buf-- = 0;
        *buf-- = 0;
        return Type = dconst;
    }

    //-----------------------------------------------
fconstant2: //выделяем вещественную лексему с мантиссой и порядком
    if (++counter >= size)
    {
        *buf = 0;
        return Type = fconst;
    }
    if (  buffer[counter] == 0x45 || buffer[counter] == 0x65) // E e
    {
        *buf++ = buffer[counter];
        goto fconstant3;
    }
    if ( buffer[counter]>=0x30 && buffer[counter]<=0x39) //числа
    {
        *buf++ = buffer[counter];
        goto fconstant2;
    }
    else
    {
        counter--;
        *buf = 0;
        return Type = fconst;
    }

    //-----------------------------------------------
fconstant3:
    if (++counter >= size)
    {
        *buf = 0;
        return Type = fconst;
    }
    if (buffer[counter] == 0x2d || buffer[counter] == 0x2b)   // - +
    {
        *buf++ = buffer[counter];
        goto fconstant4;
    }
    if ( buffer[counter]>=0x30 && buffer[counter]<=0x39) //числа
    {
        *buf++ = buffer[counter];
        goto fconstant4;
    }
    else
    {
        counter--;
        *buf = 0;
        return Type = fconst;
    }

    //-----------------------------------------------
fconstant4:
    if (++counter >= size)
    {
        *buf = 0;
        return Type = fconst;
    }
    if ( buffer[counter]>=0x30 && buffer[counter]<=0x39) //числа
    {
        *buf++ = buffer[counter];
        goto fconstant4;
    }
    else
    {
        counter--;
        *buf = 0;
        return Type = fconst;
    }

    //-----------------------------------------------
string: //выделяем строковую лексему в ковычках "
    if (counter++ >= size)
    {
        *buf = 0;
        return Type = sconst;
    }
    if ( buffer[counter] != 0x22 ) // "
    {
        *buf++ = buffer[counter];
        if (++count == MAX_STRING) return Type = sconst;
        else goto string;
    }
    else
    {
        *buf++ = 0;
        SConstLength = count+1;
        if (SConstLength & 1) { SConstLength++; *buf = 0; }
        return Type = sconst;
    }

    //-----------------------------------------------
string2: //выделяем строковую лексему в апострофах '
    if (counter++ >= size)
    {
        *buf = 0;
        return Type = sconst;
    }
    if ( buffer[counter] != 0x27 ) // '
    {
        *buf++ = buffer[counter];
        if (++count == MAX_STRING) return Type = sconst;
        else goto string2;
    }
    else
    {
        *buf++ = 0;
        SConstLength = count+1;
        if (SConstLength & 1) { SConstLength++; *buf = 0; }
        return Type = sconst;
    }

    //-----------------------------------------------
eof:
    iseof = 1;
    return Type = eof;
};


//------------------------------------------------------------- TMacro
//Формирует константу (имя - число)
class TMacro
{
public:
    char* Name;
    double Number;

    TMacro(char* str,double n) { Name = NewStr(str); Number = n;};
    //TMacro(char* str) { Name = NewStr(str);};
    TMacro(ID&);
    ~TMacro() { delete Name; };
    //ReadMacro(ID);
};


TMacro::TMacro(ID& id)
{
    Name = 0;
    Number = 0;

    if (id.Type != opnd)
        MSGBOX(0,MAKEINTRESOURCE(IDS_CMP1),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);

    Name = NewStr(id.str);
    id.Read();
    switch (id.Type){
        case dconst :
        case fconst :
        {
            Number = atof(id.str);
            break;
        }
        case hconst :
        {
            Number = strtoul(id.str,0,16);
            break;
        }
        default:
        { MSGBOX(0,MAKEINTRESOURCE(IDS_CMP1),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND); return; }
    }
}

//------------------------------------------------------------------------------
//хранит список констант
class TMacroCollection : public TSortedCollection
{
public:
    //TMacroCollection(WORD asize = 64,WORD adelta = 32) : TSortedCollection(asize,adelta){};
    TMacroCollection(char *file,C_TYPE asize = 64,C_TYPE adelta = 32);
    //C_TYPE ReadFunDefine(char*);
    virtual int Compare(pointer key1,pointer key2) { return lstrcmpi(((TMacro*)key1)->Name,((TMacro*)key2)->Name); };
    virtual void FreeItem(pointer );
    C_TYPE Search(char*,C_TYPE&);
};

void TMacroCollection::FreeItem(pointer p)
{
    delete (TMacro*)p;
};

TMacroCollection::TMacroCollection(char *file,C_TYPE asize,C_TYPE adelta) : TSortedCollection(asize,adelta)
{
    TMacro* mr;
    //TDOSStream tpl("constant.tpl",TDOSStream::stOpenRead);
    TMemoryStream tpl(1,file);
    if (tpl.status)
    { MSGBOX(0,MAKEINTRESOURCE(IDS_CMP2),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND); return; }

    ID id(tpl);
    //формируем список констант (имя - число)
    while (1)
    {
        id.Read();
        if (id.IsEOF())
            return;

        mr = new TMacro(id);
        if (Insert(mr)<0)
        {
            char s[256];
            wsprintf(s,"Duplicated constant ( %s ) in constant.tpl!",mr->Name);
            MSGBOX(0,s,_MSG_ERROR,MB_OK|MB_TASKMODAL);
            delete mr;
        }
    }
}

C_TYPE TMacroCollection::Search(char* key,C_TYPE& index)
{
    C_TYPE c;
    for (C_TYPE i=0;i<count;i++)
    {
        c=lstrcmpi(key,((TMacro*)items[i])->Name);
        if (c<=0){index=i;return c==0;  }
    }
    index=count;
    return 0;
};

//------------------------------------------------------------- TOperator
class TOperator
{
public:
    char* Name; //имя функции/оператора
    WORD out;  //номер элемента в списке виртуальных команд (commands в wmachine.cpp)
    C_TYPE imp;  //начальный приоритет оператора в диапазоне
    C_TYPE imp2; //конечный приоритет оператора в диапазоне
    C_TYPE arg; //хранит количество аргументов оператора
    C_TYPE arg2; //хранит количество необязательных аргументов оператора

    PClassListInfo ret; //ссылка на тип возвращаемого значения (TClassListInfo)
    INT32 pointers;     //хранит номера аргументов оператора, являющихся ссылками (&...)

    C_TYPE IsOut;
    C_TYPE IsIndex;
    C_TYPE IsName;

    PClassListInfo* pType;  //хранит информацию о типах аргументов оператора
    PClassListInfo* pType2; //хранит информацию о типах необязательных аргументов оператора

    TOperator();
    TOperator(ID&);
    ~TOperator();

    C_TYPE Out(TStream*,C_TYPE = 0);

    inline C_TYPE GetImp() { return imp; };
    inline C_TYPE GetImp2() { return imp2; };

    void ChangeName(char*);
};

TOperator::TOperator()
{
    Name = 0;
    out = 0; pointers = 0;
    IsOut = 0; IsIndex = 0; IsName = 0;
    pType = 0; pType2 = 0;
    imp = 0; imp2 = 0; arg = 0; arg2 = 0; ret = 0;
};

TOperator::~TOperator()
{
    if (Name) delete Name;
    if (pType) delete pType;
    if (pType2) delete pType2;
};

void TOperator::ChangeName(char* nm)
{
    if (Name) delete Name;
    Name = NewStr(nm);
}

TOperator::TOperator(ID& id)
{
    Name = 0;
    out = 0; 		pointers = 0;
    IsOut = 0; 	IsIndex = 0; 	IsName = 0;
    imp = 0; 		imp2 = 0;
    arg = 0; 		arg2 = 0;
    ret = 0;
    pType = 0; 	pType2 = 0;

    PClassListInfo pcl;
    PClassListInfo buf[64];
    C_TYPE iii = 0,j;

    //определяем имя оператора
    if (id.Read() != sconst)
        MSGBOX(0,MAKEINTRESOURCE(IDS_CMP3),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);

    Name = NewStr(id.str);
    id.Read();
    if (lstrcmpi(id.str,"imp")==0)
    {
        if (id.Read() != dconst)
        {
            MSGBOX(0,MAKEINTRESOURCE(IDS_CMP4),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);
            return;
        }
        imp = atoi(id.str);
        imp2 = imp;
        id.Read();

        if (id.str[0] == ',')
        {
            if (id.Read() != dconst)
            {
                MSGBOX(0,MAKEINTRESOURCE(IDS_CMP4),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);
                return;
            }

            imp2 = atoi(id.str);
            id.Read();
        }
    }

    if (lstrcmpi(id.str,"arg")==0)
    {
        //определяем аргументы оператора/функции
        do
        {
            if (id.Read() == optr && id.str[0] == '[')
            {
                //нашли список необязательных аргументов оператора
                pType = new PClassListInfo[iii];
                for (j = 0; j < iii; j++)
                    pType[j] = buf[j];
                arg = iii;
                iii = 0;
                goto asw1;
            }
            if (id.Type == optr && id.str[0] == '&') // ссылка !
            {
                pointers |= 1 << iii;
                id.Read();
            }
            //если следующая лексема не строковая ("..."),т.е. не название типа, то возвращаем ошибку
            if(id.Type != sconst)
            {
                MSGBOX(0,MAKEINTRESOURCE(IDS_CMP5),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);
                return;
            }
            //получаем информацию о типе по его имени
            if((pcl=GetClassListInfo(id.str)) != 0)
            {
                buf[iii++] = pcl;
            }
            else
            {
                char s[256];
                wsprintf(s,"Compiler temlate file syntax error : Unknown type: %s detected !",id.str);
                MSGBOX(0,s,_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);
                return;
            }
        }
        while (id.Read() == optr && id.str[0] == ',');

        pType = new PClassListInfo[iii];
        for (j = 0; j < iii; j++)
            pType[j] = buf[j];
        arg = iii;
    }
    goto asw2;

asw1:
    //определяем необязательные аргументы (типы и их количество)
    do
    {
        if (id.Read() == optr && id.str[0] == '&') // ссылка !
        {
            pointers|=1<<iii;
            id.Read();
        }
        if (id.Type != sconst)
        { MSGBOX(0,MAKEINTRESOURCE(IDS_CMP5),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND); return; }

        if ( (pcl=GetClassListInfo(id.str)) != 0)
        {
            buf[iii++] = pcl;
        }
        else
        {
            char s[256];
            wsprintf(s,"Compiler temlate file syntax error : Unknown type: %s detected !",id.str);
            MSGBOX(0,s,_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND); return;
        }
    }
    while (id.Read() == optr && id.str[0] == ',');
    if (id.str[0] != ']')
    { MSGBOX(0,MAKEINTRESOURCE(IDS_CMP6),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND); return; }

    id.Read();
    pType2 = new PClassListInfo[iii];
    for (j = 0; j < iii; j++)
        pType2[j] = buf[j];
    arg2 = iii;

asw2:
    //определяем тип возвращаемого значения
    if (lstrcmpi(id.str,"ret")==0)
    {
        if (id.Read() != sconst)
        { MSGBOX(0,MAKEINTRESOURCE(IDS_CMP5),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND); return; }

        if ( (pcl=GetClassListInfo(id.str)) != 0)
            ret = pcl;
        else
        { MSGBOX(0,MAKEINTRESOURCE(IDS_CMP7),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND); return; }

        id.Read();
    }
    if (lstrcmpi(id.str,"out")==0)
    {
        if (id.Read() == dconst)
        {
            out = atoi(id.str);
            IsOut = 1;
            id.Read();
            if (*id.str != ',')
                return;

            id.Read();
        }

        if (id.Type != optr)
            MSGBOX(0,MAKEINTRESOURCE(IDS_CMP8),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);

        if (id.str[0] == '$')
        {
            id.Read();
            if (lstrcmpi(id.str,"name")==0)  { IsName = 1; id.Read(); return;}
            if (lstrcmpi(id.str,"index")==0)  { IsIndex = 1; id.Read(); return;}
            MSGBOX(0,MAKEINTRESOURCE(IDS_CMP8),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);
        }
    }
    if (id.Type != opnd)
        MSGBOX(0,MAKEINTRESOURCE(IDS_CMP8),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);
}

//в поток записываем номер процедуры, обрабатывающей оператор/функцию, или
//переданный индекс, если оператор/функция имеют флаг IsIndex, или имя оператора
C_TYPE TOperator::Out(TStream* os,C_TYPE ix)
{
    if (IsOut)
        os->WriteWord(out);

    if (IsIndex)
    {
        os->WriteWord(ix);
        return 1;
    }
    if (IsName)
        os->Write(Name,lstrlen(Name));

    return 1;
}

//.............................................................   TOperatorCollection
//формирует список операторов
class TOperatorCollection : public TSortedCollection
{
public:
    TOperatorCollection(WORD asize = 64,WORD adelta = 32) :
        TSortedCollection(asize,adelta)
    {}

    TOperatorCollection(ID&,WORD asize = 64,WORD adelta = 32);

    C_TYPE ReadFunDefine(char*);

    virtual int Compare(pointer key1,pointer key2);
    virtual void FreeItem(pointer );

    C_TYPE NSearch(pointer,C_TYPE&,C_TYPE=0);

    C_TYPE FSearch(C_TYPE&, char*,
                   PClassListInfo*, C_TYPE,
                   C_TYPE=0,
                   PClassListInfo* parg2=NULL,C_TYPE arg2=-1);

    C_TYPE SpecFSearch(char*,PClassListInfo,C_TYPE&,C_TYPE=0);
    C_TYPE SSearch(char*,C_TYPE&,C_TYPE=0);

    BOOL FindKeyWorld(char*,C_TYPE &index);
};

BOOL TOperatorCollection::FindKeyWorld(char*key,C_TYPE&Index)
{
    C_TYPE  L, H, I;
    C_TYPE C;
    BOOL ret=FALSE;
    L = 0;
    H = (INT16)(count - 1);
    while (L <= H){
        I = (INT16)((L + H) >> 1);

        //	 C = Compare(items[I], Key);
        C=lstrcmpi(((TOperator*)(items[I]))->Name,key);

        if (C < 0){ L = (INT16)(I + 1);} else
        {
            H =(INT16)( I - 1);
            if (C == 0)
            {
                ret = TRUE;
                if (!Duplicates) L = I;
            }
        }
    };
    Index = L;
    return ret;
};

TOperatorCollection::TOperatorCollection(ID& id,WORD asize,WORD adelta)
    : TSortedCollection(asize,adelta)
{
    id.Read();
    //формируем список функций из подключаемых файлов с шаблонами
    while (lstrcmpi(id.str,"include")==0)
    {
        id.Read();
        lstrcat(id.str,".tpl");
        ReadFunDefine(id.str);
        id.Read();
    }
    //формируем список функций/операторов    (<name> [imp] [arg] [ret] [out])
    while (lstrcmpi(id.str,"name")==0)
    {
        TOperator* op = new TOperator(id); //создаем элемент списка
        if(Insert(op)<0) //добавляем его к списку
        {
            char s[256];
            wsprintf(s,"Duplicated operator (%s) in compiler.tpl!",id.str);
            MSGBOX(0,s,_MSG_ERROR,MB_OK|MB_TASKMODAL);
            delete op;
        }

        if (id.IsEOF())
            return;
    }
}

C_TYPE TOperatorCollection::ReadFunDefine(char* filename)
{
    char buf[255];
    // TOperator* op;
    // TDOSStream tpl(filename,TDOSStream::stOpenRead);
    lstrcpy(buf,path);
    lstrcat(buf,filename);
    lstrcpy(filename,buf);
    TMemoryStream tpl(1,filename);
    if (tpl.status)
    {
        lstrcpy(buf,"Can't open compiler template file: ");
        lstrcat(buf,filename);
        MSGBOX(0,buf,_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);
        return 0;
    }
    ID id(tpl);
    id.Read();
    while (!id.IsEOF())
    {
        if (lstrcmpi(id.str,"name")==0)
        {
            TOperator*op=new TOperator(id);
            if (Insert(op)<0)
            {
                lstrcpy(buf,"Error in file: ");
                lstrcat(buf,filename);
                MSGBOX(0,buf,_MSG_ERROR,MB_OK|MB_TASKMODAL);
                delete op;
                return 0;
            }
        }
        else
        {
            lstrcpy(buf,"Error in file: ");
            lstrcat(buf,filename);
            MSGBOX(0,buf,_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);
            return 0;
        }
    }
    return 1;
}

//ищем индекс оператора с именем, как у оператора key
C_TYPE TOperatorCollection::NSearch(pointer key,C_TYPE &index,C_TYPE beg_index)
{
    C_TYPE c;
    for (C_TYPE i=beg_index;i<count;i++)
    {
        c=lstrcmpi(((TOperator*)key)->Name,((TOperator*)items[i])->Name);
        //if (c<=0) { index=i; return c==0; }
        if (c==0) { index=i; return 1; }
    }
    index=count;
    return 0;
}

//ищем индекс оператора с именем key
C_TYPE TOperatorCollection::SSearch(char* key,C_TYPE& index,C_TYPE beg_index)
{
    C_TYPE c;
    for (C_TYPE i=beg_index;i<count;i++)
    {
        c=lstrcmpi(key,((TOperator*)items[i])->Name);
        //if (c<=0) {index=i; return c==0; }
        if (c==0)
        {
            index=i;
            return 1;
        }
    }
    index=count;
    return 0;
};

//ищем индекс оператора с именем name и с типами аргументов, совпадающими с parg и parg2.
C_TYPE TOperatorCollection::FSearch(C_TYPE &index, char* name,
                                    PClassListInfo* parg, C_TYPE arg,
                                    C_TYPE beg_index,
                                    PClassListInfo* parg2,C_TYPE arg2)
{
    C_TYPE f;
    for (C_TYPE i=beg_index;i<count;i++)
    {
        if (lstrcmpi(((TOperator*)items[i])->Name,name)==0 && ((TOperator*)items[i])->arg==arg)
        {
            f = 0;
            for (C_TYPE j = 0; j < arg; j++)
            {
                if (parg[arg-(j+1)] == ((TOperator*)items[i])->pType[j] ) f++;
            }
            if (f==arg)
            {
                if(arg2>=0 && parg2)
                {
                    if(((TOperator*)items[i])->arg2 == arg2)
                    {
                        f = 0;
                        for (C_TYPE j = 0; j < arg2; j++)
                        {
                            if (parg2[arg2-(j+1)] == ((TOperator*)items[i])->pType2[j] ) f++;
                        }
                        if (f==arg2)
                        {index = i; return 1;}
                    }
                }
                else
                {index = i; return 1;}
            }
        }
    }
    index=count;
    return 0;
};

//ищем индекс оператора с именем name и типом возвращаемого значения как у _ret.
C_TYPE TOperatorCollection::SpecFSearch(char* name,PClassListInfo _ret,C_TYPE &index,C_TYPE beg_index)
{
    C_TYPE c;
    for (C_TYPE i=beg_index;i<count;i++)
    {
        c=lstrcmpi(((TOperator*)items[i])->Name,name);
        if (c == 0 && ((TOperator*)items[i])->ret == _ret)
        {
            index=i;
            return 1;
        }
    }
    index=count;
    return 0;
};


void TOperatorCollection::FreeItem(pointer p)
{
    delete (TOperator*)p;
};

//сравниваем два оператора по имени, типам аргументов и типу возвращаемого значения
int TOperatorCollection::Compare(pointer key1,pointer key2)
{
    C_TYPE i = lstrcmpi(((TOperator*)key1)->Name,((TOperator*)key2)->Name);
    //если имена не совпадают, то возвращаем ошибку
    if (i != 0)
        return i;

    //если типы возвращаемого значения не совпадают, то ошибка
    if (((TOperator*)key1)->ret != ((TOperator*)key2)->ret)
    {
        if (((TOperator*)key1)->ret > ((TOperator*)key2)->ret)
            return 1;
        else
            return -1;
    }
    //если количество основных аргументов не совпадают, то возвращаем ошибку
    if (((TOperator*)key1)->arg != ((TOperator*)key2)->arg)
    {
        if (((TOperator*)key1)->arg > ((TOperator*)key2)->arg)
            return 1;
        else
            return -1;
    }
    //если типы основных аргументов не совпадают, то ошибка
    for (i = 0; i < ((TOperator*)key1)->arg; i++)
    {
        if (((TOperator*)key1)->pType[i] != ((TOperator*)key2)->pType[i])
        {
            if (((TOperator*)key1)->pType[i] > ((TOperator*)key2)->pType[i])
                return 1;
            else
                return -1;
        }
    }
    //если количество дополнительных аргументов не совпадают, то возвращаем ошибку
    if (((TOperator*)key1)->arg2 != ((TOperator*)key2)->arg2)
    {
        if (((TOperator*)key1)->arg2 > ((TOperator*)key2)->arg2)
            return 1;
        else
            return -1;
    }
    //если типы дополнительных аргументов не совпадают, то ошибка
    for (i = 0; i < ((TOperator*)key1)->arg2; i++)
    {
        if (((TOperator*)key1)->pType2[i] != ((TOperator*)key2)->pType2[i])
        {
            if (((TOperator*)key1)->pType2[i] > ((TOperator*)key2)->pType2[i])
                return 1;
            else
                return -1;
        }
    }
    return 0;
};

//.............................................................
//#include "dllmgr.cpp"

//формируем список операторов и функций
MCompiler::MCompiler(char* filename)
{
    char s[256];
    //определяем имя файл с шаблонами операторов и функций
    if (filename == 0)
    {
        filename = "compiler.tpl";
        path=NULL;
    }
    else
    {
        GetPath(filename, s);
        AddSlash(s);
        path=s;
    }
    Epilog = 0;
    Err_x1 = Err_x2 = 0;
    ErrText[0] = 0;
    Failed = 0;
    TDOSStream tpl(filename,TDOSStream::stOpenRead);
    if (tpl.status)
    {
        MSGBOX(0,"Can't open compiler template file...",_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);
        return;
    }
    ID id(tpl); //объект для разбора файла на лексемы
    id.Read(); //выполняет выделение лексемы из текста
    if (!lstrcmpi(id.str,"EPILOG"))
    {
        id.Read();
        if (id.IsEOF())
        {
            MSGBOX(0,MAKEINTRESOURCE(IDS_CMP9),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);
            return;
        }
        Epilog = new TOperator(id);
    }
    else
    {
        MSGBOX(0,MAKEINTRESOURCE(IDS_CMP9),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);
        return;
    }

    if (id.IsEOF())
    {
        MSGBOX(0,MAKEINTRESOURCE(IDS_CMP9),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);
        return;
    }

    //Формируем список операторов
    if (!lstrcmpi(id.str,"OPERATORS"))
        Operators = new  TOperatorCollection(id);
    else
    {
        Failed = 1;
        return;
    }
    if (id.IsEOF())
    {
        MSGBOX(0,MAKEINTRESOURCE(IDS_CMP9),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);
        return;
    }
    //формируем список функций
    if (!lstrcmpi(id.str,"FUNCTIONS"))
        Functions = new TOperatorCollection(id);
    else
    {
        Failed = 1;
        return;
    }
    //записывает индекс оператора "#" в переменную FunFlag_ix, если его нет, то ошибка
    if (!Operators->SSearch(FUN_FLAG,FunFlag_ix)){MSGBOX(0,MAKEINTRESOURCE(IDS_CMP10),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);return;}
    //записывает индекс оператора "un_minus" в переменную UNARY_ix, если его нет, то ошибка
    if (!Operators->SSearch("un_minus",UNARY_ix)){MSGBOX(0,MAKEINTRESOURCE(IDS_CMP11),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);return;}
    //if (!Operators->SSearch("un_not",UNARYN_ix)) { MSGBOX(0,MAKEINTRESOURCE(IDS_CMP11),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND); return; }
    if (!Functions->SSearch("VFunction",VFunction_ix)) { MSGBOX(0,MAKEINTRESOURCE(IDS_CMP12),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND); return; }
    if (!Functions->SSearch("DLLFunction",DLLFunction_ix)) { MSGBOX(0,MAKEINTRESOURCE(IDS_CMP13),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND); return; }
    if (!Functions->SSearch("GetElement",GetElement_ix)) { MSGBOX(0,MAKEINTRESOURCE(IDS_CMP14),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND); return; }
    if (!Functions->SSearch("SetElement",SetElement_ix)) { MSGBOX(0,MAKEINTRESOURCE(IDS_CMP15),_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND); return; }

    //формируем список констант
    {
        char file[256];
        lstrcpy(file,path);
        lstrcat(file,"constant.tpl");
        Constants = new TMacroCollection(file);
    }
}

MCompiler::~MCompiler()
{
    if (Operators) DeleteCollection(Operators);
    if (Functions) DeleteCollection(Functions);
    if (Constants) DeleteCollection(Constants);
    if (Epilog) delete Epilog;
}

//.............................................................

C_TYPE TVarStack::Push(PClassListInfo k,INT16 pos)
{
    if (ix != MAX_STACK)
    {
        buf[ix++] = k;
        code[ix] = pos;
        return 1;
    }
    else
        return 0;
}

C_TYPE TVarStack::SetPtr(TStream* os)
{
    if (code[ix] == -1)
        return 0;

    INT16 p = os->GetPos();
    os->Seek(code[ix]);

    INT16 k = os->ReadWord();
    os->Seek(code[ix]);

    switch (k)
    {
        case PUSH_FLOAT :
        case PUSH_STRING :
        case vmPUSH_LONG :
        {
            os->WriteWord(PUSHPTR);
            break;
        }
        case _PUSH_STRING :
        case _PUSH_FLOAT :
        case vm_PUSH_LONG :
        {
            os->WriteWord(PUSHPTRNEW);
            break;
        }

        default :
            return 0;
    }
    os->Seek(p);
    return 1;
}

PClassListInfo TVarStack::Pop(int& pos)
{
    if (ix)
    {
        pos=code[ix];
        return buf[--ix];
    }
    else
    {
        pos=code[0];
        return buf[0];
    }
}

//добавляет переменную по имени и типу в список pvardata и в стек varstack, если нет ошибок
TVarInfo* MCompiler::InsertVar(C_TYPE& ix, char* VarName, char* type)
{
    if (type)
        ix = pvardata->Insert(VarName,type);
    else
        ix = pvardata->Insert(VarName,0);

    //если тип неизвестен, то ошибка
    if (ix == -1) { SetError(ERR_UNTYPE); return 0; }
    //если переменная с таким именем уже есть, но с другим типом, то ошибка
    if (ix == -2)
    {
        TVarInfo * tv = pvardata->Search(VarName,&ix);
        if ( tv == 0 ) { SetError(ERR_INTERNAL); return 0; }

        char errstr[MAX_STRING];
        lstrcpy(errstr,FirstString);
        lstrcat(errstr," : ");
        lstrcat(errstr,tv->type->GetClassName());
        SetError(ERR_VAREXIST,errstr);
        return 0;
    }
    //ищем описание на эту переменную в списке переменных
    TVarInfo * tv = pvardata->Search(VarName,&ix);
    //добавляем тип переменной и ее позицию в outcode в стек переменных
    VarStack.Push(tv->type,outcode->GetPos());
    return tv;
}


C_TYPE MCompiler::ConstantOut(char* Type)
{
    C_TYPE ix;
    TClassListInfo * tc = GetClassListInfo(Type);
    if (!tc) { SetError(ERR_UNTYPE,Type); return 0; }
    VarStack.Push(tc);
    if (EQ_mode) // EQUATION !!!
    {
        TEqu* pequ = new TEqu;
        pequ->code = EC_CONST;
        pequ->value = atof(id->str);
        EQStack.Push(pequ);
        return 1;
    }
    if (Functions->SpecFSearch("push_cst",tc,ix))
        ((TOperator*)Functions->At(ix))->Out(outcode,0);
    else
    { SetError(ERR_NSTYPE,tc->GetClassName()); return 0; }
    return 1;
}

//вставляет переменную в список переменных
//проверяет наличие знака ~ перед переменной
C_TYPE MCompiler::VarOut(char* VarName)
{
    C_TYPE fix;
    C_TYPE ix;
    TVarInfo * tv = InsertVar(ix,VarName);
    if (!tv) return 0;
    char nameoptr[32];
    if (EQ_mode) // EQUATION !!!
    {
        TEqu* pequ = new TEqu;
        pequ->code = EC_VAR;
        if (New_modificator)
        { pequ->eqflag = 2; New_modificator = 0; }
        else pequ->eqflag = 0;
        pequ->index = ix;
        EQStack.Push(pequ);
        return 1;
    }
    //если перед переменной стоит знак ~
    if (New_modificator)
    {
        if (OnePhase_mode)  //если компилируется имидж-функция
            lstrcpy(nameoptr,"push");
        else
            lstrcpy(nameoptr,"push_new");
        New_modificator = 0;
    }
    else lstrcpy(nameoptr,"push");
    if (Functions->SpecFSearch(nameoptr,tv->type,fix))
    {
        ((TOperator*)Functions->At(fix))->Out(outcode,ix);
    }
    else
    { SetError(ERR_NSTYPE,tv->type->GetClassName()); return 0; }
    return 1;
}

//проверяет типы оператора,
//добавляет переменную перед оператором присваивания
//добавляет номер оператора в outcode
C_TYPE MCompiler::OperatorOut(TOperator* op,C_TYPE ix)
{
    PClassListInfo buf[64];
    C_TYPE PopVar_ix;
    TEqu* pequ;
    //если это оператор присваивания, то добавляем переменную перед ним в список переменных
    //firststring - имя предыдущего операнда, если он не был декларирован,
    //то VarStack.GetTop()->GetClassName() вернет 0
    if (op->Name[0] == ':' && !EQ_mode)
    {
        if (VarStack.Empty())
        { SetError(ERR_SYNTAX,0,1);  return 0; }

        TVarInfo * tv = InsertVar(PopVar_ix, FirstString, VarStack.GetTop()->GetClassName());
        if (!tv)
            return 0;

        tv->SetFlags(VF_LEFT);
    }
    if (!lstrcmpi(op->Name, "++"))
    {
        //if (VarStack.Empty()) { SetError(ERR_SYNTAX,0,1);  return 0; }
        TVarInfo * tv = InsertVar(PopVar_ix,FirstString,"FLOAT");
        if (!tv) return 0;
        tv->SetFlags(VF_LEFT);
    }
    //проверяем типы аргументов оператора
    if (op->arg)
    {
        //копируем аргументы в буфер
        for (C_TYPE i = 0; i < op->arg; i++)
        {
            if (VarStack.Empty()) { SetError(ERR_SYNTAX,0,1);  return 0; }
            buf[i] = VarStack.Pop(0);
        }
        //если типы аргументов не совпадают, то ошибка
        if (!Operators->FSearch(ix,op->Name,buf,op->arg))
        {
            char str[MAX_STRING];
            lstrcpy(str,op->Name);
            lstrcat(str," (");
            if (op->arg >=2) { lstrcat(str,buf[1]->GetClassName()); lstrcat(str,","); }
            if (op->arg >=1) lstrcat(str,buf[0]->GetClassName());
            lstrcat(str,")'");
            SetError(ERR_NFOPTR,str);
            return 0;
        }
    }
    if (EQ_mode) // EQUATION !!!
    {
        pequ = new TEqu;
        for (C_TYPE i = 0; i < op->arg; i++)
        {
            if (EQStack.Empty()) { SetError(ERR_EQINTERNAL);  return 0; }
            pequ->AddElement(EQStack.Pop());
        }
        pequ->code = ((TOperator*)Operators->At(ix))->out;
        EQStack.Push(pequ);
        if (op->ret != 0) VarStack.Push(((TOperator*)Operators->At(ix))->ret);
        return 1;
    }
    //если это оператор присваивания, то добавляем индекс переменной перед ним в outcode
    if (op->Name[0] == ':' || !lstrcmpi(id->str,"++"))
    {
        ((TOperator*)Operators->At(ix))->Out(outcode,PopVar_ix);
        return 1;
    }
    else ((TOperator*)Operators->At(ix))->Out(outcode,ix);

    if (op->ret != 0) VarStack.Push(((TOperator*)Operators->At(ix))->ret);
    return 1;
}

C_TYPE MCompiler::FunctionOut4(char* fname, PClassListInfo* Args, C_TYPE ArgCount) // function - DLL
{
    char str[MAX_STRING];
    TDLLFunction* fn;
    INT16 args = 0;

    lstrcpy(str,fname); lstrcat(str,"(");
    for (C_TYPE j = ArgCount-1; j >0 ; j--) { lstrcat(str,Args[j]->GetClassName()); lstrcat(str,","); }
    if (ArgCount >=1) lstrcat(str,Args[0]->GetClassName());
    lstrcat(str,")");

    // есть ли DLL с такой функцией ?
    fn = dllmanager->SearchFunction(fname);
    if (!fn)//нету!
    {
        SetError(ERR_NFFUNC,str);
        return 0;
    }

    // Проверить параметры!
    if (ArgCount > fn->arg)
    {
        SetError(ERR_EXTRAPRM,str); return 0;
    }

    if (ArgCount < fn->arg)
    {
        SetError(ERR_FEWPRM,str); return 0;
    }

    if (fn->arg) // проверим типы аргументов
    {
        for (C_TYPE i = fn->arg-1; i >= 0; i--)
        {
            if ((fn->pointers)&(1<<(fn->arg-i-1)))
                if (!VarStack.SetPtr(outcode))
                { SetError(ERR_MBVAR,""); return 0; }
            if (fn->pType[i] != Args[fn->arg-1-i])
            { //  типы не совпали !
                SetError(ERR_NFFUNC,str);
                return 0;
            }
        }
    }

    // пишем код функции
    ((TOperator*)Functions->At(DLLFunction_ix))->Out(outcode,DLLFunction_ix);
    // пишем имя функции

    StrConstantOutEx(outcode,fname);
    // возвращаемое значение
    if (fn->ret)
        VarStack.Push(fn->ret);
    return 1;
}

C_TYPE MCompiler::FunctionOut3(char* fname, PClassListInfo* Args, C_TYPE ArgCount) // function - IMAGE
{
    // а не в уравнении ли мы ?
    if (EQ_mode) { SetError(ERR_INTERNAL,"Not supported in equations !");  return 0; }

    PClassListInfo buf[64];
    char str[MAX_STRING];

    PClassListInfo pcl;
    INT16 args = 0;

    // есть ли имидж с таким именем ?
    pcl = GetClassListInfo(fname);
    if (!pcl)
        return FunctionOut4(fname,Args,ArgCount); //может это функция DLL

    // он функция ?
    PClass _class = pcl->GetClass();
    if ((_class->flags&CF_PROCEDURE) == 0 )
        return FunctionOut4(fname,Args,ArgCount); //может это функция DLL

    //достать параметры
    for(INT16 i=0;i<_class->varcount;i++)
        if(_class->vars[i].flags&VF_ARGUMENT)
            buf[args++]=_class->vars[i].type;

    lstrcpy(str,fname); lstrcat(str,"(");
    for (C_TYPE j = ArgCount-1; j >0 ; j--) { lstrcat(str,Args[j]->GetClassName()); lstrcat(str,","); }
    if (ArgCount>=1) lstrcat(str,Args[0]->GetClassName());
    lstrcat(str,")");

    if (ArgCount > args)
    { //аргументов больше, чем должно быть
        SetError(ERR_EXTRAPRM,str); return 0;
    }

    if (ArgCount < args)
    { //аргументов меньше, чем должно быть
        SetError(ERR_FEWPRM,str); return 0;
    }

    if (args)
    {
        for (C_TYPE i = args-1; i >= 0; i--)
        {
            if (buf[i] != Args[args-1-i])
            { // аргументы не совпали !
                SetError(ERR_NFFUNC,str);	return 0;
            }
        }
    }

    // пишем код функции
    ((TOperator*)Functions->At(VFunction_ix))->Out(outcode,VFunction_ix);
    // пишем имя функции
    StrConstantOutEx(outcode,fname);
    // пишем число аргументов
    outcode->WriteWord(args);
    // пишем типы аргументов
    for (i = args-1; i >= 0 ; i--)
    {
        if (buf[i]->isFLOAT())  { outcode->WriteWord(-1); continue; }
        if (buf[i]->isSTRING()) { outcode->WriteWord(-2); continue; }
        if (buf[i]->isHANDLE()) { outcode->WriteWord(-3); continue; }
        if (buf[i]->isRGB())    { outcode->WriteWord(-4); continue; }
        StrConstantOutEx(outcode,buf[i]->GetClassName());
    }

    // возвращаемое значение
    pcl = 0;
    for(i=0; i < _class->varcount; i++)
    {
        if(_class->vars[i].flags&VF_RETURN)
        {
            pcl = _class->vars[i].type;
            VarStack.Push(pcl);
            break;
        }
    }
    // пишем возвращаемое значение
    if (pcl)
    {
        if (pcl->isFLOAT())    { outcode->WriteWord(-1); goto a; }
        if (pcl->isSTRING())   { outcode->WriteWord(-2); goto a; }
        if (pcl->isHANDLE())   { outcode->WriteWord(-3); goto a; }
        if (pcl->isRGB())      { outcode->WriteWord(-4); goto a; }
        StrConstantOutEx(outcode,pcl->GetClassName());
a:
    }
    else
        outcode->WriteWord(0);
    return 1;
}

//обрабатываем функции с переменным числом параметров
// function type (args,[args2])
C_TYPE MCompiler::FunctionOut2(C_TYPE ix, PClassListInfo* Args, C_TYPE ArgCount)
{
    if(EQ_mode) { SetError(ERR_INTERNAL,"Not supported in equations !");  return 0; }

    TOperator* op = (TOperator*)Functions->At(ix);

    char str[MAX_STRING];
    PClassListInfo MainArgs[64],DopArgs[64];
    C_TYPE _vars2;
    C_TYPE vars2 = _vars2 = ArgCount - op->arg; // дополнительных

    // проверки
    if (vars2 % op->arg2) //если количество доп.переменных в стеке не кратно количеству доп.переменных функции, то ошибка.
    {
        lstrcpy(str,op->Name);lstrcat(str,"(");
        for (C_TYPE j = 0; j < op->arg-1; j++) { lstrcat(str,op->pType[j]->GetClassName()); lstrcat(str,","); }
        if (op->arg >=1) lstrcat(str,op->pType[op->arg-1]->GetClassName());
        lstrcat(str,",[...])");
        SetError(ERR_ARG,str);return 0;
    }

    // достаем основные переменные
    for (C_TYPE i = _vars2; i < ArgCount; i++)
        MainArgs[i-_vars2]=Args[i];

    // проверяем основные переменные
    if(!Functions->FSearch(ix,op->Name,MainArgs,op->arg))
    {
        lstrcpy(str,op->Name);
        lstrcat(str,"(");
        for (C_TYPE j = op->arg-1; j > 0; j--) { lstrcat(str,MainArgs[j]->GetClassName()); lstrcat(str,","); }
        if (op->arg >=1) lstrcat(str,Args[0]->GetClassName());
        lstrcat(str,",[...])");
        SetError(ERR_NFFUNC,str);
        return 0;
    }

    // проверяем дополнительные переменные
    while ( (vars2 -= op->arg2) >= 0)
    {
        for (C_TYPE i = 0; i < op->arg2; i++)
        {
            if (Args[vars2+op->arg2-i-1]!= op->pType2[i] )
            {
                lstrcpy(str,op->Name);lstrcat(str,"(");
                for (C_TYPE j = 0; j < op->arg-1; j++)
                { lstrcat(str,op->pType[j]->GetClassName()); lstrcat(str,","); }
                if (op->arg >=1) lstrcat(str,op->pType[op->arg-1]->GetClassName());
                lstrcat(str,",[...])");
                SetError(ERR_ARG,str);
                return 0;
            }
        }
    }
    // достаем дополнительные переменные
    if(_vars2>0)
    {
        for (i = 0; i<op->arg2; i++)
            DopArgs[i]=Args[i];
        if(!Functions->FSearch(ix,op->Name,MainArgs,op->arg,0,DopArgs,op->arg2))
        {
            lstrcpy(str,op->Name);
            lstrcat(str,"(");
            for (C_TYPE j = op->arg-1; j > 0; j--) { lstrcat(str,MainArgs[j]->GetClassName()); lstrcat(str,","); }
            if (op->arg >=1) lstrcat(str,Args[0]->GetClassName());
            lstrcat(str,",[...])");
            SetError(ERR_NFFUNC,str);
            return 0;
        }
    }

    // наконец пишем код функции
    ((TOperator*)Functions->At(ix))->Out(outcode,ix);
    if (op->ret != 0)	VarStack.Push(op->ret);
    //	char s[200];
    //	wsprintf(s,"arg=%d, arg2=%d, a2cnt=%d, ret=%d",op->arg,op->arg2,_vars2,op->ret->isFLOAT());
    //	MessageBox(0,s,op->Name,0);

    // и число дополнительных аргументов
    outcode->WriteWord(_vars2);
    return 1;
}

//проверяем синтаксис функции
C_TYPE MCompiler::FunctionOut()
{
    C_TYPE ix, index=0, ArgsFilling=0;
    PClassListInfo Args[64];
    int ArgsPos[64];
    char ArgNames[MAX_STRING];
    TEqu* pequ;
    char* fname;

    if (FunStack.Empty()) { SetError(ERR_INTERNAL);  return 0; }
    if (FunNStack.Empty()) { SetError(ERR_INTERNAL);  return 0; }
    fname = FunNStack.Pop();

    //получаем количество аргументов в стеке
    C_TYPE ArgCount = VarStack.GetSize() - FunStack.Pop();

    //формируем список типов аргументов из стека в линейный массив Args
    //и сохраняем позицию этого аргумента в выходном коде в массив ArgsPos
    for(C_TYPE i = 0; i < ArgCount; i++)
    {
        Args[i] = VarStack.Pop(ArgsPos[i]);
        //char str[MAX_STRING];
        //wsprintf(str, "%s, %d, %s",fname,ArgsPos[i],Args[i]->GetClassName());
        //MessageBox(0,str,"",0);
    }

    //формируем строку с описанием текущей функции и ее аргументов - имя(аргумент1,...)
    lstrcpy(ArgNames,fname);
    lstrcat(ArgNames,"(");
    for (C_TYPE j=ArgCount-1; j>0; j--){lstrcat(ArgNames,Args[j]->GetClassName());lstrcat(ArgNames,","); }
    if (ArgCount>=1) lstrcat(ArgNames,Args[0]->GetClassName());
    lstrcat(ArgNames,")");

    //ищем оператор с именем fname и аргументами Args
    TOperator* op;
    while(Functions->SSearch(fname,ix,index))
    {
        index=ix+1;
        op = (TOperator*)Functions->At(ix);

        if (op->arg2)  //это функция с переменным числом аргументов !
            if(FunctionOut2(ix,Args,ArgCount))
                return 1;
            else
                continue;

        //если типы аргументов не совпадают, то ошибка
        //иначе выходим из цикла
        if(Functions->FSearch(ix,op->Name,Args,ArgCount))
        {
            index = Functions->count + 1;
            op = (TOperator*)Functions->At(ix);
            break;
        }
        else
        {
            SetError(ERR_NFFUNC,ArgNames);
        }
    }

    //если не нашли функцию fname
    if(index<=Functions->count && ix==Functions->count)
        return FunctionOut3(fname,Args,ArgCount); // может это функция имидж или DLL ?

    //проверка на указательность (доделать сообщение об ошибке !)
    for(i = ArgCount-1; i >= 0 ; i--)
    {
        VarStack.Push(Args[i],ArgsPos[i]);
    }

    for(i = 0; i < op->arg; i++)
    {
        if ( (op->pointers) & (1 << (op->arg - i - 1)))
        {
            if (!VarStack.SetPtr(outcode))
            { SetError(ERR_MBVAR,""); return 0; }
        }
        Args[i]=VarStack.Pop(ArgsPos[i]);
    }

    //если это уравнение
    if (EQ_mode) // EQUATION !!!
    {
        pequ = new TEqu;
        for (C_TYPE i = 0; i < op->arg; i++)
        {
            if (EQStack.Empty()) { SetError(ERR_EQINTERNAL);  return 0; }
            pequ->AddElement(EQStack.Pop());
        }
        pequ->code = ((TOperator*)Functions->At(ix))->out;
        EQStack.Push(pequ);
        if (op->ret != 0) VarStack.Push(op->ret);
        return 1;
    }

    ((TOperator*)Functions->At(ix))->Out(outcode,ix);
    if (op->ret != 0) VarStack.Push(op->ret);
    return 1;
}

//выполняем синтаксический анализ операторов/функций
C_TYPE MCompiler::AnalizeOPTR(C_TYPE ix,char* fname)      // <- ANALYZE OPERATOR
{
    //получаем описание оператора по его индексу в списке
    TOperator* op_in = (TOperator*)Operators->At(ix);

    if (EQ_mode)
    {
        if (Stack.GetSize())
        {
            while (op_in->GetImp2() <= ((TOperator*)Operators->At(Stack.GetTop()))->GetImp())
            {
                if (!OperatorOut((TOperator*)Operators->At(Stack.Pop())) ) return 0;
                if (!Stack.GetSize()) break;
            }
        }
    }
    else
    {
        if (Stack.GetSize())
        {
            while (op_in->GetImp2() <= ((TOperator*)Operators->At(Stack.GetTop()))->GetImp())
            {
                if (!OperatorOut((TOperator*)Operators->At(Stack.Pop())) ) return 0;
                if (!Stack.GetSize()) break;
            }
        }
    }
    switch (op_in->Name[0])
    {
        case FUN_FLAG_CHAR :  //если первый символ - '#'
        {
            FunNStack.Push(fname);
            Stack.Push(ix);
            FunStack.Push(VarStack.GetSize());
            return 1;
        }
        case ',' :
        {
            if (FunStack.Empty()) { SetError(ERR_MISPLACED,",");  return 0; }
            return 1;
        }
        case '(' :
        {
            Stack.Push(ix);
            return 1;
        }
        case ')' :
        {
            //если перед ) не было ( и других операторов, то ошибка
            if (Stack.Empty()) { SetError(ERR_MISPLACED,")");  return 0; }
            switch (((TOperator*)Operators->At(Stack.Pop()))->Name[0]){
                case '(' : return 1;
                    //если между ( и ) есть имя функции, то проверяем функцию
                case FUN_FLAG_CHAR : return FunctionOut();
                default :  { SetError(ERR_MISPLACED,")");  return 0; }
            }
        }
            //	 case '[' :
            //	 {
            //	  Stack.Push(ix2);
            //	  Stack.Push(ix);
            //	  FunStack.Push(VarStack.GetSize());
            //	  return 1;
            //	 }
        case ']' :
        {
            //	  if (Stack.Empty()) { SetError(ERR_MISPLACED,"]");  return 0; }
            //	  switch (((TOperator*)Operators->At(Stack.Pop()))->Name[0])
            //	  {
            //	   case FUN_FLAG_CHAR : return FunctionOut();
            //	   default :  { SetError(ERR_MISPLACED,"]");  return 0; }
            //	  }
        }break;
        case '+':
        {
            if(!lstrcmpi(id->str,"++"))
            {
                if (!OperatorOut((TOperator*)Operators->At(ix)) ) return 0;
                return 1;
            }
        }
    }
    Stack.Push(ix);
    return 1;
}

C_TYPE MCompiler::CheckStack(C_TYPE IsFErrDisabled)
{
    TOperator* op_st;
    while (!Stack.Empty())
    {
        op_st = (TOperator*)Operators->At(Stack.Pop());
        if (op_st->Name[0] == FUN_FLAG_CHAR)
        {
            if (IsFErrDisabled)
            {
                FunctionOut();
                continue;
            }
            else
            { SetError(ERR_MISPLACED,")"); return 0; }
        }
        if(op_st->Name[0] == '(')
        { SetError(ERR_MISPLACED,"("); return 0; }
        else
            if(!OperatorOut(op_st)) return 0;
    }
    return CheckVarStack();
}

C_TYPE MCompiler::CheckVarStack()
{
    if (VarStack.Empty()) return 1;
    else { SetError(ERR_SYNTAX,0,1);return 0; };
}

C_TYPE MCompiler::CheckEQStack()
{
    if (EQStack.GetSize() != 1)
    { SetError(ERR_SYNTAX,0,1); return 0; };
    TEqu* tmp = EQStack.Pop();
    if (IsDiffer) tmp->code = EC_EQDIF;
    if (IsDiffer == 2) tmp->code = EC_EQSTRING;
    if (!root) root = tmp;
    else root->InsertElement(tmp);
    return 1;
}

//определяем список переменных в одной строке (<тип> <модификатор> <имя1>, <имя2>...)
C_TYPE MCompiler::ReadDefVars(char* TypeName)
{
    C_TYPE ix;
    C_TYPE count,flags = 0;
    char str2[128];
    char errstr[MAX_STRING];
    //получаем информацию о типе по его названию
    PClassListInfo cli = GetClassListInfo(TypeName);
    if (!cli) { SetError(ERR_UNTYPE,TypeName); return 0; };
    TVarInfo * tv;
    //определяем область видимости переменной
    if(!lstrcmpi(id->str,"local")) {flags += VF_LOCAL; id->Read();}
    if(!lstrcmpi(id->str,"global")) {flags += VF_GLOBAL; id->Read();}
    if(!lstrcmpi(id->str,"classglobal")) {flags += VF_CLASSGLOBAL; id->Read();}
    if(!lstrcmpi(id->str,"nosave")) {flags += VF_NOSAVE; id->Read();}
    if(!lstrcmpi(id->str,"parameter"))
    {
        //если ранее не было найдено слово function, то ошибка
        if (!FN_mode) { SetError(ERR_MISPLACED,"parameter"); return 0; }
        flags += VF_ARGUMENT;
        id->Read();
    }
    //if (!lstrcmpi(id->str,"local")) { flags += VF_LOCAL; id->Read(); }
    //if (!lstrcmpi(id->str,"global")) { flags += VF_GLOBAL; id->Read(); }
    //if (!lstrcmpi(id->str,"classglobal")) { flags += VF_CLASSGLOBAL; id->Read(); }
    //if (!lstrcmpi(id->str,"nosave")) { flags += VF_NOSAVE; id->Read(); }
    goto l1;
    do
    {
        //если после типа или модификатора стоит не операнд, то ошибка
        if (id->Read() != opnd) { SetError(ERR_MBVAR); return 0; }
l1:
        lstrcpy(str2,id->str);
        id->Read();
        count = 0;
        //если переменная является массивом
        if (id->str[0] == '[')
        {
            count = 1;
            goto l2;
            do
            {
                id->Read();
                if (id->str[0] != '[') { SetError(ERR_MISPLACED,"["); return 0; };
l2:
                //если после [ не стоит число, то ошибка
                if (id->Read() != dconst) { SetError(ERR_MBNUM); return 0; }
                count *= atoi(id->str);
                id->Read();
                if (id->str[0] != ']') { SetError(ERR_MISPLACED,"]"); return 0; };
                id->Read();
            }
            while (id->str[0] == ',');
        }
        //вставляем переменную в список переменных
        ix = pvardata->Insert(str2,TypeName,count);
        if (ix == -1) { SetError(ERR_UNTYPE,TypeName); return 0; };
        //если переменная уже существует, то ошибка
        if (ix == -2)
        {
            if ( (tv = pvardata->Search(str2,&ix)) == 0 ) { SetError(ERR_INTERNAL); return 0; }
            lstrcpy(FirstString,str2);
            lstrcpy(errstr,str2);
            lstrcat(errstr," : ");
            lstrcat(errstr,tv->type->GetClassName());
            SetError(ERR_VAREXIST,errstr);
            return 0;
        }
        //если был установлен модификатор переменной, то сохраняем его
        if (flags)
            ((TVarInfo*)(pvardata->items->At(ix)))->SetFlags(flags);
        if (id->Type == eof) return 1;
    }
    while (id->str[0] == ',');
    return 1;
}

//выполняет чтение переменных, перечисленных через запятую
C_TYPE MCompiler::SetVars(C_TYPE flag)   // <- ReadEQ Vars
{
    C_TYPE ix;
    TVarInfo * tv;
    do
    {
        //если последняя лексема не операнд, то ошибка
        if (id->Read() != opnd) { SetError(ERR_MBVAR); return 0; }
        //ищем индекс и описание операнда по его имени в списке переменных
        tv = pvardata->Search(id->str,&ix);
        //если переменной с этим именем нет, то ошибка
        if (!tv) { SetError(ERR_NFVAR,id->str); return 0; }
        //устанавливаем флаг переменной
        tv->SetFlags(flag);
        //читаем следующую лексему
        id->Read();
    } while (id->str[0] == ',');
    return 1;
}

//-----------------------------------------------------------------  COMPILE !!!
//определяем операторы, функции и константы
C_TYPE MCompiler::ReadMString(C_TYPE ChStack)
{
    New_modificator = 0;
    C_TYPE LastIndex = optr;
    id->NewLine = 0;
    C_TYPE ix;
cicle:
    id->Read();
    if (id->NewLine && id->Type != eof) // end of string !!
    {
        id->NewLine = 0;
        if (!CheckStack(ChStack))
            return 0;
        LastIndex = optr;
        return 1;
    }
    switch (id->Type){
        case optr :   //нашли оператор
        {
            //если оператор есть в списке
            if (Operators->SSearch(id->str,ix))
            {
                switch (id->str[0]) {
                    case ';' : // end of mstring
                    {
                        id->Read();
                        goto end;
                    }
                    case '~' : // tilda !
                    {
                        if (New_modificator) { SetError(ERR_MISPLACED,"~"); return 0; };
                        New_modificator = 1;  goto cicle;
                    }
                    case '-' :// unary minus ?
                    {
                        if (LastIndex == optr)
                        {
                            if (AnalizeOPTR(UNARY_ix)) goto cicle;
                            else return 0;
                        }
                    }
                    case '!' :// unary ! ?
                    {
                        if (LastIndex == optr && id->str[1] != '=')
                        {
                            if (AnalizeOPTR(ix)) goto cicle;
                            else return 0;
                        }
                        else break;
                    }
                    case '+' ://
                    {
                        if (LastIndex == optr && id->str[1] != '+')
                        {
                            if (AnalizeOPTR(ix)) goto cicle;
                            else return 0;
                        }
                        else break;
                    }
                    case ':' : // недолжно быть
                    {
                        if (id->str[1] == '='){SetError(ERR_SYNTAX); return 0; }
                    }
                }
                //если идут подряд 2 оператора, то ошибка
                if (id->str[0] != '(' && id->str[0] != ')') // syntax check !
                {
                    if(LastIndex == optr) { SetError(ERR_MISPLACED,id->str); return 0; }
                    else LastIndex = optr;
                }
                if (AnalizeOPTR(ix)) goto cicle;
                else return 0;
            }
            else
            {
                SetError(ERR_NFOPTR,id->str); return 0;
            }
        }
        case opnd : //нашли операнд
        {
            //если следующая лексема - оператор (, то этот операнд должен быть именем функции
            if (id->IsNextOptr('('))
            {
                //если этот операнд - имя функции
                //и у него нет возвращаемого значения, то ошибка
                if (Functions->SSearch(id->str,ix))
                {
                    //if (((TOperator*)Functions->At(ix))->ret == 0)
                    //{ SetError(ERR_MISPLACED,((TOperator*)Functions->At(ix))->Name); return 0; };
                }
                if (AnalizeOPTR(FunFlag_ix,id->str))
                {
                    id->Read();
                    if (id->GetNextChar() == ')') LastIndex = opnd;
                    else LastIndex = optr;
                    goto cicle;
                }
                else  return 0;
            }
            //	case '[' :                // It's Array !
            //	{
            //	 if (AnalizeOPTR(FunFlag_ix,"GetElement")) { id->Read(); LastIndex = optr; goto cicle; }
            //	  else return 0;
            //	}
            //	case '.' :                // It's Structure !
            //	{
            //	 return 0;
            //	}
            //   }
            // если перед этим операндом стоял тоже операнд, то ошибка
            if (LastIndex == opnd){	SetError(ERR_MBOPTR,id->str); return 0; }
            else LastIndex = opnd;
            // It's Macro ?
            if (Constants->Search(id->str,ix)) // It's Macro !!!
            {
                if (!ConstantOut("FLOAT")) return 0;
                //double d = (((TMacro*)Constants->At(ix))->Number);
                if (!EQ_mode) DecConstantOut(outcode,(((TMacro*)Constants->At(ix))->Number));
                goto cicle;
            }
            // It's Operand !!!
            if (VarOut(id->str)) goto cicle;
            else return 0;
        }
        case dconst : // нашли вещественную константу
        case fconst :
        {
            // если перед константой стоял операнд, то ошибка
            if (LastIndex == opnd){	SetError(ERR_MBOPTR,id->str);	return 0; }
            else LastIndex = opnd;
            if (!ConstantOut("FLOAT")) return 0;
            if (!EQ_mode) DecConstantOut(outcode,id->str);
            goto cicle;
        }
        case iconst : // integer constant :
        {
            // syntax check !
            if (LastIndex == opnd) {SetError(ERR_MBOPTR,id->str);return 0; }
            else LastIndex = opnd;
            if (!ConstantOut("HANDLE")) return 0;
            if (!EQ_mode) IConstantOut(outcode,id->str);
            goto cicle;
        }
        case sconst : // string constant :
        {
            if (EQ_mode) { SetError(ERR_SYNTAX); return 0; }
            if (LastIndex == opnd){	SetError(ERR_MBOPTR,id->str);	return 0; }
            else LastIndex = opnd;
            if (!ConstantOut("STRING")) return 0;
            StrConstantOut(outcode,id->str,id->SConstLength);
            goto cicle;
        }
        case hconst : // hex constant :
        {
            if (LastIndex == opnd)	{SetError(ERR_MBOPTR,id->str);return 0; }
            else LastIndex = opnd;
            if (!ConstantOut("INTEGER")) return 0;
            HexConstantOut(outcode,id->str);
            goto cicle;
        }
        case eof : // end of file
        {
            break;
        }
    }

end:
    if (!CheckStack(ChStack)) return 0;
    return 1;
}

//..................................................................
enum {st_normal,st_if,st_else,st_while,st_until,st_switch,st_for}; // state

C_TYPE MCompiler::EQAnalyze()
{
    char buf[128];
    TVarInfo * tv;
    C_TYPE fix,ix;
    if (id->ScanChar('?')) // Переменные + Функция решить уравнение !
    {
        // ищем спец. функцию
        if (!Functions->SSearch("equation",fix))
        { SetError(ERR_INTERNAL," equation (612) "); return 0; }
        if (id->Type != opnd) { SetError(ERR_MBVAR); return 0; }
        goto mm1;
        do
        {
            if (id->Read() != opnd) { SetError(ERR_MBVAR); return 0; }
mm1:
            tv = pvardata->Search(id->str,&ix);
            if (!tv) { SetError(ERR_NFVAR,id->str); return 0; }
            // помещаем в код функцию и индекс переменной
            ((TOperator*)Functions->At(fix))->Out(outcode,ix);
            tv->SetFlags(VF_EQVAR);
            id->Read();
        }
        while (id->str[0] == ',');
        id->Read();
        if (id->str[0] != '?')
        { SetError(ERR_SYNTAX); return 0; }
        id->Read();
        return 1;
    }
    EQ_mode = 1;
    IsDiffer = 0;
    if (id->str[0] == 'd' && id->str[1] != 0)
    {
        // first var
        lstrcpy(buf,id->str+1);
        if (!id->IsNextOptr('/')) { goto mm2; }
        id->Read();
        tv = InsertVar(ix,buf,"FLOAT");  if (!tv) return 0;
        TEqu* pequ = new TEqu;
        pequ->code = EC_VAR; pequ->index = ix;  pequ->eqflag = 1;
        EQStack.Push(pequ);
        // delimiter
        if (!Operators->SSearch("/",ix)) { SetError(ERR_INTERNAL," not found / "); return 0; }
        if (!AnalizeOPTR(ix)) return 0;
        // twelb var
        id->Read();
        if (id->str[0] != 'd' || id->str[1] == 0) { SetError(ERR_WAIT,"dt"); return 0; }
        tv = InsertVar(ix,id->str,"FLOAT");  if (!tv) { delete pequ; return 0; }
        tv->def = NewStr("0.01");
        pequ = new TEqu;
        pequ->code = EC_VAR;
        pequ->index = ix;
        pequ->eqflag = 1;
        EQStack.Push(pequ);
        // спец +
        if (!Operators->SSearch("+",ix)) { SetError(ERR_INTERNAL," not found / "); return 0; }
        if (!AnalizeOPTR(ix)) return 0;
        // + Время
        tv = InsertVar(ix,id->str+1,"FLOAT");  if (!tv) { delete pequ; return 0; }
        pequ = new TEqu;
        pequ->code = EC_VAR; pequ->index = ix; pequ->eqflag = 1;
        EQStack.Push(pequ);
        id->Read();
        if (id->str[0] != '=') { SetError(ERR_WAIT,"="); return 0; }
        if (!Operators->SSearch(id->str,ix)) { SetError(ERR_INTERNAL," = "); return 0; }
        if (!AnalizeOPTR(ix)) return 0;
        if (!ReadMString(1)) return 0;
        EQ_mode = 0;
        IsDiffer = 1;
        if (!CheckEQStack()) return 0;
        IsDiffer = 0;
        return 1;
    }
mm2:
    // this normal equations
    id->Back();
    if (!ReadMString()) return 0;
    EQ_mode = 0;
    if (!CheckEQStack()) return 0;
    return 1;
}

C_TYPE MCompiler::SetLabel(C_TYPE dest)
{
    if (dest >= 0)
    {
        outcode->Seek(-2,TStream::FROM_END);
        outcode->WriteWord(dest / 2);
        return 0;
    }
    return (C_TYPE)outcode->GetPos() - 2;
}

C_TYPE MCompiler::LinkLabel(C_TYPE position) // текущую позицию поставить в переход
{
    C_TYPE i = (C_TYPE)(outcode->GetPos() / 2);

    outcode->Seek(position);
    outcode->WriteWord(i);
    outcode->Seek(0,TStream::FROM_END);
    return 1;
}

//Выполнение лексического анализа
C_TYPE MCompiler::ReadString(C_TYPE state, C_TYPE label0, TIndexStack* plabelB)
{
    New_modificator = 0;
    C_TYPE label1,label2,label3;
    C_TYPE ix;
    C_TYPE labelS = 0;
    C_TYPE SVarState = 0;
    TIndexStack labelB;

    if(state == st_if || state==st_while)
        label1 = SetLabel();

    //выполняем выделение терминальных символов
    while (!id->IsEOF())
    {
        //если в начале строки не стоят знак ( или ~, или нет операнда, или это оператор, то ошибка
        if (id->str[0] != '(' && id->str[0] != '~' &&
            id->Type != opnd &&
            Operators->SSearch(id->str,ix))
        {	SetError(ERR_FIRSTVAR); return 0; }
        // Этап 1 - проверяем ключевые слова, входящие в синтаксис языка
        if (!lstrcmpi(id->str,"function"))             // It's PARAMETERS
        {
            OnePhase_mode = 1;
            FN_mode = 1;
            pclass->flags = pclass->flags | CF_PROCEDURE;
            id->Read();
            continue;
        }
        if (!lstrcmpi(id->str,"return"))               // It's RETURN
        {
            C_TYPE ix;
            TVarInfo * tv;
            //если раньше не было найдено слова function, то возвращаем ошибку
            if (FN_mode++ != 1) { SetError(ERR_MISPLACED,"return"); return 0; }
            if (!OnePhase_mode) { SetError(ERR_MBVAR); return 0; }

            //если после слова return нет операнда, то возвращаем ошибку
            if (id->Read() != opnd) { SetError(ERR_MBVAR); return 0; }

            //проверяем переменную после слова return
            tv = pvardata->Search(id->str,&ix);
            if (!tv) { SetError(ERR_NFVAR,id->str); return 0; }
            tv->SetFlags(VF_RETURN);
            id->Read();
            continue;
        }
        if (!lstrcmpi(id->str,"switch"))                         // It's SWITCH
        {
            if(id->str[0] == ';')
                id->Read();
            id->Read();
            //если сразу после слова switch нет слова case, то возвращаем ошибку
            if (lstrcmpi(id->str,"case")) { SetError(ERR_WAIT,"case"); return 0; }
            //выполняем рекурсивный вызов функции readstring для лексического анализа
            //текста, стоящего после слова case.
            if (!ReadString(st_switch,SVarState))
                return 0;      // goto down ..
            id->Read();
            continue;
        }
        if (!lstrcmpi(id->str,"case"))                         // It's CASE
        {
            if (state != st_switch) {SetError(ERR_MISPLACED,"SWITCH"); return 0;}
            if (labelS) // если есть предыдущий CASE
            {
                if (!AnalizeOPTR(FunFlag_ix,"JMP"))
                    return 0;   // отваливаем на конец

                if (!CheckStack(1))
                { SetError(ERR_INTERNAL,"break"); return 0; }

                labelB.Push(SetLabel());
                LinkLabel(labelS);  // начинаем отсуда
            }
            // переход
            if (!AnalizeOPTR(FunFlag_ix,"JZ")) return 0;
            id->Read();
            if (!ReadMString())
                return 0;

            if (!CheckStack(1))
            { SetError(ERR_INTERNAL,"break"); return 0; }

            labelS = SetLabel();
            //id->Read();
            continue;
        }
        if (!lstrcmpi(id->str,"default"))                      // It's DEFAULT
        {
            if (state != st_switch) {SetError(ERR_MISPLACED,"SWITCH"); return 0;}
            if (labelS) // если есть предыдущий CASE
            {
                if (!AnalizeOPTR(FunFlag_ix,"JMP")) return 0;   // отваливаем на конец
                if (!CheckStack(1)) { SetError(ERR_INTERNAL,"break"); return 0; }
                labelB.Push(SetLabel());
                LinkLabel(labelS);  // начинаем отсуда
            }
            labelS = 0;
            id->Read();
            continue;
        }
        if (!lstrcmpi(id->str,"break"))                         // It's BREAK
        {
            if (!AnalizeOPTR(FunFlag_ix,"JMP")) return 0;
            if (!CheckStack(1)) { SetError(ERR_INTERNAL,"break"); return 0; }

            if (plabelB)
                plabelB->Push(SetLabel());
            else
                labelB.Push(SetLabel());

            id->Read();
            while(id->str[0] == ';') id->Read();
            continue;
        }
        if (!lstrcmpi(id->str,"do"))                         // It's DO-UNTIL
        {
            label3 = SetLabel();
            id->Read();
            if (!ReadString(st_until,label3+2)) return 0;
            //id->Read();
            continue;
        }
        if (!lstrcmpi(id->str,"until"))                        // It's UNTIL
        {
            if (state != st_until) {SetError(ERR_MISPLACED,"UNTIL"); return 0;}
            if (!AnalizeOPTR(FunFlag_ix,"JNZ")) return 0;
            id->Read();
            if (!ReadMString()) return 0;
            if (!CheckStack(1)) { SetError(ERR_INTERNAL,"until"); return 0; }
            SetLabel(label0); // переход на DO
            while (labelB.GetSize())
                LinkLabel(labelB.Pop()); // если есть break
            return 1;
        }
        if (!lstrcmpi(id->str,"if"))                           // It's IF
        {
            if (!AnalizeOPTR(FunFlag_ix,"JZ")) return 0;
            id->Read();
            if (!ReadMString()) return 0;
            if (!ReadString(st_if,0,&labelB)) return 0;      // goto down ..
            id->Read();
            continue;
        }
        if (!lstrcmpi(id->str,"while"))                         // It's WHILE
        {
            label3 = SetLabel();
            if (!AnalizeOPTR(FunFlag_ix,"JZ")) return 0;
            id->Read();
            if (!ReadMString()) return 0;
            if (!ReadString(st_while,label3+2)) return 0;     // goto down ..
            id->Read();
            continue;
        }
        if (id->str[0] == 'e' || id->str[0] == 'E')
        {
            if (!lstrcmpi(id->str,"endswitch"))                // It's ENDSWITCH
            {
                if (state != st_switch) {SetError(ERR_MISPLACED,"ENDSWITCH"); return 0;}

                SwitchVar.Pop();
                while (labelB.GetSize())
                    LinkLabel(labelB.Pop()); // если есть break
                if (labelS) LinkLabel(labelS); // связываем предыдущий переход
                return 1;
            }
            if (!lstrcmpi(id->str,"endwhile"))                      // It's ENDWHILE
            {
                if (state != st_while) {SetError(ERR_MISPLACED,"ENDWHILE"); return 0;}
                if (!AnalizeOPTR(FunFlag_ix,"JMP")) return 0;
                if (!CheckStack(1)) { SetError(ERR_INTERNAL,"while"); return 0; }
                SetLabel(label0);
                while (labelB.GetSize()) LinkLabel(labelB.Pop()); // если есть break
                if (!LinkLabel(label1)) { SetError(ERR_INTERNAL,"link"); return 0; } // ?????
                return 1;
            }
            if (!lstrcmpi(id->str,"else"))                           // It's ELSE
            {
                if (state != st_if) {SetError(ERR_MISPLACED,"ELSE"); return 0;}
                if (!AnalizeOPTR(FunFlag_ix,"JMP")) return 0;
                if (!CheckStack(1)) { SetError(ERR_INTERNAL,"else"); return 0; }
                label2 = SetLabel();
                if (!LinkLabel(label1)) { SetError(ERR_INTERNAL,"link"); return 0; }
                state = st_else;
                id->Read();
                continue;
            }
            if (!lstrcmpi(id->str,"endif"))                           // It's ENDIF
            {
                switch (state){
                    case st_else :
                    {
                        if (!LinkLabel(label2))
                        {	SetError(ERR_INTERNAL,"link"); return 0; }
                        break;
                    }
                    case st_if :
                    {
                        if (!LinkLabel(label1))
                        { SetError(ERR_INTERNAL,"link"); return 0; }
                        break;
                    }
                    default :
                    {	SetError(ERR_MISPLACED,"ENDIF"); return 0;}
                }
                return 1;
            }
        }  // e...
        if (id->str[0] == '?' && id->str[1] == 0)       // It's EQUATION string !
        {
            if (!SetVars(VF_EQVAR)) return 0;
            continue;
        }
        if (id->IsEquation())                           // It's EQUATIONS
        {
            if (!EQAnalyze())
                return 0;
            continue;
        }
        //если после текущей лексемы стоит (, то значит это функция
        switch (id->GetNextChar())
        {
            case '(' :
            {
                if (!AnalizeOPTR(FunFlag_ix,id->str)) return 0;
                id->Read();
                if (!ReadMString()) return 0;
                else continue;
            }
            case '[' :                // It's array !
            {
                //if (!AnalizeOPTR(FunFlag_ix,SetElement_ix)) return 0;
            }
        }

        // Этап 2 - если следующий оператор ::, := или ++, обрабатываем их,
        // иначе считаем, что это строка определения переменных <тип> <модификатор> <имя1> <имя2>
        lstrcpy(FirstString,id->str);
        id->Read();
        if (id->str[0] == ':' && id->GetNextChar() == ':') // Спец строка - уравнение
        {
            EQ_mode = 1;   // first var
            InsertVar(ix,FirstString,"FLOAT");//  if (!tv) return 0;

            TEqu* pequ = new TEqu;
            pequ->code = EC_VAR;
            pequ->index = ix;
            pequ->eqflag = 1;
            EQStack.Push(pequ);

            id->Read();
            if (!Operators->SSearch("=",ix))
            {
                SetError(ERR_INTERNAL," = ");
                return 0;
            }
            if (!AnalizeOPTR(ix))
                return 0;
            if (!ReadMString(1))
                return 0;

            EQ_mode = 0;
            IsDiffer = 2;
            if (!CheckEQStack())
                return 0;

            IsDiffer = 0;
            continue;
        }
        if (!lstrcmpi(id->str,":="))                           // It's assign string!
        {
            if (OnePhase_mode)
                lstrcat(id->str,"_old");

            if (!Operators->SSearch(id->str,ix))
            {
                SetError(ERR_INTERNAL," := ");
                return 0;
            }
            if (!AnalizeOPTR(ix))
                return 0;
            if (!ReadMString())
                return 0; //компилируем выражение, стоящиее после :=
            else
                continue;
        }
        if (!lstrcmpi(id->str,"++"))
        {
            if (!Operators->SSearch(id->str,ix))
            {
                SetError(ERR_INTERNAL," ++ ");
                return 0;
            }
            if (!AnalizeOPTR(ix))
                return 0;
            id->Read();
            while(id->str[0]==';')
                id->Read();
            continue;
        }
        if (!ReadDefVars(FirstString))
            return 0;         // It's define vars!
        else
            continue;
    }
    while (labelB.GetSize())
        LinkLabel(labelB.Pop()); // если есть break
    if (state != st_normal)
    {	SetError(ERR_SYNTAX); return 0; }
    return 1;
}

//.............................................................................

C_TYPE MCompiler::Compile(TClass* _pclass, TVarData* _pvardata,char* pb)
{
    TMemoryStream in(pb); //pb - компилируемый текст
    OnePhase_mode = 0;
    EQ_mode = 0;
    FN_mode = 0;
    root = 0;
    pclass = _pclass;
    id = new ID(in);
    pvardata = _pvardata;
    // начинаем прогресс бар
    //if (id->size > 1000)
    //{
    id->Status = 1;
    SCSetStatusText(0,"Compiling");
    //}
    outcode = new TMemoryStream(TRUE);
    if (id->Read() == eof)
        goto end;

    //начинаем компиляцию
    do
    {
        if (!ReadString(st_normal))
            goto err_end;
    }
    while (!id->IsEOF());

end:
    if (id->Status)
    {
        SCSetStatusProgressBar(1,1001);
        SCSetStatusText(0,"");
    }
    Epilog->Out(outcode,0);
    LoadString(HResource,IDS_str233,ErrText,sizeof(ErrText));
    //lstrcpy(ErrText,"Готово");
    pvardata->Fix();
    pclass->SetCode(outcode);

    delete outcode;
    delete id;
    pclass->SetEqu(root);
    return 1;

err_end:
    if (id->Status)
    {
        SCSetStatusProgressBar(1,1001);
        SCSetStatusText(0,"");
    }
    pvardata->Undo();
    Stack.Clear();
    VarStack.Clear();
    FunStack.Clear();
    EQStack.Clear();
    FunNStack.Clear();
    delete outcode;
    delete id;
    delete root;
    return 0;
}

#define cABC__XYZ(a)  (((a)>=0x41) && ((a)<=0x5A))
#define cabc__xyz(a)  (((a)>=0x61) && ((a)<=0x7A))
#define c012__789(a)  (((a)>=0x30) && ((a)<=0x39))
#define c__(a)        (0x5F==(a))			 // знак нижнего подчеркивания "_"
#define cALL(a)  (cABC__XYZ(a)||cabc__xyz(a)||c012__789(a)||c__(a))
#define cRUSwin(a)    (((a)>=(char)0xC0) && ((a)<=(char)0xFF))
#define CharInWord(a)\
    (cALL(a)||cRUSwin(a))

#include <stdio.h>

void MCompiler::SetError(C_TYPE index, char* info, C_TYPE m)
{
    char errstr[MAX_STRING];

    if (m) Err_x1 = Err_x2 = id->LastPos+1;
    else
    {
        switch(index)
        {
            case ERR_FEWPRM:
            case ERR_EXTRAPRM:
            case ERR_NFFUNC:
            {
                int i=1;
                int f=1;
                for (; f > 0; i++)
                {
                    if(id->buffer[id->counter - i]==')')
                        f++;

                    if(id->buffer[id->counter - i]=='(')
                        f--;
                }
                //    	Err_x2 = id->counter-i+1;
                for (i++; CharInWord(id->buffer[id->counter-i]); i++);

                Err_x1 = id->counter-i+1;
                Err_x2 = id->counter+1;
            }break;

            case ERR_VAREXIST:
            {
                Err_x1 = id->counter+1-lstrlen(id->str)-lstrlen(FirstString);
                Err_x2 = id->counter+1-lstrlen(id->str);
            }break;
            case ERR_UNTYPE:
            {
                Err_x1 = id->counter-lstrlen(id->str)-lstrlen(info);
                Err_x2 = id->counter-lstrlen(id->str);
            }break;
            default:
            {
                Err_x1 = id->counter + 1 - lstrlen(id->str);
                Err_x2 = id->counter + 1 ;
                break;
            }
        }
    }
    lstrcpy(ErrText,"");
    //LoadString(HResource,IDS_str234,ErrText,sizeof(ErrText));
    //lstrcpy(ErrText,"Error : ");
    if(index!=0)
    {
        if (LoadString(HResource,index,errstr,sizeof(errstr)-1) == 0)
            LoadString(HResource,IDS_str235,ErrText,sizeof(ErrText));
        lstrcat(ErrText,errstr);
        if (info)
        {
            lstrcat(ErrText,info);
            lstrcat(ErrText,"'");
        }
    }
};

char* MCompiler::GetLastError()
{
    return ErrText;
}

void MCompiler::GetLastErrorPos(C_TYPE* p1,C_TYPE* p2)
{
    *p1 = Err_x1;
    *p2 = Err_x2;
}

int MCompiler::GetColor(char*s,C_TYPE size)
{
    C_TYPE Index=0;

    if(Functions->FindKeyWorld(s,Index))
    {
        return 1;
    }
    else
    {
        char *keys[]={"local","parameter","function","if","endif","while","endwhile","return","else","do","until","break","switch","case","endswitch","default"};

        for(int i=0;i<sizeof(keys)/sizeof(char*);i++)
        {
            if(!lstrcmpi(s,keys[i]))
                return -1;
        }

        C_TYPE ix;
        if (Constants->Search(s,ix))
            return -2;

        if(dllmanager)
        {
            TDLLFunction* func = dllmanager->SearchFunction(s);
            if(func)
                return -3;
        }
    }
    return 0;
};

BOOL MCompiler::GetConstant(char*key,double&d)
{
    C_TYPE index;
    if(Constants->Search(key,index))
    {
        TMacro*m=(TMacro*)Constants->At(index);
        d=(m->Number);
        return TRUE;
    }
    return FALSE;
};

void MCompiler::FillList(HWND list)
{
    for(C_TYPE i=0;i<Functions->count;i++)
    {
        TOperator *op=(TOperator *)compiler->Functions->At(i);
        char *name=op->Name;
        SendMessage(list,LB_ADDSTRING,0,(LPARAM)name);
    }
};

void MCompiler::FunctionHelp(HWND hwnd,C_TYPE i)
{
    if (i>-1 && i<Functions->count)
    {
        TOperator *op=(TOperator *)Functions->At(i);
        char s[512]="";
        char ss[256]="";
        if (op->ret)
            lstrcpy(s,op->ret->GetClassName());
        lstrcat(s," ");
        lstrcat(s,op->Name);
        lstrcat(s,"(");
        for(C_TYPE j=0;j<op->arg;j++)
        {
            lstrcat(s,op->pType[j]->GetClassName());
            if (j!=(op->arg-1))lstrcat(s,",");
            if (j==0)
            {
                LoadString(HResource,IDS_str168,ss,sizeof(ss));
            }
            lstrcat(ss,"");
            lstrcat(ss,op->pType[j]->GetClassName());
            if (j!=(op->arg-1))lstrcat(ss,"\n");
        }
        lstrcat(s,")");
        lstrcat(s,"\n");
        lstrcat(s,ss);
        MessageBox(hwnd,s,op->Name,MB_OK|MB_ICONINFORMATION);
    }
};

BOOL MCompiler::GetFunctionName(INT16 code,char*s)
{
    for(C_TYPE i=0;i<Functions->count;i++)
    {
        TOperator *op=(TOperator *)Functions->At(i);
        if(op->out==code)
        {
            lstrcpy(s,op->Name);
            return TRUE;
        }
    }
    for(i=0;i<Operators->count;i++)
    {
        TOperator *op=(TOperator *)Operators->At(i);
        if(op->out==code)
        {
            lstrcpy(s,op->Name);
            return TRUE;
        }
    }
    lstrcpy(s,"");
    return FALSE;
}
