#include "LKZ/Protocol/Message/Approach/DisconnectClientMessage.h"
#include "LKZ/Protocol/Message/Matchmaking/LeaveLobbyMessage.h"
#include "LKZ/Manager/MatchmakingManager.h"


DisconnectClientMessage::DisconnectClientMessage() {}

uint8_t DisconnectClientMessage::getId() const
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
    MatchmakingManager::RemovePlayerFromQueue(senderAddr);

    Client* currentClient = ClientManager::getClientByAddress(senderAddr);
    if (!currentClient)
    {
		Logger::Log("Client not found.", LogType::Warning);
        return;
    }

    if (currentClient->lobbyId != -1)
    {
        Lobby* lobby = LobbyManager::getLobby(currentClient->lobbyId);
        if (lobby)
        {
            uint8_t removedPosition = currentClient->positionInLobby;
            lobby->removeClient(currentClient);
            currentClient->lobbyId = -1;

            if (lobby->clients.empty())
            {
                LobbyManager::removeLobby(lobby->id);
            }
            else
            {
                uint8_t pos = 1;
                for (Client* c : lobby->clients)
                {
                    if (!c) continue;
                    if (c->positionInLobby > removedPosition)
                    {
                        c->positionInLobby--; 
                    }
                }

                LeaveLobbyMessage leaveLobbyMsg;
                leaveLobbyMsg.positionInLobby = removedPosition;
                Serializer serializer;
                std::vector<uint8_t> buffer = leaveLobbyMsg.serialize(serializer);

                std::vector<Client*> clientsCopy(lobby->clients.begin(), lobby->clients.end());
                for (Client* c : clientsCopy)
                {
                    if (!c) continue;
                    Engine::Instance().Server()->Send(senderAddr, buffer);
                }
            }
        }
    }

    ClientManager::removeClient(senderAddr);
}



