#pragma once

class Config {
   public:
    static const char* PORT;
    static const int MAX_FD;
    static const int EVENTS_SIZE;
    static const int THREADS_MAX;
    static const int REQUESTS_MAX;
    static const int READ_BUFFER_SIZE;
    static const int WRITE_BUFFER_SIZE;

   private:
    Config() = default;
    ~Config() = default;
};
