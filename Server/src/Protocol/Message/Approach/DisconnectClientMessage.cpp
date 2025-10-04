#include "Protocol/Message/Approach/DisconnectClientMessage.h"
#include "Protocol/Message/Matchmaking/LeaveLobbyMessage.h"
#include "Manager/MatchmakingManager.h"


DisconnectClientMessage::DisconnectClientMessage() {}

byte DisconnectClientMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& DisconnectClientMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    return serializer.getBuffer();
}

void DisconnectClientMessage::deserialize(Deserializer& deserializer)
{

}

void DisconnectClientMessage::process(const sockaddr_in& senderAddr)
{
    // Retirer le client de la file d'attente
    MatchmakingManager::RemovePlayerFromQueue(senderAddr);

    Client* currentClient = ClientManager::getClientByAddress(senderAddr);
    if (!currentClient)
    {
        std::cout << "[DISCONNECT] Client already removed." << std::endl;
        return;
    }

    if (currentClient->lobbyId != -1)
    {
        Lobby* lobby = LobbyManager::getLobby(currentClient->lobbyId);
        if (lobby)
        {
            // Retirer le client du lobby
            byte removedPosition = currentClient->positionInLobby;
            lobby->removeClient(currentClient);
            currentClient->lobbyId = -1;

            if (lobby->clients.empty())
            {
                LobbyManager::removeLobby(lobby->id);
            }
            else
            {
                byte pos = 1;
                for (Client* c : lobby->clients)
                {
                    if (!c) continue;
                    if (c->positionInLobby > removedPosition)
                    {
                        c->positionInLobby--; // décaler vers le bas
                    }
                }

                LeaveLobbyMessage leaveLobbyMsg;
                leaveLobbyMsg.positionInLobby = removedPosition;
                Serializer serializer;
                std::vector<uint8_t> buffer = leaveLobbyMsg.serialize(serializer);

                // Copie sécurisée pour éviter les pointeurs invalides
                std::vector<Client*> clientsCopy(lobby->clients.begin(), lobby->clients.end());
                for (Client* c : clientsCopy)
                {
                    if (!c) continue;
                    Server::Send(c->address, buffer);
                }
            }
        }
    }

    // Supprimer le client du ClientManager
    ClientManager::removeClient(senderAddr);
}



