#include <memory>

#include "ThreadPool/ThreadPool_Impl.h"
#include "ThreadPool/ThreadPool_fwd.h"
#include "config.h"
#include "myEpoll.h"
#include "webserver.h"

using namespace std;

void test();

int main() {
    unique_ptr<Config> config(new Config);
    unique_ptr<WebServer> server(new WebServer(
        config->getPORT(), config->getBUFFER_SIZE(), config->getEVENTS_SIZE(),
        config->getTHREADS_MAX(), config->getREQUESTS_MAX()));

    server->init_thread_pool();
    server->eventListen();
    server->eventLoop();

    return 0;
}