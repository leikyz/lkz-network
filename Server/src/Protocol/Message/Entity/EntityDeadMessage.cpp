#include "LKZ/Protocol/Message/Entity/EntityDeadMessage.h"
#include <cstdlib> 
#include <ctime>   
#include <LKZ/Core/ECS/Manager/EntityManager.h>

EntityDeadMessage::EntityDeadMessage() {};

EntityDeadMessage::EntityDeadMessage(uint16_t entityId)
    : entityId(entityId)
{
}

uint8_t EntityDeadMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& EntityDeadMessage::serialize(Serializer& serializer) const
{
    serializer.reset();

    serializer.writeByte(ID);
    serializer.writeUInt16(entityId);

    return serializer.getBuffer();
}

void EntityDeadMessage::deserialize(Deserializer& deserializer)
{
	entityId = deserializer.readUInt16();
}


void EntityDeadMessage::process(const sockaddr_in& senderAddr)
{
    Client* client = ClientManager::getClientByAddress(senderAddr);
    Lobby* lobby = LobbyManager::getLobby(client->lobbyId);

    if (!client || !lobby)
        return;

	EntityManager::Instance().DestroyEntity(entityId);

    Serializer serializer;
    serialize(serializer);

    Engine::Instance().Server()->SendToMultiple(
        lobby->clients,
        serializer.getBuffer(),
        getClassName()
    ); 

}