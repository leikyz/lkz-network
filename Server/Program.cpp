#include <iostream>
#include "Server.h"
#include "MatchmakingManager.h"
#include <thread>

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

    std::cout << "\033[0m";

    std::thread matchmakingThread(MatchmakingLoop);
    matchmakingThread.detach(); // start matchmaking in a separate thread

    Server::Start();
}
