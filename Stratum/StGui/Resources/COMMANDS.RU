// Определение ToolBar ov
// Русская версия
// File menu
command{370 hint "Новый рисунок" name "CM_NEWSPACE" menu "Создать векторный рисунок"}
command{154 id 3 hint "Новый проект" name "CM_NEWsimplyPROJECT" menu "Создание нового проекта"}
command{122 id 1 hint "Загрузить" name "CM_LOAD" menu "Загрузить файл"}
command{130 id 1 hint "Закрыть проект" name "CM_PROJECTCLOSE" menu "Закрыть текущий проект"}

command{368 id 13 hint "Печать" name "CM_PRINT" menu "Печатать активное окно"}
command{375 id 89 hint "Записать активное окно" name "CM_FILESAVE" menu "Записать активное окно"}
command{376 hint "Записать как" name "CM_FILESAVEAS" menu "Сохранить окно под новым именем"}
command{127 hint "Выход" name "CM_EXIT" menu "Выйти из Stratum Computer "}

// View menu
command{642 hint "Панель &инструментов" name "CM_CONTROLBARS" menu "Редактировать панель инструментов..."}
command{378 hint "Строка состояния" name "CM_VIEWSTATUSBAR" menu "Показать или спрятать строку состояния"}

command{648 hint "Главное меню" name "CM_VIEWMAINMENU" menu "Показать или спрятать главное меню"}
command{345 id 68 hint "Информация о системе" name "CM_SYSINFO" menu "Открыть окно с информацей о текущем состоянии системы"}
command{129 id 78 hint "Иерархия проекта" name "CM_HIERARCHY" menu "Показать иерархию загруженного проекта"}
command{145 id 32 hint "Сообщения" name "CM_MESSAGES" menu "Показать окно с сообщенями среды"}
command{124 id 54 hint "Библиотеки" name "CM_LIBS" menu "Показать окно с загруженными библиотеками"}
command{118 id 55 hint "Главная схема" name "CM_MAINSCHEME" menu "Открыть схему корневого класса проекта"}
//End  View menu

// Edit menu
command{4072 hint "Redo" name "CM_EDITREDO" menu "Вернуть отмененную операцию"}
command{4071 hint "Undo" name "CM_EDITUNDO" menu "Отменить последнюю операцию"}

command{109 hint "Вырезать" name "CM_EDITCUT"  menu "Скопировать объект в буфер обмена и удалить"}
command{110 hint "Копировать" name "CM_EDITCOPY" menu "Скопировать объект в буфер обмена"}
command{111 hint "Вставить" name "CM_EDITPASTE" menu "Вставить объект из буфера обмена"}
command{112 hint "Специальная вставка" name "CM_SPECIALPASTE" menu "Вставить объект из буфера обмена использую параметры"}
command{113 hint "Удалить" name "CM_EDITDELETE" menu "Удалить текущий элемент"}
command{114 hint "Дублировать" name "CM_DUPLICATE" menu "Дублировать текущий элемент"}

// End Edit menu

// Вставка меню
command{200 hint "Вставить имидж" name "CM_INSERTOBJECT" menu "Вставить имидж из библиотеки..."}
command{323 hint "Создать и вставить новый имидж..." name "CM_CREATENEWOBJECT" menu "Создать и вставить новый имидж..."}
command{310 hint "Контактную площадку" name "CM_NEWVARPOINT" menu "Создать контактную площадку"}
command{301 id 11 hint "Проведение связи" name "CM_LINK" style 4 group 2 }
command{3013 hint "Из файл" name "CM_IMPORTOBJ" menu "Импортировать из файла"}

// 2d graphics

