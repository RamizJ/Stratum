#include "audio\SCAudio.h"
#include "twindow.h"
#include <excpt.h>
#include <iostream.h>


//список функции, загружаемых из библиотеки AudioControl
ptr (*AudioOpenDevice)(char* DeviceName, char* Params);
ptr (*AudioOpenSound)(ptr Device, char* FileName, int Streaming);
void (*AudioPlay)(ptr Sound);
void (*AudioStop)(ptr Sound);
int (*AudioIsPlaying)(ptr Sound);
void (*AudioReset)(ptr Sound);
void (*AudioSetRepeat)(ptr Sound, int Repeat);
int (*AudioGetRepeat)(ptr Sound);
void (*AudioSetVolume)(ptr Sound, double Volume);
double (*AudioGetVolume)(ptr Sound);
void (*AudioSetBalance)(ptr Sound, double Balance);
double (*AudioGetBalance)(ptr Sound);
void (*AudioSetTone)(ptr Sound, double Tone);
double (*AudioGetTone)(ptr Sound);
int (*AudioIsSeekable)(ptr Sound);
void (*AudioSetPosition)(ptr Sound, int Position);
int (*AudioGetPosition)(ptr Sound);
int (*AudioGetLength)(ptr Sound);
void (*AudioDeleteSound)(ptr Sound);
void (*AudioDeleteDevice)(ptr Device);


//ссылка на библиотеку
HMODULE AudioDll = NULL;

//Списки устройств и звуков
TCollection *AudioSoundList= new TCollection(max_size,1);
ptr Device;

//шаблон функции для загрузки функций из библиотек
template<class T>
void LoadFunctionImpl(HMODULE Module, T& a_Function, const char* a_Name)
{
	a_Function = (T)GetProcAddress(Module, a_Name);
}


//загружаем функции из библиотеки AudioControl
void InitAudioCore()
{
  //char s[102400];
  //wsprintf(s,"%s%s",sc_path,"\\plugins\\AudioControl\\AudioControl.dll");
  //MessageBox(0,s,"",0);
  //AudioDll = LoadLibrary(s);
	#ifdef LOGON
 	LogMessage("Audio: load library");
 	LogMessage(sc_path);
	#endif
	AudioDll = LoadLibrary("AudioControl.dll");
  //GetLastError();
	#ifdef LOGON
  if(AudioDll==NULL)
  {
 		LogMessage("ERROR load audio library");
    return;
  }
 	LogMessage("Audio: load functions");
	#endif
  LoadFunction(AudioDll,AudioOpenDevice);
	LoadFunction(AudioDll,AudioOpenSound);
	LoadFunction(AudioDll,AudioPlay);
	LoadFunction(AudioDll,AudioStop);
	LoadFunction(AudioDll,AudioIsPlaying);
	LoadFunction(AudioDll,AudioReset);
	LoadFunction(AudioDll,AudioSetRepeat);
	LoadFunction(AudioDll,AudioGetRepeat);
	LoadFunction(AudioDll,AudioSetVolume);
	LoadFunction(AudioDll,AudioGetVolume);
	LoadFunction(AudioDll,AudioSetBalance);
	LoadFunction(AudioDll,AudioGetBalance);
	LoadFunction(AudioDll,AudioSetTone);
	LoadFunction(AudioDll,AudioGetTone);
	LoadFunction(AudioDll,AudioIsSeekable);
	LoadFunction(AudioDll,AudioSetPosition);
	LoadFunction(AudioDll,AudioGetPosition);
	LoadFunction(AudioDll,AudioGetLength);
	LoadFunction(AudioDll,AudioDeleteSound);
	LoadFunction(AudioDll,AudioDeleteDevice);

  if(AudioOpenDevice)
  {
		#ifdef LOGON
 		LogMessage("Audio: execute AudioOpenDevice");
		#endif
	  Device=AudioOpenDevice("autodetect","rate=44100");
  }
  else
  {
		#ifdef LOGON
 		LogMessage("ERROR load function AudioOpenDevice");
		#endif
  }
	#ifdef LOGON
 	LogMessage("Audio: end");
	#endif
}

//удаляем библиотеку из памяти
void DoneAudioCore()
{
	FreeLibrary(AudioDll);
}

void FreeAudio()
{
	ptr s=NULL;
	for(int i=0;i<AudioSoundList->count;i++)
  {
    s=(char*)AudioSoundList->At(i);
    AudioStop(s);
    s=0;
  }
  AudioSoundList->DeleteAll();
}

//Создаем функции для виртуальной машины Стратума

