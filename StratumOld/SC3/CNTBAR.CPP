#include "collect.h"
#include <stdlib.h>
#include <stdio.h>
#include "stream.h"
#include "twindow.h"
#include "tcntbar.h"
#include "_cntbar.h"
#include "collect.h"
#include <mem.h>
#include "sc3.rh"
#include "dialogs.rh"
#include "msgcrk.h"
#include "systab.h"
#include "_message.h"
#include <stratum\stratum.h>
#include <stratum\sc_msg.h>
#define _WIN_HOOK_DYNAMIC
#include <gwin2d3d\winhook.h>
#include "strings.rh"

class TProject;

extern TProject* project;

#define sfdrag    1
#define sfresize1 2
#define sfresize2 3
#define sfresize3 4
#define sfresize4 5
#define sfdown    10
#define ID_TIMER 10

BYTE off97=0;

void InvalidateControlBars()
{
	InvalidateRect(mainwindow->HWindow, 0, 0);
/*
  for(C_TYPE i=0;i<commands->ranges->count;i++){
    TRange *r=(TRange*)commands->ranges->At(i);
   if(r->bar) InvalidateRect(r->bar->HWindow,0,0);
  }
*/
}

void SetCntBarStyle(int d)
{
  off97=d;
  for (int i=0; i<4; i++)
    mainwindow->docks[i]->Recalc();
  mainwindow->UpdateMainWindow();
  InvalidateControlBars();
} ;

void GetTemplateDir(char*s);

extern HINSTANCE hInstance;

static TCommands * commands=NULL;

char * keyWords []= { "",      "COMMAND",   "HINT",  "STYLE",   "GROUP",    "ID",   "NAME", "ID",      "}", "MENU",
                      "RANGE", "SEPARATOR", "COMBO", "STRINGS", "LISTONLY", "SIZE", "DOCK", "RBUTTON", "HIDDEN"};

#define K_COMMAND  1
#define K_HINT     2
#define K_STYLE    3
#define K_GROUP    4
#define K_ID       5
#define K_NAME     6
#define K_WIDTH    7
#define K_END      8
#define K_MENUTEXT 9
#define K_RANGE    10
#define K_SEPARATOR 11
#define K_COMBO     12
#define K_STRINGS   13
#define K_LISTONLY  14
#define K_SIZE      15
#define K_DOCK      16
#define K_RBUTTON   17
#define K_HIDDEN    18

C_TYPE * last_i;

const long RopPSDPxax=0x00B8074AL; //
const long RopDSPDxax=0x00E20746L; //

TCombo::TCombo(TCommand *com, int _cmd): TCollection(10, 1)
{
  command=com;
  cmd=_cmd;
  inmsg=0;
  lstrcpy(currentstr, "");
  sizex=0;
  listonly=FALSE;
} ;

TCombo::~TCombo()
{
  FreeAll();
  DeleteAll();
} ;

TCommand::TCommand(char*_name, char*_menu, char*_hint, int _command, int _id, int _width, int _group, int _style,
                   int ax,     int ay)
{
  memset(this, 0, sizeof(TCommand));

  name=NewStr(_name);
  menutext=NewStr(_menu);
  tooltip=NewStr(_hint);
  command=_command;
  id=_id;
  group=_group;
  style=_style;
  _x=ax;
  _y=ay;
} ;

int TCommand::GetWidth()
{
  if(width)
    return width;
  return BUTTON_WIDTH;
} ;

TCommand::~TCommand()
{
  if(name)
    delete name;
  if(tooltip)
    delete tooltip;
  if(menutext)
    delete menutext;
  if(combo)
    delete combo;
} ;

TControlBar::TControlBar(LPCREATESTRUCT l, HWND h): TWindow(l, h)
{
  highlight=NULL;
  htimer=0;
  popup=(l->style&WS_POPUP)!=0l;
  alone=GetParent(h)!=mainwindow->HWindow;
  caption=14;
  start=0;
  state=0;
  dock=_dock=0;
  trackrect.left=trackrect.top=0;
  trackrect.right=trackrect.bottom=0;
  trackrect;
  sizes=NULL;
  char s[256];
  GetWindowText(HWindow, s, sizeof(s));
  range=commands->GetRange(s);
  if(range)
  {
    range->bar=this;
    trackrect=poprect=range->poprect;
    lastdock=range->lastdock;
    start=range->start;
    int dock_=l->style&0xf;
    UpdateCombos(dock_);
    dock=dock_;
    EvSize(0, l->cx, l->cy);
    PressControlButton(range->id, TRUE);
  }
  commands->windows->Insert(this);
  (DWORD)parent=GetWindowLong(GetParent(HWindow), 0);
} ;

void TControlBar::RemoveCombos()
{
  for (C_TYPE i=0; i<range->count; i++)
  {
    _TCommand * _c=(_TCommand *)range->At(i);
    if(_c->HWindow)
    {
      DestroyWindow(_c->HWindow);
      _c->HWindow=NULL;
    }
  }
} ;

void __SetComboStr(_TCommand *_c, char*s)
{
  BOOL update=FALSE;
  for (C_TYPE k=0; k<_c->command->combo->count; k++)
  {
    char * str=(char *)_c->command->combo->At(k);
    if(!lstrcmp(s, str))
    {
      SendMessage(_c->HWindow, CB_SETCURSEL, k, 0);
      update=TRUE;
    }
  }
  if(!update)
  { SetWindowText(_c->HWindow, s);}
}

LRESULT
scCALLBACK
_CntComboEditProc(t_pointer data, HWND hwnd, UINT&msg, WPARAM&wParam, LPARAM&lParam, BOOL&calldefault)
{
  calldefault=1;
  if(ballon_message==msg)
  {
    _TCommand * _c=(_TCommand *)data;
    BALLONSTRUCT * bs=(BALLONSTRUCT *)lParam;
    if(GetFocus()!=hwnd)
    { lstrcpyn(bs->bsText, _c->command->tooltip, bs->bsSize);}
    calldefault=0;
    return 1;
  }
  switch(msg){
  case WM_KEYDOWN:
    switch(wParam){
    case VK_RETURN:
      {
      HWND mw=GetParent(GetParent(GetParent(hwnd)));
      _TCommand * _c=(_TCommand *)data;
      int command=_c->command->combo->cmd;
      GetWindowText(hwnd, _c->command->combo->currentstr, sizeof(_c->command->combo->currentstr));
      _c->command->combo->inmsg=1;
      _SendCommand(mw, command);

      _c->command->combo->inmsg=0;
      calldefault=0;
      return 0;
      }
    }
    break;
  }
  return 0;
} ;

LRESULT
scCALLBACK
_CntComboProc(t_pointer data, HWND hwnd, UINT&msg, WPARAM&wParam, LPARAM&lParam, BOOL&calldefault)
{
  calldefault=1;
  if(ballon_message==msg)
  {
    _TCommand * _c=(_TCommand *)data;
    BALLONSTRUCT * bs=(BALLONSTRUCT *)lParam;
    if(GetFocus()!=hwnd)
    { lstrcpyn(bs->bsText, _c->command->tooltip, bs->bsSize);}
    calldefault=0;
    return 1;
  }
  return 0;
} ;

BOOL CreateWinHook(HWND hwnd, WNDHOOKPROC, t_pointer data);

void TControlBar::UpdateCombos(int dock_)
{
  for (C_TYPE i=0; i<range->count; i++)
  {
    _TCommand * _c=(_TCommand *)range->At(i);
    _c->HWindow=NULL;
    if(_c->command&&_c->command->combo)
    { //1
      if((dock_==0)||(dock_&1))
      {
        long style=WS_BORDER|WS_CHILD;
        if(_c->command->combo->listonly)
          style|=CBS_DROPDOWNLIST;
        else
          style|=CBS_DROPDOWN;
        HWND h=CreateWindow("COMBOBOX", NULL, style, 0, 0, 60, 200, HWindow, (HMENU)_c->command->combo->cmd, hInstance,
                            0);
        for (C_TYPE j=0; j<_c->command->combo->count; j++)
        { SendMessage(h, CB_ADDSTRING, 0, (LPARAM)_c->command->combo->At(j));}
        SendMessage(h, WM_SETFONT, (WPARAM)GetStockObject(ANSI_VAR_FONT), 0);
        if(!_c->command->combo->listonly)
        {
          HWND e=GetWindow(h, GW_CHILD);
          CreateWinHook(e, _CntComboEditProc, (t_pointer)_c);
        }
        CreateWinHook(h, _CntComboProc, (t_pointer)_c);

        _c->HWindow=h;
        __SetComboStr(_c, _c->command->combo->currentstr);
      }
    } //1
  }
} ;

TControlBar::~TControlBar()
{
  DoneSizeList();
  if(htimer)
    KillTimer(HWindow, ID_TIMER);
  if(commands)
  {
    range->poprect=poprect;
    range->lastdock=lastdock;
    range->start=start;
    range->ispopup=popup;
    commands->windows->Delete(this);
    range->bar=0;
    PressControlButton(range->id, FALSE);
  }
} ;

