SB0  �      _ObjectXdByName   ObjName ��� �������   STRING    hSpace3d# ���������� ����������� ������������   HANDLE    hObject) ���������� ���������� ����������� �������   HANDLE�   _disable ���������� �������   FLOAT�  
 � STRING ObjName
HANDLE hSpace3d,hObject
if(hSpace3d)

if (~hObject)
 _disable:=1
 else
 if(~ObjName=="#World")
 hObject:=handle(-1)
 else
 hObject:= GetObject2dByName(hSpace3d,#0,~ObjName)
 endif
endif

endif L ���������� ���������� ����������� ������� �� �����
� �������� (_disable==1) @    :  1     o 0   o         �?  3 0 y   z  #World  � 5 &        �   3 0        y   �      �N�  �   
 �   