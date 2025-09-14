#include "JoinLobbyMessage.h"

JoinLobbyMessage::JoinLobbyMessage() : lobbyId(0) {}

byte JoinLobbyMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& JoinLobbyMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    serializer.writeInt(lobbyId);

    return serializer.buffer;
}

void JoinLobbyMessage::deserialize(Deserializer& deserializer)
{
    lobbyId = deserializer.readInt();
}

void JoinLobbyMessage::process(const sockaddr_in& senderAddr)
{
    LobbyManager::addClientToLobby(lobbyId, ClientManager::getClientByAddress(senderAddr));

    Serializer serializer;
    serialize(serializer);

    Server::Send(senderAddr, serializer.buffer);
}
