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

private:
    char *dataSource;
    pthread_t pid;
    //AVFormatContext 包含了视频的信息（宽、高等）
    AVFormatContext *avFormatContext;
    JavaCallHelper *callHelper;
    VideoChannel *videoChannel;
    AudioChannel *audioChannel;
};


#endif //HELLOFFMPEG_NATIVEFFMPEG_H
