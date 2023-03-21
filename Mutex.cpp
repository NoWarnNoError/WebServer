#include "Mutex.h"

Mutex::Mutex() : mutex(new pthread_mutex_t()) {
    if (pthread_mutex_init(mutex, nullptr) != 0) {
        throw std::exception();
    }
}

Mutex::~Mutex() {
    if (pthread_mutex_destroy(mutex) != 0) {
        throw std::exception();
    }
}

pthread_mutex_t* Mutex::get() {
    return mutex;
}

int Mutex::lock() {
    return pthread_mutex_lock(mutex);
}

int Mutex::unlock() {
    return pthread_mutex_unlock(mutex);
}
