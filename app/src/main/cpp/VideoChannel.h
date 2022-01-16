//
// Created by songhuan on 2022/1/7.
//

#ifndef HELLOFFMPEG_VIDEOCHANNEL_H
#define HELLOFFMPEG_VIDEOCHANNEL_H

#include "BaseChannel.h"

extern "C" {
#include <libswscale/swscale.h>
}

//创建一个函数指针
typedef void (*RenderFrameCallback)(uint8_t *, int, int, int);

/**
 * 1、解码
 * 2、播放
 */
class VideoChannel : public BaseChannel {
public:
    VideoChannel(int id, AVCodecContext *avCodecContext);

    //解码+播放
    void play();

    void decode();

    void render();

    void setRenderFrameCallback(RenderFrameCallback callback);

private:
    pthread_t pid_decode;
    pthread_t pid_render;

    SwsContext *swsContext;
    RenderFrameCallback callback;
};


#endif //HELLOFFMPEG_VIDEOCHANNEL_H