void vm_audioopensound()
{
  char *FileName=POP_PTR;
  char s[999];
  GetFullFileNameEx((TProject *)GetCurrentProject(), FileName, s);
  ptr Sound=AudioOpenSound(Device,s,1);
  AudioSoundList->Insert(Sound);
  PUSH_PTR(Sound);
}

void vm_audioplay()
{
  ptr Sound=POP_PTR;
  AudioPlay(Sound);
}

void vm_audiostop()
{
  ptr Sound=POP_PTR;
  AudioStop(Sound);
}

void vm_audioisplaying()
{
  ptr Sound=POP_PTR;
  PUSH_DOUBLE(AudioIsPlaying(Sound));
}
void vm_audioreset()
{
  ptr Sound=POP_PTR;
	AudioReset(Sound);
}
void vm_audiosetrepeat()
{
  int Repeat = (int)POP_DOUBLE;
  ptr Sound=POP_PTR;
  AudioSetRepeat(Sound,Repeat);
}
void vm_audiogetrepeat()
{
  ptr Sound=POP_PTR;
	PUSH_DOUBLE(AudioGetRepeat(Sound));
}
void vm_audiosetvolume()
{
  double Volume = POP_DOUBLE;
  ptr Sound=POP_PTR;
  AudioSetVolume(Sound,Volume);
}
void vm_audiogetvolume()
{
  ptr Sound=POP_PTR;
	PUSH_DOUBLE(AudioGetVolume(Sound));
}
void vm_audiosetbalance()
{
  double Balance = POP_DOUBLE;
  ptr Sound=POP_PTR;
  AudioSetBalance(Sound,Balance);
}
void vm_audiogetbalance()
{
  ptr Sound=POP_PTR;
	PUSH_DOUBLE(AudioGetBalance(Sound));
}
void vm_audiosettone()
{
  double Tone = POP_DOUBLE;
  ptr Sound=POP_PTR;
  AudioSetTone(Sound,Tone);
}
void vm_audiogettone()
{
  ptr Sound=POP_PTR;
	PUSH_DOUBLE(AudioGetTone(Sound));
}
void vm_audioisseekable()
{
	ptr Sound =POP_PTR;
	PUSH_DOUBLE(AudioIsSeekable(Sound));
}
void vm_audiosetposition()
{
  int Position = (int)POP_DOUBLE;
  ptr Sound=POP_PTR;
  AudioSetPosition(Sound,Position);
}
void vm_audiogetposition()
{
  ptr Sound=POP_PTR;
	PUSH_DOUBLE(AudioGetPosition(Sound));
}
void vm_audiogetlength()
{
  ptr Sound=POP_PTR;
	PUSH_DOUBLE(AudioGetLength(Sound));
}
void vm_audiodeletesound()
{
  ptr *Sound=(ptr*)POP_PTR;
	AudioDeleteSound(*Sound);
  if(AudioSoundList->IndexOf(*Sound)>=0)
	  AudioSoundList->AtDelete(AudioSoundList->IndexOf(*Sound));
  *Sound=0;
}

//Добавляем созданные функции виртуальной машины в массив команд ВМ
void InitAudioCommands()
{
	commands[VM_AUDIOOPENSOUND] = vm_audioopensound;
	commands[VM_AUDIOPLAY] = vm_audioplay;
	commands[VM_AUDIOSTOP] = vm_audiostop;
	commands[VM_AUDIOISPLAYING] = vm_audioisplaying;
	commands[VM_AUDIORESET] = vm_audioreset;
	commands[VM_AUDIOSETREPEAT] = vm_audiosetrepeat;
	commands[VM_AUDIOGETREPEAT] = vm_audiogetrepeat;
	commands[VM_AUDIOSETVOLUME] = vm_audiosetvolume;
	commands[VM_AUDIOGETVOLUME] = vm_audiogetvolume;
	commands[VM_AUDIOSETBALANCE] = vm_audiosetbalance;
	commands[VM_AUDIOGETBALANCE] = vm_audiogetbalance;
	commands[VM_AUDIOSETTONE] = vm_audiosettone;
	commands[VM_AUDIOGETTONE] = vm_audiogettone;
	commands[VM_AUDIOISSEEKABLE] = vm_audioisseekable;
	commands[VM_AUDIOSETPOSITION] = vm_audiosetposition;
	commands[VM_AUDIOGETPOSITION] = vm_audiogetposition;
	commands[VM_AUDIOGETLENGTH] = vm_audiogetlength;
	commands[VM_AUDIODELETESOUND] = vm_audiodeletesound;
}
