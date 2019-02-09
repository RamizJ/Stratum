/*
Прекомпилятор
* удаяет комментарии и переводы строк
*/
class TTplReader{
 char * input;
 char * _input; //текущий указатель
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
// Загрузка
};


