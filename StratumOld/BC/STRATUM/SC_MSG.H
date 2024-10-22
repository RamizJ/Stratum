/*
Copyright (c) 1996,1997  Laboratory of United Modeling Knowledjes

Project: Stratum for Windows

Module Name:
			stratum.h

Author:
			Alexander Shelemekhov

Description:
		 code of messages in call scdllproc
*/

#if !defined(SC_CODES_HH)
#define SC_CODES_HH

// ��������� �����������

#define SC_INITLIBRARY         0x001   // ������ �������� -  TSTRATUMINFO*
#define SC_DONELIBRARY         0x002


// ������������ ���������
#define SC_GETINFO             0x003   // ���������� ���������,
									   // ������ �������� - HWND - parent
#define SC_INITMENUPOPUP       0x004   // ������������� PopupMenu
									   // ������ �������� - TSTRATUMMENU

#define SC_GETVERSION          0x005   // �������� ������
                              // ������ �������� - TSTRATUMVERSION
#define SC_SENDCMD             0x006   // ��������� �������
                              // ������ �������� - char*
                              // ������ - ��� �������
#define SC_CMD_ARG             0x0    // ��������� ������
#define SC_CMD_USER            0x10    // ��������� ������

#define SC_ARGUMENTS           0x07   // ��������� ��������� ������
                              // ������ - _argc
                              // ������ - _argv


#define SC_OBJECTSCHANGED      0x010   // ��������� ��������� ��������
#define SC_CANCLOSE            0x020   // ����� �� ��������� ������?
#define SC_PROJECTRESET        0x030   // ��� ��������
#define SC_PROJECTLOAD         0x040   // �������� ����� ������
#define SC_PROJECTCLOSE        0x050   // ��������� ������
#define SC_PROJECTPAUSE        0x060   // ������ �� �����
#define SC_PROJECTRUN          0x070   // ���������

// ��� ��������
#define SC_GETFILEFORMAT       0x100   // ������ �������� - ��� �����
#define SC_EXPORTFILE          0x101   // ������ �������� TSTRATUMFILTER
#define SC_IMPORTFILE          0x102   // ������ �������� TSTRATUMFILTER
#define SC_RESIDENT            0x103   // ����� ������ � ������

// ������

#define SC_DLLDIALOG           0x110   // ����� ������� (HWND,BOOL about)
#define SC_COMMAND             0x111   // ����� ������� (HWND,cmd)
#define SC_CUROBJ2DCHANGED     0x112



// �� Dll (���������� ��������)
#define SC_GETPROCADDRESS      0x1001  // ������ ��� �������
#define SC_INSTALLCALLBACK     0x1002
#define SC_UNINSTALLCALLBACK   0x1003

// ��� ���������� CallBackov
#define SC_CB_ONESTEP           0x01   // ���������� ��� ������ ����


// ���� SC_INITMENUPOPUP
#define SCMENU_CONTROLBAR      0x01
#define SCMENU_STATUSBAR       0x02
#define SCMENU_CURSORTITLE     0x03
#define SCMENU_GRPIMAJE        0x04
#define SCMENU_IMAJE           0x05
#define SCMENU_OBJ3D           0x06
#define SCMENU_VIEW3D          0x07
#define SCMENU_VARPLACE        0x08
#define SCMENU_OBJECT2D        0x09
#define SCMENU_LIBRARY         0x0A  // data1 -PLibrary

#define SCMENU_WATCH           0x10

#define SCMENU_MENUSYNTAX      0x11
#define SCMENU_MESSAGE         0x12
#define SCMENU_TREEITEM        0x13
#define SCMENU_SYNTAX          0x14
#define SCMENU_LINK            0x15
#define SCMENU_TABLE           0x16


#endif
