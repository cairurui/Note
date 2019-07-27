//
// Created by 蔡汝蕊 on 2019-07-22.
//

#ifndef NOTE_XJNICALL_H
#define NOTE_XJNICALL_H

#include <jni.h>

enum ThreadMode {
    THREAD_CHILD, THREAD_MAIN
};


class XJNICall {
public:
    JavaVM *javaVM;
    JNIEnv *jniEnv;

    jobject jPlayerObj;

    jmethodID jPlayerErrorMid;
    jmethodID jPlayerPreparedMid;
    jmethodID jPlayerProgressMid;

public:
    XJNICall(JavaVM *javaVM, JNIEnv *jniEnv, jobject jPlayerObj);

    ~XJNICall();

public:

    void callPlayerError(ThreadMode threadMode, int code, char *msg);


    void callPlayerPrepared(ThreadMode mode);

    void onCallProgress(ThreadMode threadMode, int current, int total);

    void onCallComplete(ThreadMode mode);
};


#endif //NOTE_XJNICALL_H
