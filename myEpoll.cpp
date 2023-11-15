#include <string.h>

#include "myEpoll.h"

void set_fd_nonblock(int fd);

void Epoll::addfd(int epoll_fd, int fd, bool oneshot, bool enable_et) {
    epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (oneshot) {
        event.events |= EPOLLONESHOT;
    }
    if (enable_et) {
        event.events |= EPOLLET;
        set_fd_nonblock(fd);  // 每个使用et模式的文件描述符都应该是非阻塞的
    }

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
}

void Epoll::modfd(int epoll_fd,
                  int fd,
                  EPOLL_EVENTS ev,
                  bool oneshut,
                  bool enable_et) {
    epoll_event event;
    event.data.fd = fd;

    if (oneshut && enable_et)
        event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    else
        event.events = ev | EPOLLRDHUP;

    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
}

// 从内核时间表删除描述符
void Epoll::removefd(int epoll_fd, int fd) {
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

void Epoll::reset_oneshut(int epoll_fd, int fd) {
    epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLONESHOT | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
}

void set_fd_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
}
