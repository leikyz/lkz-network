#include "LKZ/Protocol/Message/Entity/RequestCreateEntityMessage.h"
#include <cstdlib>
#include <ctime>

// Nouveaux includes nécessaires
#include "LKZ/Core/Threading/CommandQueue.h"
#include "LKZ/Core/Threading/ThreadManager.h"
#include "LKZ/Core/ECS/Manager/EntityManager.h"
#include "LKZ/Core/ECS/Manager/ComponentManager.h"
#include "LKZ/Core/ECS/Manager/NavMeshQueryManager.h"
#include "LKZ/Core/Manager/LobbyManager.h"
#include "LKZ/Core/Manager/ClientManager.h"
#include "LKZ/Core/Engine.h"
#include "LKZ/Protocol/Message/Entity/CreateEntityMessage.h"
#include "LKZ/Utility/Logger.h"
#include <DetourCrowd.h>

// Include for Vector3
#include "LKZ/Simulation/Math/Vector.h" 

RequestCreateEntityMessage::RequestCreateEntityMessage() {}

RequestCreateEntityMessage::RequestCreateEntityMessage(int entityTypeId)
    : entitySuperTypeId(entityTypeId)
{
}

uint8_t RequestCreateEntityMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& RequestCreateEntityMessage::serialize(Serializer& serializer) const
{
    serializer.reset();
    return serializer.getBuffer();
}

void RequestCreateEntityMessage::deserialize(Deserializer& deserializer)
{
    entitySuperTypeId = deserializer.readByte();
}

void RequestCreateEntityMessage::process(const sockaddr_in& senderAddr)
{
    Client* client = ClientManager::getClientByAddress(senderAddr);
    if (!client)
    {
        Logger::Log("RequestCreateEntityMessage: Client introuvable.", LogType::Warning);
        return;
    }

    Lobby* lobby = LobbyManager::getLobby(client->lobbyId);
    if (!lobby)
    {
        Logger::Log("RequestCreateEntityMessage: Lobby introuvable.", LogType::Warning);
        return;
    }

    int superTypeId = this->entitySuperTypeId;
    INetworkInterface* server = Engine::Instance().Server();

    // --- SPAWN PLAYER ---
    if (superTypeId == (int)EntitySuperType::Player)
    {
        CommandQueue::Instance().Push([=]() {
            auto& components = ComponentManager::Instance();
            Entity entity = EntityManager::Instance().CreateEntity(EntitySuperType(superTypeId), components, lobby);

            Logger::Log("Creating Player entity " + std::to_string(entity) +
                " for client " + client->ipAddress, LogType::Info);

            client->playerEntityId = entity;

            components.AddComponent(entity, PositionComponent{});
            components.AddComponent(entity, RotationComponent{});
            components.AddComponent(entity, PlayerInputComponent{});
            components.AddComponent(entity, PlayerStateComponent{});

            components.positions[entity].position.x = 10.0f + rand() % 5;
            components.positions[entity].position.y = 0;
            components.positions[entity].position.z = 10.0f + rand() % 5;

            CreateEntityMessage createEntityMsg;
            createEntityMsg.entityTypeId = (int)EntityType::Player1;
            createEntityMsg.entityId = entity;
            createEntityMsg.posX = components.positions[entity].position.x;
            createEntityMsg.posY = components.positions[entity].position.y;
            createEntityMsg.posZ = components.positions[entity].position.z;

            lobby->addEntity(&entity);

            Serializer serializer;
            createEntityMsg.serialize(serializer);

            server->Send(senderAddr, serializer.getBuffer(), createEntityMsg.getClassName());

            createEntityMsg.entityTypeId = (int)EntityType::PlayerSynced1;
            createEntityMsg.serialize(serializer);

            server->SendToMultiple(lobby->clients,
                serializer.getBuffer(),
                createEntityMsg.getClassName(),
                client);
            });
    }
    else // Zombie (or other AI)
    {
        // AI spawning is SLOW. Push to a worker thread first.
        ThreadManager::GetPool("pathfinding")->EnqueueTask([=]() {

            World& world = Engine::Instance().GetWorld();
            dtNavMeshQuery* simQuery = NavMeshQueryManager::GetThreadLocalQuery(world.getNavMesh());
            Vector3 randomSpawnPoint = world.getRandomNavMeshPoint(simQuery);

            // Now, push the agent creation to the main simulation thread
            CommandQueue::Instance().Push([=]() {

                // --- THIS CODE RUNS ON THE MAIN SIMULATION THREAD ---
                auto& components = ComponentManager::Instance();
                auto& world = Engine::Instance().GetWorld();
                Entity entity = EntityManager::Instance().CreateEntity(EntitySuperType(superTypeId), components, lobby);

                Logger::Log("Creating entity of type " + std::to_string(superTypeId) + " with ID " + std::to_string(entity), LogType::Info);

                components.AddComponent(entity, PositionComponent{ randomSpawnPoint.x, randomSpawnPoint.y, randomSpawnPoint.z });
                components.AddComponent(entity, RotationComponent{ 0.0f, 0.0f, 0.0f });

                components.AddComponent(entity, AIComponent{
                    std::nullopt, // targetPosition
                    0.0f,         // repathTimer
                    -1            // crowdAgentIndex (default -1)
                    });

                dtCrowd* crowd = world.getCrowd();
                if (crowd)
                {
                    dtCrowdAgentParams params;

                    memset(&params, 0, sizeof(params));

                    params.radius = 0.1f; 
                    params.height = 1.8f;
                    params.maxAcceleration = 2.0f; 
                    params.maxSpeed = 0.4f;        

                    params.collisionQueryRange = params.radius * 8.0f;
                    // How far to look for walls (static obstacles)
                    params.pathOptimizationRange = params.radius * 30.0f;
                    // This MUST match the filter you set up in World.cpp
                    params.queryFilterType = 0;
                    params.obstacleAvoidanceType = 3; // Use high quality avoidance
                    params.separationWeight = 2.0f;   // How strongly to push other agents away
                    params.updateFlags = 0;
                    params.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
                    params.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
                    params.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
                    params.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE; 
                    params.updateFlags |= DT_CROWD_SEPARATION;    

                    params.userData = (void*)((uintptr_t)entity);
                    int agentIdx = crowd->addAgent(randomSpawnPoint.data(), &params);

                    if (agentIdx != -1)
                    {
                        components.ai[entity].crowdAgentIndex = agentIdx;
                    }
                }

                CreateEntityMessage createEntityMsg;
                createEntityMsg.entityTypeId = (int)EntityType::Zombie1;
                createEntityMsg.entityId = entity;
                createEntityMsg.posX = randomSpawnPoint.x;
                createEntityMsg.posY = randomSpawnPoint.y;
                createEntityMsg.posZ = randomSpawnPoint.z;

                lobby->addEntity(&entity);

                Serializer serializer;
                createEntityMsg.serialize(serializer);

                server->SendToMultiple(
                    lobby->clients,
                    serializer.getBuffer(),
                    createEntityMsg.getClassName()
                );
                });
            });
    }
}

