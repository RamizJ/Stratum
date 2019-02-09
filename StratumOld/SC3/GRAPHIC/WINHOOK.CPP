/*
Copyright (c) 1995  Virtual World
Module Name:
	 object2d.cpp
Author:
	 Alexander Shelemekhov
*/

#include <dos.h>
#include <windows.h>
#define ____WIN_HOOK__H_EXT_
#include <gwin2d3d\winhook.h>
//#include <mem.h>
#ifndef MK_FP
#define MK_FP(seg,ofs) (void*)((seg)<<16|(ofs))
#endif

#ifdef WIN32
#include <stddef.h>
#endif

typedef  WORD uint16;
typedef  WORD uint;
typedef  DWORD uint32;
typedef  BYTE  uint8;

typedef struct tagHOOK{
WNDHOOKPROC winhook;
WNDPROC     defproc;
t_pointer   data;
HWND        hwnd;
} THOOK;
BOOL FreeInstanceThunk(WNDPROC p);
class TProcInstance {
  public:
	 #if defined(BI_PLAT_WIN16)
		TProcInstance(FARPROC p) {Instance = ::MakeProcInstance(p, _hInstance);}
	  ~TProcInstance() {::FreeProcInstance(Instance);}
	 #else
		TProcInstance(FARPROC p) {Instance = FARPROC(p);}
	 #endif

	 operator FARPROC() {return Instance;}

  private:
	 FARPROC Instance;
};

#ifdef WIN32

LRESULT CALLBACK StdWndProcNT(HWND hwnd,UINT msg, WPARAM wParam, LPARAM lParam)
{
BOOL calldef;
 LRESULT a;
 THOOK*thook=((THOOK*)_EAX);
 a=thook->winhook(thook->data,hwnd,(UINT)msg,wParam,lParam,calldef);
 if (calldef) a=thook->defproc(hwnd,msg,wParam,lParam);
 if (msg==WM_DESTROY) FreeInstanceThunk((WNDPROC)GetWindowLong(hwnd,GWL_WNDPROC));

  return a;
};

LRESULT CALLBACK StdWndProc95(HWND hwnd,UINT msg, WPARAM wParam, LPARAM lParam)
{
BOOL calldef;
 LRESULT a;
 THOOK*thook=((THOOK*)_EAX);

  a=thook->winhook(thook->data,hwnd,(UINT)msg,wParam,lParam,calldef);

  if (calldef){
    LONG old=SetWindowLong(hwnd,GWL_WNDPROC, (uint32)(thook->defproc));
    a=SendMessage(hwnd,msg, wParam, lParam);
    SetWindowLong(hwnd,GWL_WNDPROC,old);
  }

  // a=thook->defproc(hwnd,msg,wParam,lParam);

 if (msg==WM_DESTROY) FreeInstanceThunk((WNDPROC)GetWindowLong(hwnd,GWL_WNDPROC));
  return a;
};


#else

LRESULT FAR PASCAL StdWndProc(HWND hwnd, uint msg, WPARAM wParam, LPARAM lParam)
{
 BOOL calldef;
 LRESULT a;
 WNDPROC defproc;
 asm {pusha
	  push es
	  push ds
	  push si
	  push di
	  pushf
//	  mov ax,0
	  }
 THOOK*thook=((THOOK*)MK_FP(_ES,_BX));
 a=thook->winhook(thook->data,hwnd,(UINT)msg,wParam,lParam,calldef);
 defproc=thook->defproc;
 if (msg==WM_DESTROY) FreeInstanceThunk((WNDPROC)GetWindowLong(hwnd,GWL_WNDPROC));

  asm {popf
		 pop di
		 pop si
		 pop ds
		 pop es
		 popa
  }
 if (calldef) a=defproc(hwnd,msg,wParam,lParam);
  return a;
}

TProcInstance StdWndProcInstance((FARPROC)StdWndProc);

#endif



//
// Win16 version of the instance thunking mechanism
//

//
// AllocCSToDSAlias was not declared in windows.h
//

#ifdef WIN32
struct TInstanceThunk {
  uint8      Call;
  int        Offset;
  WNDPROC    Proc;
  union Ptr {    //for compatible
  THOOK *    thook;
  void  *    dataEx;
  }ptr;
  uint8      Code[6];
};
#else
extern "C" uint16 FAR PASCAL AllocCSToDSAlias(uint16);
struct TInstanceThunk {
  char   Code;
  uint16 Offset;
  union Ptr {
	 struct TInstanceThunk far* Next;
	 THOOK *  thook;
  } ptr;
};
struct TThunkBlock {
  uint16    Next;
  char      Code[5];
  void far* WndProcPtr;
  struct TInstanceThunk Thunks[34];
};
static uint16              ThunkBlockList = 0;
TInstanceThunk far* ThunkFreeList = 0;
#endif


#ifdef WIN32
static const int  CODEDISP = offsetof(TInstanceThunk, Code) -
							 offsetof(TInstanceThunk, Proc);

WNDPROC
CreateInstanceThunk(THOOK* thook)
{
  TInstanceThunk*  thunk = new TInstanceThunk;

  thunk->Call = 0xE8u;        // CALL rel32
  thunk->Offset = CODEDISP;   // relative displacement to Code[5]
  thunk->Proc = StdWndProc95;

  // POP ECX
  //
  // pop return address of call into ecx (address of member "Proc")
  //
  thunk->Code[0] = 0x59u;

  // MOV EAX,[ECX+4]
  //
  // load "Window" into ebx
  //
  thunk->Code[1] = 0x8Bu;     // MOV r32,r/m32
  thunk->Code[2] = 0x41u;     // eax,disp8[ECX]
  thunk->Code[3] = 0x04u;     // +4

  // JMP [ECX]
  //
  // jump to window function provided
  //
  thunk->Code[4] = 0xFFu;     // JMP r/m32
  thunk->Code[5] = 0x21u;     // [ECX]
  thunk->ptr.thook = thook;

  return (WNDPROC)thunk;
}

