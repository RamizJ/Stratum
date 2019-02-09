#include <windows.h>
#include "_g_def.h"
#include "_object.h"
#include "collect.h"

class TLIB2D:public _OBJECT{
 public:
  HINSTANCE lib;
  char*name;
  TLIB2D(HINSTANCE h,char*name);
  ~TLIB2D();
};
 TLIB2D::TLIB2D(HINSTANCE h,char*_name){
  lib=h;
  name=NewStr(_name);
 };
 TLIB2D::~TLIB2D(){
  delete name;
  FreeLibrary(lib);
 };

TCollection * libs=NULL;
_EXTERNC HINSTANCE _EXPORT LoadLibrary2d(char*name){
if(libs){
for(C_TYPE i=0;i<libs->count;i++){
  TLIB2D*l=(TLIB2D*)libs->At(i);
   if(!lstrcmpi(l->name,name))return l->lib;
 }
}else{
  libs=new TCollection(10,2);
}

HINSTANCE h = LoadLibrary(name);
#ifdef WIN32
 if (h)
#else
 if (h > HINSTANCE_ERROR)
#endif
{
 libs->Insert(new TLIB2D(h,name));
 return h;
}
return 0;
};
_EXTERNC BOOL _EXPORT FreeLibrary2d(HINSTANCE h){
 if(libs){
 for(C_TYPE i=0;i<libs->count;i++){
  TLIB2D*l=(TLIB2D*)libs->At(i);
   if(l->lib==h){
      libs->AtFree(i);
      libs->AtDelete(i);
      return TRUE;
   }}}
 return FALSE;
};
void DoneLibs2d(){
 if(libs){
  DeleteCollection(libs);
  libs=0;
 }
}