SB0  �      Object2D  
 WindowName ��� ���� Main Window STRING   
 ObjectName ��� �������   STRING    HSpace$ ���������� ������������ ������������   HANDLE�   HObject ���������� ������������ �������   HANDLE�   HGroup     HANDLE�   _ObjectName     STRING�   x ���������� ������������ �������   FLOAT�   y ���������� ������������ �������   FLOAT�   Width ������� ������������ �������   FLOAT�   Height ������� ������������ �������   FLOAT�   Attach ���� �������� ������� � ������   FLOAT    zOrder ����� Z - �������   FLOAT�   Show ����  ��������� 1 FLOAT    x0 ���������� ������ ��������   FLOAT    y0 ���������� ������ ��������   FLOAT    angle" ���� �������� ������������ �������   FLOAT    _x     FLOAT�   _y     FLOAT�   _Width     FLOAT�   _Height     FLOAT�   _zOrder     FLOAT�   _angle     FLOAT�   ret     FLOAT�   _enable! ����  ����������� ������� ������� 1 FLOAT        �����������������wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwxwwwwwwww�wwx�wwwwwwwwwwwwwwx�wxx����������wx�wwwy����wwwwwwx�wwxw�����ww�wwx�wwww���|��wwwwx�wwx������G�wwx�www�y|���Dwwwx�wwx��w������wwx�www��w�����Gwwx�wwx��w�����Gwwx�www��"������wwx�wwx��",����wwwx�www��""���G�wwx�wwx�""$O�wwwwx�www�"""/�w�wwx�wwxw��"/��wwwwx�wwww����'�w�wwx�wwxw�"""'wwwwwx��������������wx�wwxwwwwwwwwwwwx�wwwwwwwwwww�wwx�wwxwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwxwwwwwwwwwwwx���������������� o      c  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
     �       @r@     Pc@      @@      @@                      @@      @@    � 
 
 ��g   BMv      v   (                    �  �               �  �   �� �   � � ��  ��� ���   �  �   �� �   � � ��  ��� �����������������wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwxwwwwwwww�wwx�wwwwwwwwwwwwwwx�wxx����������wx�wwwy����wwwwwwx�wwxw�����ww�wwx�wwww���|��wwwwx�wwx������G�wwx�www�y|���Dwwwx�wwx��w������wwx�www��w�����Gwwx�wwx��w�����Gwwx�www��"������wwx�wwx��",����wwwx�www��""���G�wwx�wwx�""$O�wwwwx�www�"""/�w�wwx�wwxw��"/��wwwwx�wwww����'�w�wwx�wwxw�"""'wwwwwx��������������wx�wwxwwwwwwwwwwwx�wwwwwwwwwww�wwx�wwxwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwxwwwwwwwwwwwx����������������         End Of File <!>
 �STRING WindowName,ObjectName
HANDLE HSpace,HObject
HANDLE local HGroup
STRING local _ObjectName
FLOAT x,y,Width,Height,Attach,zOrder,Show,x0,y0,angle
FLOAT local _x,_y,_Width,_Height,_zOrder,_angle
FLOAT local ret
FLOAT _enable

 if (WindowName != ""&& (~HSpace==#0));  HSpace := GetWindowSpace(~WindowName); endif
 if (~ObjectName != "" &&  _ObjectName != ~ObjectName)
  if (GetObject2dByName(~HSpace, #0,~ObjectName))
   HObject := GetObject2dByName(~HSpace, #0,~ObjectName)
  endif
  if (~HObject)
   HGroup := GetObjectParent2d(~HSpace, ~HObject)
   x := GetObjectOrg2dx(~HSpace, ~HObject)
   y := GetObjectOrg2dy(~HSpace, ~HObject)
   Width := GetObjectWidth2d(~HSpace, ~HObject)
   Height := GetObjectHeight2d(~HSpace, ~HObject)
   _x:=~x; _y:=~y; _Width := ~Width; _Height := ~Height
   zOrder := GetZOrder2d(~HSpace,~HObject)
   _ObjectName := ~ObjectName
   exit()  
  endif
 endif

if (~HObject)

  if (_angle!=~angle)
   if (Float(HGroup)&&~Attach) 
    ret := RotateObject2d(HSpace,HGroup,~x0,~y0,~angle- _angle)
   else  
    ret := RotateObject2d(~HSpace,HObject,~x0,~y0,~angle- _angle)
   endif
   _angle:=~angle
  endif
 
 if (Float(HGroup)&&~Attach) 
  x := ~x - _x + GetObjectOrg2dx(HSpace, HGroup)
  y := ~y - _y + GetObjectOrg2dy(HSpace, HGroup)
  ret := SetObjectOrg2d(HSpace,HGroup,~x,~y)
  x := GetObjectOrg2dx(HSpace, HObject)
  y := GetObjectOrg2dy(HSpace, HObject)
  _x:=~x; _y:=~y
 else
  x := ~x - _x + GetObjectOrg2dx(~HSpace, ~HObject)
  y := ~y - _y + GetObjectOrg2dy(~HSpace, ~HObject)
  ret := SetObjectOrg2d(~HSpace,~HObject,~x,~y)
  _x:=~x; _y:=~y
 endif

  if (_Width != ~Width || _Height != ~Height)
   ret := SetObjectSize2d(~HSpace,~HObject,~Width,~Height)
   _Width := ~Width; _Height := ~Height
  endif

  if (_zOrder !=~zOrder)
   ret := SetZOrder2d(~HSpace,~HObject,~zOrder)
  endif

  zOrder := GetZOrder2d(~HSpace,~HObject);   _zOrder :=~zOrder
  ret := SetShowObject2d(~HSpace,~HObject,~Show)
endif
 @B   i  x   z    �        R + 5  y   �   y  z    � x  y  � + 5 y        y  � o 6        y  �     o y     F      D      E      H      I 	              	       `  y  {      o h    7 5 �     
 + 5 �              ~  3 �              ~          
 + 5 �          D            E           K      D      E          3 $         D            E           K              7    	 7 , 5 D       	 L       	       7 5 T      c      `            l     4g�  �   �   ;	  
 