void TControlBar::_Adjust(int dock_, POINT&_size, BOOL _popup)
{
  RECT r;
  r.left=BUTTON_fX;
  if(popup)
    r.left+=BUTTON_fX;
  r.top=BUTTON_fY;
  if(popup)
    r.top+=BUTTON_fY+caption;
  r.right=(_size.x-(BUTTON_fX+BUTTON_XX))+1;
  r.bottom=_size.y-BUTTON_fY;
  int pos=r.left;
  int posy=r.top;
  if(dock_)
  {
    if(!(dock_&1))
    { // Vertical dock
      if(off97&&!_popup&&!alone)
      { posy+=BUTTONOF_97;}
      for (C_TYPE i=0; i<range->count; i++)
      {
        _TCommand * _c=(_TCommand *)range->At(i);
        POINT p;
        _c->GetSize(&p);
        _c->_x=pos;
        _c->_y=posy;
        if(!_c->IsSeparator())
          posy+=p.y+BUTTON_DY;
        else
          posy+=p.x;
      }
      _size.y=posy+BUTTON_fX;
    }
    else
    { // horisontal dock
      if(off97&&!_popup&&!alone)
      { pos+=BUTTONOF_97;}
      for (C_TYPE i=0; i<range->count; i++)
      {
        _TCommand * _c=(_TCommand *)range->At(i);
        POINT p;
        _c->GetSize(&p);
        _c->_x=pos;
        _c->_y=posy;
        pos+=p.x;
        if(!_c->IsSeparator())
          pos+=BUTTON_DX;
      }
      _size.x=pos+BUTTON_fX;
    }
  }
  else
  { // no dock
    _size.x=0;
    _size.y=0;
    int minsizex=BUTTON_WIDTH+3;
    for (C_TYPE i=0; i<range->count; i++)
    {
      _TCommand * _c=(_TCommand *)range->At(i);
      _c->_x=_c->_y=0;
      POINT p;
      _c->GetSize(&p);
      if(p.x>minsizex)
        minsizex=p.x;
    }
    if((r.right-r.left)<(minsizex+3))
      r.right=r.left+(minsizex);
    BOOL _sepinline=FALSE, nfill=FALSE;
    for (i=0; i<range->count; i++)
    {
      _TCommand * _c=(_TCommand *)range->At(i);
      POINT p;
      _c->GetSize(&p);
      BOOL newline=FALSE;
      BOOL sep=_c->IsSeparator();
      if(sep)
      {
        _sepinline=TRUE;
        if(nfill)
        {
          newline=TRUE;
          nfill=FALSE;
        }
        else
        {
          int _pos=pos+p.x;
          for (C_TYPE j=i+1; j<range->count; j++)
          {
            _TCommand * __c=(_TCommand *)range->At(j);
            if(__c->IsSeparator())
              break;
            POINT _p;
            __c->GetSize(&_p);
            _pos+=_p.x;
            if(_pos>r.right)
            {
              newline=TRUE;
              break;
            }
            _pos+=BUTTON_DX;
          }
        }
      }
      else
      {
        if(pos+p.x>r.right)
        {
          newline=TRUE;
          nfill=TRUE;
        }
      }
      if(newline)
      {
        pos=r.left;
        posy+=BUTTON_HEIGHT+BUTTON_OFFSET*2+3;
        if(_sepinline)
          posy+=3;
        sep=_sepinline=FALSE;

        _c->_x=pos;
        _c->_y=posy;
        if(!_c->IsSeparator())
          pos+=p.x+BUTTON_DX;
      }
      else
      {
        _c->_x=pos;
        _c->_y=posy;
        pos+=p.x+BUTTON_DX;
      }
      _size.x=max(_size.x, _c->_x+p.x);
      _size.y=max(_size.y, _c->_y);
    }
    _size.x=_size.x+BUTTON_fX+BUTTON_XX;
    _size.y=_size.y+BUTTON_fY+BUTTON_HEIGHT+3+BUTTON_UP;
  }
}

void TControlBar::Adjust()
{
  POINT _size;
  GetRect(dock, _size);
  _Adjust(dock, _size, popup);
} ;

void TControlBar::EvSize(WORD, int sx, int sy)
{
  sizex=sx;
  sizey=sy;
  if(sx&&sy)
  {
    POINT p;
    p.x=sx;
    p.y=sy;
    _Adjust(dock, p, popup);
  }
  for (C_TYPE i=0; i<range->count; i++)
  {
    _TCommand * _c=(_TCommand *)range->At(i);
    if(_c->HWindow)
    { SetWindowPos(_c->HWindow, 0, _c->_x, _c->_y, _c->command->combo->sizex, BUTTON_HEIGHT,
                   SWP_NOZORDER|SWP_SHOWWINDOW);}
  }
} ;

void TControlBar::HardRemove(){range->wasremove=TRUE;};

void TControlBar::PrepareResize(int _state)
{
  state=_state;
  SetCapture(HWindow); //DrawSizeRect();
  MakeSizeList();
} ;

void TControlBar::EvLButtonDown(WORD, int xPos, int yPos)
{
  _TCommand * button=GetButton(xPos, yPos);
  CloseCombo();
  if(!button&&!alone)
  {
    int ht=NcHitTest(xPos, yPos);
    GetWindowRect(HWindow, &trackrect);
    switch(ht){
    case HTSYSMENU:
      mainwindow->RemoveControl(this);
      return;
    case HTTOP:
      PrepareResize(sfresize1);
      break;
    case HTBOTTOM:
      PrepareResize(sfresize3);
      break;
    case HTLEFT:
      PrepareResize(sfresize4);
      break;
    case HTRIGHT:
      PrepareResize(sfresize2);
      break;
    default:
      {
      state=sfdrag;
      SetCapture(HWindow);
      trackpoint.x=xPos;
      trackpoint.y=yPos;
      delta=trackpoint;
      UpdateWindow(HWindow);
      moved=FALSE;
      DrawFocusRect();
      }
    }
  }
  else
  {
    // ѕроверка на буттон  Ќадо вставить !!!
    if(!IsButton(button))
      return;
    int code;
    if(!alone&&(!parent||(code=parent->IsCommandDisable(button->command->command))>1))
    {
      SetDisableText(code);
      MessageBeep(MB_ICONHAND);
      return;
    } ;
    PaintButton(button, 1);
    state=sfdown;
    focus=button;
    SetCapture(HWindow);
  }
} ;

void TControlBar::PaintButton(_TCommand *button, BOOL pressed)
{
  if(button->command->down!=pressed)
  {
    HDC hdc=GetDC(HWindow);
    button->command->down=pressed;
    PaintButton(hdc, button, 0);
    ReleaseDC(HWindow, hdc);
  }
} ;

void TControlBar::MakeSizeList()
{
  POINT __size;
  GetRect(dock, __size);

  DoneSizeList();
  sizes=new TCollection(10, 10);
  POINT _p, _p1;
  _p.x=_p.y=0;
  _p1=_p;
  GetRect(1, _p);
  int x=_p.x+40;

  POINT * pp=&_p1;
  while(x>min_width-1)
  {
    x--;
    _p.x=x;
    _p.y=0;
    _Adjust(0, _p, 1);
    if(_p.y>pp->y)
    {
      pp=new POINT;
      * pp=_p;
      sizes->Insert(pp);
    }
    else
    {
      if(pp->x>_p.x)pp->x=_p.x;
    }
  }
  //	_Adjust(dock,_p);
  _Adjust(dock, __size, 1);
} ;

void TControlBar::DoneSizeList()
{
  if(sizes)
  {
    for (C_TYPE i=0; i<sizes->count; i++)
    { delete sizes->At(i);}
    sizes->DeleteAll();
    delete sizes;
    sizes=NULL;
  }
} ;

void TControlBar::EvMouseMove(WORD, int xPos, int yPos)
{
  if(off97&&state==0)
  {
    _TCommand * _highlight=highlight;

    _TCommand * h=GetButton(xPos, yPos);
    if(h&&!h->command->disabled)
    { highlight=h;}
    else
      highlight=0;
    if(_highlight!=highlight)
    {
      HDC hdc=GetDC(HWindow);
      if(_highlight)
        PaintButton(hdc, _highlight, 0);
      if(highlight)
      { PaintButton(hdc, highlight, 0);}
      ReleaseDC(HWindow, hdc);
    }
    if(highlight)
    {
      if(!htimer)htimer=SetTimer(HWindow,ID_TIMER,100,0);
    }
    else
    {
      KillTimer(HWindow, ID_TIMER);
      htimer=0;
    }
  }
  switch(state){
  case sfdown:
    {
    _TCommand * button=GetButton(xPos, yPos);
    if(focus)
      PaintButton(focus, button==focus);
    else
    {
      state=0;
      ReleaseCapture();
    }
    }
    break;
  case 0:
    if(popup)
    {
      int ht=NcHitTest(xPos, yPos);
      switch(ht){
      case HTTOP:
      case HTBOTTOM:
        SetCursor(0, IDC_SIZENS);
        break;
      case HTLEFT:
      case HTRIGHT:
        SetCursor(0, IDC_SIZEWE);
        break;
      default:
        {
        SetCursor(0, IDC_ARROW);
        }
        break;
      }
    }
    break;
  case sfresize1:
  case sfresize3:
  case sfresize4:
  case sfresize2:
    {
    // RECT or=trackrect;
    RECT or, or1;
    GetWindowRect(HWindow, &or);
    or1=or;
    RECT wr;
    GetClientRect(HWindow, &wr);
    switch(state){
    case sfresize1:
      {
      POINT _p;
      _p.y=0;
      int y=wr.bottom-yPos;
      for (C_TYPE i=0; i<sizes->count; i++)
      {
        POINT * p=(POINT *)sizes->At(i);
        if((_p.y==0))
          _p=*p;
        else
        {
          if(abs(_p.y-y)>abs(p->y-y))
          { _p=*p;}
        }
      }
      or.right=or.left+_p.x;
      or.top=or.bottom-_p.y;
      }
      break;
    case sfresize2:
      {
      POINT _p;
      _p.x=0;
      for (C_TYPE i=0; i<sizes->count; i++)
      {
        POINT * p=(POINT *)sizes->At(i);
        if((_p.x==0))
          _p=*p;
        else
        {
          if(abs(_p.x-xPos)>abs(p->x-xPos))
          { _p=*p;}
        }
      }
      or.right=or.left+_p.x;
      or.bottom=or.top+_p.y;
      }
      break;
    case sfresize3:
      {
      POINT _p;
      _p.y=0;
      for (C_TYPE i=0; i<sizes->count; i++)
      {
        POINT * p=(POINT *)sizes->At(i);
        if((_p.y==0))
          _p=*p;
        else
        {
          if (abs(_p.y-yPos)>abs(p->y-yPos))_p=*p;
        }
      }
      or.right=or.left+_p.x;
      or.bottom=or.top+_p.y;
      }
      break;
    case sfresize4:
      {
      POINT _p;
      _p.x=0;
      int x=wr.right-xPos;
      for (C_TYPE i=0; i<sizes->count; i++)
      {
        POINT * p=(POINT *)sizes->At(i);
        if((_p.x==0))
          _p=*p;
        else
        {
          if (abs(_p.x-x)>abs(p->x-x))_p=*p;
        }
      }
      or.left=or.right-_p.x;
      or.bottom=or.top+_p.y;
      }
      break;
    }
    if(!EqualRect(&or, &or1))
    {
      //DrawSizeRect();
      trackrect=or;
      moved=TRUE;
      RECT r=or;
      POINT pp;
      pp.x=r.right-r.left;
      pp.y=r.bottom-r.top;
      _Adjust(0, pp, 1);

      SetWindowPos(HWindow, HWND_TOP, r.left, r.top, r.right-r.left, r.bottom-r.top, SWP_NOZORDER|SWP_NOACTIVATE);

    //DrawSizeRect();
    }
    }
    break;
  case sfdrag:
    {
    DrawFocusRect();
    trackpoint.x=xPos;
    trackpoint.y=yPos;
    moved=TRUE;
    DrawFocusRect();
    }
    break;
  }
} ;

void TControlBar::GetRect(int dock_, POINT&p)
{
  if(dock_)
  {
    POINT s;
    s.y=30;
    s.x=3000;
    _Adjust(dock_, s, 0);
    if(dock_&1)
    {
      s.y=BUTTON_HEIGHT+3+BUTTON_OFFSET*2;
      s.y+=2*BUTTON_fY;
    }
    else
    {
      s.x=BUTTON_WIDTH+3+BUTTON_OFFSET*2;
      s.x+=2*BUTTON_fX;
    }
    p=s;
  }
  else
  {
    //  popup=1;
    if(poprect.right-poprect.left<0)
    {
      p.x=3000;
      p.y=30;
      _Adjust(lastdock>0 ? lastdock : 1, p, 0);
    }
    else
    {
      p.x=(poprect.right-poprect.left)+8;
      p.y=poprect.bottom-poprect.top;
      _Adjust(dock_, p, 1);
    }
  //   p.y+=caption+BUTTON_UP;
  //   p.x+=BUTTON_fX*2;
  }
} ;

