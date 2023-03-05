#include <memory>

#include "config.h"
#include "myEpoll.h"
#include "webserver.h"

using namespace std;

int main() {
    unique_ptr<Config> config(new Config);
    unique_ptr<WebServer> server(new WebServer(
        config->getPORT(), config->getBUFFER_SIZE(), config->getEVENTS_SIZE()));

    server->eventListen();
    server->eventLoop();

    return 0;
}