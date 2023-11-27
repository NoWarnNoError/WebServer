#include <pthread.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "../ConcurrentQueue.h"

#define INTERVAL int(1e7)

using namespace std;

mutex m;
queue<int> q_lock;
ConcurrentQueue<int> q;

void f1(int i);
void f2(int i);
void f3(int i);
void f4(int i);
void ff(void f1(int i), void f2(int i), auto& q);

int main() {
    ff(f1, f2, q);
    ff(f3, f4, q_lock);

    return 0;
}

void ff(void f1(int i), void f2(int i), auto& q) {
    auto pre1 = chrono::high_resolution_clock::now();

    thread t0(f1, 0*INTERVAL);
    thread t1(f1, 1*INTERVAL);
    thread t2(f1, 2*INTERVAL);
    thread t3(f1, 3*INTERVAL);
    thread t4(f1, 4*INTERVAL);

    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();

    thread t5(f2, INTERVAL);
    thread t6(f2, INTERVAL);
    thread t7(f2, INTERVAL);

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

    auto end1 = chrono::high_resolution_clock::now();
    std::chrono::duration<double, ratio<1, 1000>> diff1 = end1 - pre1;
    std::chrono::duration<double, milli> diff11 = end1 - pre1;
    cout << diff1.count() << "ms" << endl;
    cout << diff11.count() << "ms" << endl;
}

void f1(int i) {
    for (int j = i; j < i + INTERVAL; ++j) {
        q.push(j);
    }
}

void f2(int i) {
    for (int j = 0; j < i; ++j) {
        q.pop();
    }
}

void f3(int i) {
    for (int j = i; j < i + INTERVAL; ++j) {
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