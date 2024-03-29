#include "Http.h"

int Http::epoll_fd = -1;

Http::Http()
    : READ_BUFFER_SIZE(Config::READ_BUFFER_SIZE),
      WRITE_BUFFER_SIZE(Config::WRITE_BUFFER_SIZE),
      my_epoll(new Epoll()),
      http_type(HTTP_REQUEST),
      buffer_read(new char[READ_BUFFER_SIZE]),
      idx_read(0),
      buffer_write(new char[WRITE_BUFFER_SIZE]),
      idx_write(0),
      parser((http_parser*)malloc(sizeof(http_parser))) {
    http_parser_settings_init(&settings);
}

Http::~Http() {
    delete[] buffer_read;
    delete[] buffer_write;
    free(parser);
}

Http::Http(const Http& _Http)
    : READ_BUFFER_SIZE(_Http.READ_BUFFER_SIZE),
      WRITE_BUFFER_SIZE(_Http.WRITE_BUFFER_SIZE),
      my_epoll(_Http.my_epoll),
      socket_fd(_Http.socket_fd),
      ar(_Http.ar),
      ar_len(_Http.ar_len),
      http_type(_Http.http_type),
      buffer_read(new char[READ_BUFFER_SIZE]),
      idx_read(_Http.idx_read),
      buffer_write(new char[WRITE_BUFFER_SIZE]),
      idx_write(_Http.idx_write),
      settings(_Http.settings),
      parser((http_parser*)malloc(sizeof(http_parser))) {
    // cout << "拷贝构造函数" << user_count << endl;
    strncpy(buffer_read, _Http.buffer_read, READ_BUFFER_SIZE);
    strncpy(buffer_write, _Http.buffer_write, WRITE_BUFFER_SIZE);
    mempcpy((void*)(parser), (void*)(_Http.parser), sizeof(http_parser));
}

void Http::init(const int _socket_fd,
                const sockaddr_storage& _ar,
                const socklen_t _ar_len) {
    socket_fd = _socket_fd;
    ar = _ar;
    ar_len = _ar_len;

    // 由于 on_xx 均为函数指针，无法通过 function 转化
    // 所以全部回调函数均为友元非成员函数
    // settings = {.on_message_begin = on_message_begin,
    //             .on_url = on_url,
    //             .on_status = 0,
    //             .on_header_field = on_header_field,
    //             .on_header_value = on_header_value,
    //             .on_headers_complete = on_headers_complete,
    //             .on_body = on_body,
    //             .on_message_complete = on_message_complete};

    init();
}

void Http::init() {
    idx_read = 0;
    idx_write = 0;

    memset(buffer_read, 0, READ_BUFFER_SIZE);
    memset(buffer_write, 0, WRITE_BUFFER_SIZE);
    memset(parser, 0, sizeof(http_parser));
}

const http_parser_type Http::get_http_type() {
    return http_type;
}

void Http::set_http_type(const http_parser_type _http_type) {
    http_type = _http_type;
}

int Http::recv_message() {
    if (idx_read >= READ_BUFFER_SIZE) {
        return -1;
    }
    for (;;) {
        int r = 0;
        if ((r = recv(socket_fd, buffer_read + idx_read,
                      READ_BUFFER_SIZE - idx_read, 0)) < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 对于非阻塞IO，该条件表示数据已读取完毕
                my_epoll->reset_oneshut(epoll_fd, socket_fd);

                break;
            }
            return -2;
        } else if (r == 0) {
            close(socket_fd);
            std::cout << "关闭socket " << socket_fd << std::endl;
            return -3;
        } else {
            idx_read += r;
        }
    }
    // std::cout << "线程" << pthread_self() << ":" << buffer_read << std::endl;

    return idx_read;
}

int Http::process_read() {
    http_parser_init(parser, http_type);
    size_t parsed =
        http_parser_execute(parser, &settings, buffer_read, idx_read);
    if (parsed != idx_read) {  // 解析失败
        return -1;
    }

    generate_response();
    my_epoll->modfd(epoll_fd, socket_fd, EPOLLOUT, true,
                    true);  // 在此处将 epoll 事件改为 EPOLLOUT

    return 0;
}