command{3000 id 10 hint "Режим выбора" name "CM_BASEMODE" style 4 group 2 menu "Перейти в режим выбора объектов"}
command{3001 id 20 hint "Линия" name "CM_LINE" style 4 group 2 menu "Перейти в режим создания линии"}
command{3014 id 27 hint "Дуга" name "CM_ARC" style 4 group 2 menu "Перейти в режим создания дуги"}
command{3011 id 98 hint "Полилиния" name "CM_POLYLINE" style 4 group 2 menu "Перейти в режим создания полилинии"}
command{3002 id 21 hint "Прямоугольник" name "CM_RECT" style 4 group 2 menu "Перейти в режим создания прямоугольника"}
command{3004 id 22 hint "Окружность" name "CM_CIRCLE" style 4 group 2 menu "Перейти в режим создания окружности"}
command{3003 id 23 hint "Скругленный прямоугольник" name "CM_ROUNDRECT" style 4 group 2 menu "Перейти в режим создания скругленного прямоугольника"}
command{3009 id 33 hint "Текст" name "CM_TEXT" style 4 group 2 menu "Перейти в режим создания текста"}
command{3005 id 17 hint "Группа" name "CM_GROUP" style 4 group 2 menu "Перейти в режим создания группы"}
command{3013 id 30 hint "Вставить из файла" name "CM_FILE" menu "Вставить из файла"}
command{3007 hint "Создание битовой карты" name "CM_STATEbitmap" menu "Переход в диалог создание битовой карты"}
command{3008 hint "Создание двойной битовой карты" name "CM_STATEdouble" menu "Переход в диалог создание двойной битовой карты"}
command{3012 hint "Проекция 3d пространства" name "CM_STATEview3d" menu "Создание проекции 3d пространства"}
command{3006 id 17 hint "Группа регионов" name "CM_RGNGROUP" style 4 group 2 menu "Перейти в режим создания группы регионов"}
command{0 hint "3d примитивы" name "CM_PRIMITIVES3D" menu "Не поддерживается"}

// windows items
command{3020 hint "Edit" name "CM_STATEwinedit" menu "Создание строки ввода"}
command{3021 hint "ComboBox" name "CM_STATEwincombo" menu "Создание выпадающего списка"}
command{3022 hint "CheckBox" name "CM_STATEwincheck" menu "Создание CheckBox"}
command{3023 hint "RadioButton" name "CM_STATEwinradio" menu "Создание радио кнопки"}
command{3024 hint "Button" name "CM_STATEwinbutton" menu "Создание кнопки"}
command{3025 hint "ListBox" name "CM_STATEwinlistbox" menu "Создание списка"}
// 3d
command{3057 hint "Дублировать камеру" name "CM_3DCLONECAMERA" menu "Создать копию используемой в данный момент камеры"}
command{3056 hint "Создать новую камеру" name "CM_3DNEWDEFCAMERA" menu "Создать камеру с параметрами по умолчанию"}
// END Вставка меню

// Формат menu
// Z ордер
command{139 hint "На передний план" name "CM_TOFRONT" menu "Поместить текущий 2d объектт на передний план"}
command{147 hint "На задний план" name "CM_TOBACK" menu "Поместить текущий 2d объектт на задний план"}
command{148 hint "Приблизить" name "CM_FORWARDONE" menu "Поместить текущий 2d объектт поверх остальных"}
command{149 hint "Отдалить" name "CM_BACKONE" menu "Поместить текущий 2d объектт позади остальных"}


command{343 hint "Добавить в группу" name "CM_ADDTOGROUP" menu "Добавить новый объкт в выбранную группу"}
command{341 hint "Разгруппировать" name "CM_UNGROUP" menu "Разгруппировать активную группу"}
command{342 hint "Удалить из группы" name "CM_DELFROMGROUP" menu "Удалить текущий объект из родительской группы"}

command{215 id 58 hint "Свойства имиджа" name "CM_PROPERTIES" menu "Свойства выбранного объекта..."}
command{216 id 56 hint "Схема имиджа" name "CM_IMGSCHEME" menu "Схема активного имиджа"}
command{336 id 33 hint "Модель имиджа" name "CM_TEXT" menu "Текст  активного имиджа"}
command{217 id 57 hint "Изображение имиджа" name "CM_IMAGE" menu "Изображение активного имиджа"}
// End Формат menu

