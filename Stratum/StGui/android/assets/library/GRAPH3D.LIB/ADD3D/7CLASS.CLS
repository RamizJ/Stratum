SB0  d      StratumClass_histogramm3d_35357   _enable     FLOAT    from_x     FLOAT    to_x     FLOAT    from_y     FLOAT    to_y     FLOAT   
 Calculator     STRING    Color   rgb(255,255,255) COLORREF    i     FLOAT�   sizex     FLOAT    stepx     FLOAT    _sx     FLOAT�   sizey     FLOAT    stepy     FLOAT    _sy     FLOAT�   dx     FLOAT�   dy     FLOAT�   j     FLOAT�   _y     FLOAT�   __y     FLOAT�   _x     FLOAT�   mat     FLOAT    _z     FLOAT    ret     FLOAT�   matC     FLOAT   
 FLOAT _enable
FLOAT  from_x,to_x
FLOAT  from_y,to_y
STRING Calculator
COLORREF Color
i:=0
_sx:=sizex/stepx
_sy:=sizey/stepy

dx:=to_x-from_x
dy:=to_y-from_y

while (~i<(stepy))
 j:=0
_y:=~dy*(~i/stepy)+from_y
__y:=~sizey*(~i/~stepy-0.5)

//LogMessage(string(~_y))
 while (~j<(stepx))
   _x:=~dx*(~j/stepx)+from_x
SendMessage("..\..\"+Calculator,"","_x","x","_y","y","_z","z","Color","Color")
   ret:=Mput(~mat,~i+1,~j+1,~_z,1)
   ret:=Mput(~matC,~i+1,~j+1,FLOAT(~Color),1)
   j:=~j+1
 endwhile
 i:=~i+1
endwhile
 5 ��������� ����� ��� �������� ����������� ������������ @@   �                 	   
                          : 5 �                                       �?       	 : 5 �      	        z  ..\..\  x  | z    z  _x  z  x z  _y  z  y z  _z  z  z z  Color z  Color X            �?          �?          �?@              �?          �?           �?@            �?   3 N          �?   3 #    �m�  6  
 U   