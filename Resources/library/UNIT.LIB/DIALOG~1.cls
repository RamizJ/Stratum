SB0  0     	 DialogBox 	 	 MainTitle ��������� ����������� ����	 DialogBox STRING    Title �������� �������� Insert STRING    Template8 ��� ������, ����� �������� �������� ����������� ��������   STRING    Object@ ��� �������-������, ����� �������� �������� ����������� �������� ..\page STRING    hDialog ���������� ����������� ����   HANDLE�   show. 0/1 - �� ����������/���������� ���������� ����   FLOAT�   control6 0 - ������� �� ������ Cancel; 1 - ������� �� ������ Ok   FLOAT�   _enable   1 FLOAT    index     FLOAT�  
 �STRING MainTitle
STRING Title, Template, Object
HANDLE hDialog
FLOAT show, control, _enable
float local index

if(not(~index))
  if(not(~hDialog))
    hdialog := new()
  endif
  index := vinsert(~hDialog, "PROPERTY_SHEET_PAGE")
  Vset(~hDialog, ~index-1, "template", ~Template)
  Vset(~hDialog, ~index-1, "title", ~Title)
  Vset(~hDialog, ~index-1, "object", ~Object)
else
endif

if(~show)
  control := DialogBox(~MainTitle, ~hDialog)
  show := 0
endif * ����� ������ ��� �������� ����������� ���� @@     
 SYSTEM.DBM l     - 5 Y   W 5  �    z 
 PROPERTY_SHEET_PAGE �             �? z  template  y  �           �? z  title y  �           �? z  object  y  �3 Y   5 k y     `                xV[  (  
    