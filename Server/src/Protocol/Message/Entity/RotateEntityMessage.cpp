#include "LKZ/Protocol/Message/Entity/RotateEntityMessage.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>

RotateEntityMessage::RotateEntityMessage() {};


uint8_t RotateEntityMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& RotateEntityMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    serializer.writeInt(entityId);
    serializer.writeFloat(rotaY);

    return serializer.getBuffer();
}

void RotateEntityMessage::deserialize(Deserializer& deserializer)
{
    entityId = deserializer.readInt();
    rotaY = deserializer.readFloat();
}


void RotateEntityMessage::process(const sockaddr_in& senderAddr)
{
    Logger::Log("Updated rotation for entity  to yaw: " + std::to_string(rotaY), LogType::Debug);
    auto* client = ClientManager::getClientByAddress(senderAddr);
    if (!client) return;

    Lobby* lobby = LobbyManager::getLobby(client->lobbyId);
    if (!lobby) return;

    Entity entity = entityId;
    auto& components = ComponentManager::Instance();

    if (components.rotations.find(entity) == components.rotations.end())
    {
        components.rotations[entity] = RotationComponent{ 0.0f, rotaY, 0.0f };
    }
    else
    {
        components.rotations[entity].y = rotaY;
    }

    Logger::Log("Updated rotation for entity " + std::to_string(entity) +
        " to yaw: " + std::to_string(rotaY), LogType::Debug);
}
