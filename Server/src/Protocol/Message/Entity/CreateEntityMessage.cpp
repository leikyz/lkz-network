#include "LKZ/Protocol/Message/Entity/CreateEntityMessage.h"
#include <cstdlib> 
#include <ctime>   
#include <LKZ/Core/ECS/Manager/EntityManager.h>

CreateEntityMessage::CreateEntityMessage() {};

CreateEntityMessage::CreateEntityMessage(int entityId, int entityTypeId, float posX, float posY, float posZ)
    : entityId(entityId), entityTypeId(entityTypeId), posX(posX), posY(posY), posZ(posZ)
{
}

byte CreateEntityMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& CreateEntityMessage::serialize(Serializer& serializer) const
{
    serializer.reset();

    serializer.writeByte(ID);
    serializer.writeInt(entityId);
    serializer.writeInt(entityTypeId);
    serializer.writeFloat(posX);
    serializer.writeFloat(posY);
    serializer.writeFloat(posZ);
    serializer.writeBool(controlled);

    return serializer.getBuffer();
}

void CreateEntityMessage::deserialize(Deserializer& deserializer)
{
}


void CreateEntityMessage::process(const sockaddr_in& senderAddr)
{
    Lobby* lobby = LobbyManager::getLobby(ClientManager::getClientByAddress(senderAddr)->lobbyId);
    if (lobby != nullptr)
    {
        // Create entity
        Entity entity = EntityManager::Instance().CreateEntity(EntityType::Player, ComponentManager::Instance(), lobby);

        // Get the component manager
        auto& components = ComponentManager::Instance();

        // Change position
        components.positions[entity] = PositionComponent{ 0.0f, 0.0f, 0.0f };
        components.rotations[entity] = RotationComponent{ 0.0f, 0.0f, 0.0f };
        components.inputs[entity] = PlayerInput{ 0.0f, 0.0f, 0.0f, 0 };

        components.positions[entity].position.x = 10.0f + rand() % 5;
        components.positions[entity].position.y = 0;
        components.positions[entity].position.z = 10.0f + rand() % 5;

        entityId = entity;
        entityTypeId = (int)components.types[entity].type;
        posX = components.positions[entity].position.x;
        posY = components.positions[entity].position.y;
        posZ = components.positions[entity].position.z;


        lobby->addEntity(&entity);
        ClientManager::getClientByAddress(senderAddr)->playerEntityId = entityId;

        controlled = (ClientManager::getClientByAddress(senderAddr)->playerEntityId == entityId);

        Serializer serializer;
        serialize(serializer);

        // Send to the creator client
        Engine::Instance().Server()->Send(senderAddr, serializer.getBuffer(), getClassName());

        controlled = false;

        // Send to all other clients in lobby
        serialize(serializer);
        Engine::Instance().Server()->SendToMultiple(
            lobby->clients,
            serializer.getBuffer(),
            getClassName(),
            ClientManager::getClientByAddress(senderAddr) 
        );

        // test AI

        Entity entityAI = EntityManager::Instance().CreateEntity(EntityType::AI, ComponentManager::Instance(), lobby);

        // Change position
        components.positions[entityAI] = PositionComponent{ Vector3(0, 0, 0) };
        components.rotations[entityAI] = RotationComponent{ Vector3(0, 0, 0) };
        components.ai[entityAI] = AIComponent{ Vector3(0,0,0), std::vector<Vector3>(), 0 };

        components.positions[entityAI].position.x = 500;
        components.positions[entityAI].position.y = 35;
        components.positions[entityAI].position.z = 200;

        lobby->addEntity(&entityAI);
    }
}