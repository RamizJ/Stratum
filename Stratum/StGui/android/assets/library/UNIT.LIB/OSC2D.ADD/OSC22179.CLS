SB0  �      OSC2d   
 WindowName     STRING�   HSpace     HANDLE    HObject     HANDLE�   HGroup     HANDLE�   HPen     HANDLE�   ret     FLOAT�   xx     FLOAT�   yy     FLOAT�   regen   1 FLOAT�   _x     FLOAT�   _y     FLOAT�   Width     FLOAT�   Height     FLOAT�   init   1 FLOAT�   counter     FLOAT�   x     FLOAT    y     FLOAT    buffer   4192 FLOAT    ScaleX     FLOAT    ScaleY     FLOAT    Control     FLOAT    xofs     FLOAT    yofs     FLOAT�   Color     COLORREF   
 _ClassName     STRING   PrintX     FLOAT    PrintY     FLOAT    Print     FLOAT�   FontSize   8 FLOAT    Reset     FLOAT�   clr     COLORREF�   str     STRING�       �              ���������������wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�w��������������x            ��x        ��x"""""""""" ��x        ��x        ��x"""""""""" ��x ��   ��  ��x  /  ��x""�""�""�" ��x  � � � ��x  /  / ��x"""��""""����x        ��x        ��x"""""""""" ��x        ��x        ��x            ��w��������������wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�����������������               -      !  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
 ��   �        @@      8@      @@      @@                      @@      @@    � 
 
 ��h   BMv      v   (                                         �  �   �� �   � � ��  ��� ���   �  �   �� �   � � ��  ���                 ���������������wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�w��������������x            ��x        ��x"""""""""" ��x        ��x        ��x"""""""""" ��x ��   ��  ��x  /  ��x""�""�""�" ��x  � � � ��x  /  / ��x"""��""""����x        ��x        ��x"""""""""" ��x        ��x        ��x            ��w��������������wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww�wwwwwwwwwwwwww����������������                BM�       >   (                 �                      ��� �                                                                                                                          �          End Of File <!>
 �STRING WindowName
HANDLE HSpace,HObject,HGroup
HANDLE local HPen
FLOAT ret,xx,yy,regen,_x,_y,Width,Height,init,counter
FLOAT x,y,buffer,ScaleX,ScaleY,Control,counter,xofs,yofs
COLORREF Color
STRING local _ClassName
FLOAT x,y
FLOAT PrintX,PrintY,Print,FontSize

 if (Reset)
  if(Reset == 1) 
   ret := DeleteObject2d(HSpace,HObject)
  endif
  HObject := #0
  Reset := 0
 endif

if (~regen && Float(~HSpace))
 HGroup := CreateGroup2d(~HSpace)  
 clr := RGBeX(255,255,255,1)
 if (~init)
  WindowName := GetWindowName(~HSpace)
  Width := GetClientWidth(~WindowName)
  Height := GetClientHeight(~WindowName)
  yofs := GetSpaceOrg2dY(~HSpace) + ~Height - ~yofs
  init := 0
  regen := 0
 else
  regen := 0
 endif

 xx :=~x*~ScaleX + ~xofs
 yy :=~yofs - ~y*~ScaleY

else

 xx := ~x*~ScaleX + ~xofs
 yy := ~yofs - ~y*~ScaleY
 
 if (_x == ~xx && _y == ~yy)
  exit()
 endif

endif
 Print := PrintX + PrintY
 str := ""
 if(~PrintX)
  str := "x: " + String(~x) + chr(10)
 endif
 if(~PrintY)
  str := ~str + "y: " + String(~y)
 endif
_x := ~xx; _y := ~yy

 while (~counter > ~buffer)
  ret := DelPoint2d(HSpace,HObject,0)
  counter := ~counter - 1
 endwhile

 if(~HObject)
  counter := ~counter + 1
  ret := AddPoint2d(HSpace,HObject,-1,~xx,~yy)
 else
  counter := 1
  HPen:=CreatePen2d(~HSpace,0,0,~Color,13)
  HObject := CreateLine2d(~HSpace,~HPen,#0,~xx,~yy)
  ret := AddGroupItem2d(~HSpace,~HGroup,~HObject)  
 endif
  �   W  a    5 !          �?6 5      C                         + 5 �   o          �o@      �o@      �o@       �?�     5 t   � {   y   �   y   �     U                              3 {                                3 �                      	   6  
   6 + 5 �          z    {    5 � z  x:    � |        $@� | {    5 � y  z  y:  |   � | {     	    
     8 5              .           �?   3 �   o ,         �?              �    -  3 `       �?                               *@9               ,        n     �|  �   �   	  
 