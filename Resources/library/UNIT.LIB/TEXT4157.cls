SB0  D      TextEdit   FileName= ��� �������������� ����� (� ����������� �� ���������� dialog)   STRING�   dialogD ���� ��� ��������� ������� ������ ����� - �������� 1, �� ��������� 0   FLOAT    ret     FLOAT�   _enable6 ���� ������������� ���� - �������� 1, (�� ��������� 0)   FLOAT�       ���������������������������������������������������������������������������������������������������������������������������������������������� � ������������   ������������� �������������   �����������  � ���������� �� ������ ��� ���  �������������� ��� �x���������   �x����������wp�wx����������wwx� ������� �wpwx�   ����� ��wx��    ������wpx����    �����p������    ���w������ww ������ ���wwww�wp ����  p w�ww����w��wwww�wwwp����www��w�wwww���wwwww��wwwwwp�����������������
 "STRING FileName
FLOAT dialog
FLOAT local ret

if (dialog)
 FileName := FileLoadDialog("","TXT|Text files(*.txt)|*.txt|All files |*.*|","")

// ��������  ����� �����
 if (~FileName == "")
  exit()
 endif
endif

ret := WinExecute("notepad " + ~FileName,SW_NORMAL)

_enable := 0 2 �������� � �������������� ���������� ����� (*.txt) @@     
 SYSTEM.DBM H     5 . z    z  TXT|Text files(*.txt)|*.txt|All files |*.*| z    g {   y   z    � 5 .   z  notepad   y   |        �?�                 ik�  *   2  
 X   