//
// Created by 蔡汝蕊 on 2019-07-22.
//

#ifndef NOTE_XFFMPEG_H
#define NOTE_XFFMPEG_H

#include "XJNICall.h"
#include "XConstDefine.h"
#include <pthread.h>
#include "XAudio.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
};


class XFFmpeg {
public:
    char *url = NULL;
    AVFormatContext *pAVFormatContext = 0;
    XAudio *xAudio = NULL;
    XJNICall *xJniCall = NULL;

public:
    XFFmpeg(XJNICall *xJniCall, const char *url);

    ~XFFmpeg();

public:
    void play();

    void prepare();

    void prepare(ThreadMode threadMode);

    void prepareAsync();

    void callPlayerJniError(ThreadMode threadMode, int code, char *msg);

    void release();

    pthread_mutex_t releaseMutex;

    void onPause();

    void onResume();
};


#endif //NOTE_XFFMPEG_H
