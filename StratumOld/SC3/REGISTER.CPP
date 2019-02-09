#ifdef WIN32
#undef _WINREG_
#define _SC_WINREG_
#endif
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <gwin2d3d\gwintype.h>
#include <stratum\sc_msg.h>
#include <stratum\stratum.h>
#include "registry.h"
#include "register.rh"
#include "msgcrk.h"
#include "shellapi.h"
#define TO_CREATE         OF_SHARE_DENY_WRITE
BOOL rfind=0;
char *strings[]={
    "\xd1\xde\xc1\xdb\xd6\xce\xe9\xd3\xa6\xa6\xa7\xd4\xab\xd3\xa2\xbf\xa7\xa1\xab\xa1\xbf\xa3\xa3\xd6\xa0\xbf\xab\xa4\xd4\xd4\xbf\xa2\xa2\xd1\xa2\xa4\xd1\xa7\xaa\xa3\xa6\xab\xa5\xef\xce\xdb\xfc\xc2\xe0\xfd\xf1\xc1\xf7\xe0\xe4\xf7\xe0\xa1\xa0\x92",
    "\xe2\xed\xf2\xe8\xe5\xfd\xda\xe0\x95\x95\x94\xe7\x98\xe0\x91\x8c\x94\x92\x98\x92\x8c\x90\x90\xe5\x93\x8c\x98\x97\xe7\xe7\x8c\x91\x91\xe2\x91\x97\xe2\x94\x99\x90\x95\x98\x96\xdc\xfd\xe8\xcc\xd1\xcd\xc4\xcc\xc4\xcf\xd5\xc4\xc5\x81\xe2\xc0\xd5\xc4\xc6\xce\xd3\xc8\xc4\xd2\xa1",
    "\xbb\xb4\xab\xb1\xbc\xa4\x83\xbb\xc0\xbd\xc0\xcd\xcf\xcf\xc8\xd5\xcd\xcd\xca\xc0\xd5\xc9\xc9\xbc\xca\xd5\xc1\xcf\xc8\xca\xd5\xc8\xc8\xbb\xc8\xce\xbb\xcd\xc0\xc9\xcc\xc1\xcf\x85\xf8",
    "\x91\xb0\xb9\xa1\xb4\x98\xba\xbb\xbc\xbe\xb0\xa7\xd5",
    "\xa6\xa9\xb6\xac\xa1\xb9\x9e\xa6\xdd\xa0\xdd\xd0\xd2\xd2\xd5\xc8\xd0\xd0\xd7\xdd\xc8\xd4\xd4\xa1\xd7\xc8\xdc\xd2\xd5\xd7\xc8\xd5\xd5\xa6\xd5\xd3\xa6\xd0\xdd\xd4\xd1\xdc\xd2\x98\xb9\xb3\x80\x97\x96\x8c\x8a\x8b\xe5",
    "\xfc\xc6\xb3\xb3\xb2\xc1\xbe\xc6\xb6\xaa\xb2\xb4\xbe\xb4\xaa\xb6\xb6\xc3\xb5\xaa\xbe\xb1\xc1\xc1\xaa\x87"
};
/*
char *_strings[]={
"CLSID\\{A445F9A0-5393-11D2-96FF-00C06C581497}\\InProcServer32",
"CLSID\\{A445F9A0-5393-11D2-96FF-00C06C581497}\\Implemented Categories",
"CLSID\\{C8E85770-5528-11D2-9702-00C06C581497}",
"DeltaMoniker",
"CLSID\\{C8E85770-5528-11D2-9702-00C06C581497}\\Version", [4]
"{A445F9A1-5393-11D2-96FF-"
};
*/
BYTE xor_codes[]={0x92,0xa1,0xf8,0xd5,0xe5,0x87};
int codepage;

HINSTANCE hInstance;
HWND _hwnd;
int build;
char * regfile;
struct REGent{
    int id;
    char * name;
};
#define MyGetDlgItemText(hwnd,IDC,s,len)\
    SendMessage(GetDlgItem(hwnd,IDC),WM_GETTEXT,len,(LPARAM)s)
REGent re[]={{IDC_USERNAME    ,"name" },
             {IDC_ADRESS      ,"addr" },
             {IDC_ORGANISATION,"org"  },
             {IDC_TELEPHON    ,"phone"},
             {IDC_EMAIL       ,"email"},
             {IDC_SERIAL2     ,"code" },
             {IDC_SERIAL3     ,"code" }
            };

