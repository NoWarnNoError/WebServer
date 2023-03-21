#pragma once

#include <pthread.h>
#include <unistd.h>
#include <queue>
#include <vector>

#include "Cond.h"
#include "Mutex.h"

using namespace std;

template <typename T>
class ThreadPool {
   private:
    const int THREADS_MAX;
    const int REQUESTS_MAX;
    int shutdown;
    Mutex* mutex_pool;
    Cond* cond;
    vector<pthread_t*> pthread_pool;
    queue<T> request_queue;

    ThreadPool();

   public:
    ThreadPool(const int __THREADS_MAX, const int __REQUESTS_MAX);
    ~ThreadPool();

    bool request_append(T& request);
    static void* worker(void* arg);
    // void workerImpl(void);
    void run();
};

template <typename T>
ThreadPool<T>::ThreadPool() : THREADS_MAX(0), REQUESTS_MAX(0) {}

template <typename T>
ThreadPool<T>::ThreadPool(const int __THREADS_MAX, const int __REQUESTS_MAX)
    : THREADS_MAX(__THREADS_MAX),
      REQUESTS_MAX(__REQUESTS_MAX),
      shutdown(0),
      mutex_pool(new Mutex()),
      cond(new Cond()) {
    for (int i = 0; i < THREADS_MAX; ++i) {
        pthread_t* pd = new pthread_t();
        if (pthread_create(pd, nullptr, worker, this) != 0) {
            delete pd;
            throw std::exception();
        } else {
            pthread_pool.push_back(pd);
        }

        // if (pthread_detach(pd) != 0) {
        //     delete pd;
        //     throw std::exception();
        // }
    }
}

template <typename T>
ThreadPool<T>::~ThreadPool() {
    cond->broadcast();
    for (int i = 0; i < THREADS_MAX; ++i) {
        pthread_join(pthread_pool[i], nullptr);
    }
    cond->~Cond();
    mutex_pool->~Mutex();
}

template <typename T>
bool ThreadPool<T>::request_append(T& request) {
    mutex_pool->lock();
    if (request_queue.size() < REQUESTS_MAX) {
        request_queue.push(request);
        mutex_pool->unlock();
        cond->signal();  // 通知其他线程
    } else {
        mutex_pool->unlock();
        cerr << "request queue has reached max" << endl;
        return false;
    }
    return true;
}

template <typename T>
void* ThreadPool<T>::worker(void* arg) {
    ThreadPool<T>* pool = static_cast<ThreadPool<T>*>(arg);
    pool->run();
    return pool;
}

template <typename T>
void ThreadPool<T>::run() {
    while (true) {
        mutex_pool->lock();
        while (request_queue.empty() && !shutdown) {
            cond->wait(mutex_pool->get());
        }

        if (shutdown) {
            cout << pthread_self() << "线程退出" << endl;
            return;
        }

        T& request = request_queue.front();
        request_queue.pop();
        mutex_pool->unlock();
        // 线程处理request

        cout << pthread_self() << "处理任务" << request << endl;
        sleep(10);
        cout << pthread_self() << "完成任务" << request << endl;
    }
}