SB0  
      osc02    HSpace     HANDLE    HObject     HANDLE    HObject2     HANDLE    HObject3     HANDLE    x     FLOAT    y     FLOAT    x0     FLOAT    y0     FLOAT    xv     FLOAT�   yx     FLOAT    h     FLOAT    type   1 FLOAT    _enable   1 FLOAT    x2     FLOAT�   y2     FLOAT�   sw     FLOAT�   first   1 FLOAT    s1     FLOAT    s2     FLOAT    s3     FLOAT    xofs     FLOAT    ScaleX     FLOAT    yofs     FLOAT    ScaleY     FLOAT    str     STRING�   yv     FLOAT�   w     FLOAT    ButtonUp     FLOAT�  
 ButtonDown   1 FLOAT�   pressed     FLOAT    ret     FLOAT�   en     FLOAT�       ��������������������������������������������������������������������������������������������������������������������������������                wwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwpwwwwwwwwwwwwwpwwwwwwwwwwwwwpww   wpp wpwwwwwwwwwpwpwwwwwwwwwpwpwpwwww pp wpwwwwwwwwwpwpwwwwwwwwwpwpww  pwpppwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwp                       
  2D^   0                        Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
   � 
 
 ��            H@      @@��\�(V@F]t�E8@         H@      @@Fq�c�`@      @@Fq�c�`@��.��K@      H@��.��K@      H@      @@             L@      @@      R@      3@                    � 
 
  e           � 
 
  f   ���        � 
 
 ��i   ��      �   �"Arial   ���!w/�! w  ��Ȯ�! � 
 
 ��j   
 ����������� 
 
 ��k   �          �          End Of File <!>
 HANDLE HSpace,HObject,HObject2,HObject3
FLOAT x,y,x0,y0,xv,yx,h,type,_enable,x2,y2
FLOAT sw,first,s1,s2,s3

str := "x: "+String((x-xofs)/~ScaleX) +"   y: "+ String((yofs-y)/~ScaleY)

xv := ~x0 + 10
yv := ~y0 + 30
x2 := x0 + xofs + w
y2 := y0 + yofs + h

if (~ButtonUp || ~ButtonDown)
 ret := SetShowObject2d(~HSpace,~HObject,~pressed)
 ret := SetShowObject2d(~HSpace,~HObject2,~pressed)
 ret := SetShowObject2d(~HSpace,~HObject3,~pressed)

 ButtonUp := 0
 ButtonDown := 0
 sw := ~pressed
endif

en := ~sw  ��������� ����� @�   �   z  x:          � | z     y:   |         � | {           $@            >@                   
        , 5 }        l         l         l                                   a�|  �   �   �  
 �   