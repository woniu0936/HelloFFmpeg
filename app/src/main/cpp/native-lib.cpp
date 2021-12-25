#include <jni.h>
#include <string>
#include <unistd.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_woniu0936_ffmpeg_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

//extern "C" {
//    #include <libavcodec/avcodec.h>
//    #include <libavformat/avformat.h>
//    #include <libavfilter/avfilter.h>
//    #include <libavcodec/jni.h>
//
//    JNIEXPORT jstring JNICALL
//    Java_com_woniu0936_ffmpeg_MainActivity_stringFromJNI(
//            JNIEnv* env,
//            jobject /* this */) {
//        std::string hello = "Hello from C++";
//        return env->NewStringUTF(hello.c_str());
//    }
//
//    Java_com_woniu0936_ffmpeg_MainActivity_ffmpegInfo(
//            JNIEnv* env,
//            jobject /* this */) {
//        char info[40000] = {0};
//        AVCodec *c_temp = av_codec_next(NULL);
//        return env->NewStringUTF(hello.c_str());
//    }
//}
