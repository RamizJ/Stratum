HWND GetClassScheme(STRING)
// Получить дескриптор пространства заданного класса
STRING  GetParentClass()
// Получить изображение заданного класса
HWND OpenClassImage(STRING)
// Открыть окно содержащее изображение заданного класса
HWND OpenClassScheme(STRING)
// Открыть окно содержащее схему заданного класса
HWND OpenSpaceWindow(STRING,STRING);
// Создать окно пространства

CloseWindow(HWND)
// Закрыть окно
SetWindowText(HWND,STRING)
// Установить текст окна
STRING GetWindowText(HWND)
// Получить текст окна
CloseAllWindows();
// Закрыть все MDI окна
CloseIDEWindows();
// Закрыть все MDI окна не открытые самой программой

HSP2D GetObjectSpace(FLOAT load);
// Получить дескриптор пространнста схемы класса
HSP2D GetParentSpace(FLOAT load);
// Получить дескриптор пространнста схемы родителя данного класса
INTEGER CreateObject2dFromFile(HSP2D hSpace,FLOAT x,FLOAT y,STRING);
// Создает объкт из VDR файла и помещает его в пространство
DeleteObject2d(HSP2D hSpace,INTEGER);

INTEGER GetObjectHANDLE();
// Возвращает дескриптор объекта на схеме родителя
MoveObject2d(HSP2D hSpace,INTEGER obj,FLOAT x,FLOAT y);
AddOscilographPoint(HSP2D hSpace,INTEGER obj,FLOAT maxpoint,FLOAT x,FLOAT y)
