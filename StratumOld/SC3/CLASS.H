/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
                        class.h
Author:
                        Alexander Shelemekhov
*/
#ifndef __CLASS__SC3_H_
#define __CLASS__SC3_H_
#include "collect.h"
#include "messages.h"
#include "../BC/bc_include_gwin2d3d/GWINTYPE.H"
#include "../BC/bc_include_stratum/SC_TYPES.H"
#define UD_GRID     1
#define DISABLE_VAR_NAME "_disable"
#define ENABLE_VAR_NAME "_enable"
//#include "equ.h"

class TProject;
class TStream;
class TClass;
class TObject;
class TLink;
class TEqu;
struct TVarInfo;
class  TLinkInfo;
struct TLinkEntryInfo;
struct TChildInfo;
struct TClassListInfo;
struct TVarLink;
struct TLibrary;
class  TIcon;
class  TScheme;
class  TClassMSG;
class  TMemoryManager;
class  TVarData;
class  TProject;

typedef TProject* PProject;
typedef TClass* PClass;
typedef TObject* PObject;
typedef TLink*PLink;
typedef TClassMSG* PClassMSG;
typedef TVarLink* PVarLink;
typedef TVarInfo* PVarInfo;
typedef TLinkInfo* PLinkInfo;
typedef TLinkEntryInfo* PLinkEntryInfo;
typedef TChildInfo* PChildInfo;
typedef TScheme* PScheme;
typedef TIcon* PIcon;
typedef TClassListInfo* PClassListInfo;
typedef TLibrary* PLibrary;
typedef TVarData* PVarData;

#define CLF_LOADED   1
#define CLF_PROJECT  2
#define CLF_MODIFY   4
#define CLF_READONLY 8
#define CLF_NORECURSE 16
#define CLF_ADDLIB    32
#define CLF_CURRENTPROJECT 64
#define CLF_LIBRARY   256
#define CLF_PACKFILE  512       // библиотека является PACK файлом или класс в PACK файле

/*
 Это были флаги для TClassListInfo
*/

#define CF_PROTECTED     1
#define CF_ONEFASE       2
#define CF_COMPILED      4
#define CF_VIRTUAL       8      // Сгенерированн компилятором
#define CF_SCHEME       16      // Имеет схему
#define CF_MODIFY       32      // класс был изменен
#define CF_HAS_DISABLE_VAR     64      // объект класса может быть запрещен
#define CF_BUILDIN     256
#define CF_ICON        512
#define CF_PROCEDURE  1024
#define CF_STRUCTURE  2048
#define CF_USEDEFICON 4096
#define CF_SIZEABLE   8192
#define CF_HAS_ENABLE_VAR 16384
#define CF_NOSAVEVAR  32768ul
#define CF_AUTOLINK   0x10000L
#define CF_AUTORMLINK 0x20000L



#define TCF_WASSAVE  1       // Был записан как новый
#define TCF_NEWCLASS 2       // Был создан

#define OF_DISABLED  1
#define OF_ONEFASE   2
#define OF_ERROR     4

#define LF_DISABLED  1
#define LF_CORRECTPOS 0x8000ul
#define LF_AUTOLINK   0x0002  // созданна автоматически

#define VF_LOCAL     2
#define VF_NOREAD    4
#define VF_NOWRITE   8
#define VF_STATIC    16
#define VF_ONSCHEME  32
#define VF_PARAMETER 64
#define VF_LEFT      128
#define VF_EQVAR     256
#define VF_ARGUMENT  512
#define VF_RETURN    1024
#define VF_NOSAVE    2048
#define VF_GLOBAL    4096
#define VF_CLASSGLOBAL 8192

#define VF_NOTFIXED   0x10000l
#define VF_BYCOMPILER 0x20000l

#define SF_EXTERNAL  1
#define SF_EDITING   2

#define STATE_FAST   0 // Если вызванна из шага то записываются и считываются оба состояния.
#define STATE_VARS   1 // Запись как FAST, но для заданного класса записывается
// информация о переменных, типах и т. д
#define STATE_DEBUG  2
#define STATE_SET    3

// для TChildInfo.flags
#define CHF_NEEDCREATE   1
#define CHF_NEEDUPDATE   2
#define CHF_WASSIZED     4    // был изменен размер
#define CHF_PREPROCESS   8
#define CHF_POSTPROCESS 16

#define IF_CORRECTED 1

