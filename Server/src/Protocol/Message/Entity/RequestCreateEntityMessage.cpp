#include "LKZ/Protocol/Message/Entity/RequestCreateEntityMessage.h"
#include <cstdlib> 
#include <ctime>   
#include <LKZ/Core/ECS/Manager/EntityManager.h>
#include <LKZ/Protocol/Message/Entity/CreateEntityMessage.h>
#include <LKZ/Core/ECS/Manager/NavMeshQueryManager.h>

RequestCreateEntityMessage::RequestCreateEntityMessage() {};

RequestCreateEntityMessage::RequestCreateEntityMessage(int entityTypeId)
    :  entitySuperTypeId(entityTypeId)
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
    Lobby* lobby = LobbyManager::getLobby(ClientManager::getClientByAddress(senderAddr)->lobbyId);
    if (lobby != nullptr)
    {
        // Create entity
        Entity entity = EntityManager::Instance().CreateEntity(EntitySuperType(entitySuperTypeId), ComponentManager::Instance(), lobby);


		Logger::Log("Creating entity of type " + std::to_string(entitySuperTypeId) + " with ID " + std::to_string(entity) + " for client " + ClientManager::getClientByAddress(senderAddr)->ipAddress, LogType::Info);
        // Get the component manager
        auto& components = ComponentManager::Instance();

        ClientManager::getClientByAddress(senderAddr)->playerEntityId = entity;

        CreateEntityMessage createEntityMsg;

        components.positions[entity] = PositionComponent{ 0.0f, 0.0f, 0.0f };
        components.rotations[entity] = RotationComponent{ 0.0f, 0.0f, 0.0f };

        if (entitySuperTypeId == (int)EntitySuperType::Player)
        {
            components.inputs[entity] = PlayerInputComponent{ 0.0f, 0.0f, 0.0f, 0 };

            components.positions[entity].position.x = 10.0f + rand() % 5;
            components.positions[entity].position.y = 0;
            components.positions[entity].position.z = 10.0f + rand() % 5;

            createEntityMsg.entityTypeId = (int)EntityType::Player1;
        }
        else
        {
            Vector3 randomSpawnPoint = Engine::Instance().GetWorld().getRandomNavMeshPoint(
                NavMeshQueryManager::GetThreadLocalQuery(Engine::Instance().GetWorld().getNavMesh()));

            components.positions[entity].position.x = randomSpawnPoint.x;
            components.positions[entity].position.y = randomSpawnPoint.y;
            components.positions[entity].position.z = randomSpawnPoint.z;
			components.ai[entity] = AIComponent{ Vector3(0,0,0), std::vector<Vector3>(), 0 };

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
        if (entitySuperTypeId == (int)EntitySuperType::Player)
        {
            // Send to the creator client
            Engine::Instance().Server()->Send(senderAddr, serializer.getBuffer(), createEntityMsg.getClassName());

            // Send to all other clients in lobby and mark as synced entity
            createEntityMsg.entityTypeId = (int)EntityType::PlayerSynced1;
            createEntityMsg.serialize(serializer);

            Engine::Instance().Server()->SendToMultiple(
                lobby->clients,
                serializer.getBuffer(),
                createEntityMsg.getClassName(),
                ClientManager::getClientByAddress(senderAddr)
            );
        }
        // Zombie
        else
        {
            Engine::Instance().Server()->SendToMultiple(
                lobby->clients,
                serializer.getBuffer(),
                createEntityMsg.getClassName()
            );
        }
    }
}