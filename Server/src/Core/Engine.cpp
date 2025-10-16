#include "LKZ/Core/Engine.H"
#include <iostream>
#include <thread>

// Definition of static member
INetworkInterface* Engine::network = nullptr;

Engine::Engine(INetworkInterface* netInterface)
{
    network = netInterface; 
}


void Engine::Run()
{
    network->Start();

    const float targetFrameTime = 1.0f / 60.0f; 
    float elapsedTime = 0.0f; 

    clock_t lastClock = std::clock();

    while (true)
    {
        clock_t currentClock = std::clock();
        float frameTime = float(currentClock - lastClock) / CLOCKS_PER_SEC;
        lastClock = currentClock;

        elapsedTime += frameTime;
        deltaTime = elapsedTime; 


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
    std::cout << "[MAIN] Starting engine...\n";
}

INetworkInterface* Engine::Server()
{
    return network;
}

float Engine::GetDeltaTime()
{
	return deltaTime;
}
