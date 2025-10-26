#include "LKZ/Protocol/Message/Entity/Player/PlayerStateMessage.h"
#include <LKZ/Core/ECS/Manager/ComponentManager.h>
#include <LKZ/Core/ECS/Manager/EntityManager.h>

PlayerStateMessage::PlayerStateMessage() {}

PlayerStateMessage::PlayerStateMessage(uint16_t entityId, uint8_t flags)
    : entityId(entityId), flags(flags)
{
}

uint8_t PlayerStateMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& PlayerStateMessage::serialize(Serializer& serializer) const
{
    return serializer.getBuffer();
}

void PlayerStateMessage::deserialize(Deserializer& deserializer)
{
    entityId = deserializer.readUInt16();
    flags = deserializer.readByte();
}

void PlayerStateMessage::process(const sockaddr_in& senderAddr)
{
    auto* client = ClientManager::getClientByAddress(senderAddr);
    if (!client) return;

    Lobby* lobby = LobbyManager::getLobby(client->lobbyId);
    if (!lobby) return;

    Entity entity = entityId;
    auto& components = ComponentManager::Instance();

    if (components.playerState.find(entity) != components.playerState.end())
    {
        auto& state = components.playerState[entity];

		state.isAiming = isAiming();
		state.isArmed = isArmed();
		state.isRunning = isRunning();
    }
}


