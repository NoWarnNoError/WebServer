#include <string.h>
#include <unistd.h>
#include <iostream>
#include <memory>

#include "myEpoll.h"
#include "webserver.h"

using namespace std;

void* get_in_addr(struct sockaddr* sa);

WebServer::WebServer() : PORT(0), BUFFER_SIZE(0), EVENTS_SIZE(0) {}

WebServer::WebServer(const char* const __PORT,
                     const int __BUFFER_SIZE,
                     const int __EVENTS_SIZE)
    : PORT(__PORT),
      BUFFER_SIZE(__BUFFER_SIZE),
      EVENTS_SIZE(__EVENTS_SIZE),
      my_epoll(new Epoll()),
      event_arr(new epoll_event[EVENTS_SIZE]) {}

WebServer::~WebServer() {
    close(listen_fd);
    close(epoll_fd);
}

void WebServer::eventListen() {
    addrinfo* hints = new addrinfo();
    // memset(hints, 0, sizeof(hints));
    hints->ai_family = AF_UNSPEC;
    hints->ai_flags = AI_PASSIVE;
    hints->ai_socktype = SOCK_STREAM;

    addrinfo* server_ar = new addrinfo();
    int r = 0;
    if ((r = getaddrinfo(nullptr, PORT, hints, &server_ar)) < 0) {
        cerr << "getaddrinfo error: " << gai_strerror(r) << endl;
        exit(-1);
    }

    freeaddrinfo(hints);

    auto p_ar = server_ar;
    for (; p_ar != nullptr; p_ar = p_ar->ai_next) {
        if ((listen_fd = socket(p_ar->ai_family, p_ar->ai_socktype,
                                p_ar->ai_protocol)) < 0) {
            close(listen_fd);
            continue;
        }

        int flag = 1;
        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

        if ((r = bind(listen_fd, server_ar->ai_addr, server_ar->ai_addrlen)) <
            0) {
            close(listen_fd);
            continue;
        }
        break;
    }
    if (p_ar == nullptr) {
        cerr << "fail to bind server" << endl;
        exit(-1);
    }

    char ip[INET6_ADDRSTRLEN];
    inet_ntop(p_ar->ai_family, get_in_addr(p_ar->ai_addr), ip,
              INET6_ADDRSTRLEN);
    cout << ip << endl;

    freeaddrinfo(server_ar);

    if ((r = listen(listen_fd, 5)) < 0) {
        perror("listen");
        exit(-1);
    }
}

void WebServer::eventLoop() {
    epoll_fd = epoll_create(5);
    if (epoll_fd < 0) {
        perror("epoll_create");
        exit(-1);
    }
    my_epoll->addfd(epoll_fd, listen_fd, true);

    for (;;) {
        int epoll_number = epoll_wait(epoll_fd, event_arr, EVENTS_SIZE, -1);
        if (epoll_number < 0) {
            perror("epoll_wait");
            break;
        }
        et(epoll_number);
    }
}

int WebServer::dealConnect(int socket_fd) {
    sockaddr_storage ar;
    socklen_t ar_len = sizeof(ar);
    int connfd = 0;
    if ((connfd = accept(socket_fd, (sockaddr*)&ar, &ar_len)) < 0) {
        perror("accept");
        return -1;
    }
    my_epoll->addfd(epoll_fd, connfd, true);

    return 0;
}

void WebServer::dealRead(int socket_fd) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    for (;;) {
        int r = 0;
        if ((r = recv(socket_fd, buffer, BUFFER_SIZE, 0)) < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 对于非阻塞IO，该条件表示数据已读取完毕
                break;
            }
            close(socket_fd);
            break;
        } else if (r == 0) {
            close(socket_fd);
        }
    }
}

void WebServer::dealWrite(int socket_fd) {}

void WebServer::et(int epoll_number) {
    for (int i = 0; i < epoll_number; ++i) {
        int socket_fd = event_arr[i].data.fd;
        if (socket_fd == listen_fd) {
            if (dealConnect(socket_fd) < 0) {
                continue;
            }
        } else if (event_arr[i].events & EPOLLIN) {
            dealRead(socket_fd);
        } else if (event_arr[i].events & EPOLLOUT) {
            dealWrite(socket_fd);
        }
    }
}

void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}