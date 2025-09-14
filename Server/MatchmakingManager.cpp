#include "MatchmakingManager.h"
#include "Server.h"
#include "Serializer.h"

std::vector<sockaddr_in> MatchmakingManager::waitingPlayers;

void MatchmakingManager::AddPlayerToQueue(const sockaddr_in& playerAddr)
{
	// avoid double entries
    for (const auto& p : waitingPlayers)
    {
        if (p.sin_addr.s_addr == playerAddr.sin_addr.s_addr &&
            p.sin_port == playerAddr.sin_port)
        {
            return; 
        }
    }

    waitingPlayers.push_back(playerAddr);
}

void MatchmakingManager::Update()
{
	std::cout << "[MATCHMAKING] Number of players waiting : " << waitingPlayers.size() << std::endl;
    for (const auto& p : waitingPlayers)
    {
		std::shared_ptr<Lobby> getFirstAvailableLobby = LobbyManager::getAvailableLobby();

        if (getFirstAvailableLobby != nullptr)
        {
            getFirstAvailableLobby->addClient(ClientManager::getClientByAddress(p));       
			std::cout << "Ajout du joueur au lobby existant avec l'ID : " << getFirstAvailableLobby->id << std::endl;
            continue; 
		}
        else
        {
			LobbyManager::createLobby();
            int lastLobbyId = LobbyManager::getLastLobbyId();
			LobbyManager::getLobby(lastLobbyId)->addClient(ClientManager::getClientByAddress(p));
			std::cout << "Création d'un nouveau lobby avec l'ID : " << lastLobbyId << std::endl;
        }

        waitingPlayers.erase(
            std::remove_if(waitingPlayers.begin(), waitingPlayers.end(),
                [&](const sockaddr_in& addr) {
                    return addr.sin_addr.s_addr == p.sin_addr.s_addr
                        && addr.sin_port == p.sin_port;
                }),
            waitingPlayers.end()
        );
    }
}

void MatchmakingManager::StartMatch(const std::vector<sockaddr_in>& players)
{
   
}
