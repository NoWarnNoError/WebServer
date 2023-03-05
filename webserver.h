#pragma once

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "myEpoll.h"

class WebServer {
   private:
    const char* PORT;
    const int BUFFER_SIZE;
    const int EVENTS_SIZE;

    int listen_fd;
    int epoll_fd;

    Epoll* my_epoll = new Epoll(EVENTS_SIZE);

   public:
    WebServer(const char* __PORT,
              const int __BUFFER_SIZE,
              const int __EVENTS_SIZE);
    ~WebServer();

    void eventListen();
    void eventLoop();
    void et(int epoll_fd,
            int listen_fd,
            int epoll_number,
            epoll_event* events,
            int BUFFER_SIZE);
};
