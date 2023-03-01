#include <string.h>
#include <unistd.h>
#include <iostream>
#include <memory>

#include "webserver.h"

using namespace std;

void* get_in_addr(struct sockaddr* sa);

WebServer::WebServer() {}

WebServer::WebServer(const char* _PORT) : PORT(_PORT) {}

WebServer::~WebServer() {}

void WebServer::eventListen() {
    addrinfo* hints = new addrinfo();
    memset(hints, 0, sizeof(hints));
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

    int listen_fd = 0;
    auto p_ar = server_ar;
    for (; p_ar != nullptr; p_ar = p_ar->ai_next) {
        if ((listen_fd = socket(p_ar->ai_family, p_ar->ai_socktype,
                                p_ar->ai_protocol)) < 0) {
            close(listen_fd);
        } else {
            int flag = 1;
            setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &flag,
                       sizeof(flag));

            if ((r = bind(listen_fd, server_ar->ai_addr,
                          server_ar->ai_addrlen)) < 0) {
                perror("bind");
                exit(-1);
            }
            break;
        }
    }
    if (p_ar == nullptr) {
        cerr << "fail to connect server" << endl;
        exit(-1);
    }

    char ip[INET6_ADDRSTRLEN];
    inet_ntop(p_ar->ai_family, get_in_addr(p_ar->ai_addr), ip,
              INET6_ADDRSTRLEN);
    cout << ip << endl;

    if ((r = listen(listen_fd, 5)) < 0) {
        perror("listen");
        exit(-1);
    }

    freeaddrinfo(server_ar);
}

void* get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}