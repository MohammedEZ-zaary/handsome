#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    ThreadPool(unsigned long);
    ~ThreadPool();
    void enqueue(std::function<void()>);

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

#endif // THREADPOOL_HPP
