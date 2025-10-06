#include "LKZ/Core/Threading/ThreadTaskPool.h"

ThreadTaskPool::ThreadTaskPool(LoopHook hook, bool loopMode)
    : loopHook(hook), stop(false), loopMode(loopMode) {
}

ThreadTaskPool::~ThreadTaskPool()
{
    Stop();
}

void ThreadTaskPool::WorkerLoop()
{
    if (loopMode && loopHook)
    {
        while (!stop)
        {
            loopHook(1.0f);
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }
        return;
    }

    while (!stop)
    {
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
