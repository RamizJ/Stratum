SB0  �	     
 MouseState 	 
 WindowName ��� ���� Main Window STRING    HSpace$ ���������� ������������ ������������   HANDLE�   HObject     HANDLE   fwKeys ��� ������� ������   FLOAT    xPos x - ���������� ����   FLOAT    yPos y - ���������� ����   FLOAT    msg     FLOAT   init     FLOAT�   _enable! ����  ����������� ������� ������� 1 FLOAT�       �����������������wwwwwww�����wwx�wwwwww�������wx�wwwwwx�   ��wx�wwwww� ��wp���x�wwwwx����ww��x�wwwwx���wwp��x�wwww����www�x�wwwx���w���x�wwwp����w���wx�www���w���wx�wwp���ww����wx�ww��ww�����wx�ww� w�����wx�ww p�������wwx�ww wp�������wwx�ww������wwx�ww������ wwwx�ww����� wwwwx�wpp �  wwwwwx�wp ��� wwwwwwx�ww   wwwwwwwx�w�wwwwwwwwwwwx�w�wwwwwwwwwwwx�wxwwwwwwwwwwwx�wpw�����wwwwwwx�wpwww   �wwwwwx�ww   wwwwwwwwx�wwwwwwwwpwwwwwx�wwwwwwwwpwwwwwx�wwwwwwwwwwwwwx���������������� -      !  2D^   �                         Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
  � 
 
 ��   �       Hs@     Pc@      @@      @@                      @@      @@    � 
 
 ��h   BMv      v   (                                         �  �   �� �   � � ��  ��� ���   �  �   �� �   � � ��  ��� �����������������wwwwwww�����wwx�wwwwww�������wx�wwwwwx�   ��wx�wwwww� ��wp���x�wwwwx����ww��x�wwwwx���wwp��x�wwww����www�x�wwwx���w���x�wwwp����w���wx�www���w���wx�wwp���ww����wx�ww��ww�����wx�ww� w�����wx�ww p�������wwx�ww wp�������wwx�ww������wwx�ww������ wwwx�ww����� wwwwx�wpp �  wwwwwx�wp ��� wwwwwwx�ww   wwwwwwwx�w�wwwwwwwwwwwx�w�wwwwwwwwwwwx�wxwwwwwwwwwwwx�wpw�����wwwwwwx�wpwww   �wwwwwx�ww   wwwwwwwwx�wwwwwwwwpwwwwwx�wwwwwwwwpwwwwwx�wwwwwwwwwwwwwx����������������BM�       >   (                 �                      ���                                                                                                                                     �}   End Of File <!>
 FSTRING WindowName
HANDLE HSpace
HANDLE local HObject
FLOAT fwKeys,xPos,yPos
FLOAT local msg,init

if (init == 0)
 if (WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(WindowName); endif

 if (~HSpace)
  RegisterObject(~HSpace,~HObject,"",WM_ALLMOUSEMESSAGE,0)
  _enable := 0
  init:=1
 endif
endif
 , ��������� ��������� � ��������� ������ ����� @�   C              6 5 B x   z    �        R + 5  x   �     o B     z           �@         �                   �?     F��  x   �   �  
 �   