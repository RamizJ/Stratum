#pragma once
/*
Copyright (c) 2011
Project: Stratum 3.1
Module :
		  Natural User Interface
Author:
			Marchenko Sergey
*/
#include "_vmach.h"

#define DLL_CALLCONV _cdecl

typedef char* poiter;

class NUI
{
protected:
  HMODULE library;

	int DLL_CALLCONV (*NUI_GetDeviceCount)();
	poiter DLL_CALLCONV (*NUI_GetInstanceByIndex)(int index);
	char* DLL_CALLCONV (*NUI_GetDeviceName)(poiter instance);
	void DLL_CALLCONV (*NUI_DestroyInstance)(poiter instance);
  int DLL_CALLCONV (*NUI_InitInstance)(poiter instance, bool usedepth,bool usecolor, bool useskeleton);
	POINT* DLL_CALLCONV (*NUI_GetSkeletonPositions)(poiter instance, int skeletonIndex, int* count);

public:
	NUI();
	bool Init();
  int GetDeviceCount();
  poiter CreateInstance(int index);
  void DestroyInstance(poiter instance);

  char* GetDeviceName(poiter instance);
  int InitInstance(poiter instance, bool usedepth,bool usecolor, bool useskeleton);
  POINT* GetSkeletonPositions(poiter instance, int skeletonIndex, int* count);
};

NUI* nui;

void InitNUICommands();