void TControlBar::DrawSizeRect()
{
  HDC dc=GetDC(0);
  RECT rect=trackrect;
  ::DrawFocusRect(dc,&rect);
  RECT r1=rect;
  rect.left--;
  rect.top--;
  rect.right++;
  rect.bottom++;
  ::DrawFocusRect(dc,&rect);
  rect=r1;
  rect.left++;
  rect.top++;
  rect.right--;
  rect.bottom--;
  ::DrawFocusRect(dc,&rect);
  ReleaseDC(0, dc);
} ;

void TControlBar::DrawFocusRect()
{
  HDC dc=GetDC(0);
  int dock=_dock=mainwindow->IsControlChild(this);

  POINT p;
  GetRect(dock, p);
  POINT org=trackpoint;
  ClientToScreen(HWindow, &org);
  POINT _p=org;
  org.x-=delta.x;
  org.y-=delta.y;
  RECT rect;
  rect.left=org.x;
  rect.top=org.y;
  rect.right=org.x+p.x;
  rect.bottom=org.y+p.y;
  if(rect.right<_p.x)
  {
    rect.right=_p.x+4;
    rect.left=rect.right-p.x;
  }
  if(rect.left>_p.x)
  {
    rect.left=_p.x-4;
    rect.right=rect.left+p.x;
  }
  if(rect.bottom<_p.y)
  {
    rect.bottom=_p.y+4;
    rect.top=rect.bottom-p.y;
  }
  if(rect.top>_p.y)
  {
    rect.top=_p.y-4;
    rect.bottom=rect.top+p.y;
  }
  trackrect=rect;
  if(dock)
  {
    rect.left--;
    rect.top--;
    rect.right++;
    rect.bottom++;
    ::DrawFocusRect(dc,&rect);
  }
  else
  {
    ::DrawFocusRect(dc,&rect);
    RECT r1=rect;
    rect.left--;
    rect.top--;
    rect.right++;
    rect.bottom++;
    ::DrawFocusRect(dc,&rect);
    rect=r1;
    rect.left++;
    rect.top++;
    rect.right--;
    rect.bottom--;
    ::DrawFocusRect(dc,&rect);
  }
  ReleaseDC(0, dc);
} ;

void TControlBar::EvLButtonUp(WORD, int xPos, int yPos)
{
  switch(state){
  case sfdown:
    {
    _TCommand * button=GetButton(xPos, yPos);
    state=0;
    ReleaseCapture();
    if(button==focus)
    {
      if(focus->command->style)
      { commands->Press(focus->command);}
      PaintButton(focus, 0);
      TRange * r=commands->GetRange(focus->command->command);
      if(r)
      {
        if(button->command->pressed)
          ShowControlBar(focus->command->command, TRUE);
        else
        {
          if(r->bar)mainwindow->RemoveControl(r->bar);
        }
      }
      else
      {

#ifdef WIN32

        SendMessage(GetParent(HWindow), WM_COMMAND, MAKELONG(focus->command->command, 0), (LPARAM)HWindow);
#else

        SendMessage(GetParent(HWindow), WM_COMMAND, focus->command->command, 0);
#endif

      }
    }
    else
      PaintButton(focus, 0);
    focus=0;
    }
    break;
  case sfdrag:
    {
    ReleaseCapture();
    DrawFocusRect();
    state=0;
    if(!_dock)
    {
      poprect=trackrect;
      range->poprect=poprect;
    }
    if(moved)
      mainwindow->AdjustControl(this);
    }
    break;
  case sfresize2:
  case sfresize1:
  case sfresize3:
  case sfresize4:
    {
    range->poprect=poprect=trackrect;
    DoneSizeList();
    state=0;
    ReleaseCapture();
    }
    break;
  } //switch
} ;

int TControlBar::GetLenght(int dock_)
{
  if(dock_&1)
  { return sizex;}
  else
  { return sizey;}
} ;

BOOL TControlBar::GetBallonStr(BALLONSTRUCT *bs)
{
  if(state==0)
  {
    _TCommand * _c=GetButton(bs->bsPoint.x, bs->bsPoint.y);
    if(_c&&_c->command)
    {
      if(_c->command->tooltip)
      {
        lstrcpyn(bs->bsText, _c->command->tooltip, bs->bsSize);
        return TRUE;
      }
    }
  }
  return 0;
} ;

_TCommand *TControlBar::GetButton(int xPos, int yPos)
{
  for (C_TYPE i=0; i<range->count; i++)
  {
    _TCommand * _c=(_TCommand *)range->At(i);
    RECT r;
    if(!(_c->command&&_c->command->combo&&_c->HWindow))
    {
      if(_c->GetRect(&r, dock))
      {
        if(r.left<=xPos&&r.top<=yPos&&r.right>xPos&&r.bottom>yPos)
        { return _c;}
      }
    }
  }
  return 0;
} ;

void TControlBar::IdleAction()
{
  for (C_TYPE i=0; i<range->count; i++)
  {
    _TCommand * _c=(_TCommand *)range->At(i);
    if(_c->command)
    {
      int com;
      if(_c->HWindow)
        com=_c->command->combo->cmd;
      else
        com=_c->command->command;
      int code=parent->IsCommandDisable(com);
      BOOL enable=code==0;
      if(_c->HWindow)
        EnableWindow(_c->HWindow, enable);
      else
      {
        if(_c->command->disabled!=(!enable))
        {
          _c->command->disabled=!enable;
          RECT r;
          _c->GetRect(&r, dock);
          InvalidateRect(HWindow, &r, 0);
        }
      }
    }
  }
} ;

BOOL PaintCaptions()
{
  if(!commands)
    return 0;
  for (C_TYPE i=0; i<commands->windows->count; i++)
  {
    TControlBar * bar=(TControlBar *)commands->windows->At(i);
    if(bar->IsPopup())
    {
      HDC hdc=GetDC(bar->HWindow);
      bar->PaintCaption(hdc);
      ReleaseDC(bar->HWindow, hdc);
    }
  }
} ;

void ControlIdle()
{
  if(commands)
  {
    for (C_TYPE i=0; i<commands->windows->count; i++)
    {
      TControlBar * bar=(TControlBar *)commands->windows->At(i);
      bar->IdleAction();
    }
  }
} ;

void TControlBar::EvRButtonDown(WORD modKeys, int xPos, int yPos)
{
  if(state)
    return;
  _TCommand * button=GetButton(xPos, yPos);
  if(!button)
  {
    HMENU p=CreateControBarMenu();
    POINT point;
    point.x=xPos;
    point.y=yPos;
    ClientToScreen(HWindow, &point);
    TSTRATUMMENU sm;
    sm.hWindow=HWindow;
    sm.hMenu=p;
    sm.hType=SCMENU_CONTROLBAR;
    mainwindow->TrackPopupMenu(point, &sm);
  }
  else
  {
    if(button->command->rcommand)
    {
      if(parent->IsCommandDisable(button->command->rcommand)<1)
      {
        HWND mw=GetParent(HWindow);
        _SendCommand(mw, button->command->rcommand);
      }
      else
        MessageBeep(0);
    }
  }
} ;

void TControlBar::EvCommand(UINT command, HWND hwndCtl, WPARAM wNotifyCode)
{
  if(wNotifyCode==CBN_SELENDOK)
  {
    TCommand * _b=commands->Get(command);
    if(_b&&_b->combo)
    {
      SendMessage(hwndCtl, CB_GETLBTEXT, SendMessage(hwndCtl, CB_GETCURSEL, 0, 0), (LPARAM)(_b->combo->currentstr));
      _b->combo->inmsg=1;

#ifdef WIN32

      SendMessage(GetParent(HWindow), WM_COMMAND, MAKELONG(command, 0), (LPARAM)HWindow);
#else

      SendMessage(GetParent(HWindow), WM_COMMAND, command, 0);
#endif

      _b->combo->inmsg=0;
    }
  }
} ;

int TControlBar::NcHitTest(int xPos, int yPos)
{
  if(!popup)
    return HTCAPTION;
  BYTE flag=0;
  int d=4;
  if(yPos<d)
    return HTTOP;
  if(yPos>sizey-d)
    return HTBOTTOM;
  if(xPos<d)
    return HTLEFT;
  if(xPos>sizex-d)
    return HTRIGHT;
  /*
      if(yPos<d)flag|=2;
      if(yPos>sizey-d)flag|=8;
      if(xPos<d)flag|=1;
      if(xPos>sizex-d)flag|=4;
 
      switch(flag){
          case 1:return HTLEFT;
          case 2:return HTTOP;
      case 4:return HTRIGHT;
      case 8:return HTBOTTOM;
      case 3:return HTTOPLEFT;
      case 9:return HTBOTTOMLEFT;
      case 6:return HTTOPRIGHT;
      case 12:return HTBOTTOMRIGHT;
    }
 */
  if(yPos<caption&&xPos>sizex-caption)
    return HTSYSMENU;
  return HTCAPTION;
} ;

void TControlBar::PaintButtons(HDC hdc, RECT&r)
{
  BOOL g=SC_GetScVar("cntbar_grayed", 0);
  _TCommand * _c_=NULL;
  for (C_TYPE i=0; i<range->count; i++)
  {
    _TCommand * _c=(_TCommand *)range->At(i);
    if(_c->IsSeparator())
    {
      _TCommand * __c;
      if(off97&&(i<(range->count-1))&&!alone)
      {
        __c=(_TCommand *)range->At(i+1);
        if((dock&&((dock&1)==0))||((trackrect.right-trackrect.left)<(BUTTON_WIDTH*5/2))&&(dock==0))
        {
          POINT p;
          _c_->GetSize(&p);
          int y=(_c_->_y+p.y+__c->_y)/2;
          int xc;
          if(dock)
            xc=BUTTON_WIDTH;
          else
            xc=(trackrect.right-trackrect.left)-BUTTON_fX*2;
          SelectObject(hdc, Bdark);
          PatBlt(hdc, BUTTON_fX, y-1, xc, 1, PATCOPY);
          SelectObject(hdc, Blight);
          PatBlt(hdc, BUTTON_fX, y, xc, 1, PATCOPY);
        }
        else
        {
          if(_c_&&__c->_y==_c_->_y)
          {
            POINT p;
            _c_->GetSize(&p);
            int x=(_c_->_x+p.x+__c->_x)/2;
            SelectObject(hdc, Bdark);
            PatBlt(hdc, x-1, _c_->_y+BUTTON_fY, 1, BUTTON_HEIGHT, PATCOPY);
            SelectObject(hdc, Blight);
            PatBlt(hdc, x, _c_->_y+BUTTON_fY, 1, BUTTON_HEIGHT, PATCOPY);
          }
        }
      }
    }
    else
    { PaintButton(hdc, _c, g);}
    _c_=_c;
  }
} ;

