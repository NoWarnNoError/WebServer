#pragma once

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

class WebServer {
   private:
    const char* PORT;

   public:
    WebServer();
    WebServer(const char* _PORT);
    ~WebServer();

    void eventListen();
};
