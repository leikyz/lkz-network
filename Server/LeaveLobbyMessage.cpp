#include "LeaveLobbyMessage.h"
#include "UpdateLobbyMessage.h"
#include "ChangeReadyStatusMessage.h"

LeaveLobbyMessage::LeaveLobbyMessage() {}

byte LeaveLobbyMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& LeaveLobbyMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
	serializer.writeByte(positionInLobby);
    return serializer.getBuffer();
}


void LeaveLobbyMessage::deserialize(Deserializer& deserializer)
{
}

void LeaveLobbyMessage::process(const sockaddr_in& senderAddr)
{
    Client* currentClient = ClientManager::getClientByAddress(senderAddr);
    Lobby* lobby = LobbyManager::getLobby(currentClient->m_lobbyId);
    byte removedPosition = currentClient->m_positionInLobby;
    if (lobby)
    {
        for (Client* c : lobby->m_clients)
        {
            if (!c) continue;

            if (c->m_isReady)
            {
                c->m_isReady = false;
                ChangeReadyStatusMessage changeReadyMsg;
                changeReadyMsg.isReady = c->m_isReady;
                changeReadyMsg.positionInLobby = c->m_positionInLobby;
                Serializer s;
                std::vector<uint8_t> buf = changeReadyMsg.serialize(s);
                Server::SendToAllInLobby(lobby, buf);
              
            }
        }

        lobby->m_clients.remove(currentClient);

        if (lobby->m_clients.empty())
        {
            LobbyManager::removeLobby(lobby->m_id);
        }
        else
        {
            std::vector<byte> allPositions;

            for (Client* c : lobby->m_clients)
            {
                if (!c) continue;
                if (c->m_positionInLobby > removedPosition)
                {
                    c->m_positionInLobby--; 
                }
                allPositions.push_back(c->m_positionInLobby);
              
            }        

         

            for (Client* c : lobby->m_clients)
            {
                if (!c) continue;


                UpdateLobbyMessage updateLobbyMsg;
                updateLobbyMsg.updatedLobbyPos = c->m_positionInLobby;
                updateLobbyMsg.playersCount = static_cast<byte>(allPositions.size());
                updateLobbyMsg.playersInLobby = allPositions;

                Serializer s;
                std::vector<uint8_t> buf = updateLobbyMsg.serialize(s);
                Server::Send(c->m_address, buf);
            }
        }

        currentClient->m_lobbyId = -1;
        currentClient->m_positionInLobby = 0;
        currentClient->m_matchmakingMapIdRequest = 0;   
    }
}
