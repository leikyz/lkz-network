#include "LKZ/Core/Engine.H"
#include <iostream>

// Definition of static member
INetworkInterface* Engine::network = nullptr;

Engine::Engine(INetworkInterface* netInterface)
{
    network = netInterface; 
}

void Engine::Run()
{
    network->Start();

    while (true) {
        network->Poll();
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
