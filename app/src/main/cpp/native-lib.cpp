#include <jni.h>
#include <string>
#include <unistd.h>
#include <android/native_window_jni.h>
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
ANativeWindow *window = 0;
//静态锁
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * 画画的
 */
void render(uint8_t *data, int linesize, int w, int h) {
    LOGE("render frame lock");
    //添加同步锁，以防和nativeSetSurface方法中操作window冲突，导致不可预期的线程安全问题
    pthread_mutex_lock(&mutex);
    if (!window) {
        //window如果没有值，则直接返回
        return;
    }
    LOGE("set window");
    //设置窗口属性
    ANativeWindow_setBuffersGeometry(window, w, h, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;
    if (ANativeWindow_lock(window, &windowBuffer, 0)) {
        ANativeWindow_release(window);
        window = 0;
        return;
    }
    LOGE("copy data");
    //填充RGBA数据给data
    uint8_t *dst_data = static_cast<uint8_t *>(windowBuffer.bits);
    //stride: 一行有多少个数据(RGBA)*4
    int dst_linesize = windowBuffer.stride * 4;
    //一行一行的拷贝
    for (int i = 0; i < windowBuffer.height; ++i) {
        //这里+i表示一行一行的拷贝，参见指针+i，指向下一段内存
        memcpy(dst_data + i * dst_linesize, data + i * linesize, dst_linesize);
    }
    LOGE("unlock");
    ANativeWindow_unlockAndPost(window);
    pthread_mutex_unlock(&mutex);
}

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
    ffmpeg->setRenderFrameCallback(render);
    ffmpeg->prepare();
    env->ReleaseStringUTFChars(data_source, dataSource);


//    jclass  cls = env->GetObjectClass(thiz);
//    jmethodID  onErrorId = env->GetMethodID(cls, "onError", "(I)V");
//    env->CallVoidMethod(thiz, onErrorId, 100);
//    jmethodID  onPrepareId = env->GetMethodID(cls, "onPrepare", "()V");
//    env->CallVoidMethod(thiz, onPrepareId);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_woniu0936_ffmpeg_FFPlayer_nativeStart(JNIEnv *env, jobject thiz) {
    ffmpeg->start();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_woniu0936_ffmpeg_FFPlayer_nativeSetSurface(JNIEnv *env, jobject thiz, jobject surface) {
    //添加同步锁，以防和render方法中操作window冲突，导致不可预期的线程安全问题
    pthread_mutex_lock(&mutex);
    if (window) {
        ANativeWindow_release(window);
        window = 0;
    }
    window = ANativeWindow_fromSurface(env, surface);
    pthread_mutex_unlock(&mutex);
}