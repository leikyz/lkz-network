#include "MatchmakingManager.h"
#include "Server.h"
#include "Serializer.h"
#include "UpdateLobbyMessage.h"
#include "ChangeReadyStatusMessage.h"

std::vector<Client> MatchmakingManager::waitingPlayers;

void MatchmakingManager::AddPlayerToQueue(Client& playerAddr)
{
	// avoid double entries
    for (const auto& p : waitingPlayers)
    {
        if (p.m_address.sin_addr.s_addr == playerAddr.m_address.sin_addr.s_addr && p.m_address.sin_port == playerAddr.m_address.sin_port)
            return; 
    }

    waitingPlayers.push_back(playerAddr);
}

void MatchmakingManager::Update()
{
    std::cout << "\033[38;5;216m";
    std::cout << "[MATCHMAKING] Update players : " << waitingPlayers.size() << std::endl;
    std::cout << "\033[0m";

    std::vector<Client> playersToRemove;

    for (auto& p : waitingPlayers)
    {
        Client* client = ClientManager::getClientByAddress(p.m_address);
        if (!client)
        {
            playersToRemove.push_back(p);
            continue;
        }

        Lobby* lobby = LobbyManager::getAvailableLobby(p.m_matchmakingMapIdRequest);
        if (!lobby)
        {
            LobbyManager::createLobby(p.m_matchmakingMapIdRequest);
            int lastLobbyId = LobbyManager::getLastLobbyId();
            lobby = LobbyManager::getLobby(lastLobbyId);
        }

        if (!lobby)
        {
            std::cout << "[MATCHMAKING] Failed to get or create lobby for client" << std::endl;
            continue;
        }

        client->m_lobbyId = lobby->m_id;
        lobby->addClient(client);
        client->m_positionInLobby = static_cast<byte>(lobby->m_clients.size() - 1);

        std::vector<byte> allPositions;
        for (Client* c : lobby->m_clients)
        {
            if (!c) continue;
            allPositions.push_back(c->m_positionInLobby);
        }

        for (Client* c : lobby->m_clients)
        {
            if (!c) continue;

            if (c->m_isReady)
            {
                ChangeReadyStatusMessage changeReadyMsg;
                changeReadyMsg.isReady = false;
                changeReadyMsg.positionInLobby = c->m_positionInLobby;
                Serializer s;
                std::vector<uint8_t> buf = changeReadyMsg.serialize(s);
                Server::Send(c->m_address, buf);
				c->m_isReady = false;
            }
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

        playersToRemove.push_back(p);
    }
	// remove client that have been added to a lobby
    for (const auto& p : playersToRemove)
    {
        waitingPlayers.erase(
            std::remove_if(waitingPlayers.begin(), waitingPlayers.end(),
                [&](const Client& c) {
                    return c.m_address.sin_addr.s_addr == p.m_address.sin_addr.s_addr
                        && c.m_address.sin_port == p.m_address.sin_port;
                }),
            waitingPlayers.end()
        );
    }
}



void MatchmakingManager::RemovePlayerFromQueue(const sockaddr_in& addr)
{
    waitingPlayers.erase(
        std::remove_if(waitingPlayers.begin(), waitingPlayers.end(),
            [&](const Client& c) {
                return c.m_address.sin_addr.s_addr == addr.sin_addr.s_addr
                    && c.m_address.sin_port == addr.sin_port;
            }),
        waitingPlayers.end()
    );
}



void MatchmakingManager::StartMatch(const std::vector<sockaddr_in>& players)
{
   
}
