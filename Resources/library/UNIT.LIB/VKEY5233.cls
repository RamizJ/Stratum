SB0  >     
 VKeysState  
 WindowName ��� ���� Main Window STRING    HSpace$ ���������� ������������ ������������   HANDLE�   msg     FLOAT�   Repeat     FLOAT   _enable   1 FLOAT�   wVkey ��� ��������� ������� �������   FLOAT�   ScanCode2 ���� ��� ���������� �� ������� �� ����� � ��������   FLOAT�       ����� ����������ww��� wwwwwwwwx�wx���wwwwwwwx�w����� wwwwwwwx�x���wwwwwwx�������� wwwwwwx����wwwwwx��������� wwwwwx�����wwwwx���������� wwwwx��	���wwwx���������� wwwx������wwx����������� wwx�p��	���wx�w���������� wx�wp��	����x�ww���������� x�wwp�	�������www���������� �wwwp���������www�����������wwwp����p�www��������x�wwwwp���x�w�www������wx�wpw�������wx�ww wwp �p��wwx�www  wp��wwx�wwwwwwww��wwwx�wwwwwwwwp�wwwx���������� �����
 �STRING WindowName
HANDLE  HSpace
FLOAT local msg,Repeat,_enable  // ������������ fwKeys ��� �������� ���� ����������� �������
FLOAT wVkey
FLOAT ScanCode

if(msg==WM_KEYDOWN)
msg:=0
exit()
endif
if(msg==WM_KEYUP)
ScanCode:=0
wVkey:=0
msg:=0
exit()
endif

 if (WindowName != "" && (~HSpace==#0));  HSpace := GetWindowSpace(WindowName); endif
 if (~HSpace)
  registerobject(~HSpace,#0,"",WM_ALLKEYMESSAGE ,0) // ���������������� ������
  _enable:=0
 endif

  ��������� ����������� ������ @@   $  
 SYSTEM.DBM e            p@6 5                       p@6 5 2                                    x   z    �        R + 5 F x   �     o d        z          �@         �               jk�  E   M  
 .   