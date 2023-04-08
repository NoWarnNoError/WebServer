#include "http.h"

HTTP::HTTP() : state(0), READ_BUFFER_SIZE(Config::READ_BUFFER_SIZE) {}

HTTP::~HTTP() {}

int HTTP::user_count = 0;

void HTTP::init(const int __socket_fd,
                const sockaddr_storage& __ar,
                const socklen_t __ar_len) {
    socket_fd = __socket_fd;
    ar = __ar;
    ar_len = __ar_len;
}

const int HTTP::get_state() {
    return state;
}

void HTTP::set_state(const int __state) {
    state = __state;
}

int HTTP::recv_message() {
    // for (;;) {
    //     int r = 0;
    //     if ((r = recv(socket_fd, buffer, READ_BUFFER_SIZE, 0)) < 0) {
    //         if (errno == EAGAIN || errno == EWOULDBLOCK) {
    //             // 对于非阻塞IO，该条件表示数据已读取完毕
    //             break;
    //         }
    //         close(socket_fd);
    //         break;
    //     } else if (r == 0) {
    //         close(socket_fd);
    //     }
    // }
}