#include <string.h>

#include "myEpoll.h"

void set_fd_nonblock(int fd);

Epoll::Epoll() {}

Epoll::~Epoll() {}

void Epoll::addfd(int epoll_fd, int fd, bool enable_et) {
    epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;
    event.data.fd = fd;
    if (enable_et) {
        event.events |= EPOLLET;
    }

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    set_fd_nonblock(fd);  // 为什么设置为nonblock
}

void set_fd_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
}
