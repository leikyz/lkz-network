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
    auto* client = ClientManager::getClientByAddress(senderAddr);
    if (!client) return;

    Lobby* lobby = LobbyManager::getLobby(client->lobbyId);
    if (!lobby) return;

    Entity entity = entityId;
    auto& components = ComponentManager::Instance();
    float halfYaw = rotaY * 0.5f;
    float sinHalfYaw = sin(halfYaw);
    float cosHalfYaw = cos(halfYaw);
    components.rotations[entityId].rotation = Quaternion(cosHalfYaw, 0.0f, sinHalfYaw, 0.0f);
    if (components.rotations.find(entity) == components.rotations.end())
    {
        components.rotations[entity] = RotationComponent{ Quaternion::FromEuler(0.0f, rotaY, 0.0f) };
    }
    else
    {
        components.rotations[entity].rotation = Quaternion::FromEuler(0.0f, rotaY, 0.0f);

    }
    //Logger::Log("Received RotateEntityMessage for entity " + std::to_string(entityId) +
    //    " with rotaY = " + std::to_string(rotaY), LogType::Info);
}

