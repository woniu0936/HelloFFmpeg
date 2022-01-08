//
// Created by songhuan on 2022/1/7.
//

#include "JavaCallHelper.h"
#include "macro.h"

JavaCallHelper::JavaCallHelper(JavaVM *vm, JNIEnv *env, jobject instance) {
    this->vm = vm;
    this->env = env;
    //一旦涉及到jobject就需要夸方法 跨线程 就需要全局引用
    this->instance = env->NewGlobalRef(instance);
    jclass cls = env->GetObjectClass(instance);
    this->onErrorId = env->GetMethodID(cls, "onError", "(I)V");
    this->onPrepareId = env->GetMethodID(cls, "onPrepare", "()V");
    LOGE("JavaCallHelper类构造方法完成");
}

JavaCallHelper::~JavaCallHelper() {
    env->DeleteGlobalRef(instance);
}

void JavaCallHelper::onError(int thread, int errorCode) {
    if (thread == THREAD_MAIN) {
        //主线程
        //回调onError方法
        env->CallVoidMethod(instance, onErrorId, errorCode);
    } else {
        //子线程
        JNIEnv *_env;
        //获得属于当前线程的JNIEnv
        vm->AttachCurrentThread(&_env, 0);
        _env->CallVoidMethod(instance, onErrorId, errorCode);
        vm->DetachCurrentThread();
    }
}

void JavaCallHelper::onPrepared(int thread) {
    LOGE("JavaCallHelper::onPrepared");
    if (thread == THREAD_MAIN) {
        //主线程
        //回调onError方法
        env->CallVoidMethod(instance, onPrepareId);
    } else {
        //子线程
        JNIEnv *_env;
        //获得属于当前线程的JNIEnv
        vm->AttachCurrentThread(&_env, 0);
        _env->CallVoidMethod(instance, onPrepareId);
        vm->DetachCurrentThread();
    }
}