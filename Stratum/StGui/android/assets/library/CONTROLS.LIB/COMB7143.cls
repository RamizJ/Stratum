SB0        ComboBox  
 WindowName ��� ���� Main Window STRING    HSpace$ ���������� ������������ ������������   HANDLE�   FileName ��� ���������� �����   STRING    Text ����� ��������� ������ � ������   STRING�   value( ������ (����� ��������� ������ � ������)   FLOAT�   OrgX ���������� �����������    FLOAT    OrgY ���������� �����������    FLOAT    HObject     HANDLE�   _HObject     HANDLE   HStream     HANDLE�   ret     FLOAT�   wNotifyCode     FLOAT   msg     FLOAT�   _value     FLOAT�   _text     STRING�  	 _FileName     STRING�       ������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������               wwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpww                        2D^   �                        Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
    � 
 
 ��     combo      T@      H@      Y@      8@ ComboBox   !T    d d                T@      H@     @V@      1@         T@      H@      e@      H@      e@      P@      T@      P@      T@      H@             d@      H@      1@      1@         d@      H@      f@      H@      f@      P@      d@      P@      d@      H@  � 
 
  e           � 
 
  f   ���                End Of File <!>
 �STRING WindowName
HANDLE HSpace
STRING FileName,Text
FLOAT value,OrgX,OrgY
HANDLE local HObject,_HObject,HStream
FLOAT local ret,wNotifyCode,msg,_value
STRING  local _text,_FileName

if (HObject)
 if (_text != ~text)
  ret:=SetControlText2d(HSpace,HObject,~text)
  _text := ~text
  exit()
 endif

 if (_value != ~value)
   ret:=LBSetSelIndex(~HSpace,~HObject,~value)
   text := LBGetString(HSpace,~HObject,~value)
   _text := ~text
  _value := ~value
  exit()
 endif

 if (_FileName != ~FileName)
  _FileName := ~FileName
  ret := LBClearList(~HSpace,~HObject)
  text := AddSlash(GetProjectDirectory())
  text := ~text + ~FileName
  if (FileExist(~text) == 0)
   text := "file error"
   exit()
  endif
  HStream:=CreateStream("FILE",~text,"READONLY")
  While (not(Eof(~HStream)))
   text := GetLine(~HStream,255,Chr(13))
   text := Change(~text,Chr(10),"")
   text := Change(~text,Chr(13),"")
   if (length(~text))
    ret:=LBAddString(~HSpace,~HObject,~text)
   endif
  endwhile

  ret := CloseStream(~HStream)
  _value := ~value
  ret:=LBSetSelIndex(~HSpace,~HObject,~value)
  text:=LBGetString(~HSpace,~HObject,0)
 endif

 if (msg==WM_SPACEDONE)
   HObject:=#0
   HSpace:=#0
   msg:=0
   exit()
 endif

 if (msg==WM_CONTROLNOTIFY)
  if (wNotifyCode==9)
   value := LBGetSelIndex(HSpace,HObject)
   text := LBGetString(HSpace,~HObject,~value)
   _text := ~text
  endif
  if (wNotifyCode !=5)
    text:=GetControlText2d(HSpace,HObject)
   _text := ~text
  endif

 endif
else
  if (WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(WindowName); endif
  if (~HSpace); else exit(); endif
  if (~FileName == ""); exit(); endif
  if (GetWindowProp(GetWindowName(~HSpace),"CLASSNAME") != GetClassName(".."))
//   HObject:=CreateControlObject2d(HSpace,"COMBOBOX","",1342177795,OrgX,OrgY,100,150,10)
   HObject := CreateObjectFromFile2D(~HSpace,AddSlash(GetClassDirectory(GetClassName("")))+GetClassName("")+".vdr", OrgX,OrgY,PFC_MOVEOBJECT)
  else
   HObject := GetObject2dByName(~HSpace,~_HObject,"combo")
   ret := DelGroupItem2d(~HSpace,GetObjectParent2d(~HSpace,~HObject),~HObject)
  endif

  ret := SetObjectSize2d(~HSpace,~HObject,GetObjectWidth2d(~HSpace,~HObject),200)
  ret := ObjectToTop2d(~HSpace,~HObject)
  RegisterObject(~HSpace,~HObject,"",WM_CONTROLNOTIFY,0)
 registerobject(~HSpace,#0,"",WM_SPACEDONE,256)
  _FileName := ~FileName
  text := AddSlash(GetProjectDirectory())
  text := ~text + ~FileName
  if (FileExist(~text) == 0)
   text := "file error"
   exit()
  endif

  HStream:=CreateStream("FILE",~text,"READONLY")
  While (not(Eof(~HStream)))
   text := GetLine(~HStream,255,Chr(13))
   text := Change(~text,Chr(10),"")
   text := Change(~text,Chr(13),"")
   if (length(~text))
    ret:=LBAddString(~HSpace,~HObject,~text)
   endif
 endwhile

 ret := CloseStream(~HStream)
 _value := ~value
 ret:=LBSetSelIndex(~HSpace,~HObject,~value)
 text:=LBGetString(~HSpace,~HObject,0)
endif
 � ����� ������������ ����� ������ �� ������,
�������������� � ����������� �������� ��������
��������� ����������.
������ ��������� �� ���������� ���������� ����� �� �������.   �   ~    o :x  y  � 5      y  � 
 y  {        7 5 ;       � 
       �{  y  {        x  y  � 5 � y  {      � 
 � � {  y  y  | {  y  �         6 5 n z  file error  {    z  FILE  y  z  READONLY  �  	  	 � - 5 �  	       �o@       *@� � {  y         $@� z    w {  y         *@� z    w {  y  � 5 �     y  � 
 3   	 �  
           � 
              �{          �@6 5 �                                     �@6 5 8         "@6 5 #    �        �{  y  {           @7 5 8    �{  y  {  3 }x   z    �        R + 5 Nx   �     o T3 U  y  z    � 5 ^    � z  CLASSNAME �z  ..  �� 5 �  z    �� � z    �| z  .vdr  |            �@h   3 �    z  combo �         F  q 
         H       i@L 
     b 
     z           �@         �        z          �@       p@� y  {  � � {  y  y  | {  y  �         6 5 z  file error  {    z  FILE  y  z  READONLY  �  	  	 � - 5 _ 	       �o@       *@� � {  y         $@� z    w {  y         *@� z    w {  y  � 5 ]    y  � 
 3  	 �  
           � 
              �{     %x�  S   [   |  
 J   