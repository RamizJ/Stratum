SB0  <.      Ellipse ) 
 WindowName ��� ���� Main Window STRING   
 ObjectName ��� �������   STRING    _ObjectName     STRING�   HSpace$ ���������� ������������ ������������   HANDLE�   HObject ���������� �������   HANDLE�   HPen     HANDLE�   HBrush     HANDLE�   x ���������� ������   FLOAT�   y ���������� ������   FLOAT�   r1 ���������� ��������   FLOAT�   r2 ���������� ��������   FLOAT�   _x     FLOAT�   _y     FLOAT�   _r1     FLOAT�   _r2     FLOAT�   ret     FLOAT�   counter     FLOAT�   step     FLOAT�   xx     FLOAT�   yy     FLOAT�   init     FLOAT   PenStyle) ����� ����� (�������� ����� �� ���������)   FLOAT�   PenWidth" ������� ����� (1 ������ ���������)   FLOAT�   PenROP. ��������� �������� � ������ (AND �� ���������) 13 FLOAT�  
 BrushStyle% ����� ������� (�������� �� ���������)   FLOAT�  
 BrushHatch$ ��������� (����������� �� ���������)   FLOAT�   BrushROP0 ��������� �������� � �������� (AND �� ���������) 13 FLOAT�   zOrder ����� Z - �������   FLOAT�   Show ����  ��������� 1 FLOAT   	 _PenStyle     FLOAT�  	 _PenWidth     FLOAT�   _PenROP     FLOAT�   _zOrder     FLOAT�   _BrushStyle     FLOAT�   _BrushHatch     FLOAT�  	 _BrushROP     FLOAT�   PenColor
 ���� �����   COLORREF�  
 BrushColor ���� �������   COLORREF�  	 _PenColor     COLORREF�   _BrushColor     COLORREF�   _enable! ����  ����������� ������� ������� 1 FLOAT        �����������������wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwww�wwwwwwx�wwwwwwwwwwwwwwx�wwwwwww�wwwwwwx�wwwwwwwwwwwwwwx�wwwwwww�wwwwwwx�wwwww�����wwwwx�wwwy�ww�wy�wwwx�wwy�wwwwwww�wwx�ww�wwww�wwww�wx�wywwwwwwwwwwywx�wywwwww�wwwwywx�w�wwwwwwxwwxw�x�����������������w�wwwww�xwwxw�x�wywwwwx�wwwwywx�wywwwww�wwwwywx�ww�wwww�wwww�wx�wwy�wwx�www�wwx�wwwy�ww�wy�wwwx�wwwww�����wwwwx�wwwwwww�wwwwwwx�wwwwwwwwwwwwwwx�wwwwwww�wwwwwwx�wwwwwwwwwwwwwwx�wwwwwww�wwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx���������������� /      #  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
 ��            g@      c@      @@      @@                      @@      @@    � 
 
 ��g   BM6      6   (                    �  �          ������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������  �  �  �  �  �  �  �  �  �������������������������������������������������������������  �  �  ����������������������������  �  �  ����������������������������������������������  �  ����������������������������������������������  �  �������������������������������������  ����������������������������������������������������������  �������������������������������  ����������������������������������������������������������������  ����������������������������  ����������������������������������������������������������������  �������������������������  ����������������������������������������������������������������������  ����������������������������������������������������������������������������������������������������������������������  ����������������������������������������������������������������������  �������������������������  ����������������������������������������������������������������  ����������������������������  ����������������������������������������������������������������  �������������������������������  ����������������������������������������������������������  �������������������������������������  �  ����������������������������������������������  �  ����������������������������������������������  �  �  ����������������������������  �  �  �������������������������������������������������������������  �  �  �  �  �  �  �  �  ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������         End Of File <!>
 �STRING WindowName,ObjectName
STRING local _ObjectName
HANDLE HSpace,HObject
HANDLE local HPen,HBrush
FLOAT x,y,r1,r2
FLOAT local _x,_y,_r1,_r2
FLOAT local ret,counter,step,xx,yy,init
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
   x := GetObjectOrg2dx(~HSpace, ~HObject)
   y := GetObjectOrg2dy(~HSpace, ~HObject)
   r1 := GetObjectWidth2d(~HSpace, ~HObject) / 2
   r2 := GetObjectHeight2d(~HSpace, ~HObject) / 2
   _x:=~x; _y:=~y; _r1:=~r1; _r2 :=~r2
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
  HBrush:=CreateBrush2d(~HSpace,~BrushStyle,~BrushHatch,~BrushColor,Handle(0),~BrushROP)

 counter := 2*PI
 step :=6.28/(3+Average(~r1,~r2))
  xx := ~x + ~r1*cos(~counter);  yy := ~y + ~r2*sin(~counter)
  counter := ~counter - ~step
  HObject := CreateLine2d(~HSpace,~HPen,~HBrush,~xx,~yy)
 while(~counter > 0)
  xx := ~x + ~r1*cos(~counter);  yy := ~y + ~r2*sin(~counter)
  ret := AddPoint2d(~HSpace,~HObject,1,~xx,~yy)
  counter := ~counter - ~step
 endwhile
  xx := ~x + ~r1*cos(0);  yy := ~y + ~r2*sin(0)
  ret := AddPoint2d(~HSpace,~HObject,1,~xx,~yy)

  zOrder := GetZOrder2d(~HSpace,~HObject)
   _zOrder :=~zOrder;   _x:=~x; _y:=~y; _r1:=~r1; _r2 :=~r2
 else

  if (_r1!=~r1 || _r2 !=~r2)  // ��������� ������ 
   ret := DeleteObject2d(~HSpace,~HObject)
   counter := 2*PI
   step :=6.28/(3+Average(r1,r2))
   xx := ~x + ~r1*cos(~counter);  yy := ~y + ~r2*sin(~counter)
   counter := ~counter - ~step
   HObject := CreateLine2d(~HSpace,~HPen,~HBrush,~xx,~yy)
   while(~counter > 0)
    xx := ~x + ~r1*cos(~counter);  yy := ~y + ~r2*sin(~counter)
    ret := AddPoint2d(~HSpace,~HObject,1,~xx,~yy)
    counter := ~counter - ~step
   endwhile 
   xx := ~x + ~r1*cos(0);  yy := ~y + ~r2*sin(0)
   ret := AddPoint2d(~HSpace,~HObject,1,~xx,~yy)
    _r1:=~r1; _r2 :=~r2 
  endif

   x := ~x - _x + GetObjectOrg2dx(~HSpace, ~HObject)+~r1
   y := ~y - _y + GetObjectOrg2dy(~HSpace, ~HObject)+~r2
   ret := SetObjectOrg2d(~HSpace,~HObject,~x-~r1,~y-~r2)
   _x:=~x; _y:=~y

  if (FLOAT(_PenColor) != FLOAT(~PenColor))
   ret := SetPenColor2D(HSpace,HPen,~PenColor);   _PenColor:=~PenColor
  endif
  if (_PenWidth != ~PenWidth)
   ret := SetPenWidth2d(HSpace,HPen,~PenWidth);   _PenWidth := ~PenWidth
  endif
  if (_PenStyle !=~PenStyle)
   ret := SetPenStyle2d(HSpace,HPen,~PenStyle);   _PenStyle :=~PenStyle
  endif
  if (_PenROP !=~PenROP)
   ret := SetPenROP2d(HSpace,HPen,~PenROP);   _PenROP :=~PenROP
  endif
  if (FLOAT(_BrushColor) != FLOAT(~BrushColor))
   ret := SetBrushColor2D(HSpace,HBrush,~BrushColor);   _BrushColor:=~BrushColor
  endif
  if (_BrushHatch != ~BrushHatch)
   ret := SetBrushHatch2d(HSpace,HBrush,~BrushHatch);   _BrushHatch := ~BrushHatch
  endif
  if (_BrushStyle !=~BrushStyle)
   ret := SetBrushStyle2d(HSpace,HBrush,~BrushStyle);   _BrushStyle :=~BrushStyle
  endif
  if (_BrushROP !=~BrushROP)
   ret := SetBrushROP2d(HSpace,HBrush,~BrushROP);   _BrushROP :=~BrushROP
  endif
  if (_zOrder !=~zOrder)
   ret := SetZOrder2d(~HSpace,~HObject,~zOrder)
  endif

  zOrder := GetZOrder2d(~HSpace,~HObject);   _zOrder :=~zOrder
  ret := SetShowObject2d(~HSpace,~HObject,~Show)
endif
 $ ������
x,y - �����
r1,r2 - ������� @B   �  x   z    �        R + 5  y   �   y  z    � x  y  � + 5 �        y  � o 6        y  �     o �     0      /      D      E      H        @  	     I        @  
          	    
       `           : $  $  &     =          <          ;          �     #     �     "     �     !     � %  %  ' y  {      W 5 �       $   9         %             M          @ ��DT�!	@    ��Q�@       @ 	  
 (        	           
                         ,             8 5 �   	           
                   �?    -         3 W   	                  
                          �?    -      `                	    
   3 �   	 7    
 7 , 5 �    C          @ ��DT�!	@    ��Q�@       @ 	  
 (        	           
                         ,             8 5 p   	           
                   �?    -         3 7   	                  
                          �?    -   	    
            D  	             E  
           	     
  K           &   $  7 5 �     $ >   $  &     7 5       A          7 5       @          7 5 /      ?       '   %  7 5 E     % �   %  '  "   7 5 Y      �     "  !   7 5 m      �     !  #   7 5 �      �     #      7 5 �      c      `             l     P~�  �   �   	  
 �&   