#ifdef WIN32
#define _EXPORT __export __stdcall
#else
#define _EXPORT far __export _pascal
#endif
#define _EXTERNC extern "C"
