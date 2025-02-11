SB0  _     	 Surface3D   _disableE ��������� ��� ��������� ������ ������ (1 - ���������,  0 - ���������) 1 FLOAT    num ����� ������� � Z-������������ 0 FLOAT    stepx ��� ����� �� X 1 FLOAT    stepy ��� ����� �� Y 1 FLOAT    stepz ��� ����� �� Z 1 FLOAT    i     FLOAT�   j     FLOAT�   k     FLOAT   n     FLOAT   x     FLOAT   y     FLOAT   z     FLOAT�   x1     FLOAT�   y1     FLOAT�   z1     FLOAT�   x2     FLOAT�   y2     FLOAT�   z2     FLOAT�   max     FLOAT�   cr     FLOAT�   cg     FLOAT�   cb     FLOAT�   HSpace3d$ ��������� �� ��������� ������������ #0 HANDLE   	 HObject3d     HANDLE�   HGroup ��������� �� ����������� #0 HANDLE�   _buffer     FLOAT�   _prim     FLOAT�   r     FLOAT�   colour     COLORREF�   color1 ���� ��� ������� ���������� Z rgb(255,0,0) COLORREF    color2" ���� ��� ������������ ���������� Z rgb(0,0,255) COLORREF   
 �FLOAT _disable
FLOAT num
FLOAT stepx,stepy,stepz
FLOAT local i,j,k,n
FLOAT local x,y,z
FLOAT local x1,y1,z1
FLOAT local x2,y2,z2
FLOAT local max
FLOAT local cr,cg,cb
HANDLE HSpace3d
HANDLE local HObject3d
HANDLE HGroup
FLOAT local _buffer, _prim, r
COLORREF local colour
COLORREF color1,color2

if (~HSpace3d == #0)
 exit()
endif

r := MDim(~num,~r,~r,~n,~k)
i := 1
max := 0
while (~i != ~n+1)
 j := 1
 while (~j != ~k+1)
  if (~max<MGet(~num,~i,~j,1))
   max := MGet(~num,~i,~j,1)
  endif
  j := ~j + 1
 endwhile
 i := ~i + 1
endwhile
cr := (GetRValue(~color2)-GetRValue(~color1))/~max
cg := (GetGValue(~color2)-GetGValue(~color1))/~max
cb := (GetBValue(~color2)-GetBValue(~color1))/~max
i := 1
while (~i != ~n+1)
 j := 1
 HObject3d:=CreateObject3d(~HSpace3d)
 while (~j != ~k)
  x1 := ~j*~stepx
  y1 := ~i*~stepy
  z1 := MGet(~num,~i,~j,1)*~stepz
  _buffer:=AddPoint3d(~HSpace3d,~HObject3d,~x1,~y1,~z1)
  x2 := (~j+1)*~stepx
  y2 := ~i*~stepy
  z2 := MGet(~num,~i,~j+1,1)*~stepz
  _buffer:=AddPoint3d(~HSpace3d,~HObject3d,~x2,~y2,~z2)
  z := Max(~z1,~z2)
  if (~z<0)
   z := 0
  endif
  colour := RGB(trunc(~z*~cr/10)+GetRValue(~color1),trunc(~z*~cg/10)+GetGValue(~color1),trunc(~z*~cb/10)+GetBValue(~color1))
  _prim:=AddPrimitive3d(~HSpace3d,~HObject3d,16,~colour,~_buffer-1,~_buffer-2)
  if (~i==1 && ~j==1)
   HGroup := CreateGroup2d(~HSpace3d,~HObject3d)
  else
   r := AddGroupItem2d(~HSpace3d,~HGroup,~HObject3d)
  endif
  j := ~j + 1
 endwhile
 i := ~i + 1
endwhile
j := 1
while (~j != ~k+1)
 i := 1
 HObject3d:=CreateObject3d(~HSpace3d)
 while (~i != ~n)
  x1 := ~j*~stepx
  y1 := ~i*~stepy
  z1 := MGet(~num,~i,~j,1)*~stepz
  _buffer:=AddPoint3d(~HSpace3d,~HObject3d,~x1,~y1,~z1)
  x2 := ~j*~stepx
  y2 := (~i+1)*~stepy
  z2 := MGet(~num,~i+1,~j,1)*~stepz
  _buffer:=AddPoint3d(~HSpace3d,~HObject3d,~x2,~y2,~z2)
  z := Max(~z1,~z2)
  if (~z<0)
   z := 0
  endif
  colour := RGB(trunc(~z*~cr/10)+GetRValue(~color1),trunc(~z*~cg/10)+GetGValue(~color1),trunc(~z*~cb/10)+GetBValue(~color1))
 _prim:=AddPrimitive3d(~HSpace3d,~HObject3d,16,~colour,~_buffer-1,~_buffer-2)
  r := AddGroupItem2d(~HSpace3d,~HGroup,~HObject3d)
  i := ~i + 1
 endwhile
 j := ~j + 1
endwhile
 ������ ������ ����������� �� ������� �� ������� ���������.
���������� ������� �������� ���������� Z.
���������� ����� �� X ������� �������� ������� �� �����������.
���������� ����� �� Y ������� �������� ������� �� ���������.
���������� HGroup �������� ���������� �� ������ ��������, ������� ��
���� �������� ������������.
�� ����������� ����� ������ �������� �����, � ����������� �� ���������� Z.
��� ����� ����� ������� ��� �����, color1 � color2 (���� ��� ������� Z � 
���� ��� ������������ Z).
��� ��������� ������� � ���������� 3dSpace ��������� ���������� �������������� - 
stepx,stepy,stepz. �� ���� ���������� ����������.

������� ����� ����� ����� �� SendMessage("","Surface3D")
 @    k   HARDWARE.DBM �         R 5 	     � � � � `          �?                        �? 7 5 |        �?             �? 7 5 p                �?? : 5 d              �??            �?   3 8          �?   3 $   a  a        b  b        c  c             �?             �? 7 5 �       �?    ~      7 5 �                           �??                �           �?                          �?        �??                �      &              : 5 C                       $@ P   a             $@ P   b             $@ P   c �              0@           �?           @ �            �?6          �?6 + 5 �    o   3 �      n           �?   3 �          �?   3 �        �?             �? 7 5 �       �?    ~      7 5 �                           �??                �                  �?                 �?          �??                �      &              : 5 v                       $@ P   a             $@ P   b             $@ P   c �              0@           �?           @ �         n           �?   3 �         �?   3 �   怉  �  
 �   