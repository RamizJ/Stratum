SB0  \     	 WinButton   HSpace шьџ юъэр   HANDLE  
 WindowName$ фхёъ№шяђю№ у№рєшїхёъюую я№юёђ№рэёђтр Main Window STRING    HObject     HANDLE   _HObject     HANDLE    Text ђхёђ ъэюяъш Button STRING    _oldtext     STRING   pressed) ёюёђюџэшх ъэюяъш (0 - юђцрђр, 1 - эрцрђр)   FLOAT   Enable( ђшя ъэюяъш (0 - юђъыўїхэр, 1 - тъыўїхэр) 1 FLOAT    oe     FLOAT   rez     FLOAT   msg     FLOAT   wNotifyCode     FLOAT   ret     FLOAT   OrgX ъюю№фшэрђћ шчюс№рцхэшџ    FLOAT    OrgY ъюю№фшэрђћ шчюс№рцхэшџ    FLOAT        џџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџ ъ       о   2D^   С                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\       џџџџ  § 
 
  ќ 
 
 џџ     BUTTON     r@      g@      V@      7@ Button Button  T    d X               End Of File <!>
 тHANDLE HSpace
STRING WindowName
HANDLE HObject,_HObject
STRING Text
STRING local _oldtext
FLOAT pressed,Enable
FLOAT local oe,rez,msg,wNotifyCode,ret
FLOAT OrgX,OrgY

if (HObject)
 if(_oldtext!=text)
  rez:= SetControlText2d(HSpace,HObject,text)
  _oldtext:=text
 endif
 if(enable!=oe)
  rez:= EnableControl2d(HSpace,HObject,enable)
  oe:=enable
 endif

if(msg==WM_SPACEDONE)
 HObject:=#0
 HSpace:=#0
 msg:=0
 exit()
endif

 if (msg == WM_CONTROLNOTIFY)

  if (wNotifyCode==0)
   pressed:=1
  endif
   msg:=0
  endif

 else
 if (WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(WindowName); endif
 if (~HSpace)

  if (GetWindowProp(GetWindowName(~HSpace),"CLASSNAME") != GetClassName(".."))
   HObject:=CreateControlObject2d(~HSpace,"BUTTON",text,1409351680,OrgX,OrgY,88,24,10)
  else
   HObject := GetObject2dByName(~HSpace,~_HObject,"BUTTON")
   ret := DelGroupItem2d(~HSpace,GetObjectParent2d(~HSpace,~HObject),~HObject)
   rez := SetControlText2d(~HSpace,~HObject,text)
  endif

  if (~HObject)
   rez:=EnableControl2d(~HSpace,~HObject,enable)
   registerobject(~HSpace,~HObject,"",WM_CONTROLNOTIFY,0)
   registerobject(~HSpace,~HObject,"",WM_SPACEDONE,256|16)
  endif
 endif
endif  Ђ         o l x  x   5       x   	 x  {      7 5 ,         	      
       @6 5 H                          
    
        @6 5 j            6 5 c        №?            
 3 x  z             R + 5  x  Ь       o    Ы z  CLASSNAME ђz  ..   5 О    z  BUTTON  x       @ еA           V@       8@       $@  3 р      z  BUTTON  р           F  q       x   	   o         	      z           @               z          @       p@       0@j     в­Ћ  :   B   .  
 