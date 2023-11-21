#include <string.h>
#include <unistd.h>
#include <iostream>
#include <memory>

#include "webserver.h"

using namespace std;

void* get_in_addr(struct sockaddr* sa);

WebServer::WebServer()
    : PORT(0), MAX_FD(0), EVENTS_SIZE(0), THREADS_MAX(0), REQUESTS_MAX(0) {}

WebServer::WebServer(const char* const _PORT,
                     const int _MAX_FD,
                     const int _EVENTS_SIZE,
                     const int _THREADS_MAX,
                     const int _REQUESTS_MAX)
    : PORT(_PORT),
      MAX_FD(_MAX_FD),
      EVENTS_SIZE(_EVENTS_SIZE),
      THREADS_MAX(_THREADS_MAX),
      REQUESTS_MAX(_REQUESTS_MAX),
      user(new Http[MAX_FD]),
      my_epoll(new Epoll()),
      event_arr(new epoll_event[EVENTS_SIZE]) {}

WebServer::~WebServer() {
    close(listen_fd);
    close(epoll_fd);

    delete[] user;
    delete my_epoll;
    delete[] event_arr;
}

void WebServer::init_thread_pool() {
    thread_pool = new ThreadPool<Http>(THREADS_MAX, REQUESTS_MAX);
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
        setsockopt(listen_fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

        if ((r = bind(listen_fd, p_ar->ai_addr, p_ar->ai_addrlen)) < 0) {
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
    Http::set_epoll_fd(epoll_fd);
    if (epoll_fd < 0) {
        perror("epoll_create");
        exit(-1);
    }
    my_epoll->addfd(epoll_fd, listen_fd, false,
                    false);  // listen_fd 使用lt模式为,
                             // 防止et模式高并发下只触发一次导致连接丢失
                             // 此时阻塞或非阻塞均可

    for (;;) {
        int epoll_number = epoll_wait(epoll_fd, event_arr, EVENTS_SIZE, -1);
        if (epoll_number < 0) {
            perror("epoll_wait");
            break;
        }
        et(epoll_number);
    }
}

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

int WebServer::dealConnect(int socket_fd) {
    sockaddr_storage ar;
    socklen_t ar_len = sizeof(ar);
    int connfd = 0;
    if ((connfd = accept(socket_fd, (sockaddr*)&ar, &ar_len)) < 0) {
        perror("accept");
        return -1;
    }
    // if (connfd >= MAX_FD || ++Http::user_count >= MAX_FD) {
    if (connfd >= MAX_FD) {
        cerr << "Server busy" << endl;
        return -1;
    }
    int flag = 1;
    // setsockopt(connfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
    user[connfd].init(connfd, ar, ar_len);

    my_epoll->addfd(epoll_fd, connfd, true, true);
    // et 模式下需要将 connfd 设置为非阻塞
    // 因为 et 模式只返回一次，需要循环读取，使用阻塞 socket 将在 recv
    // 处发生阻塞

    return 0;
}

void WebServer::dealRead(int socket_fd) {
    thread_pool->request_append(user + socket_fd, HTTP_REQUEST);
}

void WebServer::dealWrite(int socket_fd) {
    thread_pool->request_append(user + socket_fd, HTTP_RESPONSE);
}

void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}