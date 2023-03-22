#include <memory>

#include "ThreadPool/ThreadPool_Impl.h"
#include "ThreadPool/ThreadPool_fwd.h"
#include "config.h"
#include "myEpoll.h"
#include "webserver.h"

using namespace std;

void test();

int main() {
    // unique_ptr<Config> config(new Config);
    // unique_ptr<WebServer> server(new WebServer(
    //     config->getPORT(), config->getBUFFER_SIZE(),
    //     config->getEVENTS_SIZE()));

    // server->eventListen();
    // server->eventLoop();

    test();

    return 0;
}

void test() {
    ThreadPool<int>* threadpool = new ThreadPool<int>(8, 100);
    for (int i = 0; i < 100; ++i) {
        threadpool->request_append(i);
    }
    threadpool->~ThreadPool();
}