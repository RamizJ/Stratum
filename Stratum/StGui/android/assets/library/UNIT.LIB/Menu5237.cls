SB0  |      Menu  
 WindowName ��� ���� @StratumFrame@ STRING    FileName# ��� ����� ����������� �������� ����   STRING    idItem     FLOAT   msg     FLOAT�   wNotifyCode     FLOAT   commandH ��������� ����� (�� ����� ��������� ���� ������ �������� ��� ����������)   FLOAT�   _enable   1 FLOAT�       ���������������������wwwwwwwwwwx�����wwwwwwwwwwx�������wwwwrwwwx���ww���w w""wwx���wwww� x'wwx��wwwpww"w "wwwx��wwwpw""'p wx�wwwpwp "�
�x�wwwwpw � 
���xwwwpwww"
�
��w��wwwpw ����www�wwpwp�
���wwwx�wwwpw �����ww�w|�www
���wwwx��w����p���wwww��|����z�ww wx��x����� ��www��x���������wwx��wx����������Ǐ�wx�������������wwx� ����������wwx��  ��������wwwx����  �����wwwx������  ���wwwwx�����ww �wwwwx� ���wwww�wwwwwx��  p w�wwwwwx�w��wwww�wwwwwwx�www��w�wwwwwwx�wwwww��wwwwwwwx���������������� L      @  2D  � :                        Y@      Y@      Y@      Y@          �    
 
  �]    
 
 �� N            o@      a@      @@      @@                      @@      @@   ��   
 
 ��g �    BMv      v   (                                   ��� ��� ���  �   �  �           ���   �  �   �� �   � � ��  ��� """"""""""""""""    !       !"    "" fDD  !"FadA DfD !adDDAfaFdDc3a!dF6Fc33 Dc3c33"adF3333"ac333aF33331 bUc333UUUQ331 `UUUU3af "UUfUQa ` ""UUUUU    ""UUUUUQ `     ""UUR f     "" `  ff     " U  ff    `UUU  ff  UUUUaf`fUUV   ffaf                      
 STRING WindowName,FileName
FLOAT local idItem,msg,wNotifyCode

if(msg)
 command:=idItem
 msg:=0
 exit()
endif

if (IsWindowExist(~WindowName))
 _enable:=not(LoadMenu(~WindowName,FileName,0))
 RegisterObject(~WindowName,#0,"",WM_COMMAND,256)
endif M ����� ������������� ���� ������������ ��������
����������� ������ - �������� @B   
 SYSTEM.DBM 6     5                   y   � 5 5 y   x           � -   y        z          q@       p@�    �X�  A   I   �  
 �   