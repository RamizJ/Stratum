SB0  �      DoublePicture  
 WindowName ��� ���� Main Window STRING   
 ObjectName ��� �������   STRING    HSpace$ ���������� ������������ ������������   HANDLE�   HObject ���������� ������������ �������   HANDLE�   FileName ��� ������������ �����   STRING    _ObjectName     STRING�   HDib     HANDLE�   x ���������� ������   FLOAT�   y ���������� ������   FLOAT�   xv ���������� ������� �������   FLOAT    yv ���������� ������� �������   FLOAT    Width ������� ������� �������   FLOAT�   Height ������� ������� �������   FLOAT�   zOrder ����� Z - �������   FLOAT�   Show ����  ��������� 1 FLOAT    _x     FLOAT�   _y     FLOAT�   _xv     FLOAT�   _yv     FLOAT�   _Width     FLOAT�   _Height     FLOAT�   _zOrder     FLOAT�   ret     FLOAT�   _enable! ����  ����������� ������� ������� 1 FLOAT        �����������������wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wxwwwwwwwwww�wx�wwwwwwwwwwwwwwx�wxwwwwwwwwww�wx�wwwwwwwwwwwwwwx�xwwx�8�� ww���wwww���0� �wwx�ww�w�����1wwx�w�x�xx�����wx�wwwx����� �wwx�www�L����wx�ww������@�� �wx�wx��@ �� 7wx�wx��� L�wx�wx�wL� D  �wx�wx0�    wx�wx1�0      �wx�wwxw�     �wx�www��sw�   wx�wwww�   ��wx�wwwww�����wx�xwwwww����������wwwwwwwwwwwwwwx�wxwwwwwwwwww�wx�wwwwwwwwwwwwwwx�wxwwwwwwwwww�wx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx���������������� /      #  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
 ��            h@      L@      @@      @@                      @@      @@    � 
 
 ��g   BM6      6   (                    �  �          ������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������ff�www3ffUUUUUU3ffDDD333""" 3333ff����̙��������������������������������������������������ff�ff�fff33fff�DDD3ff33333f33fDDD333""""""ff������������������������������������������������̙�̙��f��ff�fffUUU3ffff�ff�UUU3ff33fUUU3ff33ff�������������������������������������������������̙��UUUf�̙�̙�������̙��ff�UUUUUUff�ff�f��ff�UUU���������������������������������������������ff�f�����������wwwf33   f33������wwwDDD"""333ff�f�������������������������������������������̙�̙�̙��������"""f33�  �  �  fff������fff""""  ff�������������������������������������������f�����������ffff33�  �  �  �  f33www������DDDff�������������������������������f�����f��������������"  �  f33"     """�  f33333������DDD3333ff������������������������������ff�33333ffff���������"  �  DDD      f33�  3  ���fff"""33f33f������������������������������wwwf��UUU  "DDD������f33�  wwwDDD   "  f33f33"""UUU 33333"""UUU������������������������������ff�333���3ff  "   DDD"""DDD���333"  """""""""333  3"""""" 3333f33f���������������������������������3ff33fwwwwww3ff  "  "333""""""""""""""""""333DDDDDD��������������������������������̙��UUU33f��������̙��"""  "  """"""""""""" 33"""""""""33333fDDD��������������������������������̙�̙��ff� 33DDD������3f���̙��ff�33f 33"""333 3333f33333f��������������������������������������̙�̙��f��33f"""333333333""""""33f"""33f33333fDDDDDD33fUUU��������������������������������������������̙�̙��f��f��ff�fffff�3ffDDDUUUDDDDDD33fDDD33f3ff33f��������������������������������̙�������̙����������̙�̙��f��wwwff�fff3ffUUU3ffDDD33fUUU33fUUU���������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������         End Of File <!>
 �STRING WindowName,ObjectName
HANDLE HSpace,HObject
STRING FileName
STRING local _ObjectName
HANDLE local HDib
FLOAT x,y,xv,yv,Width,Height,zOrder,Show
FLOAT local _x,_y,_xv,_yv,_Width,_Height,_zOrder
FLOAT local ret
FLOAT _enable

 if (WindowName != ""&& (~HSpace==#0));  HSpace := GetWindowSpace(~WindowName); endif
 if (~ObjectName != "" &&  _ObjectName != ~ObjectName)
  if (GetObject2dByName(~HSpace, #0,~ObjectName))
   HObject := GetObject2dByName(~HSpace, #0,~ObjectName)
  endif
  if (~HObject)
   x := GetObjectOrg2dx(~HSpace, ~HObject)
   y := GetObjectOrg2dy(~HSpace, ~HObject)
   Width := GetObjectWidth2d(~HSpace, ~HObject)
   Height := GetObjectHeight2d(~HSpace, ~HObject)
   _x:=~x; _y:=~y;    _Width := ~Width; _Height := ~Height
   zOrder := GetZOrder2d(~HSpace,~HObject)
   _ObjectName := ~ObjectName
   exit()
  endif
 endif

if (not(~HObject))
  if (~FileName == ""); exit(); endif
  HDib := CreateDoubleDib2D(~HSpace,~FileName)
  HObject := CreateDoubleBitmap2D(~HSpace,~HDib,~x,~y)
  if (~Width * ~Height)
   ret:=SetBitmapSrcRect2d(~HSpace,~HDib, ~xv,~yv,~Width,~Height)
   ret := SetObjectSize2d(~HSpace,~HObject,~Width,~Height)
  endif
   zOrder := GetZOrder2d(~HSpace,~HObject)
   _x:=~x; _y:=~y;    _zOrder :=~zOrder
else
  x := ~x - _x + GetObjectOrg2dx(~HSpace,~HObject)
  y := ~y - _y + GetObjectOrg2dy(~HSpace,~HObject)
  ret := SetObjectOrg2d(~HSpace,~HObject,~x,~y)
  _x:=~x; _y:=~y

  if (_xv != ~xv || _yv != ~yv || _Width != ~Width || _Height != ~Height)
   ret:=SetBitmapSrcRect2d(~HSpace,~HObject, ~xv,~yv,~Width,~Height)
   ret := SetObjectSize2d(~HSpace,~HObject,~Width,~Height)
   _xv := ~xv; _yv := ~yv; _Width := ~Width; _Height := ~Height
  endif

  if (_zOrder !=~zOrder)
   ret := SetZOrder2d(~HSpace,~HObject,~zOrder)
  endif

  zOrder := GetZOrder2d(~HSpace,~HObject);   _zOrder :=~zOrder
  ret := SetShowObject2d(~HSpace,~HObject,~Show)
  if  (FileName != ~FileName);  ret :=  DeleteObject2d(~HSpace,~HObject);  endif
endif
 @B   k  x   z    �        R + 5  y   �   y  z    � x  y  � + 5 r        y  � o 6        y  �     o r     D      E      H      I                      `  y  {      W 5 � y  z    � 5 �     y  P          T       5 �      	  
     e          L      `              3 j         D            E           K             	 7    
 7 ,     7 ,     7 , 5 8     	  
     e          L   	    
               7 5 H      c      `            l  x  y  � 5 j    C     z�  s   {   �  
 