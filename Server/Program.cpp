#include "LKZ/Core/Server/WindowsServer.h"
#include "LKZ/Core/Manager/MatchmakingManager.h"
#include <iostream>
#include <LKZ/Core/Engine.h>
#include <LKZ/Core/Log/Logger.h>
#include <thread>
#include <LKZ/Core/Manager/SystemManager.h>
#include <LKZ/Core/ECS/System/Player/MovementSystem.h>

int main()
{
    #ifdef _WIN32
        auto server = new WindowsServer(5555);
    #else
        auto server = new LinuxServer();
    #endif

    Engine::Instance(server).Initialize();
 
    ThreadManager::CreatePool("logger", 1);   // Logger thread
    ThreadManager::CreatePool("io", 1, [server](float) { server->Poll(); }); 
    ThreadManager::CreatePool("message", 8);
    ThreadManager::CreatePool("matchmaking", 1);

    auto systemManager = std::make_shared<SystemManager>();
    auto componentsManager = std::make_shared<ComponentManager>();
    systemManager->RegisterSystem(std::make_shared<MovementSystem>());

    ThreadManager::CreatePool("player_simulation", 1,
        [systemManager, &componentsManager](float deltaTime) {
            systemManager->Update(*componentsManager, Engine::Instance().GetDeltaTime());
        },
        true // continuous loop
    );


    Engine::Instance().Run();


    // Cleanup
    ThreadManager::StopAll();
    delete server;

    return 0;
}
