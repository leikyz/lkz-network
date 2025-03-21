#include "CreateLobbyMessage.h"

CreateLobbyMessage::CreateLobbyMessage() : lobbyId(0) {}

int CreateLobbyMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& CreateLobbyMessage::serialize(Serializer& serializer) const
{
    serializer.writeInt(ID);
    serializer.writeInt(lobbyId);

    return serializer.buffer;
}

void CreateLobbyMessage::deserialize(Deserializer& deserializer)
{
    
}

void CreateLobbyMessage::process(const sockaddr_in& senderAddr)
{
    LobbyManager::createLobby();

    int lastLobbyId = LobbyManager::getLastLobbyId();

    lobbyId = lastLobbyId;

    Serializer serializer;
    serialize(serializer);

    Server::Send(senderAddr, serializer.buffer);
}
