SB0  �      VSlider1  
 WindowName ��� ���� Main Window STRING    HSpace ���������� ������������   HANDLE�   Value ������������ ��������   FLOAT�   min ����������� ��������    FLOAT    max ������������ ��������  1 FLOAT    OrgX ���������� �����������    FLOAT�   OrgY ���������� �����������    FLOAT�   ChangeBegin     FLOAT�   xPos     FLOAT   yPos     FLOAT   _Value     FLOAT�   rez     FLOAT�   msg     FLOAT�   fwKeys     FLOAT   Height     FLOAT�   v     FLOAT�   ret     FLOAT�   _disable     FLOAT�   _HObject     HANDLE�   obj     HANDLE�       ���������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������wwwwwwww�������p      w���������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������� �      �  2Dc   �                <@      Y@      Y@      i@      i@ C:\STRATUM3\data\icons\        ����  �  
        	 
     �  
 ��    
 vsl_button      O@     �X@      4@      "@                      4@      "@               R@      P@              T@          R@      P@      R@      b@             P@      P@      0@                  P@      P@      T@      P@             P@      Z@      0@                  P@      Z@      T@      Z@             P@      b@      0@        	          P@      b@      T@      b@            @P@      `@      ,@        
         @P@      `@     �S@      `@            @P@      \@      ,@                 @P@      \@     �S@      \@            @P@      ^@      ,@                 @P@      ^@     �S@      ^@  	          @P@      X@      ,@                 @P@      X@     �S@      X@  
          @P@      V@      ,@                 @P@      V@     �S@      V@    �       @P@      T@      ,@                 @P@      T@     �S@      T@            @P@      a@      ,@                 @P@      a@     �S@      a@    �       @P@      R@      ,@                 @P@      R@     �S@      R@  �  
  e           e   ���     e   ���     e   ���     e   ���     e   ���     e   ���     e   ���     e  	 ���     e  
 ���     e   ���     e   ���     � 
 
 ��g   BM*      v   (   (   	         �                       �  �   �� �   � � ��  ��� ���   �  �   �� �   � � ��  ���                     ������������������wwwwwwww�wwwwwwww�w��������w��������p      w�p      w�wwwwwwww�wwwwwwww�wwwwwwww�wwwwwwww�������������������                        Q@ �    "                        @       @     End Of File <!>
 �	STRING WindowName
HANDLE HSpace
FLOAT Value,min,max,OrgX,OrgY,ChangeBegin
FLOAT local xPos,yPos,_Value
FLOAT local rez,msg,fwKeys,Height,v,ret,_disable
HANDLE local _HObject,obj

if (~max==~min)
  _disable := 1
  exit()
endif

if (msg==WM_SPACEDONE)
  _HObject:=GetHObject()
  HSpace:=#0
  obj:=#0
  msg:=0
  exit()
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
  rez:=SetObjectOrg2d(~HSpace, ~obj, ~OrgX, ~v - 4)
  _Value := ~Value
endif

 if (msg==WM_LBUTTONDOWN)
  ChangeBegin:=1
  rez:=SetBitmapSrcRect2d(~HSpace,~obj,20,0,20,9)
  SetCapture(~HSpace,"",0)
 endif

 if ((msg==WM_MOUSEMOVE)&&(ChangeBegin==1))
  if (yPos >= ~OrgY && yPos <= (~OrgY + ~Height))
   Value := 1 - (~yPos - ~OrgY)/~Height
   Value := ~Value*(~max - ~min) + ~min
   _Value := ~Value
   rez:=SetObjectOrg2d(~HSpace,~obj,~OrgX,~yPos - 4)
  endif
 endif

 if ((msg==WM_LBUTTONUP)&&(ChangeBegin==1))
  ChangeBegin:=0
  rez:=SetBitmapSrcRect2d(~HSpace,~obj,0,0,20,9)
  ReleaseCapture()
 endif

if (~obj)
else
 if (~WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(~WindowName); endif
 if (~HSpace); else exit(); endif
 if (GetWindowProp(GetWindowName(~HSpace),"CLASSNAME") != GetClassName(".."))
   _HObject := CreateObjectFromFile2D(~HSpace,AddSlash(GetClassDirectory(GetClassName("")))+GetClassName("")+".vdr", ~OrgX,~OrgY,PFC_MOVEOBJECT)
   ret := ObjectToTop2d(~HSpace,~_HObject)
 endif

 OrgX := GetObjectOrg2dx(~HSpace,~_HObject)
 OrgY := GetObjectOrg2dy(~HSpace,~_HObject)
 Height := GetObjectHeight2d(~HSpace,~_HObject)
 obj := GetObject2dByName(~HSpace,~_HObject,"vsl_button")
 rez:=DelGroupItem2d(~HSpace,GetObjectParent2d(~HSpace,~obj),~obj)
 registerobject(~HSpace,~obj,"",WM_ALLMOUSEMESSAGE,1)
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
 rez := SetObjectOrg2d(~HSpace,~obj,~OrgX,~v - 4)
 _Value := ~Value
endif
 q ������� ���������� - ��������.
�������� ���������� � ��������� �� min �� ���,
��� ��������������� ������������� @�   �      6 5         �?            �@6 5 . �                               
   7 5 �         9 5 n     : 5 K         8 5 V                           3 �     8 5 y         : 5 �                                       @ K     
         �@6 5 �        �?             4@                4@       "@e    z             �          �@6          �?6 + 5 8 	   9  	      ; + 5 8       �? 	                          
        	        @ K          �@6          �?6 + 5 n                                        4@       "@e  �   o t3 �y   z    �        R + 5 �y   �     o �3 �    � z  CLASSNAME �z  ..  �� 5 �  z    �� � z    �| z  .vdr  |            �@h       b      D      E      I      z  vsl_button  �         F  q      z           �@       �?�        z          �@       p@�         9 5 V    : 5 3        8 5 >                          3     8 5 a        : 5 l                                      @ K     
    x�  l   t   p  
 >   