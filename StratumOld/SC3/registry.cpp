
#undef _WINREG_
#include <windows.h>
#define reg_path "Software\\Stratum Modeling Group\\Stratum 2000"

BOOL GetValueData(HKEY hTopKey,char *szSubKey,char *szValueName,char*value,int size){
   DWORD  dwDataSize = size;
   HKEY   hKey;
   BOOL   result=FALSE;
   if (hTopKey){
   if (RegOpenKeyEx(hTopKey, szSubKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS){

      // Get the size of the value data.
      if (RegQueryValueEx(hKey, szValueName, NULL, NULL, NULL,
                          &dwDataSize) == ERROR_SUCCESS){
         // Get the value data.
         RegQueryValueEx(hKey, szValueName, NULL, NULL,value, &dwDataSize);
         result=TRUE;
      }
   }
   RegCloseKey(hKey);
   }
   return result;
}
BOOL GetValueData(char *szSubKey,char *szValueName,char*value,int size){
  char s[260];
  wsprintf(s,"%s\\%s",reg_path,szSubKey);
  return GetValueData(HKEY_LOCAL_MACHINE,s,szValueName,value,size);
}
BOOL SetValueData(HKEY   hTopKey,char *szSubKey,char *szValueName,char *szValueData)
{
   HKEY   hKey;
   LONG   lResult;
   BOOL   bSuccess=0;
   DWORD  disp;
   if (RegCreateKeyEx(hTopKey, szSubKey, 0, "", REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS, NULL, &hKey, &disp) == ERROR_SUCCESS){
      lResult = RegSetValueEx(hKey, szValueName, 0, REG_SZ, (LPBYTE)szValueData,
                              strlen(szValueData) + 1);
      bSuccess = (lResult == ERROR_SUCCESS ? TRUE : FALSE);
   }
   RegCloseKey(hKey);
   return bSuccess;
}
BOOL SetValueData(char *szSubKey,char *szValueName,char *szValueData){
  char s[260];
  wsprintf(s,"%s\\%s",reg_path,szSubKey);
  return SetValueData(HKEY_LOCAL_MACHINE,s,szValueName,szValueData);
}

