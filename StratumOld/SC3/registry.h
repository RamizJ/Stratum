// для работы с регистри
BOOL GetValueData(char *szSubKey,char *szValueName,char*value,int size);
BOOL SetValueData(char *szSubKey,char *szValueName,char *szValueData);
#ifdef _SC_WINREG_
BOOL SetValueData(HKEY   hTopKey,char *szSubKey,char *szValueName,char *szValueData);
BOOL GetValueData(HKEY   hTopKey,char *szSubKey,char *szValueName,char*value,int size);
#endif
