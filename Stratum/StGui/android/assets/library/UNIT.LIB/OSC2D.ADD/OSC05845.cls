SB0  X      osc01 
  _HObject     HANDLE   
 WindowName     STRING    HSpace     HANDLE�   HObject     HANDLE   fwKeys     FLOAT    xPos     FLOAT    yPos     FLOAT    msg     FLOAT   init     FLOAT�   _enable     FLOAT�       ����������������������������������������������������������������������������������������������������������������                wwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwpwwwwwwwwwwwwwwwp���������������� �      �  2D^   c                        Y@      Y@      Y@      Y@ C:\SC3\data\icons\        ����  � 
 
   � 
 
 ��            @@      Z@��j� �s@      9@         @@      Z@��j� pu@      Z@��j� pu@      `@      @@      `@      @@      Z@             @@      Z@V��ne�s@      9@    V��nepu@      Z@V��nepu@      Z@      @@      Z@      @@      `@  � 
 
  e            e   ���      � 
 
 ��f   ���            B   End Of File <!>
 �HANDLE _HObject
STRING WindowName
HANDLE HSpace
HANDLE local HObject
FLOAT fwKeys,xPos,yPos
FLOAT local msg,init

if (init == 0)
 if (WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(WindowName); endif

 if (~HSpace)
  RegisterObject(~HSpace,#0,"",WM_ALLMOUSEMESSAGE,256)
  _enable := 0
  init:=1
 endif
endif

if (msg)
if (msg==WM_LBUTTONDOWN)
  SendMessage("..","")
endif
endif  ��������� ����� @�   [              6 5 C x  z    �        R + 5  x  �     o C        z           �@       p@�           	        �?    5 Z         �@6 5 Z z  ..  z    X     �`�  �       �  
 {   