#define EF_RUNNING    0x01
#define EF_MUSTSTOP   0x02  // после шага все чистится ClearAll
#define EF_STEPACTIVE 0x04  // Сейчас идет шаг
#define EF_EQUACTIVE  0x08  // Сейчас идет вычисление уравнений
#define EF_MSGACTIVE  0x10  // Сейчас идет обработка сообщений
#define EF_BYTIMER    0x20  // Вычисления идут по таймеру
#define EF_ONDIALG    0x40L  // Сейчас идет обработка диалога
//#define EF_EQUVALID   0x100


PClass extern changedclass;
DWORD  extern EXEC_FLAGS;
UINT32 extern calcedsteps;
PLibrary extern DefaultLib;
UINT32  extern ulinkerror;  // ошибки при раздаче линков (1 - нет имиджей |2- не те типы|4 -нет переменных|)
UINT32  extern changecount; // счетчик изменений ()
PClass  extern currentclass; // активный при открытии класс 

BOOL Running();
void   Run();
void   DoneRun();
void   Stop();


class TSetVar
{
public:
    TSetVar(char*, char*);
    TSetVar(TStream&);
    ~TSetVar();

public:
    TSetVar* next;
    char* name;
    char* data;
};

class TMSGItems : public TCollection
{
    HWND HWindow;
public:
    HSP2D hsp;
    int   lock;

    TMSGItems(HWND _HWindow,HSP2D _hsp);
    ~TMSGItems();

    void  Update();
    void  Register(TClassMSG * msg);
    void  UnRegister(HOBJ2D obj,UINT16 code);
    inline TClassMSG*  msg(C_TYPE i) { return (TClassMSG*)At(i); }
    void virtual FreeItem(pointer p);
    LRESULT WndProc(UINT message,WPARAM wParam,LPARAM lParam,BOOL &process);
    BOOL RegisterObject(PObject po,HOBJ2D obj,INT16 code,UINT32 flags);
    void DoneHandler(void*project,UINT msg=WM_DESTROY);
};

// Это определяет как записывать состояние
class TClassMSG
{
public:
    struct  TVAR
    {
        char * name;
        void * data;
        PClassListInfo type;
    };

    TProject* project;
    PObject pobject;       // Указатель на объект

    INT16   code;
    HOBJ2D  object;
    HOBJ2D* path;
    UINT16  flags;
    BYTE    lflags; // флаги выполнения
    INT16   varcount;
    INT16   pathcount;
    double* message;
    TVAR*   vars;

//    TClassMSG(PObject po, int _code, char * data);

    //Копирование данных переменных сообщения из объекта
    BOOL    Update(INT16 code);
    void    Send(INT16 code,void *data,BOOL ret);
    void    InvalidType(char *s,PClassListInfo pcli);
    void    NotFound(char *s);
};

#define CMSG_INPROCESS  1 // идет обработка сообщения
#define CMSG_MUSTREMOVE 2 // должно быть удаленно после выполнения

class   TIcon
{
public:
    WORD  flags;
    char * bits;

    TIcon(PClass _class);
    TIcon(PClass _class,TStream&st);
    ~TIcon();

    void Update(PClass _class);
    void Store(TStream&st);
};

TClassMSG * CreateMSG(PObject,HOBJ2D obj,INT16 code,UINT16 flags,char*data);

char extern scheme_path[128];
char extern * ini_file;

struct TLibrary
{
    UINT32 flags;
    TLibrary* parent;
    char* path;
    TCollection* items;
    //void * project;
    INT32 Ref;

    TLibrary(char*, TLibrary* parent);
    TLibrary(TLibrary* parent, TStream*, char*, BYTE);
    ~TLibrary();

    void  GetPath(char*);
    INT16 GetClassCount();

    void  Insert(TLibrary*);

    INT32 AddRef();
    INT32 DecRef();
    void  PostDeleteChild();

    BOOL  IsContain(PClassListInfo);
};

struct TClassListInfo
{
    UINT32 flags;
    TLibrary *library; // renamed from 'parent'
    char * filename; // имя файла
    union
    {
        char * classname; // либо имя,
        TClass * _class; //  либо класс
    };
    long posinpack,packsize;

    TClassListInfo(TLibrary*library, TStream*st, char *fileNameWithoutExt);
    TClassListInfo();

    BOOL GetFileName(char*,BOOL forsave=TRUE);
    PProject GetProject();
    char* GetClassName();
    PClass GetClass();
    BOOL isFLOAT();
    BOOL isINTEGER();
    BOOL isSTRING();
    BOOL isWORD();
    BOOL isBYTE();
    BOOL isHANDLE();
    BOOL isRGB();
};

