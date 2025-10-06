#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "ThreadTaskPool.h"
#include <unordered_map>
#include <memory>
#include <string>
#include <functional>

 /**
 * @brief Manages multiple thread pools identified by unique names.
 */
class ThreadManager 
{
public:
    /**
    * @brief Shared pointer type for ThreadTaskPool.
	*/
    using PoolPtr = std::shared_ptr<ThreadTaskPool>;

    /**
	 * @brief Creates a new thread pool with the specified name and number of threads.
     * 
     * \param name
     * \param threads
     * \param hook
     */
    static void CreatePool(const std::string& name, int threads, ThreadTaskPool::LoopHook hook = nullptr, bool isLoop = false);

    /**
	 * @brief Retrieves a thread pool by its name.
     * 
     * \param name
     * \return 
     */
    static PoolPtr GetPool(const std::string& name);

    /**
	 * @brief Stops all thread pools and their associated threads.
     * 
     */
    static void StopAll();

private:
    static std::unordered_map<std::string, PoolPtr> pools;
};

#endif
