#include "LKZ/Core/Threading/ThreadTaskPool.h"
#include <chrono>

ThreadTaskPool::ThreadTaskPool(LoopHook hook, bool loopMode)
    : loopHook(hook), stop(false), loopMode(loopMode)
{
}

ThreadTaskPool::~ThreadTaskPool()
{
    Stop();
}

void ThreadTaskPool::WorkerLoop()
{
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!stop)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        lastTime = currentTime;
        float deltaTime = elapsed.count();

        if (loopMode && loopHook)
        {
            //loopHook(deltaTime); // Passe deltaTime automatiquement
        }

        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]() { return stop || !tasks.empty(); });

            if (stop && tasks.empty()) return;

            if (!tasks.empty())
            {
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