void TControlBar::PaintButton(HDC hdc, _TCommand *_c, BOOL _gr)
{
  if(_c->IsSeparator())
  { return;}
  if(!IsButton(_c))
    return;
  POINT p;
  _c->GetSize(&p);
  int flags=BUTTON_OFFSET==0 ? 2 : 0;
  if(_c->Pressed())
    flags|=1;
  if(off97)
    flags|=4;
  HDC mdc=CreateCompatibleDC(hdc);
  if(off97&&highlight!=_c)
  {
    if((flags&1))
      DrawShadowFrame(hdc, _c->_x, _c->_y, p.x, p.y, flags);
    else
    {
      SelectObject(hdc, Bface);
      PatBlt(hdc, _c->_x, _c->_y, p.x, p.y, PATCOPY);
    }
  }
  else
  { DrawShadowFrame(hdc, _c->_x, _c->_y, p.x, p.y, flags);}
  int x=_c->_x+1+BUTTON_OFFSET;
  int y=_c->_y+1+BUTTON_OFFSET;
  if(flags&1)
  {
    x++;
    y++;
  }
  if(!_c->command->disabled||(!_gr))
  {
    if((flags&1)&&(_c->command->pressed)&&(!_c->command->down))
    {
      HGDIOBJ old=SelectObject(hdc, BGrayed);
      COLORREF bk=SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
      COLORREF text=SetTextColor(hdc, GetSysColor(COLOR_BTNHIGHLIGHT));
      PatBlt(hdc, x, y, _c->command->GetWidth(), BUTTON_HEIGHT, PATCOPY);
      SetBkColor(hdc, bk);
      SetTextColor(hdc, text);
      SelectObject(hdc, old);
    }
    else
    { flags|=2;}
    HGDIOBJ old=SelectObject(mdc, commands->hmask);

    BitBlt(hdc, x, y, _c->command->GetWidth(), BUTTON_HEIGHT, mdc, _c->command->_x, _c->command->_y, SRCAND);
    SelectObject(mdc, commands->hbuttons);
  }
  else
    SelectObject(mdc, commands->hdisabled);
  BitBlt(hdc, x, y, _c->command->GetWidth(), BUTTON_HEIGHT, mdc, _c->command->_x, _c->command->_y,
         (_c->command->disabled&&_gr) ? SRCCOPY : SRCINVERT);
  DeleteDC(mdc);
} ;

BOOL TControlBar::IsButton(_TCommand *c)
{
  if(c&&c->command)
  {
    if(!c->HWindow)return TRUE;
  }
  return FALSE;
} ;

void TControlBar::PaintCaption(HDC hdc)
{
  HGDIOBJ b=CreateSolidBrush(GetSysColor(taskactive ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION));
  HGDIOBJ old=SelectObject(hdc, b);
  PatBlt(hdc, 3, BUTTON_UP, sizex-6, caption, PATCOPY);
  char s[128];
  GetWindowText(HWindow, s, sizeof(s));
  RECT r;
  r.left=BUTTON_fX;
  r.top=BUTTON_UP;
  r.right=sizex-(BUTTON_fY+10);
  r.bottom=BUTTON_UP+caption;
  SelectObject(hdc, HFont);
  SetBkMode(hdc, TRANSPARENT);
  COLORREF oldtext=SetTextColor(hdc, GetSysColor(taskactive ? COLOR_CAPTIONTEXT : COLOR_INACTIVECAPTIONTEXT));
  ExtTextOut(hdc, 6, BUTTON_UP, ETO_CLIPPED, &r, s, lstrlen(s), NULL);
  SetTextColor(hdc, oldtext);
  DrawShadowFrame(hdc, sizex-1-caption, BUTTON_UP+1, caption-2, caption-2, 2);
  SelectObject(hdc, GetStockObject(BLACK_PEN));
  POINT pp[2];
  POINT _pp[2];
  int _d=2;
  pp[0].x=sizex-1-caption+_d;
  pp[0].y=BUTTON_UP+1+_d;
  pp[1].x=pp[0].x+caption-7;
  pp[1].y=pp[0].y+caption-7;
  _pp[0]=pp[0];
  _pp[1]=pp[1];
  Polyline(hdc, pp, 2);
  _pp[0].x++;
  _pp[1].x++;
  Polyline(hdc, _pp, 2);

  _d=pp[0].x;
  pp[0].x=pp[1].x-1;
  pp[1].x=_d-1;
  Polyline(hdc, pp, 2);

  _pp[0]=pp[0];
  _pp[1]=pp[1];
  _pp[0].x++;
  _pp[1].x++;
  Polyline(hdc, _pp, 2);

  SelectObject(hdc, old);
  DeleteObject(b);
} ;

LRESULT TControlBar::WndProc(UINT m, WPARAM wParam, LPARAM lParam)
{
  switch(m){
  case WM_CAPTURECHANGED:
    if(state)
    {
      state=0;
      ReleaseCapture();
    }
    break;
  case WM_TIMER:
    {
    if(off97)
    {
      POINT p;
      GetCursorPos(&p);
      HWND hw=WindowFromPoint(p);
      if(hw!=HWindow)
      {
        if(highlight)
        {
          _TCommand * _highlight=highlight;
          highlight=NULL;
          HDC hdc=GetDC(HWindow);
          PaintButton(hdc, _highlight, 0);
          ReleaseDC(HWindow, hdc);
        }
        KillTimer(HWindow, ID_TIMER);
        htimer=0;
      }
    }
    }
    return 0;
  case WM_MOUSEACTIVATE:
    if(popup)
    {
      // SendMessage(mainwindow->HWindow,m,wParam,lParam);
      HWND p=GetLastActivePopup(mainwindow->HWindow);
      if(p&&p!=HWindow)
      {
        //   char s[128]="";
        //   GetClassName(p,s,sizeof(s));
        TWindow * w=(TWindow *)SendMessage(p, scwnd_message, 0, 0);
        if(!w)
        { return MA_NOACTIVATEANDEAT;}
      }
    }
    break;
  case WM_PAINT:
    {
    HDC hdc;
    PAINTSTRUCT ps;
    hdc=BeginPaint(HWindow, &ps);
    int nWidth=ps.rcPaint.right-ps.rcPaint.left;
    int nHeight=ps.rcPaint.bottom-ps.rcPaint.top;
    /*
       if(nWidth<1 || nHeight<1){
         _Message("Control bar | nWidth nHeight");
       }
       if(ps.rcPaint.left<0 || ps.rcPaint.top<0){
         _Message("Control bar | top left");
       }
    */
    HDC mdc=CreateCompatibleDC(hdc);
    if(!mdc)
      _Message("Control bar | HDC");
    HBITMAP hb=CreateCompatibleBitmap(hdc, nWidth, nHeight);
    SelectObject(mdc, hb);
    POINT oldv;
    SetViewportOrgEx(mdc, -ps.rcPaint.left, -ps.rcPaint.top, &oldv);
    if(popup)
    {
      DrawShadowFrame(mdc, 0, 0, sizex, sizey, 2);
      PaintCaption(mdc);
    }
    else
    {
      SelectObject(mdc, Bface);
      PatBlt(mdc, 0, 0, sizex, sizey, PATCOPY);
      if(off97&&!alone)
      {
        if(!(dock&1))
          DrawShadowFrame(mdc, 2, 3, sizex-4, 3, 4);
        else
          DrawShadowFrame(mdc, 3, 2, 3, sizey-4, 4);
      }
    }
    PaintButtons(mdc, ps.rcPaint);
    SetViewportOrgEx(mdc, oldv.x, oldv.y, &oldv);
    if(!BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, nWidth, nHeight, mdc, 0, 0, SRCCOPY))
    { _Message("Control bar | BitBlt");}
    EndPaint(HWindow, &ps);

    DeleteDC(mdc);
    DeleteObject(hb);
    }
    return 0;
  /*
    case CB_DONECOMBO:{
    for(int i=0;i<items->count;i++){
    TControlRange *r=(TControlRange *)items->At(i);
    if (r->combo)
    SendMessage(r->combo,CB_SHOWDROPDOWN,(WPARAM)FALSE, 0);
    }
    return 0;
    }
    */
  case CB_IDLE:
    IdleAction();
    return 0;
  //  case CB_SETSCALE:SetScale(*((double*)l));return 0;
  //  case CB_GETSCALE:*((double*)l)=GetScale();return 0;
  //  case CB_SETRANGE:SetActiveRange(w);return 0;
  //  case CB_PRESSBUTTON:PressButton(w,l!=0);return 0;
  }
  return TWindow::WndProc(m, wParam, lParam);
} ;

TControlBar * _ControlBar=0;

TControlDock::TControlDock(int m): TCollection(2, 2)
{
  mode=m;
  if(mode&1)
    size=BUTTON_HEIGHT+3+BUTTON_fY*2;
  else
    size=BUTTON_HEIGHT+3+BUTTON_fX*2;
//  commands->docks[mode-1]=this;
} ;

 int TControlDock::ChkBar(TControlBar *bar)
{
  POINT p, ps;
  p.x=bar->trackrect.left;
  p.y=bar->trackrect.top;
  ps.x=bar->trackrect.right;
  ps.y=bar->trackrect.bottom;
  ScreenToClient(mainwindow->HWindow, &p);
  ScreenToClient(mainwindow->HWindow, &ps);
  int prepos=0;
  if(mode&1)
  {
    prepos=p.x;
    if(ps.x>_size)
      p.x-=ps.x-_size;
    if(p.x<0)
      p.x=0;
  }
  else
  {
    // ¬ертикальныe
    prepos=p.y;
    int s=mainwindow->docks[0]->GetSize();
    p.y-=s;
    if(ps.y>_size)
      p.y-=ps.y-_size;
    if(p.y<0)
      p.y=0;
  }
  if(mode&1)
  { bar->start=p.x;}
  else
  { bar->start=p.y;}
  bar->dock=mode;
  return prepos;
} ;

