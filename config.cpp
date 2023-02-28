#include "config.h"

Config::Config() : PORT("10000") {}

Config::~Config() {}

const char* Config::getPORT() const {
    return PORT;
}