#pragma once
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

class ThreadTaskPool
{
public:
    using LoopHook = std::function<void(float)>;

    ThreadTaskPool(LoopHook hook = nullptr, bool loopMode = false);
    ~ThreadTaskPool();

    void Start();
    void Stop();
    void EnqueueTask(std::function<void()> task);

    void WorkerLoop();
    void SetDeltaTime(float dt);

private:
    std::thread worker;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;

    std::atomic<bool> running{ false };
    std::atomic<bool> stopRequested{ false };
    bool loopMode = false;
    LoopHook loopHook;
    float deltaTime = 0.0f;
};
