#include "LobbyManager.h"

std::unordered_map<int, Lobby*> LobbyManager::lobbies;
int LobbyManager::nextLobbyId = 0; 

void LobbyManager::createLobby(byte mapId)
{
    int lobbyId = nextLobbyId++;

    Lobby* lobby = new Lobby(lobbyId);
	lobby->mapId = mapId;
    lobbies[lobbyId] = lobby;
}


void LobbyManager::addClientToLobby(int lobbyId, Client* client)
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
Lobby* LobbyManager::getAvailableLobby(byte mapId)
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
void LobbyManager::removeLobby(int lobbyId)
{
    auto it = lobbies.find(lobbyId);
    if (it != lobbies.end())
    {
        Lobby* lobby = it->second;

        delete lobby;
        lobbies.erase(it);

        std::cout << "Lobby supprimé avec l'ID: " << lobbyId << std::endl;
    }
    else
    {
        std::cout << "Aucun lobby trouvé avec l'ID: " << lobbyId << std::endl;
    }
}

Lobby* LobbyManager::getLobby(int lobbyId)
{
    auto it = lobbies.find(lobbyId);
    if (it != lobbies.end()) {
        return it->second;
    }
    return nullptr;
}

 std::vector<Lobby*> LobbyManager::getAllLobbies()
{
    std::vector<Lobby*> lobbyList;
    for (const auto& pair : lobbies)
    {
        lobbyList.push_back(pair.second);
    }
    return lobbyList;
}
