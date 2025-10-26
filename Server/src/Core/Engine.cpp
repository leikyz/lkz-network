#include "LKZ/Core/Engine.H"
#include <iostream>
#include <thread>

INetworkInterface* Engine::network = nullptr;

Engine::Engine(INetworkInterface* netInterface)
{
    network = netInterface; 
}


void Engine::Run()
{
    network->Start();

    const float targetFrameTime = 1.0f / 60.0f; 
    float accumulator = 0.0f;

    lastFrame = std::chrono::steady_clock::now();

    while (true)
    {
        auto now = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration<float>(now - lastFrame).count();
        lastFrame = now;

        accumulator += deltaTime;

        ThreadManager::SetGlobalDeltaTime(deltaTime);

        while (accumulator >= fixedDeltaTime)
        {
            ThreadManager::SetGlobalDeltaTime(fixedDeltaTime);
            accumulator -= fixedDeltaTime;
        }

        network->Poll();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}


void Engine::Initialize()
{
    std::cout << "\033[34m";
    std::cout << R"(
                                                       _    _  __ ____           
                                                      | |  | |/ /|_  /           
                                                      | |__| ' <  / /            
                                           _  _ ___ __|____|_|\_\/___|  ___ _  __
                                          | \| | __|_   _\ \    / / _ \| _ \ |/ /
                                          | .` | _|  | |  \ \/\/ / (_) |   / ' < 
                                          |_|\_|___| |_|   \_/\_/ \___/|_|_\_|\_\
                                         
    )" << std::endl;
    std::cout << "\033[0m";
    std::cout << "[Main] Starting engine...\n";
    lastFrame = std::chrono::steady_clock::now();

}

INetworkInterface* Engine::Server()
{
    return network;
}

