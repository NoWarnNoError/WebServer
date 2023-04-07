#include "http.h"

HTTP::HTTP() : state(0) {}

HTTP::~HTTP() {}

int HTTP::get_state() {
    return state;
}

void HTTP::set_state(int __state) {
    state = __state;
}
