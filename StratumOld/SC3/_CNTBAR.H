#define BUTTON_OFFSET 0 // Внутри кнопки , между картинкой
#define BUTTON_DX     1 // Между кнопками
#define BUTTON_DY     1

#define BUTTON_fX     5 // Между кнопками и границами
#define BUTTON_fY     3
#define BUTTON_UP     3  // Up border
#define BUTTON_DOWN   2  // Down border
#define BUTTON_XX     5  // Сбоку

#define BUTTONOF_97    8  // дополнительный отстут для 97 стиля


#include "template.h"
#define min_width (BUTTON_WIDTH+3+BUTTON_OFFSET*2)+2*BUTTON_fX

class TCommand;

class TCombo:public TCollection{
public:
 int sizex;
 BOOL listonly;
 int  cmd;
 char currentstr[128];
 BOOL inmsg;
 TCombo();
 TCommand*command;
 TCombo(TCommand*,int _cmd);
~TCombo();
 virtual void FreeItem(pointer item){delete (char*)item;};
};

class TCommand{
public:
 INT16 command,rcommand;
 int  id,width;
 BYTE   group,style;
 char * name;
 char * tooltip;
 char * menutext;
 TCombo*combo;
//----------
 unsigned int pressed:1;
 unsigned int disabled:1;
 unsigned int down:1;
 INT16 _x,_y;
 TCommand(char *_name,char *_menu,char *_hint,
   int _command,int _id,int _width,int group,int _style,int ax,int ay);
 int GetWidth();
 ~TCommand();
};

class _TCommand{
 public:
  TCommand*command;
 INT16 _x,_y;
 HWND HWindow;
 _TCommand(TCommand*);
 BOOL GetSize(POINT*);
 BOOL GetRect(RECT*,int dock);
 BOOL IsSeparator(){return command==0;};
 BOOL Pressed();
 int GetWidth();
};

class TRange:public TCollection{
public:
 char *name;
 int id;
 int lastdock,start;
 BOOL ispopup;
 BOOL wasremove;
 BOOL window;
 BOOL hidden;
 RECT poprect;
 TRange(char *);
 TControlBar* bar;
 ~TRange();
 void Store(TStream&st);
 void Restore(TStream&st);

 virtual void FreeItem(pointer item){delete (_TCommand*)item;};
};


class TCommands:public TCollection{
 TTplReader * tpl;
public:
 TControlDock* docks[4];
 HBITMAP hbuttons,hdisabled,hmask;
 TCollection * ranges,*windows;
 TCommands();
 ~TCommands();
 virtual void FreeItem(pointer item){delete (TCommand*)item;};
 TRange*  GetRange(char*);
 TRange*  GetRange(int id);
 void Press(TCommand*);
 TCommand* Get(char*);
 TCommand* Get(int);
 void Store(TStream&);
 void Restore(TStream&);
 void DoneBitmaps();
 void LoadBitmaps();
protected:


 void ReadFromTemlpate();

 int bmpsizex;
 int bmpsizey;
 char *  input;
 char * _input;
 BOOL ReadKeyWord(int & code);
 BOOL ReadInt(int &i);
 BOOL ReadChar(char c);
 BOOL ReadStr(char *,int _len=250);
 BOOL ReadSpaces();
 BOOL ReadCommand();
 BOOL ReadRange();
 TCombo *ReadCombo(BOOL&);

};


