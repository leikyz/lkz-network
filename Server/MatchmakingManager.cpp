#include "MatchmakingManager.h"
#include "Server.h"
#include "Serializer.h"

std::vector<Client> MatchmakingManager::waitingPlayers;

void MatchmakingManager::AddPlayerToQueue(Client& playerAddr)
{
	// avoid double entries
    for (const auto& p : waitingPlayers)
    {
        if (p.address.sin_addr.s_addr == playerAddr.address.sin_addr.s_addr)
            return; 
    }

    waitingPlayers.push_back(playerAddr);
}

void MatchmakingManager::Update()
{
    std::cout << "\033[38;5;216m";
    std::cout << "[MATCHMAKING] Update players : " << waitingPlayers.size() << std::endl;
    std::cout << "\033[0m";


    for (const auto& p : waitingPlayers)
    {
        Lobby* getFirstAvailableLobby = LobbyManager::getAvailableLobby(p.matchmakingMapIdRequest);

        if (getFirstAvailableLobby != nullptr)
        {
            getFirstAvailableLobby->addClient(ClientManager::getClientByAddress(p.address));
        }
        else
        {
            LobbyManager::createLobby(p.matchmakingMapIdRequest);
            int lastLobbyId = LobbyManager::getLastLobbyId();
            LobbyManager::getLobby(lastLobbyId)->addClient(ClientManager::getClientByAddress(p.address));
        }

        waitingPlayers.erase(
            std::remove_if(waitingPlayers.begin(), waitingPlayers.end(),
                [&](const Client& c) {
                    return c.address.sin_addr.s_addr == p.address.sin_addr.s_addr
                        && c.address.sin_port == p.address.sin_port;
                }),
            waitingPlayers.end()
        );
    }
}


void MatchmakingManager::StartMatch(const std::vector<sockaddr_in>& players)
{
   
}
