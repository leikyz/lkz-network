#pragma once
#include "LKZ/Core/Server/INetworkInterface.h"
#include "LKZ/Core/Threading/ThreadManager.h"
#include <chrono>

class Engine
{
public:
    /**
     * @brief Singleton instance accessor. Initializes with the provided network interface if not already initialized.
     *
     * \param netInterface
     * \return
     */
    static Engine& Instance(INetworkInterface* netInterface = nullptr)
    {
        static Engine instance(netInterface);
        return instance;
    }

    void Initialize();
    void Run();

    float GetFixedDeltaTime() const { return fixedDeltaTime; }
    float GetDeltaTime() const { return deltaTime; }
    static INetworkInterface* Server();

private:
    Engine(INetworkInterface* netInterface);
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    static INetworkInterface* network;

    std::chrono::steady_clock::time_point lastFrame;
    float deltaTime = 0.0f;
    float fixedDeltaTime = 0.02f; // 50 Hz

};
