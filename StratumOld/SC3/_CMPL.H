//лексические классы
enum {optr,opnd,dconst,iconst,hconst,fconst,sconst,eof};

class ID
{
    //istream& is;
    TStream& is;
public:
    ID(TStream&);
    ~ID();

    C_TYPE IsNextChar(unsigned char ch) { return buffer[counter+1] == ch; }
    unsigned char GetNextChar() { return buffer[counter+1]; }
    C_TYPE IsNextOptr(unsigned char ch);
    C_TYPE IsEquation();
    C_TYPE ScanChar(char); //поиск символа с текущей позиции и до конца строки или текста
    C_TYPE Read();//выполняет чтение след.лексемы и возвращает класс определенной лексемы
    C_TYPE IsEOF() {return iseof;}
    void Back() {counter -= lstrlen(str); Type = LastType; } //возвращаемся на одну лексему назад
    C_TYPE Wait(char*); //проверяем является ли аргумент операндом в тексте
    //long GetPos() {return is.GetPos(); };

    C_TYPE SConstLength; //хранит число символов в прочитанной символьной лексеме в ковычках или апострофах
    C_TYPE counter; //хранит текущую позицию курсора в тексте
    C_TYPE Status;  //1-отображать состояние процесса компиляции в строке состояния (текст и индикатор выполнения)
    UINT16 size;   	//размер текста
    C_TYPE Type, LastType; //класс выделенной лексемы
    C_TYPE LastPos;  //позиция курсора перед началом очередного определения лексемы
    C_TYPE iseof;  //1-достигнут конец текста
    C_TYPE Line; //хранит число обработанных строк
    C_TYPE NewLine;
    char 	 str[MAX_STRING];  //хранит найденную лексему
    char*  buffer;	//хранит текст для разбора
};
