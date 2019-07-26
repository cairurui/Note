//
// Created by xiaocai on 2019/7/24.
//

#ifndef NOTE_XAUDIO_H
#define NOTE_XAUDIO_H

#include "XJNICall.h"
#include "XConstDefine.h"
#include <pthread.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "XPacketQueue.h"
#include "XPlayerStatus.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
};

class XAudio {
public:
    XJNICall *xJniCall = NULL;
    int audioStreamIndex = -1;
    AVFormatContext *pFormatContext = NULL;
    AVCodecContext *pCodecContext = NULL;
    SwrContext *swrContext = NULL;
    uint8_t *resampleOutBuffer = 0;
    XPacketQueue *xPacketQueue = NULL;
    XPlayerStatus *xPlayerStatus = NULL;

public:

    XAudio(int audioStreamIndex, XJNICall *pJniCall, AVFormatContext *pFormatContext);

    ~XAudio();

    void play();

    void initCrateOpenSLES();

    int resampleAudio();

    void analysisStream(ThreadMode mode, AVStream **pStream);

    void callPlayerJniError(ThreadMode threadMode, int code, char *msg);

    void release();
};


#endif //NOTE_XAUDIO_H
