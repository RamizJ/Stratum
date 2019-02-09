int test1(char* pass);
int test2(char* pass);
int test3(char* pass,char* key);
int test4(char* p,char* key);
int test5(char* p,char* key);
int _test5(char* p,char* key);

BOOL IsRegistered();
BOOL IsRegistered2();
BOOL IsRegistered3();
BOOL IsRegistered4();
BOOL IsRegistered5();

BOOL IsCodeValid();    // уже все дожно быть загруженно
BYTE extern registed_copy;
char extern secondcode[13];
void GetSecondCode();  // достает из регистри в заданную переменную
void GetSecondCode2(char*);


