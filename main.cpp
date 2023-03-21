#include <memory>

#include "config.h"
#include "myEpoll.h"
#include "threadpool.h"
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

    void test();

    return 0;
}

void test() {
    ThreadPool<int>* threadpool = new ThreadPool<int>(8, 100);
    // for (int i = 0; i < 100; ++i) {
    //     threadpool->request_append(i);
    // }
}