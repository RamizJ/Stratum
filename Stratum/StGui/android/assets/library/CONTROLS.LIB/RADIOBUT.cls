SB0        RadioButton   HSpace шьџ юъэр   HANDLE  
 WindowName$ фхёъ№шяђю№ у№рєшїхёъюую я№юёђ№рэёђтр Main Window STRING    HObject     HANDLE   _HObject     HANDLE   Text ђхёђ ъэюяъш   STRING    _oldtext     STRING   State     FLOAT   Enable( ђшя ъэюяъш (0 - юђъыўїхэр, 1 - тъыўїхэр) 1 FLOAT    oe     FLOAT   rez     FLOAT   msg     FLOAT   wNotifyCode     FLOAT   oc     FLOAT   OrgX ъюю№фшэрђћ шчюс№рцхэшџ    FLOAT    OrgY ъюю№фшэрђћ шчюс№рцхэшџ    FLOAT    checked     FLOAT       џџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџџ я       у   2D^   Ц                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\       џџџџ  § 
 
  ќ 
 
      BUTTON      d@      \@      Y@      <@ Button RadioButton	 T    d d               End Of File <!>
 HANDLE HSpace
STRING WindowName
HANDLE local HObject,_HObject
STRING Text
STRING local _oldtext
FLOAT State,Enable
FLOAT local oe,rez,msg,wNotifyCode,oc//,checked
FLOAT OrgX,OrgY

 State:=IsDlgbuttonChecked2d(HSpace,HObject)
 oc:=State

if (HObject)
 if(_oldtext!=text)
  rez:= SetControlText2d(HSpace,HObject,text)
  _oldtext:=text
 endif
 if(enable!=oe)
  rez:= EnableControl2d(HSpace,HObject,enable)
  oe:=enable
 endif
 if(oc!=State)
  rez:=checkdlgbutton2d(hSpace,HObject,State)
  oc:=State
 endif

 if(msg==WM_SPACEDONE)
  HObject:=#0
  HSpace:=#0
  msg:=0
  exit()
 endif

 if (msg == WM_CONTROLNOTIFY)
  if (wNotifyCode==0)
   checked:=IsDlgbuttonChecked2d(HSpace,HObject)
   oc:=checked
  endif
   msg:=0
  endif

 else
 if (WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(WindowName); endif
 if (~HSpace)

  if (GetWindowProp(GetWindowName(~HSpace),"CLASSNAME") != GetClassName(".."))
   HObject:=CreateControlObject2d(~HSpace,"BUTTON",text, 1409351689,OrgX,OrgY,88,24,10)
  else
   HObject := GetObject2dByName(~HSpace,~_HObject,"BUTTON")
   rez := DelGroupItem2d(~HSpace,GetObjectParent2d(~HSpace,~HObject),~HObject)
  endif

  if (~HObject)
   rez:=EnableControl2d(~HSpace,~HObject,enable)
   registerobject(~HSpace,~HObject,"",WM_CONTROLNOTIFY,0)
   registerobject(~HSpace,~HObject,"",WM_SPACEDONE,256)
  endif
 endif
endif     ,               o  x  x   5 #      x   	 x  {      7 5 7         	         7 5 K         	      
       @6 5 g                          
    
        @6 5             6 5                       
 3 +x  z             R + 5 Ѓ x  Ь       o +   Ы z  CLASSNAME ђz  ..   5 с    z  BUTTON  x     @@ еA           V@       8@       $@  3 њ      z  BUTTON  р           F  q 	   o +        	      z           @               z          @       p@    Яe%  "   *     
 