#include "LKZ/Protocol/Message/Entity/Player/PlayerInputMessage.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>
#include <LKZ/Core/Manager/ClientManager.h>
#include <LKZ/Core/Manager/LobbyManager.h>
#include <LKZ/Core/Engine.h>
#include <LKZ/Utility/Constants.h> // For FIXED_DELTA_TIME if needed

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
    serializer.writeInt(sequenceId);

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

    if (components.playerInputs.find(entity) != components.playerInputs.end())
    {
        // Create your data struct
        PlayerInputData data;
        data.inputX = inputX;
        data.inputY = inputY;
        data.yaw = yaw;
        data.sequenceId = sequenceId;
        // If you aren't sending DT in the packet, set it to 0 so System uses Server FixedDT
        data.deltaTime = 0.0f;

        // --- PUSH TO QUEUE ---
        components.playerInputs[entity].inputQueue.push_back(data);

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