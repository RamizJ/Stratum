SB0        StratumClass_makeface_35356_41e   _enable     FLOAT    from_x     FLOAT    to_x     FLOAT    from_y     FLOAT    to_y     FLOAT   
 Calculator     STRING    Color   rgb(255,255,255) COLORREF    i     FLOAT�   dx     FLOAT�   dy     FLOAT�   stepy     FLOAT    j     FLOAT�   _y     FLOAT�   sizey     FLOAT    __y     FLOAT�   stepx     FLOAT    _x     FLOAT�   mat     FLOAT    sizex     FLOAT    ret     FLOAT�   _z     FLOAT    matC     FLOAT   
 GFLOAT _enable
FLOAT  from_x,to_x
FLOAT  from_y,to_y

STRING Calculator
COLORREF Color
i:=0
dx:=to_x-from_x
dy:=to_y-from_y
while (~i<(stepy+1))
 j:=0
_y:=~dy*(~i/stepy)+from_y
__y:=~sizey*(~i/~stepy-0.5)

 while (~j<(stepx+1))
  _x:=~dx*(~j/stepx)+from_x

  ret:=Mput(~mat,~i+1,(~j)*3+1,~sizex*(~j/~stepx-0.5),1)
  ret:=Mput(~mat,~i+1,(~j)*3+2,~__y,1)

  SendMessage("..\..\"+Calculator,"","_x","x","_y","y","_z","z","Color","Color")
   ret:=Mput(~mat,~i+1,(~j)*3+3,~_z,1)
   ret:=Mput(~matC,~i+1,~j+1,float(~Color),1)
 j:=~j+1
 endwhile
 i:=~i+1
endwhile
 5 ��������� ����� ��� �������� ����������� ������������ @@   5                          	    
        �? : 5 4            	    
             
         �?               �? : 5 (                        �?          @        �?               �?         �?@              �?          @         @          �?@   z  ..\..\  x  | z    z  _x  z  x z  _y  z  y z  _z  z  z z  Color z  Color X            �?          @        @          �?@              �?          �?           �?@            �?   3 F          �?   3     �f�    
 Y   