char* user_id_names[]={"user_name","user_org","user_addr","user_phone","user_email"};

// первая цифра - младшая цифра в сумме средних шести как ASCII кодов
// последня младшая цифра в сумме первых семи цифр поксоренных на их порядковый номер
BOOL IsSerial(char*s1,char *s2){
    char s[64];
    wsprintf(s,"03%04d",build);
    if(!lstrcmp(s1,s) && (lstrlen(s2)==8)){
        long c=0;
        for(int i=1;i<7;i++)c+=s2[i];
        if(c%10==s2[0]-'0'){
            c=0;
            for(i=0;i<7;i++)c+=(s2[i]-'0')^i;
            return (c%10==s2[7]-'0');
        }
    }
    return FALSE;
};
BOOL IsSerial3(char*s){
    if(lstrlen(s)==12){
        for(int i=0;i<12;i++)if(s[i]<'0'||s[i]>'9')return FALSE;
        return TRUE;
    }
    return FALSE;
}
void AllTrim(char *in){
    char *_in=in;
    while(*in && *in==' ')in++;
    if(*in){
        char *ps=in;while(*ps)ps++;ps--;
        while(*ps==' ' && ps>in){*ps=0;ps--;}
        if(in>_in && *in){
            while(*in){*_in=*in;_in++;in++;}
            *_in=0;
        }
        return;
    }
    *_in=0;
}
char URL[512]="";
void GetUrl(){
    if(URL[0])return;

#ifdef WIN32
    {
        char path[256];
        GetTempPath(sizeof(path),path);
        GetTempFileNameA(path,"tstuuuu.url",0,URL );
    }
#else
    GetTempFileName(0, "tstuuuu.url", 0,URL );
#endif
    char *_ps=URL;
    while (*_ps && *_ps!='.'){
        _ps++;
    }
    if(*_ps=='.'){
        *_ps=0;
        lstrcat(_ps,".url");
    }
};
BOOL addregdata(HWND hwnd,int id,char *out){
    char *_uns=" ?&%+";
    char s[256];
    GetDlgItemText(hwnd,id,s,sizeof(s));
    AllTrim(s);
    int l=lstrlen(s);
    if(l){
        char *_ids=NULL;
        switch(id){
            case IDC_USERNAME:    _ids="name";break;
            case IDC_ORGANISATION:_ids="firm";break;
            case IDC_ADRESS:      _ids="address";break;
            case IDC_EMAIL:       _ids="mail";break;
            case IDC_TELEPHON:    _ids="fax";break;
            case IDC_SERIAL2:     _ids="number";break;
            case IDC_SERIAL1:     _ids="ID";break;
        }
        int k=lstrlen(out);
        if(_ids){
            if(out[k-1]!='?')lstrcat(out,"&");
            lstrcat(out,_ids);lstrcat(out,"=");
            int k=lstrlen(out);out+=k;
            char *ps=s;
            while(*ps){
                char *_ps=_uns;
                BOOL valid=TRUE;
                while(*_ps){if(*_ps==*ps){valid=FALSE;break;}else _ps++;}
                if(valid){*out=*ps;out++;*out=0;}else{
                    wsprintf(out,"%%%02x",(int)*ps);
                    out+=lstrlen(out);
                }
                ps++;
            }
        }
    }
    return FALSE;
}

