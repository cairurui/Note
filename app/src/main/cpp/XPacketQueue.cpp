#include "XPacketQueue.h"

XPacketQueue::XPacketQueue() {
    pPacketQueue = new std::queue<AVPacket *>();
    pthread_mutex_init(&packetMutex, NULL);
    pthread_cond_init(&packetCond, NULL);
}

XPacketQueue::~XPacketQueue() {
    if (pPacketQueue != NULL) {
        clear();
        delete (pPacketQueue);
        pPacketQueue = NULL;
    }
    pthread_mutex_destroy(&packetMutex);
    pthread_cond_destroy(&packetCond);
}

void XPacketQueue::clear() {
    // 需要清除队列，还需要清除每个 AVPacket* 的内存数据

}

void XPacketQueue::push(AVPacket *pPacket) {
    pthread_mutex_lock(&packetMutex);
    pPacketQueue->push(pPacket);
    pthread_cond_signal(&packetCond);
    pthread_mutex_unlock(&packetMutex);
}

AVPacket *XPacketQueue::pop() {
    AVPacket *pPacket;
    pthread_mutex_lock(&packetMutex);
    while (pPacketQueue->empty()) {
        pthread_cond_wait(&packetCond, &packetMutex);
    }
    pPacket = pPacketQueue->front();
    pPacketQueue->pop();
    pthread_mutex_unlock(&packetMutex);
    return pPacket;
}

