#pragma once

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "myEpoll.h"

class WebServer {
   private:
    const char* const PORT;
    const int BUFFER_SIZE;
    const int EVENTS_SIZE;

    int listen_fd;
    int epoll_fd;

    Epoll* my_epoll;
    epoll_event* event_arr;

   public:
    WebServer();
    WebServer(const char* __PORT,
              const int __BUFFER_SIZE,
              const int __EVENTS_SIZE);
    ~WebServer();

    void eventListen();
    void eventLoop();
    int dealConnect(int socket_fd);
    void dealRead(int socket_fd);
    void dealWrite(int socket_fd);
    void et(int epoll_number);
};
