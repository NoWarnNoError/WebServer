#pragma once

#include <sys/socket.h>

#include "config.h"

class HTTP {
   private:
    const int READ_BUFFER_SIZE;

    int socket_fd;
    sockaddr_storage ar;
    socklen_t ar_len;
    int state;

    char* buffer;

   public:
    HTTP();
    ~HTTP();

    static int user_count;

    void init(const int __socket_fd,
              const sockaddr_storage& __ar,
              const socklen_t __ar_len);
    const int get_state();
    void set_state(const int __state);
    int recv_message();
};
