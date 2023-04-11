#pragma once

#include <errno.h>
#include <sys/socket.h>
#include <iostream>

#include "config.h"

class HTTP {
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

    int socket_fd;
    sockaddr_storage ar;
    socklen_t ar_len;
    int state;

    char* buffer_read;
    int idx_read;

   public:
    static int user_count;

    HTTP();
    ~HTTP();

    HTTP(const HTTP& __HTTP);

    void init(const int __socket_fd,
              const sockaddr_storage& __ar,
              const socklen_t __ar_len);
    const int get_state();
    void set_state(const int __state);
    int recv_message();
    int process();
};
