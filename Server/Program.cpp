#include "LKZ/Core/Server/WindowsServer.h"
#include "LKZ/Core/Manager/MatchmakingManager.h"
#include "LKZ/Core/Engine.h"
#include "LKZ/Core/Threading/ThreadManager.h"
#include "LKZ/Simulation/Navmesh/NavMeshLoader.h" 
#include "LKZ/Simulation/World.h"
#include <iostream>
#include <thread>
#include <DetourCrowd.h>
#include <LKZ/Core/ECS/System/Player/PlayerSystem.h>
#include "LKZ/Core/Threading/CommandQueue.h"
#include "LKZ/Utility/Logger.h"
#include <string>
#include <LKZ/Core/ECS/System/AISystem.h>



int main()
{
#ifdef _WIN32
    WindowsServer* server = new WindowsServer(5555);
#else
    LinuxServer server = new LinuxServer();
#endif

    Engine& engine = Engine::Instance(server);
    engine.Initialize();

    ComponentManager& componentManager = ComponentManager::Instance();
    EntityManager& entityManager = EntityManager::Instance();
    SystemManager& systemManager = SystemManager::Instance();

    World* world = new World();
    engine.SetWorld(world);
    world->initialize();

    systemManager.RegisterSystem(std::make_shared<PlayerSystem>());
    systemManager.RegisterSystem(std::make_shared<AISystem>());

    ThreadManager::CreatePool("logger", 1);
    ThreadManager::CreatePool("io", 1, [server](float) { server->Poll(); }, false);
    ThreadManager::CreatePool("message", 8);
    ThreadManager::CreatePool("matchmaking", 1);
    ThreadManager::CreatePool("pathfinding", 2);
    ThreadManager::CreatePool("simulation", 1, [&](float)
        {
            auto& engine = Engine::Instance();
            auto& components = ComponentManager::Instance();
			float fixedDt = Constants::FIXED_DELTA_TIME;

            CommandQueue::Instance().ProcessAllCommands();
          
            if (world)
                world->UpdateCrowd(fixedDt);

            systemManager.Update(components, fixedDt);

        }, true);


    engine.Run();

    ThreadManager::StopAll();
    delete world;
    delete server;

    return 0;
}
