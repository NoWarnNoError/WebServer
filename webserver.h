#pragma once

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "ThreadPool/ThreadPool_Impl.h"
#include "ThreadPool/ThreadPool_fwd.h"
#include "myEpoll.h"

class WebServer {
   private:
    const char* const PORT;
    const int BUFFER_SIZE;
    const int EVENTS_SIZE;
    // 线程池
    const int THREADS_MAX;
    const int REQUESTS_MAX;

    int listen_fd;
    int epoll_fd;

    Epoll* my_epoll;
    epoll_event* event_arr;

    ThreadPool<int>* thread_pool;

    WebServer();

   public:
    WebServer(const char* __PORT,
              const int __BUFFER_SIZE,
              const int __EVENTS_SIZE,
              const int __THREADS_MAX,
              const int __REQUESTS_MAX);
    ~WebServer();

    void init_thread_pool();
    void eventListen();
    void eventLoop();
    int dealConnect(int socket_fd);
    void dealRead(int socket_fd);
    void dealWrite(int socket_fd);
    void et(int epoll_number);
};
