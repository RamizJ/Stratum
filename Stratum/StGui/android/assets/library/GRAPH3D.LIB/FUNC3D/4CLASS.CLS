SB0  &      _ResizeObject3d   hSpace3d# ���������� ����������� ������������   HANDLE   hObject ���������� ����������� �������   HANDLE   hBase ���������� �������� �������   HANDLE   ax ��������������� �� ��� X   FLOAT   ay ��������������� �� ��� Y   FLOAT   az ��������������� �� ��� Z   FLOAT   rez     FLOAT�  matrix     FLOAT�  
 �function
HANDLE PARAMETER hSpace3d,hObject,hBase
FLOAT  PARAMETER ax,ay,az

if(hSpace3d && hObject)
 rez:=PushCrdSystem3d(hSpace3d)
  if(hBase)
    rez:=SelectLocalCrd3d(hSpace3d,hbase)
  endif

 matrix:=mcreate(0,1,4,1,4,1)
 rez:=Mput(~matrix,4,4,1,1)

 rez:=Mput(~matrix,1,1,ax,1)
 rez:=Mput(~matrix,2,2,ay,1)
 rez:=Mput(~matrix,3,3,az,1)

 rez:=TransformObject3d(hSpace3d,hObject,matrix)

 if(hBase)
   rez:=PopCrdSystem3d(hSpace3d)
 endif
 rez:=MDelete(matrix,1)
 
endif 
return rez 8 ������� ������������ ��������������� ����������� ������� 4      �        U o �    �
    o       �
                  �?       @       �?       @       �?< 
           @       @       �?       �?@ 
           �?       �?         �?@ 
            @        @         �?@ 
           @       @         �?@ 
         �
    o �    �
           �?= 
     `o�  o  
 r   