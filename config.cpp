#include "config.h"

Config::Config()
    : PORT("10000"),
      BUFFER_SIZE(100),
      EVENTS_SIZE(1024),
      THREADS_MAX(8),
      REQUESTS_MAX(10000) {}

Config::~Config() {}

const char* Config::getPORT() const {
    return PORT;
}

const int Config::getBUFFER_SIZE() const {
    return BUFFER_SIZE;
}

const int Config::getEVENTS_SIZE() const {
    return EVENTS_SIZE;
}

const int Config::getTHREADS_MAX() const {
    return THREADS_MAX;
}

const int Config::getREQUESTS_MAX() const {
    return REQUESTS_MAX;
}