#include "LKZ/Core/Server/WindowsServer.h"
#include "LKZ/Core/Manager/MatchmakingManager.h"
#include <iostream>
#include <LKZ/Core/Engine.h>
#include <thread>

int main()
{
    #ifdef _WIN32
        auto server = new WindowsServer(5555);
    #else
        auto server = new LinuxServer();
    #endif

    Engine::Instance(server).Initialize();

    auto& componentManager = ComponentManager::Instance();
    auto& entityManager = EntityManager::Instance();
    auto& systemManager = SystemManager::Instance();
 
    ThreadManager::CreatePool("logger", 1);   // Logger thread
    ThreadManager::CreatePool("io", 1, [server](float) { server->Poll(); }); 
    ThreadManager::CreatePool("message", 8);
    ThreadManager::CreatePool("matchmaking", 1);

    ThreadManager::CreatePool("player_simulation", 1,
        [&](float deltaTime) { // <-- match required signature
            systemManager.Update(componentManager, deltaTime);
        },
        true
    );
	systemManager.RegisterSystem(std::make_shared<MovementSystem>());   

    // Create your entities before starting the simsysulation thread
    entityManager.CreateEntity(EntityType::Player, componentManager);

    Engine::Instance().Run();


    // Cleanup
    ThreadManager::StopAll();
    delete server;

    return 0;
}
