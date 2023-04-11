#pragma once

#include <errno.h>
#include <sys/socket.h>
#include <iostream>

#include "config.h"

class HTTP {
   public:
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
    enum HTTP_CODE {
        NO_REQUEST,
        GET_REQUEST,
        BAD_REQUEST,
        NO_RESOURCE,
        FORBIDDEN_REQUEST,
        FILE_REQUEST,
        INTERNAL_ERROR,
        CLOSED_CONNECTION
    };
    enum LINE_STATUS { LINE_OK, LINE_BAD, LINE_OPEN };  // 从状态机读取状态

    static int user_count;

   private:
    const int READ_BUFFER_SIZE;
    const int WRITE_BUFFER_SIZE;

    int socket_fd;
    sockaddr_storage ar;
    socklen_t ar_len;
    int state;

    char* buffer_read;
    int idx_read;
    char* buffer_write;
    int idx_write;
    int idx_check;

   public:
    HTTP();
    ~HTTP();

    HTTP(const HTTP& __HTTP);

    void init(const int __socket_fd,
              const sockaddr_storage& __ar,
              const socklen_t __ar_len);
    const int get_state();
    void set_state(const int __state);
    int recv_message();
    HTTP_CODE process_read();
    LINE_STATUS read_line();
};
