#undef STRATUM_INTERNAL
#include <gwin2d3d\gwinfunc.h>
#include "video\vengine.h"
#include <mem.h>
#include "video\scvideo.rh"
#include "video\vid_type.h"

void * GlobalAllocPtr(long ,long lTemp){
 if (lTemp==0)return 0;
 if (lTemp<49152l){
	BYTE * mem=new BYTE[(int)lTemp];
  _fmemset(mem, 0,(int)lTemp);
  return mem;
 }
 return 0;
};

void GlobalFreePtr(void * p){
 delete p;
}

TAudio::TAudio(HWND hwnd, PAVISTREAM pavi, LONG lStart, LONG lEnd, BOOL fWait){
  _fmemset(this, 0,sizeof(TAudio));
//  glPlayStartTime=timeGetTime();
  aviaudioPlay(hwnd,pavi,lStart,lEnd, fWait);
  UpdatePlaying();
};

TAudio::~TAudio(){
  aviaudioStop();
  aviaudioCloseDevice();
};

BOOL TAudio::aviaudioPlay(HWND hwnd, PAVISTREAM pavi, LONG lStart, LONG lEnd, BOOL fStart)
{
	 if (!aviaudioOpenDevice(hwnd, pavi))
	return FALSE;
//   lStart=lStart*sdwSamplesPerSec/1000;
//   lEnd=lEnd*sdwSamplesPerSec/1000;
	 if (lStart < 0)
	lStart = AVIStreamStart(pavi);

	 if (lEnd < 0)
	lEnd = AVIStreamEnd(pavi);

	 if (lStart >= lEnd)
	return TRUE;

	 if (!sfPlaying) {

	//
	// We're beginning play, so pause until we've filled the buffers
	// for a seamless start
	//
	waveOutPause(shWaveOut);

	slBegin = lStart;
	slCurrent = lStart;
	slEnd = lEnd;
	sfPlaying = TRUE;
	 } else {
	// Gap in wave that is supposed to be played!

	slEnd = lEnd;
	 }

	 //
	 // Now unpause the audio and away it goes!
	 //
	 if (fStart){playwasstarted=1;aviaudioiFillBuffers(); waveOutRestart(shWaveOut);}else
				playwasstarted=0;
	 return TRUE;
}
void TAudio::BeginPlay(){
 if (!playwasstarted){
 playwasstarted=1;
  aviaudioiFillBuffers();
  waveOutRestart(shWaveOut);

 }
};
BOOL TAudio::aviaudioiFillBuffers(){
	 if (!playwasstarted)return 0;
	 long		lRead;
	 int		w;
	 long		lSamplesToPlay;

	 /* We're not playing, so do nothing. */
	 if (!sfPlaying)
	return TRUE;

	 while ((swBuffersOut < swBuffers)&& (swLoopOut<(swBuffers-1))) {
	if (slCurrent >= slEnd) {
		 if (sfLooping) {
		/* Looping, so go to the beginning. */
		slCurrent = slBegin;
//--------

	 {
		if (wasloop)break;
		swLoopBuffer=swNextBuffer;
		swLoopOut=0;
		wasloop=1;}

//--------
		 } else 	break;
	}

	/* Figure out how much data should go in this buffer */
	lSamplesToPlay = slEnd - slCurrent;
	if (lSamplesToPlay > AUDIO_BUFFER_SIZE / slSampleSize)
		 lSamplesToPlay = AUDIO_BUFFER_SIZE / slSampleSize;


	AVIError(AVIStreamRead(spavi, slCurrent, lSamplesToPlay,
				salpAudioBuf[swNextBuffer]->lpData,
				AUDIO_BUFFER_SIZE,
				(LONG*) &(salpAudioBuf[swNextBuffer]->dwBufferLength),
				&lRead));

	if (lRead != lSamplesToPlay) {
		 return FALSE;
	}
	slCurrent += lRead;
//--------
  if (!wasloop)
  {
//--------
	w = waveOutWrite(shWaveOut, salpAudioBuf[swNextBuffer],sizeof(WAVEHDR));

	if (w != 0) {
		 return FALSE;
	}
//--------
  }
//--------
	++swBuffersOut;
	if (wasloop)swLoopOut++;
	++swNextBuffer;
	if (swNextBuffer >= swBuffers)
		 swNextBuffer = 0;
	 }

	 if (swBuffersOut == 0 && slCurrent >= slEnd)
	aviaudioStop();

	 /* We've filled all of the buffers we can or want to. */
	 return TRUE;
};
LONG TAudio::aviaudioTime(){
 MMTIME	mmtime;
	 if (!sfPlaying)	return -1;
	 int w;
	 mmtime.wType = TIME_SAMPLES;
	 w=waveOutGetPosition(shWaveOut, &mmtime, sizeof(mmtime));
	 if (w)failed=1;
	 DWORD sample;

	 if (mmtime.wType == TIME_SAMPLES){
		sample=(mmtime.u.sample+slBegin1)-glPlayStartTime;
	 }else{
 //??????????????/
	  sample=mmtime.u.sample;
	 }

 //--------
	 if (sample>=slEnd1){


	  if (wasloop){
	 w=waveOutPause(shWaveOut);
	 if (w)failed=1;
	 while (swLoopBuffer!=swNextBuffer){
		w = waveOutWrite(shWaveOut, salpAudioBuf[swLoopBuffer],sizeof(WAVEHDR));
		if (w)failed=1;
	//	swLoopBuffer++;
			if (w != 0) {
		 return FALSE;
		}
	++swLoopBuffer;
	if (swLoopBuffer >= swBuffers) swLoopBuffer = 0;
	}
		w =waveOutRestart(shWaveOut);
		if (w)failed=1;
		wasloop=0;
		swLoopBuffer=0;
		glPlayStartTime=sample;
		sample=slBegin1;
		swLoopOut=0;
	}// looped

//	glPlayStartTime+=(sample-slEnd)+sample;
  if (sfLooping){
	 glPlayStartTime=sample;
	 sample=sample-glPlayStartTime;
	}
  }
//--------
	 long _pos1=(1000*sample/sdwSamplesPerSec);
//	 long _pos=AVIStreamSampleToTime(spavi,sample);
	 return _pos1;
};
void TAudio::aviaudioCloseDevice()
{
	// WORD	w;

	 if (shWaveOut) {
	while (swBuffers > 0) {
		 --swBuffers;
		 waveOutUnprepareHeader(shWaveOut, salpAudioBuf[swBuffers],
					sizeof(WAVEHDR));
		 GlobalFreePtr((LPSTR) salpAudioBuf[swBuffers]);
	}
//	w =
   waveOutClose(shWaveOut);

	shWaveOut = NULL;
	 }
}
void TAudio::Pause(){
 paused=TRUE;
 waveOutPause(shWaveOut);
};
void TAudio::Resume(){
 paused=FALSE;
 waveOutRestart(shWaveOut);
};

