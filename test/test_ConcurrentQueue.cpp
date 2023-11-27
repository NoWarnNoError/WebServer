#include <pthread.h>
#include <unistd.h>
#include <algorithm>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "../ConcurrentQueue.h"

using namespace std;

mutex m;
queue<int> q_lock;
ConcurrentQueue<int> q;

void f1(int i);
void f2(int i);
void f3(int i);
void f4(int i);

int main() {
    thread t0(f1, 0);
    thread t1(f1, 10000);
    thread t2(f1, 20000);
    thread t3(f1, 30000);
    thread t4(f1, 40000);

    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    thread t5(f2, 10000);
    thread t6(f2, 10000);
    thread t7(f2, 10000);

    t5.join();
    t6.join();
    t7.join();

    vector<int> vt;
    while (!q.empty()) {
        vt.push_back(q.front());
        q.pop();
    }

    sort(vt.begin(), vt.end());
    cout << vt.front() << ' ' << vt.back() << ' ' << vt.size() << endl;

    return 0;
}

void f1(int i) {
    for (int j = i; j < i + 10000; ++j) {
        q.push(j);
    }
}

void f2(int i) {
    for (int j = 0; j < i; ++j) {
        q.pop();
    }
}

void f3(int i) {
    for (int j = i; j < i + 10000; ++j) {
        m.lock();
        q_lock.push(j);
        m.unlock();
    }
}

void f4(int i) {
    for (int j = 0; j < i; ++j) {
        m.lock();
        q_lock.pop();
        m.unlock();
    }
}