#include "LobbyManager.h"

std::unordered_map<int, std::shared_ptr<Lobby>> LobbyManager::lobbies;

void LobbyManager::createLobby(int lobbyId)
{
    if (lobbies.find(lobbyId) != lobbies.end()) {
        std::cout << "Le lobby avec l'ID " << lobbyId << " existe déjà." << std::endl;
        return;
    }

    auto lobby = std::make_shared<Lobby>(std::to_string(lobbyId));  
    lobbies[lobbyId] = lobby;

    std::cout << "Lobby créé avec l'ID: " << lobbyId << std::endl;
}

void LobbyManager::addClientToLobby(int lobbyId, const std::shared_ptr<Client>& client)
{
    auto lobby = getLobby(lobbyId);
    if (lobby) {
        lobby->addClient(client);
    }
    else {
        std::cout << "Lobby introuvable avec l'ID: " << lobbyId << std::endl;
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
