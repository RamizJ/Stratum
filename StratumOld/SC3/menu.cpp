#include <windows.h>
#include "stream.h"
#include "menu.h"
#include "twindow.h"
#include "messages.h"
char *MenuCodes[]={"","MENU","POPUP","MENUITEM","SEPARATOR","DISABLED","CHECKED","GRAYED"};

#define K_MENU     1
#define K_POPUP    2
#define K_MENUITEM 3
#define K_SEPARATOR 4
#define K_DISABLED 5
#define K_CHECKED  6
#define K_GRAYED   7

TMenu::TMenu(){
   memset(this,0,sizeof(TMenu));
};
TMenu::TMenu(TStream*st){
  memset(this,0,sizeof(TMenu));
  if (!st->status){
  TTplReader t(st);
  tpl=&t;
	int code;
	while (ReadKeyWord(code)){
	 switch (code){
	  case K_MENU:if(ReadMenu())break;goto m1;
	 }
	};
m1:
 }
};
BOOL TMenu::ReadKeyWord(int & code){
return tpl->ReadKeyWord(code,MenuCodes,sizeof(MenuCodes)/4);
};
BOOL TMenu::ReadMenu(){
if (tpl->ReadChar('{')){
  menu=CreateMenu();
  do{
	 int _code;
	  if(ReadKeyWord(_code)){
	   switch(_code){
	   case K_POPUP:if (!ReadPopup(menu))return FALSE;break;
	   case K_MENUITEM:if (!ReadItem(menu))return FALSE;break;
       default:return FALSE;
       }
      }   else break;
  }while (TRUE);
  if(tpl->ReadChar('}'))return TRUE;
 }
 return FALSE;
};
BOOL TMenu::ReadItem(HMENU m){
 int _code;
  if(ReadKeyWord(_code)){
   if(_code==K_SEPARATOR)return AppendMenu(m,MF_SEPARATOR,0,"");
   return FALSE;
  }
 char _name[128];
 int code=0;
 UINT flags=MF_STRING|MF_ENABLED;
 if(tpl->ReadStr(_name,sizeof(_name))){
  while  (tpl->ReadChar(',')){
   if(!tpl->ReadInt(code)){
     if(ReadKeyWord(_code)){
      switch(_code){
       case K_DISABLED:flags|=MF_DISABLED;
                       flags&=~MF_ENABLED;
                       break;
       case K_GRAYED:flags|=MF_GRAYED;break;
       case K_CHECKED:flags|=MF_CHECKED	;break;
       default:return FALSE;
     }}
   }
  }
 }
 return AppendMenu(m,flags,code,_name);
};
BOOL TMenu::ReadPopup(HMENU m){
 char _name[128];
 int _code=0;
 UINT flags=MF_POPUP	;
 if(tpl->ReadStr(_name,sizeof(_name))){
  while  (tpl->ReadChar(',')){
     if(ReadKeyWord(_code))
     switch(_code){
      case K_GRAYED:  flags|=MF_GRAYED;break;
      case K_DISABLED:flags|=MF_DISABLED;
                      flags&=~MF_ENABLED;
                      break;
      case K_CHECKED:flags|=MF_CHECKED	;break;
      default:return FALSE;
     }
  }
  if(tpl->ReadChar('{')){
   HMENU hp=CreatePopupMenu();
   AppendMenu(m,flags,(UINT)hp,_name);

  do{
    if(tpl->ReadChar('}'))return TRUE;
	 int _code;
	  if(ReadKeyWord(_code)){
	   switch(_code){
	    case K_POPUP:if (!ReadPopup(hp))return FALSE;break;
	    case K_MENUITEM:if (!ReadItem(hp))return FALSE;break;
       default:return FALSE;
       }
      }   else break;
  }while (TRUE);
  }
 }
 return FALSE;
}
TMenu::~TMenu(){
  Remove();
  if(menu)FullDeleteMenu(menu);
};
BOOL TMenu::Assign(TWindow*_window){
  if(menu && (!window)){
   window=_window;
   BOOL rez=FALSE;
   _menu=(HMENU)window->WndProc(WM_SETMENU,(WPARAM)menu,(LPARAM)&rez);
   return rez;
  }
};
BOOL TMenu::Remove(){
    if(window && menu)
     {
       BOOL rez=FALSE;
       window->WndProc(WM_SETMENU,(WPARAM)_menu,(LPARAM)&rez);
       window=NULL;
       return rez;
     }
    return FALSE;
};
BOOL TMenu::WndProc(UINT msg,WPARAM,LPARAM){
return FALSE;
};
/*
 TMenuWindow::~TMenuWindow(){

 };
 TMenuWindow::TMenuWindow(LPCREATESTRUCT l,HWND h):TWindow(l,h){

 };
 LRESULT TMenuWindow::WndProc(UINT message, WPARAM wParam,LPARAM lParam){
   return TWindow::WndProc(message,wParam,lParam);
 };
*/
