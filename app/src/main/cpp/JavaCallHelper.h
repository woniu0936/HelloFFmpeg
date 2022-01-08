//
// Created by songhuan on 2022/1/7.
//

#ifndef HELLOFFMPEG_JAVACALLHELPER_H
#define HELLOFFMPEG_JAVACALLHELPER_H

#include <jni.h>


class JavaCallHelper {
public:
    JavaCallHelper(JavaVM* vm, JNIEnv *env, jobject instance);
    ~JavaCallHelper();

    //回调java
    void onError(int thread, int errorCode);
    void onPrepared(int thread);
private:
    //用于跨线程
    JavaVM * vm;
    JNIEnv *env;
    jobject instance;
    jmethodID onErrorId;
    jmethodID onPrepareId;
};


#endif //HELLOFFMPEG_JAVACALLHELPER_H
