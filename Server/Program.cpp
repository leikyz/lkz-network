#include "LKZ/Core/Server/WindowsServer.h"
#include "LKZ/Core/Manager/MatchmakingManager.h"
#include "LKZ/Core/Engine.h"
#include "LKZ/Core/Threading/ThreadManager.h"
#include "LKZ/Simulation/Navmesh/NavMeshLoader.h" 
#include <iostream>
#include <thread>
#include <DetourCrowd.h>

int main()
{
#ifdef _WIN32
    auto server = new WindowsServer(5555);
#else
    auto server = new LinuxServer();
#endif

    Engine& engine = Engine::Instance(server);
    engine.Initialize();

    ComponentManager& componentManager = ComponentManager::Instance();
    EntityManager& entityManager = EntityManager::Instance();
    SystemManager& systemManager = SystemManager::Instance();

    systemManager.RegisterSystem(std::make_shared<MovementSystem>());

    NavMeshLoader navMeshLoader;
    if (!navMeshLoader.LoadFromFile("NavMeshExport.txt"))
    {
        std::cerr << "Error : Can't load navmesh file" << std::endl;
        return -1;
    }

    dtNavMesh* navMesh = navMeshLoader.BuildNavMesh();
    if (!navMesh)
    {
        std::cerr << "Error : Can't build the navmesh" << std::endl;
        return -1;
    }


    dtNavMeshQuery* navQuery = dtAllocNavMeshQuery();
    if (navQuery)
    {
        navQuery->init(navMesh, 2048);
        std::cout << "dtNavMeshQuery initialized." << std::endl;
        dtFreeNavMeshQuery(navQuery);
    }

    ThreadManager::CreatePool("logger", 1);
    ThreadManager::CreatePool("io", 1,[server](float) { server->Poll(); }, false);
    ThreadManager::CreatePool("message", 8);
    ThreadManager::CreatePool("matchmaking", 1);
    ThreadManager::CreatePool("player_simulation", 1,[&](float) 
        { auto& engine = Engine::Instance();float fixedDt = engine.GetFixedDeltaTime(); systemManager.Update(componentManager, fixedDt); }, true);

    engine.Run();

    ThreadManager::StopAll();
    delete server;

    return 0;
}
