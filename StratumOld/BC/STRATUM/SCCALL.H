#ifndef _SCCALL_H_
#define _SCCALL_H_
#ifndef DLL
#ifdef __DLL__
#define DLL
#endif
#endif
#ifdef WIN32
#define _SDKPROC __export __stdcall
#define SDKPROC WINAPI
#else
#define _SDKPROC far __export _pascal
#define SDKPROC WINAPI
#endif

#ifndef scCALLBACK
#define scCALLBACK CALLBACK
#ifdef DLL
#ifndef WIN32
#undef scCALLBACK
#define scCALLBACK _export CALLBACK
#endif
#endif
#endif

#ifdef WIN32
   #ifdef STRATUM_INTERNAL
     #define _SCEXPCLASS _export
   #else
     #define _SCEXPCLASS _import
   #endif
#else
   #define _SCEXPCLASS
#endif
#endif