class TClass
{
public:
    char*     name;       // имя
    PClassListInfo listinfo;
    DWORD     flags;      // флаги
    PObject   first;      // указатель на первый объект этого класса
    char*     description;// описание класса в текстовом виде
    UINT32    _classid;   // Уникальный номер класса
    long      varsize;    // размер всех переменных(1 состояние) в байтах
    // для float -8, для long 4.
    /* Переменные */
    INT16     varcount;   // число переменных
    PVarInfo  vars;
    INT16     disablevar; // индекс переменной

    /* Связи  */
    INT16     linkcount;  // число линков
    PLinkInfo link;

    /* Чилды */
    INT16     childcount; // число детей
    PChildInfo childs;

    /* Код */
    char*     text;       // исходный текст присваиваний и уравнений
    long      codesize;   // виртуальный код
    WORD *    VM_code;
    TEqu *    equ;
    BYTE *    moveinfo;   // то-же

    BYTE      t_flags;
    PIcon     icon;       // иконка
    INT16     deficon;    // Номер иконки по умолчанию
    char *   IconName;   // Имя иконки (если 0 то default.dbm)
    char *   helpfile;   // Имя help файла

    PScheme   scheme;     // схема класса
    PScheme   image;      // двухмерное изображение класса
    BYTE      functype;   // тип функции (0)

    struct
    {
        unsigned  text:1;
        unsigned  image:1;
        unsigned  scheme:1;
        unsigned  description:1;
    }NS;

    static UINT32 fileversion;

    // metods
    TClass(TStream&,UINT16 InitFlags=0);
    TClass(char*name, char*text);
    ~TClass();

    void UpdateIcon();
    //void SetCode(int*_code,long _size);
    void Minimize();
    void SetCode(TStream*);
    void SetEqu(TEqu*);
    void Store(TStream&st);
    void Compute(PObject);
    void ClassProc(TClassMSG*);
    BOOL GetClassFile(char*,BOOL forsave=TRUE);// Если вернуло 0, то файла нет
    BOOL RemoveLinkToObject(HOBJ2D obj);
    PClassListInfo GetClassListInfo();
    PObject   GetPrevObject(PObject);//  ???
    // Для списка детей
    BOOL      AddChild(PClass,HOBJ2D); // Добавляет чилда, и если есть объекты этого класса то
    // правит их, добавляя там объекты.
    BOOL      RemoveChild(HOBJ2D);
    //BOOL      ReplaceChild(HOBJ2D,char * name); // Заменяет чилда на нового

    // для линков
    PLinkInfo GetLinkById(HOBJ2D,INT16*); //!
    BOOL      AddLink(PLinkInfo);
    BOOL      RemoveLink(HOBJ2D,BOOL fromscheme=TRUE);// По дескриптору полилинии
    BOOL      ModifyLink(HOBJ2D id,PLinkInfo _link);
    //int       ReplaceLink(PLinkInfo old,PLinkInfo _new);

    PLinkInfo GetLinkToObject(HOBJ2D); // Получает связь на объект
    PLinkInfo GetLinkByHH(HOBJ2D,HOBJ2D,INT16*num=NULL);
    INT16     GetChildById(HOBJ2D);
    // Возвращает номер чилда по его дескриптору на схеме
    // -1 ошибка

    // для переменных
    BOOL RemoveObjectFromList(PObject po);
    void UpdateCoordinates();
    void Update(TStream&st,INT16 code,UINT16 load);//читает отдельную запись
    void Update(TStream&st,INT16*); // обновляет требуемую часть класса
    BOOL UpdateItem(INT16);        // обновляет один требуемый элемент
    BOOL UpdateClassFile();      // обновляет файл

    void SetVars(PVarInfo,INT16);
    INT16 GetVarIdByName(char*n,BOOL warning);

    void inline SetModify(){flags|=CF_MODIFY;}
    BOOL SetObjectOrder(HOBJ2D id,INT16 order);

    void WriteLink(TStream&st,PLinkInfo);
    void ReadLink(TStream&st,PLinkInfo);

    void SetClassIdByDate(); // Устанавливает уникальное значение класса по дате
    BOOL GetCreateTime(char*,int n);
    // Для работы с иконками
    BOOL Protected();