int Http::generate_response() {
    time_t now = time(nullptr);
    if (parser->method == HTTP_HEAD) {
        snprintf(buffer_write, WRITE_BUFFER_SIZE,
                 "HTTP/1.0 200 OK\r\n"
                 "Server: DYK_WSL\r\n"
                 "Content-Type: text/plain; charset=UTF-8\r\n"
                 "Date: %s\r\n"
                 "\r\n"
                 "response from HTTP_HEAD\r\n",
                 ctime(&now));
    } else if (parser->method == HTTP_GET) {
        snprintf(buffer_write, WRITE_BUFFER_SIZE,
                 "HTTP/1.0 200 OK\r\n"
                 "Server: DYK_WSL\r\n"
                 "Content-Type: text/plain; charset=UTF-8\r\n"
                 "Date: %s\r\n"
                 "\r\n"
                 "response from HTTP_GET\r\n",
                 ctime(&now));
    } else if (parser->method == HTTP_POST) {
    } else {
        return -1;
    }
    idx_write += strlen(buffer_write);
    // for (int i = 0; i < 1000000; ++i) {
    //     i *= 1;
    // }

    return 0;
}

int Http::send_message() {
    if (idx_write == 0) {
        my_epoll->modfd(epoll_fd, socket_fd, EPOLLIN, true, true);
        init();

        return 0;
    }

    int idx_send = 0;
    while (idx_send < idx_read) {
        int r =
            send(socket_fd, buffer_write + idx_send, idx_write - idx_send, 0);
        if (r == -1) {
            if (errno == EWOULDBLOCK) {  // TCP 窗口过小，无法发送
                std::cerr << "TCP 窗口过小，无法发送" << std::endl;
                return -1;
            } else if (errno == EINTR) {  // 被信号中断，重试
                continue;
            } else {
                return -1;
            }
        }
        idx_send += r;
    }
    // send(socket_fd, buffer_write, idx_write, 0);
    close(socket_fd);

    return idx_send;
}

// int on_message_begin(http_parser* parser) {
//     printf("\n***MESSAGE BEGIN***\n\n");

//     return 0;
// }

// int on_url(http_parser* _, const char* at, size_t length) {
//     (void)_;
//     printf("Url: %.*s\n", (int)length, at);
//     return 0;
// }

// int on_header_field(http_parser* _, const char* at, size_t length) {
//     (void)_;
//     printf("Header field: %.*s\n", (int)length, at);
//     if (!memcmp("Content-Type", at, length)) {
//         // printf("Found Content-Type\n");
//         content_type_flag = 1;
//     }
//     if (!memcmp("Content-Length", at, length)) {
//         // printf("Found Content-Length\n");
//         content_length_flag = 1;
//     }

//     return 0;
// }

// int on_header_value(http_parser* _, const char* at, size_t length) {
//     (void)_;
//     printf("Header value: %.*s\n", (int)length, at);
//     if (content_type_flag) {
//         memcpy(http_img.type, at, length);
//         // printf("http_img.type = %s\n", http_img.type);
//         content_type_flag = 0;
//     }

//     if (content_length_flag) {
//         char value[32];
//         memcpy(value, at, length);
//         // printf("http_img.content_size = %s\n", value);
//         http_img.content_size = atoi(value);
//         content_length_flag = 0;
//     }
//     return 0;
// }

// int on_headers_complete(http_parser* _) {
//     (void)_;
//     printf("\n***HEADERS COMPLETE***\n\n");
//     return 0;
// }

// int on_body(http_parser* _, const char* at, size_t length) {
//     (void)_;
//     unsigned int z;
//     char* p = at;

//     http_img.content_start = at;
//     //	if(!memcmp("image/png", http_img.type, strlen("image/png")))
//     //	{
//     //		printf("Found PNG body!http_img.content_start=%p\n",
//     // http_img.content_start);
//     //	}
//     return 0;
// }

// int on_message_complete(http_parser* _) {
//     (void)_;
//     printf("\n***MESSAGE COMPLETE***\n\n");
//     return 0;
// }
