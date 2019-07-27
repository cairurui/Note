//
// Created by 蔡汝蕊 on 2019-07-22.
//
#include "XFFmpeg.h"

XFFmpeg::XFFmpeg(XJNICall *xJniCall, const char *url) {
    this->xJniCall = xJniCall;
    this->url = (char *) (malloc(strlen(url) + 1));
    memcpy(this->url, url, strlen(url) + 1);
    pthread_mutex_init(&releaseMutex, NULL);
}

XFFmpeg::~XFFmpeg() {
    release();
}

void *threadPrepare(void *context) {
    XFFmpeg *xfFmpeg = (XFFmpeg *) context;
    xfFmpeg->prepare(THREAD_CHILD);
    return 0;
}

void XFFmpeg::prepareAsync() {
    pthread_t pid;
    pthread_create(&pid, NULL, threadPrepare, this);
    pthread_detach(pid);
}

void XFFmpeg::prepare() {
    prepare(THREAD_MAIN);
}


void XFFmpeg::play() {
    if (xAudio != NULL) {
        xAudio->play();
    }
}

void XFFmpeg::release() {
    if (xAudio == NULL) {
        return;
    }

    pthread_mutex_lock(&releaseMutex);

    if (xAudio != NULL && xAudio->xPlayerStatus->isExit) {
        return;
    }


    if (xAudio != NULL) {
        xAudio->release();
        delete (xAudio);
        xAudio = NULL;
    }

    if (pAVFormatContext != NULL) {
        avformat_close_input(&pAVFormatContext);
        avformat_free_context(pAVFormatContext);
        pAVFormatContext = NULL;
    }
    avformat_network_deinit();
    if (url != NULL) {
        free(url);
        url == NULL;
    }

    pthread_mutex_unlock(&releaseMutex);
    pthread_mutex_destroy(&releaseMutex);
}

void XFFmpeg::callPlayerJniError(ThreadMode threadMode, int code, char *msg) {
    release();
    xJniCall->callPlayerError(threadMode, code, msg);
}


void XFFmpeg::prepare(ThreadMode threadMode) {
    av_register_all();
    avformat_network_init();

    int formatOpenInputRes = 0;
    formatOpenInputRes = avformat_open_input(&pAVFormatContext, url, NULL, NULL);
    if (formatOpenInputRes != 0) {
        LOGE("format open input error: %s", av_err2str(formatOpenInputRes));
        callPlayerJniError(threadMode, formatOpenInputRes, av_err2str(formatOpenInputRes));
    }

    int formatFindStreamInfoRes = 0;
    formatFindStreamInfoRes = avformat_find_stream_info(pAVFormatContext, NULL);
    if (formatFindStreamInfoRes < 0) {
        LOGE("format find stream info error: %s", av_err2str(formatFindStreamInfoRes));
        callPlayerJniError(threadMode, formatFindStreamInfoRes,
                           av_err2str(formatFindStreamInfoRes));
    }

    int audioStramIndex = 0;
    audioStramIndex = av_find_best_stream(pAVFormatContext, AVMediaType::AVMEDIA_TYPE_AUDIO, -1, -1,
                                          NULL, 0);
    if (audioStramIndex < 0) {
        LOGE("format audio stream error");
        callPlayerJniError(threadMode, audioStramIndex, "format audio stream error");
    }

    xAudio = new XAudio(audioStramIndex, xJniCall, pAVFormatContext);
    xAudio->analysisStream(threadMode, pAVFormatContext->streams);

    xJniCall->callPlayerPrepared(threadMode);

}

void XFFmpeg::onPause() {
    if (xAudio!=NULL){
        xAudio->pauseAudio();
    }
}

void XFFmpeg::onResume() {
    if (xAudio!=NULL){
        xAudio->resumeAudio();
    }
}