    void UpdateDisableVar(); // Обновляет переменную запрещения
    PProject GetProject();
    void DeleteLink(PLinkInfo);
    char * GetIconFile();
    BOOL IsBaseClass(); // FLOAT,HANDLE и т.д.
    void  Help();
    BOOL CanAddChildren(TClass*);
    int  ChkScheme(HSP2D hsp); /* проверяет схему на корректность и возвращает не ноль если схема неверна
                              и пользователь согласился ее поправить
                              1 - если неверные дескриторы имиджей
                            */
protected:
    void DeleteVars();        // Они для работы с полями класса
    void DeleteVar(PVarInfo);
    void DeleteChilds();
    void DeleteChild(PChildInfo);
    void DeleteLinks();
};

class TLinkInfo
{
public:
    HOBJ2D obj1Id;
    HOBJ2D obj2Id;
    HOBJ2D handle;       // дескриптор на схеме
    HOBJ2D varhandle;
    DWORD flags;
    INT16 count;
    //gr_float x,y;     // Смещение относительно origin объекта
    PLinkEntryInfo items;

    void Create(TLinkInfo*);
    void Done();
};

struct TLinkEntryInfo
{
    char * name1;
    char * name2;
};

struct TGrid
{
    HSP2D hsp;
public:
    double OffsetX,OffsetY;
    double StepX,StepY;
    BYTE   visible;
    BYTE   use;

    TGrid(HSP2D hsp);

    void  SetGrid();
    void  Remove();
    void  Update();
};

struct TChildInfo
{
    PClassListInfo type;
    HOBJ2D handle;      // дескриптор на схеме
    char * name;
    // Не менять порядок
    gr_float x,y; // координаты на схеме
    BYTE   flags;
    char * absentclass;
};
class TSchemeWindow;

class TScheme
{
public:
    char*filename;
    Hpointer scheme;
    TSchemeWindow * window;
    PClass _class;
    HGLOBAL block;
    HSP2D hsp;
    long size;
    long flags;

    TScheme(PClass);
    TScheme(PClass, HSP2D _hsp);
    TScheme(PClass, char* filename, int def);
    TScheme(PClass, TStream&);
    ~TScheme();

    BOOL  SetScheme();
    HSP2D CreateSpace();
    HSP2D GetSpace(BOOL getcopy = 0);

    // Копию вернет только
    // если уже было пространство, в противном случае вернет первое
    // Возврат копии осуществяется на уровне GetClassScheme() !!!

    void  Store(TStream& stream);
    inline BOOL Loaded(){ return hsp != 0;}
    BOOL  GetFilename(char*);
};

// TObject

typedef  pointer* TVARARRY;

class TObject
{
public:
    enum COPY_MODE{OLD_TO_NEW, NEW_TO_OLD};
    // vars
    PClass    _class;     // указатель на класс

    PObject   parent;     // Указатель на родительский объект
    PObject   childObjects;      // указатель на первый подэлемент

    PObject   next;       // указатель на следующий в родительской группе
    PObject   class_next; // указатель на такой же следующий элемент

    DWORD     flags;      // флаги
    //HSP2D     hSpace;
    TSetVar*  varData;
    TVARARRY  vars;    // указатель на переменные

    TObject();
    ~TObject();

    void LoadState(TStream&, INT16 mode);
    void StoreState(TStream&, INT16 mode);

    void Compute();
    void _Compute();

    BOOL IsDisable(BOOL chkparent=TRUE);

    void CopyState(COPY_MODE);
    void _CopyState(COPY_MODE mode);

    // void DeleteLinks();
    void UpdateLinks(TMemoryManager&m,TMemoryManager&mv);

    PObject _GetObject(HOBJ2D id);// Возвращает указатель объект по дескриптору
    pointer GetVar(INT16); // возвращает указатель на переменную(old state)
    pointer _GetVar(char*,long*size,PVarInfo* pv=NULL);// возвращает указатель на указатель на переменную
    INT16  GetVarIdByName(char* n, bool warning = FALSE);
    long GetVarSize(INT16);
    void SetDefault();
    void ClearVars();
    void ClearAndDef();
    void GetFullFileName(char*src,char*dst); // Получает полный путь к файлу в каталоге класса
    BOOL  SendMessage2(char *type,char *path,char**vars,int count,char*filter);
    PObject GetReceiver(char*name,BOOL * all=NULL);
    HOBJ2D GetHandle(INT16 *num);//Возвращает дескриптор в родительской группе и номер
    void AllEnable();
    void ClearVarPointers();
    TProject* GetProject();
    // -------------
    double GetVarF(char*,double def=0);
    UINT32 GetVarH(char*,UINT32 def=0);
    char *GetVarS(char*,char* def=NULL);

