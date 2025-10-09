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
        srand(static_cast<unsigned>(time(0)));

        // Create entity
        Entity entity = EntityManager::Instance().CreateEntity(EntityType::Player, ComponentManager::Instance(), lobby);

        // Get the component manager
        auto& components = ComponentManager::Instance();

        // Change position
        components.positions[entity] = PositionComponent{ Vector3() };
        components.rotations[entity] = RotationComponent{ Quaternion() };
        components.inputs[entity] = InputComponent{ Vector2() };

        components.positions[entity].position.x = 100.0f + rand() % 10;
        components.positions[entity].position.y = 10;
        components.positions[entity].position.z = 100.0f + rand() % 10;

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
    }
}


