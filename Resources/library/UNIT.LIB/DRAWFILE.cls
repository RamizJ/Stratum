SB0  �      DrawFile  
 WindowName ��� ���� Main Window STRING    HSpace$ ���������� ������������ ������������   HANDLE�   FileName ��� ���������� �����   STRING    FontName ��� ������������� ������ MS SANS SERIF STRING    From ������ ������������ ������   FLOAT    Len ����� ������������ �����   FLOAT    x ���������� ������   FLOAT    y ���������� ������   FLOAT    FontSize ������ ������ 8 FLOAT    flags� ����� ������: 0 - ����������, 1 - Italic(���������), 2 - Underlined(������������), 4 - StriceOut (�������������), 8 - Bold(������)   FLOAT    HString     HANDLE�   HFont     HANDLE�   HText     HANDLE�   HObject     HANDLE�   HGroup     HANDLE�   HStream     HANDLE�   text     STRING�   ret     FLOAT�   _from     FLOAT�   _len     FLOAT�   counter     FLOAT�   w     FLOAT�   FgColor ���� ������   COLORREF    BgColor	 ���� ���� rgbex(0,128,0,1) COLORREF    _enable! ����  ����������� ������� ������� 1 FLOAT    update     FLOAT�  
 �STRING WindowName
HANDLE HSpace
STRING FileName,FontName
FLOAT From,Len
FLOAT x,y
FLOAT FontSize,flags
HANDLE local HString,HFont,HText,HObject,HGroup,HStream
STRING local text
FLOAT local ret,_from,_len,counter,w
COLORREF FgColor,BgColor
FLOAT _enable

if (WindowName != "");  HSpace := GetWindowSpace(~WindowName); endif

if(FileName == "")
 exit()
else
 if (HStream)
 else
  HStream:=CreateStream("FILE",~FileName,"READONLY")
 endif
endif

if (_from != ~from || _len != ~len || update)
if (~HStream != #0 && ~HSpace != #0)

  if (HFont == #0)
   HFont := CreateFont2D(~HSpace,~FontName,~FontSize,~flags)
  endif
   w := 0
   ret := DeleteObject2d(~HSpace,HGroup)
  HGroup := #0
  update:=0
  counter := 1
  ret := Seek(~HStream,0)
  while (~counter < (~from + ~len))
   text := GetLine(~HStream,255,Chr(13))
   if (~counter >= ~from)
    text := Change(~text,Chr(10),"")
    text := Change(~text,Chr(13),"")

    HString := CreateString2D(~HSpace,~text)
    HText := CreateText2D(~HSpace,~HFont,~HString,~FgColor,~BgColor)
    HObject := CreateRasterText2D(~HSpace,~HText,~x,~y + ~w,0)

    if  (~HGroup == #0)
     HGroup := CreateGroup2d(~HSpace,~HObject)
    else
     ret := AddGroupItem2d(~HSpace,~HGroup,~HObject)
    endif
    w := ~w + GetObjectHeight2d(~HSpace,~HObject) - 1
   endif
   counter := ~counter + 1
   if (Eof(~HStream)); break; endif
  endwhile

  _from := ~from
  _len := ~len
endif
endif 1 ���������� ��������� ����� ����� ���������� ����� @P   9   WINDOWS.DBM 7  x   z    � 5  y   �   x  z    � 5    3 /   o  3 / z  FILE  y  z  READONLY  �       7     7 ,   , 5 6       S        S + 5 6       R 5 a   y     	 h                 C                           �?             �          : 5 .        �o@       *@� � {      9 5 y         $@� z    w {  y         *@� z    w {    y  i 
      
     j                      k         R 5     o   3       n        I        �?            �?     � 5 ,3 .3 �            2Y�  	  
 �	   