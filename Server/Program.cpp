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

    systemManager.RegisterSystem(std::make_shared<PlayerSystem>());
    systemManager.RegisterSystem(std::make_shared<AISystem>());

    ThreadManager::CreatePool("logger", 1);
    ThreadManager::CreatePool("io", 1,[server](float) { server->Poll(); }, false);
    ThreadManager::CreatePool("message", 8);
    ThreadManager::CreatePool("matchmaking", 1);
    ThreadManager::CreatePool("player_simulation", 1,[&](float) 
        { auto& engine = Engine::Instance();float fixedDt = engine.GetFixedDeltaTime(); systemManager.Update(componentManager, fixedDt); }, true);

    ThreadManager::CreatePool("ai_simulation", 1, [&](float)
        { auto& engine = Engine::Instance(); float deltaTime = engine.GetDeltaTime(); systemManager.Update(componentManager, deltaTime); }, true);

    World* world = new World();
	engine.SetWorld(world);
    world->initialize();

    engine.Run();

    ThreadManager::StopAll();
    delete server;

    return 0;
}
