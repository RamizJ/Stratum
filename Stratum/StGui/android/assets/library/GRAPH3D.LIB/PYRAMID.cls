SB0  #     	 Pyramid3D 
  HSpace3d# ���������� ����������� ������������   HANDLE   	 hObject3d) ���������� ���������� ����������� �������   HANDLE�   Color ���� rgb(255,255,0) COLORREF    A ������ ����� ������� 100 FLOAT    L ������ 100 FLOAT    FlagsW �����: 1 - ����������� � ����� ( X,Y,Z ); 8 - ����������� ������� ����� � ��� ��������� 9 FLOAT    X � ���������� ������   FLOAT    Y Y ���������� ������   FLOAT    Z Z ���������� ������   FLOAT    _enable ���������� ������� 1 FLOAT�       ���������������������������������������������������w�������������󻻻w����������󻻻�3�w���������;���333;�������;���;33333������;���;;33337�����3����33333�����3����;;33;������3������337������3;�����33�������3;�����;3������3;������;�������33������7��������3�������������s3���������������3;��������������3;��������������s;��������������3���������������3���������������s�������������������������������;���������������{���������������������������������������������������������������������������������������� h	      \	  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
              g@      g@      @@      @@                      @@      @@    � 
 
 ��g   BM6      6  (                               ��� ��� �� �� �� �� ��� ��� ��� ��� ��� $�� v�� +�� ��� ��� ��� Z�� ��� 3�� R�� c�� ��� e�� {�� ��� G�� �� "�� �� �� ��� H�� 7�� /�� ��� _�� C�� O�� ��� w�� ��� ��� ��� /�� [�� ��� ��� 7��                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 '                             
0$/                         "                   
&              &)	         '        '/                 )                   "
          .                                     
                                                '0	                                     	                    )                      (                       
"                        /+                          %	                          '                            #                             -                              	                                                         �    ( HSpace3d,*HSpace3d;hObject3d,*HObject3d    End Of File <!>
 �HANDLE HSpace3d,hObject3d
COLORREF Color  

if(~HSpace3d)
 hObject3d := MakeCylinder3d(~HSpace3d,#0,Color,sin(PI/3)*2*A/3,0,L,4,0)
 if(Flags&1); _enable:=SetObjectBase3d(~HSpace3d,~hObject3d,X,Y,Z);endif  
 _enable:=SetObjectName2d(HSpace3d,~hObject3d,"Pyramid_"+string(float(~hObject3d))) 
 _enable:=SetObjectAttribute2d(~hSpace3d,~hObject3d,128,1)
 if(Flags&8);_enable:= _MoveObjectPoints3d(~hSpace3d,~hObject3d,0,0,L/2);endif
 _enable:=0
endif
 F �������� ���������� �������� � ��������� ������� � �������� ���������. @B   �      o �            ��DT�!	@       @          @           @                   @         � MakeCylinder3d             �?i 5 Z            � 	      z  Pyramid_     � | { 	             `@       �?f 	           @i 5 �                                  @ �
 _MoveObjectPoints3d  ������������ 	           	    n}�        �  
 O   