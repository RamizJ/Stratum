SB0  �      _RotateObject3d 	  hSpace3d# ���������� ����������� ������������   HANDLE   hObject ���������� ����������� �������   HANDLE   hBase ���������� �������� �������   HANDLE   ax ���������� X ��� ��������   FLOAT   ay ���������� Y ��� ��������   FLOAT   az ���������� Z ��� ��������   FLOAT   angle ���� ��������   FLOAT   matrix     FLOAT�   rez     FLOAT� 
 �function
HANDLE PARAMETER hSpace3d,hObject,hBase
FLOAT  PARAMETER ax,ay,az,angle

if(hSpace3d && hObject)
 matrix:=mcreate(0,1,2,1,3,1)
 
 rez:=Mput(matrix,2,1,Ax,1)
 rez:=Mput(matrix,2,2,Ay,1)
 rez:=Mput(matrix,2,3,Az,1)

 rez:=PushCrdSystem3d(hSpace3d)
 if(hBase)
 rez:=SelectLocalCrd3d(hSpace3d,hbase)
 endif
 rez:=RotateObject3d(hSpace3d,hObject,matrix,angle)
 if(hBase)
 rez:=PopCrdSystem3d(hSpace3d)
 endif
 rez:=MDelete(matrix,1)
endif 
return rez  �������� ����������� ������� <      �        U o �                 �?        @       �?       @       �?< 
            @       �?         �?@ 
            @        @         �?@ 
            @       @         �?@ 
     �
    o z      �
           �
    o �    �
           �?= 
     n��  �  
 t   