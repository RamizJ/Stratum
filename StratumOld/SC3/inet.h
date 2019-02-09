extern int (WINAPI*URLLoadFile)(char *remote,char *local,LONG flags);
extern int (WINAPI*URLDecode)(char *file,char *dir);
BOOL IsURL(char*file);
BOOL InitInet();
