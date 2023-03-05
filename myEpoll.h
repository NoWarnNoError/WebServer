#pragma once

#include <fcntl.h>
#include <sys/epoll.h>

class Epoll {
   private:
    const int EVENTS_SIZE;

    int epoll_fd;

   public:
    Epoll();
    Epoll(const int __EVENTS_SIZE);

    ~Epoll();

    void addfd(int epoll_fd, int fd, bool enable_et);
};
