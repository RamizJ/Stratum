HWND GetClassScheme(STRING)
// �������� ���������� ������������ ��������� ������
STRING  GetParentClass()
// �������� ����������� ��������� ������
HWND OpenClassImage(STRING)
// ������� ���� ���������� ����������� ��������� ������
HWND OpenClassScheme(STRING)
// ������� ���� ���������� ����� ��������� ������
HWND OpenSpaceWindow(STRING,STRING);
// ������� ���� ������������

CloseWindow(HWND)
// ������� ����
SetWindowText(HWND,STRING)
// ���������� ����� ����
STRING GetWindowText(HWND)
// �������� ����� ����
CloseAllWindows();
// ������� ��� MDI ����
CloseIDEWindows();
// ������� ��� MDI ���� �� �������� ����� ����������

HSP2D GetObjectSpace(FLOAT load);
// �������� ���������� ������������ ����� ������
HSP2D GetParentSpace(FLOAT load);
// �������� ���������� ������������ ����� �������� ������� ������
INTEGER CreateObject2dFromFile(HSP2D hSpace,FLOAT x,FLOAT y,STRING);
// ������� ����� �� VDR ����� � �������� ��� � ������������
DeleteObject2d(HSP2D hSpace,INTEGER);

INTEGER GetObjectHANDLE();
// ���������� ���������� ������� �� ����� ��������
MoveObject2d(HSP2D hSpace,INTEGER obj,FLOAT x,FLOAT y);
AddOscilographPoint(HSP2D hSpace,INTEGER obj,FLOAT maxpoint,FLOAT x,FLOAT y)
