/*
�������������
* ������ ����������� � �������� �����
*/
class TTplReader{
 char * input;
 char * _input; //������� ���������
int Precompile();
public:
// char *  input;

 TTplReader(TStream*s);
 ~TTplReader();
 BOOL ReadInt(int &i);
 BOOL ReadChar(char c);
 BOOL ReadKeyWord(int & code,char**,int size);
 BOOL ReadStr(char *,int _len=250);
 BOOL ReadSpaces();
// ��������
};


