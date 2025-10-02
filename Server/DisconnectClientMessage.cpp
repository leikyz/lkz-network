#include "DisconnectClientMessage.h"
#include "MatchmakingManager.h"
#include "LeaveLobbyMessage.h"

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

    if (currentClient->m_lobbyId != -1)
    {
        Lobby* lobby = LobbyManager::getLobby(currentClient->m_lobbyId);
        if (lobby)
        {
            // Retirer le client du lobby
            byte removedPosition = currentClient->m_positionInLobby;
            lobby->m_clients.remove(currentClient);
            currentClient->m_lobbyId = -1;

            if (lobby->m_clients.empty())
            {
                LobbyManager::removeLobby(lobby->m_id);
            }
            else
            {
                byte pos = 1;
                for (Client* c : lobby->m_clients)
                {
                    if (!c) continue;
                    if (c->m_positionInLobby > removedPosition)
                    {
                        c->m_positionInLobby--; // décaler vers le bas
                    }
                }

                LeaveLobbyMessage leaveLobbyMsg;
                leaveLobbyMsg.positionInLobby = removedPosition;
                Serializer serializer;
                std::vector<uint8_t> buffer = leaveLobbyMsg.serialize(serializer);

                // Copie sécurisée pour éviter les pointeurs invalides
                std::vector<Client*> clientsCopy(lobby->m_clients.begin(), lobby->m_clients.end());
                for (Client* c : clientsCopy)
                {
                    if (!c) continue;
                    Server::Send(c->m_address, buffer);
                }
            }
        }
    }

    // Supprimer le client du ClientManager
    ClientManager::removeClient(senderAddr);
}



