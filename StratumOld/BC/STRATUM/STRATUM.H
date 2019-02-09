/*
Copyright (c) 1996,1997  Laboratory of United Modeling Knowledjes

Project: Stratum for Windows

Module Name:
			stratum.h

Author:
			Alexander Shelemekhov

Description:
		 functions exported by SCcoreXX.exe
*/


#if !defined(SC_FUNCTIONS_HH)
#define SC_FUNCTIONS_HH

#include <stratum\sccall.h>
#include <stratum\sc_types.h>
#include <stratum\sc_msg.h>
#if defined(STRATUM_INTERNAL)
 #define STRATUMPROC(proc) SDKPROC proc
#else

#ifdef STRATUM_DYNAMIC
 #define STRATUMPROC(proc) extern (SDKPROC * proc)
#else
 #define STRATUMPROC(proc) SDKPROC proc
#endif
#endif

#define CLIP_SCOBJ "ALSHL Classes and objects"
#define CLIP_SCCLS "ALSHL Classes and Librarys"

struct TSTRATUMINFO;
struct TSTRATUMVERSION;
struct TSTRATUMWINDOW;
extern "C"{

/*
#ifndef STRATUM_INTERNAL
// Используются загрузчиком
BOOL   STRATUMPROC(ScInitCore)(TSTRATUMINFO*);
void   STRATUMPROC(ScDoneCore)();
BOOL   STRATUMPROC(ScProcessCmdLine)(char **_argv,int _argc);
void   STRATUMPROC(ScMessageLoop)();
void   STRATUMPROC(ScPostInit)();
void   STRATUMPROC(SCVersion)(TSTRATUMVERSION*);
#endif
*/
// Используются загрузчиком


PObject        STRATUMPROC(SCGetObject)(char * path); // Only in call @

LPSTR          STRATUMPROC(SCVm_InsertString)(char*str,int size);
/* Если str==NULL выделяется буфер под строку размером (пока равный 256)      @
   и возвращается указатель на него (он не очищается).
   иначе выделяется буфер и в него копируется строка, и возвращается указатель на него
*/

DWORD          STRATUMPROC(SCInstallCallBack)(UINT16,void*,void*);
BOOL           STRATUMPROC(SCUnInstallCallBack)(DWORD);


void           STRATUMPROC(SCSetStatusProgressBar)(int id,int percent);
void           STRATUMPROC(SCSetStatusText)(int id,char*txt);

BOOL STRATUMPROC(SCSetScVarS)(char*,char*);
BOOL STRATUMPROC(SCSetScVarI)(char*,INT32);
BOOL STRATUMPROC(SCSetScVarD)(char*,double);

BOOL STRATUMPROC(SCGetScVarS)(char*,char*);
BOOL STRATUMPROC(SCGetScVarI)(char*,INT32&);
BOOL STRATUMPROC(SCGetScVarD)(char*,double&);
int  STRATUMPROC(SCGetScVar)(char*,int def);


// Следующие достают параметры среды( а не текущего проекта)
BOOL STRATUMPROC(SC_GetScVarS)(char*,char*);
BOOL STRATUMPROC(SC_GetScVarI)(char*,INT32&);
BOOL STRATUMPROC(SC_GetScVarD)(char*,double&);
int  STRATUMPROC(SC_GetScVar)(char*,int def);
BOOL STRATUMPROC(SCDeleteScVar(char*));

int  STRATUMPROC(SCRegisterMessage(char*));
BOOL STRATUMPROC(SCGetWindowInfo(HWND,TSTRATUMWINDOW*));

PProject       STRATUMPROC(SCGetObjectProject)(PObject);
PClassListInfo STRATUMPROC(SCGetClassListInfo)(char *);     //@
PClass         STRATUMPROC(SCGetClass)(char *);             //@
PClass         STRATUMPROC(SCCreateNewClass)(char* name,char *text,char * scheme,TLibrary*lib);
PClass         STRATUMPROC(SCCreateClassByClass)(char* name1,char* name2,TLibrary*lib=NULL);
BOOL           STRATUMPROC(SCChangeClassName)(PClass,char *name);

BOOL           STRATUMPROC(SCSetClassText)(PClass _class,char *txt);
LPSTR          STRATUMPROC(SCGetClassText)(PClass _class);

PClass         STRATUMPROC(SCAddChildren)(PClass _class,char* name,HOBJ2D*id,gr_float xpos,gr_float ypos);
BOOL           STRATUMPROC(SCRemoveChildren)(PClass,HOBJ2D id,BOOL notconfirm=FALSE,BOOL updatememory=TRUE);

BOOL           STRATUMPROC(SCAddLink)(PClass,PLinkInfo);
BOOL           STRATUMPROC(SCRemoveLink)(PClass,HOBJ2D id,BOOL fromscheme=TRUE);
BOOL           STRATUMPROC(SCModifyLink)(PClass,HOBJ2D id,PLinkInfo);
int            STRATUMPROC(SCDeleteClass)(char* name);

BOOL           STRATUMPROC(SCSaveSetState)(PObject po,char*name);
BOOL           STRATUMPROC(SCLoadSetState)(PObject po,char*name);


INT16          STRATUMPROC(SCGetFullObjectName)(PObject po,INT16*,INT16 size);
/* @ Позволяет запомнить положение объекта в иерархии
   Если size==0 то возвращается количество элементов типа INT16 необходимых
   для сохранения информации.
   Имеено ЭТО значение и должно использоватья при повторном вызове этой
   функции
*/

PObject        STRATUMPROC(SCGetObjectByFullName)(PProject,INT16*,INT16 size);
/* @ По запомненному пути производит возвращение объекта
   Если 0 то объект был удален
*/
void           STRATUMPROC(SCMessage)(char*);
/* @ Поместить сообщение в окно Message */


// * выделить и удалить память в среде
pointer        STRATUMPROC(SCGetMem)(int);
pointer        STRATUMPROC(ScFreeMem)(void*);

HWND STRATUMPROC(SCCreateToolPopup)(TSTRATUMTOOLBAR*);
HWND STRATUMPROC(SCCreatePropertySheet)(HWND parent,char*title,TPropertyButton*b,int active,HINSTANCE hr,LPCSTR _template);
BOOL STRATUMPROC(SCDestoyToolPopup)(int id);
BOOL STRATUMPROC(SCToolPopupVar)(int tid,int vid,BOOL set,void*);

// Диалоги
BOOL STRATUMPROC(SCFileDialog)       (HWND hwnd,char * szFile,char * szFilter,BYTE _flags);// _flags |1 -save
BOOL STRATUMPROC(SCObject2dDialog)   (HWND hwnd,HSP2D hsp,HOBJ2D object);
BOOL STRATUMPROC(SCObject3dDialog)   (HWND hwnd,HSP3D hsp,HOBJ2D object);
BOOL STRATUMPROC(SCTextureDialog)    (HWND hwnd,HSP3D,HOBJ2D);
BOOL STRATUMPROC(SCSpacePrintDialog) (HWND hwnd,HSP2D);
BOOL STRATUMPROC(SCChoseColor)       (HWND hwnd,COLORREF & clr,char*title);
BOOL STRATUMPROC(SCAboutDialog)      (HWND hwnd,int=0);
BOOL STRATUMPROC(SCMatrixDialog)     (HWND hwnd,int matrix);
BOOL STRATUMPROC(SCChoseFontDialog)  (HWND parent,LOGFONT& lf,char * title);
BOOL STRATUMPROC(SCSysInfoDialog)    (HWND hwnd);
BOOL STRATUMPROC(SCEnvironmentDialod)(HWND hwnd);
BOOL STRATUMPROC(SCProjectDialog)    (HWND hwnd,TProject*project);

int  STRATUMPROC(SCIsAbsolutePath)   (char *file);
// 0 - Ошибка
// 1 - Absolute
// 2 - relative
void STRATUMPROC(SCAddSlash)           (char *p);
};
#if !defined(STRATUM_INTERNAL)
#undef  STRATUMPROC
#define STRATUMPROC(proc) (SDKPROC * proc)

#endif
#endif
