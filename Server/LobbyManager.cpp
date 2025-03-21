#include "LobbyManager.h"

std::unordered_map<int, std::shared_ptr<Lobby>> LobbyManager::lobbies;
int LobbyManager::nextLobbyId = 0; // Initialisation du compteur

void LobbyManager::createLobby()
{
    int lobbyId = nextLobbyId++; // Génère un nouvel ID et l'incrémente

    auto lobby = std::make_shared<Lobby>(lobbyId);
    lobbies[lobbyId] = lobby;

    std::cout << "Lobby created with ID: " << lobbyId << std::endl;
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
