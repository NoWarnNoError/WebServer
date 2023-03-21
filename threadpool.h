#include <pthread.h>
#include <unistd.h>
#include <queue>
#include <vector>

#include "Cond.h"
#include "Mutex.h"

using namespace std;

template <typename T>
class ThreadPool {
   private:
    const int THREADS_MAX;
    const int REQUESTS_MAX;
    int shutdown;
    Mutex* mutex_pool;
    Cond* cond;
    vector<pthread_t*> pthread_pool;
    queue<T&> request_queue;

    ThreadPool();

   public:
    ThreadPool(const int __THREADS_MAX, const int __REQUESTS_MAX);
    ~ThreadPool();

    bool request_append(T& request);
    void* worker(void* arg);
    void run();
};
