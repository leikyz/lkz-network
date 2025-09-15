#include "LobbyManager.h"

std::unordered_map<int, std::shared_ptr<Lobby>> LobbyManager::lobbies;
int LobbyManager::nextLobbyId = 0; 

void LobbyManager::createLobby(byte mapId)
{
    int lobbyId = nextLobbyId++;

    auto lobby = std::make_shared<Lobby>(lobbyId);
	lobby->mapId = mapId;
    lobbies[lobbyId] = lobby;
}


void LobbyManager::addClientToLobby(int lobbyId, const std::shared_ptr<Client>& client)
{
    auto lobby = getLobby(lobbyId);
    if (lobby) {
        lobby->addClient(client);
        client->lobbyId = lobbyId;
    }
    else {
        std::cout << "Lobby nto found with ID: " << lobbyId << std::endl;
    }
}
std::shared_ptr<Lobby> LobbyManager::getAvailableLobby(byte mapId)
{
    for (const auto& pair : lobbies)
    {
        const auto& lobby = pair.second;
        if (lobby->clients.size() < Lobby::MAX_PLAYER && mapId == lobby->mapId)
        {
            return lobby;
        }
    }
    return nullptr;
}

std::shared_ptr<Lobby> LobbyManager::getLobby(int lobbyId)
{
    auto it = lobbies.find(lobbyId);
    if (it != lobbies.end()) {
        return it->second;
    }
    return nullptr;
}

 std::vector<std::shared_ptr<Lobby>> LobbyManager::getAllLobbies()
{
    std::vector<std::shared_ptr<Lobby>> lobbyList;
    for (const auto& pair : lobbies)
    {
        lobbyList.push_back(pair.second);
    }
    return lobbyList;
}
