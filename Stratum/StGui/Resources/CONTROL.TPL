left{
 SIZE 105
 NOARRANGE
 button{  POS 2,0 COMMAND 115 SIZE 23,23  ID 3 info "Новый проект" }
 button{  POS 26,0  COMMAND 122 SIZE 23,23  ID 1 info "Загрузить" }
 button{  POS 50,0  COMMAND 368 SIZE 23,23 ID 13 info "Печать"}

 button{  POS 109,0 COMMAND 3000 ID 10 style 4 state 1 info "Режим выбора"}
 button{  POS 145,1 SRC 0,0,8,20 size 12,24 COMMAND 11 } // << scroll

 button{  POS  2,26 COMMAND 116 SIZE 23,18 ID 41 group 1 style 36 info "Запуск"} // run
 button{  POS 26,26 COMMAND 121 SIZE 23,18 ID 40 group 1 style 36 info "Один шаг"} // step
 button{  POS 50,26 COMMAND 119 SIZE 23,18 ID 42 group 1 style 36 info "Пауза"} // pause
 button{  POS 74,26 COMMAND 141 SIZE 23,18 ID 43 group 1 style 36 info "Стоп (очистить)"} // clear all
}

right{
  NOARRANGE
  offset -16
  SIZE 70
  button{  POS 10,1 SRC 8,0,8,20 size 11,24 COMMAND 11 } // >> scroll

  button{  POS 33,0  COMMAND 303 ID 16 POS 32,-1 SIZE 23,23 info "Увеличть/уменьшить"} // Zoom
  button{  POS 60,0  COMMAND 0   ID 14 POS 58,-1 SIZE 23,23 info "Помощь"} //?help
  ScaleBox{ POS 23,28  }
 }

