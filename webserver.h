#pragma once

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include "lib/http-parser/http_parser.h"

#include "Http.h"
#include "ThreadPool/ThreadPool_Impl.h"
#include "ThreadPool/ThreadPool_fwd.h"
#include "myEpoll.h"

class WebServer {
   private:
    const char* const PORT;
    const int MAX_FD;
    // epoll
    const int EVENTS_SIZE;
    // 线程池
    const int THREADS_MAX;
    const int REQUESTS_MAX;

    Http* user;
    static int user_count;

    int listen_fd;
    int epoll_fd;

    Epoll* my_epoll;
    epoll_event* event_arr;

    ThreadPool<Http>* thread_pool;

    WebServer();

   public:
    WebServer(const char* const _PORT,
              const int _BUFFER_SIZE,
              const int _EVENTS_SIZE,
              const int _THREADS_MAX,
              const int _REQUESTS_MAX);
    ~WebServer();

    void init_thread_pool();
    void eventListen();
    void eventLoop();
    int dealConnect(int socket_fd);
    void dealRead(int socket_fd);
    void dealWrite(int socket_fd);
    void et(int epoll_number);
};
