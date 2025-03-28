#include "MoveEntityMessage.h"

MoveEntityMessage::MoveEntityMessage() {};


int MoveEntityMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& MoveEntityMessage::serialize(Serializer& serializer) const
{
    serializer.writeInt(ID);
    serializer.writeInt(entityId);
    serializer.writeFloat(inputX);
    serializer.writeFloat(inputY);
    serializer.writeBool(isRunning);

    return serializer.buffer;
}

void MoveEntityMessage::deserialize(Deserializer& deserializer)
{
    entityId = deserializer.readInt();
    inputX = deserializer.readFloat();
    inputY = deserializer.readFloat();
    isRunning = deserializer.readBool();
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

