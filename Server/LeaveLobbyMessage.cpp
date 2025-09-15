#include "LeaveLobbyMessage.h"

LeaveLobbyMessage::LeaveLobbyMessage() {}

byte LeaveLobbyMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& LeaveLobbyMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    return serializer.buffer;
}


void LeaveLobbyMessage::deserialize(Deserializer& deserializer)
{
}

void LeaveLobbyMessage::process(const sockaddr_in& senderAddr)
{
    Client* currentClient = ClientManager::getClientByAddress(senderAddr);
    Lobby* lobby = LobbyManager::getLobby(currentClient->lobbyId);

    if (lobby)
    {
        lobby->clients.remove(currentClient);
        currentClient->lobbyId = -1;
        currentClient->positionInLobby = 0;
        currentClient->matchmakingMapIdRequest = 0;
        Serializer serializer;
        serialize(serializer);

        if (lobby->clients.empty())
        {
            LobbyManager::removeLobby(lobby->id);
        }

        Server::SendToAllInLobby(lobby, serializer.buffer);
    }
}
