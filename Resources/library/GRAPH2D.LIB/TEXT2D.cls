SB0  �"      Text ! 
 WindowName ��� ���� Main Window STRING   
 ObjectName ��� �������   STRING    HSpace$ ���������� ������������ ������������   HANDLE�   HObject ���������� ������   HANDLE�   Text
 ����������   STRING�   FontName
 ��� ������ MS SANS SERIF STRING   	 oldstring     STRING�   _ObjectName     STRING�   HFont     HANDLE�   HString     HANDLE�   HText     HANDLE�  	 TextCount     FLOAT�   TextNum     FLOAT    x ���������� ������   FLOAT�   y ���������� ������   FLOAT�   Height ������ ������   FLOAT�   Width     FLOAT�   Angle ���� �������� ������   FLOAT    flagsD ����� ������ (1 -italic | 2- underline| 4 - ������������ | 8 - bold)   FLOAT    zOrder ����� Z - �������   FLOAT�   Show ����  ��������� 1 FLOAT    _enable! ����  ����������� ������� ������� 1 FLOAT    _x     FLOAT�   _y     FLOAT�   _angle     FLOAT�   _zOrder     FLOAT�   ret     FLOAT�   msg     FLOAT�   FgColor  ���� ����� (������ �� ���������)
 rgb(0,0,0) COLORREF�   BgColor! ���� ������� (�� ��������� �����) Transparent COLORREF�   _FgColor     COLORREF�   _BgColor     COLORREF�   FontSize   12 FLOAT        �����������������wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�ww�wwwwwwwwwwwx�wwwwwwwwwwwwwwx�ww�wwwwwwwwwwwx�wwwwwwwwwwwwwwx�xx���xxx�ww�����癙��wwx�ww���ww�wwx�ww�y�wwww��wwwx�ww�w�www��wwwx�ww�wy���wwwx�ww�ww������wwwx�ww�wwy�ww��wwwx�ww�www�w��wwwx�ww�wwwy�w��wwwx�ww�wwww���wwwx�ww�wwwwy���wwwx�ww�wwwww���wwwx�ww�wwwwwy��wwwx�ww�wwwwww��wwwx�ww��~~~~~y�wwwx���������������x�wwwwwwwwwwwwwwx�ww�wwwwwwwwwwwx�wwwwwwwwwwwwwwx�ww�wwwwwwwwwwwx�wwwwwwwwwwwwwwx�ww�wwwwwwwwwwwx�wwwwwwwwwwwwwwx���������������� /      #  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
 ��           �u@      c@      @@      @@                      @@      @@    � 
 
 ��g   BM6      6   (                    �  �          ������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������  �  �  �  �  �  ��� ����� ����� ���  �  �  �  �  �  �  ����������������������������������������  �  �  �  �  �  ��� ����� ����� ���  �  �  �  �  �  �  ���������������������������������������������   �  �  ��� �� ����������������� ��   �  �  �  ��� ����������������������������������������� ������  �  �  �������������������������  �  �  �  ������������������������������������������������ ������  �  �  ����������������������  �  �  �  ��������������������������������������������� ������������  �  �  �  �  �  �  �  �  �  �  �  �  ������������������������������������������������ ������������  �  �  �  �  �  �  �  �  �  �  �  ��������������������������������������������� �� ���������������  �  �  �������������  �  �  �  ������������������������������������������������ ������������������  �  �  ����������  �  �  �  ��������������������������������������������� ������������������������  �  �  �������  �  �  �  ������������������������������������������������ ������������������������  �  �  ����  �  �  �  ��������������������������������������������� �� ���������������������������  �  �  �  �  �  �  ������������������������������������������������ ������������������������������  �  �  �  �  �  ��������������������������������������������� �� ���������������������������������  �  �  �  �  ������������������������������������������������ ������������������������������������  �  �  �  ��������������������������������������������� �� �� �� ����� ����� ����� ����� ����� ���  �  �  ������������������������������������������������ ����� ����� ����� ����� ����� ����� �����   �  �������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������    ��   End Of File <!>
 �
STRING WindowName,ObjectName
HANDLE HSpace,HObject
STRING Text,FontName
STRING local oldstring,_ObjectName
HANDLE local HFont,HString,HText
FLOAT TextCount,TextNum,x,y,Height,Width,Angle,flags,zOrder,Show,_enable
FLOAT local _x,_y,_angle,_zOrder,ret,msg
COLORREF FgColor,BgColor
COLORREF local _FgColor,_BgColor

if (WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(~WindowName); endif

if (~ObjectName != "" && _ObjectName != ~ObjectName && ~HSpace!=#0)
  if (GetObject2dByName(~HSpace, #0,~ObjectName))
    HObject := GetObject2dByName(~HSpace, #0,~ObjectName)
  endif
  if(~HObject)
    HText := GetTextObject2d(~HSpace,~HObject)
    TextCount:=GetTextCount2d(~HSpace,~HText)
    HString := GetTextString2d(~HSpace,~HText)
    oldstring:=getstring2d(~HSpace,~HString)
    HFont := GetTextFont2d(~HSpace,~HObject)
    x := GetObjectOrg2dx(~HSpace, ~HObject)
    y := GetObjectOrg2dy(~HSpace, ~HObject)
    FgColor := GetTextFgColor2d(~HSpace, ~HText)
    BgColor := GetTextBkColor2d(~HSpace, ~HText)
    _x:=~x; _y:=~y; _BgColor := ~BgColor; _FgColor := ~FgColor
    zOrder := GetZOrder2d(~HSpace,~HObject)
    _ObjectName := ~ObjectName
    RegisterObject(~HSpace,#0,"",WM_SPACEDONE,16)
    exit()
  endif
endif

if(not(~HObject) && ~HSpace!=#0)
  HFont := CreateFont2D(~HSpace,~FontName,~FontSize,~flags)
  text := ~text
  HString := CreateString2D(~HSpace,~text)
  HText := CreateText2D(~HSpace,~HFont,~HString,~FgColor,~BgColor)
  HObject := CreateRasterText2D(~HSpace,~HText,~x,~y,~angle)
  zOrder := GetZOrder2d(~HSpace,~HObject)
  _zOrder :=~zOrder
  _x:=~x; _y:=~y;  _BgColor := BgColor; _FgColor := FgColor
else

  x := ~x - _x + GetObjectOrg2dx(~HSpace, ~HObject)
  y := ~y - _y + GetObjectOrg2dy(~HSpace, ~HObject)
  ret := SetObjectOrg2d(HSpace,HObject,~x,~y) 
  _x:=~x; _y:=~y

  if (_angle!=~angle)
    ret := RotateObject2d(~HSpace,~HObject,~x,~y,~angle- _angle)
    _angle:=~angle
  endif

  if (_FgColor !=~FgColor || _BgColor !=~BgColor)
    ret := SetText2D(HSpace,HText,HFont,HString,~FgColor,~BgColor)
    _FgColor := ~FgColor; _BgColor := ~BgColor
  endif

  if(oldstring != ~text)
    ret := SetString2d(~HSpace,~HString,~text)
    Width := GetActualWidth2d(~HSpace,~HObject)
    Height := GetActualHeight2d(~HSpace,~HObject)
    ret := SetObjectSize2d(~HSpace,~HObject,~Width,~Height)
    oldstring := ~text
  endif

  if (_zOrder !=~zOrder)
    ret := SetZOrder2d(~HSpace,~HObject,~zOrder)
  endif

  zOrder := GetZOrder2d(~HSpace,~HObject);  
  _zOrder :=~zOrder
  
  ret := SetShowObject2d(~HSpace,~HObject,~Show)
endif

if(msg==WM_SPACEDONE)
  _ObjectName:=""
  HObject:=#0
  HSpace:=#0
  msg:=0
endif
 * �����
x,y - ����������
text - ���������� @B   �  x   z    �        R + 5  y   �   y  z    � x  y  � +        S + 5 �        y  � o =        y  �     o �     � 
    
 P     
 � 	    	 �{      �      D      E     
 [     
 \                      `  y  {         z          �@       0@�     W        S + 5   y       h  y  {    y  i 	      	     j 
    
       k      `                      3 �         D            E           K              7 5 P             ~          S     S , 5 t   
    	     Z          x  y  � 5 �   	 y  m      �       �           L  y  {      7 5 �      c      `            l          �@6 5 �z    {                              F�|  �   �   �  
 �   