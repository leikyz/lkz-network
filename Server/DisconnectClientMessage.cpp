#include "DisconnectClientMessage.h"

DisconnectClientMessage::DisconnectClientMessage() {}

byte DisconnectClientMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& DisconnectClientMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    return serializer.buffer;
}

void DisconnectClientMessage::deserialize(Deserializer& deserializer)
{

}

void DisconnectClientMessage::process(const sockaddr_in& senderAddr)
{
    Client* currentClient = ClientManager::getClientByAddress(senderAddr);

    if (currentClient->lobbyId != -1)
    {
        Lobby* lobby = LobbyManager::getLobby(currentClient->lobbyId);
        if (lobby)
        {
            lobby->clients.remove(currentClient);
            Serializer serializer;
            serialize(serializer);
            Server::SendToAllInLobby(lobby, serializer.buffer);
            if (lobby->clients.empty())
            {
                std::cout << "Lobby " << lobby->id << " is now empty and will be deleted." << std::endl;
                delete lobby;
				LobbyManager::removeLobby(lobby->id);
            }
        }
	}

    ClientManager::removeClient(senderAddr);
}