void TControlDock::Recalc()
{
  for (C_TYPE i=0; i<count; i++)
  {
    TCollection * c=(TCollection *)At(i);
    int delta=0;
    for (C_TYPE j=0; j<c->count; j++)
    {
      RECT d;
      TControlBar * b=(TControlBar *)c->At(j);
      GetWindowRect(b->HWindow, &d);
      MapWindowPoints(0, mainwindow->HWindow, (POINT *)&d, 2);
      POINT size;
      b->GetRect(b->dock, size);
      if(mode&1)
      {
        SetWindowPos(b->HWindow, 0, d.left+delta, d.top, size.x, size.y, SWP_NOZORDER);
        delta=size.x-(d.right-d.left);
      }
      else
      {
        SetWindowPos(b->HWindow, 0, d.left, d.top+delta, size.x, size.y, SWP_NOZORDER);
        delta=size.y-(d.bottom-d.top);
      }
    /*
             for(C_TYPE k=0;k<b->range->count;k++){
               _TCommand* _c=(_TCommand*)b->range->At(k);
               if(_c->HWindow){
    #ifdef WIN32
          long s=GetWindowLong(_c->HWindow,GWL_EXSTYLE);
          if(off97)s&=~WS_EX_CLIENTEDGE;else s|=WS_EX_CLIENTEDGE;
          SetWindowLong(_c->HWindow,GWL_EXSTYLE,s);
    #else
    #endif
               }
             }
    */
    }
  }
} ;

TCollection *TControlDock::InsertNew(TControlBar *bar)
{
  TCollection * c=new TCollection(2, 2);
  ChkBar(bar);
  c->Insert(bar);
  return c;
} ;

BOOL TControlDock::InsertBar(TControlBar *bar, int flags, C_TYPE line)
{
  bar->lastdock=mode;
  BOOL use=(flags&1)!=0;
  BOOL low=(flags&2)!=0;
  if(use&&count)
  {
    //  Insert(InsertNew(bar));
    POINT p;
    p.y=(bar->trackrect.bottom+bar->trackrect.top)/2;
    p.x=(bar->trackrect.right+bar->trackrect.left)/2;
    ScreenToClient(mainwindow->HWindow, &p);
    int ssize=GetSize();
    switch(mode){
    case 1:
      break;
    case 2:
      p.y=mainwindow->mdirect.right+GetSize()-p.x;
      break;
    case 3:
      p.y=(mainwindow->mdirect.bottom+GetSize()-p.y);
      break;
    case 4:
      p.y=p.x;
      break;
    }
    if(p.y<2)
    {
      AtInsert(0, InsertNew(bar));
      goto m1;
    } // ¬верх
    C_TYPE i=p.y/size;
    if(low&&i>line)
      i--;
    if(i>=count)
    {
      Insert(InsertNew(bar));
      goto m1;
    }
    TCollection * _c=(TCollection *)At(i);
    int l=bar->GetLenght(mode);
    int mp=ChkBar(bar)+l/2;

    BOOL ins=FALSE;
    for (i=0; i<_c->count; i++)
    {
      TControlBar * _b=(TControlBar *)_c->At(i);

      int _mp=_b->start+_b->GetLenght(mode)/2;
      if(mp<_mp)
      {
        _c->AtInsert(i, bar);
        ins=TRUE;
        break;
      }
    }
    if(!ins)
      _c->Insert(bar);
    int pos=0;
    for (i=0; i<_c->count; i++)
    {
      TControlBar * _b=(TControlBar *)_c->At(i);
      if(i==0)
      {
        if(_b->start<0)_b->start=0;
      }
      else
      {
        if(_b->start<(pos))_b->start=pos;
      }
      pos=_b->start+_b->GetLenght(mode)+2;
    }
  }
  else
  { Insert(InsertNew(bar));}
  m1:
      bar->Adjust();
  return TRUE;
} ;

BOOL TControlDock::Adjust(TControlBar *bar)
{
  C_TYPE _count=count;
  C_TYPE line=Remove(bar);
  int flags=1;
  if(_count>count)
    flags|=2;
  return InsertBar(bar, flags, line);
}

C_TYPE TControlDock::Remove(TControlBar *bar)
{
  for (C_TYPE i=0; i<count; i++)
  {
    TCollection * c=(TCollection *)At(i);
    for (C_TYPE j=0; j<c->count; j++)
    {
      TControlBar * cb=(TControlBar *)c->At(j);
      if(cb==bar)
      {
        c->AtDelete(j);
        if(!c->count)
        {
          AtDelete(i);
          delete c;
        }
        return i;
      }
    }
  }
  return -1;
}

int TControlDock::GetSize()
{
  if(count)
  {
    if(mode==2||mode==4)
    { return count *(size+2)-2;}
    return count *(size+2);
  }
  return 0;
} ;

// commands
TRange::TRange(char*_n): TCollection(10, 10)
{
  hidden=FALSE;
  bar=NULL;
  id=0;
  lastdock=1;
  ispopup=TRUE;
  window=0;
  poprect.left=poprect.right=poprect.top=poprect.bottom=0;
  name=NewStr(_n);
  wasremove=FALSE;
} ;

TRange::~TRange()
{
  FreeAll();
  DeleteAll();
  delete name;
} ;

void TRange::Store(TStream&st)
{
  WORD b=0;
  if(ispopup)
    b|=1;
  if(wasremove)
    b|=2;
  if(bar)
    b|=4;
  st.WriteWord(b);
  st.WriteWord(lastdock);
  st.WriteWord(start);
  st.WriteWord(poprect.left);
  st.WriteWord(poprect.right);
  st.WriteWord(poprect.top);
  st.WriteWord(poprect.bottom);
} ;

void TRange::Restore(TStream&st)
{
  WORD b=st.ReadWord();
  ispopup=(b&1)!=0;
  wasremove=(b&2)!=0;
  window=(b&4)!=0;
  lastdock=st.ReadWord();
  start=st.ReadWord();
  poprect.left=st.ReadWord();
  poprect.right=st.ReadWord();
  poprect.top=st.ReadWord();
  poprect.bottom=st.ReadWord();
} ;

_TCommand::_TCommand(TCommand *c)
{
  command=c;
  HWindow=NULL;
} ;

 int _TCommand::GetWidth()
{
  if(!command)
    return 6;
  if(HWindow)
    return command->combo->sizex;
  return command->GetWidth();
} ;

BOOL _TCommand::Pressed(){
//return ((command->down!=0)^((command->pressed!=0)));
  return ((command->down!=0)|((command->pressed!=0)));};

BOOL _TCommand::GetSize(POINT *p)
{
  if(command)
  {
    p->x=BUTTON_WIDTH+BUTTON_OFFSET*2+3;
    if(HWindow)
      p->x=command->combo->sizex;
    p->y=BUTTON_HEIGHT+BUTTON_OFFSET*2+3;
  }
  else
  {
    p->x=6;
    p->y=0;
  }
  return TRUE;
} ;

BOOL _TCommand::GetRect(RECT *r, int dock)
{
  if(!IsSeparator())
  {
    r->left=_x;
    r->top=_y;
    if(dock==2||dock==4)
    { r->right=r->left+BUTTON_WIDTH+BUTTON_OFFSET*2+3;}
    else
    { r->right=r->left+GetWidth()+BUTTON_OFFSET*2+3;}
    r->bottom=r->top+BUTTON_HEIGHT+BUTTON_OFFSET*2+3;
    return TRUE;
  }
  return FALSE;
} ;

TCommands::TCommands(): TCollection(200, 50)
{
  LoadBitmaps();
  ranges=new TCollection(10, 10);
  windows=new TCollection(10, 10);
  //формируем панели инструментов
  ReadFromTemlpate();
} ;

TRange *TCommands::GetRange(int id)
{
  for (C_TYPE i=0; i<ranges->count; i++)
  {
    TRange * r=(TRange *)ranges->At(i);
    if(r->id==id)
      return r;
  }
  return NULL;
} ;

TRange *TCommands::GetRange(char*name)
{
  for (C_TYPE i=0; i<ranges->count; i++)
  {
    TRange * r=(TRange *)ranges->At(i);
    if(!lstrcmpi(r->name, name))
      return r;
  }
  return NULL;
} ;

void TCommands::Press(TCommand *c)
{
  if(!c) return;
  int group=(c->style==TBS_CHECKGROUP) ? c->group : 0;
  for (C_TYPE i=0; i<windows->count; i++)
  {
    TControlBar *bar=(TControlBar *)windows->At(i);
    TRange *r=bar->range;
    for (C_TYPE j=0; j<r->count; j++)
    {
      _TCommand *_c=(_TCommand *)r->At(j);
      if(_c->command)
      {
        BOOL u=FALSE;
        if(group)
        {
          if(_c->command->group==group)
          {
            if(_c->command->command==c->command)
            {
              u=TRUE;
              c->pressed=TRUE;
              c->down=FALSE;
            }
            else
            {
              u=TRUE;
              _c->command->pressed=FALSE;
            }
          }
        }
        else
        {
          if(_c->command==c)
          {
            if(c->style==TBS_CHECK)
            {
              c->pressed=!c->pressed;
              u=TRUE;
            }
          }
        }
        if(u)
        {
          RECT r;
          _c->GetRect(&r, bar->dock);
          InvalidateRect(bar->HWindow, &r, NULL);
        }
      }
    }
  }
} ;

TCommands::~TCommands()
{
  DoneBitmaps();
  FreeAll();
  DeleteAll();
  for (C_TYPE i=0; i<ranges->count; i++)
  { delete (TRange *)ranges->At(i);}
  ranges->DeleteAll();
  delete ranges;
  delete windows;
} ;

#define DSK_VER 3

void TCommands::Store(TStream&st)
{
  st.WriteWord(DSK_VER);
  st.WriteStr("bars");
  st.WriteWord(ranges->count);
  long * pos=new long[ranges->count+1];
  for (C_TYPE j=0; j<ranges->count; j++)
  {
    TRange * r=(TRange *)ranges->At(j);
    if(DSK_VER==1)
      st.WriteStr(r->name);
    else
    { st.WriteWord(r->id);}
    pos[j]=st.GetPos();
    st.WriteLong(0);
  }
  long pos1=st.GetPos();
  st.WriteLong(0);
  for (j=0; j<ranges->count; j++)
  {
    TRange * r=(TRange *)ranges->At(j);
    long p=st.GetPos();
    st.Seek(pos[j]);
    st.WriteLong(p);
    st.Seek(p);
    r->Store(st);
  }
  long _pos=st.GetPos();
  st.Seek(pos1);
  st.WriteLong(_pos);
  st.Seek(_pos);
  delete pos;
  st.WriteWord(0xfff);
  for (int i=0; i<4; i++)
  {
    st.WriteWord(docks[i]->count);
    for (C_TYPE j=0; j<docks[i]->count; j++)
    {
      TCollection * _c=(TCollection *)docks[i]->At(j);
      st.WriteWord(_c->count);
      for (C_TYPE k=0; k<_c->count; k++)
      {
        TControlBar * bar=(TControlBar *)_c->At(k);
        //st.WriteStr(bar->range->name);
        if(DSK_VER<3)
          st.WriteStr(bar->range->name);
        else
          st.WriteWord(bar->range->id);
        st.WriteWord(bar->start);
      }
    }
  }
} ;

