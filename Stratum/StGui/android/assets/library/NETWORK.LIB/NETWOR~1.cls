SB0  �      Network_Server_Object   sender8 1 ���� ����� �������� ���� ���������� � 0 ���� ��������� 1 FLOAT   
 asynchroneM 1 ���� ����� ��� ����� �������� ���� ���������� � 0 ���� ������ ��� ��������� 0 FLOAT    protocol ��� ��������� TCP/IP STRING   
 ObjectName ��� ������� � ����������� .. STRING    flags ������������ �����   FLOAT�   id ����� ����� 5000 FLOAT    _disable     FLOAT�  
 /FLOAT sender,asynchrone
STRING protocol,ObjectName

flags:=1
if(sender)
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

_disable:=RegisterNetObject(ObjectName,protocol,"",~id,~flags)
 @      L          �?     5 %           @j     5 #           @j   3 3   5 3          @j   x  x  z        �	 RegisterNetObject      �r�  �  
 