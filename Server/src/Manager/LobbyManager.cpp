#include "LKZ/Manager/LobbyManager.h"

std::unordered_map<int, Lobby*> LobbyManager::lobbies;
std::atomic<int> LobbyManager::nextLobbyId{ 1 };
std::mutex LobbyManager::lobbiesMutex;

/**
 * @brief Creates a new lobby with a unique ID and specified map ID.
 * @param mapId The ID of the map for the new lobby.
 */

void LobbyManager::createLobby(byte mapId)
{
    int lobbyId = nextLobbyId.fetch_add(1);
    auto lobby = new Lobby(lobbyId, mapId);

    std::lock_guard<std::mutex> lock(lobbiesMutex);
    lobbies[lobbyId] = lobby;
}

/**
 * @brief Adds a client to a specified lobby.
 * @param lobbyId The ID of the lobby to which the client will be added.
 * @param client Pointer to the client to be added.
 */

void LobbyManager::addClientToLobby(int lobbyId, Client* client)
{
    std::lock_guard<std::mutex> lock(lobbiesMutex);
    auto it = lobbies.find(lobbyId);
    if (it != lobbies.end())
    {
        it->second->clients.push_back(client);
    }
}

/**
 * @brief Removes a lobby by its ID and cleans up associated resources.
 * @param lobbyId The ID of the lobby to be removed.
 */

void LobbyManager::removeLobby(int lobbyId)
{
    std::lock_guard<std::mutex> lock(lobbiesMutex);
    auto it = lobbies.find(lobbyId);
    if (it != lobbies.end())
    {
        delete it->second;
        lobbies.erase(it);
    }
}


/**
 * @brief Checks if all clients in a specified lobby are marked as ready.
 * @param lobbyId The ID of the lobby to check.
 * @return True if all clients are ready, false otherwise.
 */

bool LobbyManager::IsEveryoneReadyInLobby(int lobbyId)
{
    std::lock_guard<std::mutex> lock(lobbiesMutex);
    Lobby* lobby = getLobby(lobbyId);
    if (!lobby) return false;

    for (Client* c : lobby->clients)
    {
        if (!c || !c->isReady)
            return false;
    }
    return true;
}

Lobby* LobbyManager::getLobby(int lobbyId)
{
    std::lock_guard<std::mutex> lock(lobbiesMutex);
    auto it = lobbies.find(lobbyId);
    return (it != lobbies.end()) ? it->second : nullptr;
}

int LobbyManager::getLobbyCount()
{
    std::lock_guard<std::mutex> lock(lobbiesMutex);
    return static_cast<int>(lobbies.size());
}

Lobby* LobbyManager::getAvailableLobby(byte mapId)
{
    std::lock_guard<std::mutex> lock(lobbiesMutex);
    for (auto& pair : lobbies)
    {
        if (pair.second && pair.second->mapId == mapId && pair.second->clients.size() < pair.second->MAX_PLAYER)
        {
            return pair.second;
        }
    }
    return nullptr;
}

std::vector<Lobby*> LobbyManager::getAllLobbies()
{
    std::lock_guard<std::mutex> lock(lobbiesMutex);
    std::vector<Lobby*> result;
    for (auto& pair : lobbies)
    {
        result.push_back(pair.second);
    }
    return result;
}

