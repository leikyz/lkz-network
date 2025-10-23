#include "LKZ/Protocol/Message/Entity/RequestCreateEntityMessage.h"
#include <cstdlib>
#include <ctime>

// Nouveaux includes nécessaires pour la Command Queue
#include "LKZ/Core/Threading/CommandQueue.h"
#include "LKZ/Core/ECS/Manager/EntityManager.h"
#include "LKZ/Core/ECS/Manager/ComponentManager.h"
#include "LKZ/Core/ECS/Manager/NavMeshQueryManager.h"
#include "LKZ/Core/Manager/LobbyManager.h" // Supposé, pour LobbyManager
#include "LKZ/Core/Manager/ClientManager.h" // Supposé, pour ClientManager
#include "LKZ/Core/Engine.h"
#include "LKZ/Protocol/Message/Entity/CreateEntityMessage.h"
#include "LKZ/Utility/Logger.h" // Pour Logger
#include <LKZ/Core/Threading/CommandQueue.h>

RequestCreateEntityMessage::RequestCreateEntityMessage() {};

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
    if (lobby == nullptr)
    {
        Logger::Log("RequestCreateEntityMessage: Lobby introuvable.", LogType::Warning);
        return;
    }

    int superTypeId = this->entitySuperTypeId;
    INetworkInterface* server = Engine::Instance().Server();

    CommandQueue::Instance().Push([=]() 
    {

        auto& components = ComponentManager::Instance();
        auto& world = Engine::Instance().GetWorld();

        Entity entity = EntityManager::Instance().CreateEntity(EntitySuperType(superTypeId), components, lobby);

        Logger::Log("Creating entity of type " + std::to_string(superTypeId) + " with ID " + std::to_string(entity) + " for client " + client->ipAddress, LogType::Info);

        client->playerEntityId = entity;

        CreateEntityMessage createEntityMsg;

        components.AddComponent(entity, PositionComponent{ 0.0f, 0.0f, 0.0f });
        components.AddComponent(entity, RotationComponent{ 0.0f, 0.0f, 0.0f });

        if (superTypeId == (int)EntitySuperType::Player)
        {
            components.AddComponent(entity, PlayerInputComponent{ 0.0f, 0.0f, 0.0f, 0 });

            components.positions[entity].position.x = 10.0f + rand() % 5;
            components.positions[entity].position.y = 0;
            components.positions[entity].position.z = 10.0f + rand() % 5;

            createEntityMsg.entityTypeId = (int)EntityType::Player1;
        }
        else // Zombie
        {
            dtNavMeshQuery* simQuery = NavMeshQueryManager::GetThreadLocalQuery(world.getNavMesh());
            Vector3 randomSpawnPoint = world.getRandomNavMeshPoint(simQuery);

            components.positions[entity].position.x = randomSpawnPoint.x;
            components.positions[entity].position.y = randomSpawnPoint.y;
            components.positions[entity].position.z = randomSpawnPoint.z;

            components.AddComponent(entity, AIComponent{ Vector3(0,0,0), std::vector<Vector3>(), 0 });

            createEntityMsg.entityTypeId = (int)EntityType::Zombie1;
        }

        createEntityMsg.entityId = entity;
        createEntityMsg.posX = components.positions[entity].position.x;
        createEntityMsg.posY = components.positions[entity].position.y;
        createEntityMsg.posZ = components.positions[entity].position.z;

        lobby->addEntity(&entity);

        Serializer serializer;
        createEntityMsg.serialize(serializer);

        // Player
        if (superTypeId == (int)EntitySuperType::Player)
        {
            server->Send(senderAddr, serializer.getBuffer(), createEntityMsg.getClassName());

            createEntityMsg.entityTypeId = (int)EntityType::PlayerSynced1;
            createEntityMsg.serialize(serializer);

            server->SendToMultiple(
                lobby->clients,
                serializer.getBuffer(),
                createEntityMsg.getClassName(),
                client 
            );
        }
        // Zombie
        else
        {
            server->SendToMultiple(
                lobby->clients,
                serializer.getBuffer(),
                createEntityMsg.getClassName()
            );
        }
        }); 
}
