#include "http.h"

#include <string.h>

using namespace std;

HTTP::HTTP()
    : READ_BUFFER_SIZE(Config::READ_BUFFER_SIZE),
      state(0),
      buffer_read(new char[READ_BUFFER_SIZE]),
      idx_read(0) {}

HTTP::HTTP(const HTTP& __HTTP)
    : READ_BUFFER_SIZE(__HTTP.READ_BUFFER_SIZE),
      socket_fd(__HTTP.socket_fd),
      ar(__HTTP.ar),
      ar_len(__HTTP.ar_len),
      state(__HTTP.state),
      buffer_read(new char[READ_BUFFER_SIZE]),
      idx_read(__HTTP.idx_read) {
    ++user_count;
    cout << "拷贝构造函数" << user_count << endl;
    strncpy(buffer_read, __HTTP.buffer_read, READ_BUFFER_SIZE);
}

HTTP::~HTTP() {
    --user_count;
    cout << "析构函数" << user_count << endl;
    delete[] buffer_read;
}

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
    if (idx_read >= READ_BUFFER_SIZE) {
        return -1;
    }
    for (;;) {
        int r = 0;
        if ((r = recv(socket_fd, buffer_read + idx_read,
                      READ_BUFFER_SIZE - idx_read, 0)) < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 对于非阻塞IO，该条件表示数据已读取完毕
                break;
            }
            return -1;
        } else if (r == 0) {
            return -1;
        } else {
            idx_read += r;
        }
    }
    cout << "线程" << pthread_self() << ":" << buffer_read << endl;

    return idx_read;
}

int HTTP::process() {}