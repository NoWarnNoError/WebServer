#include "config.h"

const char* Config::PORT = "10000";
const int Config::MAX_FD = 65536;
const int Config::EVENTS_SIZE = 1024;
// grep 'core id' /proc/cpuinfo | sort -u | wc -l
const int Config::THREADS_MAX = 100;
const int Config::REQUESTS_MAX = 10000;
const int Config::READ_BUFFER_SIZE = 2048;
const int Config::WRITE_BUFFER_SIZE = 1024;
