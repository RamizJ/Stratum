#ifndef MULTIMEDIAFUNCTIONS_H
#define MULTIMEDIAFUNCTIONS_H

namespace StVirtualMachine {

extern void setupMultimediaFunctions();

extern void openVideo();
extern void closeVideo();
extern void createVideoFrame2d();
extern void videoSetPos2d();
extern void frameSetPos2d();
extern void frameSetSrcRect2d();
extern void videoPlay2d();
extern void videoPause2d();
extern void videoResume2d();
extern void videoStop2d();
extern void beginWriteVideo2d();
extern void writeVideoFrame2d();
extern void videoDialog();
extern void frameGetPos2d();
extern void frameGetVideo2d();

extern void mciSendStringInt();
extern void mciSendStringStr();
extern void mciSendStringEx();
extern void getLastMCIError();
extern void getMCIerrorStr();

extern void sndPlaySound();
extern void audioOpenSound();
extern void audioPlay();
extern void audioStop();
extern void audioIsPlaying();
extern void audioReset();
extern void audioGetRepeat();
extern void audioSetRepeat();
extern void audioGetVolume();
extern void audioSetVolume();
extern void audioGetBalance();
extern void audioSetBalance();
extern void audioSetTone();
extern void audioGetTone();
extern void audioIsSeekable();
extern void audioSetPosition();
extern void audioGetPosition();
extern void audioGetLength();

}

#endif // MULTIMEDIAFUNCTIONS_H
