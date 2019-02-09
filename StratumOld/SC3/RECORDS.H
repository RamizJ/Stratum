// RECORDS TYPES
#ifndef __fRECORDS_t_H_
#define __fRECORDS_t_H_
// CLASS records
#define CR_MATH      1
//
#define CR_MATHDATA  2
//
// размер переменной
#define CR_FLAGS     4
// флаг класса
#define CR_VARS      5
// переменные
#define CR_CHILDS    6
// чилды
#define CR_LINKS     7
// св¤зи
#define CR_ICON      8
// иконка (пока ее нет)
#define CR_SCHEME    9
// схема класса
#define CR_TEXT     10
// исходный текст присваиваний
#define CR_VARSIZE  11
// размер переменных (пишетс¤ только встроенными классами)
#define CR_IMAGE    12
// двухмерное изображение класса
#define CR_CODE     13
// это выполнимый код
#define CR_INFO     14
// текст примечаний
#define CR_VARS1    15
#define CR_CHILDSname  16

#define CR_CHILDSnameXY  17    // — координатами и флагами

#define CR_DEFICON     20
#define CR_CLASSTIME   21
#define CR_ICONFILE    22   // им¤ иконки

#define CR_HELPFILE    25

#define CR_EQU         30
#define CR_MOVEINFO    31

#define RR_VERSION     40                  

// Project file records (записи файла проекта)
#define PR_MAINCLASS 100
#define PR_PASSWORD  101
#define PR_VARS      102
#define PR_WATCH     103
// State file records

#define  VR_CLASSES  1000  // копи¤ структуру класса и подклассов
#define  VR_VARS     1001  // Ќепосредственно значени¤
#define  VR_SETVAR   1002  // ѕредустановленные переменные

#define CR_CLASSINFO 8000

// ??????

//ƒл¤ описани¤ страницы дл¤ печати

#define PPR_FIELDS    2048   // пол¤
#define PPR_ADDFIELDS 2049   // добавочные пол¤
#define PPR_SCALEMODE 2050
#define PPR_SCALECUSTOM 2051
#define PPR_FIELDSMODE  2052 // ѕол¤ дл¤ внешних листов
#define PPR_USEADDF     2053 // »спользовать добавочные пол¤
#define PPR_SRCRECT     2054 // исходник
#define PPR_USESRC      2055 // использовать исходник
#define PPR_PAGECOUNT   2056 // число копий

#endif
