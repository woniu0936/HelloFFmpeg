//
// Created by songhuan on 2022/1/7.
//

#ifndef HELLOFFMPEG_AUDIOCHANNEL_H
#define HELLOFFMPEG_AUDIOCHANNEL_H


#include "BaseChannel.h"

class AudioChannel : public BaseChannel{
public:
    AudioChannel(int id, AVCodecContext *avCodecContext);

    void play();
};


#endif //HELLOFFMPEG_AUDIOCHANNEL_H
