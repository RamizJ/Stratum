SB0  �      Network_Client_Object   sender8 1 ���� ����� �������� ���� ���������� � 0 ���� ��������� 0 FLOAT   
 asynchroneM 1 ���� ����� ��� ����� �������� ���� ���������� � 0 ���� ������ ��� ��������� 0 FLOAT    address �������� ������  �������  195.19.160.54 STRING    protocol ��� ��������� TCP/IP STRING   
 ObjectName ��� ������� � ����������� .. STRING    flags ������������ �����   FLOAT�   id ����� ����� 5000 FLOAT    _disable     FLOAT�  
 ?FLOAT sender,asynchrone
STRING address,protocol,ObjectName

flags:=0
if(~sender)
// ����������
  flags:=~flags | 2
  if(asynchrone)
   flags:=~flags | 8
  endif
else
// ����������
if(asynchrone)
   flags:=~flags | 4
  endif
endif

_disable:=RegisterNetObject(~ObjectName,protocol,~address,~id,~flags)
 @      K                 5 %           @j     5 #           @j   3 3   5 3          @j   y  x  y      �	 RegisterNetObject      0t�  �  
 