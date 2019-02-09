/*
Copyright (c) 1996,97  ML

Project: Stratum 3.0

Module Name:
			messages.h
Description:
	define all messages for stratum
Author:
			Alexander Shelemekhov
*/

#ifndef __SC_MESSAGES_H_
#define __SC_MESSAGES_H_


//******************************************************************
//   Сообщения дерева
//******************************************************************


// Message from ViewTree in Parent Window:
#define TVN_GETITEMINFO 0x0500
 // Запрос на уточнение информации об элементе ViewTree,
 // lParam содержит Define_User_Data этого элемента.
#define TVN_SETACTIVEITEM 0x0501
 // Сообщение о новом активном элементе ViewTree,
 // lParam содержит Define_User_Data этого элемента.
#define TVN_INSERTITEM 0x0502
 // Сообщение о нажатии клавиши INSERT ,
 // lParam содержит Define_User_Data активного элемента.
#define TVN_DELETEITEM 0x0503
 // Сообщение о нажатии клавиши DELETE ,
 // lParam содержит Define_User_Data активного элемента.

#define TVN_ACTIVECHANGED 0x0504
#define TVN_DBLCLK        0x0505

#define TVN_ITEMMENU      0x0506
 // Сообщение о нажатии клавиши RButton ,
 // lParam содержит Define_User_Data активного элемента.

#define TVN_NEEDPAINT      0x0507
 // lParam содержит указатель на TREEPAINTSTRUCT.
 // Message from Parent Window in ViewTree:


#define TVN_GETITEMINFO 0x0500
 // Запрос на получение информации об элементе ViewTree,
 // lParam содержит указатель на TreeItemData куда
 // помещаются данные этого элемента.
 // data - содержит данные

#define TVN_GETACTIVEITEM 0x0501
 // Запрос на получение информации об активном элементе ViewTree,
 // lParam содержит указатель на TreeItemData куда
 // Если нет автивного то возвращается 0,иначе data

#define TVN_INSERTITEM 0x0502
 // Добавление элемента в TreeVindow,
 // lParam содержит TreeItemData* с информацией об элементе.
#define TVN_DELETEITEM 0x0503
 // удаление элемента из TreeVindow,
 // lParam содержит TreeItemData* с информацией об элементе.
#define TVN_UPDATEITEM 0x0504
 // Update элемента из TreeVindow,
 // lParam содержит TreeItemData* с информацией об элементе.
 // ParentData содержит Define_User_Data обновлямого элемента.
#define TVN_GETPARENT  0x0505

#define TV_EXPAND      0x0506
// раскрывает ветку
// wParam : 0 -
// wParam : 1 - Полностью раскрывает
// lParam содержит Define_User_Data этого элемента.

#define TV_SETSHIFT    0x508
// wParam : 0 - нет сдвига
// wParam : 1 - сдвиг на 1

// делает сдвиг

#define TV_SETACTIVEITEM 0x0509
 // lParam содержит Define_User_Data активного элемента.
//******************************************************************
//   Сообщения DRAG drop'а
//******************************************************************


#define TW_BEGINDRAGDROP  0x520
// извещение о том что начался драг дроп
#define TW_ACCEPTDROP    0x521
// посылается окну, над которым находится курсор с
// если вертает 0 то в это окно сделать дроп нельзя
//
#define TW_DROP          0x522
// сообщение о том что сделали дроп

//******************************************************************
//   Разное
//******************************************************************


#define TW_SETFOCUS      0x530
// Сообщение посылается чилду MDI childa о том что оно должно установить (wParam=1)
// или убрать свои окна, меню и ControlBar кнопки
// если возвращаемое значение не равно 5,то устанавливаем сами
#define TW_UPDATECLASSWINDOWS 0x531

enum UPDATE_MODE{UW_DELETE,UW_CREATE,UW_MODIFY};
enum UPDATE_WHAT{UW_LIBRARY,UW_CLASS,UW_OBJECT,UW_VARS};

//******************************************************************
// Сообщения графических окон и к графическим окнам
//******************************************************************
#define TW_GETSPACEINFO   0x532
// lparam - указатель на текстовый буфер для информации о графическом пространстве

//******************************************************************
// Сообщения закладок и к закладкам
//******************************************************************

#define PSN_PAGESELECT 0x540
// Посылается родителю с извещением о том, что изменилась активная страница
// Если вернули ноль то переключения не происходит

#define PST_GETACTIVEPAGE 0x540
// Возвращает указатель на TPropertyButton;
#define PST_GETALLPAGES 0x541
// Возвращает указатель на TPropertyButton;

#define PST_SETACTIVEPAGE 0x542
// Устанавливает активным TPropertyButton;

