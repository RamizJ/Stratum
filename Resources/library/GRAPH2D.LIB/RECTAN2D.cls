SB0  �*     	 Rectangle $ 
 WindowName ��� ���� Main Window STRING   
 ObjectName ��� �������   STRING    _ObjectName     STRING�   HSpace$ ���������� ������������ ������������   HANDLE�   HObject ���������� ��������������   HANDLE�   HPen     HANDLE�   HBrush     HANDLE�   x ����������    FLOAT�   y ����������    FLOAT�   Width ������   FLOAT�   Height ������   FLOAT�   _x     FLOAT�   _y     FLOAT�   ret     FLOAT�   _Width     FLOAT�   _Height     FLOAT�   PenStyle) ����� ����� (�������� ����� �� ���������)   FLOAT�   PenWidth" ������� ����� (1 ������ ���������)   FLOAT�   PenROP. ��������� �������� � ������ (AND �� ���������) 13 FLOAT�  
 BrushStyle% ����� ������� (�������� �� ���������)   FLOAT�  
 BrushHatch$ ��������� (����������� �� ���������)   FLOAT�   BrushROP. ��������� �������� � ������ (AND �� ���������) 13 FLOAT�   zOrder ����� Z - �������   FLOAT�   Show ����  ��������� 1 FLOAT   	 _PenStyle     FLOAT�  	 _PenWidth     FLOAT�   _PenROP     FLOAT�   _zOrder     FLOAT�   _BrushStyle     FLOAT�   _BrushHatch     FLOAT�  	 _BrushROP     FLOAT�   PenColor  ���� ����� (������ �� ���������)   COLORREF�  
 BrushColor" ���� ������� (�� ��������� ������)   COLORREF�  	 _PenColor     COLORREF�   _BrushColor     COLORREF�   _enable! ����  ����������� ������� ������� 1 FLOAT        �����������������wwwwwwwwwwwwwwx�wtwwwwwwwwwtwwx�wwxwwwwwwwxwwwx�wt����������wwx�wwxwwwwwwwxwwwx�wtwwwwwwwwwtwwx�wwwwwwwwwwwwwwx�wp          ttx�wp����������w�x�wp����������w�x�wp����������x�x�wp����������w�x�wp����������w�x�wp����������w�x�wp����������w�x�wp����������w�x�wp����������w�x�wp����������x�x�wp����������w�x�wp����������w�x�tt          ttx�wwwwwwwwwwwwwwx�wtwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wtwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wtwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wtwwwwwwwwwwwwx�wwwwwwwwwwwwwwx���������������� /      #  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
 ��            m@      d@      @@      @@                      @@      @@    � 
 
 ��g   BM6      6   (                    �  �          ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������  ����������������������������������������������������������  ����������������������������������������������������������������������������������������������������������������������������������  ����������������������������������������������������������  ����������������������������������������������������������������������������������������������������������������������������������  ����������������������������������������������������������  ���������������������������������������������������������������������������������������������������������������������������������                                                               ����  ����  ���������������������    �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �    ���������������������������������    �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �    ���������������������������������    �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �    ���������������������������������    �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �    ���������������������������������    �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �    ���������������������������������    �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �    ���������������������������������    �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �    ���������������������������������    �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �    ���������������������������������    �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �    ���������������������������������    �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �    ���������������������������������    �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �    ���������������������������������    �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �    ����������������������  ����  ����                                                              ����  ����  ����������������������������������������������������������������������������������������������������������������������  ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������  ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������  ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������  ������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������         End Of File <!>
 �STRING WindowName,ObjectName
STRING local _ObjectName
HANDLE HSpace,HObject
HANDLE local HPen,HBrush
FLOAT x,y,Width,Height
FLOAT local _x,_y,ret,_Width,_Height
FLOAT PenStyle,PenWidth,PenROP,BrushStyle,BrushHatch,BrushROP,zOrder,Show
FLOAT local  _PenStyle,_PenWidth,_PenROP,_zOrder,_BrushStyle,_BrushHatch,_BrushROP
COLORREF PenColor,BrushColor
COLORREF local _PenColor,_BrushColor
FLOAT _enable

 if (WindowName != ""&& (~HSpace==#0));  HSpace := GetWindowSpace(~WindowName); endif
 if (~ObjectName != "" &&  _ObjectName != ~ObjectName)
  if (GetObject2dByName(~HSpace, #0,~ObjectName))
   HObject := GetObject2dByName(~HSpace, #0,~ObjectName)
  endif
  if (~HObject)
   HPen := GetPenObject2d(~HSpace, ~HObject)
   HBrush := GetBrushObject2d(~HSpace, ~HObject)
   x := GetVectorPoint2dx(~HSpace, ~HObject, 0)
   y := GetVectorPoint2dy(~HSpace, ~HObject, 0)
   Width := GetVectorPoint2dx(~HSpace, ~HObject, 1) - ~x
   if (~Width == 0); Width := GetVectorPoint2dx(~HSpace, ~HObject, 2) - ~x; endif
   Height := GetVectorPoint2dy(~HSpace, ~HObject, 1) - ~y
   if (~Height == 0); Height := GetVectorPoint2dy(~HSpace, ~HObject, 2) - ~y; endif
   _x:=~x; _y:=~y;  _Width:=~Width; _Height:=~Height

   zOrder := GetZOrder2d(~HSpace,~HObject);    _zOrder :=~zOrder
   PenColor := GetPenColor2D(~HSpace,~HPen);   _PenColor := ~PenColor
   PenWidth := GetPenWidth2d(~HSpace,~HPen);   _PenWidth := ~PenWidth
   PenStyle := GetPenStyle2d(HSpace,HPen);   _PenStyle := ~PenStyle
   PenROP := GetPenROP2d(~HSpace,~HPen);   _PenROP :=~PenROP
   BrushROP := GetBrushROP2d(~HSpace,~HBrush);   _BrushROP :=~BrushROP
   BrushHatch := GetBrushHatch2d(~HSpace,~HBrush);   _BrushHatch :=~BrushHatch
   BrushStyle := GetBrushStyle2d(~HSpace,~HBrush);   _BrushStyle :=~BrushStyle
   BrushColor := GetBrushColor2D(~HSpace,~HBrush);   _BrushColor := ~BrushColor
   _ObjectName := ~ObjectName
   exit()  
  endif
 endif

 if (not(~HObject))
  HPen:=CreatePen2d(~HSpace,~PenStyle,~PenWidth,~PenColor,~PenROP)
  HBrush:=CreateBrush2d(~HSpace,~BrushStyle,~BrushHatch,~BrushColor,#0,~BrushROP)
  HObject := CreatePolyLine2d(~HSpace,~HPen,~HBrush,~x,~y,~x,~y+~Height,~x+~Width,~y+~Height,~x+~Width,~y,~x,~y)
  if (~zOrder)
   ret := SetZOrder2d(~HSpace,~HObject,~zOrder)
  else
   zOrder := GetZOrder2d(~HSpace,~HObject)
  endif 
   _zOrder :=~zOrder
   _x:=~x; _y:=~y;
 else
   x := ~x - _x + GetVectorPoint2dx(~HSpace, ~HObject, 0)
   y := ~y - _y + GetVectorPoint2dy(~HSpace, ~HObject, 0)

   ret := SetVectorPoint2d(HSpace,HObject,0,~x,~y)
   ret := SetVectorPoint2d(HSpace,HObject,1,~x,~y+~Height)
   ret := SetVectorPoint2d(HSpace,HObject,2,~x+~Width,~y+~Height)
   ret := SetVectorPoint2d(HSpace,HObject,3,~x+~Width,~y)
   ret := SetVectorPoint2d(HSpace,HObject,4,~x,~y)
   _x:=~x; _y:=~y;

  if (FLOAT(_PenColor) != FLOAT(~PenColor))
   ret := SetPenColor2D(~HSpace,~HPen,~PenColor);   _PenColor:=~PenColor
  endif
  if (_PenWidth != ~PenWidth)
   ret := SetPenWidth2d(~HSpace,~HPen,~PenWidth);   _PenWidth := ~PenWidth
  endif
  if (_PenStyle !=~PenStyle)
   ret := SetPenStyle2d(~HSpace,~HPen,~PenStyle);   _PenStyle :=~PenStyle
  endif
  if (_PenROP !=~PenROP)
   ret := SetPenROP2d(~HSpace,~HPen,~PenROP);   _PenROP :=~PenROP
  endif
  if (FLOAT(_BrushColor) != FLOAT(~BrushColor))
   ret := SetBrushColor2D(~HSpace,~HBrush,~BrushColor);   _BrushColor:=~BrushColor
  endif
  if (_BrushHatch != ~BrushHatch)
   ret := SetBrushHatch2d(~HSpace,~HBrush,~BrushHatch);   _BrushHatch := ~BrushHatch
  endif
  if (_BrushStyle !=~BrushStyle)
   ret := SetBrushStyle2d(~HSpace,~HBrush,~BrushStyle);   _BrushStyle :=~BrushStyle
  endif
  if (_BrushROP !=~BrushROP)
   ret := SetBrushROP2d(~HSpace,~HBrush,~BrushROP);   _BrushROP :=~BrushROP
  endif
  if (_zOrder !=~zOrder)
   ret := SetZOrder2d(~HSpace,~HObject,~zOrder)
  endif

  zOrder := GetZOrder2d(~HSpace,~HObject);   _zOrder :=~zOrder
  ret := SetShowObject2d(~HSpace,~HObject,~Show)
endif
 d ������������� �� ���� ������
x1,y1 - ���������� �������
x2,y2 - ���������� ��������������� ������� @B   �  x   z    �        R + 5  y   �   y  z    � x  y  � + 5 (       y  � o 6        y  �     o (    0      /               3               4             �?3    	  	          6 5 �             @3    	            �?4    
  
          6 5 �             @4    
          	    
       `          :     !     =          <          ;          �          �          �          �       " y  {      W 5 �          9                  M                 
     	     
     	        �
     5 �      c  3 �    `              3 �                  3                     4                    8             �?     
  8              @   	     
  8             @   	    8             @    8           !     7 5 6      >     !     7 5 J      A          7 5 ^      @          7 5 r      ?       "      7 5 �       �      "     7 5 �      �          7 5 �      �          7 5 �      �          7 5 �      c      `            l     �d�  �   �   �  
 p$   