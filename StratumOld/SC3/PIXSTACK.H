#include <windows.h>
class PIX {
public:
	int x,y;
	COLORREF clr;
};

class clPIX {
public:
	clPIX();  //Конструктор
	~clPIX(); //Деструктор
	void Push(PIX ppix);  //Помещалка в стек
	void DeleteNext();//Цепная реакция по удалению всего
	PIX Pop(); //Выталкивание из стека
   BOOL Empty(); //Проверка: не пуст ли стек. Если пуст, то TRUE
private:
	PIX pix;
	clPIX * next;
};
