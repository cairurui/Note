//
// Created by xiaocai on 2019/7/22.
//


#ifndef XPLAYER_XCONSTDEFINE_H
#define XPLAYER_XCONSTDEFINE_H

#include <android/log.h>

#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

#define AUDIO_SAMPLE_RATE 44100

// ---------- 播放错误码 start ----------
#define FIND_STREAM_ERROR_CODE -0x10
#define CODEC_FIND_DECODER_ERROR_CODE -0x11
#define CODEC_ALLOC_CONTEXT_ERROR_CODE -0x12
#define SWR_ALLOC_SET_OPTS_ERROR_CODE -0x13
#define SWR_CONTEXT_INIT_ERROR_CODE -0x14
// ---------- 播放错误码 end ----------

#endif //XPLAYER_XCONSTDEFINE_H