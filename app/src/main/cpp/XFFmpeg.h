//
// Created by 蔡汝蕊 on 2019-07-22.
//

#ifndef NOTE_XFFMPEG_H
#define NOTE_XFFMPEG_H

#include "XJNICall.h"
#include "XConstDefine.h"
#include <pthread.h>

extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
};


class XFFmpeg {
public:
    XJNICall *xJniCall = NULL;
    char *url = NULL;
    AVFormatContext *pAVFormatContext = 0;
    AVCodecContext *pCodecContext = 0;
    SwrContext *swrContext;
    uint8_t *resampleOutBuffer;

public:
    XFFmpeg(XJNICall *xJniCall, const char *url);

    ~XFFmpeg();

public:
    void play();

    void prepare();

    void prepareAsync();

    void callPlayerJniError(int code, char *msg);

    void release();

};


#endif //NOTE_XFFMPEG_H
