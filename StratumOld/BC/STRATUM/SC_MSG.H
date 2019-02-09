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

// Сообщения библиотекам

#define SC_INITLIBRARY         0x001   // первый параметр -  TSTRATUMINFO*
#define SC_DONELIBRARY         0x002


// Уведомляющие сообщения
#define SC_GETINFO             0x003   // произвести настройку,
									   // первый параметр - HWND - parent
#define SC_INITMENUPOPUP       0x004   // инициализация PopupMenu
									   // первый параметр - TSTRATUMMENU

#define SC_GETVERSION          0x005   // получить версию
                              // первый параметр - TSTRATUMVERSION
#define SC_SENDCMD             0x006   // выполнить команду
                              // второй параметр - char*
                              // первый - код команды
#define SC_CMD_ARG             0x0    // командная строка
#define SC_CMD_USER            0x10    // командная строка

#define SC_ARGUMENTS           0x07   // выполнить командную строку
                              // первый - _argc
                              // второй - _argv


#define SC_OBJECTSCHANGED      0x010   // произошло изменение объектов
#define SC_CANCLOSE            0x020   // можно ли закончить работу?
#define SC_PROJECTRESET        0x030   // Все очистили
#define SC_PROJECTLOAD         0x040   // Загружен новый проект
#define SC_PROJECTCLOSE        0x050   // Закрываем проект
#define SC_PROJECTPAUSE        0x060   // нажали на паузу
#define SC_PROJECTRUN          0x070   // запустили

// Для фильтров
#define SC_GETFILEFORMAT       0x100   // первый параметр - имя файла
#define SC_EXPORTFILE          0x101   // первый параметр TSTRATUMFILTER
#define SC_IMPORTFILE          0x102   // первый параметр TSTRATUMFILTER
#define SC_RESIDENT            0x103   // можно висеть в памяти

// Разное

#define SC_DLLDIALOG           0x110   // вызов диалога (HWND,BOOL about)
#define SC_COMMAND             0x111   // вызов команды (HWND,cmd)
#define SC_CUROBJ2DCHANGED     0x112



// Из Dll (посылаются внешними)
#define SC_GETPROCADDRESS      0x1001  // первый имя функции
#define SC_INSTALLCALLBACK     0x1002
#define SC_UNINSTALLCALLBACK   0x1003

// Для инсталяции CallBackov
#define SC_CB_ONESTEP           0x01   // Вызывается при каждом шаге


// типы SC_INITMENUPOPUP
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
