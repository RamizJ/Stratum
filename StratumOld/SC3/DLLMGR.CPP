// дополнительный модуль
#include <stdlib.h>
#include "collect.h"

#include "dllman.h"
#ifndef MAX_STRING
#define MAX_STRING 256
#endif
#include "stream.h"
#include "_cmpl.h"
#include <stratum\stratum.h>
#include <stratum\sc_types.h>
//#include <stratum\sc_types.h>
#include <stratum\sc_msg.h>
#include "project.h"
#include "twindow.h"
#include "strings.rh"


extern HWND title;
void ShowError(char* dllname,char* comment)
{
    char str[MAX_STRING];
    lstrcpy(str,"DLL template ");
    lstrcpy(str,dllname);
    lstrcat(str," init fault: ");
    lstrcat(str,comment);
    if(title)ShowWindow(title,SW_HIDE);
    HWND parent=mainwindow?mainwindow->HWindow:0;
    MSGBOX(parent,str,_MSG_ERROR,MB_TASKMODAL | MB_ICONHAND);
}
INT16 TDLLFunction::GetTSize(INT16 type)
{
    switch (type)
    {
        case 1:
        case 2:
        case 3:
        case 4: return 2;
        case 5:
        case 6:
        case 7: return 4;
        case 8: return 8;
        case 9: return 10;
    }
    ShowError("","incorrect type dll argument found !");
    return 0;
}


TDLLFunction::~TDLLFunction()
{
    if (Name) delete Name;
    if (RealName) delete RealName;
    if (pType) delete pType;
};

void (*pfun)();

