#ifdef __DLL__
#include <windows.h>
#include <stratum\sccall.h>
#endif

HINSTANCE hcore;
#ifdef __DLL__
extern "C" HINSTANCE _SDKPROC ldprocedure(char*path,int code)
#else
HINSTANCE  ldprocedure(char*path, int code)
#endif
{
    switch(code)
    {
        case 1:
        {
            char s[256];
            lstrcpy(s,path);
            lstrcat(s,"\\");
#ifdef WIN32
            lstrcat(s,"sccore32.dll");
#else
            lstrcat(s,"sccore16.dll");
#endif

            hcore=LoadLibrary(s);

#ifndef WIN32
            if(hcore<HINSTANCE_ERROR)hcore=0;
#endif
            return hcore;
        }
        case 2:
        {
            if(hcore)
            {
                FreeLibrary(hcore);
                hcore = 0;
            }
            return hcore;
        }
    }
    return NULL;
};

#ifdef __DLL__

#if defined(WIN32)
//#pragma argsused
BOOL WINAPI DllEntryPoint( HINSTANCE hinstDll,
                           DWORD fdwRreason,
                           LPVOID plvReserved)
#else /* not flat model  */
int FAR _pascal LibMain( HINSTANCE _hInstance,
                         WORD ,
                         WORD wHeapSize,
                         LPSTR  )
#endif /* __FLAT */
{
#ifdef  WIN32
    switch(fdwRreason)
    {
        case DLL_PROCESS_ATTACH:
        {
            return TRUE;// Indicate that the DLL was initialized successfully.
        }
        case DLL_PROCESS_DETACH:
        {
            ldprocedure(NULL,2);
            return TRUE;
        }
    }
    return FALSE;
#else
    if ( wHeapSize != 0 ) UnlockData( 0 );
    return TRUE;
#endif
}

#if !defined(WIN32)
int CALLBACK WEP(int nExitType)
{
    ldprocedure(2);
    return 1;
}
#endif
#endif // __DLL__