// Лист
command{137 hint "Сетка вкл/выкл" name "CM_LAYERGRID" menu "Переключение видимости сетки"}
command{134 hint "Видимость имиджей" name "CM_LAYERCLASSES" menu "Переключение видимости имиджей"}
command{135 hint "Видимость связей" name "CM_LAYERLINKS" menu "Переключение видимости связей"}
command{136 hint "Видимость графики" name "CM_LAYEROTHER" menu "Переключение видимости графических объектов"}
command{151 hint "Редактировать слои..." name "CM_LAYERCUSTOM" menu "Редактировать слои..."}
command{153 hint "Windows on/off" name "CM_HOOKCONTROLS" menu "Запретить или разрешить работу windows окон на листе"}

command{101 hint "Экспортировать..." name "CM_SAVESCHEME" menu "Экспортировать текущее окно"}
command{344 hint "Параметры листа" name "CM_SPACEPROP" menu "Открытие диалога со свойствами активного листа"}
command{138 id 12 hint "Порядок вычисления" name "CM_CALCORDER" style 4 group2  menu "Редактировать порядок вычислений"}
// end Лист

// Моделирование
command{116 id 41 hint "Запуск" name "CM_RUN" style 4 group 1 menu "Запустить модель на выполнение или продолжить работу"}
command{121 id 40 hint "Один шаг" name "CM_STEP" style 4 group 1 menu "Выполнить один шаг"}
command{119 id 42 hint "Пауза" name "CM_PAUSE" style 4 group 1 menu "Приостановить работу модели"}
command{141 id 43 hint "Стоп (очистить)" name "CM_CLEARALL" style 4 group 1 menu "Закрыть все, остановить модель и установить переменные по умолчанию"}
command{652 id 34 hint "Предыдущий проект" name "CM_CLOSECHILDPROJECT" menu "Закрыть текущий проект и вернуться к предудущему"}

command{340 id 74 hint "Записать состояние в default.stt" name "CM_SAVETODEF" menu "Записать переменные в файл с именем по умолчанию : default.stt"}
command{339 id 75 hint "Загрузить состояние из default.stt" name "CM_LOADFROMDEF" menu "Загрузить переменные из файла с именем по умолчанию : default.stt"}
command{377 id 65 hint "Записать все изменения" name "CM_SAVEALL" menu "Записать все изменения внесенные в классы"}

command{313 id 74 hint "Записать состояние..." name "CM_STATESAVE" menu "Записать переменные в файл с произвольным именем"}
command{314 id 75 hint "Загрузить состояние..." name "CM_LOADSTATE" menu "Загрузить переменные из файла с произвольным именем"}

command{337 hint "Сохранить переменные имиджа" name "CM_OBJECTVARSAVE" menu "Сохранить переменные активного имиджа в файл"}
command{338 hint "Прочитать переменные имиджа" name "CM_OBJECTVARLOAD" menu "Прочитать переменные активного имиджа из файла"}
command{2268 hint "Установить по умолчанию" name "CM_OBJECTVARDEF" menu "Установить по умолчанию переменные активного имиджа "}

command{312 hint "Обнулить переменные" name "CM_VARCLEAR" menu "Обнулить все переменные переменные"}
command{143 hint "Закрыть потоки" name "CM_CLOSESTREAM" menu "Закрыть все открытые потоки"}
command{144 hint "Закрыть окна" name "CM_CLOSEWINDOWS" menu "Закрыть окна открытые моделью"}
command{329 hint "Закрыть все базы" name "CM_CLOSEDATABASES" menu "Закрыть все открытые базы"}
command{2275 hint "Закрыть видео файлы" name "CM_CLOSEVIDEOS" menu "Закрыть видео файлы не используемые фреймами"}

command{117 hint "Параметры среды" name "CM_DESCKTOP" menu "Диалог с параметрами среды"}

// Окна
command{102 hint "Каскадом" name "CM_CASCADECHILDREN" menu "Расположить окна каскадом"}
command{103 hint "Черепица горизонтально" name "CM_TILEHORZ" menu "Черепица горизонтально"}
command{104 hint "Черепица вертикально" name "CM_TILEVERT" menu "Черепица вертикально"}
command{105 hint "Упорядочить иконки" name "CM_ARRANGEICONS" menu "Упорядочить иконки"}
command{4053 hint "Закрыть окна редакторов" name "CM_CLOSEEDITCHILDREN" menu "Закрыть окна редакторов"}
command{106 hint "Закрыть все окна" name "CM_CLOSECHILDREN" menu "Закрыть все окна"}
// end

