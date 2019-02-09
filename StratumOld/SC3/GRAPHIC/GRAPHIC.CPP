/*
Copyright (c) 1995  Virtual World
Module Name:
	 graphic.cpp
Author:
	 Alexander Shelemekhov
*/

#include <windows.h>
#include <mem.h>
#include "_gr_sys.h"
#include "error_h.h"
//#define CHECKHEAP 1
BYTE done_mode;
HINSTANCE hInstance;
#ifdef CHECKHEAP
void CheckFinalMemoryState();
#include  <stdlib.h>
 extern          long refcount;
 extern unsigned long newcount;
 extern unsigned long memcount;

		  void * cdecl operator new(size_t size);
		  void cdecl operator delete( void* );
#endif
//HGLOBAL testmem;
//HBITMAP testbmp;
#if defined(WIN32)
//#pragma argsused
BOOL WINAPI DllEntryPoint( HINSTANCE hinstDll,
									DWORD fdwRreason,
									LPVOID plvReserved)
#else /* not flat model  */
int FAR _pascal LibMain( HINSTANCE hinstDll,
								WORD ,
								WORD wHeapSize,
								LPSTR  )
#endif /* __FLAT */
{


#ifdef  WIN32
switch(fdwRreason){
 case DLL_PROCESS_ATTACH:{
	 ::hInstance=hinstDll;
	 SetErrorLogFile("graphic.log");
	 InitSystem();
	 done_mode=0xff;
	 return TRUE;
 }
 case DLL_PROCESS_DETACH:{
	done_mode=0;
	DoneSystem();
#ifdef CHECKHEAP
	 CheckFinalMemoryState();
#endif
	return TRUE;
 }
 return FALSE;
}

#else

// The startup code for the DLL initializes the local heap(if there is one)
// with a call to LocalInit which locks the data segment.

	 if ( wHeapSize != 0 )
		  UnlockData( 0 );
	 ::hInstance=hinstDll;
	 SetErrorLogFile("graphic.log");
	 InitSystem();
	 done_mode=0xff;
	 return 1;   // Indicate that the DLL was initialized successfully.

#endif
}

// Turn off warning: Parameter '' is never used
#pragma argsused

#if !defined(WIN32)
int FAR PASCAL WEP ( int bSystemExit )
{  // MessageBox(0,"Unload DLL !","Good !",MB_ICONHAND);
//	 _Error(789);

//	 GlobalUnlock(testmem);
//	 GlobalFree(testmem);
	 done_mode=0;
	 if (bSystemExit==WEP_FREE_DLL)DoneSystem();
#ifdef CHECKHEAP
	 CheckFinalMemoryState();
#endif

	 //return ::WEP(bSystemExit);
	return 1;
}
#endif

#ifdef CHECKHEAP
#define memsinature 798979l
 struct TMemInfo{
  TMemInfo*Next;
  TMemInfo*Prev;
  long     num;
  int      size;
  long signature;
 };
 TMemInfo*first=NULL;
 TMemInfo*last=NULL;
 long refcount = 0;
 unsigned long newcount= 0;
 unsigned long memcount=0;
  void * cdecl operator  new( size_t size) {
					 refcount++;
					 newcount++;
		memcount+=(long)size;
				  TMemInfo*m=(TMemInfo*)malloc(size+sizeof(TMemInfo));
				  memset(m,0xff,size+sizeof(TMemInfo));
				  if (last)last->Next=m;
				  m->Prev=last;
				  m->Next=NULL;
				  m->num=newcount;
				  m->signature=memsinature;
				  m->size=size;
				  last=m;
				  if (!first)first=m;
				  void *r=(((char*)m)+sizeof(TMemInfo));

				  if (m->num==0x29){
					long i=m->num;
					long j=i;
					m->num=j;
				  }
				  return r;
  }
  void cdecl operator  delete( void* addr ) {
		 refcount--;
	//   memcount-=size;
	  if (addr){
	  if ((DWORD)(addr)==0xfffffffful){
  MessageBox(0,"Attempt to delete ffff:ffff pointer",
		"(GRAPHIC)MEMORY CHECK ERROR!",MB_OK|MB_TASKMODAL);
	  }else{
	  TMemInfo*m=(TMemInfo*)((long)(addr)-sizeof(TMemInfo));
	  if (m->signature==memsinature){
	  if (m->Prev)m->Prev->Next=m->Next;
	  if (m->Next)m->Next->Prev=m->Prev;
	  if (last==m){
	  last=m->Prev;
	  }
	  if (first==m)first=m->Next;
		memset(m,0,m->size+sizeof(TMemInfo));
		free(m);
		return ;
		}// signature
		 else{
		MessageBox(0,"Attempt to delete non alloctaed blok!\n(delete without new)",
		"(GRAPHIC)MEMORY CHECK ERROR!",MB_OK|MB_TASKMODAL);
		}}

	  }else{
 MessageBox(0,"Attempt to delete NULL pointer",
		"(GRAPHIC)MEMORY CHECK ERROR!",MB_OK|MB_TASKMODAL);

	  }
 int i=5;
 i++;
 }
 void CheckFinalMemoryState(){

  if (refcount){
  if (!first){
  MessageBox(0,"Memory refernce count dont ZERO but first memory block iz NULL !",
		"(GRAPHIC)MEMORY CHECK ERROR!",MB_OK|MB_ICONSTOP|MB_TASKMODAL);
  }
  };

  HFILE log=_lcreat("c:\\memory.log",OF_SHARE_DENY_WRITE);
 char s[256];
 wsprintf(s,"Total new called : %ld \r\n",newcount);
  _lwrite(log,s,lstrlen(s));

  if (first){
//	MessageBox(0,"Same Memory block dont free correctly!","(GRAPHIC)MEMORY CHECK ERROR!",MB_OK|MB_ICONSTOP|MB_TASKMODAL);

  TMemInfo*m=first;

  wsprintf(s,"******* E R R O R ********\r\nRefcount: %ld\r\n\r\n",refcount);
  _lwrite(log,s,lstrlen(s));

  while (m){
  int size=m->size;

  wsprintf(s,"Num: %ld size: %ld\r\n",m->num,m->size);
  _lwrite(log,s,lstrlen(s));
  m=m->Next;
  }
  }else _lwrite(log,"Ok",2);

  _lclose(log);
 };
#endif

