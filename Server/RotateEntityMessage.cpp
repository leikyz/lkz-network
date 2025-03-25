#include "RotateEntityMessage.h"

RotateEntityMessage::RotateEntityMessage() {};


int RotateEntityMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& RotateEntityMessage::serialize(Serializer& serializer) const
{
    serializer.writeInt(ID);
    serializer.writeInt(entityId);
    serializer.writeFloat(rotaY);

    return serializer.buffer;
}

void RotateEntityMessage::deserialize(Deserializer& deserializer)
{
    entityId = deserializer.readInt();
    rotaY = deserializer.readFloat();
}


void RotateEntityMessage::process(const sockaddr_in& senderAddr)
{
    std::shared_ptr<Lobby> lobby = LobbyManager::getLobby(ClientManager::getClientByAddress(senderAddr)->lobbyId);
    std::shared_ptr<Entity> entity = lobby->getEntityById(entityId);

    if (lobby != nullptr && entity != nullptr)
    {
        Serializer serializer;
        serialize(serializer);

        Server::SendToAllInLobbyExcept(lobby, senderAddr, serializer.buffer);
    }
}

