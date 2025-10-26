#include "LKZ/Protocol/Message/Entity/Player/PlayerInputMessage.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>

PlayerInputMessage::PlayerInputMessage() {}

PlayerInputMessage::PlayerInputMessage(uint16_t entityId, float inputX, float inputY, float yaw, int sequenceId)
    : entityId(entityId), inputX(inputX), inputY(inputY), yaw(yaw), sequenceId(sequenceId)
{
}

uint8_t PlayerInputMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& PlayerInputMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    serializer.writeUInt16(entityId);
    serializer.writeFloat(inputX);
    serializer.writeFloat(inputY);
	serializer.writeFloat(yaw);

    return serializer.getBuffer();
}

void PlayerInputMessage::deserialize(Deserializer& deserializer)
{
    entityId = deserializer.readUInt16();
    inputX = deserializer.readFloat();
    inputY = deserializer.readFloat();
    yaw = deserializer.readFloat();
    sequenceId = deserializer.readInt();
}

void PlayerInputMessage::process(const sockaddr_in& senderAddr)
{
    auto* client = ClientManager::getClientByAddress(senderAddr);
    if (!client) return;

    Lobby* lobby = LobbyManager::getLobby(client->lobbyId);
    if (!lobby) return;

    Entity entity = entityId;
    auto& components = ComponentManager::Instance();

    if (components.positions.find(entity) != components.positions.end())
    {
        components.playerInputs[entity] = PlayerInputComponent{ inputX, inputY, yaw, sequenceId };
        components.lastReceivedSequence[entity] = sequenceId;

        if (components.rotations.find(entity) != components.rotations.end())
        {
            components.rotations[entity].rotation.y = yaw;
        }
    }


    Serializer serializer;
    serialize(serializer);

    Engine::Instance().Server()->SendToMultiple(
        lobby->clients,
        serializer.getBuffer(),
        getClassName(),
        ClientManager::getClientByAddress(senderAddr));

}


