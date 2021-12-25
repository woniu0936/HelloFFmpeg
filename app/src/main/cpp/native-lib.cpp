#include <jni.h>
#include <string>
#include <unistd.h>

extern "C" {
    #include <libavcodec/version.h>
    #include <libavcodec/avcodec.h>
    #include <libavformat/version.h>
    #include <libavutil/version.h>
    #include <libavfilter/version.h>
    #include <libswresample/version.h>
    #include <libswscale/version.h>

    JNIEXPORT jstring JNICALL
    Java_com_woniu0936_ffmpeg_MainActivity_stringFromJNI(
            JNIEnv* env,
            jobject /* this */) {
        std::string hello = "Hello from C++";
        return env->NewStringUTF(hello.c_str());
    }

    JNIEXPORT jstring JNICALL
    Java_com_woniu0936_ffmpeg_MainActivity_ffmpegInfo(
            JNIEnv* env,
            jobject /* this */) {
        char strBuffer[1024 * 4] = {0};
        strcat(strBuffer, "libavcodec : ");
        strcat(strBuffer, AV_STRINGIFY(LIBAVCODEC_VERSION));
        strcat(strBuffer, "\nlibavformat : ");
        strcat(strBuffer, AV_STRINGIFY(LIBAVFORMAT_VERSION));
        strcat(strBuffer, "\nlibavutil : ");
        strcat(strBuffer, AV_STRINGIFY(LIBAVUTIL_VERSION));
        strcat(strBuffer, "\nlibavfilter : ");
        strcat(strBuffer, AV_STRINGIFY(LIBAVFILTER_VERSION));
        strcat(strBuffer, "\nlibswresample : ");
        strcat(strBuffer, AV_STRINGIFY(LIBSWRESAMPLE_VERSION));
        strcat(strBuffer, "\nlibswscale : ");
        strcat(strBuffer, AV_STRINGIFY(LIBSWSCALE_VERSION));
        strcat(strBuffer, "\navcodec_configure : \n");
        strcat(strBuffer, avcodec_configuration());
        strcat(strBuffer, "\navcodec_license : ");
        strcat(strBuffer, avcodec_license());
        return env->NewStringUTF(strBuffer);
    }

}
