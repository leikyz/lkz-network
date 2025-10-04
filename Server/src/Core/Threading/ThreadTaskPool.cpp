#include "LKZ/Core/Threading/ThreadTaskPool.h"
#include <iostream>

ThreadTaskPool::ThreadTaskPool(size_t threadCount)
    : stop(false)
{
    for (size_t i = 0; i < threadCount; ++i) {
        workers.emplace_back([this]() { WorkerLoop(); });
    }
}

ThreadTaskPool::~ThreadTaskPool() {
    Stop();
}

void ThreadTaskPool::WorkerLoop()
{
    while (!stop) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]() { return stop || !tasks.empty(); });

            if (stop && tasks.empty()) return;

            if (!tasks.empty()) {
                task = std::move(tasks.front());
                tasks.pop();
            }
        }

        if (task)
            task(); 
    }
}

void ThreadTaskPool::EnqueueTask(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.push(std::move(task));
    }
    condition.notify_one();
}

void ThreadTaskPool::Stop() {
    stop = true;
    condition.notify_all();

    for (auto& worker : workers) {
        if (worker.joinable())
            worker.join();
    }
}
