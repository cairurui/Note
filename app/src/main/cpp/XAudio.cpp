//
// Created by xiaocai on 2019/7/24.
//


#include "XAudio.h"

XAudio::XAudio(int audioStreamIndex, XJNICall *pJniCall, AVFormatContext *pFormatContext) {
    this->audioStreamIndex = audioStreamIndex;
    this->xJniCall = pJniCall;
    this->pFormatContext = pFormatContext;

    xPlayerStatus = new XPlayerStatus();
    xPacketQueue = new XPacketQueue();


}

void *threadPlay(void *context) {
    XAudio *xAudio = (XAudio *) context;
    xAudio->initCrateOpenSLES();
    return 0;
}

void *threadReadPacket(void *context) {
    XAudio *xAudio = (XAudio *) context;
    while (xAudio->xPlayerStatus != NULL && !xAudio->xPlayerStatus->isExit) {
        AVPacket *pPacket = av_packet_alloc();
        if (av_read_frame(xAudio->pFormatContext, pPacket) >= 0) {
            if (pPacket->stream_index == xAudio->audioStreamIndex) {
                xAudio->xPacketQueue->push(pPacket);
            } else {
                av_packet_free(&pPacket);
            }
        } else {
            av_packet_free(&pPacket);
        }
    }

    return 0;
}

void XAudio::play() {
    pthread_t pthread_t_read;
    pthread_create(&pthread_t_read, NULL, threadReadPacket, this);
    pthread_detach(pthread_t_read);

    pthread_t pid;
    pthread_create(&pid, NULL, threadPlay, this);
    pthread_detach(pid);
}

int XAudio::resampleAudio() {
    int dataSize = 0;
    AVPacket *pPacket = NULL;
    AVFrame *pFrame = av_frame_alloc();
    int index = 0;
    while (xPlayerStatus != NULL && !xPlayerStatus->isExit) {
        pPacket = xPacketQueue->pop();
        int codecSendPacketRes = avcodec_send_packet(pCodecContext, pPacket);
        if (codecSendPacketRes == 0) {
            int codecReceiveFrameRes = avcodec_receive_frame(pCodecContext, pFrame);
            if (codecReceiveFrameRes == 0) {
                index++;
                LOGE("解码第 %d 帧", index);
                // 调用重采样的方法
                int nb = swr_convert(swrContext, &resampleOutBuffer, pFrame->nb_samples,
                                     (const uint8_t **) pFrame->data, pFrame->nb_samples);


                int outChannels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
                dataSize = nb * outChannels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
                currentTime = pFrame->pts * av_q2d(timeBase);
                break;
            }
        }
        av_packet_unref(pPacket);
        av_frame_unref(pFrame);
    }

    av_packet_free(&pPacket);
    av_frame_free(&pFrame);
    return dataSize;
}

void playerCallback(SLAndroidSimpleBufferQueueItf caller, void *pContext) {
    XAudio *xAudio = (XAudio *) pContext;
    if (xAudio != NULL) {
        int bufferSize = xAudio->resampleAudio();
        if (bufferSize > 0) {
            xAudio->currentTime += bufferSize / ((double) (xAudio->sampleRate * 2 * 2));

            // 1s 回调更新一次进度
            if (xAudio->currentTime - xAudio->lastTime > 1) {
                xAudio->lastTime = xAudio->currentTime;
                if (xAudio->xJniCall != NULL) {
                    xAudio->xJniCall->onCallProgress(THREAD_CHILD, xAudio->currentTime,
                                                     xAudio->duration);
                }
            }

            if (xAudio->duration > 0 && xAudio->duration <= xAudio->currentTime) {
                xAudio->xJniCall->onCallComplete(THREAD_CHILD);
            }

            (*caller)->Enqueue(caller, (char *) xAudio->resampleOutBuffer, bufferSize);
        }
    }

//    int dataSize = xAudio->resampleAudio();
//    // 这里为什么报错，留在后面再去解决
//    (*caller)->Enqueue(caller, xAudio->resampleOutBuffer, dataSize);
}


void XAudio::initCrateOpenSLES() {
    // 3.1 创建引擎接口对象
    SLObjectItf engineObject = NULL;
    SLEngineItf engineEngine;
    slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    // realize the engine
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    // get the engine interface, which is needed in order to create other objects
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);

    // 3.2 设置混音器
    static SLObjectItf outputMixObject = NULL;
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                     &outputMixEnvironmentalReverb);
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb,
                                                                      &reverbSettings);
    // 3.3 创建播放器
    SLObjectItf pPlayer = NULL;
    SLDataLocator_AndroidSimpleBufferQueue simpleBufferQueue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM formatPcm = {
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN};
    SLDataSource audioSrc = {&simpleBufferQueue, &formatPcm};
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&outputMix, NULL};
    SLInterfaceID interfaceIds[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_PLAYBACKRATE};
    SLboolean interfaceRequired[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    (*engineEngine)->CreateAudioPlayer(engineEngine, &pPlayer, &audioSrc, &audioSnk, 3,
                                       interfaceIds, interfaceRequired);
    (*pPlayer)->Realize(pPlayer, SL_BOOLEAN_FALSE);
    (*pPlayer)->GetInterface(pPlayer, SL_IID_PLAY, &slPlayItf);
    // 3.4 设置缓存队列和回调函数
    SLAndroidSimpleBufferQueueItf playerBufferQueue;
    (*pPlayer)->GetInterface(pPlayer, SL_IID_BUFFERQUEUE, &playerBufferQueue);
    (*playerBufferQueue)->RegisterCallback(playerBufferQueue, playerCallback, this);
    // 3.5 设置播放状态
    (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PLAYING);
    // 3.6 调用回调函数
    playerCallback(playerBufferQueue, this);
}