extern "C"{
UINT32 SDKPROC SCCoreProc(UINT16  msg,UINT32 p1,UINT32 p2);
}
BOOL scCALLBACK RegisterProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    //_STDHELP(Project_creating_Dialog)
    HWND  hwndCtl;
    INT16 wNotifyCode;
    INT16 command;

    switch(msg){
        case WM_ACTIVATE:{
            char d[64];
            GetDlgItemText(hwnd,IDC_SERIAL3,d,20);
            DWORD id=GetWindowThreadProcessId( hwnd,NULL );
            GetWindowText(GetDlgItem(hwnd,IDC_SERIAL3),d+12,20);
        }break;
        case WM_INITDIALOG:{
            SCMessage("**");
            TSTRATUMVERSION _info;
            _info.size=sizeof(_info);
            //   SCVersion(&_info);
            SCCoreProc(SC_GETVERSION,(UINT32)&_info,0);
            build=_info.build;
            char s[256];
            wsprintf(s,"%s build ( %d )",_info.internalname,_info.build);
            SetDlgItemText(hwnd,IDC_INFO,s);
#ifdef WIN32
            OSVERSIONINFO osv;
            osv.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
            GetVersionEx(&osv);

#endif
            for(int i=0;i<sizeof(re)/sizeof(re[0]);i++){
                char ss[255]="";
#ifdef WIN32
                if(osv.dwPlatformId==VER_PLATFORM_WIN32_NT){
                    switch(i){
                        case 0:GetValueData(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion","RegisteredOwner",ss,sizeof(ss));break;
                        case 2:GetValueData(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion","RegisteredOrganization",ss,sizeof(ss));break;

                    }
                }else
                    switch(i){
                        case 0:GetValueData(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion","RegisteredOwner",ss,sizeof(ss));break;
                        case 2:GetValueData(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion","RegisteredOrganization",ss,sizeof(ss));break;
                    }
                if(i==4){
                    GetValueData(HKEY_CURRENT_USER,"Software\\Microsoft\\Internet Mail and News\\Mail","Sender EMail",ss,sizeof(ss));
                    if(lstrlen(ss)==0){
                        GetValueData(HKEY_CURRENT_USER,"Software\\Microsoft\\Internet Account Manager","Default Mail Account",ss,sizeof(ss));

                        if(lstrlen(ss)){
                            char p[256];
                            wsprintf(p,"Software\\Microsoft\\Internet Account Manager\\Accounts\\%s",ss);
                            *ss=0;
                            GetValueData(HKEY_CURRENT_USER,p,"SMTP Email Address",ss,sizeof(ss));
                        }
                    }
                }
#endif
                if(lstrlen(ss)==0){
                    GetPrivateProfileString("reg",re[i].name,ss,ss,sizeof(ss),regfile);
                    if(lstrlen(ss)==0){
                        SCGetScVarS(user_id_names[i],ss);
                    }

                }
                SetDlgItemText(hwnd,re[i].id,ss);
            }

        };return TRUE;
        case WM_COMMAND:{
            EV_COMMANDnoVar
                    switch (command){
        case IDC_ONLINE:{ // переходим на страницу ONLINE регистрации

                    char szBuf[512];
                    GetUrl();

                    UINT _HANDLE = _lcreat(URL,TO_CREATE);
                    char *_ps="[InternetShortcut]\n\r";
                    _lwrite(_HANDLE,_ps,lstrlen(_ps));
                    lstrcpy(szBuf,"URL=http://stratum.pstu.ac.ru/s2000/reg1.pl?");
                    char s[256];
                    addregdata(hwnd,IDC_USERNAME,szBuf);
                    addregdata(hwnd,IDC_ORGANISATION,szBuf);
                    addregdata(hwnd,IDC_EMAIL,szBuf);
                    addregdata(hwnd,IDC_TELEPHON,szBuf);
                    addregdata(hwnd,IDC_ADRESS,szBuf);
                    addregdata(hwnd,IDC_SERIAL1,szBuf);
                    addregdata(hwnd,IDC_SERIAL2,szBuf);
                    _lwrite(_HANDLE,szBuf,lstrlen(szBuf));
                    _lclose(_HANDLE);
                    ShellExecute(hwnd,"open",URL,NULL,NULL,SW_SHOWDEFAULT);
                }break;
                case IDC_USERNAME:
                    //    case IDC_ORGANISATION:
                case IDC_ADRESS:
                    //    case IDC_TELEPHON:
                case IDC_EMAIL:
                    //	 case IDC_SERIAL1:
                    //    case IDC_SERIAL2:
                case IDC_SERIAL3:
                    if(wNotifyCode==EN_UPDATE){
                        char ss1[256];char ss2[256];

                        BOOL b=TRUE;
                        GetDlgItemText(hwnd,IDC_USERNAME,ss1,sizeof(ss1));
                        if(lstrlen(ss1)<1)b=FALSE;

                        GetDlgItemText(hwnd,IDC_EMAIL,ss1,sizeof(ss1));
                        if(lstrlen(ss1)<1)b=FALSE;
                        char *ps=ss1;
                        while(*ps){if(*ps=='@')break;ps++;}
                        if(*ps==0)b=FALSE;
                        EnableWindow(GetDlgItem(hwnd,IDC_SERIAL3),b);

                        if(b){
                            GetDlgItemText(hwnd,IDC_SERIAL3,ss1,sizeof(ss1));
                            if(!IsSerial3(ss1))b=FALSE;
                        }
                        EnableWindow(GetDlgItem(hwnd,IDOK),b);

                    };return TRUE;
                case IDOK:{
                    {
                        char d[20];
                        GetDlgItemText(hwnd,IDC_SERIAL3,d,13);
                        if(lstrlen(d)!=12)return FALSE;
                        //    HWND f=GetFocus();
                        //    if(f!=hwndCtl)return FALSE;
                    }
                    char ss1[256];
                    MyGetDlgItemText(hwnd,IDC_USERNAME,ss1,sizeof(ss1));
                    SetValueData("Register","Name",ss1);
                    SCSetScVarS("user_name",ss1);
                    MyGetDlgItemText(hwnd,IDC_EMAIL,ss1,sizeof(ss1));
                    SetValueData("Register","EMail",ss1);
                    SCSetScVarS("user_email",ss1);
                    {
                        for(int i=0;i<6;i++){
                            char *_ps=strings[i];
                            while(1){
                                *_ps=((BYTE)(*_ps))^(xor_codes[i]);
                                if(((BYTE)(*_ps))==xor_codes[i])goto m2;
                                _ps++;
                            }
m2:
                        }
                    }
                    char ss[256];
                    lstrcpy(ss,strings[5]);
                    char d[64];
                    MyGetDlgItemText(hwnd,IDC_SERIAL3,d,13);
                    //SendMessage(GetDlgItem(hwnd,IDC_SERIAL3),WM_GETTEXT,13,(LPARAM)d);
                    {
                        {
                            char d1[64];
                            for(int i=0;i<12;i++){
                                d1[i*3]='Z'-(d[i]-'0');
                                d1[i*3+1]='A'+random(20);
                                d1[i*3+2]='A'+random(20);
                            }
                            d1[36]=0;
                            SetValueData(HKEY_CLASSES_ROOT,strings[4],"",d1);
                        }
                        char *_ps=d;
                        int i=1;
                        while(*_ps){
                            *_ps=*_ps+(i%6);
                            if(*_ps>'9')*_ps+=('A'-'9');
                            _ps++;
                            i++;
                        }}
                    lstrcat(ss,d);
                    lstrcat(ss,"}");
                    SetValueData(HKEY_CLASSES_ROOT,strings[0],"","OLE32.DLL");
                    SetValueData(HKEY_CLASSES_ROOT,strings[1],"",ss);
                    SetValueData(HKEY_CLASSES_ROOT,strings[2],"",strings[3]);

                    rfind=1;
                    EndDialog(hwnd,TRUE);return TRUE;}
                case IDCANCEL:rfind=1; EndDialog(hwnd,0);return TRUE;
            }}break;
    }
    return FALSE;
}

BOOL _SDKPROC Register(char * file){
    int rez=0;
    regfile=file;
    char * dlg=MAKEINTRESOURCE(codepage);
    if(DialogBox(hInstance,dlg,_hwnd,RegisterProc)){
        rez=TRUE;
    }
    if(!rfind){
        dlg=MAKEINTRESOURCE(1252);
        if(DialogBox(hInstance,dlg,_hwnd,RegisterProc)){
            rez=TRUE;
        }
    }

    //MessageBox(_hwnd,"This program is unregistred","Error",MB_OK|MB_ICONINFORMATION);
    return rez;
};



extern "C" UINT32 ScDllProc(UINT16 code,UINT32 p1,UINT32 p2)
{
    switch(code)
    {
        case SC_GETPROCADDRESS:
        {
            char *name=(char *)p1;
            if(!lstrcmpi(name,"ScRegister"))
                return (DWORD)Register;
        }return NULL;

        case SC_INITLIBRARY:
        {
            TSTRATUMINFO*sci=(TSTRATUMINFO*)p1;
            hInstance=sci->hModule;
            _hwnd=sci->hMainWindow;
            codepage=sci->codepage;
        }return TRUE;

        case SC_DONELIBRARY:
        {
            if(URL[0])
            {
                DeleteFile(URL);
            }break;
        }
    }
    return 0;
};


