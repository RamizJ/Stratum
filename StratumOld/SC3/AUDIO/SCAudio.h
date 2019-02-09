#pragma once
#include <windows.h>
#include "_vmach.h"
#include "collect.h"

//функции ядра
void InitAudioCore();
void DoneAudioCore();
void InitAudioCommands();
void FreeAudio();

typedef char* ptr;

template<class T>
void LoadFunctionImpl(HMODULE Module,T& a_Function, const char* a_Name);

#define LoadFunction(module, name) LoadFunctionImpl(module, name, #name);

extern "C"
{
extern ptr (*AudioOpenDevice)(char* DeviceName, char* Params);
extern ptr (*AudioOpenSound)(ptr Device, char* FileName, int Streaming);
extern void (*AudioPlay)(ptr Sound);
extern void (*AudioStop)(ptr Sound);
extern int (*AudioIsPlaying)(ptr Sound);
extern void (*AudioReset)(ptr Sound);
extern void (*AudioSetRepeat)(ptr Sound, int Repeat);
extern int (*AudioGetRepeat)(ptr Sound);
extern void (*AudioSetVolume)(ptr Sound, double Volume);
extern double (*AudioGetVolume)(ptr Sound);
extern void (*AudioSetBalance)(ptr Sound, double Balance);
extern double (*AudioGetBalance)(ptr Sound);
extern void (*AudioSetTone)(ptr Sound, double Tone);
extern double (*AudioGetTone)(ptr Sound);
extern int (*AudioIsSeekable)(ptr Sound);
extern void (*AudioSetPosition)(ptr Sound, int Position);
extern int (*AudioGetPosition)(ptr Sound);
extern int (*AudioGetLength)(ptr Sound);
extern void (*AudioDeleteSound)(ptr Sound);
extern void (*AudioDeleteDevice)(ptr Device);
}


//список кодов функции ВМ. можно использовать только коды 801-820

#define VM_AUDIOOPENDEVICE		801
#define VM_AUDIOOPENSOUND			802
#define VM_AUDIOPLAY					803
#define VM_AUDIOSTOP					804
#define VM_AUDIOISPLAYING			805
#define VM_AUDIORESET					806
#define VM_AUDIOSETREPEAT			807
#define VM_AUDIOGETREPEAT			808
#define VM_AUDIOSETVOLUME			809
#define VM_AUDIOGETVOLUME			810
#define VM_AUDIOSETBALANCE		811
#define VM_AUDIOGETBALANCE		812
#define VM_AUDIOSETTONE				813
#define VM_AUDIOGETTONE				814
#define VM_AUDIOISSEEKABLE		815
#define VM_AUDIOSETPOSITION		816
#define VM_AUDIOGETPOSITION		817
#define VM_AUDIOGETLENGTH			818
#define VM_AUDIODELETESOUND		819
#define VM_AUDIODELETEDEVICE	820

