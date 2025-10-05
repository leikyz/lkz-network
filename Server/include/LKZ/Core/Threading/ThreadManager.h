#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "ThreadTaskPool.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <functional>


class ThreadManager {
public:
    using PoolPtr = std::shared_ptr<ThreadTaskPool>;

    static void CreatePool(const std::string& name, int threads, ThreadTaskPool::LoopHook hook = nullptr);
    static PoolPtr GetPool(const std::string& name);
    static void StopAll();

private:
    static std::unordered_map<std::string, PoolPtr> pools;
};

#endif
