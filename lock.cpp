#include "lock.h"

Mutex::Mutex() {
    if (pthread_mutex_init(&mutex, nullptr) != 0) {
        throw std::exception();
    }
}

Mutex::~Mutex() {
    if (pthread_mutex_destroy(&mutex) != 0) {
        throw std::exception();
    }
}

int Mutex::lock() {
    return pthread_mutex_lock(&mutex);
}

int Mutex::unlock() {
    return pthread_mutex_unlock(&mutex);
}
