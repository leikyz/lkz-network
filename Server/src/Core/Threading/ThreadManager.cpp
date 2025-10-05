#include "LKZ/Core/Threading/ThreadManager.h"
#include <iostream>
#include <thread>

std::unordered_map<std::string, ThreadManager::PoolPtr> ThreadManager::pools;

void ThreadManager::CreatePool(const std::string& name, int threads, ThreadTaskPool::LoopHook hook) 
{
    auto pool = std::make_shared<ThreadTaskPool>(hook);

	std::cout << "[ThreadManager] Initialize pool '" << name << "' with " << threads << " threads.\n";

    for (int i = 0; i < threads; i++) {
        std::thread([pool]() {
            pool->WorkerLoop();
            }).detach();
    }

    pools[name] = pool;
}

ThreadManager::PoolPtr ThreadManager::GetPool(const std::string& name) 
{
    auto it = pools.find(name);
    if (it != pools.end()) return it->second;

    std::cerr << "[ThreadManager] Pool '" << name << "' not found!\n";
    return nullptr;
}

void ThreadManager::StopAll()
{
    for (auto& pair : pools) {
        pair.second->Stop();
    }
}
