SB0  �      DialogWindow 
  HSpace ���������� ������������   HANDLE�  
 WindowName ��� ����   STRING�   _disable   1 FLOAT�   msg     FLOAT�   ret     FLOAT�   _hSpace     HANDLE   _HObject     HANDLE   _WindowName     STRING   _Target     STRING  	 ClassName     STRING�  
 HANDLE HSpace
STRING WindowName
FLOAT _disable
FLOAT local msg, ret
HANDLE local _hSpace, _HObject
STRING local _WindowName, _Target, ClassName

if (~ClassName=="")
  ClassName := GetVarS("..", "_ClassName")
  ret := SetObjectName(~ClassName, ~_HObject, "DialogHandler")
  _disable := 1
endif

if(msg==WM_SPACEINIT)
 WindowName := _WindowName
 HSpace := ~_hSpace
 msg := 0
 RegisterObject(~HSpace,~HSpace,"",WM_SPACEDONE,0)
endif

if (msg==WM_SPACEDONE)
 HSpace := #0
 WindowName := ""
 msg := 0
endif
 � ����� ������ ��� ��������� ����� ���� � ����������� ������������
��� �������� ����������� ����.
����� ������������ ������ � ������� DialogWindow @    =   SCIENCE.DBM u   y 	 z    � 5 . z  ..  z  _ClassName  �{ 	 y 	   z  DialogHandler Y         �?          �@6 5 Y x  {                        z          �@         �         �@6 5 t         z    {                �fF  3  
 F   