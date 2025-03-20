#ifndef LOBBY_MANAGER_H
#define LOBBY_MANAGER_H

#include <unordered_map>
#include <memory>
#include <string>
#include "Lobby.h"  // Inclut le fichier Lobby.h

class LobbyManager
{
public:
    static void createLobby(int lobbyId);
    static void addClientToLobby(int lobbyId, const std::shared_ptr<Client>& client);
    static std::shared_ptr<Lobby> getLobby(int lobbyId);

private:
    static std::unordered_map<int, std::shared_ptr<Lobby>> lobbies;
};

#endif // LOBBY_MANAGER_H
