SB0        _MoveObjectPoints3d   hSpace3d# ���������� ����������� ������������   HANDLE   hObject ���������� ����������� �������   HANDLE   x �������� �� �   FLOAT   y �������� �� Y   FLOAT   z �������� �� Z   FLOAT   matrix     FLOAT�   rez     FLOAT� 
 _function
HANDLE PARAMETER hSpace3d,hObject
FLOAT  PARAMETER x,y,z

 if(hSpace3d && hObject)

  matrix:=mcreate(0,1,4,1,4,1)
  rez:=MDiag(matrix,1,1)  
  rez:=Mput(matrix,4,1,x,1)
  rez:=Mput(matrix,4,2,y,1)
  rez:=Mput(matrix,4,3,z,1)
  rez:=TransformObjectPoints3d(hSpace3d,hObject,matrix)
  rez:=MDelete(matrix,1)
 
 endif 
return rez K ������� ���������� ����� ����������� ������� ��
�������� �������� �� X,Y,Z 0      �        U o �                 �?       @       �?       @       �?< 
           �?       �?B 
           @       �?         �?@ 
           @        @         �?@ 
           @       @         �?@ 
         �
           �?= 
     �h�    
 �   