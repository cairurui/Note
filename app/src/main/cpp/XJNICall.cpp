//
// Created by 蔡汝蕊 on 2019-07-22.
//
#include "XConstDefine.h"
#include "XJNICall.h"

XJNICall::XJNICall(JavaVM *javaVM, JNIEnv *env, jobject jPlayerObj) {
    this->javaVM = javaVM;
    this->jniEnv = env;
    this->jPlayerObj = jniEnv->NewGlobalRef(jPlayerObj);

    jclass jPlayerClass = jniEnv->GetObjectClass(jPlayerObj);
    jPlayerErrorMid = jniEnv->GetMethodID(jPlayerClass, "onError", "(ILjava/lang/String;)V");
    jPlayerPreparedMid = jniEnv->GetMethodID(jPlayerClass, "onPrepared", "()V");
    jPlayerProgressMid = jniEnv->GetMethodID(jPlayerClass, "onProgress", "(II)V");
}


XJNICall::~XJNICall() {
    jniEnv->DeleteGlobalRef(jPlayerObj);
}

void XJNICall::callPlayerError(ThreadMode threadMode, int code, char *msg) {
    if (threadMode == THREAD_MAIN) {
        jstring jstr = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jPlayerObj, jPlayerErrorMid, code, jstr);
        jniEnv->DeleteLocalRef(jstr);
    } else if (threadMode == THREAD_CHILD) {
        JNIEnv *env;
        if (javaVM->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE("get child thread jniEnv error!");
            return;
        }

        jstring jMsg = env->NewStringUTF(msg);
        env->CallVoidMethod(jPlayerObj, jPlayerErrorMid, code, jMsg);
        env->DeleteLocalRef(jMsg);

        javaVM->DetachCurrentThread();
    }

}


void XJNICall::callPlayerPrepared(ThreadMode mode) {
    if (mode == THREAD_MAIN) {
        jniEnv->CallVoidMethod(jPlayerObj, jPlayerPreparedMid);
    } else if (mode == THREAD_CHILD) {
        JNIEnv *env;
        if (javaVM->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE("get child thread jniEnv error!");
            return;
        }
        env->CallVoidMethod(jPlayerObj, jPlayerPreparedMid);
        javaVM->DetachCurrentThread();
    }

}

void XJNICall::onCallProgress(ThreadMode threadMode, int current, int total) {
    if (threadMode == THREAD_MAIN) {
        jniEnv->CallVoidMethod(jPlayerObj, jPlayerProgressMid, current, total);
    } else if (threadMode == THREAD_CHILD) {
        JNIEnv *env;
        if (javaVM->AttachCurrentThread(&env, 0) != JNI_OK) {
            LOGE("get child thread jniEnv error!");
            return;
        }
        env->CallVoidMethod(jPlayerObj, jPlayerProgressMid, current, total);
        javaVM->DetachCurrentThread();
    }

}

void XJNICall::onCallComplete(ThreadMode mode) {

}