XAudio::~XAudio() {
    release();
}

void XAudio::release() {

    stopAudio();

    if (xPacketQueue != NULL) {
        xPacketQueue->clear();
        delete (xPacketQueue);
        xPacketQueue = NULL;
    }
    if (xPlayerStatus != NULL) {
        xPlayerStatus->isExit = true;
        delete (xPlayerStatus);
        xPlayerStatus = NULL;
    }
    if (resampleOutBuffer) {
        free(resampleOutBuffer);
        resampleOutBuffer = NULL;
    }
    if (pCodecContext != NULL) {
        avcodec_close(pCodecContext);
        avcodec_free_context(&pCodecContext);
        pCodecContext = NULL;
    }
    if (swrContext != NULL) {
        swr_close(swrContext);
        swr_free(&swrContext);
        swrContext = NULL;
    }

}


void XAudio::callPlayerJniError(ThreadMode threadMode, int code, char *msg) {
    release();
    xJniCall->callPlayerError(threadMode, code, msg);
}


void XAudio::analysisStream(ThreadMode threadMode, AVStream **pStream) {

    AVCodecParameters *pCodecParameters;
    AVCodec *pCodec;
    pCodecParameters = pFormatContext->streams[audioStreamIndex]->codecpar;
    pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (pCodec == NULL) {
        LOGE("codec find audio decoder error");
        callPlayerJniError(threadMode, CODEC_FIND_DECODER_ERROR_CODE,
                           "codec find audio decoder error");
    }

    pCodecContext = avcodec_alloc_context3(pCodec);
    if (pCodecContext == NULL) {
        LOGE("codec alloc context error");
        callPlayerJniError(threadMode, CODEC_ALLOC_CONTEXT_ERROR_CODE, "codec alloc context error");
    }

    int codecParametersToContextRes = 0;
    codecParametersToContextRes = avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    if (codecParametersToContextRes < 0) {
        LOGE("codec parameters to context error: %s", av_err2str(codecParametersToContextRes));
        callPlayerJniError(threadMode, codecParametersToContextRes,
                           ("codec parameters to context error: %s", av_err2str(
                                   codecParametersToContextRes)));
    }

    int codecOpenRes = 0;
    codecOpenRes = avcodec_open2(pCodecContext, pCodec, NULL);
    if (codecOpenRes != 0) {
        LOGE("codec audio open error: %s", av_err2str(codecOpenRes));
        callPlayerJniError(threadMode, codecOpenRes,
                           ("codec audio open error: %s", av_err2str(codecOpenRes)));
    }

    this->sampleRate = pCodecContext->frame_size;
    this->duration = pFormatContext->duration / AV_TIME_BASE;
    this->timeBase = pFormatContext->streams[audioStreamIndex]->time_base;


    // ---------- 重采样 start ----------
    int64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
    enum AVSampleFormat out_sample_fmt = AVSampleFormat::AV_SAMPLE_FMT_S16;
    int out_sample_rate = AUDIO_SAMPLE_RATE;
    int64_t in_ch_layout = pCodecContext->channel_layout;
    enum AVSampleFormat in_sample_fmt = pCodecContext->sample_fmt;
    int in_sample_rate = pCodecContext->sample_rate;
    swrContext = swr_alloc_set_opts(NULL, out_ch_layout, out_sample_fmt,
                                    out_sample_rate, in_ch_layout, in_sample_fmt, in_sample_rate, 0,
                                    NULL);
    if (swrContext == NULL) {
        // 提示错误
        callPlayerJniError(threadMode, SWR_ALLOC_SET_OPTS_ERROR_CODE, "swr alloc set opts error");
        return;
    }

    int swrInitRes = swr_init(swrContext);
    if (swrInitRes < 0) {
        callPlayerJniError(threadMode, SWR_CONTEXT_INIT_ERROR_CODE, "swr context swr init error");
        return;
    }

    // ---------- 重采样 end ----------
    resampleOutBuffer = (uint8_t *) malloc(pCodecContext->frame_size * 2 * 2);
}

void XAudio::pauseAudio() {
    if (slPlayItf != NULL) {
        (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PAUSED);
    }
}

void XAudio::resumeAudio() {
    if (slPlayItf != NULL) {
        (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PLAYING);
    }
}

void XAudio::stopAudio() {
    if (slPlayItf != NULL) {
        (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_STOPPED);
    }
}