main{
/* Пример */
range{
ID 10
 name "Схема"
 info "Работа со схемой"
 button{ COMMAND 301  ID 11 style 4 info "Проведение связи"}
 button{ COMMAND 138  ID 12 style 4 info "Порядок вычисления"}

 separator (6)

 button{ COMMAND 124  ID 54 info "Библиотеки"} //lib
 button{ COMMAND 118  ID 55 info "Главная схема"} // main scheme

 separator (6)

 button{ COMMAND 216  ID 56 info "Схема имиджа"} // view scheme
 button{ COMMAND 336  ID 33 info "Модель имиджа"} // view text
 button{ COMMAND 217  ID 57 info "Изображение имиджа"} // view image
 button{ COMMAND 215  ID 58 info "Свойства имиджа"} // properties

 separator (6)

 button{ COMMAND 345  ID 36 info "Системная информация"} // sys info

// button{ COMMAND 0 ID 18 }
// button{ COMMAND 0 ID 19 }

}
range{
ID 11
 name "Редактор"
 info "2D редактор"
 button{ COMMAND 3001 ID 20 STYLE 4 info "Линия"}
 button{ COMMAND 3011 ID 27 STYLE 4 info "Полилиния"}
 button{ COMMAND 3002 ID 21 STYLE 4 info "Прямоугольник"}
 button{ COMMAND 3004 ID 22 STYLE 4 info "Окружность"}
 button{ COMMAND 3003 ID 23 STYLE 4 info "Скругленный прямоугольник"}
 button{ COMMAND 3009 ID 33 STYLE 4 info "Текст"}
 button{ COMMAND 3005 ID 17 STYLE 4 info "Группа"}
 Separator (10)

 button{ COMMAND 3013 ID 30 STYLE 4 info "Вставить из файла"}
/* button{ COMMAND 3007 ID 24 STYLE 4}
	button{ COMMAND 3008 ID 25 STYLE 4}
	button{ COMMAND 3010 ID 32 STYLE 4} */

 Separator (10)

 button{ COMMAND 305 ID 26 STYLE 3 info "Редактирование точек"}
}

range{
ID 12
 name "Видео"
 info "Для работы с видео"
 button{ COMMAND 331 ID 5  info "На начало"}
 button{ COMMAND 330 ID 7  info "Проиграть"}
 button{ COMMAND 333 ID 8  info "Пауза"}
 button{ COMMAND 335 ID 4  info "Стоп"}
 button{ COMMAND 332 ID 9  info "На конец"}
}
range{
ID 14
 name "Bitmap"
 info "Битовый редактор"
 button{ COMMAND 3036 ID 47  style 4 info "пипетка"}
 button{ COMMAND 3028 ID 19  style 4 info "карандаш"}
 button{ COMMAND 3029 ID 20  style 4 info "линия"}
 button{ COMMAND 3030 ID 21  style 4 info "прямоугольник"}
 button{ COMMAND 3031 ID 44  style 4 info "закрашенный прямоугольник"}
 button{ COMMAND 3032 ID 22  style 4 info "эллипс"}
 button{ COMMAND 3033 ID 45  style 4 info "закрашенный эллипс"}
 button{ COMMAND 3034 ID 18  style 4 info "закраска"}
 button{ COMMAND 3035 ID 29  style 4 info "Ножницы"}

 Separator (10)
 button{ COMMAND 363 ID 39 info "Цвет" }

 Separator (10)
 //button{ COMMAND 2265 ID 28  style 3 info "Сетка"}
 Separator (10)
 button{ COMMAND 361  ID 37 info "Увеличить"}
 button{ COMMAND 362  ID 38 info "Уменьшить"}
}


range {
ID 13
  NOARRANGE
  name "Слои"
  info "Слои листа"
  button{  POS 10,1  SRC 217,2,8,8 size 12,12 COMMAND 4036 STYLE 3 info "Слой 16 (Имиджи)"}
  button{  POS 10,13 SRC 217,2,8,8 size 12,12 COMMAND 4037 STYLE 3 info "Слой 17 (Имиджи)"}
  button{  POS 24,1  SRC 217,2,8,8 size 12,12 COMMAND 4038 STYLE 3 info "Слой 18 (Имиджи)"}
  button{  POS 24,13 SRC 217,2,8,8 size 12,12 COMMAND 4039 STYLE 3 info "Слой 19 (Имиджи)"}
  button{  POS 38,1  SRC 217,2,8,8 size 12,12 COMMAND 4040 STYLE 3 info "Слой 20 (Имиджи)"}
  button{  POS 38,13 SRC 217,2,8,8 size 12,12 COMMAND 4041 STYLE 3 info "Слой 21 (Имиджи)"}
  button{  POS 52,1  SRC 217,2,8,8 size 12,12 COMMAND 4042 STYLE 3 info "Слой 22 (Имиджи)"}
  button{  POS 52,13 SRC 217,2,8,8 size 12,12 COMMAND 4043 STYLE 3 info "Слой 23 (Имиджи)"}

  button{  POS 70,1  SRC 217,2,8,8 size 12,12 COMMAND 4044 STYLE 3 info "Слой 24 (Связи)"}
  button{  POS 70,13 SRC 217,2,8,8 size 12,12 COMMAND 4045 STYLE 3 info "Слой 25 (Связи)"}
  button{  POS 84,1  SRC 217,2,8,8 size 12,12 COMMAND 4046 STYLE 3 info "Слой 26 (Связи)"}
  button{  POS 84,13 SRC 217,2,8,8 size 12,12 COMMAND 4047 STYLE 3 info "Слой 27 (Связи)"}
  button{  POS 98,1  SRC 217,2,8,8 size 12,12 COMMAND 4048 STYLE 3 info "Слой 28  (Связи)"}
  button{  POS 98,13 SRC 217,2,8,8 size 12,12 COMMAND 4049 STYLE 3 info "Слой 29  (Связи)"}
  button{  POS 112,1  SRC 217,2,8,8 size 12,12 COMMAND 4050 STYLE 3 info "Слой 30 (Связи)"}
  button{  POS 112,13 SRC 217,2,8,8 size 12,12 COMMAND 4051 STYLE 3 info "Слой 31 (Связи)"}

  button{  POS 150,1  SRC 217,2,8,8 size 12,12 COMMAND 4020 STYLE 3 info "Слой 0 (Графика)"}
  button{  POS 150,13 SRC 217,2,8,8 size 12,12 COMMAND 4021 STYLE 3 info "Слой 1 (Графика)"} 
  button{  POS 164,1  SRC 217,2,8,8 size 12,12 COMMAND 4022 STYLE 3 info "Слой 2 (Графика)"}
  button{  POS 164,13 SRC 217,2,8,8 size 12,12 COMMAND 4023 STYLE 3 info "Слой 3 (Графика)"}
  button{  POS 178,1  SRC 217,2,8,8 size 12,12 COMMAND 4024 STYLE 3 info "Слой 4 (Графика)"}
  button{  POS 178,13 SRC 217,2,8,8 size 12,12 COMMAND 4025 STYLE 3 info "Слой 5 (Графика)"}
  button{  POS 192,1  SRC 217,2,8,8 size 12,12 COMMAND 4026 STYLE 3 info "Слой 6 (Графика)"}
  button{  POS 192,13 SRC 217,2,8,8 size 12,12 COMMAND 4027 STYLE 3 info "Слой 7 (Графика)"}

  button{  POS 210,1  SRC 217,2,8,8 size 12,12 COMMAND 4028 STYLE 3 info "Слой 8 (Графика)"}
  button{  POS 210,13 SRC 217,2,8,8 size 12,12 COMMAND 4029 STYLE 3 info "Слой 9 (Графика)"}
  button{  POS 224,1  SRC 217,2,8,8 size 12,12 COMMAND 4030 STYLE 3 info "Слой 10 (Графика)"}
  button{  POS 224,13 SRC 217,2,8,8 size 12,12 COMMAND 4031 STYLE 3 info "Слой 11 (Графика)"}
  button{  POS 238,1  SRC 217,2,8,8 size 12,12 COMMAND 4032 STYLE 3 info "Слой 12 (Графика)"}
  button{  POS 238,13 SRC 217,2,8,8 size 12,12 COMMAND 4033 STYLE 3 info "Слой 13 (Графика)"}
  button{  POS 252,1  SRC 217,2,8,8 size 12,12 COMMAND 4034 STYLE 3 info "Слой 14 (Графика)"}
  button{  POS 252,13 SRC 217,2,8,8 size 12,12 COMMAND 4035 STYLE 3 info "Слой 15 (Графика)"}

 }
range {
 ID 30
  name "3d"
  info "Работа с трехмерными объектами"

  button{ COMMAND 3046 ID 71 STYLE 4 group 30 info "Перемещение"}
  button{ COMMAND 3047 ID 70 STYLE 4 group 30 info "Размер объекта"}
  button{ COMMAND 3048 ID 72 STYLE 4 group 30 info "Вращение"}

  Separator(6)

  button{ COMMAND 3050 ID 60 STYLE 4 group 30 info "Масштабировать"}
  button{ COMMAND 3051 ID 61 STYLE 4 group 30 info "Поворот камеры"}
  button{ COMMAND 3053 ID 62 STYLE 4 group 30 info "Пермещение точки взгляда"}
  button{ COMMAND 3052 ID 63 STYLE 4 group 30 info "Pan (Смещение)"}

 }
}
