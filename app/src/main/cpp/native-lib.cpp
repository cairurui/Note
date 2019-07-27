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
Java_com_xc_note_media_XPlayer_nStart(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);

    if (pXFFmpeg != NULL) {
        pXFFmpeg->play();
    }

    env->ReleaseStringUTFChars(url_, url);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xc_note_media_XPlayer_nPrepareAsync(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);

    if (pXFFmpeg == NULL) {
        pJniCall = new XJNICall(javaVM, env, instance);
        pXFFmpeg = new XFFmpeg(pJniCall, url);
        pXFFmpeg->prepareAsync();
    }

    env->ReleaseStringUTFChars(url_, url);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xc_note_media_XPlayer_nPrepare(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);

    if (pXFFmpeg == NULL) {
        pJniCall = new XJNICall(javaVM, env, instance);
        pXFFmpeg = new XFFmpeg(pJniCall, url);
        pXFFmpeg->prepare(THREAD_MAIN);
    }

    env->ReleaseStringUTFChars(url_, url);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xc_note_media_XPlayer_nPause(JNIEnv *env, jobject instance) {
    if (pXFFmpeg != NULL) {
        pXFFmpeg->onPause();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xc_note_media_XPlayer_nResume(JNIEnv *env, jobject instance) {
    if (pXFFmpeg != NULL) {
        pXFFmpeg->onResume();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_xc_note_media_XPlayer_nStop(JNIEnv *env, jobject instance) {

    if (pXFFmpeg != NULL) {
        pXFFmpeg->release();
        delete(pXFFmpeg);
        pXFFmpeg = NULL;
    }

    if (pJniCall!=NULL){
        delete(pJniCall);
        pJniCall = NULL;
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_xc_note_media_XPlayer_nSeekTo(JNIEnv *env, jobject instance, jint seconds) {

    // TODO

}