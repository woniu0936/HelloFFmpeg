//
// Created by songhuan on 2022/1/6.
//

#include <cstring>
#include <pthread.h>
#include "NativeFFmpeg.h"
#include "macro.h"

void *task_prepare(void *args) {
    NativeFFmpeg *ffmpeg = static_cast<NativeFFmpeg *>(args);
    ffmpeg->_prepare();
    return 0;
}

NativeFFmpeg::NativeFFmpeg(JavaCallHelper *javaCallHelper, const char *dataSource) {
    this->callHelper = javaCallHelper;
    //内存拷贝，以防外面传进来的dataSource被释放之后，这里变成一个悬空指针
    //strlen获得字符串的长度，不包括\0，所以这里+1，不然可能有隐患
    this->dataSource = new char[strlen(dataSource) + 1];
    strcpy(this->dataSource, dataSource);
}

NativeFFmpeg::~NativeFFmpeg() {
    //释放
    DELETE(dataSource);
    DELETE(callHelper);
}

void NativeFFmpeg::prepare() {
    //创建一个线程
    pthread_create(&pid, 0, task_prepare, this);
}

/**
 * 无论编码还是解码，这一部分的代码都是相同的
 */
void NativeFFmpeg::_prepare() {
    //初始化网络，使ffmpeg可以使用网络
    avformat_network_init();
    //1、打开媒体地址（文件地址、直播地址 ）
    //avFormatContext的地址传给avformat_open_input，相当于通过avformat_open_input方法给avFormatContext申请内存
    avFormatContext = 0;
    //ret带边返回值，0：成功，否则失败，例如dataSource不对
    int ret = avformat_open_input(&avFormatContext, dataSource, 0, 0);
    //ret不为0表示失败
    if (ret != 0) {
        callHelper->onError(THREAD_CHILD, FFMPEG_CAN_NOT_OPEN_URL);
        LOGE("视屏url不对: %s", av_err2str(ret));
        return;
    }

    //2、查找多媒体文件中的音视频流，相当于给avFormatContext的内存中赋值
    ret = avformat_find_stream_info(avFormatContext, 0);
    if (ret < 0) {
        //ret小于0，表示失败
        callHelper->onError(THREAD_CHILD, FFMPEG_CAN_NOT_FIND_STREAMS);
        LOGE("查找对媒体文件中的音视频流信息失败: %s", av_err2str(ret));
        return;
    }

    //nb_streams表示有几段音视频流
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        //stream可能代表一个视频，也可能代表一个音频
        AVStream *stream = avFormatContext->streams[i];
        //包含了 解码这段流的各种参数信息（宽、高、码率、帧率等）
        AVCodecParameters *codecpar = stream->codecpar;

        //无论视频还是音频都需要干的一些事情(获得解码器)
        //a、通过当前流使用的编码方式获取解码器
        const AVCodec *codec = avcodec_find_decoder(codecpar->codec_id);
        if (codec == NULL) {
            callHelper->onError(THREAD_CHILD, FFMPEG_FIND_DECODER_FAIL);
            LOGE("获取多媒体文件的解码器失败: %s", av_err2str(ret));
            return;
        }
        //b、获取解码器上下文
        AVCodecContext *context = avcodec_alloc_context3(codec);
        if (context == NULL) {
            callHelper->onError(THREAD_CHILD, FFMPEG_ALLOC_CODEC_CONTEXT_FAIL);
            LOGE("获取解码器的上下文失败: %s", av_err2str(ret));
            return;
        }

        //c、设置上下文中的一些参数()，即将codecpar中的参数拷贝给上下文,同context->width = codecpar->width等一大堆参数
        ret = avcodec_parameters_to_context(context, codecpar);
        if (ret < 0) {
            callHelper->onError(THREAD_CHILD, FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL);
            LOGE("获取上下文中的参数失败: %s", av_err2str(ret));
            return;
        }

        //d、打开解码器
        ret = avcodec_open2(context, codec, 0);
        if (ret != -0) {
            callHelper->onError(THREAD_CHILD, FFMPEG_OPEN_DECODER_FAIL);
            LOGE("打开解码器失败: %s", av_err2str(ret));
            return;
        }

        if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            //音频
            audioChannel = new AudioChannel();
        } else if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频
            videoChannel = new VideoChannel();
        }

        if (!audioChannel && !videoChannel) {
            //如果既没有videoChannel，也没有audioChannel，那就不是一个多媒体文件
            callHelper->onError(THREAD_CHILD, FFMPEG_NOMEDIA);
            LOGE("多媒体文件中没有任何音视频流: %s", av_err2str(ret));
            return;
        }

    }

    //准备完毕，通知java随时可以开始播放
    callHelper->onPrepared(THREAD_CHILD);

}