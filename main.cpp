#include <memory>

#include "ThreadPool/ThreadPool_Impl.h"
#include "ThreadPool/ThreadPool_fwd.h"
#include "config.h"
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

    return 0;
}