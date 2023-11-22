#pragma once

#include "ThreadPool_fwd.h"
// 模板类实现无法放在.cpp中，会产生链接错误

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

        if (pthread_detach(*pd) != 0) {
            delete pd;
            throw std::exception();
        }
    }
}

template <typename T>
ThreadPool<T>::~ThreadPool() {
    cond->broadcast();
    for (int i = 0; i < THREADS_MAX; ++i) {
        pthread_join(*pthread_pool[i], nullptr);
    }
    cond->~Cond();
    mutex_pool->~Mutex();
}

template <typename T>
bool ThreadPool<T>::request_append(T* request, http_parser_type http_type) {
    request->set_http_type(http_type);
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
    pool->work();
    return pool;
}

template <typename T>
void ThreadPool<T>::work() {
    while (true) {
        mutex_pool->lock();
        while (request_queue.empty() && !shutdown) {
            cond->wait(mutex_pool->get());
        }

        if (shutdown) {
            cout << pthread_self() << "线程退出" << endl;
            return;
        }

        T* request = request_queue.front();
        request_queue.pop();
        // cout << " request.buffer_write: " << request->buffer_write << endl;
        mutex_pool->unlock();
        // 线程处理request
        // cout << pthread_self() << " 开始读取socket" << endl;

        if (request->get_http_type() == HTTP_REQUEST) {  // read
            // cout << pthread_self() << " 开始读取socket" << endl;

            int r = request->recv_message();
            if (r >= 0) {
                request->process_read();
            } else {
                cerr << pthread_self() << " recv() 错误 " << r << endl;
            }

            // cout << pthread_self() << " 完成读取socket" << endl;
        } else {  // write
            // cout << pthread_self() << " 开始写入socket" << endl;

            int r = request->send_message();
            if (r < 0) {
                cout << "send() " << r << endl;
            }

            // cout << pthread_self() << " 完成写入socket" << endl;
            // return;
        }

        // cout << pthread_self() << " 完成任务" << endl;
    }
}