SB0  б     
 StringEdit  
 WindowName шьџ юъэр Main Window STRING    HSpace$ фхёъ№шяђю№ у№рєшїхёъюую я№юёђ№рэёђтр   HANDLE   text ёђ№юъютрџ ях№хьхээрџ   STRING   OrgX ъюю№фшэрђћ шчюс№рцхэшџ    FLOAT    OrgY ъюю№фшэрђћ шчюс№рцхэшџ    FLOAT    HObject     HANDLE   _HObject     HANDLE   ret     FLOAT   wNotifyCode     FLOAT   msg     FLOAT   _Height     FLOAT   _Width     FLOAT   _text     STRING   Width   100 FLOAT    Height   20 FLOAT        џџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџ         2D^   З                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        џџџџ  § 
 
  ќ 
 
 џџ     edit     р`@     РP@      Y@      4@ Edit   T   d d          :Р ў    "                        @       @     End Of File <!>
 STRING WindowName
HANDLE HSpace
STRING text
FLOAT OrgX,OrgY

HANDLE local HObject,_HObject
FLOAT local ret,wNotifyCode,msg,_Height,_Width
STRING  local _text

if (HObject)
  
  if (_text != ~text)
    ret := SetControlText2d(HSpace,HObject,~text)
    _text := ~text
    exit()
  endif
  
  if (_Width != ~Width || _Height != ~Height)
    ret:=SetObjectSize2d(~HSpace,~HObject, ~Width, ~Height)
    _Height := ~Height
    _Width := ~Width
    exit()
  endif


  if (msg==WM_SPACEDONE)
    _HObject:=GetHObject()
    HSpace:=#0
    HObject:=#0
    msg:=0
    exit()
  endif

  if (msg==WM_CONTROLNOTIFY)
    if (wNotifyCode==768)
      text:=GetControlText2d(HSpace,HObject)
      _text := ~text
    endif
    msg:=0
  endif
 
else

  if (WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(WindowName); endif
  if (~HSpace==#0); exit(); endif
  if (GetWindowProp(GetWindowName(~HSpace),"CLASSNAME") != GetClassName(".."))
    HObject := CreateObjectFromFile2D(~HSpace,AddSlash(GetClassDirectory(GetClassName("")))+GetClassName("")+".vdr", OrgX,OrgY,PFC_MOVEOBJECT)
    ret := ObjectToTop2d(~HSpace,~HObject)
  else
    HObject := GetObject2dByName(~HSpace,~_HObject,"edit")
    ret:=DelGroupItem2d(~HSpace,GetObjectParent2d(~HSpace,~HObject),~HObject)
    ret:=SetObjectSize2d(~HSpace,~HObject, ~Width, ~Height)
  endif
  RegisterObject(~HSpace,~HObject,"",WM_CONTROLNOTIFY,0)
  RegisterObject(~HSpace,#0,"",WM_SPACEDONE,256)
  ret := SetControlText2d(~HSpace,~HObject,~text)
endif
 D Шьшфц яючтюыџхђ я№юёьрђ№штрђќ ш №хфръђш№ютрђќ
ёђ№юъютѓў ях№хьхээѓў.  Ђ   +    o  x  y   5      y    y  {        7  
   7 , 5 :         L     
        	       @6 5 Y п                           	    	        @6 5           @6 5 x     {  y  {            	 3 *x   z            R + 5  x   Ь          R 5      Ы z  CLASSNAME ђz  ..   5 й   z    щ ю z    | z  .vdr  |            р@h       b  3 ќ     z  edit  р         F  q          L      z           @                 z          @       p@     y       Vx  є   ќ     
    