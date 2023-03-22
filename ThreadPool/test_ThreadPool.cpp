
#include "ThreadPool_Impl.h"
#include "ThreadPool_fwd.h"

using namespace std;

void test();

int main() {
    test();

    return 0;
}

void test() {
    ThreadPool<int>* threadpool = new ThreadPool<int>(8, 100);
    for (int i = 0; i < 100; ++i) {
        threadpool->request_append(i);
    }
    threadpool->~ThreadPool();
}