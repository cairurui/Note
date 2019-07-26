
#ifndef MUSICPLAYER_XPACKETQUEUE_H
#define MUSICPLAYER_XPACKETQUEUE_H

#include <queue>
#include <pthread.h>
extern "C"{
#include <libavcodec/avcodec.h>
};

class XPacketQueue {
public:
    std::queue<AVPacket *> *pPacketQueue;
    pthread_mutex_t packetMutex;
    pthread_cond_t packetCond;
public:
    XPacketQueue();

    ~XPacketQueue();

public:
    void push(AVPacket *pPacket);

    AVPacket *pop();

    /**
     * 请求整个队列
     */
    void clear();
};


#endif //MUSICPLAYER_XPACKETQUEUE_H
