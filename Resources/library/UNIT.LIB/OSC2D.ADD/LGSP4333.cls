SB0  �     	 LGSpaceEx   HSpace ���������� ������������   HANDLE�   HObject ���������� ������������ �������   HANDLE�  
 WindowName ��� ����   STRING�  
 ObjectName     STRING    WindowNameEx ��� ������������� ����   STRING    Flags ����� ��� �������� ����   STRING   	 ClassName     STRING�   ret     FLOAT�   Height     FLOAT�   Width     FLOAT�   x& ��������� ������� ���� � ������������    FLOAT�   y& ��������� ������� ���� � ������������    FLOAT�   xv     FLOAT    yv     FLOAT    _enable   1 FLOAT�   x0     FLOAT�   y0     FLOAT�       ����������������������������������������������������������������������������������������������������������������� ���������������� ������������w�� ����������www�� �������� www�� ������wp www�� ����w�wp www�� �wwwp www�����w��wp www���x��w�wwwp wp�x�wwy�w��w�w�x��www�w��wwp�wx�w��z��w�w�wx�www��ww�wp�wwx� wwww��w�w�wwx��  wwww�wp�wwwx����  wwww�wwwx������  wp�wwwwx�����ww �wwwwx� ���wwww�wwwwwx��  p w�wwwwwx�w��wwww�wwwwwwx�www��w�wwwwwwx�wwwww��wwwwwwwx���������������� -      !  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
 ��   �       Hs@     Pc@      @@      @@                      @@      @@    � 
 
 ��h   BMv      v   (                                         �  �   �� �   � � ��  ��� ���   �  �   �� �   � � ��  ��� ����������������������������������������������������������������������������������������������������������������� ���������������� ������������w�� ����������www�� �������� www�� ������wp www�� ����w�wp www�� �wwwp www�����w��wp www���x��w�wwwp wp�x�wwy�w��w�w�x��www�w��wwp�wx�w��z��w�w�wx�www��ww�wp�wwx� wwww��w�w�wwx��  wwww�wp�wwwx����  wwww�wwwx������  wp�wwwwx�����ww �wwwwx� ���wwww�wwwwwx��  p w�wwwwwx�w��wwww�wwwwwwx�www��w�wwwwwwx�wwwww��wwwwwwwx����������������BM�       >   (                 �                      ���                                                                                                                                     B   End Of File <!>
 �HANDLE HSpace,HObject
STRING WindowName,ObjectName,WindowNameEx
STRING Flags
STRING ClassName
FLOAT local ret
FLOAT Height,Width
FLOAT x,y,xv,yv,_enable

 ClassName := GetClassName("..")

 if(~WindowNameEx == "")
  exit()
 endif

 if(~WindowName == "")
  WindowName := String(rnd(100))
 endif

 HSpace := GetWindowSpace(~WindowNameEx)
 if (~HObject == #0); HObject := GetObject2dByName(~HSpace, #0,~ObjectName); endif

 if (~x==0&&~y==0)
  x0 := GetSpaceOrg2dx(~HSpace)
  y0 := GetSpaceOrg2dy(~HSpace)

  x := GetObjectOrg2dx(~HSpace, ~HObject)
  y := GetObjectOrg2dy(~HSpace, ~HObject)
 endif

 Width := GetObjectWidth2d(~HSpace, ~HObject)
 Height:= GetObjectHeight2d(~HSpace,~HObject)
 HSpace:=CreateWindowEx(~WindowName,~WindowNameEx,~ClassName,~x,~y,~Width,~Height,"WS_CHILD|WS_BYSPACE"+~Flags)

 if (~HObject == #0); HObject := GetObject2dByName(~HSpace, #0,~ObjectName); endif

 x := GetObjectOrg2dx(~HSpace, ~HObject)
 y := GetObjectOrg2dy(~HSpace, ~HObject)
 Width := GetObjectWidth2d(~HSpace, ~HObject)
 Height:= GetObjectHeight2d(~HSpace,~HObject)

 ret := SetClientSize(~WindowName,~Width,~Height)
 ret := SetSpaceOrg2d(~HSpace,~x,~y)

if (HSpace)
 _enable := 0
endif
 � ����� ������� ��������� ����������� ������������ � ������������
������� ������������ ������� (�� �������� �������� �������������
��������������� ������� �������), � ���������� � ��� ����������
� ������ ����. @�   �   z  ..  �{  y  z    � 5    y  z    � 5 !        Y@* � {  y  �           R 5 8         y  �    
          6            6 + 5 c    V     U       D 
      E       H 	      I  y  y  y   
    	   z 
 WS_CHILD|WS_BYSPACE y  | �          R 5 �         y  �        D 
      E       H 	      I  y   	   �       
   W     o �               L��  Q   Y   �  
 U   