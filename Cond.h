#pragma once

#include <pthread.h>
#include <iostream>

class Cond {
   private:
    pthread_cond_t* cond;

   public:
    Cond();
    ~Cond();

    pthread_cond_t* get();
    int wait(pthread_mutex_t* mutex);
    int signal();
    int broadcast();
};
