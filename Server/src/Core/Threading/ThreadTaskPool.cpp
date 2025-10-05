#include "LKZ/Core/Threading/ThreadTaskPool.h"

ThreadTaskPool::ThreadTaskPool(LoopHook hook) : loopHook(hook), stop(false)
{
}

ThreadTaskPool::~ThreadTaskPool()
{
    std::unique_lock<std::mutex> lock(queueMutex);
    stop = true;

    condition.notify_all();
}

void ThreadTaskPool::WorkerLoop() 
{
    while (!stop) 
    {
        if (loopHook) loopHook();

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

void ThreadTaskPool::EnqueueTask(std::function<void()> task) 
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.push(std::move(task));
    }
    condition.notify_one();
}
void ThreadTaskPool::Stop()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
}