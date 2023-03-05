#include "config.h"

Config::Config() : PORT("10000"), BUFFER_SIZE(100), EVENTS_SIZE(1024) {}

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