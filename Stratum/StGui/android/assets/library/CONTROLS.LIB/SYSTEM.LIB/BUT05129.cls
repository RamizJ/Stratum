SB0  �      but02   HSpace     HANDLE    _HObject     HANDLE    HEdit     HANDLE    HObject     HANDLE�   xPos     FLOAT   yPos     FLOAT   OrgX     FLOAT�   OrgY     FLOAT�   msg     FLOAT�   sizex     FLOAT�   sizey     FLOAT�   rez     FLOAT�   captured     FLOAT�   Step     FLOAT    _disable     FLOAT�   pressed     FLOAT�   Value     FLOAT�   ret     FLOAT�       ����������������������������������������������������������������������������������������������������������������                wwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpw pwwwwpwwwpw  w www wwwwpw pwww wwwwpw wpwpwwwwpw pwwwwpwwwwpw  wwwwpwwwwpw p wpwwpwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwp                         2D^   0                        Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
   � 
 
 ��   �        8@      8@     @Z@      9@         8@      8@      `@      8@      `@      H@      8@      H@      8@      8@             @@      8@      V@      3@                    � 
 
 ��e           � 
 
 ��f   ���        � 
 
 ��i   ��      �   �"Arial   ,�8w�8 w  '�л79 � 
 
 ��j    ������ ����� 
 
 ��k   �          �           End Of File <!>
 �HANDLE HSpace,_HObject,HEdit
HANDLE HObject
FLOAT  local xPos,yPos,OrgX,OrgY,msg
FLOAT  local sizex,sizey,rez,captured
FLOAT Step,_disable

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

 if ((msg==WM_MOUSEMOVE) && (captured))
//  if (Type); exit(); endif
  pressed:= ((xPos>=OrgX && xPos<=OrgX+sizex)&&(yPos>=OrgY && yPos<=OrgY+sizey))

 endif

 if ((msg==WM_LBUTTONUP) && (captured))
 if(pressed)
   Value := ~Value - Step
   SetVar("..","_Value",~Value)
   ret := SetControlText2d(HSpace,HEdit,String(~Value))
  endif
  ReleaseCapture()
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
 if ((_HObject != #0) && (~HSpace != #0))
  HObject := GetObject2dByName(~HSpace,~_HObject,"b_down")
  if (~HObject == #0);exit();endif
  rez:=DelGroupItem2d(~HSpace,GetObjectParent2d(~HSpace,~HObject),~HObject)
  OrgX:=GetObjectOrg2dX(~HSpace,~HObject)
  OrgY:=GetObjectOrg2dY(~HSpace,~HObject)
  sizey:=getobjectHeight2d(~HSpace,~HObject)
  sizex:=getobjectWidth2d(~HSpace,~HObject)
  registerobject(~HSpace,~HObject,"",WM_ALLMOUSEMESSAGE,1 |16| 256)
  registerobject(~HSpace,#0,"",WM_SPACEDONE,256)
  _disable:=1
 endif
endif
  ��������� �����.
 @�   e    5 �         �@6 5 "                                        �@6 5 E        �?         �?     z             �          �@6   + 5 q     9      	  ; +     9      
  ; + +           �@6   + 5 �   5 �        z  ..  z  _Value    �       � �  �                           7 5 �  
                       	  
 e                      R 5 d       S         S + 5 d     z  b_down  �          R 5           F  q       D       E       I 
      H 	      z           �@       �?       0@j        p@j �         z          �@       p@�        �?     o�  �   �   �  
 �   