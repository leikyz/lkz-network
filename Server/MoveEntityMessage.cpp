#include "MoveEntityMessage.h"

MoveEntityMessage::MoveEntityMessage() {};


byte MoveEntityMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& MoveEntityMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    serializer.writeInt(entityId);
    serializer.writeFloat(inputX);
    serializer.writeFloat(inputY);

    return serializer.buffer;
}

void MoveEntityMessage::deserialize(Deserializer& deserializer)
{
    entityId = deserializer.readInt();
    inputX = deserializer.readFloat();
    inputY = deserializer.readFloat();
}


void MoveEntityMessage::process(const sockaddr_in& senderAddr)
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

