SB0  �      HSlider  
 WindowName ��� ���� Main Window STRING    HSpace ���������� ������������   HANDLE�   Value ������������ ��������   FLOAT�   min ����������� ��������   FLOAT    max ������������ �������� 1 FLOAT    Width ������ �������� 100 FLOAT    ChangedO Value ���� ��������, ���� =2 �� ����� ���������� ����� (�������� ��� ���� ����)   FLOAT�   OrgX ���������� �����������    FLOAT�   OrgY ���������� �����������    FLOAT�   ChangeBegin     FLOAT�   xPos     FLOAT   yPos     FLOAT   _Value     FLOAT�   rez     FLOAT�   msg     FLOAT�   fwKeys     FLOAT   v     FLOAT�   dx     FLOAT�   _disable     FLOAT�   _HObject     HANDLE�   obj     HANDLE�       ��������������������������������������������������������������������������������wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwpwwpwwwwwwwwwwwwwxwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwwww�wwwwwwwwwwwwwww�wwwwwwwwwwwwwww�wwwwwwwwwwww��w�����������wxwww�wwwwwwwwwwwx  w�         wx��w�����������wwwww�wwwwwwwwwwwwwww�wwwwwwwwwwwwwww�wwwwwwwwwwwwwww�wwwwwwwwwwwwwww�wwwwwwwwwwwwwww�wwwwwwwwwwwwwww�wwwwwwwwwwwwwww�wwwwwwwwwwwwww��wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww �      w  2D^   �       �T@              Y@      Y@      i@      i@ C:\SC3\data\icons\        ����  �  
     	        �  
 ��     line�
                �        h@������`@         >����@          h@������`@      h@ܹ����`@    �        h@     �^@      Y@�������?          h@������^@      h@     �^@     @r@     �^@    �        g@     �[@     �\@      ;@          g@     �[@     �r@     �[@     �r@     @a@      g@     @a@      g@     �[@    �        h@������^@      Y@�������?         @r@������^@�����?r@�����?_@      h@a����?_@    �        h@������^@     �X@                  h@������^@     0r@������^@  	  �
 gsl_button      j@     @\@      &@      5@                      &@      5@      �  �	ףp5q@X   �`@     `�=�������?    j
ףp5q@X   �`@�	ףp5q@S�����`@    �  �����?r@   �`@         >����@    �����?r@   �`@�����?r@      a@    �  Ą�Q�*p@������`@     ��= ������?    x��Q�*p@������`@Ą�Q�*p@������`@    �  �����?n@������`@     p�=�������?       @n@������`@�����?n@������`@    �  {�G�*l@F�����`@     p�=�������?    ��G�*l@F�����`@{�G�*l@A�����`@    �  �
ףpj@������`@     p�=�������?    �ףpj@������`@�
ףpj@������`@  � 
 
 ��e         e         e         � 
 
 ��f            � 
 
 ��h   BMr      v   (               �   ,  ,              �  �   �� �   � � ��  ��� ���   �  �   �� �   � � ��  ���            �  �    �  � w�   �x  � �wx  w�� �www� �x ��wwwxw������wwwx���wwwxw������wwwx���wwwxw������wwwx���wwwxw������wwwx���wwwxw������wwwx���wwwxw������wwwx���wwwxw������wwwx���wwwxwwww�������������BM�       >   (               T   ,  ,        ���      ������?����������������������������������������������������������������������    Q@ �    "                       �?      �?    End Of File <!>
 �
STRING WindowName
HANDLE HSpace
FLOAT Value,min,max,Width,Changed,OrgX,OrgY,ChangeBegin
FLOAT local xPos,yPos,_Value
FLOAT local rez,msg,fwKeys,v,dx,_disable
HANDLE local _HObject,obj

if (~max==~min)
  _disable := 1
  exit()
endif

if (obj)
if(msg)
if (msg==WM_SPACEDONE)
  _HObject:=GetHObject()
  HSpace:=#0
  obj:=#0
  msg:=0
  exit()
endif

 if (msg==WM_LBUTTONDOWN)
  ChangeBegin:=1
  rez:=SetBitmapSrcRect2d(HSpace,~obj,11,0,11,21)
  dx:=~xPos-GetObjectOrg2dX(HSpace,~obj)
  SetCapture(HSpace,"",0)
 endif

 if (and(msg==WM_MOUSEMOVE,ChangeBegin==1))
   Value := ((xPos-dx) - (OrgX)-3) / Width
   Value := ~Value*(max-min)+min
   if (~min<~max)
     Value:=max(min,~Value)
     Value:=min(max,~Value)
   else
     Value:=max(max,~Value)
     Value:=min(min,~Value)
   endif
 endif

 if (and(msg==WM_LBUTTONUP,ChangeBegin==1))
  ChangeBegin:=0
  rez:=SetBitmapSrcRect2d(HSpace,~obj,0,0,11,21)
  if(changed)
   Changed:=2
  endif
  ReleaseCapture()
 endif
 msg:=0
endif

if (_Value != ~Value)
  v := ~Value
  if (~max > ~min)
   if (~Value < ~min); v := ~min; endif
   if(~Value > ~max); v := ~max; endif
   v := ~OrgX + (~v - ~min)/(~max - ~min)*~Width + 3
  else
   if (~Value > ~min); v := ~min; endif
   if(~Value < ~max); v := ~max; endif
   v := ~OrgX - (~v - ~min)/(~min - ~max)*~Width + 3
  endif
  rez:=SetObjectOrg2d(HSpace,obj,~v,OrgY+2)
  _Value := ~Value
  Changed:=1
endif

else
 if (WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(WindowName); endif
 if (~HSpace); else exit(); endif
 if (GetWindowProp(GetWindowName(~HSpace),"CLASSNAME") != GetClassName(".."))
   _HObject := CreateObjectFromFile2D(~HSpace,AddSlash(GetClassDirectory(GetClassName("")))+GetClassName("")+".vdr", OrgX,OrgY,PFC_MOVEOBJECT)
   rez := ObjectToTop2d(~HSpace,~_HObject)
 endif

 OrgX := GetObjectOrg2dx(~HSpace,~_HObject)
 OrgY := GetObjectOrg2dy(~HSpace,~_HObject)

 obj := GetObject2dByName(~HSpace,~_HObject,"line")
 rez:=DelGroupItem2d(~HSpace,GetObjectParent2d(~HSpace,~obj),~obj)
 rez:=SetObjectSize2d(~HSpace,~obj,~Width,14)

 obj := GetObject2dByName(~HSpace,~_HObject,"gsl_button")
 rez:=DelGroupItem2d(~HSpace,GetObjectParent2d(~HSpace,~obj),~obj)
 rez:=SetObjectSize2d(~HSpace,~_HObject,~Width+16,27)
 registerobject(~HSpace,~obj,"",WM_ALLMOUSEMESSAGE,257)
 registerobject(~HSpace,#0,"",WM_SPACEDONE,256)
 v := ~Value
 if (~max > ~min)
   if (~Value < ~min); v := ~min; endif
   if(~Value > ~max); v := ~max; endif
   v := ~OrgX + (~v - ~min)/(~max - ~min)*~Width + 3
 else
   if (~Value > ~min); v := ~min; endif
   if(~Value < ~max); v := ~max; endif
   v := ~OrgX - (~v - ~min)/(~min - ~max)*~Width + 3
 endif
 rez:=SetObjectOrg2d(~HSpace,~obj,~v,~OrgY+2)
 _Value := ~Value
endif
 q ������� ���������� - ��������.
�������� ���������� � ��������� �� min �� ���,
��� ��������������� ������������� @�         6 5         �?      o �  5         �@6 5 6 �                                      �@6 5 w        �? 	            &@                &@       5@e   
     D     z             �          �@6  	        �?6 + 5 �  
              @                       : 5 �     &       '   3 �     &       '           �@6  	        �?6 + 5           	                              &@       5@e    5         @  �                7 5 �        8 5 Z    : 5 4        8 5 ?                            @   3 �    8 5 e        : 5 p                            @                   @ K             �?  3 x   z    �        R + 5 �x   �     o �3 �    � z  CLASSNAME �z  ..  �� 5 �  z    �� � z    �| z  .vdr  |            �@h       b      D      E      z  line  �         F  q               ,@L      z  gsl_button  �         F  q               0@        ;@L      z           �@      p@�        z          �@       p@�         8 5 �    : 5 �        8 5 �                            @   3 �    8 5 �        : 5 �                            @                   @ K         x�  �   �   m  
 h   