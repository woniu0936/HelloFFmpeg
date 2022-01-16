//
// Created by songhuan on 2022/1/8.
//

#ifndef HELLOFFMPEG_BASECHANNEL_H
#define HELLOFFMPEG_BASECHANNEL_H

#include "safe_queue.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
};

class BaseChannel {
public:
    BaseChannel(int id, AVCodecContext *avCodecContext) : id(id), avCodecContext(avCodecContext) {
        avPackets.setReleaseHandle(BaseChannel::releaseAvPacket);
        avFrames.setReleaseHandle(BaseChannel::releaseAVFrame);
    }

    //virtual
    virtual ~BaseChannel() {
        avPackets.clear();
        avFrames.clear();
    }

    /**
     * 释放
     * @param avPacket
     */
    static void releaseAvPacket(AVPacket **avPacket) {
        if (avPacket) {
            //释放avPacket需要使用特定的函数，就像申请内存的时候，也要使用特定的函数
            av_packet_free(avPacket);
            //为什么用指针的指针？
            //指针的指针能够修改传递进来的指针的指向
            *avPacket = 0;
        }
    }

    /**
    * 释放
    * @param avPacket
    */
    static void releaseAVFrame(AVFrame **avFrame) {
        if (avFrame) {
            //释放avPacket需要使用特定的函数，就像申请内存的时候，也要使用特定的函数
            av_frame_free(avFrame);
            //为什么用指针的指针？
            //指针的指针能够修改传递进来的指针的指向
            *avFrame = 0;
        }
    }

    //纯虚方法，相当于抽象方法
    virtual void play() = 0;

    int isPlaying = 0;
    int id;
    AVCodecContext *avCodecContext;
    //编码数据包队列
    SafeQueue<AVPacket *> avPackets;
    //解码数据包队列
    SafeQueue<AVFrame *> avFrames;
};

#endif //HELLOFFMPEG_BASECHANNEL_H