void TCommands::Restore(TStream&st)
{
  if(st.status==0)
  {
    INT16 v=st.ReadWord();
    char s[128];
    st.ReadStr(s, sizeof(s));
    if(v<=DSK_VER)
    {
      INT16 count_=st.ReadWord();
      for (C_TYPE i=0; i<count_; i++)
      {
        TRange * r;
        if(v<2)
        {
          st.ReadStr(s, sizeof(s));
          r=GetRange(s);
        }
        else
        {
          int id=st.ReadWord();
          r=GetRange(id);
        }
        long rp=st.ReadLong();
        if(r)
        {
          {
            long p=st.GetPos();
            st.Seek(rp);
            r->Restore(st);
            st.Seek(p);
          }
        }
      }
      long _pos=st.ReadLong();
      st.Seek(_pos);

      count_=st.ReadWord();
      if(count_==0xfff)
      {
        for (int i=0; i<4; i++)
        {
          count_=st.ReadWord();
          for (C_TYPE j=0; j<count_; j++)
          {
            C_TYPE _count=st.ReadWord();
            TCollection * _c=NULL;
            for (C_TYPE k=0; k<_count; k++)
            {
              TRange * r=NULL;
              if(v<3)
              {
                st.ReadStr(s, sizeof(s));
                r=GetRange(s);
              }
              else
              {
                r=GetRange(st.ReadWord());
                if(r)
                { lstrcpyn(s, r->name, sizeof(s));}
              }
              int start=st.ReadWord();
              if(r)
              {
                if(!_c)
                  _c=new TCollection(2, 2);
                int dock=i+1;
                DWORD style=WS_CLIPCHILDREN|WS_CHILD|WS_CLIPSIBLINGS|dock;
                HWND controlbar=CreateWindow(ControlBarName,       s, style, 0, 0, 0, 0, mainwindow->HWindow, 0,
                                             (HINSTANCE)hInstance, NULL);
                TControlBar * bar=(TControlBar *)(GetWindowLong(controlbar, 0));
                bar->dock=dock;
                bar->start=start;
                bar->Adjust();
                _c->Insert(bar);
              }
            }
            if(_c)
              docks[i]->Insert(_c);
          }
        }
      }
      // CreatePopup
      for (i=0; i<ranges->count; i++)
      {
        TRange * r=(TRange *)ranges->At(i);
        if((r->window)&&(r->bar==NULL))
        { mainwindow->InsertControl(r->name, 0);}
      }
    }
    else
      _Message(_M_BADDESKTOPFILE);
  }
} ;

//формируем панели инструментов
void TCommands::ReadFromTemlpate()
{
  char cnt_tpl[256];
  GetTemplateDir(cnt_tpl);
  char _s[128];
  LoadString(HResource, IDS_str121, _s, sizeof(_s)); //добавл€ем им€ файла шаблонов к пути "\\commands.tpl"
  lstrcat(cnt_tpl, _s);
  TDOSStream st(cnt_tpl, TDOSStream::stOpenRead);
  TTplReader t(&st);
  tpl=&t;
  if(!st.status)
  {
    int code;
    while(ReadKeyWord(code))
    {
      switch(code){
      case K_COMMAND:
        if(ReadCommand())
          break;
        goto m1;
      case K_RANGE:
        if(ReadRange())
          break;
        goto m1;
      default:
        ;
      }
    } ;
    m1:
  }
}

BOOL TCommands::ReadKeyWord(int&code){return tpl->ReadKeyWord(code, keyWords, sizeof(keyWords)/4);};

BOOL TCommands::ReadInt(int&i){return tpl->ReadInt(i);};

BOOL TCommands::ReadChar(char c){return tpl->ReadChar(c);};

BOOL TCommands::ReadStr(char*str, int _len){return tpl->ReadStr(str, _len);};

BOOL TCommands::ReadSpaces(){return tpl->ReadSpaces();};

TCombo *TCommands::ReadCombo(BOOL&ok)
{
  if(ReadChar('{'))
  {
    TCombo * combo=new TCombo(0, 0);
    int code;
    while(ReadKeyWord(code))
    {
      switch(code){
      case K_STRINGS:
        if(ReadChar('{'))
        {
          char s[128];
          do
          {
            if(!ReadStr(s, sizeof(s)))
              return combo;
            combo->Insert(NewStr(s));
            if(ReadChar('!'))
            { lstrcpyn(combo->currentstr, s, 128);}
            if(ReadChar('}'))
              break;
          }
          while(ReadChar(','));
        }
        break;
      case K_LISTONLY:
        combo->listonly=TRUE;
        break;
      case K_SIZE:
        if(!ReadInt(combo->sizex))
          return combo;
        break;
      case K_COMMAND:
        if(!ReadInt(combo->cmd))
          return combo;
        break;
      case K_END:
        ok=TRUE;
        return combo;
      }
    }
  }
  return NULL;
} ;

BOOL TCommands::ReadCommand()
{
  if(ReadChar('{'))
  {
    int group=0, id=0, style=0, width=0, command;
    char hint[128]="";
    char name[64]="";
    char menu[128]="";
    int code;
    int rbutton=0;
    TCombo * combo=NULL;
    if(!ReadInt(command))
      return FALSE;
    while(ReadKeyWord(code))
    {
      switch(code){
      case K_COMBO:
        {
        BOOL s=0;
        combo=ReadCombo(s);
        if(!s)
          goto m1;
        }
        break;
      case K_RBUTTON:
        if(!ReadInt(rbutton))
          goto m1;
        break;
      case K_HINT:
        if(!ReadStr(hint, sizeof(hint)))
          goto m1;
        break;
      case K_NAME:
        if(!ReadStr(name, sizeof(name)))
          goto m1;
        break;
      case K_MENUTEXT:
        if(!ReadStr(menu, sizeof(menu)))
          goto m1;
        break;
      case K_GROUP:
        if(!ReadInt(group))
          goto m1;
        break;
      case K_WIDTH:
        if(!ReadInt(group))
          goto m1;
        break;
      case K_ID:
        if(!ReadInt(id))
          goto m1;
        break;
      case K_STYLE:
        if(!ReadInt(style))
          goto m1;
        break;
      case K_END:
        {
        int perline=bmpsizex/(BUTTON_WIDTH+1);
        if(id>-1)
        {
          int c1=id/perline;
          int c2=id%perline;
          int _s2=(c1) *(BUTTON_HEIGHT+1);
          int _s1=(c2) *(BUTTON_WIDTH+1);
          if(c1) _s2++;
          if(c2) _s1++;
          TCommand *c=new TCommand(name, menu, hint, command, id, width, group, style, _s1, _s2);
          c->rcommand=rbutton;
          c->combo=combo;
          if(combo)
          { combo->command=c;}
          Insert(c);
        } ;
        return TRUE;
        }
      }
    }
    m1:
        if(combo)
          delete combo;
  }
  return FALSE;
} ;

BOOL TCommands::ReadRange()
{
  char name[64];
  if(ReadStr(name, sizeof(name)))
  {
    if(ReadChar('{'))
    {
      TRange * range=new TRange(name);
      ranges->Insert(range);
      char _name[64];
      do
      {
        TCommand * c=NULL;
        BOOL separator=FALSE;
        if(ReadStr(_name, sizeof(_name)))
        { c=Get(_name);}
        else
        {
          int code;
          if(ReadInt(code))
          { c=Get(code);}
          else
          {
            int _code;
            if(ReadKeyWord(_code))
            {
              switch(_code){
              case K_HIDDEN:
                range->hidden=TRUE;
                break;
              case K_SEPARATOR:
                separator=TRUE;
                break;
              case K_ID:
                if(!ReadInt(range->id))
                  return FALSE;
                break;
              case K_DOCK:
                if(!ReadInt(range->lastdock))
                  return FALSE;
                range->ispopup=FALSE;
                break;
              default:
                return FALSE;
              }
            }
            else
              return FALSE;
          } ;
        }
        if(separator)
          range->Insert(new _TCommand(NULL));
        else
        {
          if(c)range->Insert(new _TCommand(c));
        }
      }
      while(ReadChar(','));
      if(ReadChar('}'))
        return TRUE;
    }
  }
  return FALSE;
} ;

//получаем описание команды по ее имени
TCommand *TCommands::Get(char*name)
{
  for (C_TYPE i=0; i<count; i++)
  {
    TCommand * c=(TCommand *)At(i);
    if(c->name&&(!lstrcmp(c->name, name)))
      return c;
  }
  return NULL;
} ;

//получаем описание команды по ее номеру
TCommand *TCommands::Get(int command)
{
  for (C_TYPE i=0; i<count; i++)
  {
    TCommand * c=(TCommand *)At(i);
    if(c->command==command||(c->combo&&c->combo->cmd==command))
      return c;
  }
  return NULL;
} ;

void ControlColorChanged()
{
  if(commands)
  {
    commands->DoneBitmaps();
    commands->LoadBitmaps();
  }
} ;

void TCommands::DoneBitmaps()
{
  DeleteObject(hbuttons);
  DeleteObject(hdisabled);
  DeleteObject(hmask);
} ;

