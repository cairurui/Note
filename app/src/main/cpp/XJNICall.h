//
// Created by 蔡汝蕊 on 2019-07-22.
//

#ifndef NOTE_XJNICALL_H
#define NOTE_XJNICALL_H

#include <jni.h>

enum ThreadMode{
    THREAD_CHILD,THREAD_MAIN
};


class XJNICall {
public:
    JavaVM *javaVM;
    JNIEnv *jniEnv;

    jobject jPlayerObj;

    jobject jAudioTrackObj;
    jmethodID jAudioTrackWriteMid;
    jmethodID jPlayerErrorMid;

public:
    XJNICall(JavaVM *javaVM, JNIEnv *jniEnv, jobject jPlayerObj);

    ~XJNICall();

private:
    void initCreateAudioTrack();

public:
    void callAudioTrackWrite(jbyteArray audioData, int offsetInBytes, int sizeInBytes);

    void callPlayerError(int code, char *msg);


};


#endif //NOTE_XJNICALL_H