// help
command{318 hint "Содержание" name "CM_HELPINDEX1" menu "Содержание помощи"}
command{321 hint "Клавиатура" name "CM_HELPKEYBOARD" menu "Помощь по работе с клавиатурой"}
command{320 hint "Использование справки" name "CM_HELPUSING_HELP" menu "Как правильно использовать справочную систему"}
command{315 hint "Работа с графикой" name "CM_HELPGRAPI" menu "Помощь по работе с графикой"}
command{327 hint "Программирование" name "CM_APIREFERENCE" menu "Помощь по программированию"}
command{316 hint "Теория моделирования" name "CM_HELPTHEORY" menu "Помощь по теории моделирования"}
command{4065 hint "Системные библиотека" name "CM_LIBHELP" menu "Помощь по системным библиотека"}
command{346 hint "Техническая поддержка" name "CM_SUPPORT" menu "Как получить техническую поддержку"}
command{319 hint "О программе..." name "CM_HELPABOUT" menu "Получить информацию о программе..."}
// end help

/* Редактирование */

command{305 id 26 hint "Редактировать точки " name "CM_EDITPOINTS" style 3 menu "Перейти в режим редактирования точек полилинии"}

command{4063 id 64 hint "Масштаб" name "CM_SCALEDIALOG"
  combo {command 2262 size 50 strings{"5%","10%","25%","33%","50%","100%"!,"150%","200%","300%","400%","500%"}}
}

command{303 id 16 hint "Линза" name "CM_ZOOM" style 4 group 2}
command{3015 id 46 hint "Переместить сдержимое окна" name "CM_DRAG" style 4 group 2}

command{0   id 14 hint "Помощь" name "CM_LOCALHELP" }


/* Видео */
command{331 id 5 hint "На начало" name "CM_VSTART" }
command{330 id 7 hint "Проиграть" name "CM_VPLAY" }
command{333 id 8 hint "Пауза"     name "CM_VPAUSE" }
command{335 id 4 hint "Стоп"      name "CM_VSTOP" }
command{332 id 9 hint "На конец"  name "CM_VEND" }
command{646 id 73 hint "Проигрывать без звука"   style 3 name "CM_VNOAUDIO" }
command{647 id 61 hint "Цикличское проигрывание" style 3 name "CM_VCYCLE" }
// end video

/* Редактирование  битовых карт*/

command{3036 id 47 group 3 hint "пипетка" style 4 name "CM_bPIPE" }
command{3028 id 19 group 3 hint "карандаш" style 4 name "CM_bPEN" }
command{3029 id 20 group 3 hint "линия"     style 4 name "CM_bLINE" }
command{3030 id 21 group 3 hint "прямоугольник"   style 4 name "CM_bRECT" }
command{3031 id 44 group 3 hint "закрашенный прямоугольник" style 4 name "CM_bFILLRECT" }
command{3032 id 22 group 3 hint "эллипс" style 4 name "CM_bELLIPSE" }
command{3033 id 45 group 3 hint "закрашенный эллипс" style 4 name "CM_bFILLELLIPSE" }
command{3034 id 18 group 3 hint "закраска"     style 4 name "CM_bFILL" }
command{3035 id 29 group 3 hint "Ножницы"      style 4 name "CM_bCUTTER" }
command{3039 id 99 group 3 hint "Распылитель"  style 4 name "CM_bSPRAY" }

command{363  id 39 hint "Цвет"          name "CM_bCOLOR" }
command{3037 id 28 hint "Сетка (On/Off)" style 3 name "CM_bGRID" }