//загружаем изображени€ кнопок панели инструментов
void TCommands::LoadBitmaps()
{
  //получаем ссылку на ресурс с изображени€ми кнопок
  hbuttons=LoadBitmap(HResource, "CONTROL_BUTTONS");
  BITMAP bm;
  //загружаем изображение из ресурса в объект bm
  GetObject(hbuttons, sizeof(bm), &bm);
  bmpsizex=bm.bmWidth;
  bmpsizey=bm.bmHeight;

  //создаем битовую карту
  hmask=CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

  HDC maskDC, hDC, wdc, dDC, maskDC1;
  wdc=GetDC(0);
  hDC=CreateCompatibleDC(wdc);
  maskDC=CreateCompatibleDC(wdc);
  COLORREF _color;
  {
  SelectObject(hDC, hbuttons);
  _color=GetPixel(hDC, 0, 0);
  HGDIOBJ pen=CreatePen(PS_SOLID, 0, _color);
  HGDIOBJ old=SelectObject(hDC, pen);

  int perline=bmpsizex/(BUTTON_WIDTH+1);
  int perw=bmpsizey/(BUTTON_HEIGHT+1);

  POINT pp[2];
  pp[0];
  pp[0].x=pp[0].y=0;
  pp[1];
  pp[1].x=bmpsizex;
  pp[1].y=0;
  for (int i=0; i<perw; i++)
  {
    pp[0].y=pp[1].y=(BUTTON_HEIGHT+1)*i;
    Polyline(hDC, pp, 2);
  }
  pp[0].x=pp[0].y=0;
  pp[1].x=0;
  pp[1].y=bmpsizey;
  for (i=0; i<perline; i++)
  {
    pp[0].x=pp[1].x=(BUTTON_WIDTH+1)*i;
    Polyline(hDC, pp, 2);
  }
  SelectObject(hDC, old);
  DeleteObject(pen);
  }
  SelectObject(maskDC, hmask);
  dDC=CreateCompatibleDC(wdc);
  maskDC1=CreateCompatibleDC(wdc);

  hdisabled=CreateCompatibleBitmap(wdc, bm.bmWidth, bm.bmHeight);
  SelectObject(dDC, hdisabled);
  BitBlt(dDC, 0, 0, bmpsizex, bmpsizey, hDC, 0, 0, SRCCOPY);
  PatBlt(maskDC, 0, 0, bmpsizex, bmpsizey, WHITENESS);

  SetBkColor(hDC, _color); // btnface to white
  BitBlt(maskDC, 0, 0, bmpsizex, bmpsizey, hDC, 0, 0, SRCCOPY);

  SetBkColor(hDC, _color); // btnface to white
  BitBlt(hDC, 0, 0, bmpsizex, bmpsizey, maskDC, 0, 0, SRCPAINT);
  hmask=CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

  SelectObject(maskDC1, hmask);
  BitBlt(maskDC1, 0, 0, bmpsizex, bmpsizey, maskDC, 0, 0, SRCCOPY); //  опируем в маску
  // √отова маска
  //  BitBlt(wdc,0, 0, bmpsizex, bmpsizey,maskDC,0,0,SRCCOPY);

  //__draw
  HBITMAP hd=CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
  SelectObject(maskDC1, hd);
  PatBlt(maskDC1, 0, 0, bmpsizex, bmpsizey, WHITENESS);
  BitBlt(maskDC1, 0, 0, bmpsizex, bmpsizey, maskDC, 0, 0, SRCINVERT);
  //__draw
  SetTextColor(hDC, RGB(0, 0, 0));
  SetBkColor(hDC, RGB(255, 255, 255));
  BitBlt(hDC, 0, 0, bmpsizex, bmpsizey, maskDC1, 0, 0, SRCAND);

  // BitBlt(wdc,0, 0, bmpsizex, bmpsizey,maskDC,0,0,SRCCOPY);

  //__draw
  //  SetTextColor(hDC,0);
  //  SetBkColor(hDC,GetSysColor(COLOR_BTNFACE));
  //  BitBlt(hDC,0, 0, bmpsizex, bmpsizey,maskDC,0,0,SRCPAINT);
  //__draw
  //****************************************************************
  //  √отова маска дл€ нормальных кнопок
  //****************************************************************
  // Generate Disabled
  //__draw
  SetTextColor(dDC, RGB(0, 0, 0));     // 0 to black
  SetBkColor(dDC, RGB(255, 255, 255)); // 1 to white
  SelectObject(dDC, Blight);           // 0 -> highlight color
  BitBlt(dDC, 1, 1, bmpsizex-1, bmpsizey-1, maskDC, 0, 0, RopPSDPxax);
  //__draw
  SelectObject(dDC, Bdark); // 0 -> highlight color
  BitBlt(dDC, 0, 0, bmpsizex, bmpsizey, maskDC, 0, 0, RopPSDPxax);
  //__draw
  SetTextColor(maskDC1, 0);
  SetBkColor(maskDC1, RGB(255, 255, 255));

  // BitBlt(wdc,0, 0, bmpsizex, bmpsizey,maskDC1,0,0,SRCCOPY);
  PatBlt(maskDC, 0, 0, bmpsizex, bmpsizey, DSTINVERT);
  BitBlt(maskDC1, 1, 1, bmpsizex-1, bmpsizey-1, maskDC, 0, 0, SRCPAINT);
  SetTextColor(dDC, RGB(0, 0, 0));
  SetBkColor(dDC, RGB(255, 255, 255));
  BitBlt(dDC, 0, 0, bmpsizex, bmpsizey, maskDC1, 0, 0, SRCAND); // «ачерн€ем фоновую область
  //  BitBlt(wdc,0, 0, bmpsizex, bmpsizey,dDC,0,0,SRCCOPY);
  /////////
  //__draw
  PatBlt(maskDC1, 0, 0, bmpsizex, bmpsizey, DSTINVERT);
  //__draw
  SetTextColor(dDC, 0);
  SetBkColor(dDC, GetSysColor(COLOR_BTNFACE));
  SetTextColor(maskDC1, 0);
  SetBkColor(maskDC1, RGB(255, 255, 255));

  BitBlt(dDC, 0, 0, bmpsizex, bmpsizey, maskDC1, 0, 0, SRCPAINT);
  //  BitBlt(wdc,0, 0, bmpsizex, bmpsizey,dDC,0,0,SRCCOPY);
  //__draw
  // √отова маска дл€ дисабле кнопок

  //  BitBlt(dDC,0, 0, bmpsizex, bmpsizey,maskDC1,0,0,SRCAND);

  DeleteDC(hDC);
  DeleteDC(maskDC);
  DeleteDC(maskDC1);
  DeleteDC(dDC);
  ReleaseDC(0, wdc);
  DeleteObject(hd);
} ;

BOOL InitControlBars(TControlDock *_docks [])
{
  commands=new TCommands();
  for (int i=0; i<4; i++)
    commands->docks[i]=_docks[i];
  return commands->count>0;
} ;

BOOL DoneControlBars()
{
  if(commands)
  {
    delete commands;
    commands=NULL;
    return TRUE;
  }
  return FALSE;
} ;

BOOL StoreControlBars(TStream&st)
{
  commands->Store(st);
  return TRUE;
} ;

BOOL RestoreControlBars(TStream&st)
{
  commands->Restore(st);
  return TRUE;
} ;

BOOL IsControlButtonPressed(int command)
{
  TCommand * _c=commands->Get(command);
  if(_c&&_c->pressed)
    return TRUE;
  return FALSE;
} ;

BOOL GetCommandInfo(int command, char*s)
{
  TCommand * c=commands->Get(command);
  if(c&&c->menutext)
  {
    lstrcpy(s, c->menutext);
    return TRUE;
  }
  return FALSE;
} ;

//¬ыполн€ем нажатие на кнопку панели инструментов
void PressControlButton(int command, int down)
{
  // if (mainwindow)SendMessage(mainwindow->controlbar,CB_PRESSBUTTON,command,down);
  switch(command){
  case CM_RUN:
  case CM_ONESTEP:
  {
    char s[256];
    LoadString(HResource, IDS_str214, s, sizeof(s));
    SCSetStatusText(0, s);
  } ;
  break;
  case CM_PAUSE:
    {
    char s[256]="";
    if(project)
      LoadString(HResource, IDS_str215, s, sizeof(s));
    SCSetStatusText(0, s);
    }
    break;
  }
  if(commands)//провер€ем наличие списка команд
  {
  	//ищем описание команды по ее коду
    TCommand * c=commands->Get(command);
    if(c)
    {
      if(c->style==3)
      {
      	//если состо€ние команды совпадает с down, то выходим
        if(c->pressed==down)return;
      }
      //выполн€ем команду
      commands->Press(c);
    }
  }
} ;

BOOL SwapControlBar(int i)
{
  C_TYPE j=i;
  if(j>-1&&j<commands->ranges->count)
  {
    TRange * r=(TRange *)commands->ranges->At(j);
    if(r->bar)
      mainwindow->RemoveControl(r->bar);
    else
      mainwindow->InsertControl(r->name, r->ispopup ? 0 : r->lastdock);
  }
} ;

void ShowControlBar(int id, BOOL always)
{
  if(id&&commands)
    for (C_TYPE i=0; i<commands->ranges->count; i++)
    {
      TRange * r=(TRange *)commands->ranges->At(i);
      if(r->id==id)
      {
        if(r->bar)
          return;
        if(r->wasremove)
        {
          if(!always)return;
        }
        mainwindow->InsertControl(r->name, r->ispopup ? 0 : r->lastdock);
      }
    }
} ;

//создаем контекстное меню дл€ настройки панелей инструментов
HMENU CreateControBarMenu()
{
  HMENU p=CreatePopupMenu();
  //формируем список панелей инструментов
  for (C_TYPE i=0; i<commands->ranges->count; i++)
  {
    TRange * r=(TRange *)commands->ranges->At(i);
    if(!r->hidden)
      AppendMenu(p, MF_ENABLED|((r->bar) ? MF_CHECKED : 0), CM_CONTROLRANGE+i, r->name);
  }
  AppendMenu(p, MF_SEPARATOR, 0, 0);
  ScAppendMenu(p, MF_ENABLED, CM_CONTROLBARS, IDS_MENU27);
  ScAppendMenu(p, MF_ENABLED, CM_CONTROLPROP, IDS_MENU28);
  return p;
} ;

BOOL GetComboStr(int id, char*s, int size)
{
  if(commands)
  {
    for (C_TYPE i=0; i<commands->ranges->count; i++)
    {
      TRange * r=(TRange *)commands->ranges->At(i);
      for (C_TYPE j=0; j<r->count; j++)
      {
        _TCommand * _c=(_TCommand *)r->At(j);
        if(_c->HWindow&&_c->command->combo&&_c->command->combo->cmd==id)
        {
          {
            lstrcpyn(s, _c->command->combo->currentstr, size);
            return TRUE;
          }
        }
      }
    }
  }
  return FALSE;
} ;

BOOL SetComboStr(int id, char*s)
{
  if(commands)
  {
    for (C_TYPE i=0; i<commands->ranges->count; i++)
    {
      TRange * r=(TRange *)commands->ranges->At(i);
      for (C_TYPE j=0; j<r->count; j++)
      {
        _TCommand * _c=(_TCommand *)r->At(j);
        if(_c->HWindow&&_c->command->combo&&_c->command->combo->cmd==id)
        {
          if(!_c->command->combo->inmsg)
          {
            __SetComboStr(_c, s);
            lstrcpyn(_c->command->combo->currentstr, s, 128);
          }
          return TRUE;
        }
      }
    }
  }
  return FALSE;
} ;

void SetScaleBox(double scale)
{
  char s[32];
  scale*=100.0;
  sprintf(s, "%g%%", scale);
  SetComboStr(CM_SCALEBOX, s);
} ;

double GetScaleBox()
{
  double scale=1.0;
  char s[32];
  if(GetComboStr(CM_SCALEBOX, s, sizeof(s)))
  {
    int l=lstrlen(s);
    if(l>0)
    {
      if(s[l-1]=='%')
      {
        s[l-1]=0;
        scale=atof(s);
        if(scale==0)
          scale=1.0;
        return scale;
      }
    }
    return 1.0;
  }
  return scale;
} ;

HBITMAP list_items=0;

#define LIST_W 13

