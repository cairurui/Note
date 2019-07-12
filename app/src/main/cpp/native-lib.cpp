#include <jni.h>

#include <android/log.h>

extern "C" {
#include "libavformat/avformat.h"
}

#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)


extern "C" void JNICALL
Java_com_xc_note_MainActivity_test(JNIEnv *env, jobject instance_) {


    av_register_all();
    LOGE("xiaocai test ");
}