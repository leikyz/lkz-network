#include "LKZ/Protocol/Message/Entity/Player/PlayerInputMessage.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>
#include <LKZ/Core/Manager/ClientManager.h>
#include <LKZ/Core/Manager/LobbyManager.h>
#include <LKZ/Core/Engine.h>
#include <LKZ/Utility/Constants.h>
#include "LKZ/Core/Threading/CommandQueue.h" // Required for the fix

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

    // Capture data locally to pass into the lambda
    Entity entity = entityId;
    float currentYaw = yaw;

    PlayerInputData packet;
    packet.inputX = inputX;
    packet.inputY = inputY;
    packet.yaw = yaw;
    packet.sequenceId = sequenceId;

    CommandQueue::Instance().Push([entity, packet, currentYaw]()
        {
            auto& components = ComponentManager::Instance();

            // Check if entity still exists (it might have disconnected by the time this runs)
            if (components.playerInputs.find(entity) != components.playerInputs.end())
            {
                // Safe to push_back now because we are on the main thread
                components.playerInputs[entity].inputQueue.push_back(packet);
            }

            if (components.rotations.find(entity) != components.rotations.end())
            {
                components.rotations[entity].rotation.y = currentYaw;
            }
        });

    Serializer serializer;
    serialize(serializer);

    Engine::Instance().Server()->SendToMultiple(
        lobby->clients,
        serializer.getBuffer(),
        getClassName(),
        client);
}