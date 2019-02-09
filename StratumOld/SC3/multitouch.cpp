#pragma once
#include "multitouch.h"

typedef struct _GESTURECONFIG {
  DWORD dwID;
  DWORD dwWant;
  DWORD dwBlock;
} GESTURECONFIG, *PGESTURECONFIG;

#define GID_BEGIN					1	//A gesture is starting.
#define GID_END						2	//A gesture is ending.
#define GID_ZOOM					3	//Indicates configuration settings for the zoom gesture.
#define GID_PAN						4	//Indicates the pan gesture.
#define GID_ROTATE				5	//Indicates the rotation gesture.
#define GID_TWOFINGERTAP	6	//Indicates the two-finger tap gesture.
#define GID_PRESSANDTAP		7 //Indicates the press and tap gesture.

#define GC_ALLGESTURES			0x00000001	//Indicates all of the gestures.
#define GC_ZOOM							0x00000001	//Indicates the zoom gesture.
#define GC_PAN							0x00000001	//Indicates all pan gestures.
#define GC_PAN_WITH_SINGLE_FINGER_VERTICALLY	0x00000002	//Indicates vertical pans with one finger.
#define GC_PAN_WITH_SINGLE_FINGER_HORIZONTALLY	0x00000004	//Indicates horizontal pans with one finger.
#define GC_PAN_WITH_GUTTER	0x00000008	//Limits perpendicular movement to primary direction until a threshold is reached to break out of the gutter.
#define GC_PAN_WITH_INERTIA	0x00000010	//Indicates panning with inertia to smoothly slow when pan gestures stop.
#define GC_ROTATE						0x00000001	//Indicates the rotation gesture.
#define GC_TWOFINGERTAP			0x00000001	//Indicates the two-finger tap gesture.
#define GC_PRESSANDTAP			0x00000001	//Indicates the press and tap gesture.

typedef struct _GESTUREINFO {
  UINT      cbSize;
  DWORD     dwFlags;
  DWORD     dwID;
  HWND      hwndTarget;
  POINTS    ptsLocation;
  DWORD     dwInstanceID;
  DWORD     dwSequenceID;
  ULONG			ullArguments;
  UINT      cbExtraArgs;
} GESTUREINFO, *PGESTUREINFO;
DECLARE_HANDLE(HGESTUREINFO);


#define GF_BEGIN		0x00000001	//A gesture is starting.
#define GF_INERTIA	0x00000002	//A gesture has triggered inertia.
#define GF_END			0x00000004	//A gesture has finished.

LONG _stdcall (*RegisterTouchWindow)(
	HWND hWnd,
  ULONG ulFlags
);

BOOL _stdcall (*SetGestureConfig)(
  HWND hwnd,
  DWORD dwReserved,
  UINT cIDs,
  PGESTURECONFIG pGestureConfig,
  UINT cbSize
);
BOOL _stdcall (*GetGestureConfig)(
  HWND hwnd,
  DWORD dwReserved,
  DWORD dwFlags,
  PUINT pcIDs,
  PGESTURECONFIG pGestureConfig,
  UINT cbSize
);

BOOL _stdcall (*GetGestureInfo)(
  HGESTUREINFO hGestureInfo,
  PGESTUREINFO pGestureInfo
);

BOOL _stdcall (*CloseGestureInfoHandle)(
    HGESTUREINFO hGestureInfo
);

template<class T>
void LoadFunction(HMODULE Module, T& a_Function, const char* a_Name)
{
	a_Function = (T)GetProcAddress(Module, a_Name);
}

bool functionsLoaded=false;
bool LoadMultiTouchFunctions()
{
  if(functionsLoaded)return true;

  OSVERSIONINFO osvi;
  ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  GetVersionEx(&osvi);
  if(osvi.dwMajorVersion < 6  || osvi.dwMajorVersion == 6 && osvi.dwMinorVersion < 1)//it windows 7 or later
  	return false;

 	HMODULE m = GetModuleHandle("user32.dll");

  LoadFunction(m,RegisterTouchWindow,"RegisterTouchWindow");
  if(!RegisterTouchWindow) return false;
  LoadFunction(m,SetGestureConfig,"SetGestureConfig");
  if(!SetGestureConfig) return false;
  LoadFunction(m,GetGestureConfig,"GetGestureConfig");
  if(!GetGestureConfig) return false;
  LoadFunction(m,GetGestureInfo,"GetGestureInfo");
  if(!GetGestureInfo) return false;
  LoadFunction(m,CloseGestureInfoHandle,"CloseGestureInfoHandle");
  if(!CloseGestureInfoHandle) return false;

  functionsLoaded = true;
  return functionsLoaded;
}

LONG RegistrationWindowOnMultitouch(HWND hWnd)
{
  if(!LoadMultiTouchFunctions())return -1;
	if(!RegisterTouchWindow(hWnd, 0))
  {
   	//MessageBox(0, "Cannot register application window for touch input", "Error", 0);
    return -2;
  }

	GESTURECONFIG gestureConfig;
  gestureConfig.dwID = 0;
	gestureConfig.dwBlock = 0;
  gestureConfig.dwWant = GC_ALLGESTURES;

  BOOL result = SetGestureConfig(hWnd, 0, 1, &gestureConfig, sizeof(gestureConfig));

 	//MessageBox(0, "Success register application window for touch input", "Success", 0);
  return 1;
}
