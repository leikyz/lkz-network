#include "LKZ/Core/Threading/ThreadTaskPool.h"
#include <chrono>
#include "LKZ/Core/ECS/Manager/NavMeshQueryManager.h"

ThreadTaskPool::ThreadTaskPool(LoopHook hook, bool loopMode)
    : loopHook(hook), loopMode(loopMode)
{
}

ThreadTaskPool::~ThreadTaskPool()
{
    Stop();
}

void ThreadTaskPool::Start()
{
    if (running) return;
    running = true;
    stopRequested = false;
    worker = std::thread(&ThreadTaskPool::WorkerLoop, this);
}

void ThreadTaskPool::Stop()
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopRequested = true;
    }
    condition.notify_all();
    if (worker.joinable())
        worker.join();
    running = false;
}

void ThreadTaskPool::EnqueueTask(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        tasks.push(std::move(task));
    }
    condition.notify_one();
}

void ThreadTaskPool::SetDeltaTime(float dt)
{
    deltaTime = dt;
}

void ThreadTaskPool::WorkerLoop()
{
    if (loopMode && loopHook)
    {
        while (!stopRequested)
        {
            loopHook(deltaTime);
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
        }

        NavMeshQueryManager::CleanupThreadQuery();
        return;
    }

    while (!stopRequested)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this]() { return stopRequested || !tasks.empty(); });
            if (stopRequested && tasks.empty())
                break; 

            task = std::move(tasks.front());
            tasks.pop();
        }

        if (task) task();
    }

    NavMeshQueryManager::CleanupThreadQuery();
}
