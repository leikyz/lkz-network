#include "LKZ/Core/Threading/ThreadManager.h"
#include <iostream>

void ThreadManager::CreatePool(const std::string& name, int threads, ThreadTaskPool::LoopHook hook, bool isLoop)
{
    std::lock_guard<std::mutex> lock(managerMutex);

    if (pools.contains(name))
    {
        std::cerr << "[ThreadManager] Pool '" << name << "' already exists.\n";
        return;
    }

    auto pool = std::make_shared<ThreadTaskPool>(hook, isLoop);
    std::cout << "[ThreadManager] Created pool '" << name << "' with " << threads << " thread(s).\n";

    // Start only one thread since each pool = single worker
    pool->Start();

    pools[name] = pool;
}

ThreadManager::PoolPtr ThreadManager::GetPool(const std::string& name)
{
    std::lock_guard<std::mutex> lock(managerMutex);
    auto it = pools.find(name);
    if (it != pools.end())
        return it->second;

    std::cerr << "[ThreadManager] Pool '" << name << "' not found!\n";
    return nullptr;
}

void ThreadManager::StopAll()
{
    std::lock_guard<std::mutex> lock(managerMutex);
    for (auto& [name, pool] : pools)
    {
        pool->Stop();
    }
    pools.clear();
}

void ThreadManager::SetGlobalDeltaTime(float dt)
{
    std::lock_guard<std::mutex> lock(managerMutex);
    globalDeltaTime = dt;

    for (auto& [name, pool] : pools)
    {
        pool->SetDeltaTime(dt);
    }
}

float ThreadManager::GetGlobalDeltaTime()
{
    std::lock_guard<std::mutex> lock(managerMutex);
    return globalDeltaTime;
}
