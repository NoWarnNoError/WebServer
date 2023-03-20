#pragma once

#include <pthread.h>
#include <iostream>

class Mutex {
   private:
    pthread_mutex_t* mutex;

   public:
    Mutex();
    ~Mutex();

    pthread_mutex_t* get();
    int lock();
    int unlock();
};
