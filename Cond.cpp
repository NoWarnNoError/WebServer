#include "Cond.h"

Cond::Cond() : cond(new pthread_cond_t()) {
    if (pthread_cond_init(cond, nullptr) != 0) {
        throw std::exception();
    }
}

Cond::~Cond() {
    if (pthread_cond_destroy(cond) != 0) {
        // throw std::exception();
    }
}

pthread_cond_t* Cond::get() {
    return cond;
}

int Cond::wait(pthread_mutex_t* mutex) {
    return pthread_cond_wait(cond, mutex);
}

int Cond::signal() {
    return pthread_cond_signal(cond);
}

int Cond::broadcast() {
    return pthread_cond_broadcast(cond);
}