command{3046 id 81 group 20 hint "Перемещение" style 4              name "CM_MOVE3D" }
command{3047 id 80 group 20 hint "Размер объекта" style 4           name "CM_SIZE3D" }
command{3048 id 82 group 20 hint "Вращение" style 4                 name "CM_ROTATE3D" }
command{3050 id 60 group 20 hint "Масштабировать" style 4           name "CM_scnSIZE3D" }
command{3051 id 62 group 2  hint "Поворот камеры" style 4           name "CM_EDITCAMERA3D" }

command{4056 id 92 hint "Bar 3d"     name "CM_primBAR3D" }
command{4057 id 95 hint "Tore 3d"  name "CM_primTORE3D" }
command{4058 id 93 hint "Cylinder 3d"   name "CM_primCYLINDER3D" }
command{4059 id 91 hint "Сфера 3d"  name "CM_primSPHERE3D" }
command{4060 id 96 hint "Cone 3d"  name "CM_primCONE3D" }
command{4061 id 94 hint "Полусфера"  name "CM_primSUBSPHERE3D" }



command{3053 id 16 hint "Режим привязки координат" name "CM_3Dsystem"
  combo {command 3053 size 60 listonly strings{"View"!,"World","Local","Parent"}}
}

command{3054 id 16 hint "Режим привязки координат" name "CM_3Daxis"
  combo {command 3054 size 55 listonly strings{"X-Y-Z","X-Y","Z-X","Z-Y","X","Y"!,"Z"}}
}

command{3055 id 86 hint "Фиксирование системы координат" style 3 name "CM_LOCKaxis3D" }

command{4075 id 100 hint "Добавить новую запись в конец" name "CM_APPEND" }
command{4080  id 101 hint "Удалить текущую запись" name  "CM_REMOVERECORD" }
command{4076 id 102 hint "Сортировка в порядке возрастания" name "CM_SORTAZ" }
command{4077 id 103 hint "Сортировка в порядке убывания" name "CM_SORTZA" }
// Сами бары


// 2d - 20
// 3d - 30
// битовый 40
// базы 50

range "Стандартная" {id 1,dock 4, "CM_NEWsimplyPROJECT","CM_LOAD","CM_PRINT",separator,"CM_SAVETODEF",
"CM_LOADFROMDEF","CM_SAVEALL","CM_FILESAVE",separator,"CM_SYSINFO","CM_HIERARCHY","CM_LIBS","CM_MESSAGES"}

range "Управление" {id 2,dock 1,"CM_PAUSE","CM_RUN","CM_STEP","CM_CLEARALL","CM_CLOSECHILDPROJECT"}
range "Рисование" {id 20,dock 1, "CM_BASEMODE","CM_LINK","CM_CALCORDER","CM_LINE","CM_POLYLINE","CM_RECT",
				   "CM_ROUNDRECT","CM_CIRCLE","CM_ARC","CM_TEXT","CM_GROUP",separator,"CM_FILE",separator,"CM_DRAG","CM_ZOOM","CM_SCALEDIALOG",separator,"CM_EDITPOINTS"}

range "Битовый редактор" {id 40, "CM_bPIPE","CM_bPEN","CM_bLINE","CM_bRECT","CM_bFILLRECT",
      "CM_bELLIPSE","CM_bFILLELLIPSE","CM_bFILL","CM_bSPRAY","CM_bCUTTER",separator,"CM_bCOLOR","CM_bGRID"}

range "Видео" {"CM_VSTART","CM_VPLAY","CM_VPAUSE","CM_VSTOP","CM_VEND",separator,"CM_VNOAUDIO","CM_VCYCLE" }

range "3d графика" {id 30,dock 3, "CM_MOVE3D","CM_SIZE3D","CM_ROTATE3D",separator,
                     "CM_EDITCAMERA3D",separator,"CM_3Dsystem",separator,"CM_3Daxis",separator,"CM_LOCKaxis3D",separator,
// примитивы
"CM_primBAR3D","CM_primTORE3D","CM_primCYLINDER3D","CM_primSPHERE3D","CM_primCONE3D","CM_primSUBSPHERE3D" }

range "Базы данных" {id 50,"CM_APPEND","CM_REMOVERECORD",separator,"CM_SORTAZ","CM_SORTZA" }