BOOL FreeInstanceThunk(WNDPROC proc)
{
 TInstanceThunk* thunk=(TInstanceThunk*)proc;
 if ((BYTE)thunk->Call == (BYTE)0xE8){
  SetWindowLong(thunk->ptr.thook->hwnd,GWL_WNDPROC, (uint32)(thunk->ptr.thook->defproc));
  delete (thunk->ptr.thook);
  delete (TInstanceThunk*)proc;
  return TRUE;
 }
 return FALSE;
}
#else


WNDPROC
CreateInstanceThunk(THOOK* thook)
{
  char                blockCode[5];
  TThunkBlock far*    block;
  TInstanceThunk far* thunk;
  WNDPROC             objInstance;

  // POP BX
  blockCode[0] = 0x5b;

  // LES BX, CS:[BX]
  blockCode[1] = 0x2e;
  blockCode[2] = 0xc4;
  blockCode[3] = 0x1f;

  // JMP FAR StdWndProc
  blockCode[4] = 0xea;

  if (ThunkFreeList==NULL) {
	 block = (TThunkBlock far*)GlobalLock(GlobalAlloc(GMEM_FIXED | GMEM_SHARE | GMEM_NOT_BANKED,
													  sizeof(TThunkBlock)));
	 block->Next = ThunkBlockList;
	 for(int i=0;i<5;i++)block->Code[i]=blockCode[i];
  //	 memcpy(block->Code, blockCode, 5);
	 block->WndProcPtr = (void far*)StdWndProcInstance;
	 thunk = block->Thunks;

	 do {
		thunk->Code = 0xE8;
		thunk->Offset = (2 - 3) - FP_OFF(thunk);
		thunk->ptr.Next = ThunkFreeList;
		ThunkFreeList = thunk;
		thunk = (TInstanceThunk far*)MK_FP(
						  FP_SEG(thunk),
						  FP_OFF(thunk)+sizeof(TInstanceThunk)
					  );
	 } while (FP_OFF(thunk) != sizeof(TThunkBlock));

	 ThunkBlockList = FP_SEG(block);
	 PrestoChangoSelector(FP_SEG(block), FP_SEG(block));
  }

  objInstance = (WNDPROC)ThunkFreeList;
  thunk = (TInstanceThunk far*)MK_FP(AllocCSToDSAlias(FP_SEG(ThunkFreeList)),
																		FP_OFF(ThunkFreeList));
  ThunkFreeList = thunk->ptr.Next;
  thunk->ptr.thook = thook;
  uint seg=FP_SEG(thunk);
  FreeSelector(seg);
  return objInstance;
}


BOOL _export FreeInstanceThunk(WNDPROC p)
{
  TInstanceThunk far* thunk = (TInstanceThunk far*)MK_FP(AllocCSToDSAlias(FP_SEG(p)),
																		 FP_OFF(p));
  if ((BYTE)thunk->Code == (BYTE)0xE8){

  SetWindowLong(thunk->ptr.thook->hwnd,GWL_WNDPROC, (uint32)(thunk->ptr.thook->defproc));
  delete (thunk->ptr.thook);
  thunk->ptr.Next = ThunkFreeList;
  FreeSelector(FP_SEG(thunk));
  ThunkFreeList = (TInstanceThunk far*)p;

  return TRUE;

  }else{
  FreeSelector(FP_SEG(thunk));
	return FALSE;
  }
}

#endif /// WIN16


WNDPROC FAR PASCAL _export CreateWindowHook(HWND hwnd,WNDHOOKPROC whp,t_pointer data){
//return NULL;
  THOOK *thook=new THOOK;
  WNDPROC thunk=CreateInstanceThunk(thook);
 if (thunk){
  thook->defproc =(WNDPROC)SetWindowLong(hwnd,GWL_WNDPROC,(LONG)thunk);

#ifdef WIN32
  SetWindowLong(hwnd,GWL_USERDATA,(UINT)thunk);
#endif
  thook->winhook=whp;
  thook->hwnd=hwnd;
  thook->data=data;
  return thook->defproc;
 }delete thook;
  return NULL;
};

BOOL FAR PASCAL _export ReleaseWindowHook(HWND hwnd){
 WNDPROC wp=(WNDPROC)GetWindowLong(hwnd,GWL_WNDPROC);
 if (wp) return FreeInstanceThunk(wp);
 return 0;
};

t_pointer FAR PASCAL _export GetWindowData(HWND hwnd){
#ifdef WIN32
 TInstanceThunk far* thunk = (TInstanceThunk*) GetWindowLong(hwnd,GWL_USERDATA);
#else
 TInstanceThunk far* thunk = (TInstanceThunk*)GetWindowLong(hwnd,GWL_WNDPROC);

#endif

if(thunk){

#ifdef WIN32
  if ((BYTE)thunk->Call == (BYTE)0xE8){
#else
  if ((BYTE)thunk->Code == (BYTE)0xE8){
#endif
  return thunk->ptr.thook->data;
 }
 }
 return 0;
};


