SB0  C!      VSlider  
 WindowName ��� ���� Main Window STRING    HSpace ���������� ������������   HANDLE�   _HObject     HANDLE�   obj     HANDLE�   Value ������������ ��������   FLOAT�   min ����������� ��������   FLOAT    max ������������ ��������  1 FLOAT    Height ������ �������� 100 FLOAT    ChangedO Value ���� ��������, ���� =2 �� ����� ���������� ����� (�������� ��� ���� ����)   FLOAT�   OrgX ���������� �����������    FLOAT�   OrgY ���������� �����������    FLOAT�   ChangeBegin     FLOAT�   xPos     FLOAT   yPos     FLOAT   _Value     FLOAT�   _disable     FLOAT�   rez     FLOAT�   msg     FLOAT�   fwKeys     FLOAT   v     FLOAT�   dy     FLOAT�       wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwpwwww�wwww wwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwpwwww�wwww wwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwwwwwwxwwwwwwwwwwwwwxwwwwwwwwwwwwwxwww �	      v	  2D^          �`@      y@      Y@      Y@      y@      y@ C:\SC3\data\icons\        ����  �  
       
   	          �  
 ��   �       �^@�����l@      @     ذ=         �^@     �l@     �_@�����l@    �       �^@+�Q�b@       @                 �^@+�Q�b@     @_@+�Q�b@     
 vsl_button     @Y@     �^@      5@      &@                      5@      &@              �[@      `@      @������X@         �[@�����_l@�	   �[@9    `@     @\@      `@            �W@     @^@      A@      ]@    &�   �W@     �m@     �W@4&   @^@m����`@     @^@      `@������m@&�   �W@     �m@            �[@�    `@@:   @������X@    �	   @\@�    `@     @\@�����l@     �[@�����l@            �[@�
    `@     r�=�����?X@         �[@�����?l@9   �[@�
    `@    �       �^@      `@      @     ذ=         �^@    `@     �_@      `@  	  �       �^@�(\�*d@       @                 �^@�(\�*d@     @_@�(\�*d@  
  �       �^@�����?f@       @                 �^@�����?f@     @_@�����?f@      line�         	 
             �       �^@�Q�Uh@       @                 �^@�Q�Uh@     @_@�Q�Uh@    �       @X@      `@      @                  Y@      `@     @X@      `@    �       �^@�(\�jj@       @                 �^@�(\�jj@     @_@�(\�jj@    �  �.   �X@`�G�jj@       @     p�=    �.    Y@.�G�jj@�.   �X@`�G�jj@    �       @X@�����l@      @     ذ=          Y@�����l@     @X@�����l@    �  �]   �X@�ףpUh@       @     p�=    �]    Y@�ףpUh@�]   �X@�ףpUh@    �  H�   �X@L����?f@       @     p�=    H�    Y@   @f@H�   �X@L����?f@    �  �   �X@z�G�*d@       @     p�=    �    Y@H�G�*d@�   �X@z�G�*d@    �  ��   �X@�
ףpb@       @     p�=    ��    Y@�ףpb@��   �X@�
ףpb@  � 
 
  e         e         e         e         e         e         e         � 
 
 ��f            � 
 
 ��g   BM~      v   (                 ,  ,              �  �   �� �   � � ��  ��� ���   �  �   �� �   � � ��  ���           �������������x�������������x�������������x�������������x��wwwwwwwwx������������          �������������wwwwwwwwx��wwwwwwwwx��wwwwwwwwx��wwwwwwwwx��wwwwwwwwx��wwwwwwwwx��wwwwwwwwx��wwwwwwwwx������������    Q@ �    "                       �?      �?    End Of File <!>
 8STRING WindowName
HANDLE HSpace
HANDLE local _HObject,obj
FLOAT Value,min,max,Height,Changed,OrgX,OrgY,ChangeBegin
FLOAT local  xPos,yPos,_Value, _disable
FLOAT local rez,msg,fwKeys,v,dy

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
  rez:=SetBitmapSrcRect2d(HSpace,~obj,0,11,21,11)
  dy:=GetObjectOrg2dY(HSpace,~obj)-~yPos
  SetCapture(HSpace,"",0)
 endif

 if (and(msg==WM_MOUSEMOVE,ChangeBegin==1))
   Value := 1 - ((yPos+dy) - OrgY-2) / Height
   Value := ~Value*(max-min)+min
   if (~max>~min)
     Value:=max(min,~Value)
     Value:=min(max,~Value)
   else
     Value:=max(max,~Value)
     Value:=min(min,~Value)
   endif
   _Value := ~Value
 endif

 if (and(msg==WM_LBUTTONUP,ChangeBegin==1))
  ChangeBegin:=0
  rez:=SetBitmapSrcRect2d(HSpace,~obj,0,0,21,11)
  ReleaseCapture()
  if(Changed)
   changed:=2
  endif
 endif
 msg:=0
endif

if (_Value != ~Value)
  v := ~Value
  if (~max>=~min)
   if (~Value < ~min); v := ~min; endif
   if(~Value > ~max); v := ~max; endif
   v := ~OrgY - (~v - ~min)/(~max - ~min)*~Height + ~Height
  else
   if (~Value > ~min); v := ~min; endif
   if(~Value < ~max); v := ~max; endif
   v := ~OrgY + (~v - ~max)/(~min - ~max)*~Height
  endif

//  if (~Value < min); v:= min; endif
//  if(~Value > max); v := max; endif
//  v := OrgY - (~v-min)/(max-min) * Height + Height
  rez:=SetObjectOrg2d(~HSpace,~obj,~OrgX+7 ,~v +2)
  _Value := ~Value
  changed:=1
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
 rez:=SetObjectSize2d(~HSpace,~obj,29,~Height)

 obj := GetObject2dByName(~HSpace,~_HObject,"vsl_button")
 rez:=DelGroupItem2d(~HSpace,GetObjectParent2d(~HSpace,~obj),~obj)
 rez:=SetObjectSize2d(~HSpace,~_HObject,34,~Height+16)
 registerobject(~HSpace,~obj,"",WM_ALLMOUSEMESSAGE,273)
 registerobject(~HSpace,#0,"",WM_SPACEDONE,256)
 v := ~Value
 if (~max>=~min)
   if (~Value < ~min); v := ~min; endif
   if(~Value > ~max); v := ~max; endif
   v := ~OrgY - (~v - ~min)/(~max - ~min)*~Height + ~Height
 else
   if (~Value > ~min); v := ~min; endif
   if(~Value < ~max); v := ~max; endif
   v := ~OrgY + (~v - ~max)/(~min - ~max)*~Height
 endif

// if (~Value < ~min); v:= ~min; endif
// if(~Value > ~max); v := ~max; endif
// v := ~OrgY - (~v-~min)/(~max-~min) * ~Height + ~Height
 rez:=SetObjectOrg2d(~HSpace,~obj,~OrgX+7,~v + 2)
 _Value := ~Value
endif
 q ������� ���������� - ��������.
�������� ���������� � ��������� �� min �� ���,
��� ��������������� ������������� @�         6 5         �?      o �  5 !        �@6 5 6 �                                      �@6 5 w        �?                      &@       5@       &@e      E       z             �          �@6          �?6 + 5 �        �?      
          @                        8 5 �     &       '   3 �     &       '               �@6          �?6 + 5                                         5@       &@e  �   5         @                 7 5 �        9 5 a    : 5 >        8 5 I     
                     3 �    8 5 l        : 5 w     
                       	        @           @ K             �?  3 x   z    �        R + 5 �x   �     o �3 �    � z  CLASSNAME �z  ..  �� 5   z    �� � z    �| z  .vdr  |  	  
        �@h       b      D 	     E 
     z  line  �         F  q             =@  L      z  vsl_button  �         F  q             A@         0@ L      z           �@      q@�        z          �@       p@�         9 5 �    : 5 �        8 5 �     
                     3 �    8 5 �        : 5 �     
                       	        @           @ K         x�  �   �   n  
 �   