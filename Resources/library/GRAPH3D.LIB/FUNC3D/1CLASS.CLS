SB0  &      _RotateObjectPoints3d   hSpace3d# ���������� ����������� ������������   HANDLE   hObject ���������� ����������� �������   HANDLE   ax ���������� X ��� ��������   FLOAT   ay ���������� Y ��� ��������   FLOAT   az ���������� Z ��� ��������   FLOAT   angle ���� ��������   FLOAT   matrix     FLOAT�   rez     FLOAT� 
 Rfunction
HANDLE PARAMETER hSpace3d,hObject
FLOAT  PARAMETER ax,ay,az,angle

 if(hSpace3d && hObject)

  matrix:=mcreate(0,1,2,1,3,1)
  rez:=Mput(matrix,2,1,Ax,1)
  rez:=Mput(matrix,2,2,Ay,1)
  rez:=Mput(matrix,2,3,Az,1)
  rez:=RotateObjectPoints3d(hSpace3d,hObject,matrix,angle)
  rez:=MDelete(matrix,1)
 
 endif 
return rez K ������� ������� ����� ����������� �������,
�� ���������� ��������� ������� 8      �        U o                  �?        @       �?       @       �?< 
            @       �?         �?@ 
            @        @         �?@ 
            @       @         �?@ 
           �
           �?= 
     R~�  i  
 �   