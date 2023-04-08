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
bool ThreadPool<T>::request_append(T& request, int state) {
    request.set_state(state);
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

        T& request = request_queue.front();
        request_queue.pop();
        mutex_pool->unlock();
        // 线程处理request
        if (request.get_state() == 0) {  // read
            // for (;;) {
            //     int r = 0;
            //     if ((r = recv(socket_fd, buffer, BUFFER_SIZE, 0)) < 0) {
            //         if (errno == EAGAIN || errno == EWOULDBLOCK) {
            //             // 对于非阻塞IO，该条件表示数据已读取完毕
            //             break;
            //         }
            //         close(socket_fd);
            //         break;
            //     } else if (r == 0) {
            //         close(socket_fd);
            //     }
            // }
        } else {  // write
        }

        // cout << pthread_self() << "处理任务" << request << endl;
        // sleep(10);
        // cout << pthread_self() << "完成任务" << request << endl;
    }
}