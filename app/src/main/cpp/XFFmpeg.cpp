//
// Created by 蔡汝蕊 on 2019-07-22.
//
#include "XFFmpeg.h"

XFFmpeg::XFFmpeg(XJNICall *xJniCall, const char *url) {
    this->xJniCall = xJniCall;
    this->url = (char *) (malloc(strlen(url) + 1));
    memcpy(this->url, url, strlen(url) + 1);

}

XFFmpeg::~XFFmpeg() {
    release();
}

void XFFmpeg::prepareAsync() {

}

void *threadPlay(void *context) {
    XFFmpeg *xfFmpeg = (XFFmpeg *) context;
    xfFmpeg->prepare();
    return 0;
}

void XFFmpeg::play() {
    pthread_t pid;
    pthread_create(&pid, NULL, threadPlay, this);
    pthread_detach(pid);
}

void XFFmpeg::release() {

    if (pCodecContext != NULL) {
        avcodec_close(pCodecContext);
        avcodec_free_context(&pCodecContext);
        pCodecContext = NULL;
    }

    if (pAVFormatContext != NULL) {
        avformat_close_input(&pAVFormatContext);
        avformat_free_context(pAVFormatContext);
        pAVFormatContext = NULL;
    }

    if (swrContext != NULL) {
        swr_free(&swrContext);
        free(swrContext);
        swrContext = NULL;
    }

    if (resampleOutBuffer != NULL) {
        free(resampleOutBuffer);
        resampleOutBuffer = NULL;
    }

    free(url);

    avformat_network_deinit();

}

void XFFmpeg::callPlayerJniError(int code, char *msg) {
//    release();
    xJniCall->callPlayerError(code, msg);
}


void XFFmpeg::prepare() {
    // env 是线程私有的，这里是子线程，env 是主线程的，所以需要重新创建
    xJniCall->changeToChildThread();
    // use of deleted local reference 0x89 对象被删除了，简单点就直接全部重新创建
    xJniCall = new XJNICall(xJniCall->javaVM, xJniCall->jniEnv, xJniCall->jPlayerObj);

    av_register_all();
    avformat_network_init();

    int formatOpenInputRes = 0;
    formatOpenInputRes = avformat_open_input(&pAVFormatContext, url, NULL, NULL);
    if (formatOpenInputRes != 0) {
        LOGE("format open input error: %s", av_err2str(formatOpenInputRes));
        callPlayerJniError(formatOpenInputRes, av_err2str(formatOpenInputRes));
    }

    int formatFindStreamInfoRes = 0;
    formatFindStreamInfoRes = avformat_find_stream_info(pAVFormatContext, NULL);
    if (formatFindStreamInfoRes < 0) {
        LOGE("format find stream info error: %s", av_err2str(formatFindStreamInfoRes));
        callPlayerJniError(formatFindStreamInfoRes, av_err2str(formatFindStreamInfoRes));
    }

    int audioStramIndex = 0;
    audioStramIndex = av_find_best_stream(pAVFormatContext, AVMediaType::AVMEDIA_TYPE_AUDIO, -1, -1,
                                          NULL, 0);
    if (audioStramIndex < 0) {
        LOGE("format audio stream error");
        callPlayerJniError(audioStramIndex, "format audio stream error");
    }

    AVCodecParameters *pCodecParameters;
    AVCodec *pCodec;
    pCodecParameters = pAVFormatContext->streams[audioStramIndex]->codecpar;
    pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (pCodec == NULL) {
        LOGE("codec find audio decoder error");
        callPlayerJniError(CODEC_FIND_DECODER_ERROR_CODE, "codec find audio decoder error");
    }


    pCodecContext = avcodec_alloc_context3(pCodec);
    if (pCodecContext == NULL) {
        LOGE("codec alloc context error");
        callPlayerJniError(CODEC_ALLOC_CONTEXT_ERROR_CODE, "codec alloc context error");
    }

    int codecParametersToContextRes = 0;
    codecParametersToContextRes = avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    if (codecParametersToContextRes < 0) {
        LOGE("codec parameters to context error: %s", av_err2str(codecParametersToContextRes));
        callPlayerJniError(codecParametersToContextRes,
                           ("codec parameters to context error: %s", av_err2str(
                                   codecParametersToContextRes)));
    }

    int codecOpenRes = 0;
    codecOpenRes = avcodec_open2(pCodecContext, pCodec, NULL);
    if (codecOpenRes != 0) {
        LOGE("codec audio open error: %s", av_err2str(codecOpenRes));
        callPlayerJniError(codecOpenRes, ("codec audio open error: %s", av_err2str(codecOpenRes)));
    }



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
        callPlayerJniError(SWR_ALLOC_SET_OPTS_ERROR_CODE, "swr alloc set opts error");
        return;
    }
    int swrInitRes = swr_init(swrContext);
    if (swrInitRes < 0) {
        callPlayerJniError(SWR_CONTEXT_INIT_ERROR_CODE, "swr context swr init error");
        return;
    }
    // size 是播放指定的大小，是最终输出的大小
    int outChannels = av_get_channel_layout_nb_channels(out_ch_layout);
    int dataSize = av_samples_get_buffer_size(NULL, outChannels, pCodecParameters->frame_size,
                                              out_sample_fmt, 0);
    resampleOutBuffer = (uint8_t *) malloc(dataSize);
    // ---------- 重采样 end ----------


    AVPacket *pPacket;
    AVFrame *pFrame;
    pPacket = av_packet_alloc();
    pFrame = av_frame_alloc();

    LOGE("format open input error: 1111111111111111111111111111");

    // native 创建 c 数组
    jbyteArray jPcmByteArray = xJniCall->jniEnv->NewByteArray(dataSize);

    jbyte *jPcmData = xJniCall->jniEnv->GetByteArrayElements(jPcmByteArray, NULL);


    int index = 0;
    while (av_read_frame(pAVFormatContext, pPacket) >= 0) {

        if (pPacket->stream_index == audioStramIndex) {
            int codecSendPacketRes = avcodec_send_packet(pCodecContext, pPacket);
            if (codecSendPacketRes == 0) {
                int codecReceiveFrameRes = avcodec_receive_frame(pCodecContext, pFrame);
                if (codecReceiveFrameRes == 0) {
                    index++;
                    LOGE("解码第 %d 帧", index);

                    // 调用重采样的方法
                    swr_convert(swrContext, &resampleOutBuffer, pFrame->nb_samples,
                                (const uint8_t **) pFrame->data, pFrame->nb_samples);
                    memcpy(jPcmData, resampleOutBuffer, dataSize);


                    // 0 把 c 的数组的数据同步到 jbyteArray , 然后释放native数组
                    xJniCall->jniEnv->ReleaseByteArrayElements(jPcmByteArray, jPcmData,
                                                               JNI_COMMIT); /* 1 copy content, do not free buffer */

                    xJniCall->callAudioTrackWrite(jPcmByteArray, 0, dataSize);
                }
            }
        }

        av_packet_unref(pPacket);
        av_frame_unref(pFrame);
    }

    av_packet_free(&pPacket);
    av_frame_free(&pFrame);

    xJniCall->jniEnv->ReleaseByteArrayElements(jPcmByteArray, jPcmData, 0);

    xJniCall->jniEnv->DeleteLocalRef(jPcmByteArray);

}
