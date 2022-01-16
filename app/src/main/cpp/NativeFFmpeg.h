//
// Created by songhuan on 2022/1/6.
//

#ifndef HELLOFFMPEG_NATIVEFFMPEG_H
#define HELLOFFMPEG_NATIVEFFMPEG_H

#include "JavaCallHelper.h"
#include "VideoChannel.h"
#include "AudioChannel.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}


class NativeFFmpeg {
public:

    NativeFFmpeg(JavaCallHelper *javaCallHelper, const char *dataSource);

    ~NativeFFmpeg();

    void prepare();

    void _prepare();

    void start();

    void _start();

    void setRenderFrameCallback(RenderFrameCallback callback);

private:
    char *dataSource;
    pthread_t pid;
    pthread_t play_pid;
    //AVFormatContext 包含了视频的信息（宽、高等）
    AVFormatContext *avFormatContext = 0;
    //在构造方法里面赋值，所以可以不赋值
    JavaCallHelper *callHelper;
    //指针初始化，一定要赋值，不然就不知道他会指向那块内存了，这时候判空就失效了
    VideoChannel *videoChannel = 0;
    AudioChannel *audioChannel = 0;
    int isPlaying;
    RenderFrameCallback callback;
};


#endif //HELLOFFMPEG_NATIVEFFMPEG_H
