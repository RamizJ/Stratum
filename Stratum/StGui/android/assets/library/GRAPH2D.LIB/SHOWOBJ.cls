SB0  �     
 ShowObject  
 WindowName ��� ���� Main Window STRING   
 ObjectName ��� �������   STRING    _ObjectName     STRING   HSpace$ ���������� ������������ ������������   HANDLE�   HObject ���������� ������������ �������   HANDLE�   _HObject     HANDLE�   x
 ����������   FLOAT    y
 ����������   FLOAT    Motion( ����,  ����������� ������������ �������    FLOAT    _enable" ����,  ����������� ������� ������� 1 FLOAT    ret     FLOAT�       �����������������wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwpwwwwwwwx�wpwwwwpwwwwwwx�wwwwwpwwwwpwwx�wwp�������wwwx�wwx�������wwwwx�wwx�      wwwx�wwx�������wwwx�wwx�������wwwx�wwx�������wwwx�wwx�������wwwx�wwx�������wwwx�p �������p x�wwx�������wwwx�wwx�������wwwx�wwx�������wwwx�wwx�������wwwx�wwwp������wwwx�wwwp      wwwx�wwwwwwwwwwwwwwx�wwpwwwpwwwwwwx�wwwwwpwwwwpwwx�wpwwwwpwwwwwwx�wwwwwwpwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx���������������� -      !  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
 ��   �       Hs@     Pc@      @@      @@                      @@      @@    � 
 
 ��h   BMv      v   (                                         �  �   �� �   � � ��  ��� ���   �  �   �� �   � � ��  ��� �����������������wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwpwwwwwwwx�wpwwwwpwwwwwwx�wwwwwpwwwwpwwx�wwp�������wwwx�wwx�������wwwwx�wwx�      wwwx�wwx�������wwwx�wwx�������wwwx�wwx�������wwwx�wwx�������wwwx�wwx�������wwwx�p �������p x�wwx�������wwwx�wwx�������wwwx�wwx�������wwwx�wwx�������wwwx�wwwp������wwwx�wwwp      wwwx�wwwwwwwwwwwwwwx�wwpwwwpwwwwwwx�wwwwwpwwwwpwwx�wpwwwwpwwwwwwx�wwwwwwpwwwwwwwx�wwwwwwwwwwwwwwx�wwwwwwwwwwwwwwx����������������BM�       >   (                 �                      ���                                                                                                                                          End Of File <!>
 =STRING WindowName,ObjectName
STRING local _ObjectName
HANDLE HSpace,HObject
HANDLE local _HObject
FLOAT x,y,Motion
FLOAT _enable
FLOAT local ret

 if (WindowName != ""&& (~HSpace==#0));  HSpace := GetWindowSpace(~WindowName); endif
 if (~ObjectName != "" &&  _ObjectName != ~ObjectName)
  HObject := GetObject2dByName(~HSpace, #0,~ObjectName)
  if (_HObject != ~HObject)
   if (~Motion)
    ret := SetObjectOrg2d(~HSpace,~HObject,~x,~y)
   endif
   HideObject2d(~HSpace,_HObject)
   ShowObject2d(~HSpace,~HObject)
   _HObject := ~HObject
  endif
 endif
 � ����� ��������� ���������� ������������� ������������ �������.
�������� ��������� ��� ���������� show:  0 - ��������, 1 -��������. @B   Q   x   z    �        R + 5  y   �   y  z    � x  y  � + 5 P        y  �       S 5 P   5 B         K 
     �           �f�  �   �     
 W
   