#ifndef _SC3_HYPER_H_
#define _SC3_HYPER_H_

#define UD_HYPERKEY   10
#define UD_HYPERSPACE 11
#define UD_ANIMATE    12
#define UD_VARS       13 // связываемые переменные
#define UD_OBJINFO    14

class THyperKey
{
    HSP2D hsp;
    HOBJ2D   obj;
    TMemoryStream * st;
    BOOL modify;
    BOOL exist;

    void _SetStr(char*&,char*);
    void Load(PStream);

public:
    BOOL  disabled;
    char * target;
    char * winname;
    char * object;
    char * add;
    UINT   openmode;
    char * effect;
    INT16 time;

    THyperKey(HSP2D _hsp, HOBJ2D _obj, PStream=NULL);
    ~THyperKey();

    void Store(PStream,BOOL storeNULL=TRUE);
    Update();
    void inline SetTarget(char*s){_SetStr(target,s);};
    void inline SetWinName(char*s){_SetStr(winname,s);};
    void inline SetObject(char*s){_SetStr(object,s);};
    void inline SetEffect(char*s){_SetStr(effect,s);};
    void inline SetAdd(char*s){_SetStr(add,s);};
    void inline SetMode(int m){if (m!=openmode){openmode=m;modify=1;}}
    void inline SetTime(INT16 t){if (t!=time){time=t;modify=1;}}
    BOOL inline IsModifyed(){return modify;}
};


class TSpaceData
{
    HSP2D hsp;
    BOOL  wasdata; //Данные были

public:
    DWORD style;
    UINT16 sizex;
    UINT16 sizey;

    TSpaceData(HSP2D _hsp);

    void Update();

    BOOL inline IsDefined() {return  wasdata;}
};

#define SWF_AUTOSCROLL   1
#define SWF_HSCROLL      2
#define SWF_VSCROLL      4

#define SWF_SPACESIZE    8
#define SWF_NORESIZE    16

#define SWF_DIALOG      32
#define SWF_POPUP       64
#define SWF_MDI        128
#define SWF_MODAL      256
#define SWF_AUTOORG    512

#define SWF_WINTYPE   (32|64|128)
#define SWF_MAXIMIZE   1024
#define SWF_MINIMIZE   2048

struct TAniVar{
    char          *name;
    PClassListInfo type;
    char          *data;
    ~TAniVar();
};

class TAnime{
public:
    double time1,time2;
    LONG   flags;
    char *name;
    TCollection*vars;
    TAnime*next;
    TAnime(char*);
    TAnime(PStream);
    ~TAnime();
    PClass GetClass(BOOL dlg);
    void Store(PStream ps);
    void Insert(TAnime*);
};

TMemoryStream * ObjectData(HSP2D hsp,HOBJ2D obj,UINT16 id);
#endif