BOOL TDLLFunction::Call(BYTE** stuck)
{
    if (!pproc) if(!pdll->Init()) return 0;

    pfun = (void (*)())pproc;
    BYTE* pstuck = *stuck;

    // толкаем в стек аргументы:
    BYTE* parg; // CDECL // прямой порядок
    INT16 size;
    INT16 i;
    INT16 max;
    INT16 lstuck;

#ifndef WIN32
    int _rES;
#endif

    // Type = !Type;

    if (!Type)     // PASCAL обратный порядок
    {
        parg = pstuck + argsize;
        i = arg-1;
        max = -1;
    }
    else
    {
        parg = pstuck; // CDECL прямой порядок
        size = 0;
        i = 0;
        max = arg;
        // запомним стек
        if (Type)
        {
            asm {
                mov word ptr ss:[lstuck],sp
            }
            }

            }

                  for (i; i != max; i+= Type?1:-1)
                {
                    if (pType[arg-1-i]->isFLOAT()) size = 8;
                    else size = 4;
                    //  parg -= (Type==0)*GetTSize(argconv[i+1]);
                    parg -= (Type==0)*size;
#if defined(WIN32)

                    switch (10*(size==8)+argconv[arg-1-i+1])
                    {
                        case 1 : // word32 -> int8
                        case 2 : // word32 -> word8
                        case 3 : // word32 -> int16
                        case 4 : // word32 -> word16
                        {
                            asm {
                                mov esi,dword ptr parg
                                        push dword ptr [esi]
                            }
                            break;
                        }
                        case 5 : // word32 -> int32
                        case 6 : // word32 -> word32
                        {
                            asm {
                                mov esi,dword ptr parg
                                        push dword ptr [esi]
                            }
                            break;
                        }
                        case 7 : // word32 -> float32
                        {
                            asm {
                                mov esi,dword ptr parg
                                        fild dword ptr [esi]
                                        sub sp,4
                                        mov ebx,esp
                                        fstp dword ptr [ebx]
                            }
                            break;
                        }
                        case 8 : // word32 -> float64
                        {
                            asm {
                                mov esi,dword ptr parg
                                        fild dword ptr [esi]
                                        sub sp,8
                                        mov ebx,esp
                                        fstp qword ptr [ebx]
                            }
                            break;
                        }
                        case 9 : // word32 -> float80
                        {
                            asm {
                                mov esi,dword ptr parg
                                        fild dword ptr [esi]
                                        sub sp,10
                                        mov ebx,esp
                                        fstp tbyte ptr [ebx]
                            }
                            break;
                        }
                        case 11 : // float64 -> int8
                        case 12 : // float64 -> word8
                        case 13 : // float64 -> int16
                        case 14 : // float64 -> word16
                        {
                            asm {
                                //	 mov esi,dword ptr parg
                                //	 fld qword ptr [esi]
                                //	 mov ebx,esp
                                //	 fistp dword ptr [ebx-4]
                                //    mov ax,word ptr [ebx-4]
                                //    mov word ptr [ebx-2],ax
                                //    sub sp,2
                                mov esi,dword ptr parg
                                        fld qword ptr [esi]
                                        sub sp,4
                                        mov ebx,esp
                                        fistp dword ptr [ebx]
                            }
                            break;
                        }
                        case 15 : // float64 -> int32
                        case 16 : // float64 -> word32
                        {
                            asm {
                                mov esi,dword ptr parg
                                        fld qword ptr [esi]
                                        sub sp,4
                                        mov ebx,esp
                                        fistp dword ptr [ebx]
                            }
                            break;
                        }
                        case 17 : // float64 -> float32
                        {
                            asm {
                                mov esi,dword ptr parg
                                        fld qword ptr [esi]
                                        sub sp,8
                                        mov ebx,esp
                                        fstp dword ptr [ebx]
                            }
                            break;
                        }
                        case 18 : // float64 -> float64
                        {
                            asm {
                                mov esi,dword ptr parg
                                        push dword ptr [esi+4]
                                        push dword ptr [esi]
                            }
                            break;
                        }
                        case 19 : // float64 -> float80
                        {
                            asm {
                                mov esi,dword ptr parg
                                        fld qword ptr [esi]
                                        sub sp,10
                                        mov ebx,esp
                                        fstp tbyte ptr [ebx]
                            }
                            break;
                        }
                    }
#else
                    asm {
                        mov _rES,es
                    }
                    switch (10*(size==8)+argconv[arg-1-i+1])
                    {
                        case 1 : // word32 -> int8
                        case 2 : // word32 -> word8
                        case 3 : // word32 -> int16
                        case 4 : // word32 -> word16
                        {
                            asm {
                                les si,dword ptr parg
                                        push word ptr es:[si]
                            }
                                  break;
                            }
                        case 5 : // word32 -> int32
                            case 6 : // word32 -> word32
                            {
                                asm {
                                    les si,dword ptr parg
                                            push word ptr es:[si+2]
                                                              push word ptr es:[si]
                                }
                                      break;
                                }
                            case 7 : // word32 -> float32
                                {
                                    asm {
                                        les si,dword ptr parg
                                                fild dword ptr es:[si]
                                                                   sub sp,4
                                                                   mov bx,sp
                                                                   fstp dword ptr ss:[bx]
                                    }
                                          break;
                                    }
                                case 8 : // word32 -> float64
                                    {
                                        asm {
                                            les si,dword ptr parg
                                                    fild dword ptr es:[si]
                                                                       sub sp,8
                                                                       mov bx,sp
                                                                       fstp qword ptr ss:[bx]
                                        }
                                              break;
                                        }
                                    case 9 : // word32 -> float80
                                        {
                                            asm {
                                                les si,dword ptr parg
                                                        fild dword ptr es:[si]
                                                                           sub sp,10
                                                                           mov bx,sp
                                                                           fstp tbyte ptr ss:[bx]
                                            }
                                                  break;
                                            }
                                        case 11 : // float64 -> int8
                                            case 12 : // float64 -> word8
                                                //  {
                                                //	asm {
                                                //	 lds si,dword ptr parg
                                                //	 fild byte ptr ds:[si]
                                                //	 sub sp,4
                                                //	 mov bx,sp
                                                //	 fstp dword ptr ss:[bx]
                                                //	}
                                                //	break;
                                                //   }
                                            case 13 : // float64 -> int16
                                            case 14 : // float64 -> int16
                                            {
                                                asm {
                                                    les si,dword ptr parg
                                                            fld qword ptr es:[si]
                                                                              sub sp,2
                                                                              mov bx,sp
                                                                              fistp word ptr ss:[bx]
                                                                              mov ax,word ptr ss:[bx]
                                                }
                                                      break;
                                                }
                                            case 15 : // float64 -> int32
                                                case 16 : // float64 -> int32
                                                {
                                                    asm {
                                                        les si,dword ptr parg
                                                                fld qword ptr es:[si]
                                                                                  sub sp,4
                                                                                  mov bx,sp
                                                                                  fistp dword ptr ss:[bx]
                                                    }
                                                          break;
                                                    }
                                                case 17 : // float64 -> float32
                                                    {
                                                        asm {
                                                            les si,dword ptr parg
                                                                    fld qword ptr es:[si]
                                                                                      sub sp,4
                                                                                      mov bx,sp
                                                                                      fstp dword ptr ss:[bx]
                                                        }
                                                              break;
                                                        }
                                                    case 18 : // float64 -> float64
                                                        {
                                                            asm {
                                                                les si,dword ptr parg
                                                                        push word ptr es:[si+6]
                                                                                          push word ptr es:[si+4]
                                                                                          push word ptr es:[si+2]
                                                                                          push word ptr es:[si]
                                                            }
                                                                  break;
                                                            }
                                                        case 19 : // float64 -> float80
                                                            {
                                                                asm {
                                                                    les si,dword ptr parg
                                                                            fld qword ptr es:[si]
                                                                                              sub sp,10
                                                                                              mov bx,sp
                                                                                              fstp tbyte ptr ss:[bx]
                                                                }
                                                                      break;
                                                                }
                                                            }
                                                                asm mov es,_rES;
#endif
                                                                //  parg += (Type==1)*GetTSize(argconv[i+1]);
                                                                parg += (Type==1)*size;
                                                            }


                                                            // запускаем функцию
                                                            asm
                                                        {
#if defined(WIN16)
                                                            //  mov ax,ss
                                                            //  mov ds,ax
                                                            call dword ptr [pfun]
                                                                    mov cx,ax
        #else
                                                            call dword ptr [pfun]
                                                                    mov ebx,eax
        #endif
                                                            }

                                                            // если надо удаляем аргументы из стека
                                                            if (Type)
                                                            {
                                                                asm {
                                                                    mov sp,word ptr [lstuck]
                                                                }
                                                            }


                                                            // меняем стек ВМ
                                                            // *stuck += argsize;
                                                            // *stuck -= retsize;
                                                            // pstuck = *stuck;
                                                            pstuck = *stuck = *stuck + argsize - retsize; // осторожно регистры !!!!!!!!!!

                                                            // берем возвращаемое значение
#if defined(WIN32)
                                                            switch (10*(retsize==8) + retconv)
                                                            {
                                                                case 1 :  // int8 -> word32
                                                                case 2 :  // word8 -> word32
                                                                {
                                                                    asm {
                                                                        mov esi,dword ptr pstuck
                                                                                mov byte ptr [esi],bl
                                                                                mov byte ptr [esi+1],0
                                                                                mov word ptr [esi+2],0
                                                                    }
                                                                    break;
                                                                }
                                                                case 3 :  // int16 -> word32
                                                                case 4 :  // word16 -> word32
                                                                {
                                                                    asm {
                                                                        mov esi,dword ptr pstuck
                                                                                mov word ptr [esi],bx
                                                                                mov word ptr [esi+2],0
                                                                    }
                                                                    break;
                                                                }
                                                                case 5 : // int32 -> word32
                                                                case 6 : // word32 -> word32
                                                                {
                                                                    asm {
                                                                        mov esi,dword ptr pstuck
                                                                                mov dword ptr [esi],ebx
                                                                    }
                                                                    break;
                                                                }
                                                                case 7 : // float32 -> word32
                                                                case 8 : // float64 -> word32
                                                                case 9 : // float80 -> word32
                                                                {
                                                                    asm {
                                                                        mov esi,dword ptr pstuck
                                                                                fistp dword ptr [esi]
                                                                    }
                                                                    break;
                                                                }
                                                                case 11 : // int8 -> float64
                                                                case 12 : // word8 -> float64
                                                                case 13 : // int16 -> float64
                                                                case 14 : // word16 -> float64
                                                                {
                                                                    asm {
                                                                        mov esi,dword ptr pstuck
                                                                                mov word ptr [esi],bx
                                                                                fild word ptr [esi]
                                                                                fstp qword ptr [esi]
                                                                    }
                                                                    break;
                                                                }
                                                                case 15 : // int32 -> float64
                                                                case 16 : // word32 -> float64
                                                                {
                                                                    asm {
                                                                        mov esi,dword ptr pstuck
                                                                                mov dword ptr [esi],ebx
                                                                                fild dword ptr [esi]
                                                                                fstp qword ptr [esi]
                                                                    }
                                                                    break;
                                                                }

                                                                case 17 : // float32 -> float64
                                                                case 18 : // float64 -> float64
                                                                case 19 : // float80 -> float64
                                                                {
                                                                    if (Type)
                                                                    {
                                                                        asm {
                                                                            mov esi,dword ptr pstuck
                                                                                    mov dword ptr [esi],ebx
                                                                                    // 	 fstp qword ptr [esi]
                                                                        }
                                                                    } else
                                                                    {
                                                                        asm {
                                                                            mov esi,dword ptr pstuck
                                                                                    mov dword ptr [esi],ebx
                                                                                    //  fstp qword ptr [esi]
                                                                        }
                                                                    }

                                                                    break;
                                                                }
                                                            }

#else
                                                            asm {
                                                            mov _rES,es
                                                            }

                                                            switch (10*(retsize==8) + retconv)
                                                            {
                                                                case 1 :  // int8 -> word32
                                                                case 2 :  // word8 -> word32
                                                                {
                                                                    asm {
                                                                        les si,dword ptr pstuck
                                                                                mov byte ptr es:[si],cl
                                                                                                 inc si
                                                                                                 mov byte ptr es:[si],0
                                                                                                 inc si
                                                                                                 mov word ptr es:[si],0
                                                                    }
                                                                          break;
                                                                    }
                                                                case 3 :  // int16 -> word32
                                                                    case 4 :  // word16 -> word32
                                                                    {
                                                                        asm {
                                                                            les si,dword ptr pstuck
                                                                                    mov word ptr es:[si],cx
                                                                                                     add si,2
                                                                                                     mov word ptr es:[si],0
                                                                        }
                                                                              break;
                                                                        }
                                                                    case 5 : // int32 -> word32
                                                                        case 6 : // word32 -> word32
                                                                        {
                                                                            asm {
                                                                                les si,dword ptr pstuck
                                                                                        mov word ptr es:[si],cx
                                                                                                         add si,2
                                                                                                         mov word ptr es:[si],dx
                                                                            }
                                                                                  break;
                                                                            }
                                                                        case 7 : // float32 -> word32
                                                                            case 8 : // float64 -> word32
                                                                            case 9 : // float80 -> word32
                                                                            {
                                                                                asm {
                                                                                    les si,dword ptr pstuck
                                                                                            fistp dword ptr es:[si]
                                                                                }
                                                                                      break;
                                                                                }
                                                                            case 11: // int8 -> float64
                                                                                case 12: // word8 -> float64
                                                                                {
                                                                                    asm {
                                                                                        les si,dword ptr pstuck
                                                                                                mov byte ptr es:[si],cl
                                                                                                                 mov byte ptr es:[si+1],0
                                                                                                                 fild word ptr es:[si]
                                                                                                                 fstp qword ptr es:[si]
                                                                                    }
                                                                                          break;
                                                                                    }
                                                                                case 13: // int16 -> float64
                                                                                    case 14: // word16 -> float64
                                                                                    {
                                                                                        asm {
                                                                                            les si,dword ptr pstuck
                                                                                                    mov word ptr es:[si],cx
                                                                                                                     fild word ptr es:[si]
                                                                                                                     fstp qword ptr es:[si]
                                                                                        }
                                                                                              break;
                                                                                        }
                                                                                    case 15: // int32 -> float64
                                                                                        case 16: // word32 -> float64
                                                                                        {
                                                                                            asm {
                                                                                                les si,dword ptr pstuck
                                                                                                        mov word ptr es:[si],cx
                                                                                                                         mov word ptr es:[si+2],dx
                                                                                                                         fild dword ptr es:[si]
                                                                                                                         fstp qword ptr es:[si]
                                                                                            }
                                                                                                  break;
                                                                                            }
                                                                                        case 17 : // float32 -> float64
                                                                                            case 18 : // float64 -> float64
                                                                                            case 19 : // float80 -> float64
                                                                                            {
                                                                                                asm {
                                                                                                    les si,dword ptr pstuck
                                                                                                            fstp qword ptr es:[si]
                                                                                                }
                                                                                                      break;
                                                                                                }

                                                                                            }
                                                                                                asm mov es,_rES;

#endif

                                                                                                return 1;
                                                                                            }


                                                                                            INT16 TDLLFunction::Init(HINSTANCE hdll)
                                                                                            {
                                                                                                pproc = GetProcAddress(hdll,RealName?RealName:Name);
                                                                                                if (!pproc)
                                                                                                {
                                                                                                    char s[256];
                                                                                                    wsprintf(s,"function %s init error",Name);
                                                                                                    ShowError(Name,s);
                                                                                                    return 0;
                                                                                                }
                                                                                                return 1;
                                                                                            }

                                                                                            TDLLFunction::TDLLFunction(TFCollection* ptr,ID& id,char* dllname,INT16 deftype)
                                                                                            {
                                                                                                Name = 0; RealName = 0; arg = 0; ret = 0; pType = 0;
                                                                                                pdll = ptr; pproc = 0; pointers = 0;
                                                                                                char tmpname[64];
                                                                                                PClassListInfo pcl;
                                                                                                PClassListInfo buf[64];
                                                                                                C_TYPE i = 0,j;
                                                                                                retsize = retconv = argsize = 0;
                                                                                                Type = deftype;

                                                                                                id.Read();
                                                                                                if (!lstrcmpi(id.str,"CDECL"))
                                                                                                {
                                                                                                    Type = 1;
                                                                                                    id.Read();
                                                                                                } else
                                                                                                {
                                                                                                    if (!lstrcmpi(id.str,"PASCAL"))
                                                                                                    {
                                                                                                        Type = 0;
                                                                                                        id.Read();
                                                                                                    }
                                                                                                }
                                                                                                // имя
                                                                                                if (id.Type != sconst) { ShowError(dllname,"name missed"); return; }
                                                                                                lstrcpy(tmpname,id.str);

                                                                                                // имя в DLL
                                                                                                if (id.Read() == optr && id.str[0] == ',')
                                                                                                {
                                                                                                    if (id.Read() != sconst) { ShowError(dllname,"real name missed"); return; }
                                                                                                    RealName = NewStr(id.str);
                                                                                                    id.Read();
                                                                                                } else RealName = NewStr(Name);

                                                                                                // аргументы
                                                                                                if (!lstrcmpi(id.str,"arg"))
                                                                                                {
                                                                                                    do
                                                                                                    {
                                                                                                        if (id.Read() == optr && id.str[0] == '&') // ссылка !
                                                                                                        {
                                                                                                            pointers|=1<<i; id.Read();
                                                                                                        }
                                                                                                        if (id.Type != sconst) { ShowError(dllname,"syntax error"); return; }
                                                                                                        if ( (pcl=GetClassListInfo(id.str)) != 0)
                                                                                                        {
                                                                                                            buf[i++] = pcl;
                                                                                                        }
                                                                                                        else
                                                                                                        {
                                                                                                            char s[256];
                                                                                                            wsprintf(s,"Unknown type: %s detected !",id.str);
                                                                                                            ShowError(dllname,s);
                                                                                                            return;
                                                                                                        }
                                                                                                        if (!lstrcmpi(id.str,"FLOAT")) { argsize += 8; argconv[i] = 8; }// общий размер аргументов
                                                                                                        else { argsize += 4; argconv[i] = 6; }

                                                                                                        // может есть конверсия ?
                                                                                                        if (id.Read() == optr && id.str[0] == ':')
                                                                                                        {
                                                                                                            argconv[i] = GetCType(id);
                                                                                                            id.Read();
                                                                                                        }

                                                                                                    } while (id.Type == optr && id.str[0] == ',');

                                                                                                    pType = new PClassListInfo[i]; // <- можно упростить !
                                                                                                    for (j = 0; j < i; j++) pType[j] = buf[j];

                                                                                                    arg = i;
                                                                                                }

                                                                                                if (!lstrcmpi(id.str,"ret"))
                                                                                                {
                                                                                                    if (id.Read() != sconst) { ShowError(dllname,"syntax error"); return; }
                                                                                                    if ( (pcl=GetClassListInfo(id.str)) != 0)
                                                                                                    {
                                                                                                        ret = pcl;
                                                                                                        if (!lstrcmpi(id.str,"FLOAT")) { retsize = retconv = 8; }
                                                                                                        else  { retsize = 4; retconv = 5; }
                                                                                                    }
                                                                                                    else
                                                                                                    {
                                                                                                        char s[256];
                                                                                                        wsprintf(s,"Unknown type: %s detected !",id.str);
                                                                                                        ShowError(dllname,s);
                                                                                                        return;
                                                                                                    }
                                                                                                    // может есть конверсия ?
                                                                                                    if (id.Read() == optr && id.str[0] == ':')
                                                                                                    {
                                                                                                        retconv = GetCType(id);
                                                                                                        id.Read();
                                                                                                    }
                                                                                                }
                                                                                                Name = NewStr(tmpname);
                                                                                            }

                                                                                            INT16 TDLLFunction::GetCType(ID& id)
                                                                                            {
                                                                                                if (id.Read() == optr && id.str[0] == '&') // ссылка !
                                                                                                {
                                                                                                    return 5;
                                                                                                }
                                                                                                if (id.Type != sconst) { ShowError("","type missed"); return 0; }
                                                                                                if (!lstrcmpi(id.str,"word8"))  return 1;
                                                                                                if (!lstrcmpi(id.str,"int8"))   return 2;
                                                                                                if (!lstrcmpi(id.str,"word16")) return 3;
                                                                                                if (!lstrcmpi(id.str,"int16"))  return 4;
                                                                                                if (!lstrcmpi(id.str,"word32")) return 5;
                                                                                                if (!lstrcmpi(id.str,"int32"))  return 6;
                                                                                                if (!lstrcmpi(id.str,"float32")) return 7;
                                                                                                if (!lstrcmpi(id.str,"float64")) return 8;
                                                                                                if (!lstrcmpi(id.str,"float80")) return 9;

                                                                                                char s[256];
                                                                                                wsprintf(s,"Unknown type: %s detected !",id.str);
                                                                                                ShowError("",s);
                                                                                                return 0;
                                                                                            }

                                                                                            //------------------------------------------------------------------------------
                                                                                            //------------------------------------------------------------------------------

                                                                                            HINSTANCE extern hInstance;
