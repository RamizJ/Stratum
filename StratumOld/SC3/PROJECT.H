#ifndef __SC_PROJECT_H__
#define __SC_PROJECT_H__
#include "vars_m.h"
#include "MEMMGR.H"
#include "../BC/bc_include_stratum/STREAM.H"

class Equations;
class PObject;

struct THyperAdd
{
    char*state;
    char*defscheme;
    BYTE readonly;
    BYTE autorun;     // (1 - авто запуск, 2 - нет , 0 - по умолчанию)
    BYTE autoedit;    // показ схемы при загрузке проекта
    BYTE autoclose;   // авто закрытие по остановке проекта
    BYTE editonclose; // показ схемы при остановке проекта
    BYTE save;        // запись при остановке: (нельзя записться, 1 - можно но только специально, 2 - запрос)
    BYTE hidewindows; // прятать окна всех проектов

    THyperAdd(char*);
    ~THyperAdd();

    void Generate(char*);
};

struct TPostProcess
{
    int code;
    PClass _class;
    void * data;
    TPostProcess*next;

    TPostProcess();
    ~TPostProcess();
};
#define PPF_UNLOADPROJECT 1
#define PPF_SETCLASSTEXT  2

class TProject
{
    bool status;
    bool Init();
    TPostProcess* post;

public:
    unsigned int eqvalid:1;
    unsigned int modifyed:1;
    unsigned int varmodifyed:1;
    unsigned int newproject:1;
    unsigned int watchdisable:1;
    unsigned int readonly:1;
    unsigned int nohandleunload:1;
    unsigned int unloadondeactivate:1;

    TProject* next;
    TProject* prev;

    TCollection* watch;

    void * temp;
    DWORD _exec_flags;
    HWND wnd;
    THyperAdd     *hyperadd;
    TScVarManager *vars;

    char * file;      //путь и имя файла проекта
    char * path;      //путь к проекту
    char * tmpfile;
    char * realname;
    char * projecttoactivate; // путь к парентовсого проекта

    PObject object;
    PClass  _class;   //главный имидж проекта

    TMemoryManager* VarsMemory;
    TMemoryManager* VarsPointers;

    Equations     *equations;
    //TLibrary      *lib;
    TCollection*    libs;

    TProject();
#ifndef RUNTIME
    TProject(char*, char* _classname);
#endif
    TProject(TDOSStream&);
    ~TProject();

    void AddPostprocess(int code,PClass _cl=NULL,void*data=NULL);
    void PostProcess();
    BOOL Ok(){return status==0;}
    BOOL IsModifyed(){return modifyed;}
    void SetModify(BOOL m=TRUE){modifyed=m;}
    void VarChange(BOOL vc=TRUE){varmodifyed=vc;}
    BOOL IsVarModifyed(){return varmodifyed;};

    BOOL Store();
    BOOL DoOneStep(PObject=NULL);
    void SetTitle();
    void ResetEq(){eqvalid=0;}
    BOOL EQExist() { return equations != 0; };
    void EQBuild(PObject _po);
    BOOL RebuidEQUs();
    BOOL CalcEQUs();
    bool UpdateVarsLinks(bool olddone=0);
    // INT16 ModifyVar(PClass _class,PVarData data);
    BOOL PostModifyVar(PClass _class);
    BOOL PreModifyVar(PClass _class);
    void ResetProject();
    void CloseWindows();
    void CloseStreams();
    void VarToDef(BOOL);
    void ClearVariables();
    void CloseBases();
    BOOL ChkEqu();

    double SetProjectProp(char*,double);
    double SetProjectProp(char*v,char*s);
    double GetProjectProp(char*);
    //----------------------- Далее все для WATCH
    BOOL AddWatch(PObject po,char *var);
    BOOL RemoveWatch(PObject po,char *var);
    BOOL IsWatched(PObject po,char *var,C_TYPE *index=NULL);
    BOOL GetWatchText(C_TYPE y,C_TYPE x,char*);
    BOOL SetWatchText(C_TYPE y,C_TYPE x,char*);
    void UpdateWatchWindow();  // Вызывается для обновления окна
    BOOL UpdateWatches();      // Вызывается для обновления данных при модификации объектов

    // Для переменных
    INT32 GetVarDef(char*,INT32);
    void  SetVarDef(char*n,INT32 data,INT32 def);
    void  SetVarI(char*n,INT32 data);
    BOOL  GetVarS(char*,char*);
    void  SetVarS(char*,char*);
    double GetVarD(char*,double);
    void  SetVarD(char*,double,double);

    void  GetPrloadFile(char*);
    void  DeleteVar(char*);

    static int UpdateLinks(PClass);
    bool UpdateLinks(PObject ,TMemoryManager& m, TMemoryManager& mv);
    void GetFullFileName(char* src,char*dst,BOOL useURL=0);
    void DeleteWatch(C_TYPE i);
    // для гиперпроектности
    void OnOpenScheme();
    void OnLoad();
    void OnStop();
#ifdef PROTECT
    BOOL  CanSaveProject();
#endif

private:
    //void DoneLib(PLibrary);
    //void _DoneLib(PLibrary);
};
typedef TProject* PProject;

void GetFullFileNameEx(PProject, char*src, char*dst);
BOOL SaveProjectAs(PProject project,char*path);
extern PProject project;
extern PProject projects;

#endif
