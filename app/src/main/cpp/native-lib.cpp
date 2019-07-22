#include <jni.h>

#include "XConstDefine.h"

#include "XJNICall.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
}

XJNICall *pJniCall;


extern "C" JNIEXPORT void JNICALL
Java_com_xc_note_media_XPlayer_nPlay(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);
    pJniCall = new XJNICall(NULL, env, instance);


    av_register_all();
    avformat_network_init();

    AVFormatContext *pAVFormatContext = 0;
    int formatOpenInputRes = 0;


    formatOpenInputRes = avformat_open_input(&pAVFormatContext, url, NULL, NULL);
    if (formatOpenInputRes != 0) {
        LOGE("format open input error: %s", av_err2str(formatOpenInputRes));
        // todo release
    }

    int formatFindStreamInfoRes = 0;
    formatFindStreamInfoRes = avformat_find_stream_info(pAVFormatContext, NULL);
    if (formatFindStreamInfoRes < 0) {
        LOGE("format find stream info error: %s", av_err2str(formatFindStreamInfoRes));
        // todo release
    }


    int audioStramIndex = 0;
    audioStramIndex = av_find_best_stream(pAVFormatContext, AVMediaType::AVMEDIA_TYPE_AUDIO, -1, -1,
                                          NULL, 0);
    if (audioStramIndex < 0) {
        LOGE("format audio stream error: %s");
        // 这种方式一般不推荐这么写，但是的确方便
        // todo release
    }

    AVCodecParameters *pCodecParameters;
    AVCodec *pCodec;
    pCodecParameters = pAVFormatContext->streams[audioStramIndex]->codecpar;
    pCodec = avcodec_find_decoder(pCodecParameters->codec_id);
    if (pCodec == NULL) {
        LOGE("codec find audio decoder error");
        // 这种方式一般不推荐这么写，但是的确方便
        // todo release
    }

    AVCodecContext *pCodecContext;
    pCodecContext = avcodec_alloc_context3(pCodec);
    if (pCodecContext == NULL) {
        LOGE("codec alloc context error");
        // 这种方式一般不推荐这么写，但是的确方便
        // todo release
    }

    int codecParametersToContextRes = 0;
    codecParametersToContextRes = avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    if (codecParametersToContextRes < 0) {
        LOGE("codec parameters to context error: %s", av_err2str(codecParametersToContextRes));
        // 这种方式一般不推荐这么写，但是的确方便
        // todo release
    }

    int codecOpenRes = 0;
    codecOpenRes = avcodec_open2(pCodecContext, pCodec, NULL);
    if (codecOpenRes != 0) {
        LOGE("codec audio open error: %s", av_err2str(codecOpenRes));
        // 这种方式一般不推荐这么写，但是的确方便
        // todo release
    }

    SwrContext *swrContext;

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
//        callPlayerJniError(SWR_ALLOC_SET_OPTS_ERROR_CODE, "swr alloc set opts error");
        return;
    }
    int swrInitRes = swr_init(swrContext);
    if (swrInitRes < 0) {
//        callPlayerJniError(SWR_CONTEXT_INIT_ERROR_CODE, "swr context swr init error");
        return;
    }
    // size 是播放指定的大小，是最终输出的大小
    int outChannels = av_get_channel_layout_nb_channels(out_ch_layout);
    int dataSize = av_samples_get_buffer_size(NULL, outChannels, pCodecParameters->frame_size,
                                              out_sample_fmt, 0);
    uint8_t *resampleOutBuffer = (uint8_t *) malloc(dataSize);
    // ---------- 重采样 end ----------


    jclass jAudioTrackClass;
    jmethodID jWriteMid;
    jobject jAudioTrackObj;
    jAudioTrackClass = env->FindClass("android/media/AudioTrack");
    jWriteMid = env->GetMethodID(jAudioTrackClass, "write", "([BII)I");


    AVPacket *pPacket;
    AVFrame *pFrame;
    pPacket = av_packet_alloc();
    pFrame = av_frame_alloc();

    // native 创建 c 数组
    jbyteArray jPcmByteArray = env->NewByteArray(dataSize);
    jbyte *jPcmData = env->GetByteArrayElements(jPcmByteArray, NULL);


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
                    env->ReleaseByteArrayElements(jPcmByteArray, jPcmData,
                                                  JNI_COMMIT); /* 1 copy content, do not free buffer */

                    pJniCall->callAudioTrackWrite(jPcmByteArray, 0, dataSize);

                }


            }


        }

        av_packet_unref(pPacket);
        av_frame_unref(pFrame);
    }


    env->ReleaseByteArrayElements(jPcmByteArray, jPcmData, 0);

    env->DeleteLocalRef(jPcmByteArray);

    av_packet_free(&pPacket);
    av_frame_free(&pFrame);
    env->DeleteLocalRef(jAudioTrackObj);

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
    avformat_network_deinit();

    env->ReleaseStringUTFChars(url_, url);
}