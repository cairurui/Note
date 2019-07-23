#include <jni.h>

#include "XConstDefine.h"

#include "XJNICall.h"
#include <pthread.h>

extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
}

#include "XFFmpeg.h"

XJNICall *pJniCall;
XFFmpeg *pXFFmpeg;

JavaVM *javaVM;


extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *j_vm, void *reserved) {
    jint result = -1;
    javaVM = j_vm;
    JNIEnv *env;
    if (javaVM->GetEnv((void **) (&env), JNI_VERSION_1_4) != JNI_OK) {
        return result;
    }
    // 可能会动态注册
    return JNI_VERSION_1_4;
}


extern "C" JNIEXPORT void JNICALL
Java_com_xc_note_media_XPlayer_nPlay(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);
    pJniCall = new XJNICall(javaVM, env, instance);
    pXFFmpeg = new XFFmpeg(pJniCall, url);
    pXFFmpeg->play();
    env->ReleaseStringUTFChars(url_, url);
}