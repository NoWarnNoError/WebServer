#pragma once

#include <pthread.h>
#include <unistd.h>
#include <queue>
#include <vector>

#include "../lib/http-parser/http_parser.h"

#include "../ConcurrentQueue.h"
#include "../Cond.h"
#include "../Mutex.h"

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
    ConcurrentQueue<T*> request_queue;

    ThreadPool();

   public:
    ThreadPool(const int __THREADS_MAX, const int __REQUESTS_MAX);
    ThreadPool(const ThreadPool&) = delete;
    ~ThreadPool();

    bool request_append(T* request, http_parser_type http_type);
    // pthread_create()第三个参数要求工作函数必须是static成员函数
    // 静态成员函数只能访问静态成员变量
    static void* worker(void* arg);
    void work();
};
