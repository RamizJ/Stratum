/*
Copyright (c) 2011
Project: Stratum 3.1
Module :
		  Natural User Interface
Author:
			Marchenko Sergey
*/

#include "NUI.h"
#include "twindow.h"
#include "dynodata.h"

template< class T >
void LoadFunctionImpl(HMODULE Module, T& a_Function, const char* a_Name)
{
	a_Function = (T)GetProcAddress(Module, a_Name);
}
#define LoadFunction(module, func, name) LoadFunctionImpl(module, func, name);

NUI::NUI()
{
}
bool NUI::Init()
{
  char s[1000];
  wsprintf(s,"%s\\NUI.dll",sc_path);
  //MessageBox(0,s,"",0);
  library = LoadLibrary(s); if(!library)return false;
  LoadFunction(library, NUI_GetDeviceCount, "NUI_GetDeviceCount"); if(!NUI_GetDeviceCount)return false;
  LoadFunction(library, NUI_GetInstanceByIndex, "NUI_GetInstanceByIndex"); if(!NUI_GetInstanceByIndex)return false;
  LoadFunction(library, NUI_GetDeviceName, "NUI_GetDeviceName"); if(!NUI_GetDeviceName)return false;
  LoadFunction(library, NUI_DestroyInstance, "NUI_DestroyInstance"); if(!NUI_DestroyInstance)return false;
  LoadFunction(library, NUI_InitInstance, "NUI_InitInstance"); if(!NUI_InitInstance)return false;
  LoadFunction(library, NUI_GetSkeletonPositions, "NUI_GetSkeletonPositions"); if(!NUI_GetSkeletonPositions)return false;
  //MessageBox(0,"end","",0);
  return true;
}
int NUI::GetDeviceCount()
{
	return NUI_GetDeviceCount();
}

poiter NUI::CreateInstance(int index)
{
	return NUI_GetInstanceByIndex(index);
}

void NUI::DestroyInstance(poiter instance)
{
	NUI_DestroyInstance(instance);
}

char* NUI::GetDeviceName(poiter instance)
{
	return NUI_GetDeviceName(instance);
}

int NUI::InitInstance(poiter instance, bool usedepth,bool usecolor, bool useskeleton)
{
	return NUI_InitInstance(instance,usedepth,usecolor,useskeleton);
}

POINT* NUI::GetSkeletonPositions(poiter instance, int skeletonIndex, int* count)
{
	return NUI_GetSkeletonPositions(instance,skeletonIndex,count);
}



//Добавляем созданные функции виртуальной машины в массив команд ВМ

void vm_nui_init()
{
   nui->Init();
}

void vm_nui_getdevicecount()
{
   PUSH_DOUBLE(nui->GetDeviceCount());
}

void vm_nui_createinstance()
{
   PUSH_PTR(nui->CreateInstance((int)(POP_DOUBLE)));
}

void vm_nui_destroyinstance()
{
   nui->DestroyInstance(POP_PTR);
}

void vm_nui_getdevicename()
{
  poiter instance= (poiter)(POP_PTR);
	PUSH_PTR(nui->GetDeviceName(instance));
}

void vm_nui_initinstance()
{
	bool useskeleton = (bool)POP_DOUBLE;
	bool usecolor = (bool)POP_DOUBLE;
	bool usedepth = (bool)POP_DOUBLE;
  poiter instance= (poiter)(POP_PTR);
  PUSH_DOUBLE(nui->InitInstance(instance,usedepth,usecolor,useskeleton));
}

void vm_nui_getskeletonpositions()
{
	bool skeletonIndex = (bool)POP_DOUBLE;
	dyno_t h = (dyno_t)POP_LONG;
  poiter instance= (poiter)(POP_PTR);
  if(h==0 || instance==0){PUSH_DOUBLE(1);return;}

  int count=0;
	POINT* ps = nui->GetSkeletonPositions(instance,skeletonIndex,&count);
  if(count==0){PUSH_DOUBLE(2);return;}
  while(dyno_mem->GetCount(h)>0)dyno_mem->DeleteInVar(h,0);
  double x,y;
  for(int i=0; i<count;i++)
  {
  	x=(double)ps[i].x/1000000.0; y=(double)ps[i].y/1000000.0;
	 	dyno_mem->InsertInVar(h,"FLOAT");
		dyno_mem->SetVar(h,i*2,"FLOAT",0,&x);
	 	dyno_mem->InsertInVar(h,"FLOAT");
		dyno_mem->SetVar(h,i*2+1,"FLOAT",0,&y);
  }
  PUSH_DOUBLE(0);
}

//можно использовать только 1501-1600
void InitNUICommands()
{
  nui=new NUI();

	commands[1501] = vm_nui_init;
	commands[1502] = vm_nui_getdevicecount;
	commands[1503] = vm_nui_createinstance;
	commands[1504] = vm_nui_destroyinstance;
	commands[1505] = vm_nui_getdevicename;
	commands[1506] = vm_nui_initinstance;
	commands[1507] = vm_nui_getskeletonpositions;
}
