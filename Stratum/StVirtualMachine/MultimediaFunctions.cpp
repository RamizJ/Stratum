#include "MultimediaFunctions.h"
#include "VmCodes.h"
#include "VmGlobal.h"
#include "VmLog.h"

#include <SpaceWidget.h>
#include <SpaceScene.h>
#include <Space.h>

using namespace StData;
using namespace StSpace;

namespace StVirtualMachine {

void setupMultimediaFunctions()
{
    operations[VM_AUDIOGETBALANCE] = audioGetBalance;
    operations[VM_AUDIOGETLENGTH] = audioGetLength;
    operations[VM_AUDIOGETPOSITION] = audioGetPosition;
    operations[VM_AUDIOGETREPEAT] = audioGetRepeat;
    operations[VM_AUDIOGETTONE] = audioGetTone;
    operations[VM_AUDIOGETVOLUME] = audioGetVolume;
    operations[VM_AUDIOISPLAYING] = audioIsPlaying;
    operations[VM_AUDIOISSEEKABLE] = audioIsSeekable;
    operations[VM_AUDIOOPENSOUND] = audioOpenSound;
    operations[VM_AUDIOPLAY] = audioPlay;
    operations[VM_AUDIORESET] = audioReset;
    operations[VM_AUDIOSETBALANCE] = audioSetBalance;
    operations[VM_AUDIOSETPOSITION] = audioSetPosition;
    operations[VM_AUDIOSETREPEAT] = audioSetRepeat;
    operations[VM_AUDIOSETTONE] = audioSetTone;
    operations[VM_AUDIOSETVOLUME] = audioSetVolume;
    operations[VM_AUDIOSTOP] = audioStop;

    operations[SNDPLAYSOUND] = sndPlaySound;

    operations[GETLASTMCIERROR] = getLastMCIError;
    operations[GETMCIERROR] = getMCIerrorStr;
    operations[MCISENDSTRING_INT] = mciSendStringInt;
    operations[MCISENDSTRING] = mciSendStringStr;
    operations[MCISENDSTRINGEX] = mciSendStringEx;

    operations[VM_CREATEVIDEOFRAME2D] = createVideoFrame2d;
    operations[VM_FRAMEGETPOS] = frameGetPos2d;
    operations[VM_FRAMEGETHVIDEO] = frameGetVideo2d;
    operations[VM_FRAMESETPOS2D] = frameSetPos2d;
    operations[VM_SETVIDEOSRC2D] = frameSetSrcRect2d;

    operations[VM_OPENVIDEO] = openVideo;
    operations[VM_CLOSEVIDEO] = closeVideo;
    operations[VM_VIDEOPAUSE2D] = videoPause2d;
    operations[VM_VIDEOPLAY2D] = videoPlay2d;
    operations[VM_VIDEORESUME2D] = videoResume2d;
    operations[VM_VIDEOSETPOS] = videoSetPos2d;
    operations[VM_VIDEOSTOP2D] = videoStop2d;
    operations[VM_VIDEOCOMPRESSDIALOG] = videoDialog;
    operations[VM_BEGINWRITEVIDEO] = beginWriteVideo2d;
}

void openVideo()
{
    throw std::runtime_error("Function 'openVideo' not implemented");
}

void closeVideo()
{
    throw std::runtime_error("Function 'closeVideo' not implemented");
}

void createVideoFrame2d()
{
    throw std::runtime_error("Function 'createVideoFrame2d' not implemented");
}

void videoSetPos2d()
{
    throw std::runtime_error("Function 'videoSetPos2d' not implemented");
}

void frameSetPos2d()
{
    throw std::runtime_error("Function 'frameSetPos2d' not implemented");
}

void frameSetSrcRect2d()
{
    throw std::runtime_error("Function 'frameSetSrcRect2d' not implemented");
}

void videoPlay2d()
{
    throw std::runtime_error("Function 'videoPlay2d' not implemented");
}

void videoPause2d()
{
    throw std::runtime_error("Function 'videoPause2d' not implemented");
}

void videoResume2d()
{
    throw std::runtime_error("Function 'videoResume2d' not implemented");
}

void videoStop2d()
{
    throw std::runtime_error("Function 'videoStop2d' not implemented");
}

void beginWriteVideo2d()
{
    throw std::runtime_error("Function 'beginWriteVideo2d' not implemented");
}

void writeVideoFrame2d()
{
    throw std::runtime_error("Function 'writeVideoFrame2d' not implemented");
}

void videoDialog()
{
    throw std::runtime_error("Function 'videoDialog' not implemented");
}

void frameGetPos2d()
{
    throw std::runtime_error("Function 'frameGetPos2d' not implemented");
}

void frameGetVideo2d()
{
    throw std::runtime_error("Function 'frameGetVideo2d' not implemented");
}

void mciSendStringInt()
{
    QString cmd = valueStack->popString();
//    VmLog::instance().warning("Function 'mciSendString(int)' not implemented");
    valueStack->pushDouble(0.0);
}

void mciSendStringStr()
{
    QString cmd = valueStack->popString();
//    VmLog::instance().warning("Function 'mciSendString(str)' not implemented");
    valueStack->pushString("");
}

void mciSendStringEx()
{
    double num = valueStack->popDouble();
    QString cmd = valueStack->popString();
//    VmLog::instance().warning("Function 'mciSendString(ex)' not implemented");
    valueStack->pushString("");
}

void getLastMCIError()
{
    throw std::runtime_error("Function 'getLastMCIError' not implemented");
}

void getMCIerrorStr()
{
    throw std::runtime_error("Function 'getMCIerrorStr' not implemented");
}

void sndPlaySound()
{
    int flags = valueStack->popDouble();
    QString fileName = valueStack->popString();

    SystemLog::instance().error("Function 'sndPlaySound' not implemented");

    valueStack->pushDouble(0.0);
}

void audioOpenSound()
{
    throw std::runtime_error("Function 'audioOpenSound' not implemented");
}

void audioPlay()
{
    throw std::runtime_error("Function 'audioPlay' not implemented");
}

void audioStop()
{
    throw std::runtime_error("Function 'audioStop' not implemented");
}

void audioIsPlaying()
{
    throw std::runtime_error("Function 'audioIsPlaying' not implemented");
}

void audioReset()
{
    throw std::runtime_error("Function 'audioReset' not implemented");
}

void audioGetRepeat()
{
    throw std::runtime_error("Function 'audioGetRepeat' not implemented");
}

void audioSetRepeat()
{
    throw std::runtime_error("Function 'audioSetRepeat' not implemented");
}

void audioGetVolume()
{
    throw std::runtime_error("Function 'audioGetVolume' not implemented");
}

void audioSetVolume()
{
    throw std::runtime_error("Function 'audioSetVolume' not implemented");
}

void audioGetBalance()
{
    throw std::runtime_error("Function 'audioGetBalance' not implemented");
}

void audioSetBalance()
{
    throw std::runtime_error("Function 'audioSetBalance' not implemented");
}

void audioSetTone()
{
    throw std::runtime_error("Function 'audioSetTone' not implemented");
}

void audioGetTone()
{
    throw std::runtime_error("Function 'audioGetTone' not implemented");
}

void audioIsSeekable()
{
    throw std::runtime_error("Function 'audioIsSeekable' not implemented");
}

void audioSetPosition()
{
    throw std::runtime_error("Function 'audioSetPosition' not implemented");
}

void audioGetPosition()
{
    throw std::runtime_error("Function 'audioGetPosition' not implemented");
}

void audioGetLength()
{
    throw std::runtime_error("Function 'audioGetLength' not implemented");
}


}