    void SetVarF(char*,double def);
    void SetVarH(char*,UINT32 def);
    void SetVarS(char*,char* def);

    INT32 GetChildCount();
    BOOL IsContainClass(PClass);
    BOOL IsContainObject(PObject);
    PObject GetMainObject(PObject po1,PObject po2);
    // для установленных
    void SetSettingsVars();
    BOOL IsSetExist();
    BOOL DeleteSetVar(char*);
    void DoneSetVar();
    void SaveSetState(TStream &st);
    bool LoadSetState(TStream &st,long pos);
};
bool _LoadSetState(PObject po,TStream &st);

struct TVarInfo
{
    char * name;
    char * info;
    char * def;
    // int  handle; // дескриптор на схеме
    PClassListInfo classType;


    UINT32 flags;

    void SetFlags(UINT32 f){flags|=f;};
    long GetFlags() {return flags;};
    void Done();
    void Create(TVarInfo*);
    // long size; //???
};

struct TClassItem
{
    long pos;
    WORD code;
};
/*
 Эта запись записывается в файл класса, как информация о
 элементах класса,(код элемента и его позиция в файле)
*/
//    Glabal Core Functions

C_TYPE InitClassList(char*);// Инициализация списка классов
C_TYPE InitAddLib();        // Инициализация списка дополнительных классов
void   DoneClassList();     // Уничтожение списка классов.

PObject CreateObjectByClass(PClass _class);
PObject CreateObjectByName(char*);
PClass  GetClassByName(char*,INT16 flag=0); // Если надо, то грузит если flag>-1
PClassListInfo GetClassListInfo(char *name);

// Работа со схемеми

BOOL    CreateDefClassScheme(char*);              //!
HSP2D   GetClassScheme(char*, PClass *_c, BOOL createcopy=0);     //!
BOOL    CloseClassScheme(PClass);                 //!
/*
 Закрывает схему и обновляет файл класса
*/
BOOL    SaveClassScheme(PClass);            //!
/*
  Обновляет информацию в имидже информацией из пространства,
  и обновляет файл класса.
*/

/*===================================================================
                  Работа с изображениями классов
=====================================================================*/
BOOL  CreateClassImage(char*name,HSP2D hsp,char* _name=0);
HSP2D GetClassImage(PClass _class);
BOOL  CloseClassImage(PClass _class,BOOL update =TRUE);
BOOL  UpdateClassDefIcon(PClass _class);
//int     PaintIcon(PClass,HDC,int x,int y); // becose hdc ???
HSP2D    ConvertClassScheme(PClass,BOOL fromdisk, BOOL createnew = 0);

/*===================================================================
                  Работа с классов
=====================================================================*/
PClass CreateNewClass(char* name,char *text,char * scheme,TLibrary*lib);
PClass CreateClassByClass(char* name1,char* name2,TLibrary*lib=NULL);
BOOL   ChangeClassName(PClass,char *name);

BOOL    SetClassText(PClass _class,TVarData*,char *txt);
BOOL   SetClassText(PClass _class,char *txt, bool showErrorDialog=true);
char*  GetClassText(PClass _class);
char*  GetClassInfo(PClass _class);
INT16  ModifyVar(PClass _class,PVarData);
// return -1 if Error
// if (id<0)then AddNewVar

PClass CopyClassToNewClass(PClass,char* name);

PClass AddChildren(PClass _class,char* name,HOBJ2D*id,gr_float xpos,gr_float ypos);

/*
 Функция добавляет чилдрена в список чилдренов класса определяемого
 параметром _class, name - имя класса вставляемого чилдрена
 id -его дескриптор, если он равен 0, то создается его иконка
 xpos,ypos - его координаты(только если id==0)
*/
BOOL  RemoveChildren(PClass,HOBJ2D id,BOOL notconfirm=FALSE,BOOL updatememory=TRUE);
/*
 Функция удаляет чилдрена в классе.
 Чилдрен определяется параметром id- дескриптор объекта в пространстве
 notconfirm - определяет задавать или не задавать вопросы
*/
HOBJ2D ReplaceChild(PClass,HOBJ2D,char * name); // Заменяет чилда на нового
BOOL AddLink(PClass,PLinkInfo);              //!
BOOL RemoveLink(PClass,HOBJ2D id,BOOL fromscheme=TRUE);              //!
BOOL ModifyLink(PClass,HOBJ2D id,PLinkInfo,BOOL onlyconnect=FALSE);    //!
/*

*/
BOOL AcceptChanges(BOOL accept,TStringCollection*items=NULL);
BOOL SaveAllModifyClasses(BOOL save=TRUE); // Возвращает 1 если что то записал
BOOL IsNeedAcceptChanges(char*);
/*
  Записывает все измененные классы в их файлы
*/
// Переменные

