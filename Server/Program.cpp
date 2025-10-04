#include <iostream>
#include <thread>
#include "LKZ/Core/Server/WindowsServer.h"
#include "LKZ/Manager/MatchmakingManager.h"
#include <LKZ/Core/Log/Logger.h>
#include <LKZ/Core/Engine.h>

int main()
{
    std::cout << "[MAIN] Starting engine...\n";

    // Create thread pools
    ThreadManager::CreatePool("logger", 1);   // Logger thread
    ThreadManager::CreatePool("network", 10); // Network thread pool
    ThreadManager::CreatePool("matchmaking", 1); // Matchmaking thread

    // Create Windows server
    auto windowsServer = new WindowsServer(5555);

    // Start the poll loop in its own thread
    std::thread pollingThread([windowsServer]() {
        windowsServer->Poll();
        });

    // Create engine with the server interface
    Engine engine(windowsServer);

    // Run engine
    engine.Run();

    // Cleanup
    pollingThread.join(); // wait for polling thread to finish (or detach if desired)
    // ThreadManager::StopAll();
    delete windowsServer;

    return 0;
}
