/*
Copyright (c) 1996  TST

Project: Stratum 3.0

Module Name:
            dialogs.h
Author:
            Alexander Shelemekhov
*/

BOOL ReminderDialog(HWND hwnd);
BOOL ErrorDialog(HWND,char*,char*);
BOOL ClassDialog(HWND, PClass, PObject, PLibrary, PClass* __class = NULL, PClass _parent=0, HOBJ2D handle=0);
BOOL VarPlaceDialog(HWND,HSP2D,HOBJ2D ,PClass=NULL);
BOOL LibraryDialog(HWND,PLibrary);
BOOL LinksDialog(HWND hwnd,PClass _class,HSP2D hsp,HOBJ2D id);
BOOL NewChildDialog(HWND,PClass,char*);
BOOL _ChoseColor(HWND hwnd,COLORREF & clr,char*);
BOOL Object2dDialog(HWND hwnd,HSP2D hsp,HOBJ2D object,int type,PClass _class=NULL);
// _class если является изображением этого класса
BOOL Object3dDialog(HWND hwnd,HSP3D hsp,HOBJ2D object);
BOOL NewProjectDialog(HWND hwnd,char*path,char*name);
BOOL AboutDialog(HWND hwnd,int=0);
BOOL MatrixDialog(HWND hwnd,int matrix);

BOOL InputBox(HWND hwnd,char *title,char*msg, char*s);
BOOL ChoseFontDialog(HWND parent,LOGFONT& lf,char * title);
BOOL SpaceDialog(HWND hwnd,HSP2D,int mode,TGrid*grid,PClass _class=NULL,PObject _pobject=NULL);
#ifndef RUNTIME
BOOL SysInfoDialog(HWND hwnd);
BOOL ProjectDialog(HWND hwnd,TProject*project);
#endif
BOOL EnvironmentDialod(HWND hwnd);

BOOL BitmapDialog(HWND hwnd,BITMAP*,char *title);
BOOL SpacePrintDialog(HWND hwnd,HSP2D);
BOOL PrinterSetupDialog(HWND hwnd);
BOOL Variable2dDialog(HWND hwnd,PObject,INT16 index);
BOOL ControlBarsDialog(HWND hwnd);
BOOL AddDirErrorDialog(HWND hwnd);
BOOL TableDialog(HWND hwnd,int table);
BOOL TableSearchDialog(HWND hwnd,void*d);
BOOL UnusedClassesDialog(HWND tree,PLibrary lib);
HOBJ2D TextureDialog(HWND hwnd,HSP3D,HOBJ2D);
BOOL TexturesDialog(HWND hwnd,HSP3D);
BOOL ClassInfoDialog(HWND hwnd,PClass);

int SearchTextDialog(HWND ,BOOL replace);   // 2 - replace all
int ExecuteFind(HWND,BYTE flags); // 1 replace | 2 - again | 4 -all

void GetRopString(INT16,char *);
void GetPenStyleString(INT16,char *);
void GetBrushStyleString(INT16,char *);
BOOL GetObjectTypeName2d(INT16,char*s,int size);
void SetDefaultSyntaxColor(PClass,HWND edit);

double ScaleDialog(HWND,double);
BOOL SearchDialog(HWND,UINT16 f=0);  // f 1 - нелзя искать в значениях переменных
//   2 - нельзя искать в имиджах
//   4 - нельзя искать в связях
//   8 - нельзя искать в графике
//   16 - нельзя группировать
int  MathErrorDialog(char*);
class TAnime;
int  AnimateDiloag(HWND ,TAnime*);
struct GETFILETYPE;
BOOL GetFileTypeDialog(HWND ,GETFILETYPE&);
BOOL DoSqlDialog(HWND);

char * NewIntStr(char*);
void LoadStr(int,char*);
BOOL InitSyntax();

BOOL SaveTableDialog(HWND hwnd,int );
int ModelDialogBox(char* name,int size,char **p);
BOOL PluginsDialog(HWND);
BOOL SetVarDialog(HWND,TCollection*);
BOOL NonLinearDialog(HWND,TProject*);
BOOL LinearDialog(HWND hwnd,TProject*prj);
BOOL SyntaxSetupDialog(HWND hwnd); // настройка синтаксического редактора
BOOL ManualRotateDialog3d(HWND,HSP3D,HOBJ2D);
TStringCollection* SaveChangesDialog(HWND hwnd);
int  IsNeedSaveChanges(HWND hwnd,char*);

BOOL scCALLBACK TxtToolProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL scCALLBACK VertToolProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL scCALLBACK PrimToolProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
BOOL ChoseDirDialog(HWND hwnd,char *dir,char *title,UINT32 flags);
BOOL ChoseColor(HWND parent,POINT&org,COLORREF& color,long data,int flags);
// 1 палитра | 2 системные | 4 - прозрачность

#define CHDIR_FORSAVE 1

// Для поиска
#define SR_CASE   1
#define SR_WORDS  2
#define SR_BACK   4
#define SR_SELECT 8
#define SR_FROMCURSOR 16
#define SR_PROMPT     32
#define SR_MAKEGROUP  128    // создавать группу

struct PASTESTRUCT
{
    UINT     cformat;
    POINT2D  point;
    UINT16   flags;
};

BOOL PasteDialog(HWND ,PASTESTRUCT &);

#ifdef WIN32
extern HINSTANCE hRTFLib;
#endif

void RegisterIcons();
