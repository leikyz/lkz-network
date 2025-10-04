#include <iostream>
#include "LKZ/Core/Server.h"
#include "LKZ/Manager/MatchmakingManager.h"
#include <thread>
#include <LKZ/Core/Logger.h>

void MatchmakingLoop()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        MatchmakingManager::Update();
     
    }
}

int main()
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

    int score = 42;
    ThreadManager::CreatePool("logger", 1);
    Logger::Log("Server initialized.", LogType::Info);
    ThreadManager::CreatePool("logger", 1);
    Logger::Log("Waiting for clients...", LogType::Debug);

    std::cout << "\033[0m";

    std::thread matchmakingThread(MatchmakingLoop);
    matchmakingThread.detach(); // start matchmaking in a separate thread

    Server::Start();
}
