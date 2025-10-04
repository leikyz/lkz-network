#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "ThreadTaskPool.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <functional>

class ThreadManager {
private:
    static std::unordered_map<std::string, std::unique_ptr<ThreadTaskPool>> threadPools;

public:
    static void CreatePool(const std::string& name, size_t threadCount);
    static void EnqueueTask(const std::string& name, std::function<void()> task);
    static void StopAll();
};

#endif
