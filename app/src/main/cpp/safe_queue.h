//
// Created by songhuan on 2022/1/5.
//

#ifndef HELLOFFMPEG_SAFE_QUEUE_H
#define HELLOFFMPEG_SAFE_QUEUE_H

#include <queue>
#include <pthread.h>

template<typename T>
class SafeQueue {

    // typedef 定义一个函数指针
    typedef void (*ReleaseCallback)( &

    T);

public:
    SafeQueue() {
        pthread_mutex_init(&mutex, 0);
        pthread_cond_init(&cond, 0);
    }

    ~SafeQueue() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }

    void push(T value) {
        pthread_mutex_lock(&mutex);
        q.push(value);
        //通知有了新数据到达
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }

    int pop( &

    T value
    ) {
        //表示取数据成功还是失败
        int ret = 0;
        pthread_mutex_lock(&mutex);
        while (q.empty()) {
            //没有数据就等待
            pthread_cond_wait(&cond, &mutex);
        }
        if (!q.empty()) {
            value = q.front();
            q.pop();
            ret = 1;
        }
        pthread_mutex_unlock(&mutex);
        return ret;
    }

    void clear() {
        pthread_mutex_lock(&mutex);
        uint32_t size = q.size();
        for (int i = 0; i < size; ++i) {
            //取出队首的数据
            T value = q.front();
            //释放value
            if (releaseCallback) {
                releaseCallback(value);
            }
            q.pop();
        }
        pthread_mutex_unlock(&mutex);
    }

    void setReleaseCallback(ReleaseCallback callback) {
        this->releaseCallback = callback;
    }

private:
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    queue <T> q;
    ReleaseCallback releaseCallback;
};

#endif //HELLOFFMPEG_SAFE_QUEUE_H
