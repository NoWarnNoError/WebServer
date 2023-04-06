rootDir=$(cd $(dirname $0); pwd)

g++ test_ThreadPool.cpp ../Mutex.cpp ../Cond.cpp -o test_ThreadPool "-lpthread"
./test_ThreadPool