// Сообщения для диалоговых панелей

#define PSP_INITPAGE 0x550
// Посылается странице при ее первом выборе

#define PSP_VALID    0x551
// Посылается странице при ее уходе на второй план
// если wParam = 1 если происходит закрытие всего диалога(только OK)
// Если вернули ноль то переключения не происходит
// lParam TPropertyButton

#define PSP_APPLY    0x552
// Посылается странице , если нажали кнопку Apply
#define PSP_CLOSE  0x553
// Посылается странице при ее закрытии
// wParam!=0 OK (1 страница была иницализированна,2 если нет).

// wParam=0 Cancel
#define PSP_ACTIVATE 0x554
// Посылается странице , которая активируется
//******************************************************************
//   Сообщения для ColorButton
//******************************************************************

#define COLORB_GETCOLOR 0x570
// Возвращает цвет
#define COLORB_SETCOLOR 0x571
// Устанавливает цвет (в lParam'e)
#define COLORB_COLORCHANGE 0x572
// Посылается родителю о том что сменился цвет
#define COLORB_ISCHANGE 0x572
// Определяет факт измененения цвета


#define TB_ADDCOLLUMN   0x580
#define TB_SETRANGEY    0x581
#define TB_SETACTIVE    0x582
#define TB_DELCOLLUMN   0x583
#define TB_GETACTIVE    0x584
#define TB_SETLABEL     0x585
#define TB_SETTEXT      0x586
#define TB_RESETCOLLUMNS 0x587
#define TB_GETCOLLUMN   0x588 // Возвращает прямоугольник столбца
#define TB_GETCELLRECT  0x589 // Возвращает прямоугольник ячейки
#define TB_UPDATECELL   0x58A
#define TB_GETCOUNT     0x58D
// wParam 0 по Y
//        1 по X

// Для курсора (просмотр баз)
#define TB_CURSORCHANGE 0x58C  // сообщение о изменении базы
// lParam указатель на структуру TCursorChangeInfo;

// lParam указывает на массив из двух long (y и x)
#define TBN_GETTEXT     0x580
#define TBN_UPDATECOMBO 0x581
#define TBN_GETCELLINFO 0x582
#define TBN_PAINTCELL   0x583 // надо поставить Ok если нарисовали
#define TBN_COLLUMNINFO 0x584
#define TBN_SETTEXT     0x585
#define TBN_ACTIVECHANGED 0x586
#define TBL_GETTEXT     0x587 // Посылается с прозьбой дать текст
#define TBN_BEGINDRAW   0x588
#define TBN_ENDRAW      0x589
#define TBL_LBUTTON     0x58A
#define TBN_RBUTTON     0x58B
#define TBN_GETEDITRECT 0x58D // запрос о размере и положении EDIT
#define TBN_DBLCLK      0x58E

/*
#define IV_SETLIBRARY   0x590
#define IV_SETACTIVE    0x591
#define IV_GETACTIVE    0x592
#define IV_GETLIBRARY   0x593
#define IVN_ACTIVECHANGED 0x590
#define IVN_DBLCLK      0x591
*/
#define CB_SETSCALE     0x592  // lparam - указатель на double
#define CB_GETSCALE     0x593  // lparam - указатель на double
#define CB_SETRANGE     0x594  // wparam - range
#define CB_PRESSBUTTON  0x595  // wparam - button id ,lparam - press
#define CB_DONECOMBO    0x596
#define CB_IDLE         0x597

//
#define EMN_RDOWN        0x598
#define EMN_HELP         0x599
#define ESM_HELP         0x0503


#define WM_ALLMOUSEMESSAGE 0x600
#define WM_ALLKEYMESSAGE   0x601
#define WM_CANCLOSE        0x602
#define WM_SPACEDONE       0x603
#define WM_SPACEINIT       0x604

#define BMP_SETDIB         0x605
#define BMP_GETDIB         0x607
/*
Lo(lParam)  xor mask
Hi(lParam)  and mask
*/
/*
для TMBPView
BMP_SETDIB wParam - HGLOBAL
BMP_GETDIB - return HGLOBAL
*/

#define WM_CONTROLNOTIFY   0x608

#define TW_SETCLASS        0x609
#define WM_HYPERJUMP       0x60A
#define WM_GETSCINFO       0x60B // Посылается от одного Sc другому
// В wParam  тип запроса
// 1 AllowMultiple

#define WM_GETTOOLVAR      0x60C
#define WM_SETTOOLVAR      0x60D                                  
#define WM_SETMENU         0x60E // установить menu  wParam - menu // Lparam указатель на BOOL

#endif
