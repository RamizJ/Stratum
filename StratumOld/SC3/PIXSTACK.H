#include <windows.h>
class PIX {
public:
	int x,y;
	COLORREF clr;
};

class clPIX {
public:
	clPIX();  //�����������
	~clPIX(); //����������
	void Push(PIX ppix);  //��������� � ����
	void DeleteNext();//������ ������� �� �������� �����
	PIX Pop(); //������������ �� �����
   BOOL Empty(); //��������: �� ���� �� ����. ���� ����, �� TRUE
private:
	PIX pix;
	clPIX * next;
};
