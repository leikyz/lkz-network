#pragma once
#include <functional>
#include <queue>
#include <mutex>
#include <vector>

class CommandQueue
{
public:
    using Command = std::function<void()>;

    static CommandQueue& Instance()
    {
        static CommandQueue instance;
        return instance;
    }

    void Push(Command cmd)
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_queue.push(std::move(cmd));
    }

    void ProcessAllCommands()
    {
        std::vector<Command> commandsToRun;
        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            commandsToRun.reserve(m_queue.size());
            while (!m_queue.empty())
            {
                commandsToRun.push_back(std::move(m_queue.front()));
                m_queue.pop();
            }
        } 

        for (const auto& cmd : commandsToRun)
        {
            cmd(); 
        }
    }


private:
    CommandQueue() = default;
    CommandQueue(const CommandQueue&) = delete;
    CommandQueue& operator=(const CommandQueue&) = delete;

    std::queue<Command> m_queue;
    std::mutex m_queueMutex; 
};
