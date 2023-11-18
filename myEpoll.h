#pragma once

#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>

class Epoll {
   private:
   public:
    Epoll() = default;

    ~Epoll() = default;

    void addfd(int epoll_fd, int fd, bool oneshut, bool enable_et);
    void modfd(int epoll_fd,
                      int fd,
                      EPOLL_EVENTS ev,
                      bool oneshut,
                      bool enable_e);
    // 从内核时间表删除描述符
    void removefd(int epoll_fd, int fd);
    void reset_oneshut(int epoll_fd, int fd);
};
