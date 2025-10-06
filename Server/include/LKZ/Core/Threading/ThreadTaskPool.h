#pragma once
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <memory>
#include <string>

/**
 * @brief A thread pool that manages a queue of tasks to be executed by worker threads.
 */
class ThreadTaskPool
{
public:
    /**
    * @brief Type definition for a loop hook function that can be executed in the worker loop.
	*/
    using LoopHook = std::function<void()>;

    ThreadTaskPool(LoopHook hook = nullptr, bool loopMode = false);
    ~ThreadTaskPool();

    /**
    * @brief The main loop for worker threads that processes tasks from the queue.
	*/ 
    void WorkerLoop();

    /**
	* @brief Enqueues a new task to be executed by the thread pool.
    * 
    * \param task
    */
    void EnqueueTask(std::function<void()> task);

    /**
	*@brief Stops the thread pool and notifies all worker threads to exit.
    */
    void Stop();

private:
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
    bool loopMode;
    LoopHook loopHook;
};