#include "twindow.h"
#include <stratum\sc_msg.h>
                                                                                            //------------------------------------------------------------------------------
                                                                                            //------------------------------------------------------------------------------

                                                                                            HINSTANCE ScLoadLibrary(char *src,TProject*prj,char *addpath)
                                                                                            {
                                                                                                BOOL _try=3;
                                                                                                char dst[260];
                                                                                                int code1,code2;
                                                                                                HINSTANCE hdll=0;
                                                                                                lstrcpy(dst,src); // Сначала грузим в путях по умолчанию

                                                                                                while (_try>0){
                                                                                                    hdll = LoadLibrary(dst);
#ifdef WIN32
                                                                                                    if(_try==3)code1=GetLastError();else code2=GetLastError();
#else
                                                                                                    if ((UINT) hdll <= 32){
                                                                                                        if(_try==3)code1=(UINT) hdll;else code2=(UINT) hdll;
                                                                                                        hdll=0;}
#endif
                                                                                                    if(hdll)break;
                                                                                                    switch(_try){
                                                                                                        case 3: GetFullFileNameEx(prj,src,dst);break;
                                                                                                        case 2: if(addpath){
                                                                                                                lstrcpy(dst,addpath);
                                                                                                                AddSlash(dst);lstrcat(dst,src);
                                                                                                                break;
                                                                                                            }
                                                                                                    }
                                                                                                    _try--;
                                                                                                }
                                                                                                if(!hdll){
                                                                                                    wsprintf(dst,"dll load error (%d:%d)",code1,code2);
                                                                                                    ShowError(src,dst);
                                                                                                }
                                                                                                return hdll;
                                                                                            }

                                                                                            extern "C" UINT32 _SDKPROC ScDllProc(UINT16  msg,UINT32 p1,UINT32 p2);

                                                                                            INT16 TFCollection::Init()
                                                                                            {
                                                                                                if (hdll) return 1;
                                                                                                char _path[260]="";
                                                                                                if(tdl){
                                                                                                    GetPath(tdl,_path);
                                                                                                }

                                                                                                hdll = ScLoadLibrary(dllname,project,_path);
                                                                                                if(hdll)
                                                                                                {
                                                                                                    (void*)_proc=GetProcAddress(hdll,"ScDllProc");
                                                                                                    if(_proc){
                                                                                                        // Инициализация
                                                                                                        TSTRATUMINFO info;
                                                                                                        info.size=sizeof(info);
                                                                                                        info.version=0x0300;
                                                                                                        info.hInstance=hInstance;
                                                                                                        info.hModule=hdll;
                                                                                                        info.hMainWindow=mainwindow->HWindow;
                                                                                                        info.ScProc=ScDllProc;
                                                                                                        info.codepage=SCcodepage;
                                                                                                        Procedure(SC_INITLIBRARY,(UINT32)&info,0);
                                                                                                        // Инициализация
                                                                                                    }
                                                                                                    for (INT16 i=0; i<count; i++)
                                                                                                    {
                                                                                                        if (!((TDLLFunction*)items[i])->Init(hdll)) return 0;
                                                                                                    }
                                                                                                }
                                                                                                else {
                                                                                                    return 0; }
                                                                                                return 1;
                                                                                            }
                                                                                            INT16 TFCollection::Free()
                                                                                            {
                                                                                                if ((UINT) hdll > 32){
                                                                                                    Procedure(SC_DONELIBRARY,0,0);
                                                                                                    FreeLibrary(hdll);
                                                                                                }
                                                                                                return 1;
                                                                                            }

                                                                                            C_TYPE TFCollection::SSearch(char* key,C_TYPE& index)
                                                                                            {
                                                                                                C_TYPE c;
                                                                                                for (C_TYPE i=0;i<count;i++)
                                                                                                {
                                                                                                    c=lstrcmpi(key,((TDLLFunction*)items[i])->Name);
                                                                                                    if (c<=0) {index=i; return c==0; }
                                                                                                }
                                                                                                index=count;
                                                                                                return 0;
                                                                                            };

                                                                                            INT16 TFCollection::Load(ID& id,INT16 tp)
                                                                                            {
                                                                                                while (!lstrcmpi(id.str,"name"))
                                                                                                {
                                                                                                    TDLLFunction* fn = new TDLLFunction(this,id,dllname,tp);
                                                                                                    if (!fn->Name) { delete fn; return 0; }
                                                                                                    if (Insert(fn)<0)
                                                                                                    {
                                                                                                        char s[256];
                                                                                                        wsprintf("Duplicated function - (%s)",id.str);
                                                                                                        ShowError(dllname,s);
                                                                                                        return 0;
                                                                                                    }
                                                                                                }
                                                                                                return 1;
                                                                                            }

                                                                                            void TFCollection::FreeItem(pointer p)
                                                                                            {
                                                                                                delete (TDLLFunction*)p;
                                                                                            };

                                                                                            int TFCollection::Compare(pointer key1,pointer key2)
                                                                                            {
                                                                                                C_TYPE i = lstrcmpi(((TDLLFunction*)key1)->Name,((TDLLFunction*)key2)->Name);
                                                                                                if (i != 0) return i;
                                                                                                if (((TDLLFunction*)key1)->ret != ((TDLLFunction*)key2)->ret)
                                                                                                {
                                                                                                    if (((TDLLFunction*)key1)->ret > ((TDLLFunction*)key2)->ret) return 1;
                                                                                                    else return -1;
                                                                                                }
                                                                                                if (((TDLLFunction*)key1)->arg != ((TDLLFunction*)key2)->arg)
                                                                                                {
                                                                                                    if (((TDLLFunction*)key1)->arg > ((TDLLFunction*)key2)->arg) return 1;
                                                                                                    else return -1;
                                                                                                }
                                                                                                for (i = 0; i < ((TDLLFunction*)key1)->arg; i++)
                                                                                                {
                                                                                                    if (((TDLLFunction*)key1)->pType[i] != ((TDLLFunction*)key2)->pType[i])
                                                                                                    {
                                                                                                        if (((TDLLFunction*)key1)->pType[i] > ((TDLLFunction*)key2)->pType[i]) return 1;
                                                                                                        else return -1;
                                                                                                    }
                                                                                                }
                                                                                                return 0;
                                                                                            };

                                                                                            //------------------------------------------------------------------------------
                                                                                            //------------------------------------------------------------------------------

                                                                                            TDLLManager::TDLLManager()
                                                                                            {
                                                                                                dlls = new TCollection(10,10);
                                                                                                messages=NULL;
                                                                                            };

                                                                                            TDLLManager::~TDLLManager()
                                                                                            {
                                                                                                for (C_TYPE i=0;i<dlls->count;i++)
                                                                                                {
                                                                                                    ((TFCollection*)dlls->At(i))->Free();
                                                                                                    DeleteCollection((TFCollection*)dlls->At(i));
                                                                                                }
                                                                                                if (dlls) DeleteCollection(dlls);
                                                                                                if(messages){
                                                                                                    for(i=0;i<messages->count;i++){
                                                                                                        char *_n=(char*)messages->At(i);
                                                                                                        if(_n)delete _n;
                                                                                                        messages->AtPut(i,0);
                                                                                                    }
                                                                                                    DeleteCollection(messages);
                                                                                                }
                                                                                            };

                                                                                            TFCollection* TDLLManager::Search(char* dllname)
                                                                                            {
                                                                                                TFCollection* ptr;
                                                                                                if (!dlls) return 0;

                                                                                                for (C_TYPE i=0;i<dlls->count;i++)
                                                                                                {
                                                                                                    ptr = (TFCollection*)dlls->At(i);
                                                                                                    if (!lstrcmpi(ptr->dllname,dllname)) return ptr;
                                                                                                }
                                                                                                return 0;
                                                                                            }

                                                                                            INT16 TDLLManager::LoadDLL(char* filename,DWORD data)
                                                                                            {
                                                                                                INT16 t;
                                                                                                INT16 pr;
#if defined(WIN32)
                                                                                                INT16 tp = 1;  // CDECL
#else
                                                                                                INT16 tp = 0;  // PASCAL
#endif

                                                                                                char dllname[MAX_STRING];
                                                                                                TFCollection* dll;
                                                                                                TDOSStream tpl(filename,TDOSStream::stOpenRead);
                                                                                                if (tpl.status) { ShowError(dllname,"can't open file"); return 0; }
                                                                                                ID id(tpl);

                                                                                                id.Read();
                                                                                                while (!lstrcmpi(id.str,"dll"))
                                                                                                {
                                                                                                    // имя DLL
                                                                                                    if (id.Read() != sconst) { ShowError(dllname,"syntax error"); return 0; }
                                                                                                    lstrcpy(dllname,id.str);

                                                                                                    // тип DLL
                                                                                                    if (id.Read() != dconst) { ShowError(dllname,"dll type missed"); return 0; }
                                                                                                    t = atoi(id.str);

                                                                                                    // пошукаем
#if defined(WIN32)
                                                                                                    if (t == 16) if (!id.Wait("dll")) return 0; else continue;
#else
                                                                                                    if (t == 32) if (!id.Wait("dll")) return 0; else continue;
#endif

                                                                                                    // а нет ли уже такой ?
                                                                                                    if (Search(dllname)) { ShowError(dllname,"already loaded"); return 0; }

                                                                                                    id.Read();
                                                                                                    if (!lstrcmpi(id.str,"preload")) { pr = 1; id.Read(); }
                                                                                                    else pr = 0;

                                                                                                    // создаем новую коллекцию
                                                                                                    dll = new TFCollection(dllname,data);
                                                                                                    dll->tdl=NewStr(filename);
                                                                                                    if (!dll->Load(id,tp)) return 0;
                                                                                                    dlls->Insert(dll);
                                                                                                    if (pr) dll->Init();
                                                                                                }

                                                                                                if (!id.IsEOF()) { ShowError(dllname,"syntax error"); return 0; }
                                                                                                return 1;
                                                                                            };
                                                                                            TFCollection::~TFCollection(){
                                                                                                if(tdl)delete tdl;

                                                                                            };
                                                                                            INT16 TDLLManager::InitDLL(DWORD data)
                                                                                            {
                                                                                                INT16 cr = 0;
                                                                                                TFCollection* ptr;
                                                                                                if (!dlls) { ShowError("","incorrect call"); return 0; }

                                                                                                for (C_TYPE i=0;i<dlls->count;i++)
                                                                                                {
                                                                                                    ptr = (TFCollection*)dlls->At(i);
                                                                                                    if (ptr->data == data)
                                                                                                    {
                                                                                                        if (!ptr->Init()) return 0;
                                                                                                        cr ++;
                                                                                                    }
                                                                                                }
                                                                                                return cr;
                                                                                            };

                                                                                            UINT32 TFCollection::Procedure(UINT16 code,UINT32 p1,UINT32 p2){
                                                                                                if(_proc)return _proc(code,p1,p2);
                                                                                                return 0;
                                                                                            };


                                                                                            BOOL TDLLManager::FreeDLL(TFCollection* ptr,BOOL)
                                                                                            {
                                                                                                if (!dlls) { ShowError("","incorrect call"); return 0; }
                                                                                                C_TYPE i=dlls->IndexOf(ptr);
                                                                                                if(i<0)return FALSE;

                                                                                                ptr->Free();
                                                                                                DeleteCollection(ptr);
                                                                                                dlls->AtDelete(i);
                                                                                                return TRUE;
                                                                                            };


                                                                                            INT16 TDLLManager::FreeDLL(DWORD data)
                                                                                            {
                                                                                                INT16 cr = 0;
                                                                                                TFCollection* ptr;
                                                                                                if (!dlls) { ShowError("","incorrect call"); return 0; }

                                                                                                C_TYPE i=0;
                                                                                                while(i<dlls->count)
                                                                                                {
                                                                                                    ptr = (TFCollection*)dlls->At(i);
                                                                                                    if (ptr->data == data)
                                                                                                    {
                                                                                                        ptr->Free();
                                                                                                        DeleteCollection(ptr);
                                                                                                        dlls->AtDelete(i);
                                                                                                        cr++;
                                                                                                    }else i++;
                                                                                                }
                                                                                                return cr;
                                                                                            };

                                                                                            TDLLFunction* TDLLManager::SearchFunction(char* fname)
                                                                                            {
                                                                                                C_TYPE ix;
                                                                                                TFCollection* ptr;
                                                                                                if (!dlls) return 0;
                                                                                                for (C_TYPE i=0;i<dlls->count;i++)
                                                                                                {
                                                                                                    ptr = (TFCollection*)dlls->At(i);
                                                                                                    if (ptr->SSearch(fname,ix)) return (TDLLFunction*)ptr->At(ix);
                                                                                                }
                                                                                                return 0;
                                                                                            };

                                                                                            TFCollection* TDLLManager::LoadPlugin(char*dllname, DWORD data)
                                                                                            {
                                                                                                TFCollection* dll = Search(dllname);
                                                                                                if (dll)
                                                                                                {
                                                                                                    if(!dll->hdll)
                                                                                                        dll->Init();
                                                                                                    return dll;
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                    dll = new TFCollection(dllname,data);
                                                                                                    dlls->Insert(dll);
                                                                                                    dll->Init();
                                                                                                    if(dll->hdll)
                                                                                                        return dll;
                                                                                                    FreeDLL(data);
                                                                                                }
                                                                                                return 0;
                                                                                            };

                                                                                            BOOL TDLLManager::ForEach(UINT16 code,UINT32 p1,UINT32 p2){
                                                                                                if (!dlls) return 0;
                                                                                                TFCollection* ptr;
                                                                                                for (C_TYPE i=0;i<dlls->count;i++)
                                                                                                {
                                                                                                    ptr = (TFCollection*)dlls->At(i);
                                                                                                    if(ptr->_proc)ptr->_proc(code,p1,p2);
                                                                                                }
                                                                                                return TRUE;
                                                                                            };

                                                                                            BOOL TDLLManager::FirstThat(UINT16 code,UINT32 p1,UINT32 p2){
                                                                                                if (!dlls) return 0;
                                                                                                TFCollection* ptr;
                                                                                                for (C_TYPE i=0;i<dlls->count;i++)
                                                                                                {
                                                                                                    ptr = (TFCollection*)dlls->At(i);
                                                                                                    if(ptr->_proc){
                                                                                                        if(ptr->_proc(code,p1,p2)==0)return FALSE;
                                                                                                    }
                                                                                                }
                                                                                                return FALSE;
                                                                                            };
                                                                                            extern "C" _SDKPROC int SCRegisterMessage(char *name){
                                                                                                if(name){
                                                                                                    if(!dllmanager->messages)dllmanager->messages=new TCollection(10,10);
                                                                                                    for(C_TYPE i=0;i<dllmanager->messages->count;i++){
                                                                                                        char * _n=(char*)dllmanager->messages->At(i);
                                                                                                        if(_n){
                                                                                                            if(!lstrcmp(_n,name))return i+16384;
                                                                                                        }else{
                                                                                                            dllmanager->messages->AtPut(i,NewStr(name));
                                                                                                            return i+16384;
                                                                                                        }
                                                                                                    }
                                                                                                    if(dllmanager->messages->count<8192){
                                                                                                        return dllmanager->messages->Insert(NewStr(name))+16384;
                                                                                                    }
                                                                                                }
                                                                                                return 0;
                                                                                            }



                                                                                            TDLLManager * dllmanager=NULL;

                                                                                            //BOOL SCSpaceDialog(HWND hwnd,HSP2D,int mode,TGrid*grid,PClass _class=NULL,PObject _pobject=NULL);


