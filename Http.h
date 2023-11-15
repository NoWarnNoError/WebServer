#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <functional>
#include <iostream>

#include "lib/http-parser/http_parser.h"

#include "config.h"
#include "myEpoll.h"

class Http {
   private:
    enum METHOD {
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        TRACE,
        OPTIONS,
        CONNECT,
        PATCH
    };

    const int READ_BUFFER_SIZE;
    const int WRITE_BUFFER_SIZE;

    static int epoll_fd;
    Epoll* my_epoll;

    int socket_fd;
    sockaddr_storage ar;
    socklen_t ar_len;
    http_parser_type http_type;

    char* buffer_read;
    int idx_read;
    char* buffer_write;
    int idx_write;

    http_parser_settings settings;
    http_parser* parser;

   public:
    Http();
    ~Http();
    Http(const Http& _Http);

    void init(const int _socket_fd,
              const sockaddr_storage& _ar,
              const socklen_t _ar_len);
    static void inline set_epoll_fd(int _epoll_fd) { epoll_fd = _epoll_fd; }

    const http_parser_type get_http_type();
    void set_http_type(const http_parser_type _http_type);

    int Http::recv_message();
    int process_read();
    int generate_response();
};
