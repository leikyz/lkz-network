#include <iostream>
#include <thread>
#include "LKZ/Core/Server/WindowsServer.h"
#include "LKZ/Manager/MatchmakingManager.h"
#include <LKZ/Core/Log/Logger.h>
#include <LKZ/Core/Engine.h>

int main()
{
    #ifdef _WIN32
        auto server = new WindowsServer(5555);
    #else
        auto server = new LinuxServer();
    #endif

    Engine::Instance(server).Initialize();
 
    ThreadManager::CreatePool("logger", 1);   // Logger thread
    ThreadManager::CreatePool("io", 1, [server]() { server->Poll();});
	ThreadManager::CreatePool("message", 10); // Message logic processing threads
    ThreadManager::CreatePool("matchmaking", 1); // Matchmaking thread

    Engine::Instance().Run();
    // Cleanup
    ThreadManager::StopAll();
    delete server;

    return 0;
}
