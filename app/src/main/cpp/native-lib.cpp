#include <jni.h>
#include <string>
#include <unistd.h>
#include "NativeFFmpeg.h"
#include "macro.h"

extern "C" {
#include <libavcodec/version.h>
#include <libavcodec/avcodec.h>
#include <libavformat/version.h>
#include <libavutil/version.h>
#include <libavfilter/version.h>
#include <libswresample/version.h>
#include <libswscale/version.h>

JNIEXPORT jstring JNICALL
Java_com_woniu0936_ffmpeg_FFPlayer_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT jstring JNICALL
Java_com_woniu0936_ffmpeg_FFPlayer_ffmpegInfo(
        JNIEnv *env,
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

NativeFFmpeg *ffmpeg = 0;
JavaVM *javaVM = 0;

int JNI_OnLoad(JavaVM *vm, void *r) {
    javaVM = vm;
    LOGE("JNI_OnLoad");
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_woniu0936_ffmpeg_FFPlayer_nativePrepare(JNIEnv *env, jobject thiz, jstring data_source) {
    const char *dataSource = env->GetStringUTFChars(data_source, 0);
    LOGE("native prepare dateSource:%s", dataSource);
    //创建一个播放器
    JavaCallHelper *javaCallHelper = new JavaCallHelper(javaVM, env, thiz);
    ffmpeg = new NativeFFmpeg(javaCallHelper, dataSource);
    LOGE("native prepare");
    ffmpeg->prepare();
    env->ReleaseStringUTFChars(data_source, dataSource);


//    jclass  cls = env->GetObjectClass(thiz);
//    jmethodID  onErrorId = env->GetMethodID(cls, "onError", "(I)V");
//    env->CallVoidMethod(thiz, onErrorId, 100);
//    jmethodID  onPrepareId = env->GetMethodID(cls, "onPrepare", "()V");
//    env->CallVoidMethod(thiz, onPrepareId);
}