SB0  Q      CheckBox   HSpace шьџ юъэр   HANDLE  
 WindowName$ фхёъ№шяђю№ у№рєшїхёъюую я№юёђ№рэёђтр Main Window STRING    HObject     HANDLE   _HObject     HANDLE   Text ђхёђ ъэюяъш CheckBox STRING    _oldtext     STRING   State ђхъѓљхх ёюёђюџэшх ъэюяъш   FLOAT   Enable( ђшя ъэюяъш (0 - юђъыўїхэр, 1 - тъыўїхэр) 1 FLOAT    oe     FLOAT   rez     FLOAT   msg     FLOAT   wNotifyCode     FLOAT   oc     FLOAT   OrgX ъюю№фшэрђћ шчюс№рцхэшџ    FLOAT    OrgY ъюю№фшэрђћ шчюс№рцхэшџ    FLOAT        џџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџ ь       р   2D^   У                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\       џџџџ  § 
 
  ќ 
 
 џџ    BUTTON      f@      Z@      Y@      <@ Button CheckBox T    d d               End Of File <!>
 ЎHANDLE HSpace
STRING WindowName
HANDLE local HObject, _HObject
STRING Text
STRING local _oldtext
FLOAT State,Enable
FLOAT local oe,rez,msg,wNotifyCode,oc
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
 if(oc!=~State)
  rez:=checkdlgbutton2d(hSpace,HObject,~State)
  oc:=~State
 endif

 if(msg==WM_SPACEDONE)
  HObject:=#0
  HSpace:=#0
  msg:=0
  exit()
 endif

 if (msg == WM_CONTROLNOTIFY)
  if (wNotifyCode==0)
   State:=IsDlgbuttonChecked2d(hSpace,HObject)
   oc:=~State
  endif
   msg:=0
  endif

 else
 if (WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(WindowName); endif
 if (~HSpace)

  if (GetWindowProp(GetWindowName(~HSpace),"CLASSNAME") != GetClassName(".."))
   HObject:=CreateControlObject2d(~HSpace,"BUTTON",text, 1342242819,OrgX,OrgY,88,24,10)
   rez:=checkdlgbutton2d(hSpace,HObject,~State)
   oc:=~State
  else
   HObject := GetObject2dByName(~HSpace,~_HObject,"BUTTON")
   rez:=SetControlText2d(HSpace,~HObject,text)
   rez := DelGroupItem2d(~HSpace,GetObjectParent2d(~HSpace,~HObject),~HObject)
  endif

  if (~HObject)
   rez:=EnableControl2d(~HSpace,~HObject,enable)
   registerobject(~HSpace,~HObject,"",WM_CONTROLNOTIFY,256)
   registerobject(~HSpace,~HObject,"",WM_SPACEDONE,256)
  endif
 endif
endif     7    o  x  x   5       x   	 x  {      7 5 ,         	         7 5 @         	      
       @6 5 \                          
    
        @6 5             6 5 {                      
 3 6x  z             R + 5  x  Ь       o 6   Ы z  CLASSNAME ђz  ..   5 у    z  BUTTON  x     Р @ дA           V@       8@       $@          	     3      z  BUTTON  р        x   	         F  q 	   o 6        	      z           @       p@      z          @       p@    lТ  '   /     
 