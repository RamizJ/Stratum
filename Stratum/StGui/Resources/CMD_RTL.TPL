// Определение ToolBar ov
// Русская версия
// File menu
command{122 id 1 hint "Открыть" name "CM_LOAD" menu "Загрузить проект"}
command{130 id 1 hint "Закрыть проект" name "CM_PROJECTCLOSE" menu "Закрыть текущий проект"}
command{368 id 3 hint "Печать" name "CM_PRINT" menu "Печатать активное окно" rbutton 4074}
command{127 hint "Выход" name "CM_EXIT" menu "Выйти из Stratum Computer "}


// View menu
command{145 id 24 hint "Сообщения" name "CM_MESSAGES" menu "Показать окно с сообщенями среды"}
//End  View menu


// Моделирование
command{116 id 11 hint "Запуск" name "CM_RUN" style 4 group 1 menu "Запустить модель на выполнение или продолжить работу"}
command{121 id 10 hint "Один шаг" name "CM_STEP" style 4 group 1 menu "Выполнить один шаг"}
command{119 id 12 hint "Пауза" name "CM_PAUSE" style 4 group 1 menu "Приостановить работу модели"}
command{141 id 13 hint "Стоп (очистить)" name "CM_CLEARALL" style 4 group 1 menu "Закрыть все, остановить модель и установить переменные по умолчанию"}
command{652 id 14 hint "Предыдущий проект" name "CM_CLOSECHILDPROJECT" menu "Закрыть текущий проект и вернуться к предудущему"}
command{4100 id 25 hint "Преыдущая старница" name "CM_PREVPAGE"  menu "Преыдущая старница"}
command{4101 id 27 hint "Следующая старница" name "CM_NEXTPAGE" menu "Следующая старница"}


command{313 id 22 hint "Записать состояние..." name "CM_STATESAVE" menu "Записать переменные в файл с произвольным именем"}
command{314 id 23 hint "Загрузить состояние..." name "CM_LOADSTATE" menu "Загрузить переменные из файла с произвольным именем"}


/* Видео */
command{331 id 5 hint "На начало" name "CM_VSTART" }
command{330 id 7 hint "Проиграть" name "CM_VPLAY" }
command{333 id 8 hint "Пауза"     name "CM_VPAUSE" }
command{335 id 4 hint "Стоп"      name "CM_VSTOP" }
command{332 id 9 hint "На конец"  name "CM_VEND" }
command{646 id 20 hint "Проигрывать без звука"   style 3 name "CM_VNOAUDIO" }
command{647 id 21 hint "Цикличское проигрывание" style 3 name "CM_VCYCLE" }
// end video


command{4075 id 15 hint "Добавить новую запись в конец" name "CM_APPEND" }
command{4080 id 16 hint "Удалить текущую запись" name  "CM_REMOVERECORD" }
command{4076 id 17 hint "Сортировка в порядке возрастания" name "CM_SORTAZ" }
command{4077 id 18 hint "Сортировка в порядке убывания" name "CM_SORTZA" }
command{4082 id 19 hint "Выполнить SQl запрос" name "CM_DOSQL" }

// Сами бары

// базы 50

range "Стандартная" {id 1,dock 4,"CM_LOAD","CM_PRINT",
separator,
"CM_PAUSE","CM_RUN","CM_STEP","CM_CLEARALL","CM_PREVPAGE","CM_NEXTPAGE"
}

range "Видео" {"CM_VSTART","CM_VPLAY","CM_VPAUSE","CM_VSTOP","CM_VEND",separator,"CM_VNOAUDIO","CM_VCYCLE" }


range "Базы данных" {id 50,"CM_APPEND","CM_REMOVERECORD",separator,"CM_SORTAZ","CM_SORTZA",separator,"CM_DOSQL" }

