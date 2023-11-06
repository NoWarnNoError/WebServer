#include <memory>

#include "ThreadPool/ThreadPool_Impl.h"
#include "ThreadPool/ThreadPool_fwd.h"
#include "config.h"
#include "lib/http-parser/http_parser.h"
#include "myEpoll.h"
#include "webserver.h"

using namespace std;

void test();

int main() {
    unique_ptr<WebServer> server(
        new WebServer(Config::PORT, Config::MAX_FD, Config::EVENTS_SIZE,
                      Config::THREADS_MAX, Config::REQUESTS_MAX));

    server->init_thread_pool();
    server->eventListen();
    server->eventLoop();

    http_parser_settings settings;

    http_parser* parser = (http_parser*)malloc(sizeof(http_parser));
    http_parser_init(parser, HTTP_REQUEST);
    // parser->data = my_socket;

    return 0;
}