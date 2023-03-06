#pragma once

#include <fcntl.h>
#include <sys/epoll.h>

class Epoll {
   private:
   public:
    Epoll();

    ~Epoll();

    void addfd(int epoll_fd, int fd, bool enable_et);
};
