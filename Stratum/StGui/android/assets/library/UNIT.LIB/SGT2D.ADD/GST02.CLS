SB0  �      gst02   x     FLOAT    y     FLOAT    N     FLOAT    min     FLOAT    max     FLOAT    hmax     FLOAT    xc     FLOAT    yc     FLOAT    HSpace     HANDLE    bars     HANDLE    items     HANDLE    _enable     FLOAT    i     FLOAT�   t     FLOAT�   s     FLOAT�   hobj     HANDLE�   yh     FLOAT�   step     FLOAT    xx     FLOAT�   ret     FLOAT�       ����������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������wwwwwwwwwwwww��wwwwwwwwwwwww��wwwwwwwwwwwww��wwwwwwwwwwwww��pwwwpwwwpww��pwwwpwwwpwp��wpppwwwwpwp��wpppwwwwpw��wpppwwwwpp��wpwppwwwwpw��wpwppwwwwpww��wwpwpwpwp��wwpww  ww��wwwwwwwwwwpww��wwwwwwwwwwpww��wwwwwwwwwwpww��wwwwwwwwwwwww��wwwwwwwwwwwww��wwwwwwwwwwwww�� W      K  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
 ��            T@      R@      D@      3@                    � 
 
 ��i                    Arial Cyr �i� �� �zm ��    ��� 
 
 ��j    work� 
 
 ��k   �       ���            End Of File <!>
 FLOAT x,y,N,min,max,hmax,xc,yc
HANDLE HSpace,bars,items
FLOAT _enable

if (~x < ~min || ~ x > ~max || ~y == 0); exit(); endif

// ������� ����� ��������
i := trunc((~x - ~min)/(~max-~min)*~N)
t := vGetF(~items,~i,"")
vSet(~items,~i,"",~t+~y)

// ������� �����
 s := s + ~y

// ��������������
 i := 0
 while (~i < ~N)
  t := vGetF(~items,~i,"")
  hobj := vGetH(~bars,~i,"")

  yh := ~yc - ~t*~hmax/~s
  xx := ~xc+~step*~i

//  ret := SetVectorPoint2d(~HSpace,~hobj,2,~xx+~bwidth,~yh)
//  ret := SetVectorPoint2d(~HSpace,~hobj,3,~xx,~yh)
  ret := SetVectorPoint2d(~HSpace,~hobj,1,~xx,~yh)
  i := ~i + 1
 endwhile
 @B   �        :      8 ,            6 , 5                   P    
   z    �   
   z         �                      : 5 �  
   z    �   	   z    �                                    �?    8           �?   3 M    �f�  �   �     
 