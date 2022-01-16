//
// Created by songhuan on 2022/1/7.
//

#include "VideoChannel.h"
#include "macro.h"

extern "C" {
#include <libavutil/imgutils.h>
}

void *task_decode(void *args) {
    VideoChannel *channel = static_cast<VideoChannel *>(args);
    channel->decode();
    return 0;
}

void *task_render(void *args) {
    VideoChannel *channel = static_cast<VideoChannel *>(args);
    channel->render();
    return 0;
}

VideoChannel::VideoChannel(int id, AVCodecContext *avCodecContext) : BaseChannel(id,
                                                                                 avCodecContext) {  //将子类的id传递给父类

}

void VideoChannel::play() {
    isPlaying = 1;
    //如果VideoChannel有值，则让其消息队列开启工作模式
    avPackets.setWork(1);
    avFrames.setWork(1);
    //1、解码
    pthread_create(&pid_decode, 0, task_decode, this);
    //2、播放
    pthread_create(&pid_render, 0, task_render, this);

}

//解码
void VideoChannel::decode() {
    AVPacket *avPacket = 0;
    LOGE("--------------VideoChannel::decode, isPlaying:%d----------------------", isPlaying);
    while (isPlaying) {
        //取出一个数据包
        int ret = avPackets.deQueue(avPacket);
//        LOGE("--------------deQueue-avPacket----------------------");
        if (!isPlaying) {
            //由于avPackets.deQueue在取数据的时候，如果没有数据，则会等待，所以这里在拿到数据之后，再判断一下是否还在播放
            break;
        }
        if (!ret) {
            LOGE("--------------deQueue-avPacket----ret:%s------------------", av_err2str(ret));
            //如果拿取数据失败，则处理下一帧
            continue;
        }
        //把包丢给解码器
        ret = avcodec_send_packet(avCodecContext, avPacket);
        //send出去之后avPacket就可以释放了
        releaseAvPacket(&avPacket);
//        if (ret == AVERROR(EAGAIN)) {
//            //重试,这时候需要avcodec_send_frame先调用一下
//            continue;
//        } else if (ret != 0) {
//            //出错退出
//            break;
//        }
        if (ret != 0) {
            //出错退出
            LOGE("------------------------decode avcodec_send_packet error: %s------------------------------------", av_err2str(ret));
            break;
        }

        //代表了一个图像
        AVFrame *avFrame = av_frame_alloc();
        //从解码器中读取解码后的数据包 AVFrame
        ret = avcodec_receive_frame(avCodecContext, avFrame);
        if (ret == AVERROR(EAGAIN)) {
            //需要更多的数据才能够进行解码
            continue;
        } else if (ret != 0) {
            break;
        }
        avFrames.enQueue(avFrame);
        //再开一个线程，用来播放
    }
    releaseAvPacket(&avPacket);
}

/**
 * 渲染播放
 */
void VideoChannel::render() {
    //目标 RGBA
    swsContext = sws_getContext(avCodecContext->width, avCodecContext->height, avCodecContext->pix_fmt,
                                avCodecContext->width, avCodecContext->height, AV_PIX_FMT_RGBA,
                                SWS_BILINEAR, NULL, NULL, NULL);
    AVFrame *avFrame = 0;
    //数组长度定义为4的原因是AV_PIX_FMT_RGBA中有R、G、B、A四位
    //指针数组
    uint8_t *dst_data[4];
    int dst_linesize[4];
    //申请内存
    av_image_alloc(dst_data, dst_linesize,
                   avCodecContext->width, avCodecContext->height, AV_PIX_FMT_RGBA, 1);
    while (isPlaying) {
        int ret = avFrames.deQueue(avFrame);
        LOGE("------------------------render isPlaying, deQueue result: %d------------------------------------", ret);
        if (!isPlaying) {
            break;
        }
        if (!ret) {
            continue;
        }
        //avFrame->linesize表示每一行存放的字节的长度
        sws_scale(swsContext, avFrame->data, avFrame->linesize, 0, avCodecContext->height,
                  dst_data,
                  dst_linesize);

        //这里传递0，是因为数据都存放在0的位置
        callback(dst_data[0], dst_linesize[0], avCodecContext->width, avCodecContext->height);
        releaseAVFrame(&avFrame);
    }

    //释放内存
    av_freep(&dst_data[0]);
    releaseAVFrame(&avFrame);
}

void VideoChannel::setRenderFrameCallback(RenderFrameCallback callback) {
    this->callback = callback;
}