#include "LKZ/Core/Server/WindowsServer.h"
#include "LKZ/Core/Manager/MatchmakingManager.h"
#include "LKZ/Core/Engine.h"
#include "LKZ/Core/Threading/ThreadManager.h"

#include <iostream>
#include <thread>

int main()
{
#ifdef _WIN32
    auto server = new WindowsServer(5555);
#else
    auto server = new LinuxServer();
#endif

    srand(static_cast<unsigned>(time(nullptr)));

    auto& engine = Engine::Instance(server);
    engine.Initialize();

    auto& componentManager = ComponentManager::Instance();
    auto& entityManager = EntityManager::Instance();
    auto& systemManager = SystemManager::Instance();

    systemManager.RegisterSystem(std::make_shared<MovementSystem>());

    ThreadManager::CreatePool("logger", 1);

    ThreadManager::CreatePool("io", 1,[server](float) { server->Poll(); }, false);

    ThreadManager::CreatePool("message", 8);

    ThreadManager::CreatePool("matchmaking", 1);

    ThreadManager::CreatePool("player_simulation", 1,
        [&](float) { // we ignore the thread manager deltaTime
            auto& engine = Engine::Instance();
            float fixedDt = engine.GetFixedDeltaTime();

            systemManager.Update(componentManager, fixedDt);
        },
        true
    );

    engine.Run();

    ThreadManager::StopAll();
    delete server;

    return 0;
}
