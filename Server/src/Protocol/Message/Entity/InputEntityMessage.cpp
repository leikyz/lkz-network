#include "LKZ/Protocol/Message/Entity/InputEntityMessage.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>

InputEntityMessage::InputEntityMessage() {}

InputEntityMessage::InputEntityMessage(int entityId, float inputX, float inputY, float yaw, int sequenceId)
    : entityId(entityId), inputX(inputX), inputY(inputY), yaw(yaw), sequenceId(sequenceId)
{
}

uint8_t InputEntityMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& InputEntityMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    serializer.writeInt(entityId);
    serializer.writeFloat(inputX);
    serializer.writeFloat(inputY);
    serializer.writeFloat(yaw);
    serializer.writeInt(sequenceId);

    return serializer.getBuffer();
}

void InputEntityMessage::deserialize(Deserializer& deserializer)
{
    entityId = deserializer.readInt();
    inputX = deserializer.readFloat();
    inputY = deserializer.readFloat();
    yaw = deserializer.readFloat();
    sequenceId = deserializer.readInt();
}

void InputEntityMessage::process(const sockaddr_in& senderAddr)
{
    auto* client = ClientManager::getClientByAddress(senderAddr);
    if (!client) return;

    Lobby* lobby = LobbyManager::getLobby(client->lobbyId);
    if (!lobby) return;

    Entity entity = entityId;
    auto& components = ComponentManager::Instance();

    if (components.positions.find(entity) != components.positions.end())
    {
        components.inputs[entity] = PlayerInput{ inputX, inputY, yaw, sequenceId };
        components.lastReceivedSequence[entity] = sequenceId;

        if (components.rotations.find(entity) != components.rotations.end())
        {
            components.rotations[entity].rotation.y = yaw;
        }
    }
}


