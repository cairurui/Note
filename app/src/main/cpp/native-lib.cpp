#include <jni.h>

#include "XConstDefine.h"

extern "C" {
#include "libavformat/avformat.h"
}


jobject initCreateAudioTrack(JNIEnv *env) {
    /*AudioTrack(int streamType, int sampleRateInHz, int channelConfig, int audioFormat,
            int bufferSizeInBytes, int mode)*/
    jclass jAudioTrackClass = env->FindClass("android/media/AudioTrack");
    jmethodID jAudioTackCMid = env->GetMethodID(jAudioTrackClass, "<init>", "(IIIIII)V");

    int streamType = 3;
    int sampleRateInHz = AUDIO_SAMPLE_RATE;
    int channelConfig = (0x4 | 0x8);
    int audioFormat = 2;
    int mode = 1;

    // int getMinBufferSize(int sampleRateInHz, int channelConfig, int audioFormat)
    jmethodID getMinBufferSizeMid = env->GetStaticMethodID(jAudioTrackClass, "getMinBufferSize",
                                                           "(III)I");
    int bufferSizeInBytes = env->CallStaticIntMethod(jAudioTrackClass, getMinBufferSizeMid,
                                                     sampleRateInHz, channelConfig, audioFormat);
    LOGE("bufferSizeInBytes = %d", bufferSizeInBytes);

    jobject jAudioTrackObj = env->NewObject(jAudioTrackClass, jAudioTackCMid, streamType,
                                            sampleRateInHz, channelConfig, audioFormat,
                                            bufferSizeInBytes, mode);

    // play
    jmethodID playMid = env->GetMethodID(jAudioTrackClass, "play", "()V");
    env->CallVoidMethod(jAudioTrackObj, playMid);

    return jAudioTrackObj;
}

extern "C" JNIEXPORT void JNICALL
Java_com_xc_note_media_XPlayer_nPlay(JNIEnv *env, jobject instance, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);

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

    jclass jAudioTrackClass;
    jmethodID jWriteMid;
    jobject jAudioTrackObj;
    jAudioTrackClass = env->FindClass("android/media/AudioTrack");
    jWriteMid = env->GetMethodID(jAudioTrackClass, "write", "([BII)I");
    jAudioTrackObj = initCreateAudioTrack(env);

    AVPacket *pPacket;
    AVFrame *pFrame;
    pPacket = av_packet_alloc();
    pFrame = av_frame_alloc();
    int index = 0;
    while (av_read_frame(pAVFormatContext, pPacket) >= 0) {

        if (pPacket->stream_index == audioStramIndex) {
            int codecSendPacketRes = avcodec_send_packet(pCodecContext, pPacket);
            if (codecSendPacketRes == 0) {
                int codecReceiveFrameRes = avcodec_receive_frame(pCodecContext, pFrame);
                if (codecReceiveFrameRes == 0) {
                    index++;
                    LOGE("解码第 %d 帧", index);

                    int dataSize = av_samples_get_buffer_size(NULL, pFrame->channels,
                                                              pFrame->nb_samples,
                                                              pCodecContext->sample_fmt, 0);
                    jbyteArray jPcmByteArray = env->NewByteArray(dataSize);
                    jbyte *jPcmData = env->GetByteArrayElements(jPcmByteArray, NULL);
                    memcpy(jPcmData, pFrame->data, dataSize);

                    env->ReleaseByteArrayElements(jPcmByteArray, jPcmData, 0);
                    env->CallIntMethod(jAudioTrackObj, jWriteMid, jPcmByteArray, 0, dataSize);
                    env->DeleteLocalRef(jPcmByteArray);

                }


            }


        }

        av_packet_unref(pPacket);
        av_frame_unref(pFrame);
    }
    av_packet_free(&pPacket);
    av_frame_free(&pFrame);
    env->DeleteLocalRef(jAudioTrackObj);

    __av_resources_destroy:
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