#include <memory>

#include "config.h"
#include "webserver.h"

using namespace std;

int main() {
    unique_ptr<Config> config(new Config);
    unique_ptr<WebServer> server(new WebServer(config->getPORT()));

    server->eventListen();

    return 0;
}