SB0  �      but01   HSpace     HANDLE    _HObject     HANDLE    HEdit     HANDLE    HObject     HANDLE�   xPos     FLOAT   yPos     FLOAT   OrgX     FLOAT�   OrgY     FLOAT�   msg     FLOAT�   sizex     FLOAT�   sizey     FLOAT�   _disable     FLOAT�   rez     FLOAT�   captured     FLOAT�   Step     FLOAT    pressed     FLOAT�   Type     FLOAT    Value     FLOAT�   ret     FLOAT�       ����������������������������������������������������������������������������������������������������������������                wwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpw pwwwwwpw  w wwwp pwwpw pwwww pwwpw wpw  wwwpw pwwwwp pwwpw  wwwwp pwwpw p wpwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwp                         2D^   0                        Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
   � 
 
 ��            P@      H@     @Z@      9@         P@      H@      e@      H@      e@      R@      P@      R@      P@      H@             R@      H@      V@      3@                    � 
 
  e           � 
 
  f   ���        � 
 
 ��i   ��      �   �"Arial   ,�8wW8 w  '�л79 � 
 
 ��j    ������ ������ 
 
 ��k   �          �           End Of File <!>
 �HANDLE HSpace,_HObject,HEdit
HANDLE HObject
FLOAT local xPos,yPos,OrgX,OrgY,msg
FLOAT local sizex,sizey,_disable,rez,captured
FLOAT Step

if (msg)

if (msg==WM_SPACEDONE)
//  _HObject:=GetHObject()
//  HSpace:=#0
  HObject:=#0
  msg:=0
  _disable := 0
  exit()
endif

 if (msg==WM_LBUTTONDOWN)
  pressed:=1
  captured:=1
  SetCapture(~HSpace,"",0)
 endif

 if ((msg==WM_MOUSEMOVE) && (captured==1))
  if (Type); exit(); endif
  pressed:= ((xPos>=OrgX && xPos<=OrgX+sizex)&&(yPos>=OrgY && yPos<=OrgY+sizey))
 endif

 if ((msg==WM_LBUTTONUP) && (captured==1))
  ReleaseCapture()
  if(pressed)
   Value := ~Value + Step
   ret := SetControlText2d(HSpace,HEdit,String(~Value))
   SetVar("..","_Value",~Value)
  endif
  pressed:=0
  captured:=0

 endif

 if (pressed != ~pressed)
  rez:=sizey*~pressed
  rez:=SetBitmapSrcRect2d(~HSpace,HObject,0,~rez,sizex,sizey)
 endif
 msg:=0
 exit()
endif

if (HObject == #0)
 if ((~_HObject != #0) && (~HSpace != #0))
  HObject := GetObject2dByName(~HSpace,~_HObject,"b_up")
  if (~HObject == #0);exit();endif
  rez:=DelGroupItem2d(~HSpace,GetObjectParent2d(~HSpace,~HObject),~HObject)
  OrgX:=GetObjectOrg2dX(~HSpace,~HObject)
  OrgY:=GetObjectOrg2dY(~HSpace,~HObject)
  sizey:=getobjectHeight2d(~HSpace,~HObject)
  sizex:=getobjectWidth2d(~HSpace,~HObject)
  registerobject(~HSpace,~HObject,"",WM_ALLMOUSEMESSAGE,1|256|16)
  registerobject(~HSpace,#0,"",WM_SPACEDONE,256)
  _disable:=1
 endif
endif
  ��������� �����. @�   u    5 �         �@6 5 "                                        �@6 5 E        �?         �?     z             �          �@6          �?6 + 5 |   5 ]       9      	  ; +     9      
  ; + +           �@6          �?6 + 5 � �   5 �               � �  z  ..  z  _Value    �                          7 5 �  
                       	  
 e                      R 5 t       S         S + 5 t     z  b_up  �          R 5           F  q       D       E       I 
      H 	      z           �@       �?       p@j        0@j �         z          �@       p@�        �?     o�  �   �   �  
 �   