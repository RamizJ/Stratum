left{
 SIZE 105
 NOARRANGE
 button{  POS 2,0 COMMAND 115 SIZE 23,23  ID 3 info "����� ������" }
 button{  POS 26,0  COMMAND 122 SIZE 23,23  ID 1 info "���������" }
 button{  POS 50,0  COMMAND 368 SIZE 23,23 ID 13 info "������"}

 button{  POS 109,0 COMMAND 3000 ID 10 style 4 state 1 info "����� ������"}
 button{  POS 145,1 SRC 0,0,8,20 size 12,24 COMMAND 11 } // << scroll

 button{  POS  2,26 COMMAND 116 SIZE 23,18 ID 41 group 1 style 36 info "������"} // run
 button{  POS 26,26 COMMAND 121 SIZE 23,18 ID 40 group 1 style 36 info "���� ���"} // step
 button{  POS 50,26 COMMAND 119 SIZE 23,18 ID 42 group 1 style 36 info "�����"} // pause
 button{  POS 74,26 COMMAND 141 SIZE 23,18 ID 43 group 1 style 36 info "���� (��������)"} // clear all
}

right{
  NOARRANGE
  offset -16
  SIZE 70
  button{  POS 10,1 SRC 8,0,8,20 size 11,24 COMMAND 11 } // >> scroll

  button{  POS 33,0  COMMAND 303 ID 16 POS 32,-1 SIZE 23,23 info "��������/���������"} // Zoom
  button{  POS 60,0  COMMAND 0   ID 14 POS 58,-1 SIZE 23,23 info "������"} //?help
  ScaleBox{ POS 23,28  }
 }

