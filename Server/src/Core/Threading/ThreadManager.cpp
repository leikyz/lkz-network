#include "LKZ/Core/Threading/ThreadManager.h"
#include <iostream>

std::unordered_map<std::string, std::unique_ptr<ThreadTaskPool>> ThreadManager::threadPools;

void ThreadManager::CreatePool(const std::string& name, size_t threadCount) {
    if (threadPools.find(name) != threadPools.end()) {
        std::cerr << "[ThreadManager] Pool '" << name << "' already exists!\n";
        return;
    }

    threadPools[name] = std::make_unique<ThreadTaskPool>(threadCount);

    std::cout << "\033[37m[INITIALIZE] ThreadTaskPool : " << name << " (" << threadCount << " threads)\033[0m\n";
}

void ThreadManager::EnqueueTask(const std::string& name, std::function<void()> task) {
    auto it = threadPools.find(name);
    if (it != threadPools.end())
        it->second->EnqueueTask(task);
    else
        std::cerr << "[ThreadManager] Pool '" << name << "' not found!\n";
}

void ThreadManager::StopAll() {
    for (auto& pair : threadPools) {
        pair.second->Stop();
    }
}
