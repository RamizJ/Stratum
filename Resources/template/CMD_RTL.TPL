// ����������� ToolBar ov
// ������� ������
// File menu
command{122 id 1 hint "�������" name "CM_LOAD" menu "��������� ������"}
command{130 id 1 hint "������� ������" name "CM_PROJECTCLOSE" menu "������� ������� ������"}
command{368 id 3 hint "������" name "CM_PRINT" menu "�������� �������� ����" rbutton 4074}
command{127 hint "�����" name "CM_EXIT" menu "����� �� Stratum Computer "}


// View menu
command{145 id 24 hint "���������" name "CM_MESSAGES" menu "�������� ���� � ���������� �����"}
//End  View menu


// �������������
command{116 id 11 hint "������" name "CM_RUN" style 4 group 1 menu "��������� ������ �� ���������� ��� ���������� ������"}
command{121 id 10 hint "���� ���" name "CM_STEP" style 4 group 1 menu "��������� ���� ���"}
command{119 id 12 hint "�����" name "CM_PAUSE" style 4 group 1 menu "������������� ������ ������"}
command{141 id 13 hint "���� (��������)" name "CM_CLEARALL" style 4 group 1 menu "������� ���, ���������� ������ � ���������� ���������� �� ���������"}
command{652 id 14 hint "���������� ������" name "CM_CLOSECHILDPROJECT" menu "������� ������� ������ � ��������� � �����������"}
command{4100 id 25 hint "��������� ��������" name "CM_PREVPAGE"  menu "��������� ��������"}
command{4101 id 27 hint "��������� ��������" name "CM_NEXTPAGE" menu "��������� ��������"}


command{313 id 22 hint "�������� ���������..." name "CM_STATESAVE" menu "�������� ���������� � ���� � ������������ ������"}
command{314 id 23 hint "��������� ���������..." name "CM_LOADSTATE" menu "��������� ���������� �� ����� � ������������ ������"}


/* ����� */
command{331 id 5 hint "�� ������" name "CM_VSTART" }
command{330 id 7 hint "���������" name "CM_VPLAY" }
command{333 id 8 hint "�����"     name "CM_VPAUSE" }
command{335 id 4 hint "����"      name "CM_VSTOP" }
command{332 id 9 hint "�� �����"  name "CM_VEND" }
command{646 id 20 hint "����������� ��� �����"   style 3 name "CM_VNOAUDIO" }
command{647 id 21 hint "���������� ������������" style 3 name "CM_VCYCLE" }
// end video


command{4075 id 15 hint "�������� ����� ������ � �����" name "CM_APPEND" }
command{4080 id 16 hint "������� ������� ������" name  "CM_REMOVERECORD" }
command{4076 id 17 hint "���������� � ������� �����������" name "CM_SORTAZ" }
command{4077 id 18 hint "���������� � ������� ��������" name "CM_SORTZA" }
command{4082 id 19 hint "��������� SQl ������" name "CM_DOSQL" }

// ���� ����

// ���� 50

range "�����������" {id 1,dock 4,"CM_LOAD","CM_PRINT",
separator,
"CM_PAUSE","CM_RUN","CM_STEP","CM_CLEARALL","CM_PREVPAGE","CM_NEXTPAGE"
}

range "�����" {"CM_VSTART","CM_VPLAY","CM_VPAUSE","CM_VSTOP","CM_VEND",separator,"CM_VNOAUDIO","CM_VCYCLE" }


range "���� ������" {id 50,"CM_APPEND","CM_REMOVERECORD",separator,"CM_SORTAZ","CM_SORTZA",separator,"CM_DOSQL" }

