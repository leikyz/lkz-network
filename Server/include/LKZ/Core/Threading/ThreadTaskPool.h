#ifndef THREAD_TASK_POOL_H
#define THREAD_TASK_POOL_H

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ThreadTaskPool {
public:
    explicit ThreadTaskPool(size_t threadCount);
    ~ThreadTaskPool();

    void EnqueueTask(std::function<void()> task);
    void Stop();

private:
    void WorkerLoop();

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

#endif
