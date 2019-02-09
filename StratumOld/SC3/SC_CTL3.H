/*

   Функции подключаемые из CTL3DV2.dll   

*/
#ifdef WIN32
extern BOOL (WINAPI *  Ctl3dColorChange)(void);    //!
extern BOOL (WINAPI *  Ctl3dAutoSubclass)(HANDLE);  //|
extern BOOL (WINAPI *  Ctl3dRegister)(HANDLE);      //!
extern BOOL (WINAPI *  Ctl3dUnregister)(HANDLE);    //!

#else
extern BOOL (far _pascal *  Ctl3dColorChange)(void);    //!
extern BOOL (far _pascal *  Ctl3dAutoSubclass)(HANDLE);  //|
extern BOOL (far _pascal *  Ctl3dRegister)(HANDLE);      //!
extern BOOL (far _pascal *  Ctl3dUnregister)(HANDLE);    //!
#endif