main{
/* ������ */
range{
ID 10
 name "�����"
 info "������ �� ������"
 button{ COMMAND 301  ID 11 style 4 info "���������� �����"}
 button{ COMMAND 138  ID 12 style 4 info "������� ����������"}

 separator (6)

 button{ COMMAND 124  ID 54 info "����������"} //lib
 button{ COMMAND 118  ID 55 info "������� �����"} // main scheme

 separator (6)

 button{ COMMAND 216  ID 56 info "����� ������"} // view scheme
 button{ COMMAND 336  ID 33 info "������ ������"} // view text
 button{ COMMAND 217  ID 57 info "����������� ������"} // view image
 button{ COMMAND 215  ID 58 info "�������� ������"} // properties

 separator (6)

 button{ COMMAND 345  ID 36 info "��������� ����������"} // sys info

// button{ COMMAND 0 ID 18 }
// button{ COMMAND 0 ID 19 }

}
range{
ID 11
 name "��������"
 info "2D ��������"
 button{ COMMAND 3001 ID 20 STYLE 4 info "�����"}
 button{ COMMAND 3011 ID 27 STYLE 4 info "���������"}
 button{ COMMAND 3002 ID 21 STYLE 4 info "�������������"}
 button{ COMMAND 3004 ID 22 STYLE 4 info "����������"}
 button{ COMMAND 3003 ID 23 STYLE 4 info "����������� �������������"}
 button{ COMMAND 3009 ID 33 STYLE 4 info "�����"}
 button{ COMMAND 3005 ID 17 STYLE 4 info "������"}
 Separator (10)

 button{ COMMAND 3013 ID 30 STYLE 4 info "�������� �� �����"}
/* button{ COMMAND 3007 ID 24 STYLE 4}
	button{ COMMAND 3008 ID 25 STYLE 4}
	button{ COMMAND 3010 ID 32 STYLE 4} */

 Separator (10)

 button{ COMMAND 305 ID 26 STYLE 3 info "�������������� �����"}
}

range{
ID 12
 name "�����"
 info "��� ������ � �����"
 button{ COMMAND 331 ID 5  info "�� ������"}
 button{ COMMAND 330 ID 7  info "���������"}
 button{ COMMAND 333 ID 8  info "�����"}
 button{ COMMAND 335 ID 4  info "����"}
 button{ COMMAND 332 ID 9  info "�� �����"}
}
range{
ID 14
 name "Bitmap"
 info "������� ��������"
 button{ COMMAND 3036 ID 47  style 4 info "�������"}
 button{ COMMAND 3028 ID 19  style 4 info "��������"}
 button{ COMMAND 3029 ID 20  style 4 info "�����"}
 button{ COMMAND 3030 ID 21  style 4 info "�������������"}
 button{ COMMAND 3031 ID 44  style 4 info "����������� �������������"}
 button{ COMMAND 3032 ID 22  style 4 info "������"}
 button{ COMMAND 3033 ID 45  style 4 info "����������� ������"}
 button{ COMMAND 3034 ID 18  style 4 info "��������"}
 button{ COMMAND 3035 ID 29  style 4 info "�������"}

 Separator (10)
 button{ COMMAND 363 ID 39 info "����" }

 Separator (10)
 //button{ COMMAND 2265 ID 28  style 3 info "�����"}
 Separator (10)
 button{ COMMAND 361  ID 37 info "���������"}
 button{ COMMAND 362  ID 38 info "���������"}
}


range {
ID 13
  NOARRANGE
  name "����"
  info "���� �����"
  button{  POS 10,1  SRC 217,2,8,8 size 12,12 COMMAND 4036 STYLE 3 info "���� 16 (������)"}
  button{  POS 10,13 SRC 217,2,8,8 size 12,12 COMMAND 4037 STYLE 3 info "���� 17 (������)"}
  button{  POS 24,1  SRC 217,2,8,8 size 12,12 COMMAND 4038 STYLE 3 info "���� 18 (������)"}
  button{  POS 24,13 SRC 217,2,8,8 size 12,12 COMMAND 4039 STYLE 3 info "���� 19 (������)"}
  button{  POS 38,1  SRC 217,2,8,8 size 12,12 COMMAND 4040 STYLE 3 info "���� 20 (������)"}
  button{  POS 38,13 SRC 217,2,8,8 size 12,12 COMMAND 4041 STYLE 3 info "���� 21 (������)"}
  button{  POS 52,1  SRC 217,2,8,8 size 12,12 COMMAND 4042 STYLE 3 info "���� 22 (������)"}
  button{  POS 52,13 SRC 217,2,8,8 size 12,12 COMMAND 4043 STYLE 3 info "���� 23 (������)"}

  button{  POS 70,1  SRC 217,2,8,8 size 12,12 COMMAND 4044 STYLE 3 info "���� 24 (�����)"}
  button{  POS 70,13 SRC 217,2,8,8 size 12,12 COMMAND 4045 STYLE 3 info "���� 25 (�����)"}
  button{  POS 84,1  SRC 217,2,8,8 size 12,12 COMMAND 4046 STYLE 3 info "���� 26 (�����)"}
  button{  POS 84,13 SRC 217,2,8,8 size 12,12 COMMAND 4047 STYLE 3 info "���� 27 (�����)"}
  button{  POS 98,1  SRC 217,2,8,8 size 12,12 COMMAND 4048 STYLE 3 info "���� 28  (�����)"}
  button{  POS 98,13 SRC 217,2,8,8 size 12,12 COMMAND 4049 STYLE 3 info "���� 29  (�����)"}
  button{  POS 112,1  SRC 217,2,8,8 size 12,12 COMMAND 4050 STYLE 3 info "���� 30 (�����)"}
  button{  POS 112,13 SRC 217,2,8,8 size 12,12 COMMAND 4051 STYLE 3 info "���� 31 (�����)"}

  button{  POS 150,1  SRC 217,2,8,8 size 12,12 COMMAND 4020 STYLE 3 info "���� 0 (�������)"}
  button{  POS 150,13 SRC 217,2,8,8 size 12,12 COMMAND 4021 STYLE 3 info "���� 1 (�������)"} 
  button{  POS 164,1  SRC 217,2,8,8 size 12,12 COMMAND 4022 STYLE 3 info "���� 2 (�������)"}
  button{  POS 164,13 SRC 217,2,8,8 size 12,12 COMMAND 4023 STYLE 3 info "���� 3 (�������)"}
  button{  POS 178,1  SRC 217,2,8,8 size 12,12 COMMAND 4024 STYLE 3 info "���� 4 (�������)"}
  button{  POS 178,13 SRC 217,2,8,8 size 12,12 COMMAND 4025 STYLE 3 info "���� 5 (�������)"}
  button{  POS 192,1  SRC 217,2,8,8 size 12,12 COMMAND 4026 STYLE 3 info "���� 6 (�������)"}
  button{  POS 192,13 SRC 217,2,8,8 size 12,12 COMMAND 4027 STYLE 3 info "���� 7 (�������)"}

  button{  POS 210,1  SRC 217,2,8,8 size 12,12 COMMAND 4028 STYLE 3 info "���� 8 (�������)"}
  button{  POS 210,13 SRC 217,2,8,8 size 12,12 COMMAND 4029 STYLE 3 info "���� 9 (�������)"}
  button{  POS 224,1  SRC 217,2,8,8 size 12,12 COMMAND 4030 STYLE 3 info "���� 10 (�������)"}
  button{  POS 224,13 SRC 217,2,8,8 size 12,12 COMMAND 4031 STYLE 3 info "���� 11 (�������)"}
  button{  POS 238,1  SRC 217,2,8,8 size 12,12 COMMAND 4032 STYLE 3 info "���� 12 (�������)"}
  button{  POS 238,13 SRC 217,2,8,8 size 12,12 COMMAND 4033 STYLE 3 info "���� 13 (�������)"}
  button{  POS 252,1  SRC 217,2,8,8 size 12,12 COMMAND 4034 STYLE 3 info "���� 14 (�������)"}
  button{  POS 252,13 SRC 217,2,8,8 size 12,12 COMMAND 4035 STYLE 3 info "���� 15 (�������)"}

 }
range {
 ID 30
  name "3d"
  info "������ � ����������� ���������"

  button{ COMMAND 3046 ID 71 STYLE 4 group 30 info "�����������"}
  button{ COMMAND 3047 ID 70 STYLE 4 group 30 info "������ �������"}
  button{ COMMAND 3048 ID 72 STYLE 4 group 30 info "��������"}

  Separator(6)

  button{ COMMAND 3050 ID 60 STYLE 4 group 30 info "��������������"}
  button{ COMMAND 3051 ID 61 STYLE 4 group 30 info "������� ������"}
  button{ COMMAND 3053 ID 62 STYLE 4 group 30 info "���������� ����� �������"}
  button{ COMMAND 3052 ID 63 STYLE 4 group 30 info "Pan (��������)"}

 }
}
