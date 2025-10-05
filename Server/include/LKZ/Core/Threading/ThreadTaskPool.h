#pragma once
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <memory>
#include <string>

class ThreadTaskPool {
public:
    using LoopHook = std::function<void()>;

    ThreadTaskPool(LoopHook hook = nullptr);
    ~ThreadTaskPool();

    void WorkerLoop();
    void EnqueueTask(std::function<void()> task);
    void Stop();

private:
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
    LoopHook loopHook;
};