INT16   GetObjectVarId(PClass,char*);
char*   GetVarType(PClass,INT16 id);
pointer GetObjectVar(PObject,INT16 id);
BOOL    SetObjectVar(PObject,INT16 id,pointer);
// Непосредственно текстовый вид переменной
char*   GetObjectVarA(PObject,INT16 id,char*);
BOOL    _GetVarA(void*,PClassListInfo,char*);
// Если вертает не 0, то это указатель на строку.

BOOL    SetObjectVarA(PObject,INT16 id,char*);
BOOL   _SetObjectVarA(PObject,INT16 id,char*);

int DeleteClass(char* name);

BOOL LoadProject(char * file,BOOL destroy=TRUE);
BOOL CreateNewProject(char * path,char*name);
BOOL CreateSimplyProject();
BOOL CloseProject(TProject * project);
TProject*GetProject(char*);
BOOL SetActiveProject(TProject*);


void AddRecentlyProject(char *);
BOOL SaveProjectAs();
BOOL GetRootObject(PClass*_class,PObject*object);
HOBJ2D GetObjectHandle(PObject po);
TLibrary*GetParentLibrary();
TLibrary*MakeNewLib(TLibrary*,char*);
BOOL     DeleteLib(TLibrary*);

// Моделирование

BOOL SaveSetState(PObject po,char*name);
bool LoadSetState(PObject po, char* sttFileName);

BOOL LoadState(PObject po,char*name); // Загрузить состояние
BOOL SaveState(PObject po,char*name);
BOOL __SaveState(TStream &st,PObject po,BYTE flags=1);// 1 - Просто состояние, 2 - setVars
bool __LoadState(TStream &st,PObject po,BYTE flags=0); // Загрузить состояние
// Если Po - NULL то всей схемы
void SetNextSaveStateMode(int);
BOOL UpdateSystemVars();

//
BOOL DoOneStep();

BOOL UpdateClassWindows(UPDATE_MODE,UPDATE_WHAT,void *,void *);
//int EnumerateClasses(PLibrary,DWORD,int Action(char*,void*),void*);
HOBJ2D CreateVarPlace(PClass,HSP2D,gr_float x,gr_float y);
#define ECF_BUILDIN     1
#define ECF_MAYVARS     2
#define ECF_MAYBECHILD  4
//#define ECF_
int StrToDouble(char*text,double&value);
//int EnumerateVars(PClass,int Action(PVarInfo,void*),void*);
int EnumerateLibs(PLibrary,DWORD,int Action(PLibrary,void*data),void*);

BOOL AddObjectName(char*);
void Recompile(PLibrary);


HOBJ2D CreateObjectFromFile(HSP2D hsp,char * file,gr_float x,gr_float y,UINT16 flags);

BOOL   GetProjectPath(char*);

BOOL GetNewProjectPath(char*file);
void  * GetCurrentProject();

INT16 _WhatType(PClass _class,HSP2D hsp,HOBJ2D obj);
void  ObjectsChanged();
int   RebuidEQUs();
int   CalcEQUs();
BOOL  EQExist();
BOOL  IsNameValid(char *);

UINT32 StratumProc(UINT16 code,UINT32 p1,UINT32 p2);

INT16  GetRunningObject(PClass*,PObject*po=NULL);
HSP2D extern spec_space;
void  SetLastSteps(); // выводит количество проиденных шагов

void GetClassesInfo(TStream&stream);
BOOL GetUniqueClassName(char*name,BOOL copy=FALSE);
int IsAbsolutePath(char *file);
TSortedCollection * GetClasses();
BOOL StrToRGB(char*,COLORREF&,COLORREF* realcolor);
BOOL RGBToStr(COLORREF,char*);
TLibrary *GetDefaultLib();

void DoRenamePaths(HWND ,PLibrary lib);

#ifdef LOGON
void  LogMessage(char*);
void  DoneLog();
void  LogStep(int);

class TLOGoffset
{
    BYTE offset;
public:
    TLOGoffset(BYTE _offset=1);
    ~TLOGoffset();
};

#define LOGOFFSET TLOGoffset __offset();
#endif

#define ELF_NOHIERARCHCY 1

#ifdef DEMO
void DemoMSG();
#endif

#endif // NOT include