BOOL TAudio::aviaudioOpenDevice(HWND hwnd, PAVISTREAM pavi){
    int 	w;
#ifdef WIN32
	 LPWAVEFORMATEX	lpFormat;
#else
	 LPWAVEFORMAT	lpFormat;
#endif
	 LONG		cbFormat;
	 AVISTREAMINFO	strhdr;

	 if (!pavi)		// no wave data to play
	return FALSE;

	 if (shWaveOut)	// already something playing
	return TRUE;

	 spavi = pavi;

	 AVIError(AVIStreamInfo(pavi, &strhdr, sizeof(strhdr)));

	 slSampleSize = (LONG) strhdr.dwSampleSize;
	 if (slSampleSize <= 0 || slSampleSize > AUDIO_BUFFER_SIZE)
	return FALSE;

	 AVIError(AVIStreamFormatSize(pavi, 0, &cbFormat));

	 (void*)lpFormat = GlobalAllocPtr(GHND, cbFormat);
	 if (!lpFormat)
	return FALSE;

	 AVIError(AVIStreamReadFormat(pavi, 0, lpFormat, &cbFormat));

	 sdwSamplesPerSec = ((LPWAVEFORMAT) lpFormat)->nSamplesPerSec;
	 sdwBytesPerSec = ((LPWAVEFORMAT) lpFormat)->nAvgBytesPerSec;

static UINT U=waveOutOpen(NULL,WAVE_MAPPER,lpFormat,NULL,NULL,WAVE_FORMAT_QUERY);
	 w = waveOutOpen(&shWaveOut, WAVE_MAPPER, lpFormat,
			(DWORD) (UINT) hwnd, 0L, CALLBACK_WINDOW);
	 //
	 // Maybe we failed because someone is playing sound already.
	 // Shut any sound off, and try once more before giving up.
	 //
	 if (w) {
	sndPlaySound(NULL, 0);
	w = waveOutOpen(&shWaveOut, WAVE_MAPPER, lpFormat,
			(DWORD) (UINT) hwnd, 0L, CALLBACK_WINDOW);
	 }

	 if (w != 0) {
	/* Show error message here? */

	return FALSE;
	 }

	 for (swBuffers = 0; swBuffers < MAX_AUDIO_BUFFERS; swBuffers++) {
	if (!(salpAudioBuf[swBuffers] =
		(LPWAVEHDR)GlobalAllocPtr(GMEM_MOVEABLE | GMEM_SHARE,
			(DWORD)(sizeof(WAVEHDR) + AUDIO_BUFFER_SIZE))))
		 break;
	salpAudioBuf[swBuffers]->dwFlags = WHDR_DONE;
	salpAudioBuf[swBuffers]->lpData = (LPSTR) salpAudioBuf[swBuffers]
							 + sizeof(WAVEHDR);
	salpAudioBuf[swBuffers]->dwBufferLength = AUDIO_BUFFER_SIZE;
	if (!waveOutPrepareHeader(shWaveOut, salpAudioBuf[swBuffers],
					sizeof(WAVEHDR)))
		 continue;

	GlobalFreePtr((LPSTR) salpAudioBuf[swBuffers]);
	break;
	 }

	 if (swBuffers < MIN_AUDIO_BUFFERS) {
	  aviaudioCloseDevice();
	  return FALSE;
	 }

	 swBuffersOut = 0;
	 swNextBuffer = 0;
	 sfPlaying = FALSE;
	 return TRUE;
};
void TAudio::aviaudioStop(void)
{
	 //int	w;
	 if (shWaveOut != 0) {
	  //w =
      waveOutReset(shWaveOut);
	  sfPlaying = FALSE;
	aviaudioCloseDevice();
	 }
};