int ListBoxProc(HWND hwnd, UINT msg, WPARAM&wParam, LPARAM&lParam, int _id)
{
  switch(msg){
  case WM_PARENTNOTIFY:
    {
    #ifdef WIN32
    int fwEvent=LOWORD(wParam); // event flags
    int idChild=HIWORD(wParam); // identifier of child window
    int wValue1=LOWORD(lParam); /* child handle/cursor x-coordinate */
    int wValue2=HIWORD(lParam); /* child ID/cursor y-coordinate     */
		#else
    int fwEvent=wParam;         /* event flags                      */
    int wValue1=LOWORD(lParam); /* child handle/cursor x-coordinate */
    int wValue2=HIWORD(lParam); /* child ID/cursor y-coordinate     */
		#endif
    if(WM_LBUTTONDOWN==fwEvent)
    {
      HWND list=GetDlgItem(hwnd, _id);
      POINT p;
      p.x=wValue1;
      p.y=wValue2;
      //ClientToScreen(hwnd,&p);
      GetCursorPos(&p);
      RECT r;
      GetWindowRect(list, &r);
      if(PtInRect(&r, p))
      {
        int count=SendMessage(list, LB_GETCOUNT, 0, 0);
        ScreenToClient(list, &p);
        for (int i=0; i<count; i++)
        {
          if(SendMessage(list, LB_GETITEMRECT, i, (LPARAM)&r)!=LB_ERR)
          {
            r.right=r.left+LIST_W;
            if(PtInRect(&r, p))
            {
              char s[256];
              SendMessage(list, LB_GETTEXT, i, (LPARAM)s);
              switch(s[0]){
              case 'C':
                s[0]='U';
                break;
              case 'c':
                s[0]='u';
                break;
              case 'U':
                s[0]='g';
                break;
              case 'u':
                s[0]='c';
                break;
              case 'g':
                s[0]='C';
                break;
              default:
                return TRUE;
              }
              SendMessage(list, LB_DELETESTRING, i, (LPARAM)s);
              SendMessage(list, LB_INSERTSTRING, i, (LPARAM)s);
              SendMessage(list, LB_SETCURSEL, i, 0);
              wParam=i;
              lParam=s[0];
              return 2;
            }
          }
        }
        return TRUE;
      }
    }
    }
    break;
  case WM_MEASUREITEM:
    {
		#ifdef WIN32
    HWND list=GetDlgItem(hwnd, _id);
    LONG ex=GetWindowLong(list, GWL_EXSTYLE);
    ex&=~WS_EX_NOPARENTNOTIFY;
    SetWindowLong(list, GWL_EXSTYLE, ex);
		#endif
    int nIDCtl=(int)wParam;
    if(nIDCtl==_id)
    {
      MEASUREITEMSTRUCT FAR *lm=(MEASUREITEMSTRUCT FAR *)lParam;
      HDC dc=GetDC(0);
      SelectObject(dc, GetStockObject(ANSI_VAR_FONT));
      TEXTMETRIC tm;
      GetTextMetrics(dc, &tm);
      lm->itemHeight=tm.tmHeight+tm.tmExternalLeading+1;
      if(lm->itemHeight<LIST_W)
        lm->itemHeight=LIST_W;
      ReleaseDC(0, dc);
      RECT r;
      GetClientRect(GetDlgItem(hwnd, _id), &r);
      lm->itemWidth=r.right-4;
      return TRUE;
    }
  }
  return FALSE;
  case WM_DRAWITEM:
  {
    DRAWITEMSTRUCT FAR *pdis=(DRAWITEMSTRUCT FAR *)lParam;
    if(pdis->CtlID==_id)
    {
      char s[256];
      if(SendMessage(pdis->hwndItem, LB_GETTEXT, pdis->itemID, (LPARAM)s)!=LB_ERR)
      {
        if(!s)
          s[1]=0;
        int x=0;
        switch(s[0]){
        case 'C':
        case 'c':
          x=LIST_W;
          break;
        case 'U':
        case 'u':
          x=0;
          break;
        case 'g':
          x=LIST_W*2;
          break;
        }
        COLORREF rgb=GetSysColor(((pdis->itemState&ODS_SELECTED)!=0) ? COLOR_HIGHLIGHT : COLOR_WINDOW);
        //  COLORREF rgb=((pdis->itemState&ODS_SELECTED)!=0)?RGB(128,0,128):RGB(255,255,255);
        HGDIOBJ b=CreateSolidBrush(rgb);
        HGDIOBJ old=SelectObject(pdis->hDC, b);
        PatBlt(pdis->hDC,                            pdis->rcItem.left,                    pdis->rcItem.top,
               pdis->rcItem.right-pdis->rcItem.left, pdis->rcItem.bottom-pdis->rcItem.top, PATCOPY);
        SelectObject(pdis->hDC, old);
        DeleteObject(b);
        if(!list_items)
        { list_items=LoadBitmap(HResource, MAKEINTRESOURCE(BITMAP_LIST));}
        HDC dc=CreateCompatibleDC(pdis->hDC);
        SelectObject(dc, list_items);
        BitBlt(pdis->hDC, pdis->rcItem.left, pdis->rcItem.top, LIST_W, LIST_W, dc, x, 0, SRCCOPY);
        DeleteDC(dc);
        SetBkMode(pdis->hDC, TRANSPARENT);
        COLORREF oldc=SetTextColor(pdis->hDC, GetSysColor(
                      (pdis->itemState&ODS_SELECTED) ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));
        TextOut(pdis->hDC, pdis->rcItem.left+LIST_W+2, pdis->rcItem.top, s+1, lstrlen(s+1));
        SetTextColor(pdis->hDC, oldc);
        if(pdis->itemState&ODS_FOCUS)
        {
          RECT r=pdis->rcItem;
          r.top++;
          r.bottom--;
          r.right--;
          r.left+=LIST_W+2;
          DrawFocusRect(pdis->hDC, &r);
        }
      }
      return TRUE;
    }
    }
    return FALSE;
  }
  return FALSE;
}

BOOL scCALLBACK ControlBarsProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  if(ListBoxProc(hwnd, msg, wParam, lParam, IDC_BARS))
    return TRUE;
  switch(msg){
  case PSP_INITPAGE:
  {
    for (C_TYPE i=0; i<commands->ranges->count; i++)
    {
      TRange * r=(TRange *)commands->ranges->At(i);
      if(!r->hidden)
      {
        char s[256];
        lstrcpy(s+1, r->name);
        if(r->bar)
          s[0]='c';
        else
          s[0]='u';
        SendDlgItemMessage(hwnd, IDC_BARS, LB_ADDSTRING, 0, (LPARAM)s);
      }
    }
    CheckDlgButton(hwnd, IDC_CNTGRAYED, SC_GetScVar("cntbar_grayed", 0));
    CheckDlgButton(hwnd, IDC_TOOLBAR97, SC_GetScVar("cntbar_style", 0));
    CheckDlgButton(hwnd, IDC_BARSUNVISIBLE, !mainwindow->viewcontrol);
  }
  return TRUE;
  case PSP_CLOSE:
    if(wParam==1)
    {
      SCSetScVarI("cntbar_grayed", IsDlgButtonChecked(hwnd, IDC_CNTGRAYED));
      SCSetScVarI("cntbar_style", IsDlgButtonChecked(hwnd, IDC_TOOLBAR97));
      mainwindow->ShowControl(!IsDlgButtonChecked(hwnd, IDC_BARSUNVISIBLE));
      C_TYPE j=0;
      for (C_TYPE i=0; i<commands->ranges->count; i++)
      {
        TRange * r=(TRange *)commands->ranges->At(i);
        if(!r->hidden)
        {
          char s[256];
          SendDlgItemMessage(hwnd, IDC_BARS, LB_GETTEXT, j, (LPARAM)s);
          BOOL c=s[0]=='c';
          if(c!=(r->bar!=0))
          { SwapControlBar(j);}
          j++;
        }
      }
    }
    return TRUE;
  case WM_COMMAND:
  {
    EV_COMMANDsimply
    switch(command){
    }
    return TRUE;
  }
  break;
  }
  return DefPropertyItemProc(hwnd, msg, wParam, lParam);
} ;

TTplReader::TTplReader(TStream *st)
{
  _input=NULL;
  long size=st->GetSize();
  size=min(32767, size);
  input=_input=new char[(int)size+1];
  st->Read(input, (int)size);
  input[(int)size]=0;
  Precompile();
//	_input=input;
}

TTplReader::~TTplReader()
{
  if(input)delete input;
} ;

int TTplReader::Precompile()
{
  char * i1, * i2;
  i1=i2=input;
  int state=0;
  while(*i1)
  {
    switch(state){
    case 0:
      if(*i1=='/')
      {
        if(*(i1+1)=='/')
        {
          state=1;
          i1+=2;
          break;
        }
        if(*(i1+1)=='*')
        {
          state=2;
          i1+=2;
          break;
        }
      }
      if(*i1==13||*i1==10)
        * i2=' ';
      else
        * i2=*i1;
      i2++;
      break;
    case 1: //  омментарий на строку
      if(*i1==13||*i1==10)
        state=0;
      break;
    case 2: // /*  омментарии
      if(*i1=='*'&&*(i1+1)=='/')
      {
        state=0;
        i1+=2;
      }
      break;
    } ;
    i1++;
  }
  * i2=0;
  return 0;
} ;

BOOL TTplReader::ReadInt(int&i)
{
  if(ReadSpaces())
  {
    long ii=0;
    int signum=1;
    BOOL readnum=FALSE;
    if(*_input=='-')
    {
      signum=-1;
      _input++;
    }
    while(*_input&&*_input>='0'&&*_input<='9')
    {
      ii=ii*10+(*_input-'0');
      if(ii>32768l)
        return FALSE;
      _input++;
      readnum=TRUE;
    }
    i=(int)(ii*signum);
    return readnum;
  }
  return FALSE;
}

BOOL TTplReader::ReadSpaces()
{
  while(*_input&&(*_input==' '||*_input==9))
    _input++;
  return (*_input!=0);
} ;

BOOL TTplReader::ReadKeyWord(int&code, char**keys, int size)
{
  if(ReadSpaces())
  {
    char * start=_input;
    while(*_input&&((*_input>='A'&&*_input<='Z')||(*_input>='a'&&*_input<='z')||(*_input=='_')||(*_input=='}')))
      _input++;
    int len=(int)(_input-start);
    if(len&&len<32)
    {
      char key[33];
      lstrcpyn(key, start, len+1);
      AnsiUpper(key);
      for (int i=0; i<size; i++)
      {
        if(!lstrcmp(keys[i], key))
        {
          code=i;
          return TRUE;
        }
      }
    }
  }
  return FALSE;
} ;

BOOL TTplReader::ReadStr(char*str, int _len)
{
  if(ReadChar('"'))
  {
    int len=0;
    while(*_input&&*_input!='"'&&len<250)
    {
      _input++;
      len++;
    }
    if(*_input&&len<_len)
    {
      _input++;
      lstrcpyn(str, _input-(len+1), len+1);
      return TRUE;
    }
    else
      return 0;
  }
  return 0;
} ;
BOOL TTplReader::ReadChar(char c)
{
  if(ReadSpaces()&&*_input==c)
  {
    _input++;
    return TRUE;
  }
  return FALSE;
} ;
