SB0        _SetObjectBase3d   hSpace3d# ���������� ����������� ������������   HANDLE   hObject ���������� ����������� �������   HANDLE   hBase ���������� �������� �������   HANDLE   x ���������� X   FLOAT   y ���������� Y   FLOAT   z ���������� Z   FLOAT   rez     FLOAT� 
 Dfunction
HANDLE PARAMETER hSpace3d,hObject,hBase
FLOAT  PARAMETER x,y,z

if(hSpace3d && hObject)
 rez:=PushCrdSystem3d(hSpace3d)
 
 if(hBase)
  rez:=SelectLocalCrd3d(hSpace3d,hbase)
 endif

 rez:=SetObjectBase3d(hSpace3d,hObject,x,y,z)
 
 if(hBase)
  rez:=PopCrdSystem3d(hSpace3d)
 endif

endif 
return rez # ������� ���������� ��������� ������ ,      .        U o -    �
    o       �
             �
    o -    �
     ���  2  
 z   